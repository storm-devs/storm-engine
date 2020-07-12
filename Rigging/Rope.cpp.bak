#include "ROPE.h"
#include "..\common_h\defines.h"
#include "..\common_h\sail_msg.h"
#include "..\common_h\model.h"
#include "..\common_h\ship_base.h"
#include "rigging_define.h"
#include <stdio.h>

extern void _cdecl sailPrint(VDX8RENDER *rs, const CVECTOR & pos3D, float rad, long line, const char * format, ...);

ROPE::ROPE()
{
    bUse=false;
    bYesDeleted=false;
    wRopeLast=0;
    RenderService=0;
    gdata=0; groupQuantity=0;
    rlist=0; ropeQuantity=0;

    TextureName=0; texl=-1;
    bFirstRun=true;

	ZERO(mat);
	mat.Diffuse.r = 1.f;
	mat.Diffuse.g = 1.f;
	mat.Diffuse.b = 1.f;
	mat.Ambient.r = 1.f;
	mat.Ambient.g = 1.f;
	mat.Ambient.b = 1.f;

    vBuf = iBuf = -1;
    nVert = nIndx = 0;
}

ROPE::~ROPE()
{
	// очистка и удаление списка веревок
	if(rlist)
	{
		for(int i=0; i<ropeQuantity; i++)
			PTR_DELETE(rlist[i]);
		PTR_DELETE(rlist);
		ropeQuantity = 0;
	}
	// очистка и удаление списка групп
	if(gdata)
	{
		for(int i=0; i<groupQuantity; i++)
			PTR_DELETE(gdata[i].ropeIdx);
		PTR_DELETE(gdata);
		groupQuantity = 0;
	}
	// удаление текстур
	TEXTURE_RELEASE(RenderService,texl);
	PTR_DELETE(TextureName);

	VERTEX_BUFFER_RELEASE(RenderService,vBuf);
	INDEX_BUFFER_RELEASE(RenderService,iBuf);
	nVert = nIndx = 0;
}

bool ROPE::Init()
{
	GUARD(ROPE::ROPE())
	SetDevice();
	UNGUARD
	return true;
}
 
void ROPE::SetDevice()
{
    // получить сервис рендера
	RenderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!RenderService)
	{
		_THROW("No service: dx8render");
	}

    LoadIni();

    texl=RenderService->TextureCreate(TextureName);
}

bool ROPE::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool ROPE::LoadState(ENTITY_STATE * state)
{
	return true;
}

FILETIME ft_old;
void ROPE::Execute(dword Delta_Time)
{
    DWORD rtm;
    _asm rdtsc _asm mov rtm,eax

    if(bFirstRun)
        FirstRun();
    if(bYesDeleted)
        DoDelete();

    if(bUse)
    {
	    vertBuf=(ROPEVERTEX*)RenderService->LockVertexBuffer(vBuf);
	    if( vertBuf )
	    {
            float dtime = (float)Delta_Time * .02f;
		    for(int i=0; i<ropeQuantity; i++)
		    {
                if(rlist[i]->bUse && !gdata[rlist[i]->HostGroup].bDeleted)
                    SetVertexes(rlist[i],dtime);
			        //DoMove(rlist[i]);
                else
                    if(rlist[i]->len!=0.f) // set all vertex to point(0,0,0)
                    {
                        CVECTOR nulVect=CVECTOR(0.f,0.f,0.f);
                        for(dword idx=rlist[i]->sv; idx<rlist[i]->sv + rlist[i]->nv; idx++)
                            vertBuf[idx].pos=nulVect;
                    }
		    }

		    RenderService->UnLockVertexBuffer(vBuf);
	    }
    }

    _asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
    execute_tm=rtm;
}

void ROPE::Realize(dword Delta_Time)
{
    if(bUse)
    {
        DWORD rtm;

        _asm rdtsc _asm mov rtm,eax
	    if (Delta_Time==0)
    	{
	    	// draw mirror rope
    	}
        else
        {
            // draw nature rope
			CVECTOR cp,ca;
			float pr;
			RenderService->GetCamera(cp,ca,pr);
			pr = tanf(pr*.5f);

			bool bDraw = RenderService->TechniqueExecuteStart("ShipRope");

            if (bDraw)
				for(int i=0; i<groupQuantity; i++)
					if(!gdata[i].bDeleted && gdata[i].nt!=0 && nVert!=0)
						if((~(gdata[i].pMatWorld->Pos()-cp))*pr<fMaxRopeDist) // если расстояние до корабля не больше максимального
						{
							((SHIP_BASE*)gdata[i].shipEI.pointer)->SetLightAndFog(true);
							RenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)gdata[i].pMatWorld);

							RenderService->TextureSet(0,texl);
							RenderService->SetMaterial(mat);
							RenderService->DrawBuffer(vBuf, sizeof(ROPEVERTEX), iBuf, 0, nVert, gdata[i].st, gdata[i].nt);
							((SHIP_BASE*)gdata[i].shipEI.pointer)->RestoreLightAndFog();
						}
			if (bDraw) while (RenderService->TechniqueExecuteNext()) {};

            _asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
            realize_tm=rtm;
            //RenderService->Print(0,10,"exec=%d, real=%d",execute_tm,realize_tm);
			/*if(api->Controls->GetDebugAsyncKeyState('M')<0)
				for(int i=0; i<ropeQuantity; i++)
				{
					sailPrint(RenderService,*(rlist[i]->bMatWorld) * rlist[i]->pBeg,25.f,0,"B%d_%s%d",rlist[i]->ropeNum,rlist[i]->btie?"tie":"",rlist[i]->bgnum);
					sailPrint(RenderService,*(rlist[i]->eMatWorld) * rlist[i]->pEnd,25.f,0,"E%d_%s%d",rlist[i]->ropeNum,rlist[i]->etie?"tie":"",rlist[i]->bgnum);
				}*/
        }
    }
}

dword _cdecl ROPE::ProcessMessage(MESSAGE & message)
{
	long code = message.Long();
    ENTITY_ID tmp_id;

	switch (code)
    {
    case MSG_ROPE_INIT:
		{
			ENTITY_ID tmp_shipEI = message.EntityID();
			ENTITY_ID tmp_modelEI = message.EntityID();
			MODEL* mdl = (MODEL*)_CORE_API->GetEntityPointer(&tmp_modelEI);
			if( mdl==null )
			{
				api->Trace("WARNING!!! Missing INIT message to ROPE - bad ship model");
				return 0;
			}

			int wFirstRope = ropeQuantity;
			if(gdata!=0)
			{
				GROUPDATA* oldgdata=gdata;
				gdata = NEW GROUPDATA[groupQuantity+1];
				if(gdata==NULL)
				{
					_THROW("allocate memory error");
				}
				memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
				delete oldgdata; groupQuantity++;
			}
			else
			{
				gdata = NEW GROUPDATA[1]; groupQuantity=1;
				if(gdata==NULL)
				{
					_THROW("allocate memory error");
				}
			}

			ZERO(gdata[groupQuantity-1]);
			gdata[groupQuantity-1].shipEI = tmp_shipEI;
			gdata[groupQuantity-1].modelEI = tmp_modelEI;
			gdata[groupQuantity-1].pMatWorld=&mdl->mtx;

			NODE* nod;
			GEOS::INFO gi;
			GEOS::LABEL gl;
			int i,j;
			for(j=0;true;j++)
			{
				nod=mdl->GetNode(j);
				if(!nod) break;

				nod->geo->GetInfo(gi);
				for(i=0; i<gi.nlabels; i++)
				{
					nod->geo->GetLabel(i,gl);
					if(!strncmp(gl.name,"rope",4))
						AddLabel(gl,nod, false);
					if(!strncmp(gl.name,"fal",3))
						AddLabel(gl,nod, true);
				}
			}

			// небыло никаких новых веревок
			if(wFirstRope == ropeQuantity)
			{
				groupQuantity--;
				if(groupQuantity<=0) PTR_DELETE(gdata);
				return 0;
			}

			if( !bFirstRun ) {
				nIndx *= 3;
				bFirstRun=true;
			}
			SetAdd(wFirstRope);

			gdata[groupQuantity-1].nt = 0;
			gdata[groupQuantity-1].nv = 0;
			gdata[groupQuantity-1].st = nIndx;
			gdata[groupQuantity-1].sv = nVert;
			gdata[groupQuantity-1].ropeQuantity = 0;
			for(int rn=wFirstRope; rn<ropeQuantity; rn++)
			{
				if(rlist[rn]->HostGroup==groupQuantity-1)
				{
					gdata[groupQuantity-1].ropeQuantity++;
					gdata[groupQuantity-1].nv+=rlist[rn]->nv;
					gdata[groupQuantity-1].nt+=rlist[rn]->nt;
					rlist[rn]->sv=nVert;
					rlist[rn]->st=nIndx;
					nVert+=rlist[rn]->nv;
					nIndx+=rlist[rn]->nt*3;
				}
			}
			gdata[groupQuantity-1].ropeIdx = NEW int[gdata[groupQuantity-1].ropeQuantity];
			if( gdata[groupQuantity-1].ropeIdx==NULL )
			{
				_THROW("allocate memory error");
			}
			int idx=0;
			for(rn=wFirstRope; rn<ropeQuantity; rn++)
				if(rlist[rn]->HostGroup==groupQuantity-1)
					gdata[groupQuantity-1].ropeIdx[idx++]=rn;
		}
        break;

    // удаление веревки из отображения и расчета
    case MSG_ROPE_DELETE:
        {
            tmp_id = message.EntityID();
            int rope_number; rope_number=message.Long();
            for(int i=0; i<ropeQuantity; i++)
            {
                if(rlist[i]->ropeNum==rope_number && gdata[rlist[i]->HostGroup].modelEI==tmp_id)
                {
                    rlist[i]->bDeleted=true;
                    bYesDeleted=true;
                    //rlist[i]->bUse=false;
                    /*ENTITY_ID sailEI;
                    if(_CORE_API->FindClass(&sailEI,"sail",0))
                        if(rlist[i]->btie || rlist[i]->etie) // отвяжем парус от веревки
                            _CORE_API->Send_Message(sailEI,"ll",MSG_SAIL_ROPE_UNTIE,rope_number);*/
                    break;
                }
            }
        }
        break;

    case MSG_ROPE_DEL_GROUP:
        {
            tmp_id = message.EntityID();
            for(int gn=0; gn<groupQuantity; gn++)
                if(gdata[gn].modelEI==tmp_id)
                {
                    gdata[gn].bDeleted=true;
                    bYesDeleted=true;
                    break;
                }
        }
        break;
    }

    return 0;
}

void  ROPE::SetIndex()
{
    int i,j;
    int ti,vi;

    WORD* pt=(WORD*)RenderService->LockIndexBuffer(iBuf);
    if(pt)
    {
        for(int rn=0; rn<ropeQuantity; rn++)
        {
            ti=rlist[rn]->st;
            vi=rlist[rn]->sv;

            // set begin edge point triangles
            for(j=0; j<ROPE_EDGE; j++)
            {
                pt[ti]=vi; pt[ti+1]=vi+1+j;
                if(j<ROPE_EDGE-1)  pt[ti+2]=vi+2+j;
                else pt[ti+2]=vi+1;
                ti+=3;
            }
            vi++;

            // set medium triangles
            for(i=0; i<rlist[rn]->segquant; i++)
            {
                for(j=0; j<ROPE_EDGE; j++)
                {
                    pt[ti]=vi+j; pt[ti+1]=pt[ti+4]=vi+j+ROPE_EDGE;
                    if( j<ROPE_EDGE-1)
                    {
                        pt[ti+2]=pt[ti+3]=vi+j+1;
                        pt[ti+5]=vi+j+1+ROPE_EDGE;
                    }
                    else
                    {
                        pt[ti+2]=pt[ti+3]=vi;
                        pt[ti+5]=vi+ROPE_EDGE;
                    }
                    ti+=6;
                }
                vi+=ROPE_EDGE;
            }

            // set end edge point triangles
            for(j=0; j<ROPE_EDGE; j++)
            {
                pt[ti]=vi+j; pt[ti+1]=vi+1;
                if(j<ROPE_EDGE-1)  pt[ti+2]=vi+j+1;
                else pt[ti+2]=vi;
                ti+=3;
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void ROPE::SetVertexes()
{
    vertBuf = (ROPEVERTEX*)RenderService->LockVertexBuffer(vBuf);
    if(vertBuf)
    {
        for(int rn=0; rn<ropeQuantity; rn++)
            if(rlist[rn]->bUse && !rlist[rn]->bDeleted && !gdata[rlist[rn]->HostGroup].bDeleted)
            {
                SetVertexes(rlist[rn],0.f);
                SetTextureGrid(rlist[rn]);
            }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

void ROPE::SetVertexes(ROPEDATA *pr,float dtime)
{
    // установить параметры формы веревки
    float deepVal;
    pr->segnum=0;
    deepVal=pr->ropeWave*(MIN_DEEP_MUL+VAR_DEEP_MUL*sinf(pr->angDeep));
    pr->vDeep.x = deepVal*sinf(VAR_ROTATE_ANGL*sinf(pr->angRot));
    pr->vDeep.y = deepVal;
    pr->vDeep.z = 0.;
    pr->angDeep += DEEP_CHANGE_SPEED*dtime; if( pr->angDeep>2.f*PI ) pr->angDeep=0.f;
    pr->angRot += ROTATE_SPEED*dtime; if( pr->angRot>2.f*PI ) pr->angRot=0.f;

    int vertnum = pr->sv;

    CVECTOR cvb,cve,cv;
    // Get begin point into Ship coordinate
    gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->bMatWorld*pr->pBeg,cvb);
    // Get end point into Ship coordinate
    gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->eMatWorld*pr->pEnd,cve);

    // Установить первую и последнюю точки веревки
    vertBuf[vertnum].pos=cvb;
    vertBuf[vertnum+pr->nv-1].pos=cve;

    cvb += pr->cv;

    vertnum++;

    CVECTOR dtV = (cve - cvb - pr->cv)/(float)pr->segquant;
    CVECTOR deepV = pr->vDeep*(4.f/(float)(pr->segquant*pr->segquant));
    for(int segn=0; segn<=pr->segquant; segn++)
    {
        CVECTOR cv = cvb;
		if( pr->bMakeWave ) cv += deepV*(float)(segn*(pr->segquant-segn));

        // установить новые координаты в текущем сечении
        for( int i=0; i<ROPE_EDGE; i++ )
            vertBuf[vertnum+i].pos = cv + pr->pos[i];

        cvb += dtV;
        vertnum += ROPE_EDGE;
    }
}
/*
void ROPE::DoMove(ROPEDATA *pr)
{
    if(pr->bUse)
    {
        int vertnum=pr->sv;
        CVECTOR cvb,cve,cv;
        // Get begin point into Ship coordinate
        gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->bMatWorld*pr->pBeg,cvb);
        // Get end point into Ship coordinate
        gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->eMatWorld*pr->pEnd,cve);

        // Установить первую и последнюю точки веревки
        vertBuf[vertnum].pos=cvb;
        vertBuf[vertnum+pr->nv-1].pos=cve;

        cvb += pr->cv;

        CVECTOR chV = pr->pOld - pr->pBeg;
        if( chV.x>MaxCh || chV.x<-MaxCh ||
            chV.y>MaxCh || chV.y<-MaxCh ||
            chV.z>MaxCh || chV.z<-MaxCh )
        {
            vertnum++;
            CVECTOR dc = (cve - cvb - pr->cv)/(float)pr->segquant;
            for(int j=0; j<=pr->segquant; j++)
            {
                // установить новые координаты в текущем сечении
                for( int i=0; i<ROPE_EDGE; i++ )
                    vertBuf[vertnum++].pos=cvb+pr->pos[i];
                cvb+=dc;
            }

            pr->pOld=pr->pBeg;
            pr->segnum=pr->segquant;
        }
        else
        {
            // базовый номер вертексов текущего сечения
            vertnum+=pr->segnum*ROPE_EDGE+1;

            cvb += (cve - cvb - pr->cv)*((float)pr->segnum/(float)pr->segquant);
            cvb += pr->vDeep*((float)(pr->segnum*(pr->segquant - pr->segnum))*4.f/(float)(pr->segquant*pr->segquant));

            // установить новые координаты в текущем сечении
            for( int i=0; i<ROPE_EDGE; i++ )
                vertBuf[vertnum+i].pos=cvb+pr->pos[i];

            if( ++(pr->segnum)>pr->segquant ) 
            {
                float deepVal;
                pr->segnum=0;
                deepVal=pr->ropeWave*(1.f+.7f*sinf(pr->angDeep));
                pr->vDeep.x = deepVal*sinf(.2f*PI*sinf(pr->angRot));
                pr->vDeep.y = deepVal;
                pr->vDeep.z = 0.;
                pr->angDeep += .1f*PI; if( pr->angDeep>2.f*PI ) pr->angDeep=0.f;
                pr->angRot += .03f*PI; if( pr->angRot>2.f*PI ) pr->angRot=0.f;
            }
        }
    }
}
*/
void ROPE::AddLabel(GEOS::LABEL &lbl,NODE *nod, bool bDontSage)
{
    ROPEDATA *rd;
    int  ropeNum,grNum;
    int  rn;

    if(nod==0) return;

	if( bDontSage ) ropeNum = atoi( &lbl.name[4] ); // fal
	else ropeNum = atoi( &lbl.name[5] ); // rope
	if( ropeNum==0 ) {
		NODE* pTmpRootNod = nod;
		while( pTmpRootNod->parent ) pTmpRootNod = pTmpRootNod->parent;
		api->Trace("Warning! Found rope with number 0: (model = %s) (label = %s)",
			pTmpRootNod->GetName(), lbl.name );
	}
	if( bDontSage ) ropeNum += 1000;

    for(rn=0; rn<ropeQuantity; rn++)
        if(rlist[rn]->ropeNum==ropeNum && rlist[rn]->HostGroup==groupQuantity-1) break;
    if(rn==ropeQuantity) // добавляем новую веревку
    {
        // изменим список веревок
        if(rlist==0)
        {
            rlist=NEW ROPEDATA*[1];
            ropeQuantity=1;
        }
        else
        {
            ROPEDATA** oldrlist=rlist;
            rlist= NEW ROPEDATA*[ropeQuantity+1];
            memcpy(rlist,oldrlist,sizeof(ROPEDATA*)*ropeQuantity);
            delete oldrlist; ropeQuantity++;
        }
        rd = rlist[ropeQuantity-1] = NEW ROPEDATA;
        PZERO(rd,sizeof(ROPEDATA));
        rd->ropeNum = ropeNum;
        rd->HostGroup = groupQuantity-1;
        rd->btie = rd->etie = false;
		rd->bMakeWave = !bDontSage;
    }
    else rd=rlist[rn];

    if( !strncmp(lbl.group_name,"sail",4) )
        grNum=atoi(&lbl.group_name[5]);
    else
        grNum=-1;

	switch( bDontSage ? lbl.name[3] : lbl.name[4] )
    {
    case 'b': // begining tip of rope
        rd->pBeg.x=lbl.m[3][0];
        rd->pBeg.y=lbl.m[3][1];
        rd->pBeg.z=lbl.m[3][2];
        rd->bMatWorld=&nod->glob_mtx; // get host matrix
        rd->bgnum=grNum;
        // привязка к парусу
        if( grNum>=0 ) // begin tip of rope - tie with sail
        {
            rd->pBeg.x += nod->glob_mtx.matrix[3];
            rd->pBeg.y += nod->glob_mtx.matrix[7];
            rd->pBeg.z += nod->glob_mtx.matrix[11];
            rd->btie=true;
        }
        break;
    case 'e': // ending tip of rope
        rd->pEnd.x=lbl.m[3][0];
        rd->pEnd.y=lbl.m[3][1];
        rd->pEnd.z=lbl.m[3][2];
        rd->eMatWorld=&nod->glob_mtx; // get host matrix
        rd->egnum=grNum;
        // привязка к парусу
        if( grNum>=0 ) // begin tip of rope - tie with sail
        {
            rd->pEnd.x += nod->glob_mtx.matrix[3];
            rd->pEnd.y += nod->glob_mtx.matrix[7];
            rd->pEnd.z += nod->glob_mtx.matrix[11];
            rd->etie=true;
        }
        break;
    }

    // now getting all two points
    CVECTOR ce,cb;
    if( rd->bMatWorld!=0 && rd->eMatWorld!=0 )
    {
		cb=*rd->bMatWorld*rd->pBeg;
		ce=*rd->eMatWorld*rd->pEnd;
        rd->len = sqrtf(~(ce-cb));

		if(rd->len<.1f || rd->len>200.f)
		{
			api->Trace("Bad rope length = %f for rope num %d",rd->len,rd->ropeNum);
			delete rd;
			if(ropeQuantity==1)
			{
				delete rlist;
				rlist=0;
			}
			else
			{
				ROPEDATA** oldrlist=rlist;
				rlist=NEW ROPEDATA*[ropeQuantity-1];
				if(rn>0)
					memcpy(rlist,oldrlist,sizeof(ROPEDATA*)*rn);
				if(rn<ropeQuantity-1)
					memcpy(&rlist[rn],&oldrlist[rn+1],sizeof(ROPEDATA*)*(ropeQuantity-1-rn));
				delete oldrlist;
			}
			ropeQuantity--;
		}
		else
		{
			rd->segquant=(WORD)(rd->len/ROPE_SEG_LENGTH)+1;
			rd->cv = (!(ce - cb))*ROPE_END_LENGTH; // vector for rope edge length
			// Set normals with length equal the rope width
			CVECTOR norm;
			float cvert,chorz, svert,shorz;
			norm = ce - cb;
			cvert= norm.z/rd->len;
			svert= norm.y/rd->len;
			float ftmp = sqrtf(norm.x*norm.x+norm.z*norm.z);
			if(ftmp>=0.0001f)
			{
				chorz= norm.z/ftmp;
				shorz= norm.x/ftmp;

				float ca,sa;
				for(int i=0; i<ROPE_EDGE; i++)
				{
					ca=cosf((float)i/(float)ROPE_EDGE*2.f*PI);
					sa=sinf((float)i/(float)ROPE_EDGE*2.f*PI);
					// vert & horz
					rd->pos[i].x= ROPE_WIDTH/2.f*(sa*chorz + ca*svert*shorz);
					rd->pos[i].y= ROPE_WIDTH/2.f*ca*cvert;
					rd->pos[i].z= ROPE_WIDTH/2.f*(ca*svert*chorz - sa*shorz);
				}
			}
			else
			{
				for(int i=0; i<ROPE_EDGE; i++)
				{
					// vert & horz
					rd->pos[i].x= ROPE_WIDTH/2.f * sinf((float)i/(float)ROPE_EDGE*2.f*PI);
					rd->pos[i].y= 0.f;
					rd->pos[i].z= ROPE_WIDTH/2.f * cosf((float)i/(float)ROPE_EDGE*2.f*PI);
				}
			}

			rd->angDeep=0.f;
			rd->angRot=0.f;
			rd->vDeep=0.f;

			ENTITY_ID sailEI;
			if(_CORE_API->FindClass(&sailEI,"sail",0))
			{
				MODEL* mdl=(MODEL*)_CORE_API->GetEntityPointer(&gdata[rd->HostGroup].modelEI);
				if(mdl==0)
					rd->btie=rd->etie=false;
				else for(int i=0; i<10000; i++)
				{
					NODE* nd=mdl->GetNode(i);
					if(nd==0) break;
                
					if( rd->btie && rd->bMatWorld==&nd->glob_mtx )
						_CORE_API->Send_Message(sailEI,"lplpl",MSG_SAIL_ROPE_TIE,nd,rd->bgnum,&rd->pBeg,rd->ropeNum);
					if( rd->etie && rd->eMatWorld==&nd->glob_mtx )
						_CORE_API->Send_Message(sailEI,"lplpl",MSG_SAIL_ROPE_TIE,nd,rd->egnum,&rd->pEnd,-rd->ropeNum);
				}
			}
			else
			{
				rd->btie=rd->etie=false;
			}
		}
	}
}

// получить конечную точку веревки в координатах начальной точки
void ROPE::GetEndPoint(CVECTOR* cv,int ropenum,ENTITY_ID &mdl_id)
{
    int rn;

    if(cv==0) // плохой указатель на вектор
        return;

    // выяснить какой конец веревки нам надо получить
    bool bGetEnd=false;
    if(ropenum<0) ropenum=-ropenum;
    else bGetEnd=true;

    // найти веревку с данным номером
    for(rn=0; rn<ropeQuantity; rn++)
        if(rlist[rn]->ropeNum == ropenum && gdata[rlist[rn]->HostGroup].modelEI==mdl_id) break;

    if(rn==ropeQuantity) // нет такой веревки
        return;

    if(bGetEnd)
        rlist[rn]->bMatWorld->MulToInv(*rlist[rn]->eMatWorld * rlist[rn]->pEnd,*cv);
    else
        rlist[rn]->eMatWorld->MulToInv(*rlist[rn]->bMatWorld * rlist[rn]->pBeg,*cv);
}

void ROPE::LoadIni()
{
    GUARD(ROPE::LoadIni());
	char	section[256];
    char    param[256];

	INIFILE * ini;
	ini = _CORE_API->fio->OpenIniFile("resource\\ini\\rigging.ini");
	if(!ini) THROW("rigging.ini file not found!");

	sprintf(section,"ROPES");

    // имя текстуры
    ini->ReadString(section,"TextureName",param,sizeof(param)-1,"sail_rope.tga");
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
    // длина одного сегмента веревки
    ROPE_SEG_LENGTH=ini->GetFloat(section,"fSEG_LENGTH",2.f);
    // толщина веревки
    ROPE_WIDTH=ini->GetFloat(section,"fWIDTH",0.025f);
    // длина веревки (треугольника) в точке соединения с парусом относительно общей длинны
    ROPE_END_LENGTH=ini->GetFloat(section,"fEND_LENGTH",0.05f);
    // амплитуда колебания веревки в абсолютных значениях
    ROPE_WAVE=ini->GetFloat(section,"fWAVE",0.1f);
    // максимальное изменение координаты при котором происходит перерасчет всей веревки
    MaxCh=ini->GetFloat(section,"fMaxCh",.1f);
    // длина веревки на которую ложится текстура
    ROPE_TEX_LEN=ini->GetFloat(section,"fTexHeight",.5f);
	// максимальное растояние с кототрого видны веревки
	fMaxRopeDist=ini->GetFloat(section,"fMaxDist",5000.f);
    // скорость изменения глубины прогиба веревки
    DEEP_CHANGE_SPEED= ini->GetFloat(section,"fDeepChangeSpeed",0.15f);
    // скорость качания веревки
    ROTATE_SPEED= ini->GetFloat(section,"fRotateSpeed",0.08f);
    // минимальное значение коэффициента использования прогиба веревки
    MIN_DEEP_MUL= ini->GetFloat(section,"fMinDeepMul",1.f);
    // предел изменения коэффициента использования прогиба веревки
    VAR_DEEP_MUL= ini->GetFloat(section,"fVarDeepMul",.7f);
    // амплитуда угла качания веревки
    VAR_ROTATE_ANGL= ini->GetFloat(section,"fRotateAng",.1f);

    delete ini;
    UNGUARD
}

void ROPE::FirstRun()
{
/*    SetAdd(wRopeLast);
    if(wRopeLast>0)
        nIndx*=3;
    else
    {
        nVert=0; nIndx=0;
    }

    int gn;
    if(wRopeLast<ropeQuantity)
        gn=rlist[wRopeLast]->HostGroup;
    else
        gn=groupQuantity;

    for(; gn<groupQuantity; gn++)
    {
        gdata[gn].nt=0;
        gdata[gn].nv=0;
        gdata[gn].st=nIndx;
        gdata[gn].sv=nVert;
        gdata[gn].ropeQuantity=0;
        for(int rn=wRopeLast; rn<ropeQuantity; rn++)
        {
            if(rlist[rn]->HostGroup==gn)
            {
                gdata[gn].ropeQuantity++;
                gdata[gn].nv+=rlist[rn]->nv;
                gdata[gn].nt+=rlist[rn]->nt;
                rlist[rn]->sv=nVert;
                rlist[rn]->st=nIndx;
                nVert+=rlist[rn]->nv;
                nIndx+=rlist[rn]->nt*3;
            }
        }
        gdata[gn].ropeIdx = NEW int[gdata[gn].ropeQuantity];
        int idx=0;
        for(rn=wRopeLast; rn<ropeQuantity; rn++)
            if(rlist[rn]->HostGroup==gn)
                gdata[gn].ropeIdx[idx++]=rn;
    }*/

    if(nVert && nIndx)
    {
        if(ropeQuantity>wRopeLast)
        {
            if(wRopeLast)
            {
				VERTEX_BUFFER_RELEASE(RenderService,vBuf);
				INDEX_BUFFER_RELEASE(RenderService,iBuf);
            }
            vBuf=RenderService->CreateVertexBuffer(ROPEVERTEX_FORMAT,nVert*sizeof(ROPEVERTEX),D3DUSAGE_WRITEONLY);
            iBuf=RenderService->CreateIndexBuffer(nIndx*2);

			if(vBuf>=0 && iBuf>=0)
			{
				SetVertexes();
				SetIndex();
			}
			else api->Trace("Can`t create index or vertex buffer (index = %d, vertex = %d)",nIndx,nVert);
        }
        nIndx/=3; // translate index quantity to triangle quantity
        bUse=true;
    }

    bFirstRun=false;
    wRopeLast=ropeQuantity;
}

void ROPE::SetTextureGrid(ROPEDATA *pv)
{
    int iv=pv->sv;

    float tvMax = sqrtf(~(*pv->eMatWorld*pv->pEnd - *pv->bMatWorld*pv->pBeg));

    vertBuf[iv].tu=0.f;
    vertBuf[iv].tv=0.f;

    vertBuf[iv+pv->nv-1].tu=1.f;
    vertBuf[iv+pv->nv-1].tv=tvMax;

    // Set begin segment point and delta point
    iv++;
    float tv = ROPE_END_LENGTH/ROPE_TEX_LEN;
    float dtv = (tvMax/ROPE_TEX_LEN-tv)/(float)pv->segquant;
    for(int j=0; j<=pv->segquant; j++)
    {
        for(int i=0; i<ROPE_EDGE; i++ )
        {
            if(i<=ROPE_EDGE/2)
                vertBuf[iv+i].tu= (float)i/(float)(ROPE_EDGE/2);
            else
                vertBuf[iv+i].tu= 1.f - (float)(i-ROPE_EDGE/2)/(float)((ROPE_EDGE+1)/2);
            vertBuf[iv+i].tv= tv;
        }
        iv+=ROPE_EDGE;
        tv+=dtv;
    }
}

void ROPE::SetAdd(int firstNum)
{
    // set vertex and index buffers
    for(int rn=firstNum; rn<ropeQuantity; rn++)
    {
        // удалить плохие веревки
        while( rlist[rn]->bMatWorld==0 || rlist[rn]->eMatWorld==0 )
        {
			long gn = rlist[rn]->HostGroup;
			const char* pcModlName = 0;
			MODEL* pMdl = (MODEL*)api->GetEntityPointer(&gdata[gn].modelEI);
			if( pMdl && pMdl->GetNode(0) )
				pcModlName = pMdl->GetNode(0)->GetName();

			api->Trace("Bad rope data for rope: (model=%s) (rope num = %d) (begin group=%d, end group=%d)",
				pcModlName,rlist[rn]->ropeNum, rlist[rn]->bgnum, rlist[rn]->egnum);
			api->Trace("Begin pointer = %d? end pointer = %d",rlist[rn]->bMatWorld,rlist[rn]->eMatWorld);
            //_THROW("Rope error: Not label");
            delete rlist[rn];
            ropeQuantity--;
            if(ropeQuantity)
            {
                ROPEDATA** oldrlist=rlist;
                rlist = NEW ROPEDATA*[ropeQuantity];
                if(rlist==0)
                    rlist=oldrlist;
                if(rn>0)
                    memcpy(rlist,oldrlist,sizeof(ROPEDATA*)*rn);
                if(rn<ropeQuantity)
                    memcpy(&rlist[rn],&oldrlist[rn+1],sizeof(ROPEDATA*)*(ropeQuantity-rn));
                if(oldrlist!=rlist)
                    delete oldrlist;
            }
            else
                delete rlist;
            if(rn==ropeQuantity) break;
        }
        if(rn==ropeQuantity) break;

        rlist[rn]->bUse=true;
        rlist[rn]->nv=(rlist[rn]->segquant+1)*ROPE_EDGE+2;
        rlist[rn]->nt=(rlist[rn]->segquant+1)*ROPE_EDGE*2;
        rlist[rn]->segnum=0;
        if(rlist[rn]->segquant<=2)
            rlist[rn]->ropeWave=-ROPE_WAVE*.2f;
        else
            rlist[rn]->ropeWave=-ROPE_WAVE;
    }

	for(rn=firstNum; rn<ropeQuantity; rn++)
	{
		if( rlist[rn]->segquant > 100 || rlist[rn]->segquant<0 )
			rlist[rn]->segquant = rlist[rn]->segquant;
	}
}

void ROPE::DoDelete()
{
    DWORD oldnVert=nVert;

    // пройтись по удаленным группам и пометить на удаление все принадлежащие им веревки
    for(int gn=0; gn<groupQuantity; gn++)
        if(gdata[gn].bDeleted)
            for(int idx=0; idx<gdata[gn].ropeQuantity; idx++)
                rlist[gdata[gn].ropeIdx[idx]]->bDeleted=true;

    // удалить удаленные веревки в списке веревок
    int nrn=0;
    for(int rn=0; rn<ropeQuantity; rn++)
    {
        if(rlist[rn]->bDeleted)
        {
            // удалить веревку
            delete rlist[rn];
            continue;
        }

        if(nrn<rn)
            rlist[nrn]=rlist[rn];
        nrn++;
    }
    ropeQuantity=nrn;

    // переделаем список групп
    int ngn=0;
    nIndx=0; nVert=0;
    for(gn=0; gn<groupQuantity; gn++)
    {
        // если группа удалена, то пропускаем ее
        if(gdata[gn].bDeleted)
        {
            if(gdata[gn].ropeIdx)
                delete gdata[gn].ropeIdx;
            continue;
        }

        // подправим индексы на вертекс и индекс буферы
        gdata[gn].st=nIndx;
        gdata[gn].sv=nVert;
        gdata[gn].nt=0;
        gdata[gn].nv=0;

        // для всех веревок принадлежащих этой группе исправим номер хозяина
        int rq=0;
        for(rn=0; rn<ropeQuantity; rn++)
            if(rlist[rn]->HostGroup==gn)
            {
                rlist[rn]->HostGroup = ngn;
                gdata[gn].ropeIdx[rq++] = rn;
                gdata[gn].nt += rlist[rn]->nt;
                gdata[gn].nv += rlist[rn]->nv;
                rlist[rn]->sv = nVert;
                rlist[rn]->st = nIndx;
                nVert += rlist[rn]->nv;
                nIndx += rlist[rn]->nt*3;
            }

        // если группа стала пустой, то удаляем ее физически
        if(rq==0)
        {
            if(gdata[gn].ropeIdx)
                delete gdata[gn].ropeIdx;
            continue;
        }

        // если изменился список веревок в хозяине, то изменим его
        if(rq<gdata[gn].ropeQuantity)
        {
            int *oldropeIdx = gdata[gn].ropeIdx;
            gdata[gn].ropeIdx = NEW int[rq];
            if(gdata[gn].ropeIdx==0)
                gdata[gn].ropeIdx=oldropeIdx;

            gdata[gn].ropeQuantity=rq;
            memcpy(gdata[gn].ropeIdx,oldropeIdx,sizeof(int)*rq);
            if(gdata[gn].ropeIdx!=oldropeIdx)
                delete oldropeIdx;
        }

        if(ngn<gn)
            memcpy(&gdata[ngn],&gdata[gn],sizeof(GROUPDATA));
        ngn++;
    }
    groupQuantity=ngn;

    // изменим размер вертекс и индекс буферов
    nIndx/=3;
    if(oldnVert!=nVert)
    {
		VERTEX_BUFFER_RELEASE(RenderService,vBuf);
		INDEX_BUFFER_RELEASE(RenderService,iBuf);

        if(nVert>0)
            vBuf=RenderService->CreateVertexBuffer(ROPEVERTEX_FORMAT,nVert*sizeof(ROPEVERTEX),D3DUSAGE_WRITEONLY);
        if(nIndx>0)
            iBuf=RenderService->CreateIndexBuffer(nIndx*6);

        if(nVert>0 && nIndx>0)
        {
            SetVertexes();
            SetIndex();
        }
    }

    wRopeLast=ropeQuantity;
    bYesDeleted=false;
    bUse = ropeQuantity>0;
}

bool ROPE::IsAbsentRope(ENTITY_ID &mdl_id, int ropenum)
{
    bool retVal=true;

    // найдем нужную группу
    for(int gn=0; gn<groupQuantity; gn++)
        if(gdata[gn].modelEI==mdl_id) break;

    // пройдемся по всем веревкам и найдем нужную нам веревку
    int rn=ropeQuantity;
    if(gn<groupQuantity)
        for(rn=0; rn<ropeQuantity; rn++)
            if(rlist[rn]->ropeNum==ropenum && rlist[rn]->HostGroup==gn) break;

    if(rn<ropeQuantity)
        if( (!rlist[rn]->bDeleted) && (!gdata[gn].bDeleted) ) retVal=false;

    return retVal;
}

void ROPE::DoDeleteUntie(ENTITY_ID &mdl_id, NODE *rnod, int gNum)
{
    for(int gn=0; gn<groupQuantity; gn++)
        if(gdata[gn].modelEI==mdl_id) break;

    if(gn<groupQuantity)
    {
        for(int ri=0; ri<gdata[gn].ropeQuantity; ri++)
        {
            int rn=gdata[gn].ropeIdx[ri];
            if( ((rlist[rn]->bMatWorld==&rnod->glob_mtx) &&
                 (rlist[rn]->bgnum==gNum)) ||
                ((rlist[rn]->eMatWorld==&rnod->glob_mtx) &&
                 (rlist[rn]->egnum==gNum)) )
            {
                rlist[rn]->bDeleted=true;
                bYesDeleted=true;
            }
        }
    }
}
