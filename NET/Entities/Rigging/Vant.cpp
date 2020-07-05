#include "Vant.h"
#include "rigging_define.h"
#include <stdio.h>

NetVant::NetVant()
{
    bUse=false;
    RenderService=0;
    TextureName=0;
    texl=-1;
    bRunFirstTime=true;
    bYesDeleted=false;
    wVantLast=0;
    gdata=0; groupQuantity=0;
    vlist=0; vantQuantity=0;
    vBuf = iBuf = -1;
    nVert = nIndx = 0;
}

NetVant::~NetVant()
{
    TEXTURE_RELEASE(RenderService,texl);
    PTR_DELETE(TextureName);
    while(groupQuantity>0)
	{
		groupQuantity--;
        PTR_DELETE(gdata[groupQuantity].vantIdx);
	}
    PTR_DELETE(gdata);
    while(vantQuantity>0)
	{
		vantQuantity--;
        PTR_DELETE(vlist[vantQuantity]);
	}
    PTR_DELETE(vlist);
	VERTEX_BUFFER_RELEASE(RenderService,vBuf);
	INDEX_BUFFER_RELEASE(RenderService,iBuf);
	nVert = nIndx = 0;
}

bool NetVant::Init()
{
	GUARD(NetVant::VANT())
	SetDevice();
	UNGUARD
	return true;
}
 
void NetVant::SetDevice()
{
    // получить сервис рендера
	RenderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!RenderService)
	{
		_THROW("No service: dx8render");
	}

    LoadIni();

    if(texl==-1) texl=RenderService->TextureCreate(TextureName);
}

bool NetVant::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool NetVant::LoadState(ENTITY_STATE * state)
{
	return true;
}

void NetVant::Execute(dword Delta_Time)
{
    if(bRunFirstTime)  FirstRun();
    if(bYesDeleted)  DoDelete();

    if(bUse)
    {
        //====================================================
        // Если был изменен ини-файл, то считать инфо из него
        WIN32_FIND_DATA	wfd;
	    HANDLE h = _CORE_API->fio->_FindFirstFile("resource\\ini\\rigging.ini",&wfd);
        if (INVALID_HANDLE_VALUE != h) 
	    {
		    FILETIME ft_new = wfd.ftLastWriteTime;
    	    _CORE_API->fio->_FindClose(h);

	        if (CompareFileTime(&ft_old,&ft_new)!=0) 
            {
                LoadIni();
            }
	    }

		doMove();
    }
}

void NetVant::Realize(dword Delta_Time)
{
    if(bUse)
    {
        DWORD rtm;

        _asm rdtsc _asm mov rtm,eax

        RenderService->TextureSet(0,texl);
		DWORD ambient;
		RenderService->GetRenderState(D3DRS_AMBIENT,&ambient);
		RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,ambient);
		bool bDraw = RenderService->TechniqueExecuteStart("ShipVant");
		if (!bDraw) return;

        // draw nature vants
		CVECTOR cp,ca;
		float pr;
		RenderService->GetCamera(cp,ca,pr);
		pr = tanf(pr*.5f);
        for(int gn=0; gn<groupQuantity; gn++)
			if( gdata[gn].nIndx && nVert && (~(gdata[gn].pMatWorld->Pos() - cp))*pr<fVantMaxDist )
            {
				((SHIP_BASE*)gdata[gn].shipEI.pointer)->SetLightAndFog(true);
				((SHIP_BASE*)gdata[gn].shipEI.pointer)->SetLights();
				
                RenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)gdata[gn].pMatWorld);
			    RenderService->DrawBuffer(vBuf, sizeof(VANTVERTEX), iBuf, 0, nVert, gdata[gn].sIndx, gdata[gn].nIndx);

				((SHIP_BASE*)gdata[gn].shipEI.pointer)->UnSetLights();
				((SHIP_BASE*)gdata[gn].shipEI.pointer)->RestoreLightAndFog();
				_asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
			}
		while (RenderService->TechniqueExecuteNext()) {};
    	//RenderService->Print(0,200,"Vants vert=%d, tr=%d, time=%d",nVert,nIndx,rtm);
    }
}

dword _cdecl NetVant::ProcessMessage(MESSAGE & message)
{
	long code = message.Long();

	switch (code)
    {
    case MSG_VANT_INIT:
		{
			int oldvantQuantity = vantQuantity;
			if(gdata==0)
			{
				if( (gdata=NEW GROUPDATA[1]) == 0 )
					_THROW("Not memory allocation");
				groupQuantity = 1;
			}
			else
			{
				GROUPDATA *oldgdata=gdata;
				if((gdata=NEW GROUPDATA[groupQuantity+1]) == 0)
					_THROW("Not memory allocation");
				memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
				delete oldgdata; groupQuantity++;
			}
			ZERO(gdata[groupQuantity-1]);
			gdata[groupQuantity-1].shipEI = message.EntityID();
			gdata[groupQuantity-1].model_id = message.EntityID();
			MODEL* mdl;
			mdl=(MODEL*)_CORE_API->GetEntityPointer(&gdata[groupQuantity-1].model_id);
			if(mdl==0)
				_THROW("Bad Vant INIT");

			gdata[groupQuantity-1].pMatWorld=&mdl->mtx;
			NODE* nod;
			GEOS::INFO gi;
			GEOS::LABEL gl;
			int i,j;
			for(j=0;j<1000;j++)
			{
				nod=mdl->GetNode(j);
				if(!nod) break;

				nod->geo->GetInfo(gi);
				for(i=0; i<gi.nlabels; i++)
				{
					nod->geo->GetLabel(i,gl);
					if(!strncmp(gl.name,"vant",4))
						AddLabel(gl,nod);
				}
			}

			if(vantQuantity==oldvantQuantity) // не было вантов - удаляем всю группу
			{
				if(groupQuantity==1)
				{
					delete gdata; gdata=0;
					groupQuantity=0;
				}
				else
				{
					groupQuantity--;
					GROUPDATA *oldgdata=gdata;
					gdata=NEW GROUPDATA[groupQuantity];
					if(gdata==0) gdata=oldgdata;
					else
					{
						memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
						delete oldgdata;
					}
				}
				return 0;
			}

			bRunFirstTime=true;
			SetAdd(oldvantQuantity);

			nIndx *= 3;
//			if(gdata[groupQuantity-1].vantIdx) delete gdata[groupQuantity-1].vantIdx;
			gdata[groupQuantity-1].sIndx=nIndx; gdata[groupQuantity-1].nIndx=0;
			gdata[groupQuantity-1].sVert=nVert; gdata[groupQuantity-1].nVert=0;

			gdata[groupQuantity-1].vantQuantity = vantQuantity-oldvantQuantity;
			gdata[groupQuantity-1].vantIdx= NEW int[vantQuantity-oldvantQuantity];
			if(gdata[groupQuantity-1].vantIdx==NULL)	{_THROW("allocate memory error");}

			int idx=0;
			for(int vn=oldvantQuantity; vn<vantQuantity; vn++)
			{
				gdata[groupQuantity-1].vantIdx[idx++] = vn;
				vlist[vn]->sv = nVert;
				vlist[vn]->st = nIndx;
				gdata[groupQuantity-1].nIndx += vlist[vn]->nt;
				gdata[groupQuantity-1].nVert += vlist[vn]->nv;
				nVert += vlist[vn]->nv;
				nIndx += vlist[vn]->nt*3;
			}

			nIndx /= 3;
		}
    break;

    case MSG_VANT_DEL_GROUP:
		{
			ENTITY_ID tmp_id=message.EntityID();
			for(int i=0; i<groupQuantity; i++)
				if(gdata[i].model_id==tmp_id)
				{
					gdata[i].bDeleted = true;
					bYesDeleted = true;
					break;
				}
		}
    break;

	case MSG_VANT_DEL_MAST:
		{
			ENTITY_ID tmp_id = message.EntityID();
			NODE * mastNode = (NODE*)message.Pointer();
			if(mastNode==NULL) break;
			for(int i=0; i<groupQuantity; i++)
				if(gdata[i].model_id==tmp_id)
				{
					for(int j=0; j<gdata[i].vantQuantity; j++)
						if( &mastNode->glob_mtx == vlist[gdata[i].vantIdx[j]]->pUpMatWorld )
						{
							vlist[gdata[i].vantIdx[j]]->bDeleted = true;
							bYesDeleted = true;
						}
					break;
				}
		}
	break;
    }

    return 0;
}

void  NetVant::SetIndex()
{
    int i,j;
    int ti,vi;

    WORD *pt=(WORD*)RenderService->LockIndexBuffer(iBuf);
    if(pt)
    {
        for(int vn=0; vn<vantQuantity; vn++)
        {
            ti=vlist[vn]->st;
            vi=vlist[vn]->sv;

            // set center treangle & square
            pt[ti++]=vi; pt[ti++]=vi+2; pt[ti++]=vi+1;
            pt[ti++]=vi+3; pt[ti++]=vi+4; pt[ti++]=vi+5;
            pt[ti++]=vi+4; pt[ti++]=vi+6; pt[ti++]=vi+5;
            vi+=7;

            // set balk treangles
            pt[ti++]=vi; pt[ti++]=vi+1; pt[ti++]=vi+2;
            pt[ti++]=vi+3; pt[ti++]=vi+4; pt[ti++]=vi+5;
            pt[ti++]=vi; pt[ti++]=vi+3; pt[ti++]=vi+4;
            pt[ti++]=vi; pt[ti++]=vi+4; pt[ti++]=vi+1;
            pt[ti++]=vi; pt[ti++]=vi+2; pt[ti++]=vi+3;
            pt[ti++]=vi+3; pt[ti++]=vi+2; pt[ti++]=vi+5;
            pt[ti++]=vi+2; pt[ti++]=vi+4; pt[ti++]=vi+5;
            pt[ti++]=vi+1; pt[ti++]=vi+4; pt[ti++]=vi+2;
            vi+=6;

            int dIdx=vi+(VANT_EDGE+1)*ROPE_QUANT;
            // set ropes treangles
            for(i=0; i<ROPE_QUANT; i++)
            {
                for(j=0; j<VANT_EDGE; j++)
                {
                    pt[ti]=pt[ti+3]=vi+j; pt[ti+2]=dIdx+j;
//                    if(j<(VANT_EDGE-1))
//                    {
                        pt[ti+4]=vi+j+1;
                        pt[ti+1]=pt[ti+5]=dIdx+j+1;
//                    }
/*                    else
                    {
                        pt[ti+4]=vi;
                        pt[ti+1]=pt[ti+5]=dIdx;
                    }*/
                    ti+=6;
                }
                dIdx+=VANT_EDGE+1;
                vi+=VANT_EDGE+1;
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void NetVant::SetVertexes()
{
    int j,i;
    DWORD iv;
    CVECTOR uPos, lPos, rPos;

    VANTVERTEX* pv=(VANTVERTEX*)RenderService->LockVertexBuffer(vBuf);
    if(pv)
    {
        for(int vn=0; vn<vantQuantity; vn++)
        {
			if(gdata[vlist[vn]->HostGroup].bDeleted) continue;
            iv=vlist[vn]->sv;

			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pUpMatWorld)*vlist[vn]->pUp,uPos);
			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pDownMatWorld)*vlist[vn]->pLeft,lPos);
			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pDownMatWorld)*vlist[vn]->pRight,rPos);

            // Set last parameters
            vlist[vn]->pLeftStart= vlist[vn]->pLeftOld= lPos;
            vlist[vn]->pUpStart= vlist[vn]->pUpOld= uPos;

            CVECTOR horzDirect=!(rPos-lPos);
            CVECTOR vertDirect=!((rPos+lPos)*.5f - uPos);
            // Set angles point
            pv[iv].pos = uPos;
            pv[iv+3].pos=pv[iv+1].pos = uPos - horzDirect*(upWidth*.5f) + vertDirect*upHeight*(1.f-fBalkHeight);
            pv[iv+4].pos=pv[iv+2].pos = uPos + horzDirect*(upWidth*.5f) + vertDirect*upHeight*(1.f-fBalkHeight);
            pv[iv+5].pos = lPos;
            pv[iv+6].pos = rPos;
            //
            pv[iv].tu=(treangXl+treangXr)*.5f; pv[iv].tv=treangYu;
            pv[iv+1].tu=treangXl; pv[iv+1].tv=treangYd;
            pv[iv+2].tu=treangXr; pv[iv+2].tv=treangYd;
            //
            float fh=sqrtf(~((rPos+lPos)*.5f-uPos));
            float ftmp=(float)(int)(fh/hRopeHeight + .5f);
            pv[iv+3].tu=ropeXl; pv[iv+3].tv=0.f;
            pv[iv+4].tu=ropeXr; pv[iv+4].tv=0.f;
            pv[iv+5].tu=ropeXl; pv[iv+5].tv=ftmp;
            pv[iv+6].tu=ropeXr; pv[iv+6].tv=ftmp;
            iv+=7;

            // установить точки балки
            CVECTOR tvec= uPos - horzDirect*(upWidth*.5f) + vertDirect*upHeight;
            pv[iv].pos = tvec - vertDirect*upHeight*fBalkHeight;
            pv[iv+1].pos = tvec + vlist[vn]->pos[0]*fBalkWidth;
            pv[iv+2].pos = tvec + vlist[vn]->pos[VANT_EDGE/2]*fBalkWidth;
            tvec += horzDirect*upWidth;
            pv[iv+3].pos = tvec - vertDirect*upHeight*fBalkHeight;
            pv[iv+4].pos = tvec + vlist[vn]->pos[0]*fBalkWidth;
            pv[iv+5].pos = tvec + vlist[vn]->pos[VANT_EDGE/2]*fBalkWidth;
            //
            pv[iv].tu= pv[iv+1].tu= pv[iv+2].tu = treangXl;
            pv[iv+3].tu= pv[iv+4].tu= pv[iv+5].tu = treangXr;
            pv[iv].tv=pv[iv+3].tv = balkYu;
            pv[iv+1].tv=pv[iv+2].tv=pv[iv+4].tv=pv[iv+5].tv = balkYd;
            iv+=6;

            // Set up ropes points
            CVECTOR sp = uPos-horzDirect*(.5f*upWidth) + vertDirect*upHeight;
            CVECTOR dp = horzDirect*(upWidth/(float)(ROPE_QUANT-1));
            float dtmp=(vRopeXr-vRopeXl)/(float)VANT_EDGE;
            for(i=0; i<ROPE_QUANT; i++)
            {
                for(j=0; j<=VANT_EDGE; j++)
                {
                    if(j==VANT_EDGE)
                        pv[iv+j].pos = sp + vlist[vn]->pos[0];
                    else
                        pv[iv+j].pos = sp + vlist[vn]->pos[j];
                    pv[iv+j].tu = vRopeXl+ dtmp*(float)j;
                    pv[iv+j].tv = 0.f;
                }
                iv+=VANT_EDGE+1;
                sp+=dp;
            }

            // Set down ropes points
            sp = lPos;
            dp = (rPos-lPos)/(float)(ROPE_QUANT-1);
            ftmp=fh/vRopeHeight;
            for(i=0; i<ROPE_QUANT; i++)
            {
                for( j=0; j<=VANT_EDGE; j++ )
                {
                    if(j==VANT_EDGE)
                        pv[iv+j].pos = sp + vlist[vn]->pos[0];
                    else
                        pv[iv+j].pos = sp + vlist[vn]->pos[j];
                    pv[iv+j].tu = vRopeXl+ dtmp*(float)j;
                    pv[iv+j].tv= ftmp;
                }
                iv+=VANT_EDGE+1;
                sp+=dp;
            }
        }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

void NetVant::AddLabel(GEOS::LABEL &lbl,NODE *nod)
{
    VANTDATA *vd;
    int  vantNum;

    if(nod==0) return;

    vantNum = atoi( &lbl.name[4] );

    int vn;
    for(vn=0; vn<vantQuantity; vn++)
        if( (vlist[vn]->HostGroup==groupQuantity-1) &&
            (vlist[vn]->vantNum==vantNum) )
        {
            vd=vlist[vn]; break;
        }
    if(vn==vantQuantity)
    {
        //создаем новый вант
        vd= NEW VANTDATA;
        if(vd==0)
            _THROW("Not memory allocate");
        PZERO(vd,sizeof(VANTDATA));
        vd->bDeleted=false;
        vd->vantNum=vantNum;
        vd->pUpMatWorld=vd->pDownMatWorld=0;
        vd->HostGroup=groupQuantity-1;

        if(vantQuantity==0)
        {
            vlist=NEW VANTDATA*[1];
            vantQuantity=1;
        }
        else
        {
            VANTDATA **oldvlist=vlist;
            vlist = NEW VANTDATA*[vantQuantity+1];
            if(vlist==0)
                _THROW("Not memory allocate");
            memcpy(vlist,oldvlist,sizeof(VANTDATA*)*vantQuantity);
            delete oldvlist; vantQuantity++;
        }

        vlist[vantQuantity-1]=vd;
    }

    switch( lbl.name[5] )
    {
    case 'u': // up edge of vant
		vd->pUp = CVECTOR(lbl.m[3][0],lbl.m[3][1],lbl.m[3][2]) - gdata[groupQuantity-1].pMatWorld->Pos();// + nod->glob_mtx.Pos();
        vd->pUpMatWorld=&nod->glob_mtx; // get host matrix
        break;
    case 'l': // left edge of vant
		vd->pLeft = CVECTOR(lbl.m[3][0],lbl.m[3][1],lbl.m[3][2]) - gdata[groupQuantity-1].pMatWorld->Pos();// + nod->glob_mtx.Pos();
		if(vd->pDownMatWorld==0)
			vd->pDownMatWorld=&nod->glob_mtx; // get host matrix
		else if(vd->pDownMatWorld!=&nod->glob_mtx)
			vd->pDownMatWorld->MulToInv(nod->glob_mtx*vd->pLeft,vd->pLeft);
        break;
    case 'r': // right edge of vant
		vd->pRight = CVECTOR(lbl.m[3][0],lbl.m[3][1],lbl.m[3][2]) - gdata[groupQuantity-1].pMatWorld->Pos();// + nod->glob_mtx.Pos();
		if(vd->pDownMatWorld==0)
			vd->pDownMatWorld=&nod->glob_mtx; // get host matrix
		else if(vd->pDownMatWorld!=&nod->glob_mtx)
			vd->pDownMatWorld->MulToInv(nod->glob_mtx*vd->pLeft,vd->pLeft);
        break;
    }
}

void NetVant::SetAll()
{
    // set vertex and index buffers
    for(int vn=0; vn<vantQuantity; vn++)
    {
        while(1)
        {
			if( !gdata[vlist[vn]->HostGroup].bDeleted )
				if( vlist[vn]->pUpMatWorld && vlist[vn]->pDownMatWorld ) break;

            delete vlist[vn];
            vantQuantity--;
            if(vantQuantity>0)
            {
                VANTDATA **oldvlist=vlist;
                vlist = NEW VANTDATA*[vantQuantity];
                if(vlist)
                {
                    if(vn>0)
                        memcpy(vlist,oldvlist,sizeof(VANTDATA*)*vn);
                    if(vn<vantQuantity)
                        memcpy(&vlist[vn],&oldvlist[vn+1],sizeof(VANTDATA*)*(vantQuantity-vn));
                    delete oldvlist;
                }
                else
                    vlist=oldvlist;
            }
            else
            {
                delete vlist; vlist=0;
            }
            if(vn==vantQuantity) break;
        }
        if(vn==vantQuantity) break;

        // Set normals with length equal the rope width
		CMatrix tmat;
		tmat.BuildViewMatrix(.5f*(vlist[vn]->pLeft+vlist[vn]->pRight),vlist[vn]->pUp,CVECTOR(0.f,0.f,1.f));
        float ca,sa;
        for(int i=0; i<VANT_EDGE; i++)
        {
            ca=cosf((float)i/(float)VANT_EDGE*2.f*PI);
            sa=sinf((float)i/(float)VANT_EDGE*2.f*PI);
			//tmat.MulToInvNorm(CVECTOR(ROPE_WIDTH/2.f*ca,ROPE_WIDTH/2.f*sa,0.f),vlist[vn]->pos[i]);
			vlist[vn]->pos[i] = CVECTOR(ROPE_WIDTH/2.f*ca,0.f,ROPE_WIDTH/2.f*sa);
        }

        vlist[vn]->nv=(VANT_EDGE+1)*ROPE_QUANT*2 + 7 + 6;
        vlist[vn]->nt=VANT_EDGE*ROPE_QUANT*2 + 3 + 8;
    }
}

void NetVant::LoadIni()
{
    GUARD(NetVant::LoadIni());
	char	section[256];
    char    param[256];

	INIFILE * ini;
	WIN32_FIND_DATA	wfd;
	HANDLE h = _CORE_API->fio->_FindFirstFile("resource\\ini\\rigging.ini",&wfd);
	if (INVALID_HANDLE_VALUE != h) 
	{
		ft_old = wfd.ftLastWriteTime;
		_CORE_API->fio->_FindClose(h);
	}
	ini = _CORE_API->fio->OpenIniFile("resource\\ini\\rigging.ini");
	if(!ini) THROW("rigging.ini file not found!");

	sprintf(section,"VANTS");

    // имя текстуры
    ini->ReadString(section,"TextureName",param,sizeof(param)-1,"vant.tga");
    if(texl!=-1)
    {
        if(strcmp(TextureName,param))
            if(RenderService)
            {
                delete TextureName; TextureName=NEW char[strlen(param)+1];
                strcpy(TextureName,param);
                RenderService->TextureRelease(texl);
                texl=RenderService->TextureCreate(TextureName);
            }
    }
    else
    {
        TextureName=NEW char[strlen(param)+1];
        strcpy(TextureName,param);
    }
    // толщина веревки
    ROPE_WIDTH=ini->GetFloat(section,"fWidth",0.1f);
    // количество веревок
    ROPE_QUANT=(int)ini->GetLong(section,"fRopeQuant",5);
    if(ROPE_QUANT<2) ROPE_QUANT=2;
    // координаты текстуры горизонтальной веревки xBeg
    ropeXl=ini->GetFloat(section,"fHRopeXbeg",0.5f);
    ropeXr=ini->GetFloat(section,"fHRopeXend",1.f);
    // координаты текстуры треугольника
    treangXl=ini->GetFloat(section,"fTreangXbeg",0.f);
    treangXr=ini->GetFloat(section,"fTreangXend",0.5f);
    treangYu=ini->GetFloat(section,"fTreangYbeg",0.f);
    treangYd=ini->GetFloat(section,"fTreangYend",1.f);
    // координаты текстуры балки
    balkYu=ini->GetFloat(section,"fBalkYbeg",0.6f);
    balkYd=ini->GetFloat(section,"fBalkYend",1.f);
    // координаты текстуры вертикальной веревки
    vRopeXl=ini->GetFloat(section,"fVRopeXbeg",0.f);
    vRopeXr=ini->GetFloat(section,"fVRopeXend",0.1f);
    // ширина верхнего треугольника
    upWidth=ini->GetFloat(section,"fTreangWidth",1.f);
    // высота верхнего треугольника
    upHeight=ini->GetFloat(section,"fTreangHeight",1.f);
    // высота вертикальной веревки
    vRopeHeight=ini->GetFloat(section,"fVRopeHeight",1.f);
    // высота горизонтальной веревки
    hRopeHeight=ini->GetFloat(section,"fHRopeHeight",1.f);
    // высота балки относительно высоты треугольника
    fBalkHeight=ini->GetFloat(section,"fBalkHeight",0.1f);
    fBalkWidth=ini->GetFloat(section,"fBalkWidth",1.2f);
	// квадрат расстояния с которого не видны ванты
    fVantMaxDist=ini->GetFloat(section,"fVantMaxDist",10000.f);
    // шаг дискретизации движения ванта
    ZERO_CMP_VAL=ini->GetFloat(section,"fDiscrValue",0.01f);
    // максимальное изменение положения ванта при котором вант перестает отображаться
    MAXFALL_CMP_VAL=ini->GetFloat(section,"fDisapearValue",5.f);

    delete ini;
    UNGUARD
}

void NetVant::doMove()
{
    int j,i;
    DWORD iv;
    CVECTOR uPos, lPos, rPos;

    VANTVERTEX* pv=(VANTVERTEX*)RenderService->LockVertexBuffer(vBuf);
    if(pv)
    {
        for(int vn=0; vn<vantQuantity; vn++)
        {
			if( gdata[vlist[vn]->HostGroup].bDeleted || vlist[vn]->bDeleted )	{bYesDeleted = true;	continue;}
            CVECTOR vtmp,htmp;
			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pUpMatWorld*vlist[vn]->pUp,uPos);
			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pDownMatWorld*vlist[vn]->pLeft,lPos);
			gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pDownMatWorld*vlist[vn]->pRight,rPos);

            if( !VectCmp(lPos, vlist[vn]->pLeftStart, MAXFALL_CMP_VAL) ||
                !VectCmp(uPos, vlist[vn]->pUpStart, MAXFALL_CMP_VAL) )
            {
                vlist[vn]->bDeleted=true; // установим признак удаления ванта
                bYesDeleted=true;
            }

            if( !VectCmp(lPos, vlist[vn]->pLeftOld, ZERO_CMP_VAL) ||
                !VectCmp(uPos, vlist[vn]->pUpOld, ZERO_CMP_VAL) )
            {
                // Set last parameters
                vlist[vn]->pLeftOld=lPos;
                vlist[vn]->pUpOld=uPos;

                CVECTOR horzDirect=!(rPos-lPos);
                CVECTOR vertDirect=!((rPos+lPos)*.5f - uPos);

                iv=vlist[vn]->sv;

                // Set angles point
                pv[iv].pos = uPos;
                htmp = horzDirect*(upWidth*.5f);
                vtmp = vertDirect*upHeight*(1.f-fBalkHeight);
                pv[iv+3].pos=pv[iv+1].pos = uPos - htmp + vtmp;
                pv[iv+4].pos=pv[iv+2].pos = uPos + htmp + vtmp;
                pv[iv+5].pos = lPos;
                pv[iv+6].pos = rPos;
                iv+=7;

                // установить точки балки
                CVECTOR tvec= uPos - htmp + vertDirect*upHeight;
                pv[iv].pos = uPos - htmp + vtmp;
                pv[iv+1].pos = tvec + vlist[vn]->pos[0]*fBalkWidth;
                pv[iv+2].pos = tvec + vlist[vn]->pos[VANT_EDGE/2]*fBalkWidth;
                tvec += horzDirect*upWidth;
                pv[iv+3].pos = uPos + htmp + vtmp;
                pv[iv+4].pos = tvec + vlist[vn]->pos[0]*fBalkWidth;
                pv[iv+5].pos = tvec + vlist[vn]->pos[VANT_EDGE/2]*fBalkWidth;
                iv+=6;

                // Set up ropes points
                CVECTOR sp = uPos-horzDirect*(.5f*upWidth) + vertDirect*upHeight;
                CVECTOR dp = horzDirect*(upWidth/(float)(ROPE_QUANT-1));
                for(i=0; i<ROPE_QUANT; i++)
                {
                    for(j=0; j<=VANT_EDGE; j++)
                    {
                        if(j==VANT_EDGE)
                            pv[iv+j].pos = sp + vlist[vn]->pos[0];
                        else
                            pv[iv+j].pos = sp + vlist[vn]->pos[j];
                    }
                    iv+=VANT_EDGE+1;
                    sp+=dp;
                }

                // Set down ropes points
                sp = lPos;
                dp = (rPos-lPos)/(float)(ROPE_QUANT-1);
                for(i=0; i<ROPE_QUANT; i++)
                {
                    for( j=0; j<=VANT_EDGE; j++ )
                    {
                        if(j==VANT_EDGE)
                            pv[iv+j].pos = sp + vlist[vn]->pos[0];
                        else
                            pv[iv+j].pos = sp + vlist[vn]->pos[j];
                    }
                    iv+=VANT_EDGE+1;
                    sp+=dp;
                }
            }
        }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

bool NetVant::VectCmp(CVECTOR v1,CVECTOR v2,float minCmpVal) // return true if equal
{
    CVECTOR dv=v1-v2;

    if( dv.x>minCmpVal || dv.x<-minCmpVal ||
        dv.y>minCmpVal || dv.y<-minCmpVal ||
        dv.z>minCmpVal || dv.z<-minCmpVal )  return false;
    return true;
}

void NetVant::FirstRun()
{
    if(nVert>0 && nIndx>0)
    {
		VERTEX_BUFFER_RELEASE(RenderService,vBuf);
		INDEX_BUFFER_RELEASE(RenderService,iBuf);
        vBuf = RenderService->CreateVertexBuffer(VANTVERTEX_FORMAT,nVert*sizeof(VANTVERTEX),D3DUSAGE_WRITEONLY);
        iBuf = RenderService->CreateIndexBuffer(nIndx*6);
        SetVertexes();
        SetIndex();
    }

    bUse = (vBuf!=-1 && iBuf!=-1);

    bRunFirstTime=false;
    wVantLast=vantQuantity;
}

void NetVant::SetAdd(int firstNum)
{
    // set vertex and index buffers
    for(int vn=firstNum; vn<vantQuantity; vn++)
    {
        while( vlist[vn]->pUpMatWorld==0 || vlist[vn]->pDownMatWorld==0 )
        {
            delete vlist[vn];
            vantQuantity--;
            if(vantQuantity>0)
            {
                VANTDATA **oldvlist=vlist;
                vlist = NEW VANTDATA*[vantQuantity];
                if(vlist==0)
                    vlist=oldvlist;
                if(vn>0)
                    memcpy(vlist,oldvlist,sizeof(VANTDATA*)*vn);
                if(vn<vantQuantity)
                    memcpy(&vlist[vn],&oldvlist[vn+1],sizeof(VANTDATA*)*(vantQuantity-vn));
                if(vlist!=oldvlist)
                    delete oldvlist;
            }
            else
            {
                delete vlist; vlist=0;
            }
            if(vn==vantQuantity) break;
        }
        if(vn==vantQuantity) break;

        // Set normals with length equal the rope width
		CMatrix tmat;
		tmat.BuildViewMatrix(.5f*(vlist[vn]->pLeft+vlist[vn]->pRight),vlist[vn]->pUp,CVECTOR(0.f,0.f,1.f));
        float ca,sa;
        for(int i=0; i<VANT_EDGE; i++)
        {
            ca=cosf((float)i/(float)VANT_EDGE*2.f*PI);
            sa=sinf((float)i/(float)VANT_EDGE*2.f*PI);
			tmat.MulToInvNorm(CVECTOR(ROPE_WIDTH/2.f*ca,ROPE_WIDTH/2.f*sa,0.f),vlist[vn]->pos[i]);
        }

        vlist[vn]->nv=(VANT_EDGE+1)*ROPE_QUANT*2 + 7 + 6;
        vlist[vn]->nt=VANT_EDGE*ROPE_QUANT*2 + 3 + 8;
    }
}

void NetVant::DoDelete()
{
    // для всех удаленных групп удалим принадлежащие им ванты
    int ngn=0;
    int nvn=0;
    nVert=0; nIndx=0;
    for(int gn=0; gn<groupQuantity; gn++)
    {
        int gs=0; // размер группы
        gdata[gn].sVert = nVert;
        gdata[gn].sIndx = nIndx;
        gdata[gn].nVert = 0;
        gdata[gn].nIndx = 0;
        for(int idx=0; idx<gdata[gn].vantQuantity; idx++)
        {
            int vn=gdata[gn].vantIdx[idx];
            if(vlist[vn]->bDeleted || gdata[gn].bDeleted)
            {
                delete vlist[vn];
                continue;
            }

            vlist[vn]->HostGroup=ngn;
            vlist[vn]->sv=nVert;
            vlist[vn]->st=nIndx;
            gdata[gn].nVert += vlist[vn]->nv;
            gdata[gn].nIndx += vlist[vn]->nt;
            nVert += vlist[vn]->nv;
            nIndx += vlist[vn]->nt*3;

            gdata[gn].vantIdx[gs++] = nvn;

            // поставим вант на нужное местов списке
            vlist[nvn++]=vlist[vn];
        }

        // если группа пустая, то удалим ее
        if(gs==0)
        {
            if(gdata[gn].vantIdx)
                delete gdata[gn].vantIdx;
            continue;
        }

        gdata[gn].vantQuantity=gs;
        // поставим группу на нужное место в списке
        if(ngn<gn)
            memcpy(&gdata[ngn],&gdata[gn],sizeof(GROUPDATA));
        ngn++;
    }

    nIndx/=3;
    // если нет больше вант, то уберем их все
    if(ngn==0 || nvn==0)
    {
        vantQuantity = groupQuantity = 0;
		VERTEX_BUFFER_RELEASE(RenderService,vBuf);
		INDEX_BUFFER_RELEASE(RenderService,iBuf);
        delete vlist; vlist=0;
        delete gdata; gdata=0;
    }
    else
        if(nvn!=vantQuantity || ngn!=groupQuantity)
        {
            vantQuantity=nvn; groupQuantity=ngn;
			VERTEX_BUFFER_RELEASE(RenderService,vBuf);
			INDEX_BUFFER_RELEASE(RenderService,iBuf);
            vBuf=RenderService->CreateVertexBuffer(VANTVERTEX_FORMAT,nVert*sizeof(VANTVERTEX),D3DUSAGE_WRITEONLY);
            iBuf=RenderService->CreateIndexBuffer(nIndx*6);
            SetVertexes();
            SetIndex();
        }

    bYesDeleted = false;
    wVantLast = vantQuantity;
    bUse = vantQuantity>0;
}
