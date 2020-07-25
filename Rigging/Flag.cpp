#include "flag.h"
#include "..\common_h\defines.h"
#include "..\common_h\sail_msg.h"
#include "..\common_h\Weather_base.h"
#include "rigging_define.h"
#include <stdio.h>

FLAG::FLAG()
{
    bUse			= false;
    RenderService	= 0;
    TextureName		= 0;
    bFirstRun		= true;
    texl			= -1;
    flist			= 0; 
	flagQuantity	= 0;
    gdata			= 0; 
	groupQuantity	= 0;
    bYesDeleted		= false;
    wFlagLast		= 0;
	vBuf 			= iBuf 	= -1;
	nVert 			= nIndx = 0;
}

FLAG::~FLAG()
{
    TEXTURE_RELEASE(RenderService,texl);
    PTR_DELETE(TextureName);
    PTR_DELETE(gdata);
	VERTEX_BUFFER_RELEASE(RenderService,vBuf);
	INDEX_BUFFER_RELEASE(RenderService,iBuf);

    while(flagQuantity > 0)
	{
		flagQuantity--;
        PTR_DELETE(flist[flagQuantity]);
	}
    PTR_DELETE(flist);
}

bool FLAG::Init()
{
	GUARD(FLAG::FLAG())
	SetDevice();
	UNGUARD
	return true;
}
 
void FLAG::SetDevice()
{
    // получить сервис рендера
	RenderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!RenderService)
	{
		SE_THROW_MSG("No service: dx8render");
	}
	globalWind.ang.x	= 0.f;
	globalWind.ang.y	= 0.f;
	globalWind.ang.z	= 1.f;
	globalWind.base		= 1.f;
    LoadIni();
    texl = RenderService->TextureCreate(TextureName);
}

bool FLAG::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool FLAG::LoadState(ENTITY_STATE * state)
{
	return true;
}

void FLAG::Execute(dword Delta_Time)
{
    if(bFirstRun) FirstRun();
    if(bYesDeleted) DoDelete();
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

        // получим значение ветра
        ENTITY_ID ei;
        if( _CORE_API->FindClass(&ei,"weather",0) )
        {
            WEATHER_BASE *wb = (WEATHER_BASE*)_CORE_API->GetEntityPointer(&ei);
            globalWind.ang.x=wb->GetFloat(whf_wind_angle);
            globalWind.ang.z=cosf(globalWind.ang.x);
            globalWind.ang.x=sinf(globalWind.ang.x);
            globalWind.base=wb->GetFloat(whf_wind_speed)/fWindMaxValue;
        }

        // расчет формы флага
        vertBuf=(FLAGLXVERTEX*)RenderService->LockVertexBuffer(vBuf);
        if( vertBuf )
        {
            float dt=(float)Delta_Time*0.02f;
            for(int fn=0; fn<flagQuantity; fn++)
                DoMove(flist[fn],dt);
            RenderService->UnLockVertexBuffer(vBuf);
        }
    }
}

void FLAG::Realize(dword Delta_Time)
{
    if(bUse)
    {
        DWORD rtm;

        _asm rdtsc _asm mov rtm,eax

        RenderService->TextureSet(0,texl);
		DWORD ambient;
		RenderService->GetRenderState(D3DRS_AMBIENT,&ambient);
		RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,ambient);
        RenderService->SetTransform(D3DTS_WORLD,&rootMatrix);

        // draw nature flag
		if(nVert!=0 && nIndx!=0) RenderService->DrawBuffer(vBuf, sizeof(FLAGLXVERTEX), iBuf, 0, nVert, 0, nIndx, "ShipFlag");
        _asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
        // Print info
        // RenderService->Print(0,220,"Flags tics= %d",rtm);
    }
}

dword _cdecl FLAG::ProcessMessage(MESSAGE & message)
{
	long code = message.Long();

	switch (code)
    {
		case MSG_FLAG_INIT:
		{
			ENTITY_ID eidModel = message.EntityID();
			long nNation = message.Long();
			ENTITY_ID eidShip = message.EntityID();

			MODEL *host_mdl;
			host_mdl=(MODEL*)api->GetEntityPointer(&eidModel);
			if( host_mdl==0 ) 
			{
				api->Trace("Missing INIT message to FLAG: bad MODEL");
			}

			if(groupQuantity==0)
			{
				gdata = NEW GROUPDATA[1];
				if(gdata==0) SE_THROW_MSG("Not memory allocation");
				groupQuantity=1;
			}
			else
			{
				GROUPDATA *oldgdata=gdata;
				gdata = NEW GROUPDATA[groupQuantity+1];
				if(gdata==0)
					SE_THROW_MSG("Not memory allocation");
				memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
				delete oldgdata; groupQuantity++;
			}
			gdata[groupQuantity-1].model_id = eidModel;
			gdata[groupQuantity-1].nation = nNation;
			gdata[groupQuantity-1].bDeleted = false;			
			gdata[groupQuantity-1].ship_id = eidShip;			
			gdata[groupQuantity-1].isShip = true;
			gdata[groupQuantity-1].char_attributes = ((VAI_OBJBASE*)gdata[groupQuantity-1].ship_id.pointer)->GetACharacter();

			NODE* nod;
			GEOS::INFO gi;
			GEOS::LABEL gl;
			int i,j;
			for(j=0;true;j++)
			{
				nod=host_mdl->GetNode(j);
				if(nod==NULL || nod->geo==NULL) break;

				nod->geo->GetInfo(gi);
				for(i=0; i<gi.nlabels; i++)
				{
					nod->geo->GetLabel(i,gl);
					if(!strncmp(gl.group_name,"sflag",5))	// special flag
					{
						AddLabel(gl,nod,1,1);
					}	
					else
					{
						if(!strncmp(gl.group_name,"flag",4))	// ordinary flag
						AddLabel(gl,nod,0,1);					
					}	
				}
			}
			bFirstRun=true;
		}
        break;

		case MSG_FLAG_INIT_TOWN:
		{
			ENTITY_ID eidModel = message.EntityID();
			long nNation = message.Long();

			MODEL *host_mdl;
			host_mdl=(MODEL*)api->GetEntityPointer(&eidModel);
			if( host_mdl==0 ) 
			{
				api->Trace("Missing INIT message to FLAG: bad MODEL");
			}

			if(groupQuantity==0)
			{
				gdata = NEW GROUPDATA[1];
				if(gdata==0) SE_THROW_MSG("Not memory allocation");
				groupQuantity=1;
			}
			else
			{
				GROUPDATA *oldgdata=gdata;
				gdata = NEW GROUPDATA[groupQuantity+1];
				if(gdata==0)
					SE_THROW_MSG("Not memory allocation");
				memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
				delete oldgdata; groupQuantity++;
			}
			gdata[groupQuantity-1].model_id = eidModel;
			gdata[groupQuantity-1].nation = nNation;
			gdata[groupQuantity-1].bDeleted = false;			
			gdata[groupQuantity-1].isShip = false;

			NODE* nod;
			GEOS::INFO gi;
			GEOS::LABEL gl;
			int i,j;
			for(j=0;true;j++)
			{
				nod=host_mdl->GetNode(j);
				if(nod==NULL || nod->geo==NULL) break;

				nod->geo->GetInfo(gi);
				for(i=0; i<gi.nlabels; i++)
				{
					nod->geo->GetLabel(i,gl);
					if(!strncmp(gl.group_name,"sflag",5))	// special flag
					{
						AddLabel(gl,nod,1,0);
					}	
					else
					{
						if(!strncmp(gl.group_name,"flag",4))	// ordinary flag
						AddLabel(gl,nod,0,0);					
					}	
				}
			}
			bFirstRun=true;		
		}
		break;
		
		case MSG_FLAG_DEL_GROUP:
			GroupDelete(message.EntityID());
        break;

		case MSG_FLAG_TO_NEWHOST: 
		{
			ENTITY_ID oldModelID = message.EntityID();
			long flagGroupNum = message.Long();
			ENTITY_ID newModelID = message.EntityID();
			MoveOtherHost(newModelID, flagGroupNum, oldModelID);
		}
        break;
    }
    return 0;
}

void FLAG::SetTextureCoordinate()
{
    if(bUse)
    {
        int i;
        long sIdx;
        float stu,addtu,dtu;
        float stv,addtv,dtv;

        FLAGLXVERTEX* pv=(FLAGLXVERTEX*)RenderService->LockVertexBuffer(vBuf);
        if(pv)
        {
            for(int fn=0; fn<flagQuantity; fn++)
            {
				if(flist[fn]==NULL) continue;
                sIdx=flist[fn]->sv;
                addtu=1.f/(float)FlagTextureQuantity;
				addtv=1.f/(float)FlagTextureQuantityRow;
                stu=addtu*flist[fn]->texNumC;				
                stv=addtv*flist[fn]->texNumR; 
                if(flist[fn]->triangle)
                {
                    dtu=addtu/(float)(flist[fn]->vectQuant+1);
					dtv=0.5f/(float)(flist[fn]->vectQuant+1);
                }
                else
                {
                    dtu=addtu/(float)flist[fn]->vectQuant;
					dtv=0.f;
                }
                for(i=0; i<=flist[fn]->vectQuant; i++)
                {
                    pv[sIdx].tu=stu;
                    pv[sIdx++].tv=stv;
                    pv[sIdx].tu=stu;
					pv[sIdx++].tv=stv+addtv;
					
                    stu+=dtu;
                    stv+=dtv;
                }
                if( flist[fn]->triangle )
                {
                    pv[sIdx].tv=stv;
                    pv[sIdx].tu=stu;
                }
            }
            RenderService->UnLockVertexBuffer(vBuf);
        }
    }
}

void FLAG::DoMove(FLAGDATA *pr,float delta_time)
{
	if(pr==NULL) return;
    CVECTOR cPos;
    cPos=  *pr->pMatWorld*pr->spos;

    float xMul = globalWind.ang.x; 
    float zMul = globalWind.ang.z;

    if( (pr->Alfa+=(ALFA_DEPEND+ALFA_RAND*(float)rand()/(float)RAND_MAX)*delta_time)>PIm2 ) pr->Alfa=0.f;
    if( (pr->Beta+=(BETA_DEPEND+BETA_RAND*(float)rand()/(float)RAND_MAX)*delta_time)>PIm2 ) pr->Beta=0.f;
    float Alfa=-pr->Alfa;
    float Beta=-pr->Beta;
    float sAlfa=sinf(Alfa); float cAlfa=cosf(Alfa);
    float sBeta=sinf(Beta); float cBeta=cosf(Beta);

    CVECTOR dhVect;
    dhVect= *pr->pMatWorld*pr->dhv - pr->pMatWorld->Pos();

    long sIdx = pr->sv;
    CVECTOR dopVect;
    CVECTOR incrVect;
    incrVect.x = FLAGVECTORLEN*xMul;
    incrVect.z = FLAGVECTORLEN*zMul;
    incrVect.y = -DOWNVAL;

    float oldSa=sAlfa; float oldCa=cAlfa;
    float oldSb=sBeta; float oldCb=cBeta;
    float hIncr=pr->dhv.y;

    float tmp = fAlfaStep+fAlfaMax/(float)pr->vectQuant;
    float sa=sinf(tmp); float ca=cosf(tmp);
    tmp = fBetaStep+fBetaMax/(float)pr->vectQuant;
    float sb=sinf(tmp); float cb=cosf(tmp);

    for( int i=0; i<=pr->vectQuant; i++ )
    {
        tmp = fWindAm*(sAlfa-oldSa);
        dopVect.y = fWindAm*(cAlfa-oldCa);
        dopVect.x = tmp*zMul;
        dopVect.z = tmp*xMul;

        vertBuf[sIdx].pos = cPos + dopVect + dhVect;
		sIdx++;
        vertBuf[sIdx].pos = cPos + dopVect - dhVect;
		sIdx++;

        // посчитать новый угол (синусы\косинусы)
        tmp = sAlfa;
        sAlfa = sAlfa*ca + sa*cAlfa;
        cAlfa = cAlfa*ca - tmp*sa;

        // посчитать новый угол (синусы\косинусы)
        tmp = sBeta;
        sBeta = sBeta*cb + sb*cBeta;
        cBeta = cBeta*cb - tmp*sb;

        hIncr-=pr->ddhv.y;
        tmp = hIncr*(sBeta-oldSb)*fRotAm;
        if(tmp>hIncr || tmp<-hIncr)
        {
            dhVect.y = 0.f;
            dhVect.x = hIncr*zMul;
            dhVect.z = hIncr*xMul;
        }
        else
        {
			float fT = hIncr*hIncr - tmp*tmp;
			if( fT<0.f ) fT = -fT;
            dhVect.y = sqrtf(hIncr*hIncr - tmp*tmp);
            dhVect.x = tmp*zMul;
            dhVect.z = tmp*xMul;
        }
        dhVect= *pr->pMatWorld*dhVect - pr->pMatWorld->Pos();

        cPos += incrVect;
    }

    if( pr->triangle )
    {
        dopVect.x = fWindAm*(sAlfa-oldSa);
        dopVect.z = dopVect.x*xMul;
        dopVect.x *= zMul;
        dopVect.y = -fWindAm*(cAlfa-oldCa);
        vertBuf[sIdx].pos = cPos + dopVect;
    }
}

void FLAG::AddLabel(GEOS::LABEL &gl, NODE *nod, bool isSpecialFlag, bool isShip)
{
    FLAGDATA *fd;
    int grNum;

    // for fail parameters do not set of data
    if( nod==0 ) return;

	if(isSpecialFlag) 	grNum=atoi(&gl.group_name[5]);	
    else 				grNum=atoi(&gl.group_name[4]);

	int fn = 0;
    for(fn=0; fn<flagQuantity; fn++)
        if( flist[fn]!=NULL &&
			flist[fn]->HostGroup==groupQuantity-1 &&
            flist[fn]->grNum==grNum &&
            flist[fn]->nod==nod)
        {
            fd=flist[fn];
            break;
        }
    if(fn==flagQuantity)
    {
        // create new flag
        fd = NEW FLAGDATA;
        if(fd==0)
            SE_THROW_MSG("Not memory allocation");
        PZERO(fd,sizeof(FLAGDATA));
        fd->triangle=true; // this is Vimpel		
		fd->isSpecialFlag=isSpecialFlag;
		fd->isShip=isShip;
        fd->pMatWorld=&nod->glob_mtx;
        fd->nod=nod;
        fd->grNum=grNum;
        fd->Alfa=0.f;
        fd->Beta=0.f;
        fd->HostGroup=groupQuantity-1;
        fd->bDeleted=false;

        if(flagQuantity==0)
        {
            flist= NEW FLAGDATA*[1];
            if(flist==0)
                SE_THROW_MSG("Not memory allocation");
            flagQuantity=1;
        }
        else
        {
            FLAGDATA **oldflist=flist;
            flist = NEW FLAGDATA*[flagQuantity+1];
            if(flist==0)
                SE_THROW_MSG("Not memory allocation");
            memcpy(flist,oldflist,sizeof(FLAGDATA*)*flagQuantity);
            delete oldflist; flagQuantity++;
        }
        flist[flagQuantity-1]=fd;
    }

    CVECTOR cv;
    cv.x = gl.m[3][0] + nod->loc_mtx.matrix[3];
    cv.y = gl.m[3][1] + nod->loc_mtx.matrix[7];
    cv.z = gl.m[3][2] + nod->loc_mtx.matrix[11];
    
    // set flag point
    switch(gl.name[1])
    {
    case '2': 	// bottom begin of flag
        fd->dhv = cv;
        break;
    case '3': 	// top end of flag (or center of the flag end for triangle flag)
        fd->ddhv = cv;
        break;
    case '4': 	// bottom ebd of flag
        fd->dv = cv;
        fd->triangle=false; // this not Vimpel - that Flag
        break;
    default: 	// case 1: top begin of flag
        fd->spos = cv;
    }
}

void FLAG::SetAll()
{
	// set vertex and index buffers
	nVert=nIndx=0;
	SetAdd(0);
}

void FLAG::SetTreangle()
{
    int i,idx;

    WORD *pt=(WORD*)RenderService->LockIndexBuffer(iBuf);
    if( pt )
    {
        for(int fn=0; fn<flagQuantity; fn++)
        {
			if(flist[fn]==NULL) continue;
            idx=flist[fn]->st;
            for(i=0; i<(int)flist[fn]->nt; i++)
            {
                pt[idx++]=(WORD)(i+flist[fn]->sv);
                pt[idx++]=(WORD)(i+flist[fn]->sv+1);
                pt[idx++]=(WORD)(i+flist[fn]->sv+2);
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void FLAG::LoadIni()
{
    GUARD(FLAG::LoadIni());
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
	if(!ini) SE_THROW("rigging.ini file not found!");

	sprintf(section,"FLAGS");
	
	// load texture parameters
    ini->ReadString(section,"TextureName",param,sizeof(param)-1,"flagall_ship_b.tga");
    if(TextureName!=0)
    {
        if(strcmp(TextureName,param))
        {
            delete TextureName;
            NEW char[strlen(param)+1];
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
	
    FlagTextureQuantity=(int)ini->GetLong(section,"TextureCountColumn", 4);	
    FlagTextureQuantityRow=(int)ini->GetLong(section,"TextureCountRow", 8);
    	
	SetTextureCoordinate();
	
    // длина сегмента флага
    FLAGVECTORLEN=ini->GetFloat(section,"fSegLen",0.2f);

    // Параметры влияния ветра
    // скорость ветра по вертикали
    ALFA_DEPEND=ini->GetFloat(section,"alfa_depend",0.1f);
    // скорость ветра по горизонтали
    BETA_DEPEND=ini->GetFloat(section,"beta_depend",0.06f);
    // максимальное значение случайного изменения угла Alpha
    ALFA_RAND=ini->GetFloat(section,"alpha_rand",0.1f);
    // максимальное значение случайного изменения угла Beta
    BETA_RAND=ini->GetFloat(section,"beta_rand",0.1f);
    // максимальный угол (для конца флага)
    fAlfaMax = ini->GetFloat(section,"fAlfaMax",4.71f);
    fAlfaStep = ini->GetFloat(section,"fAlfaStep",.2f);
    // максимальный угол (для конца флага)
    fBetaMax = ini->GetFloat(section,"fBetaMax",7.85f);
    fBetaStep = ini->GetFloat(section,"fBetaStep",.3f);
    // Параметры формирования формы флага и колебания его
    // амплитуда колебания направляющей линии флага
    fWindAm=ini->GetFloat(section,"fWindAm",0.2f);
    // амплитуда колебания(вращения) поперечника флага
    fRotAm=ini->GetFloat(section,"fRotAm",0.2f);
    // значение опускания флага на каждом шаге от мачты
    DOWNVAL=ini->GetFloat(section,"fDownVal",0.05f);
    // максимальное значение ветра
    fWindMaxValue=ini->GetFloat(section,"fWindMaxValue",12.f);
    // минимальное количество сегментов во флаге
    MinSegmentQuantity=ini->GetLong(section,"MinSegQuantity",4);

    delete ini;
    UNGUARD
}


void FLAG::FirstRun()
{
    if(wFlagLast) SetAdd(wFlagLast);
    else
    {
        SetAll();
        ZERO(rootMatrix);
        rootMatrix._11=rootMatrix._22=rootMatrix._33=rootMatrix._44=1.f;
    }

    if(nVert)
    {
        bUse=true;
        iBuf=RenderService->CreateIndexBufferManaged(nIndx*2);
        SetTreangle();
        vBuf=RenderService->CreateVertexBufferManaged(FLAGLXVERTEX_FORMAT,nVert*sizeof(FLAGLXVERTEX),D3DUSAGE_WRITEONLY);
        SetTextureCoordinate();
        nIndx/=3;
    }

    bFirstRun=false;
    wFlagLast=flagQuantity;
}

void FLAG::GroupDelete(ENTITY_ID m_id)
{
    // найдем группу соответствующую полученной модели
    for(int gn=0; gn<groupQuantity; gn++)
        if(gdata[gn].model_id==m_id)
        {
            gdata[gn].bDeleted=true; // пометим ее как удаленную
            bYesDeleted=true;
            break;
        }
}

void FLAG::DoDelete()
{
    // пройтись по всем удаленным группам и удалить содержащиеся в них флаги
    int ngn = 0;
    nVert = 0; nIndx = 0;
    for(int gn=0; gn<groupQuantity; gn++)
    {
        int gs=0; // число неудаленных флагов в группе
        for(int fn=0; fn<flagQuantity; fn++)
            if(flist[fn]!=NULL && flist[fn]->HostGroup==gn)
            {
                // если флаг помечен удаленным, то удалить физически
                if(gdata[gn].bDeleted || flist[fn]->bDeleted || flist[fn]->nv<=0 || flist[fn]->nt<=0)
                {
                    flist[fn]->bDeleted=true;
                    continue;
                }

                flist[fn]->HostGroup = ngn;
                flist[fn]->sv = nVert;
                flist[fn]->st = nIndx;
                nVert += flist[fn]->nv;
                nIndx += flist[fn]->nt*3;
                gs++;
            }
        // если все флаги удалены из группы, то удалить группу
        if( gs==0 ) continue;

        // записать группу на свою позицию
        if(ngn<gn)
            memcpy(&gdata[ngn],&gdata[gn],sizeof(GROUPDATA));
        ngn++;
    }

    int nfn=0;
    for(int fn=0; fn<flagQuantity; fn++)
    {
		if(flist[fn]==NULL) continue;
        if(flist[fn]->bDeleted)
        {
            delete flist[fn];
			flist[fn]=NULL;
            continue;
        }

        flist[nfn++]=flist[fn];
    }

    nIndx/=3;
    if(nfn==0 || ngn==0)
    {
        VERTEX_BUFFER_RELEASE(RenderService,vBuf);
        INDEX_BUFFER_RELEASE(RenderService,iBuf);
        flagQuantity= groupQuantity= 0;
        delete flist; flist=0;
        delete gdata; gdata=0;
    }
    else
        if(nfn!=flagQuantity || ngn!=groupQuantity)
        {
			VERTEX_BUFFER_RELEASE(RenderService,vBuf);
			INDEX_BUFFER_RELEASE(RenderService,iBuf);
            vBuf=RenderService->CreateVertexBufferManaged(FLAGLXVERTEX_FORMAT,nVert*sizeof(FLAGLXVERTEX),D3DUSAGE_WRITEONLY);
            iBuf=RenderService->CreateIndexBufferManaged(nIndx*6);

            flagQuantity=nfn; groupQuantity=ngn;

            FLAGDATA **oldflist = flist;
            flist = NEW FLAGDATA*[flagQuantity];
            if(flist)
            {
                memcpy(flist,oldflist,sizeof(FLAGDATA*)*flagQuantity);
                delete oldflist;
            }
            else
                flist = oldflist;

            GROUPDATA *oldgdata = gdata;
            gdata = NEW GROUPDATA[groupQuantity];
            if(gdata)
            {
                memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
                delete oldgdata;
            }
            else
                gdata = oldgdata;

            SetTreangle();
            SetTextureCoordinate();
        }

    wFlagLast= flagQuantity;
    bYesDeleted= false;
    bUse= flagQuantity>0;
}

void FLAG::SetAdd(int flagNum)
{
	VDATA * pvdat;
    // set vertex and index buffers
    nIndx*=3;
    for(int fn=flagNum; fn<flagQuantity; fn++)
    {
		if(flist[fn]==NULL) continue;
        CVECTOR bmpos,empos;
        float len;

        CVECTOR p0=flist[fn]->spos;
        CVECTOR p1=flist[fn]->dv;
        CVECTOR p2=flist[fn]->dhv;
        CVECTOR p3=flist[fn]->ddhv;
        flist[fn]->spos=bmpos=(p0+p1)*.5f; // середина начального края флага
        // вычислить середину конечного края флага
        if(flist[fn]->triangle)  empos=p2;
        else  empos=(p2+p3)*.5f;
        flist[fn]->dhv=p0-bmpos;
        len=sqrtf(~(empos-bmpos)); // длина флага
        if(len<FLAGVECTORLEN)
        {
            delete flist[fn];
            if(flagQuantity==1)
            {
                delete flist; flist=0; flagQuantity=0;
            }
            else
            {
                FLAGDATA **oldflist=flist;
                flagQuantity--;
                flist = NEW FLAGDATA*[flagQuantity];
                if(flist==0)
                    flist=oldflist;
                if(fn>0)
                    memcpy(flist,oldflist,sizeof(FLAGDATA*)*fn);
                if(fn<flagQuantity)
                    memcpy(&flist[fn],&oldflist[fn+1],sizeof(FLAGDATA*)*(flagQuantity-fn));
                if(flist!=oldflist)
                    delete oldflist;
            }
        }
        else
        {
			long curTexNumC = 0;
			long curTexNumR = 0;
            // установить номер текстуры
			if(flist[fn]->isShip) // корабль
			{
				pvdat = api->Event("GetRiggingData","sllla","GetShipFlagTexNum",flist[fn]->triangle,gdata[flist[fn]->HostGroup].nation, flist[fn]->isSpecialFlag, gdata[flist[fn]->HostGroup].char_attributes);
			}
			else
			{
				pvdat = api->Event("GetRiggingData","slll","GetTownFlagTexNum",flist[fn]->triangle,gdata[flist[fn]->HostGroup].nation, flist[fn]->isSpecialFlag);
			}			
            if(pvdat==null)	
			{
				flist[fn]->texNumC = 0;
				flist[fn]->texNumR = 0;
			}	
			else
			{	
				pvdat->Get(curTexNumC,0);
				pvdat->Get(curTexNumR,1);
				flist[fn]->texNumC = curTexNumC;
				flist[fn]->texNumR = curTexNumR;
			}	
			
            flist[fn]->vectQuant=(int)(len/FLAGVECTORLEN); // число сегментов флага
            if(flist[fn]->vectQuant<MinSegmentQuantity)
                flist[fn]->vectQuant=MinSegmentQuantity;
            // вычислить приращение флага
            flist[fn]->dv=(empos-bmpos)/(float)flist[fn]->vectQuant;
            if(flist[fn]->triangle)
                flist[fn]->ddhv=(p2-p0-empos+bmpos)/(float)(flist[fn]->vectQuant);
            else
                flist[fn]->ddhv=(p2-p0-empos+bmpos)/(float)(flist[fn]->vectQuant+1);

            flist[fn]->sv=nVert;
            flist[fn]->st=nIndx;
            flist[fn]->vectQuant;
            if(flist[fn]->triangle)
            {
                nVert+=(flist[fn]->nv=flist[fn]->vectQuant*2+3);
                nIndx+=(flist[fn]->nt=flist[fn]->vectQuant*2+1)*3;
            }
            else
            {
                nVert+=(flist[fn]->nv=flist[fn]->vectQuant*2+2);
                nIndx+=(flist[fn]->nt=flist[fn]->vectQuant*2)*3;
            }
        }
    }
    // удалим старые буферы
    VERTEX_BUFFER_RELEASE(RenderService,vBuf);
    INDEX_BUFFER_RELEASE(RenderService,iBuf);
}

void FLAG::MoveOtherHost(ENTITY_ID newm_id,long flagNum,ENTITY_ID oldm_id)
{
	int oldgn = 0, newgn = 0;
    // найдем старую группу
    for(oldgn=0; oldgn<groupQuantity; oldgn++)
        if(gdata[oldgn].model_id==oldm_id) break;
    // если нет такой группы, то пустой возврат
    if(oldgn==groupQuantity) return;

    // найдем новую группу
    for(newgn=0; newgn<groupQuantity; newgn++)
        if(gdata[newgn].model_id==newm_id) break;
    // если нет новой группы, то создаем ее
    if(newgn==groupQuantity)
    {
        GROUPDATA *oldgdata=gdata;
        gdata = NEW GROUPDATA[groupQuantity+1];
        if(gdata==0)
            SE_THROW_MSG("Not memory allocation");
        memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
        delete oldgdata;
        groupQuantity++;

        // заполним параметры новой группы
        gdata[newgn].bDeleted = false;
        gdata[newgn].model_id = newm_id;
        gdata[newgn].nation = gdata[oldgn].nation;
		
		if(gdata[oldgn].isShip)
		{
			gdata[newgn].ship_id = gdata[oldgn].ship_id;
			gdata[newgn].char_attributes = gdata[oldgn].char_attributes;
		}	
		
    }

	int fn = 0;
    // найдем нужный нам флаг
    for(fn=0; fn<flagQuantity; fn++)
        if(flist[fn]!=NULL && flist[fn]->grNum==flagNum && flist[fn]->HostGroup==oldgn) break;
    // переназначим его хозяина на нового хозяина
    if(fn<flagQuantity)
        flist[fn]->HostGroup=newgn;
}


