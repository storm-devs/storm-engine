#include "SAIL.h"
#include "..\common_h\model.h"
#include "..\common_h\sail_msg.h"
#include "..\common_h\defines.h"
#include "..\common_h\Weather_base.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\ship_msg.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "rigging_define.h"
#include <stdio.h>
#include "..\common_h\mast_msg.h"

#include "..\\engine\\program\\sea_ai\\script_defines.h"
#include "..\\engine\\program\\battle_interface\\msg_control.h"

#define WIND_SPEED_MAX 12.f

void _cdecl sailPrint(VDX8RENDER *rs, const CVECTOR & pos3D, float rad, long line, const char * format, ...);
int traceSail = -1;
long g_iBallOwnerIdx = -1;

float g_fSailHoleDepend = 1.0f;

float GetSailSpeed(int holeQ,int holeMax,float maxSpeed,float fSailHoleDepend)
{
	if(holeMax<=0)	holeMax = 1;
	if(holeQ<=0)
		return maxSpeed;
    float curSpeed = maxSpeed*(1.f-(float)holeQ/holeMax*fSailHoleDepend);
	if(curSpeed<0.f) curSpeed=0.f;
	return curSpeed;
}

SAIL::SAIL()
{
    // установка первоначальных значений всех общих данных
    // переписываемых затем из INI файла
    // ---------------------------------------------------
	// texture names
    texQuantity= 1;
    texNumCommon= 0;
    texNumEnglish= 0;
    texNumTreangle= 0;

    // speed calculate parameters:
    ts_min=0.2f;    ts_xdep=0.6f;    ts_zdep=0.8f;
    fs_min=0.4f;    fs_xdep=0.5f;    fs_zdep=0.6f;
    ss_min=0.4f;    ss_xdep=0.1f;    ss_zdep=0.6f;

    // wind depend parameters
    SsailWindDepend= .05f;
    TsailWindDepend= .5f;
    fWindAdding= .3f;
    FLEXSPEED= .001f;
    MAXSUMWIND= .02f;
    WINDVECTOR_QUANTITY= 60;
    WINDVECTOR_TINCR= 3;
    WINDVECTOR_TADD= 3;
    WINDVECTOR_SINCR= 6;
    WINDVECTOR_SADD= 3;

    // load rolling sail parameters
    ROLL_Z_VAL= .01f;
    ROLL_Z_DELTA= .001f;
    ROLL_Y_VAL= .04f;
    ROLL_Y_DELTA= .001f;

    // sail turning parameters
    WINDANGL_DISCRETE= .01f;
    MAXTURNANGL= .6f;
	TURNSTEPANGL= .002f;
    ROLLINGSPEED= .0003f;

    // material parameters
    mat.Diffuse.r= 0.f;  mat.Diffuse.g= 0.f;  mat.Diffuse.b= 0.f;  mat.Diffuse.a= 0.f;
    mat.Ambient.r= 0.f;  mat.Ambient.g= 0.f;  mat.Ambient.b= 0.f;  mat.Ambient.a= 0.f;
    mat.Specular.r= 0.f; mat.Specular.g= 0.f; mat.Specular.b= 0.f; mat.Specular.a= 0.f;
    mat.Emissive.r= .7f; mat.Emissive.g= .7f; mat.Emissive.b= .7f; mat.Emissive.a= 0.f;
	mat.Power = 0.f;

    // ROLLING SAIL form table
        // square sail form
    SSailRollForm[0]= 0.2f;    SSailRollForm[1]= 0.8f;    SSailRollForm[2]= 1.0f;
    SSailRollForm[3]= 0.8f;    SSailRollForm[4]= 0.4f;    SSailRollForm[5]= 1.0f;
    SSailRollForm[6]= 1.3f;    SSailRollForm[7]= 1.0f;    SSailRollForm[8]= 0.4f;
    SSailRollForm[9]= 0.8f;    SSailRollForm[10]= 1.0f;    SSailRollForm[11]= 0.8f;
    SSailRollForm[12]= 0.2f;
        // treangle sail form
    TSailRollForm[0]= 0.1f;    TSailRollForm[1]= 0.6f;    TSailRollForm[2]= 0.3f;
    TSailRollForm[3]= 0.8f;    TSailRollForm[4]= 0.2f;
    TR_FORM_MUL= 2.f;

    // hole depend parameters
    fTHoleFlexDepend= .01f;
    fSHoleFlexDepend= .01f;

    // parameters for sails of fall mast
        // square sails
    FALL_SSAIL_ADD_MIN= 0.2f;
    FALL_SSAIL_ADD_RAND= 0.2f;
        // treangle sails
    FALL_TSAIL_ADD_MIN= 0.2f;
    FALL_TSAIL_ADD_RAND= 0.2f;
    //-----------------------------------------------------

	GROUP_UPDATE_TIME = 4000;

    SailQuantity=0;
    SailCurNum=0;

    globalWind.base=.4f;
    globalWind.ang.x=0.f;
    globalWind.ang.y=0.f;
    globalWind.ang.z=1.f;
    WindVect = null;

    texl = -1;
	m_nEmptyGerbTex = -1;

    bFirstRun=true; wFirstIndx=0;
    bDeleteState=false;
    RenderService=0;

    bCannonTrace=false;

    groupQuantity=0; gdata=0;
    sailQuantity=0; slist=0;
    bUse=false;

	m_nMastCreatedCharacter = -1;
	m_sMastName = null;

	sg.indxBuf = -1; sg.vertBuf = -1;

	m_nLastUpdate = 0;

	m_fMinSpeedVal = 0.f;
}

SAIL::~SAIL()
{
    if(slist!=null)
    {
        for(int i=0; i<sailQuantity; i++)
            PTR_DELETE(slist[i]);
        PTR_DELETE(slist);
    }
    if(gdata!=null)
    {
        for(int i=0; i<groupQuantity; i++)
		{
            PTR_DELETE(gdata[i].sailIdx);
		}
        VOIDPTR_DELETE(gdata);
    }

	VERTEX_BUFFER_RELEASE(RenderService,sg.vertBuf);
	INDEX_BUFFER_RELEASE(RenderService,sg.indxBuf);
    TEXTURE_RELEASE(RenderService,texl);
	TEXTURE_RELEASE(RenderService,m_nEmptyGerbTex);
    PTR_DELETE(WindVect);
	m_nMastCreatedCharacter = -1;
	PTR_DELETE(m_sMastName);
}

bool SAIL::Init()
{
	GUARD(SAIL::SAIL())

    SetDevice();

    UNGUARD
	return true;
}

void SAIL::SetDevice()
{
    int i;

    // матрица будет единичной
    mtx.SetIdentity();

    // получить сервис рендера
	RenderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!RenderService)
	{
		_THROW("No service: dx8render");
	}

    LoadSailIni();

    if(WindVect==null)
    {
        WindVect=NEW float[WINDVECTOR_QUANTITY];
        if(WindVect)
            // расчет таблицы векторов ветра
            for( i=0; i<WINDVECTOR_QUANTITY; i++ )
                WindVect[i]=sinf((float)i/(float)(WINDVECTOR_QUANTITY)*2.f*PI);
        else
			{_THROW("No memory allocation: WindVect");}
    }
}

bool SAIL::CreateState(ENTITY_STATE_GEN * state_gen)
{
    return true;
}

bool SAIL::LoadState(ENTITY_STATE * state)
{
	return true;
}

void SAIL::Execute(dword Delta_Time)
{
    DWORD rtime;
    int i;

	// тестовая убойка мачт
	if( gdata &&
		api->Controls->GetDebugAsyncKeyState(VK_MENU)<0 &&
		api->Controls->GetDebugAsyncKeyState(VK_CONTROL)<0 &&
		api->Controls->GetDebugAsyncKeyState(VK_SHIFT)<0 )
	{
		long nTmpMastNum = -1;
		if( api->Controls->GetDebugAsyncKeyState('1')<0 ) nTmpMastNum = 1;
		else if( api->Controls->GetDebugAsyncKeyState('2')<0 ) nTmpMastNum = 2;
		else if( api->Controls->GetDebugAsyncKeyState('3')<0 ) nTmpMastNum = 3;
		else if( api->Controls->GetDebugAsyncKeyState('4')<0 ) nTmpMastNum = 4;
		else if( api->Controls->GetDebugAsyncKeyState('5')<0 ) nTmpMastNum = 5;
		else if( api->Controls->GetDebugAsyncKeyState('6')<0 ) nTmpMastNum = 6;
		else if( api->Controls->GetDebugAsyncKeyState('7')<0 ) nTmpMastNum = 7;
		else if( api->Controls->GetDebugAsyncKeyState('0')<0 ) nTmpMastNum = 0;
		if( nTmpMastNum >= 0 )
		{
			MODEL * pTmpMdl = 0;
			if( (pTmpMdl=(MODEL*)api->GetEntityPointer(&gdata[0].modelEI)) !=0 )
			{
				char pcTmpMastName[256];
				sprintf(pcTmpMastName,"mast%d",nTmpMastNum);
				NODE* nod = pTmpMdl->FindNode(pcTmpMastName);
				if(nod)
				{
					ENTITY_ID eiMastTmp;
					if( api->CreateEntity( &eiMastTmp, "MAST" ) )
					{
						api->Send_Message( eiMastTmp, "lpii", MSG_MAST_SETGEOMETRY, nod, gdata[0].shipEI, gdata[0].modelEI );
					}
				}
			}
		}
	}

    if(bFirstRun)
    {
        FirstRun();
        wFirstIndx=sailQuantity;
    }
    if(bDeleteState)
    {
        DeleteSailGroup();
        wFirstIndx=sailQuantity;
    }

	float fMaxTurnAngl = Delta_Time * TURNSTEPANGL;

	bool bSailUpdate = false;
	m_nLastUpdate -= Delta_Time;
	if(m_nLastUpdate<=0)
	{
		m_nLastUpdate = GROUP_UPDATE_TIME;
		bSailUpdate = true;
	}

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
                int oldWindQnt=WINDVECTOR_QUANTITY;
                LoadSailIni();
                if(oldWindQnt!=WINDVECTOR_QUANTITY) // если изменили размер таблицы ветров
                {
                    PTR_DELETE(WindVect);
                    WindVect=NEW float[WINDVECTOR_QUANTITY];
                    if(WindVect) // расчет таблицы векторов ветра
                        for(i=0; i<WINDVECTOR_QUANTITY; i++ )
                            WindVect[i]=sinf((float)i/(float)(WINDVECTOR_QUANTITY)*2.f*PI);
                    else
                    {
                        _THROW("No memory allocation: WindVect");
                    }
                }
                for(i=0; i<sailQuantity; i++)
                {
                    slist[i]->MaxSumWind=slist[i]->sailHeight*MAXSUMWIND;
                    // для нового размера таблици векторов поправим текущие индексы
                    while(slist[i]->VertIdx>=WINDVECTOR_QUANTITY)
                        slist[i]->VertIdx-=WINDVECTOR_QUANTITY;
                    while(slist[i]->HorzIdx>=WINDVECTOR_QUANTITY)
                        slist[i]->HorzIdx-=WINDVECTOR_QUANTITY;
                }
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
            globalWind.base=wb->GetFloat(whf_wind_speed)/WIND_SPEED_MAX;
			if(globalWind.base>1.f) globalWind.base = 1.f;
        }

        // установить бокс нулевого размера в центр групп и параметры расчета ветра
        for(i=0; i<groupQuantity; i++)
        {
			if(gdata[i].bDeleted) continue;
            MODEL* cmod;
            cmod=(MODEL*)_CORE_API->GetEntityPointer(&gdata[i].modelEI);
            if(cmod==0) continue;
            gdata[i].boxCenter = gdata[i].boxSize = slist[gdata[i].sailIdx[0]]->ss.boundSphere.rc;//CVECTOR(0.f,0.f,0.f);
            gdata[i].speed_c=0.f;
            gdata[i].maxSpeed=0.f;
			gdata[i].curHole=0;
			gdata[i].bFinalSailDoOld = gdata[i].bFinalSailDo;
			gdata[i].bFinalSailDo = false;
			VDATA * pvdat = api->Event("evntGetSRollSpeed", "l", GetCharacterForGroup(i));
			if(pvdat==null)	gdata[i].fRollingSpeed = ROLLINGSPEED;
			else gdata[i].fRollingSpeed = pvdat->GetFloat() * ROLLINGSPEED;
        }

        CVECTOR pos,ang;
        float perspect;
        _asm rdtsc _asm mov rtime,eax
        RenderService->GetCamera(pos,ang,perspect);
        CMatrix tmpMtx; tmpMtx.BuildMatrix(ang);
        CVECTOR vCamDirect=tmpMtx*CVECTOR(0,0,1.f);
        float minCos=cosf(atanf(perspect));

        for(i=0; i<sailQuantity; i++)
        {
			if(gdata[slist[i]->HostNum].bDeleted) continue;
            // сворачивать.разворачивать парус по нужде
            if( slist[i]->sroll && !slist[i]->bFreeSail)
                slist[i]->DoRollingStep(Delta_Time);
			if( slist[i]->sroll && !slist[i]->bFreeSail )
				gdata[slist[i]->HostNum].bFinalSailDo = true;
            // Если поменялась сетка на парусе, то установить новые индексы
            slist[i]->GetGrid(pos,perspect);
            // расчет ветра
            slist[i]->CalculateSailWind();
            // повернуть парус по нужде
            if( slist[i]->ss.turningSail )
                slist[i]->TurnSail(fMaxTurnAngl);
            // расчет влияния состояния парусов на скорость корабля (от 0 до 1)
            gdata[slist[i]->HostNum].speed_c+=slist[i]->curSpeed;
			gdata[slist[i]->HostNum].curHole+=slist[i]->GetMaxHoleCount() - slist[i]->ss.holeCount;
        }

        SAILVERTEX* pv=(SAILVERTEX*)RenderService->LockVertexBuffer(sg.vertBuf);
        if( pv )
        {
            for(i=0; i<sailQuantity; i++)
            {
				if(gdata[slist[i]->HostNum].bDeleted) continue;
                // заколебать парусь
                slist[i]->goWave(&pv[slist[i]->ss.sVert],Delta_Time);
                // установить сворачивание\разворачивание паруса
//                if( gdata[slist[i]->HostNum].bFinalSailDo )
//                    if(!slist[i]->ss.rollingSail)
//                        slist[i]->SetRolling(gdata[slist[i]->HostNum].bFinalSailUp);
                // расчет бокса ограничивающего паруса
                CVECTOR vtmp = slist[i]->ss.boundSphere.rc - slist[i]->ss.boundSphere.r;
                int itmp = slist[i]->HostNum;
                if( gdata[itmp].boxCenter.x > vtmp.x ) gdata[itmp].boxCenter.x = vtmp.x;
                if( gdata[itmp].boxCenter.y > vtmp.y ) gdata[itmp].boxCenter.y = vtmp.y;
                if( gdata[itmp].boxCenter.z > vtmp.z ) gdata[itmp].boxCenter.z = vtmp.z;

                vtmp = slist[i]->ss.boundSphere.rc + slist[i]->ss.boundSphere.r;
                if( gdata[itmp].boxSize.x < vtmp.x ) gdata[itmp].boxSize.x = vtmp.x;
                if( gdata[itmp].boxSize.y < vtmp.y ) gdata[itmp].boxSize.y = vtmp.y;
                if( gdata[itmp].boxSize.z < vtmp.z ) gdata[itmp].boxSize.z = vtmp.z;
            }

            RenderService->UnLockVertexBuffer(sg.vertBuf);
        }

        // сброс признака спуска\подъема для всех кораблей и расчет скорости
        for(i=0; i<groupQuantity; i++)
        {
			if(gdata[i].bDeleted) continue;

			// звук подъема спуска паруса
			if( gdata[i].bFinalSailDoOld != gdata[i].bFinalSailDo ) {
				if( gdata[i].bYesShip ) {
					ATTRIBUTES* pA = ((VAI_OBJBASE*)gdata[i].shipEI.pointer)->GetACharacter();
					api->Event( "Ship_SailsMoveSound", "al", pA, (long)gdata[i].bFinalSailDo );
				}
			}

			float fSP = (float)gdata[i].maxSP;
            if(gdata[i].speed_m<0.001f)
            {
                gdata[i].shipSpeed=0.f;
                gdata[i].maxSpeed=0.f;
            }
            else
            {
                gdata[i].shipSpeed=gdata[i].speed_c/gdata[i].speed_m;
                gdata[i].maxSpeed /= gdata[i].speed_m;

				if( gdata[i].maxHole != 0 )
					fSP = (float)gdata[i].maxSP * gdata[i].curHole / gdata[i].maxHole;
				if(fSP > gdata[i].maxSP) fSP = (float)gdata[i].maxSP;
            }
            gdata[i].boxCenter = (gdata[i].boxCenter + gdata[i].boxSize)*.5f;
            gdata[i].boxSize -= gdata[i].boxCenter;
            gdata[i].boxRadius=sqrtf(~gdata[i].boxSize);

			if(gdata[i].bYesShip)
			{
				VAI_OBJBASE * pVai = (VAI_OBJBASE *)_CORE_API->GetEntityPointer(&gdata[i].shipEI);
				if(pVai!=null && pVai->GetACharacter()!=null)
				{
					ATTRIBUTES * pA = pVai->GetACharacter()->GetAttributeClass("Ship");
					if(pA!=null)
						pA->SetAttributeUseDword("SP",(long)fSP);
				}
			}
        }
        // инициализация параметров трассировки луча
        LastTraceGroup=0;
        _asm rdtsc  _asm sub eax,rtime _asm mov rtime,eax
        tm.idx=rtime;
    }
}

void SAIL::Realize(dword Delta_Time)
{
	dword dwOldTextureFactor;
	int i,j,idx;
    if(bUse)
    {
        DWORD tm_draw;

        _asm rdtsc	_asm mov tm_draw,eax
		bool bDraw = RenderService->TechniqueExecuteStart("ShipSail");
		if (!bDraw) return;
        RenderService->SetMaterial(mat);
		RenderService->TextureSet(2,texl);
		CMatrix matv,matp,matc;
		RenderService->GetTransform(D3DTS_VIEW,(D3DXMATRIX*)&matv);
		RenderService->GetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&matp);
		matc = matv*matp;
        if( false )//Delta_Time==0 )
        {
			for(j=0; j<groupQuantity; j++)
			{
				if(gdata[j].bDeleted) continue;
				RenderService->GetRenderState(D3DRS_TEXTUREFACTOR,&dwOldTextureFactor);
				RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,gdata[j].dwSailsColor);
				for(idx=0; idx<gdata[j].sailQuantity; idx++)
				{
					i = gdata[j].sailIdx[idx];
					if(slist[i]->bFreeSail) continue;
					//if(gdata[slist[i]->HostNum].bDeleted) continue;
					RenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)slist[i]->pMatWorld);
					RenderService->TextureSet( 0, slist[i]->surfaceTex );
					if( slist[i]->m_bIsGerald ) {
						if( slist[i]->m_pGeraldTex ) RenderService->SetTexture( 1, slist[i]->m_pGeraldTex );
						else RenderService->TextureSet( 1, slist[i]->m_nGeraldTex );
					} else RenderService->TextureSet( 1, m_nEmptyGerbTex );
					// Draw hole sail
					if(slist[i]->ss.Nh!=0) {
						RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf,
										slist[i]->ss.sVert, slist[i]->ss.nVert, slist[i]->ss.shi, slist[i]->ss.Nh);
					}
					// Draw normal sail
					if(slist[i]->ss.Nn!=0) {
						RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf,
										slist[i]->ss.sVert, slist[i]->ss.nVert, slist[i]->ss.sni, slist[i]->ss.Nn);
					}
				}
				RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,dwOldTextureFactor);
			}
        }
        else
        {
			for(j=0; j<groupQuantity; j++)
			{
				if(gdata[j].bDeleted) continue;
				RenderService->GetRenderState(D3DRS_TEXTUREFACTOR,&dwOldTextureFactor);
				RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,gdata[j].dwSailsColor);
				for(idx=0; idx<gdata[j].sailQuantity; idx++)
				{
					i = gdata[j].sailIdx[idx];
					//if(gdata[slist[i]->HostNum].bDeleted) continue;
					RenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)slist[i]->pMatWorld);
					RenderService->TextureSet(0,slist[i]->surfaceTex);
					if( slist[i]->m_bIsGerald ) {
						if( slist[i]->m_pGeraldTex ) RenderService->SetTexture( 1, slist[i]->m_pGeraldTex );
						else RenderService->TextureSet( 1, slist[i]->m_nGeraldTex );
					} else RenderService->TextureSet( 1, m_nEmptyGerbTex );
					// Draw hole texture sail
					dword dwOld;
					RenderService->GetTextureStageState(2,D3DTSS_ADDRESSU,&dwOld);
					RenderService->SetTextureStageState(2,D3DTSS_ADDRESSU,D3DTADDRESS_MIRROR);
					//slist[i]->FillIndex(pt);
					#ifndef _XBOX
					WORD* pt=(WORD*)RenderService->LockIndexBuffer(sg.indxBuf,D3DLOCK_DISCARD);
					#else
					WORD* pt=(WORD*)RenderService->LockIndexBuffer(sg.indxBuf,0);
					#endif
					if(pt)	slist[i]->FillIndex(pt);
					RenderService->UnLockIndexBuffer(sg.indxBuf);
					if( gdata[j].bYesShip )
					{
						((SHIP_BASE*)gdata[j].shipEI.pointer)->SetLightAndFog(true);
						((SHIP_BASE*)gdata[j].shipEI.pointer)->SetLights();
					}
					if(slist[i]->ss.nholeIndx!=0) {
						RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf,
										slist[i]->ss.sVert, slist[i]->ss.nVert, slist[i]->ss.sholeIndx, slist[i]->ss.nholeIndx);
					}
					// Draw normal texture sail
					RenderService->SetTextureStageState(2,D3DTSS_ADDRESSU,dwOld);
					if(slist[i]->ss.nnormIndx!=0) {
						RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf,
										slist[i]->ss.sVert, slist[i]->ss.nVert, slist[i]->ss.sIndx, slist[i]->ss.nnormIndx);
					}
					if( gdata[j].bYesShip )
					{
						((SHIP_BASE*)gdata[j].shipEI.pointer)->UnSetLights();
						((SHIP_BASE*)gdata[j].shipEI.pointer)->RestoreLightAndFog();
					}
				}
				RenderService->SetRenderState(D3DRS_TEXTUREFACTOR,dwOldTextureFactor);
			}

       	    _asm rdtsc	_asm sub eax,tm_draw _asm mov tm_draw,eax
        }
		while (RenderService->TechniqueExecuteNext()) {};
    }
}

dword _cdecl SAIL::ProcessMessage(MESSAGE & message)
{
    GUARD(dword _cdecl SAIL::ProcessMessage(MESSAGE message))
    ENTITY_ID tmpEI;
    int i;

	switch (message.Long())
    {
    case MSG_SAIL_INIT: // format "liil" (code,shipEntity,modelEntity,bSailUp)
		{
			// resize group data block
			if(gdata)
			{
				GROUPDATA* oldgdata = gdata;
				gdata = NEW GROUPDATA[groupQuantity+1];
				memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
				VOIDPTR_DELETE(oldgdata);
				groupQuantity++;
			}
			else
			{
				gdata = NEW GROUPDATA[1];
				groupQuantity = 1; sailQuantity=0; slist=0;
			}
			// Set new data into buffer
			gdata[groupQuantity-1].bDeleted = false;
			gdata[groupQuantity-1].bYesShip = true;
			gdata[groupQuantity-1].shipEI = message.EntityID();
			gdata[groupQuantity-1].modelEI = message.EntityID();
			gdata[groupQuantity-1].shipSpeed = 0.f;
			gdata[groupQuantity-1].fSpeedMul = 1.f;
			gdata[groupQuantity-1].speed_m = 0;
			gdata[groupQuantity-1].speed_c = 0;
			gdata[groupQuantity-1].fRollingSpeed = ROLLINGSPEED;
			gdata[groupQuantity-1].dwSailsColor = 0xFFFFFFFF;
			gdata[groupQuantity-1].maxSP = 100;
			bool bSailUp = (message.Long()!=0);
			if(bSailUp)
			{
				gdata[groupQuantity-1].bFinalSailDoOld = gdata[groupQuantity-1].bFinalSailDo = false;
				gdata[groupQuantity-1].bFinalSailUp = false;
				gdata[groupQuantity-1].curSailSet = ROLLTYPE_FAST;
			}
			else
			{
				gdata[groupQuantity-1].bFinalSailDoOld = gdata[groupQuantity-1].bFinalSailDo = false;
				gdata[groupQuantity-1].bFinalSailUp = false;
				gdata[groupQuantity-1].curSailSet = ROLLTYPE_NONE;
			}

			MODEL *mdl;
			if((mdl=(MODEL*)_CORE_API->GetEntityPointer(&gdata[groupQuantity-1].modelEI))!=0)
			{
				GEOS::INFO gi;   GEOS::LABEL gl;
				for(int j=0; true; j++)
				{
					NODE* nod = mdl->GetNode(j);
					if(nod==null || nod->geo==null) break;

					nod->geo->GetInfo(gi);
					for(i=0; i<gi.nlabels; i++)
					{
						nod->geo->GetLabel(i,gl);

						if(!strncmp(gl.name,"sail",4))
							AddSailLabel(gl,nod,bSailUp);
					}
				}
			}
			else
			{
				api->Trace("WARNING! Can`t model class pointer for ShipModel");
			}
			// Set all getting sails
			SetAllSails(groupQuantity-1);
			bFirstRun=true;
		}
    break;

    // Do rope tie
    case MSG_SAIL_ROPE_TIE: // format "lplpl" (code,&NODE,groupNum,&position,ropenum)
        int iMax,posNum;
        float posDelta,curDelta;
        NODE* nod;
        nod=(NODE*)message.Pointer();
        int gNum; gNum=message.Long();
        CVECTOR* pos; pos=(CVECTOR*)message.Pointer();
        if(pos==0) break;
        int ropenum; ropenum=message.Long();

        for(i=0; i<sailQuantity; i++)
        {
            if(slist[i]->hostNode==nod && slist[i]->groupNum==gNum)
            {
                SAILONE* so=slist[i];
                if(so->ss.eSailType==SAIL_TREANGLE) iMax=3;
                else iMax=4;
                posDelta=1000.f;
                for(int j=posNum=0; j<iMax; j++)
                {
                    curDelta=~(so->ss.hardPoints[j]-*pos);
                    if(curDelta<posDelta)
                    {
                        posDelta=curDelta;
                        posNum=j;
                    }
                }
                so->sailtrope.pnttie[posNum]=true;
                so->sailtrope.pPos[posNum]=pos;

                CVECTOR epos;
                if(so->ss.turningSail && posNum!=0) // установка только для поворачивающихся парусов
                if(_CORE_API->FindClass(&tmpEI,"rope",0))
                if(so->sailtrope.rrs[0]==0)
                {
                    so->sailtrope.rrs[0]=NEW ROTATEROPEDSAIL;
                    so->sailtrope.rrs[0]->ropenum=ropenum;
                    so->sailtrope.rrs[0]->tiePoint=posNum;
                    CVECTOR bpos;
                    switch(posNum)
                    {
                    case 1:
                        bpos=so->ss.hardPoints[0];
                        break;
                    case 2:
                        bpos=so->ss.hardPoints[0];
                        break;
                    case 3:
                        bpos=so->ss.hardPoints[1];
                        break;
                    }
                    ((ROPE_BASE*)_CORE_API->GetEntityPointer(&tmpEI))->GetEndPoint(&epos,so->sailtrope.rrs[0]->ropenum,gdata[so->HostNum].modelEI);
                    so->sailtrope.rrs[0]->r1=sqrtf(~(*pos - bpos));
                    so->sailtrope.rrs[0]->r2=sqrtf(~(*pos - epos));
                    if(so->ss.eSailType!=SAIL_TREANGLE)
                    {
                        CVECTOR tmpv;
                        tmpv=so->sgeo.dVv + so->sgeo.ddVv*(float)SAIL_ROW_MAX;
                        tmpv=so->sgeo.dVv + so->sgeo.ddVv*(float)SAIL_ROW_MAX;
                        tmpv=(so->sailtrope.rrs[0]->r1)*(!tmpv);
                        tmpv=*pos-tmpv;

                        so->sailtrope.rrs[0]->b=tmpv;
                    }
                }
                else if(so->sailtrope.rrs[1]==0)
                {
                    so->sailtrope.rrs[1]=NEW ROTATEROPEDSAIL;
                    so->sailtrope.rrs[1]->ropenum=ropenum;
                    so->sailtrope.rrs[1]->tiePoint=posNum;
                    CVECTOR bpos;
                    switch(posNum)
                    {
                    case 1:
                        bpos=so->ss.hardPoints[0];
                        break;
                    case 2:
                        bpos=so->ss.hardPoints[0];
                        break;
                    case 3:
                        bpos=so->ss.hardPoints[1];
                        break;
                    }
                    so->sailtrope.rrs[1]->r1=sqrtf(~(*pos - bpos));
                    ((ROPE_BASE*)_CORE_API->GetEntityPointer(&tmpEI))->GetEndPoint(&epos,so->sailtrope.rrs[1]->ropenum,gdata[so->HostNum].modelEI);
                    so->sailtrope.rrs[1]->r2=sqrtf(~(*pos - epos));
                    if(so->ss.eSailType!=SAIL_TREANGLE)
                    {
                        CVECTOR tmpv;
                        tmpv=so->sgeo.dVv + so->sgeo.ddVv*(float)SAIL_ROW_MAX;
                        tmpv=(so->sailtrope.rrs[1]->r1)*(!tmpv);
                        tmpv=*pos-tmpv;

                        so->sailtrope.rrs[1]->b=tmpv;
                    }
                }
            }
        }
    break;

    // получить от парусов скорость перемещения корабля
    case MSG_SAIL_GET_SPEED:
        tmpEI=message.EntityID();
        float *speedPtr; speedPtr=(float*)message.Pointer();
        float maxVal; maxVal=message.Float();
        if(speedPtr)
        {
            for(i=0; i<groupQuantity; i++)
                if(tmpEI==gdata[i].shipEI)
                {
					if(gdata[i].bDeleted)
					{
						*speedPtr = 0;
						break;
					}
					if( gdata[i].curSailSet != ROLLTYPE_NONE &&
						gdata[i].shipSpeed < m_fMinSpeedVal )
							*speedPtr = m_fMinSpeedVal;
					else	*speedPtr = gdata[i].shipSpeed;

                    bool yesChange=false;
                    int rt=ROLLTYPE_FAST;
                    if(maxVal<0.3f)
                    {
                        if(gdata[i].curSailSet!=ROLLTYPE_NONE)
                        {
                            yesChange=true;
                            rt=ROLLTYPE_NONE;
                        }
                    }
                    else if(maxVal>0.7f)
                    {
                        if(gdata[i].curSailSet!=ROLLTYPE_FAST)
                        {
                            yesChange=true;
                        }
                    }
                    else
                    {
                        if(gdata[i].curSailSet!=ROLLTYPE_NORMAL)
                        {
                            yesChange=true;
                            rt=ROLLTYPE_NORMAL;
                        }
                    }

                    // установим выбранный тип положения парусов
                    if(yesChange)
                    {
                        float tmpf=0.f;
                        for(int j=0; j<gdata[i].sailQuantity; j++)
                        {
                            int snum=gdata[i].sailIdx[j];
                            if(slist[snum]->rollType>rt)
							{
                                slist[snum]->SetRolling(true);
                                tmpf+=slist[snum]->maxSpeed;
							}
                            else
                            {
                                slist[snum]->SetRolling(false);
                            }
                        }
                        if(tmpf>0.f) gdata[i].fSpeedMul = maxVal/(tmpf/gdata[i].speed_m);
                        gdata[i].curSailSet=rt;
                    }

                    break;
                }

            if(i==groupQuantity)
			{
                *speedPtr = (maxVal>0.3f) ? m_fMinSpeedVal : 0.f; // нет корабля в парусных группах - скорость 0
			}
        }
    break;

    case MSG_SAIL_GET_PARAMS: // "lip"
        {
            ENTITY_ID shipEI = message.EntityID();
            float *pMaxSpeed = (float*)message.Pointer();
            // найдем нужную группу парусов
            for(int gn=0; gn<groupQuantity; gn++)
                if(gdata[gn].shipEI==shipEI) break;
            // запишем по указателю параметра значение для него
            if(pMaxSpeed)
                if(gn<groupQuantity)
                    *pMaxSpeed = gdata[gn].maxSpeed;
                else *pMaxSpeed = 0.f;
        }
    break;

    // перебросить парус в другую группу
    case MSG_SAIL_TO_NEWHOST:
		{
			ENTITY_ID oldModelEI = message.EntityID();
			NODE * nod = (NODE*)message.Pointer();
			long groupNum = message.Long();
			ENTITY_ID newHostEI = message.EntityID();
			ENTITY_ID newModelEI = message.EntityID();
			DoSailToNewHost(newModelEI, newHostEI, groupNum,
							nod, oldModelEI);
		}
    break;
    case MSG_SAIL_CHECK:
		{
			ENTITY_ID oldHostEI = message.EntityID();
			ENTITY_ID newHostEI = message.EntityID();
			ENTITY_ID newModelEI = message.EntityID();
			DoNoRopeSailToNewHost(newModelEI, newHostEI, oldHostEI);
		}
    break;

    // удалить группу парусов
    case MSG_SAIL_DEL_GROUP:
        tmpEI=message.EntityID(); // получим ENTITY хозяина группы
        for(i=0; i<groupQuantity; i++)
            if(tmpEI==gdata[i].shipEI)
            {
                gdata[i].bDeleted=true;
                bDeleteState=true;
                break;
            }
    break;

    // освоботить от стандартного надувания группу парусов
    case MSG_SAIL_FREE_GROUP:
        tmpEI=message.EntityID(); // ENTITY хозяина группы
        for(i=0; i<groupQuantity; i++)
            if(tmpEI==gdata[i].shipEI)
            {
				if(gdata[i].bDeleted) break;
                for(int j=0; j<gdata[i].sailQuantity; j++)
                {
                    int sn = gdata[i].sailIdx[j];
                    slist[sn]->bFreeSail=true;
                }
                break;
            }
    break;

	// начать/закончить присоединение парусов к падающей мачте
	case MSG_SAIL_MAST_PROCESSING:
		PTR_DELETE(m_sMastName);
		m_nMastCreatedCharacter = message.Long();
		if(m_nMastCreatedCharacter!=-1)
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			param[sizeof(param)-1] = 0;
			int slen = strlen(param);
			if(slen>0)
				if( (m_sMastName=NEW char[slen+1]) == null )
					{_THROW("allocate memory error");}
				else strcpy(m_sMastName,param);
		}
	break;

	// скриптовое сообщение
	case MSG_SAIL_SCRIPT_PROCESSING:
		{
			char param[256];
			param[0] = 0;
			message.String(sizeof(param),param);
			return ScriptProcessing(param,message);
		}
	break;
    }

    UNGUARD
    return 0;
}

void SAIL::AddSailLabel(GEOS::LABEL &lbl, NODE *nod, bool bSailUp)
{
    SAILONE* cs;
    int grNum, i;
    grNum=atoi(&lbl.group_name[5]);
    for(i=0; i<sailQuantity; i++)
        if(slist[i]->hostNode==nod && slist[i]->groupNum==grNum)
        {
            cs=slist[i];
            break;
        }

    if(i==sailQuantity) // нет такого паруса - надо создавать новый
    {
        // create new sail data and fill that
        if(slist)
        {
            SAILONE** oldslist=slist;
            slist = NEW SAILONE*[sailQuantity+1];
            memcpy(slist,oldslist,sizeof(SAILONE*)*sailQuantity);
            delete oldslist;
        }
        else
            slist = NEW SAILONE*[1];
        cs=slist[sailQuantity]=NEW SAILONE;
        sailQuantity++;
        cs->pp=this;
        cs->HostNum = groupQuantity-1;
        cs->hostNode = nod; cs->groupNum = grNum; // write identify info
        cs->pMatWorld = &nod->glob_mtx;
        cs->curSpeed=0;
        cs->sailWind=CVECTOR(0.f,0.f,0.f);
		cs->bRolling=!bSailUp;

        cs->mastNum=0;
        cs->rollType = ROLLTYPE_NORMAL;
        // получим номер мачты к которой привязан парус (0-нет мачты)
        if( !strncmp(nod->GetName(),"rey",3) )
        {
            if(nod->parent)
                if( !strncmp(nod->parent->GetName(),"mast",4) )
                    cs->mastNum=atoi(&nod->parent->GetName()[4]);
            if( nod->GetName()[4]=='a' || cs->mastNum==1 )
                cs->rollType = ROLLTYPE_FAST;
        }

        switch( lbl.group_name[4] )
        {
        case 't': // triangle sail
            cs->ss.eSailType=SAIL_TREANGLE;
            cs->ss.texNum=1;
            cs->ss.rollingSail=false;
            break;
        case 's': // square sail
            cs->ss.eSailType=SAIL_SQUARE;
            cs->ss.texNum=2;
            cs->ss.rollingSail=true;
            break;
        case 'g': // square sail with gerb picture
            cs->ss.eSailType=SAIL_SQUARE;
            cs->ss.texNum=0;
            cs->ss.rollingSail=true;
            break;
        case 'f': // trapecidal sail
            cs->ss.eSailType=SAIL_TRAPECIDAL;
            cs->ss.texNum=2;
            cs->ss.rollingSail=false;
            break;
        case 'd': // rolling square sail
            cs->ss.eSailType=SAIL_SQUARE;
            cs->ss.rollingSail=true;
            cs->ss.texNum=2;
            break;
        }

        cs->ss.turningSail = !strncmp(nod->GetName(),"rey_",4);
    }

    CVECTOR vtmp;
    vtmp.x=lbl.m[3][0];//+nod->glob_mtx.matrix[3];
    vtmp.y=lbl.m[3][1];//+nod->glob_mtx.matrix[7];
    vtmp.z=lbl.m[3][2];//+nod->glob_mtx.matrix[11];
    switch( lbl.name[4] )
    {
    case '1':
        cs->ss.hardPoints[0]=vtmp;
        break;
    case '2':
        cs->ss.hardPoints[1]=vtmp;
        break;
    case '3':
        if(cs->ss.eSailType==SAIL_TREANGLE)
            cs->ss.hardPoints[2]=vtmp;
        else
            cs->ss.hardPoints[3]=vtmp;
        break;
    case '4':
        cs->ss.hardPoints[2]=vtmp;
		break;
	case '0':
		cs->ss.bYesLimitPoint = true;
		cs->ss.LimitPoint = vtmp;
    }
}

void SAIL::SetAllSails(int groupNum)
{
    gdata[groupNum].bDeleted=false;
    gdata[groupNum].sailQuantity=0;
	gdata[groupNum].maxHole = 0;

    for(int i=0; i<sailQuantity; i++)
        if(slist[i]->HostNum==groupNum)
        {
            // Set sail
            slist[i]->RenderService=RenderService;
            if(slist[i]->SetSail())
            {
                gdata[groupNum].sailQuantity++;
                slist[i]->SetGeometry();
				gdata[groupNum].speed_m += slist[i]->maxSpeed;
				gdata[groupNum].maxHole += (int)slist[i]->GetMaxHoleCount();
            }
            else
            {
                //_THROW("SAIL: Null size");
				api->Trace("SAIL: Can`t init sail");
                PTR_DELETE(slist[i]);
                sailQuantity--;
                if(sailQuantity>0)
                {
                    SAILONE** oldslist=slist;
                    slist=NEW SAILONE*[sailQuantity];
                    if(!slist) {slist=oldslist; oldslist=0;}
                    if(i>0) memcpy(slist,oldslist,sizeof(SAILONE*)*i);
                    if(i<sailQuantity) memcpy(&slist[i],&oldslist[i+1],sizeof(SAILONE*)*(sailQuantity-i));
                    PTR_DELETE(oldslist);
                }
                else	{PTR_DELETE(slist);}
				i--;
            }
        }

    if(gdata[groupNum].sailQuantity>0)
    {
        gdata[groupNum].sailIdx = NEW int[gdata[groupNum].sailQuantity];
        int idx=0;
        for(i=0; i<sailQuantity; i++)
            if(slist[i]->HostNum==groupNum)
            {
                gdata[groupNum].sailIdx[idx++]=i;
            }
		// Посмотрим наличие дырочек
		if(gdata[groupNum].bYesShip && !gdata[groupNum].bDeleted)
		{
			ATTRIBUTES * pACh = ((VAI_OBJBASE*)gdata[groupNum].shipEI.pointer)->GetACharacter();
			ATTRIBUTES * pA = null;
			// Запустим установку текстур на паруса
			SetSailTextures( groupNum, api->Event("GetSailTextureData","l",pACh->GetAttributeAsDword("index",-1)) );
			if(pACh!=null)
			{
				pA = pACh->FindAClass(pA,"ship.sails");
				if(pA==null) pA = pACh->CreateSubAClass(pACh,"ship.sails");
			}
			if(pA!=null)
			{
				char param[256];
				sprintf(param,"%d",gdata[groupNum].maxHole);
				pA->SetValue(param);
				for(i=0;i<(int)pA->GetAttributesNum();i++)
				{
					ATTRIBUTES * pAttr = pA->GetAttributeClass(i);
					if(pAttr!=null) for(int j=0; j<(int)pAttr->GetAttributesNum(); j++)
					{
						ATTRIBUTES *pASail = pAttr->GetAttributeClass(j);
						if(pASail!=null)
						{
							SAILONE * ps = FindSailFromData(groupNum,pAttr->GetThisName(),pASail->GetThisName());
							if(ps!=null)
							{
								ps->SetAllHole(pASail->GetAttributeAsDword("hd"));
								/*if( (ps->ss.eSailType==SAIL_TREANGLE?10:12) == (int)ps->ss.holeCount )
								{ // весь парус подпорчен - нафиг он нужен
									ps->bDeleted = true;
									bDeleteState = true;
								}*/
							}
						}
					}
				}
			}
		}
    }
    else
    {
        groupQuantity--;
        if(groupQuantity>0)
        {
			GROUPDATA* oldgdata=gdata;
            gdata=NEW GROUPDATA[groupQuantity];
            if(gdata)
            {
                memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
                VOIDPTR_DELETE(oldgdata);
            }
            else gdata=oldgdata;
        }
        else
        {
            VOIDPTR_DELETE(gdata);
        }
    }
}

void SAIL::SetAllSails()
{
    sg.nVert = 0; sg.nIndx = 54;

    for(int i=0; i<sailQuantity; i++)
    {
        // сформировать параметры буферов вертекса и индекса
		slist[i]->SetTurnLimits();
        slist[i]->ss.sVert = sg.nVert;
        slist[i]->ss.sIndx = sg.nIndx;
        sg.nVert += slist[i]->ss.nVert;
    }
	if( sg.nVert == 0 ) return;
	sg.nIndx += 1152;

    if( texl == -1 ) texl = RenderService->TextureCreate("ships\\parus_hole.tga");
	if( m_nEmptyGerbTex == -1 ) m_nEmptyGerbTex = RenderService->TextureCreate("ships\\emptygerald.tga");

    sg.vertBuf = RenderService->CreateVertexBuffer( SAILVERTEX_FORMAT, sg.nVert*sizeof(SAILVERTEX), D3DUSAGE_WRITEONLY );
    sg.indxBuf = RenderService->CreateIndexBuffer( sg.nIndx*2, D3DUSAGE_DYNAMIC );

    SAILVERTEX* pv; pv=(SAILVERTEX*)RenderService->LockVertexBuffer(sg.vertBuf);
    if(pv)
    {
        for(int i=0; i<sailQuantity; i++)
        {
            slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
            slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
        }
        RenderService->UnLockVertexBuffer(sg.vertBuf);
    }

    // Set triangle buffer for sea mirror
    WORD* pt; pt = (WORD*)RenderService->LockIndexBuffer(sg.indxBuf);
    if( pt )
    {
        for(int i=0; i<sailQuantity; i++)
        {
            slist[i]->FillIndex(pt);
        }

        // for triangle sail
        pt[0]=pt[15]=pt[24] = 0;//0
        pt[1]=pt[3]=pt[6]=pt[12]=pt[16]=pt[21]=pt[25] =
            SAIL_ROW_MAX/2*(SAIL_ROW_MAX/2+1)/2;//1
        pt[2]=pt[8]=pt[9]=pt[17]=pt[18]=pt[23]=pt[26] =
            SAIL_ROW_MAX/2*(SAIL_ROW_MAX/2+3)/2;//2
        pt[4]=pt[13] = (SAIL_ROW_MAX-1)*SAIL_ROW_MAX/2;//3
        pt[5]=pt[7]=pt[10]=pt[14]=pt[19]=pt[22] =
            (SAIL_ROW_MAX-1)*SAIL_ROW_MAX/2+SAIL_ROW_MAX/2;//4
        pt[11]=pt[20] = (SAIL_ROW_MAX-1)*(SAIL_ROW_MAX+2)/2;//5

        // for square sail
        pt[27]=pt[42]=pt[51]=0;//0
        pt[28]=pt[31]=pt[40]=pt[43]=pt[52] = SAIL_ROW_MAX*SAIL_COL_MAX-SAIL_ROW_MAX;//3
        pt[29]=pt[30]=pt[33]=pt[39]=pt[44]=pt[48]=pt[53] = SAIL_ROW_MAX/2;//1
        pt[32]=pt[34]=pt[37]=pt[41]=pt[46]=pt[49] = SAIL_ROW_MAX*SAIL_COL_MAX-SAIL_ROW_MAX/2-1;//4
        pt[35]=pt[36]=pt[45]=pt[50] = SAIL_ROW_MAX-1;//2
        pt[38]=pt[47] = SAIL_ROW_MAX*SAIL_COL_MAX-1;//5

        RenderService->UnLockIndexBuffer(sg.indxBuf);
    }
}

void SAIL::LoadSailIni()
{
    GUARD(SAIL::LoadSailIni());
	char	section[256],param[256];

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

	sprintf(section,"SAILS");

	// load texture names
    texQuantity= (int)ini->GetLong(section,"TextureCount",1);
    if(texQuantity==0) texQuantity=1;
    texNumCommon= (float)ini->GetLong(section,"TexNumCommon",0) / (float)texQuantity;
    texNumEnglish= (float)ini->GetLong(section,"TexNumEnglish",0) / (float)texQuantity;
    texNumTreangle= (float)ini->GetLong(section,"TexNumTreangle",0) / (float)texQuantity;

    // load speed calculate parameters:
    g_fSailHoleDepend= ini->GetFloat(section,"fHoleDepend",1.0f);
    ini->ReadString(section,"TreangleWindSpeed",param,sizeof(param)-1,"0.2,0.6,0.8");
    sscanf(param,"%f,%f,%f",&ts_min,&ts_xdep,&ts_zdep);
    ini->ReadString(section,"TrapecidalWindSpeed",param,sizeof(param)-1,"0.4,0.5,0.6");
    sscanf(param,"%f,%f,%f",&fs_min,&fs_xdep,&fs_zdep);
    ini->ReadString(section,"SquareWindSpeed",param,sizeof(param)-1,"0.4,0.1,0.6");
    sscanf(param,"%f,%f,%f",&ss_min,&ss_xdep,&ss_zdep);

    // load wind depend parameters
    SsailWindDepend= ini->GetFloat(section,"fSsailWindDepend",.05f);
    TsailWindDepend= ini->GetFloat(section,"fTsailWindDepend",.5f);
    fWindAdding= ini->GetFloat(section,"fWindAdding",.3f);
    FLEXSPEED= ini->GetFloat(section,"FLEXSPEED",.001f);
    MAXSUMWIND= ini->GetFloat(section,"MAXSUMWIND",.02f);
    WINDVECTOR_QUANTITY= (int)ini->GetLong(section,"WINDVECTOR_QNT",60);
    WINDVECTOR_TINCR= (int)ini->GetLong(section,"WINDVECTOR_TINCR",3);
    WINDVECTOR_TADD= (int)ini->GetLong(section,"WINDVECTOR_TADD",3);
    WINDVECTOR_SINCR= (int)ini->GetLong(section,"WINDVECTOR_SINCR",6);
    WINDVECTOR_SADD= (int)ini->GetLong(section,"WINDVECTOR_SADD",3);

    // load rolling sail parameters
    ROLL_Z_VAL= ini->GetFloat(section,"ROLL_Z_VAL",.01f);
    ROLL_Z_DELTA= ini->GetFloat(section,"ROLL_Z_DELTA",.001f);
    ROLL_Y_VAL= ini->GetFloat(section,"ROLL_Y_VAL",.04f);
    ROLL_Y_DELTA= ini->GetFloat(section,"ROLL_Y_DELTA",.001f);

    // sail turning parameters
    WINDANGL_DISCRETE= ini->GetFloat(section,"WINDANGLDISCRETE",.01f);
    MAXTURNANGL= ini->GetFloat(section,"MAXTURNANGL",.6f);
	TURNSTEPANGL= ini->GetFloat(section,"TURNSTEPANGL",.002f);
    ROLLINGSPEED= ini->GetFloat(section,"ROLLINGSPEED",.0003f);

    // load material parameters
    ini->ReadString(section,"Diffuse",param,sizeof(param)-1,"0.0,0.0,0.0,0.0");
    sscanf(param,"%f,%f,%f,%f",&mat.Diffuse.r,&mat.Diffuse.g,&mat.Diffuse.b,&mat.Diffuse.a);
    ini->ReadString(section,"Ambient",param,sizeof(param)-1,"0.0,0.0,0.0,0.0");
    sscanf(param,"%f,%f,%f,%f",&mat.Ambient.r,&mat.Ambient.g,&mat.Ambient.b,&mat.Ambient.a);
    ini->ReadString(section,"Specular",param,sizeof(param)-1,"0.0,0.0,0.0,0.0");
    sscanf(param,"%f,%f,%f,%f",&mat.Specular.r,&mat.Specular.g,&mat.Specular.b,&mat.Specular.a);
    ini->ReadString(section,"Emissive",param,sizeof(param)-1,"0.7,0.7,0.7,0.7");
    sscanf(param,"%f,%f,%f,%f",&mat.Emissive.r,&mat.Emissive.g,&mat.Emissive.b,&mat.Emissive.a);
	mat.Power = ini->GetFloat(section,"Power",0.f);

    // load ROLLING SAIL form table
    // load square sail form
    ini->ReadString(section,"rollSSailForm",param,sizeof(param)-1,
        "0.2,0.8,1.0,0.8,0.4,1.0,1.3,1.0,0.4,0.8,1.0,0.8,0.2");
    sscanf(param,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &SSailRollForm[0],
        &SSailRollForm[1], &SSailRollForm[2], &SSailRollForm[3], &SSailRollForm[4],
        &SSailRollForm[5], &SSailRollForm[6], &SSailRollForm[7], &SSailRollForm[8],
        &SSailRollForm[9], &SSailRollForm[10], &SSailRollForm[11], &SSailRollForm[12] );
    // load treangle sail form
    ini->ReadString(section,"rollTSailForm",param,sizeof(param)-1,
        "0.1,0.6,0.3,0.8,0.2");
    sscanf(param,"%f,%f,%f,%f,%f", &TSailRollForm[0], &TSailRollForm[1],
        &TSailRollForm[2], &TSailRollForm[3], &TSailRollForm[4] );
    TR_FORM_MUL= ini->GetFloat(section,"tr_form_mul",2.f);

    // load hole depend parameters
    fTHoleFlexDepend= ini->GetFloat(section,"fTHoleFlexDepend",.01f);
    if(fTHoleFlexDepend>.1f) fTHoleFlexDepend=0.1f;
    fSHoleFlexDepend= ini->GetFloat(section,"fSHoleFlexDepend",.01f);
    if(fSHoleFlexDepend>(1.f/12.f)) fSHoleFlexDepend=1.f/12.f;

    // load parameter for sails of fall mast
        // square sails
    FALL_SSAIL_ADD_MIN = ini->GetFloat(section,"fFallSSailAddMin",0.2f);
    FALL_SSAIL_ADD_RAND = ini->GetFloat(section,"fFallSSailAddRand",0.2f);
        // treangle sails
    FALL_TSAIL_ADD_MIN = ini->GetFloat(section,"fFallTSailAddMin",0.2f);
    FALL_TSAIL_ADD_RAND = ini->GetFloat(section,"fFallTSailAddRand",0.2f);

	GROUP_UPDATE_TIME = ini->GetLong(section,"msecSailUpdateTime",GROUP_UPDATE_TIME);

    delete ini;

    UNGUARD
}

float SAIL::Trace(const CVECTOR &src,const CVECTOR &dst)
{
    float retVal=2.f;
    if(!bUse) return retVal;

    for(int i=0; i<groupQuantity; i++)
    {
		if(gdata[i].bDeleted) continue;
        CVECTOR minp=gdata[i].boxCenter-gdata[i].boxSize;
        CVECTOR maxp=gdata[i].boxCenter+gdata[i].boxSize;

        if(  !(src.x<=maxp.x && src.x>=minp.x &&
               src.y<=maxp.y && src.y>=minp.y &&
               src.z<=maxp.z && src.z>=minp.z)
             &&
             !(dst.x<=maxp.x && dst.x>=minp.x &&
               dst.y<=maxp.y && dst.y>=minp.y &&
               dst.z<=maxp.z && dst.z>=minp.z) )
        {
            bool bYesTrace=false;
            CVECTOR vmed;
            // сечение по X:
            if(src.x<minp.x)
                if(dst.x<minp.x) continue;
                else
                {
                    vmed = src + (dst-src)*(minp.x-src.x)/(dst.x-src.x);
                    if( vmed.y>=minp.y && vmed.y<=maxp.y &&
                        vmed.z>=minp.z && vmed.z<=maxp.z )
                        bYesTrace=true;
                }
            if(!bYesTrace && src.x>maxp.x)
                if(dst.x>maxp.x) continue;
                else
                {
                    vmed = src + (dst-src)*(maxp.x-src.x)/(dst.x-src.x);
                    if( vmed.y>=minp.y && vmed.y<=maxp.y &&
                        vmed.z>=minp.z && vmed.z<=maxp.z )
                        bYesTrace=true;
                }
            // сечение по Z:
            if(!bYesTrace && src.z<minp.z)
                if(dst.z<minp.z) continue;
                else
                {
                    vmed = src + (dst-src)*(minp.z-src.z)/(dst.z-src.z);
                    if( vmed.y>=minp.y && vmed.y<=maxp.y &&
                        vmed.x>=minp.x && vmed.x<=maxp.x )
                        bYesTrace=true;
                }
            if(!bYesTrace && src.z>maxp.z)
                if(dst.z>maxp.z) continue;
                else
                {
                    vmed = src + (dst-src)*(maxp.z-src.z)/(dst.z-src.z);
                    if( vmed.y>=minp.y && vmed.y<=maxp.y &&
                        vmed.x>=minp.x && vmed.x<=maxp.x )
                        bYesTrace=true;
                }
            // сечение по Y:
            if(!bYesTrace && src.y<minp.y)
                if(dst.y<minp.y) continue;
                else
                {
                    vmed = src + (dst-src)*(minp.y-src.y)/(dst.y-src.y);
                    if( vmed.z>=minp.z && vmed.z<=maxp.z &&
                        vmed.x>=minp.x && vmed.x<=maxp.x )
                        bYesTrace=true;
                }

            if(!bYesTrace) continue;
        }


        LastTraceGroup=i;
        //==============================================
        // лучь прошел через куб вокруг парусов корабля
        // начинаем искать парус в который он попал
        //==============================================
        float tmp;
		traceSail=-1;
        for(int j=0; j<gdata[i].sailQuantity; j++)
        {
            tmp=slist[gdata[i].sailIdx[j]]->Trace(src,dst,bCannonTrace);
            if(tmp<=1.f)
                if(bCannonTrace)
                {
                    if(retVal>tmp) {retVal=tmp; traceSail=gdata[i].sailIdx[j];}
                }
                else {retVal=tmp; traceSail=gdata[i].sailIdx[j]; break;}
        }

        if(retVal<=1.f) break; // трассируем только по одному кораблю
    }

    return retVal;
}

const char* SAIL::GetCollideMaterialName()
{
    return "sail";
}

bool SAIL::GetCollideTriangle(struct TRIANGLE & trg)
{
	return false;
}

bool SAIL::Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly)
{
    return false;
}

void SAIL::FirstRun()
{
    int i,sn;

    if(wFirstIndx)       SetAddSails(wFirstIndx);
    else       SetAllSails();

    ENTITY_ID ropeEI;
    if(_CORE_API->FindClass(&ropeEI,"rope",0))
    // расчет позиции согласно положения веревок
    for(sn=wFirstIndx; sn<sailQuantity; sn++)
    {
        bool bChange=false;
        if(slist[sn]->sroll==0 && !slist[sn]->bRolling) // производим перерасчет натяжения веревки если парус не в режиме поднятия
        for(i=0; i<2; i++)
            if(slist[sn]->sailtrope.rrs[i])
            {
                int tieNum=slist[sn]->sailtrope.rrs[i]->tiePoint;
                CVECTOR endVect;
                ((ROPE_BASE*)_CORE_API->GetEntityPointer(&ropeEI))->GetEndPoint(&endVect,slist[sn]->sailtrope.rrs[i]->ropenum,gdata[slist[sn]->HostNum].modelEI);
                CVECTOR medVect;
                medVect=slist[sn]->ss.hardPoints[tieNum];
                CVECTOR begVect;
                switch(tieNum)
                {
                case 1:
                    begVect=slist[sn]->ss.hardPoints[0];
                    break;
                case 2:
                    begVect=slist[sn]->ss.hardPoints[0];
                    break;
                case 3:
                    begVect=slist[sn]->ss.hardPoints[1];
                    break;
                }

                float r1; r1=slist[sn]->sailtrope.rrs[i]->r1;
                float r2; r2=slist[sn]->sailtrope.rrs[i]->r2;

                if(slist[sn]->ss.eSailType!=SAIL_TREANGLE)
                {
                    medVect= (endVect*r1 + slist[sn]->sailtrope.rrs[i]->b*r2)/(r1+r2);
                }
                else
                {
                    medVect.x= (endVect.x*r1+begVect.x*r2)/(r1+r2);
                    medVect.z= (endVect.z*r1 + begVect.z*r2)/(r1+r2);
                }
                *slist[sn]->sailtrope.pPos[tieNum]=slist[sn]->ss.hardPoints[tieNum]=medVect;

                bChange=true;
            }
        if(bChange)
        {
            slist[sn]->SetGeometry();
            if(slist[sn]->ss.eSailType==SAIL_TREANGLE)
                slist[sn]->sailHeight=sqrtf(~(slist[sn]->ss.hardPoints[1]-slist[sn]->ss.hardPoints[2]));
            else
                slist[sn]->sailHeight=sqrtf(~(slist[sn]->ss.hardPoints[0]-slist[sn]->ss.hardPoints[2]));
        }
    }

    bFirstRun=false;
    wFirstIndx=sailQuantity;
    bUse = sailQuantity>0;
}

float SAIL::Cannon_Trace(long iBallOwner, const CVECTOR &src,const CVECTOR &dst)
{
    bCannonTrace = true;
	g_iBallOwnerIdx = iBallOwner;

    float retVal=Trace(src,dst);

    bCannonTrace=false;
	g_iBallOwnerIdx = -1;

	if(retVal<2.f)
	{
		if( !slist[traceSail]->bFreeSail && !gdata[slist[traceSail]->HostNum].bDeleted )
		{
			CVECTOR damagePoint = src+(dst-src)*retVal;
			VAI_OBJBASE * pvai = (VAI_OBJBASE *)_CORE_API->GetEntityPointer(&gdata[slist[traceSail]->HostNum].shipEI);
			ATTRIBUTES * pA;
			if(pvai!=null) pA=pvai->GetACharacter();
			long charIdx=-1;
			if(pA!=null) charIdx = pA->GetAttributeAsDword("index",-1);
			api->Event(SHIP_SAIL_DAMAGE,"lfff",charIdx, damagePoint.x,damagePoint.y,damagePoint.z);
		}
	}

    return retVal;
}

void SAIL::DoSailToNewHost(ENTITY_ID newModelEI, ENTITY_ID newHostEI, int grNum, NODE *nod, ENTITY_ID oldModelEI)
{
    if(groupQuantity<1 || sailQuantity<1) return;

    // найдем старого хозяина
    for(int oldg=0; oldg<groupQuantity; oldg++)
        if(gdata[oldg].modelEI==oldModelEI && !gdata[oldg].bDeleted)  break;
    if(oldg==groupQuantity) return; // нет старой модели - возвращаемся ничего не сделав

    // найдем парус
    for(int sn=0; sn<sailQuantity; sn++)
        if( slist[sn]->hostNode==nod &&
            slist[sn]->HostNum==oldg &&
            (grNum==0 || slist[sn]->groupNum==grNum) )  break;
    if(sn==sailQuantity) return; // нет такого паруса - возвращаемся без результата

    // в старом хозяине найдем ссылку на наш парус
    for(int idx=0; idx<gdata[oldg].sailQuantity; idx++)
        if(gdata[oldg].sailIdx[idx]==sn) break;
    if(idx==gdata[oldg].sailQuantity) return; // нет паруса в группе - возврат без результата

    // найдем нового хозяина
    for(int gn=0; gn<groupQuantity; gn++)
        if(gdata[gn].modelEI==newModelEI) break;
    if(gn==groupQuantity) // нет такого хозяина - создаем нового
    {
		GROUPDATA *oldgdata=gdata;
        gdata= NEW GROUPDATA[groupQuantity+1];
        if(gdata==0)
            _THROW("Not memory allocation");
        memcpy(gdata,oldgdata,sizeof(GROUPDATA)*groupQuantity);
		gdata[gn].bYesShip = false;
        gdata[gn].bDeleted=false;
        gdata[gn].sailQuantity = 0;
        gdata[gn].boxCenter = gdata[gn].boxSize=CVECTOR(0.f,0.f,0.f);
        gdata[gn].boxRadius = 0.f;
        gdata[gn].shipEI = newHostEI;
        gdata[gn].speed_m = 0.f;
        gdata[gn].modelEI=newModelEI;
		gdata[gn].dwSailsColor = 0xFFFFFFFF;
		gdata[gn].maxSP = 100;
        VOIDPTR_DELETE(oldgdata);
		groupQuantity++;
		bDeleteState = true;
    }

	// поищем новый парус в новой группе
	for(int i=0; i<gdata[gn].sailQuantity; i++)
		if(gdata[gn].sailIdx[i]==sn) break;

	if(m_nMastCreatedCharacter>=0 && slist[sn]!=null)
		api->Event("DoSailHole","llssllllf", -1, m_nMastCreatedCharacter,
					(m_sMastName==null?"#":m_sMastName),
					slist[sn]->hostNode->GetName(),slist[sn]->groupNum,
					slist[sn]->GetMaxHoleCount(),
					(1<<slist[sn]->GetMaxHoleCount())-1,
					slist[sn]->GetMaxHoleCount(),
					(float)slist[sn]->maxSpeed/gdata[oldg].speed_m);

	if(i==gdata[gn].sailQuantity)
	{
		// добавим найденный парус в новую группу
		if(gdata[gn].sailQuantity==0)
		{
			gdata[gn].sailIdx= NEW int[1];
			gdata[gn].sailQuantity=1;
		}
		else
		{
			int* oldIdx=gdata[gn].sailIdx;
			if((gdata[gn].sailIdx= NEW int[gdata[gn].sailQuantity+1])==0)
				{_THROW("Not memory allocation");}
			memcpy(gdata[gn].sailIdx,oldIdx,sizeof(int)*gdata[gn].sailQuantity);
			delete oldIdx; gdata[gn].sailQuantity++;
		}
		gdata[gn].sailIdx[gdata[gn].sailQuantity-1] = sn;
		gdata[gn].speed_m += slist[sn]->maxSpeed;
	}
	slist[sn]->HostNum=gn;

    // удалим парус из старой группы
    if(gdata[oldg].sailQuantity==1)
    // единственный парус убираем вместе с группой
    {
		// для корабля записываем значение состояния парусов = 0
		VAI_OBJBASE * pVai = (VAI_OBJBASE *)api->GetEntityPointer(&gdata[oldg].shipEI);
		if(pVai && pVai->GetACharacter())
		{
			ATTRIBUTES * pA = pVai->GetACharacter()->GetAttributeClass("Ship");
			if(pA) pA->SetAttributeUseDword("SP",0);
		}
        PTR_DELETE(gdata[oldg].sailIdx);
		gdata[oldg].sailQuantity = 0;
		gdata[oldg].bDeleted = true;
    }
    else
    {
        gdata[oldg].sailQuantity--;
		gdata[oldg].sailIdx[idx] = gdata[oldg].sailIdx[gdata[oldg].sailQuantity];
    }
	bDeleteState = true;

    // изменим некоторые значения для нашего паруса
    PTR_DELETE(slist[sn]->sailtrope.rrs[0]);
    PTR_DELETE(slist[sn]->sailtrope.rrs[1]);
    slist[sn]->sailtrope.pnttie[0] = slist[sn]->sailtrope.pnttie[1] =
    slist[sn]->sailtrope.pnttie[2] = slist[sn]->sailtrope.pnttie[3] = false;
    slist[sn]->sailtrope.pPos[0] = slist[sn]->sailtrope.pPos[1] =
    slist[sn]->sailtrope.pPos[2] = slist[sn]->sailtrope.pPos[3] = null;
    slist[sn]->ss.turningSail = false;
}

void SAIL::DeleteSailGroup()
{
	bUse = false;
    int gn,sn,i;

    int old_sailQuantity = sailQuantity;
    int old_groupQuantity = groupQuantity;

    // удалим все паруса из удаленных группы //
    //---------------------------------------//
    for(sn=0,sailQuantity=0; sn<old_sailQuantity; sn++)
        if(gdata[slist[sn]->HostNum].bDeleted || slist[sn]->bDeleted)	{PTR_DELETE(slist[sn]);}
		else	sailQuantity++;

	if(sailQuantity==0)
		sailQuantity=0;

	// подсчет числа групп
	for(gn=0,groupQuantity=0; gn<old_groupQuantity; gn++)
		if(!gdata[gn].bDeleted) groupQuantity++;
		else
		{
			PTR_DELETE(gdata[gn].sailIdx);
			gdata[gn].sailQuantity = 0;
		}

    // новые списки парусов и групп
    SAILONE **oldslist = slist;
	GROUPDATA *oldgdata = gdata;
	if(sailQuantity==0 || groupQuantity==0)
	{
		for(i=0;i<old_sailQuantity;i++) {PTR_DELETE(oldslist[i]);}
		slist=null;
		for(i=0;i<old_groupQuantity;i++) {PTR_DELETE(oldgdata[i].sailIdx);}
		gdata=null;
		sailQuantity=0;	groupQuantity=0;
	}
	else
	{
		slist = NEW SAILONE*[sailQuantity];
		gdata = NEW GROUPDATA[groupQuantity];
		if(slist==null || gdata==null)	{_THROW("allocate memory error");}

		groupQuantity = 0;
		sailQuantity = 0;
		for( gn=0; gn<old_groupQuantity; gn++ )
		{
			PTR_DELETE(oldgdata[gn].sailIdx);
			// подсчет число парусов в группе
			int nsn = 0;
			for( sn=0; sn<old_sailQuantity; sn++ )	if(oldslist[sn]!=null && oldslist[sn]->HostNum==gn) nsn++;
			if(nsn==0) continue;
			//  в новом месте создаем запись о группе парусов
			memcpy(&gdata[groupQuantity],&oldgdata[gn],sizeof(GROUPDATA));
			if( (gdata[groupQuantity].sailIdx=NEW int[nsn]) == null )	{_THROW("allocate memory error");}
			gdata[groupQuantity].sailQuantity = nsn;
			// заполняем список парусов для группы и общий
			for( sn=0,nsn=0; sn<old_sailQuantity; sn++ )
				if(oldslist[sn]!=null && oldslist[sn]->HostNum==gn)
				{
					gdata[groupQuantity].sailIdx[nsn++] = sailQuantity;
					slist[sailQuantity] = oldslist[sn];
					slist[sailQuantity]->HostNum = groupQuantity;
					sailQuantity++;
				}
			// переход на другую группу
			groupQuantity++;
		}
	}
	VOIDPTR_DELETE(oldgdata);
	PTR_DELETE(oldslist);

    //подсчет новых параметров буферов вертексов и индексов
    long vIndx=0; // индекс на вертекс буфер
    // проход по всем группам и простановка новых ссылок на буфера
    for(gn=0; gn<groupQuantity; gn++)
    {
        for(sn=0; sn<gdata[gn].sailQuantity; sn++)
        {
            slist[gdata[gn].sailIdx[sn]]->ss.sVert = vIndx;
			slist[gdata[gn].sailIdx[sn]]->ss.sIndx = 54;
            vIndx += slist[gdata[gn].sailIdx[sn]]->ss.nVert;
        }
    }
    sg.nVert=vIndx;

    // удалим старые буферы
	VERTEX_BUFFER_RELEASE(RenderService,sg.vertBuf);
    if(sg.nVert>0)
    {
        // создадим новые буферы
        sg.vertBuf=RenderService->CreateVertexBuffer(SAILVERTEX_FORMAT,sg.nVert*sizeof(SAILVERTEX),D3DUSAGE_WRITEONLY);

        SAILVERTEX* pv; pv=(SAILVERTEX*)RenderService->LockVertexBuffer(sg.vertBuf);
        if(pv)
        {
            for(i=0; i<sailQuantity; i++)
            {
                slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
                slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
            }
            RenderService->UnLockVertexBuffer(sg.vertBuf);
        }
    }

    bDeleteState=false;
    wFirstIndx=sailQuantity;
    bUse = sailQuantity>0;
}

void SAIL::SetAddSails(int firstSail)
{
    for(int i=firstSail; i<sailQuantity; i++)
    {
        // сформировать параметры буферов вертекса и индекса
		slist[i]->SetTurnLimits();
        slist[i]->ss.sVert=sg.nVert;
        slist[i]->ss.sIndx=54;
        sg.nVert+=slist[i]->ss.nVert;
    }

    // удалим старые буферы
	VERTEX_BUFFER_RELEASE(RenderService,sg.vertBuf);
    // создадим новые буферы
    sg.vertBuf=RenderService->CreateVertexBuffer(SAILVERTEX_FORMAT,sg.nVert*sizeof(SAILVERTEX),D3DUSAGE_WRITEONLY);

    // заполним вертекс буфер и установим текстурные координаты
    SAILVERTEX* pv; pv=(SAILVERTEX*)RenderService->LockVertexBuffer(sg.vertBuf);
    if(pv)
    {
        for(int i=0; i<sailQuantity; i++)
        {
            slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
            slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
        }
        RenderService->UnLockVertexBuffer(sg.vertBuf);
    }
    else
        _THROW("Vertex buffer error");
}

void SAIL::DoNoRopeSailToNewHost(ENTITY_ID newModel, ENTITY_ID newHost, ENTITY_ID oldHost)
{
    ENTITY_ID rope_id;
    if( !_CORE_API->FindClass(&rope_id,"rope",0) ) return; // нет веревок нет концерта
    ROPE_BASE *rb = (ROPE_BASE*)_CORE_API->GetEntityPointer(&rope_id);
    if(rb==null) return;

    // найдем группу старого хозяина
    for(int ogn=0; ogn<groupQuantity; ogn++)
        if(gdata[ogn].bYesShip && gdata[ogn].shipEI==oldHost && !gdata[ogn].bDeleted) break;
	if(ogn==groupQuantity) return;

    // новый root NODE
    MODEL *nmdl = (MODEL*)api->GetEntityPointer(&newModel);
    if(nmdl==null) return;
    NODE *nroot = nmdl->GetNode(0);
    if(nroot==null) return;

    MODEL *omdl = (MODEL*)api->GetEntityPointer(&gdata[ogn].modelEI);
    if(omdl==null) return;

    // в найденной группе пройдемся по парусам 
    for(int si=0; si<gdata[ogn].sailQuantity; si++)
    {
        int sn= gdata[ogn].sailIdx[si];
        NODE *nod=null;
        for(int k=0; k<1000; k++)
        {
            nod = omdl->GetNode(k);
            if(nod==null) break;
            if(nod==slist[sn]->hostNode) break;
        }
        if(nod==null) continue;

        GEOS *tgeo = slist[sn]->hostNode->geo;
        GEOS::INFO gi; tgeo->GetInfo(gi);
        for(int i=0; i<gi.nlabels; i++)
        {
            GEOS::LABEL gl; tgeo->GetLabel(i,gl);
            if(!strncmp(gl.group_name,"sail",4)) // это группа парусов
                if(atoi(&gl.group_name[5])==slist[sn]->groupNum) // с правильным номером группы
                    if( !strncmp(gl.name,"rope",4) || !strncmp(gl.name,"fal",3) ) // и парус связан с веревкой
					{
						long nRopeNum;
						if( gl.name[0]=='r' ) nRopeNum = atoi(&gl.name[5]);
						else nRopeNum = atoi(&gl.name[4]) + 1000;
                        if( rb->IsAbsentRope(gdata[ogn].modelEI, nRopeNum) )
                        {
                            // удалим веревки принадлежащие этому парусу
                            if( slist[sn]->sailtrope.pnttie[0] ||
                                slist[sn]->sailtrope.pnttie[1] ||
                                slist[sn]->sailtrope.pnttie[2] ||
                                slist[sn]->sailtrope.pnttie[3] )
                            {
                                rb->DoDeleteUntie( gdata[ogn].modelEI,
                                                   slist[sn]->hostNode,
                                                   slist[sn]->groupNum );
                            }
                            // делаем перенос паруса в новую группу
                            DoSailToNewHost(newModel,newHost,slist[sn]->groupNum,slist[sn]->hostNode,gdata[ogn].modelEI);
                            // перенесем этот парус в другой NODE
                            int iMax=4;
                            if(slist[sn]->ss.eSailType==SAIL_TREANGLE) iMax=3;
                            for(int j=0; j<iMax; j++)
                                nroot->glob_mtx.MulToInv(
                                    slist[sn]->hostNode->glob_mtx*slist[sn]->ss.hardPoints[j],
                                    slist[sn]->ss.hardPoints[j]);
                            slist[sn]->hostNode = nroot;
                            slist[sn]->pMatWorld = &nroot->glob_mtx;
                        }
					}
        }
    }
}

void _cdecl sailPrint(VDX8RENDER *rs, const CVECTOR & pos3D, float rad, long line, const char * format, ...)
{
	static char buf[256];
	//Печатаем в буфер
	long len = _vsnprintf(buf, sizeof(buf) - 1, format, (char *)(&format + 1));
	buf[sizeof(buf) - 1] = 0;
	//Ищем позицию точки на экране
	static CMatrix mtx, view, prj;
	static D3DVIEWPORT8 vp;
	MTX_PRJ_VECTOR vrt;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	view.Transposition();
	float dist = ~(pos3D - view.Pos());
	if(dist >= rad*rad) return;
	float d = view.Vz() | view.Pos();
	if((pos3D | view.Vz()) < d) return;
	rs->GetViewport(&vp);
	mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width*0.5f, vp.Height*0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
	//Ищем позицию
	long fh = rs->CharHeight(FONT_DEFAULT)/2;
	vrt.y -= (line + 0.5f)*fh;
	//Прозрачность	
	long color = 0xffffffff;
	const float kDist = 0.75f;
	if(dist > kDist*kDist*rad*rad)
	{
		dist = 1.0f - (sqrtf(dist) - kDist*rad)/(rad - kDist*rad);
		color = (dword(dist*255.0f) << 24) | 0xffffff;
	}
	rs->ExtPrint(FONT_DEFAULT, color, 0x00000000, ALIGN_CENTER, 0, 1.0f, 0, 0, long(vrt.x), long(vrt.y), buf);
}

SAILONE * SAIL::FindSailFromData(int gn,char* nodeName,char* grName)
{
	if(nodeName==null || grName==null) return null;
	int grNum;
	sscanf(grName,"%d",&grNum);
	for(int i=0; i<gdata[gn].sailQuantity; i++)
	{
		int sn = gdata[gn].sailIdx[i];
		if( slist[sn]->hostNode!=null &&
			strcmp(slist[sn]->hostNode->GetName(),nodeName)==0 &&
			slist[sn]->groupNum==grNum )	return slist[sn];
	}
	return null;
}

void SAIL::SetSailTextures(long grNum, VDATA* pvd)
{
	if( grNum<0 || grNum>=groupQuantity || pvd==null ) return;

	ATTRIBUTES * pA = pvd->GetAClass();
	if(pA==null) return;

	gdata[grNum].maxSP = pA->GetAttributeAsDword("MaxSP",gdata[grNum].maxSP);

	// основная текстура
	char* pcNormalName = pA->GetAttribute("normalTex");
	// герб текстуры
	IDirect3DTexture8* pGeraldTexture = (IDirect3DTexture8*)pA->GetAttributeAsDword("geraldTexPointer",0);
	char* pcGeraldName = pA->GetAttribute("geraldTex");
	//
	gdata[grNum].dwSailsColor = pA->GetAttributeAsDword("sailscolor",0xFFFFFFFF);

	for(int i=0; i<gdata[grNum].sailQuantity; i++)
	{
		SAILONE * so = slist[gdata[grNum].sailIdx[i]];
		if(so==null || so->hostNode==null) continue;
		char param[256];
		sprintf(param,"%s",so->hostNode->GetName());
		ATTRIBUTES* pAGerald = pA->GetAttributeClass( param );
		if( pAGerald ) {
			if( pAGerald->GetAttribute("Gerald") )
				so->m_nGeraldTex = RenderService->TextureCreate( pAGerald->GetAttribute("Gerald") );
			if( so->m_nGeraldTex==-1 && pcGeraldName )
				so->m_nGeraldTex = RenderService->TextureCreate( pcGeraldName );
			if( so->m_nGeraldTex==-1 && pGeraldTexture ) {
				pGeraldTexture->AddRef();
				so->m_pGeraldTex = pGeraldTexture;
			}
			if( so->m_nGeraldTex!=-1 || so->m_pGeraldTex!=0 ) {
				so->m_bIsGerald = true;
				so->m_fHorzGeraldScale = pAGerald->GetAttributeAsFloat( "hscale", 0.5f );
				so->m_fVertGeraldScale = pAGerald->GetAttributeAsFloat( "vscale", so->m_fHorzGeraldScale );
				if( so->m_fHorzGeraldScale>0.f ) so->m_fHorzGeraldScale = 1.f / so->m_fHorzGeraldScale;
				else so->m_fHorzGeraldScale = 0.5f;
				if( so->m_fVertGeraldScale>0.f ) so->m_fVertGeraldScale = 1.f / so->m_fVertGeraldScale;
				else so->m_fVertGeraldScale = 0.5f;
			}
		}
		if(	so->surfaceTex==-1 )
			so->surfaceTex = RenderService->TextureCreate(pcNormalName);
	}
}

int SAIL::FindGroupForCharacter(int chrIdx)
{
	for(int gn=0; gn<groupQuantity; gn++)
	{
		if( gdata[gn].bDeleted || !gdata[gn].bYesShip ) continue;
		ATTRIBUTES* pA = ((VAI_OBJBASE*)gdata[gn].shipEI.pointer)->GetACharacter();
		if(pA!=null)
			if( (int)pA->GetAttributeAsDword("index",-1) == chrIdx )
				return gn;
	}
	return -1;
}

int SAIL::GetCharacterForGroup(int grNum)
{
	ATTRIBUTES* pA = null;
	if( gdata[grNum].bYesShip )
		pA = ((VAI_OBJBASE*)gdata[grNum].shipEI.pointer)->GetACharacter();
	if(pA!=null) return (int)pA->GetAttributeAsDword("index",-1);
	return -1;
}

SAILONE_BASE * SAIL::FindSailForCharacter(int chrIdx,char* nodeName,int grNum)
{
	if( nodeName==null ) return null;

	int gn = FindGroupForCharacter(chrIdx);

	if(gn>=0 && gn<groupQuantity)
	{
		for(int idx=0; idx<gdata[gn].sailQuantity; idx++)
		{
			int sn = gdata[gn].sailIdx[idx];
			if( slist[sn]->hostNode!=null &&
				slist[sn]->groupNum==grNum &&
				strcmp(slist[sn]->hostNode->GetName(),nodeName)==0 )
				return slist[sn];
		}
	}

	return null;
}

void SAIL::LostRender()
{
	if( sg.indxBuf != -1 ) RenderService->ReleaseIndexBuffer( sg.indxBuf );
	sg.indxBuf = -1;
}

void SAIL::RestoreRender()
{
	if( sg.nVert == 0 ) return;
	sg.indxBuf = RenderService->CreateIndexBuffer( sg.nIndx*2, D3DUSAGE_DYNAMIC );

	// Set triangle buffer for sea mirror
	WORD* pt = (WORD*)RenderService->LockIndexBuffer(sg.indxBuf);
	if( pt )
	{
		for(int i=0; i<sailQuantity; i++)
		{
			slist[i]->FillIndex(pt);
		}

		// for triangle sail
		pt[0]=pt[15]=pt[24] = 0;//0
		pt[1]=pt[3]=pt[6]=pt[12]=pt[16]=pt[21]=pt[25] =
			SAIL_ROW_MAX/2*(SAIL_ROW_MAX/2+1)/2;//1
		pt[2]=pt[8]=pt[9]=pt[17]=pt[18]=pt[23]=pt[26] =
			SAIL_ROW_MAX/2*(SAIL_ROW_MAX/2+3)/2;//2
		pt[4]=pt[13] = (SAIL_ROW_MAX-1)*SAIL_ROW_MAX/2;//3
		pt[5]=pt[7]=pt[10]=pt[14]=pt[19]=pt[22] =
			(SAIL_ROW_MAX-1)*SAIL_ROW_MAX/2+SAIL_ROW_MAX/2;//4
		pt[11]=pt[20] = (SAIL_ROW_MAX-1)*(SAIL_ROW_MAX+2)/2;//5

		// for square sail
		pt[27]=pt[42]=pt[51]=0;//0
		pt[28]=pt[31]=pt[40]=pt[43]=pt[52] = SAIL_ROW_MAX*SAIL_COL_MAX-SAIL_ROW_MAX;//3
		pt[29]=pt[30]=pt[33]=pt[39]=pt[44]=pt[48]=pt[53] = SAIL_ROW_MAX/2;//1
		pt[32]=pt[34]=pt[37]=pt[41]=pt[46]=pt[49] = SAIL_ROW_MAX*SAIL_COL_MAX-SAIL_ROW_MAX/2-1;//4
		pt[35]=pt[36]=pt[45]=pt[50] = SAIL_ROW_MAX-1;//2
		pt[38]=pt[47] = SAIL_ROW_MAX*SAIL_COL_MAX-1;//5

		RenderService->UnLockIndexBuffer(sg.indxBuf);
	}
}

int SAIL::GetSailStateForCharacter(int chrIdx)
{
	int gn = FindGroupForCharacter(chrIdx);
	if( gn<0 || gn>=groupQuantity ) return 0;
	switch( gdata[gn].curSailSet )
	{
	case ROLLTYPE_NONE:	return 0; break;
	case ROLLTYPE_NORMAL: return 1; break;
	case ROLLTYPE_FAST: return 2; break;
	}
	return 0;
}

dword _cdecl SAIL::ScriptProcessing(char * name, MESSAGE & message)
{
	if( name==null ) return 0;

	if( stricmp(name,"RandomSailsDmg")==0 )
	{
		long chrIdx = message.Long();
		float fDmg = message.Float();
		int gn = FindGroupForCharacter(chrIdx);
		if(gn>=0 && gn<groupQuantity)	DoRandomsSailsDmg(chrIdx,gn,fDmg);
	}

	if( stricmp(name,"SailRollSpeed")==0 )
	{
		long chrIdx = message.Long();
		float fSpeed = message.Float();
		int gn = FindGroupForCharacter(chrIdx);
		if(gn>=0 && gn<groupQuantity)
			gdata[gn].fRollingSpeed = fSpeed*ROLLINGSPEED;
	}

	return 0;
}

void SAIL::DoRandomsSailsDmg(int chrIdx, int gn, float fDmg)
{
	if(gn<0 || gn>=groupQuantity) return;

	int i;
	int _sailsIdx[256];
	for(i=0; i<gdata[gn].sailQuantity; i++)
		_sailsIdx[i] = gdata[gn].sailIdx[i];

	for(int _sailsQnt=gdata[gn].sailQuantity; _sailsQnt>0;)
	{
		int tmp = rand()%_sailsQnt;
		int sn = _sailsIdx[tmp];

		int holeIdx = rand()%slist[sn]->GetMaxHoleCount();
		for( int j=0; j<14 && slist[sn]->ss.hole[holeIdx]; j++ )
			if(holeIdx<(int)slist[sn]->GetMaxHoleCount()-1) holeIdx++;
			else	holeIdx = 0;

		bool bOldHole = slist[sn]->ss.hole[holeIdx];
		slist[sn]->ss.hole[holeIdx] = true;
		int nNewHoleCount = slist[sn]->ss.holeCount;
		if(!bOldHole) nNewHoleCount++;
		VDATA * pvd = api->Event("evntRandomSailDmg","lslfflll",
			chrIdx,slist[sn]->hostNode->GetName(),slist[sn]->groupNum,
			fDmg, (float)slist[sn]->maxSpeed/gdata[gn].speed_m,
			nNewHoleCount, slist[sn]->GetMaxHoleCount(), slist[sn]->GetHoleDword());
		slist[sn]->ss.hole[holeIdx] = bOldHole;
		if(pvd==null)
		{
			api->Trace("WARNING!!! Event evntRandomSailDmg not float return!");
			return;
		}
		float fDoDmg = pvd->GetFloat();

		if(fDoDmg>0.f)
		{
			slist[sn]->ss.holeCount = nNewHoleCount;
			slist[sn]->ss.hole[holeIdx] = true;
			slist[sn]->CalculateMirrorSailIndex();
		}
		else	fDoDmg = -fDoDmg;
		if(fDoDmg==0.f)
		{
			_sailsQnt--;
			_sailsIdx[sn] = _sailsIdx[_sailsQnt];
		}
		else
		{
			fDmg -= fDoDmg;
			if(fDmg<=0.f)	{ return; }
		}
	}
}

dword SAIL::AttributeChanged(ATTRIBUTES * pAttr)
{
	if(pAttr==null) return 0;
	if( * pAttr == "MinSpeed" )	m_fMinSpeedVal = pAttr->GetAttributeAsFloat();
	return 0;
}
