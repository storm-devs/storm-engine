#include "sail.h"

#include "core.h"

#include "Weather_Base.h"
#include "defines.h"
#include "entity.h"
#include "shared/battle_interface/msg_control.h"
#include "shared/mast_msg.h"
#include "shared/sail_msg.h"
#include "shared/sea_ai/Script_defines.h"
#include "ship_base.h"
#include "vfile_service.h"

#define WIND_SPEED_MAX 12.f

static const char *RIGGING_INI_FILE = "resource\\ini\\rigging.ini";

void sailPrint(VDX9RENDER *rs, const CVECTOR &pos3D, float rad, long line, const char *format, ...);
int traceSail = -1;
long g_iBallOwnerIdx = -1;

float g_fSailHoleDepend = 1.0f;

float GetSailSpeed(int holeQ, int holeMax, float maxSpeed, float fSailHoleDepend)
{
    if (holeMax <= 0)
        holeMax = 1;
    if (holeQ <= 0)
        return maxSpeed;
    auto curSpeed = maxSpeed * (1.f - static_cast<float>(holeQ) / holeMax * fSailHoleDepend);
    if (curSpeed < 0.f)
        curSpeed = 0.f;
    return curSpeed;
}

SAIL::SAIL()
{
    // setting all general data to initial values
    // then rewritten from INI file
    // ---------------------------------------------------
    // texture names
    texQuantity = 1;
    texNumCommon = 0;
    texNumEnglish = 0;
    texNumTreangle = 0;

    // speed calculate parameters:
    ts_min = 0.2f;
    ts_xdep = 0.6f;
    ts_zdep = 0.8f;
    fs_min = 0.4f;
    fs_xdep = 0.5f;
    fs_zdep = 0.6f;
    ss_min = 0.4f;
    ss_xdep = 0.1f;
    ss_zdep = 0.6f;

    // wind depend parameters
    SsailWindDepend = .05f;
    TsailWindDepend = .5f;
    fWindAdding = .3f;
    FLEXSPEED = .001f;
    MAXSUMWIND = .02f;
    WINDVECTOR_QUANTITY = 60;
    WINDVECTOR_TINCR = 3;
    WINDVECTOR_TADD = 3;
    WINDVECTOR_SINCR = 6;
    WINDVECTOR_SADD = 3;

    // load rolling sail parameters
    ROLL_Z_VAL = .01f;
    ROLL_Z_DELTA = .001f;
    ROLL_Y_VAL = .04f;
    ROLL_Y_DELTA = .001f;

    // sail turning parameters
    WINDANGL_DISCRETE = .01f;
    MAXTURNANGL = .6f;
    TURNSTEPANGL = .002f;
    ROLLINGSPEED = .0003f;

    // material parameters
    mat.Diffuse.r = 0.f;
    mat.Diffuse.g = 0.f;
    mat.Diffuse.b = 0.f;
    mat.Diffuse.a = 0.f;
    mat.Ambient.r = 0.f;
    mat.Ambient.g = 0.f;
    mat.Ambient.b = 0.f;
    mat.Ambient.a = 0.f;
    mat.Specular.r = 0.f;
    mat.Specular.g = 0.f;
    mat.Specular.b = 0.f;
    mat.Specular.a = 0.f;
    mat.Emissive.r = .7f;
    mat.Emissive.g = .7f;
    mat.Emissive.b = .7f;
    mat.Emissive.a = 0.f;
    mat.Power = 0.f;

    // ROLLING SAIL form table
    // square sail form
    SSailRollForm[0] = 0.2f;
    SSailRollForm[1] = 0.8f;
    SSailRollForm[2] = 1.0f;
    SSailRollForm[3] = 0.8f;
    SSailRollForm[4] = 0.4f;
    SSailRollForm[5] = 1.0f;
    SSailRollForm[6] = 1.3f;
    SSailRollForm[7] = 1.0f;
    SSailRollForm[8] = 0.4f;
    SSailRollForm[9] = 0.8f;
    SSailRollForm[10] = 1.0f;
    SSailRollForm[11] = 0.8f;
    SSailRollForm[12] = 0.2f;
    // treangle sail form
    TSailRollForm[0] = 0.1f;
    TSailRollForm[1] = 0.6f;
    TSailRollForm[2] = 0.3f;
    TSailRollForm[3] = 0.8f;
    TSailRollForm[4] = 0.2f;
    TR_FORM_MUL = 2.f;

    // hole depend parameters
    fTHoleFlexDepend = .01f;
    fSHoleFlexDepend = .01f;

    // parameters for sails of fall mast
    // square sails
    FALL_SSAIL_ADD_MIN = 0.2f;
    FALL_SSAIL_ADD_RAND = 0.2f;
    // treangle sails
    FALL_TSAIL_ADD_MIN = 0.2f;
    FALL_TSAIL_ADD_RAND = 0.2f;
    //-----------------------------------------------------

    GROUP_UPDATE_TIME = 4000;

    SailQuantity = 0;
    SailCurNum = 0;

    globalWind.base = .4f;
    globalWind.ang.x = 0.f;
    globalWind.ang.y = 0.f;
    globalWind.ang.z = 1.f;
    WindVect = nullptr;

    texl = -1;
    m_nEmptyGerbTex = -1;

    bFirstRun = true;
    wFirstIndx = 0;
    bDeleteState = false;
    RenderService = nullptr;

    bCannonTrace = false;

    groupQuantity = 0;
    gdata = nullptr;
    sailQuantity = 0;
    slist = nullptr;
    bUse = false;

    m_nMastCreatedCharacter = -1;
    m_sMastName = nullptr;

    sg.indxBuf = -1;
    sg.vertBuf = -1;

    m_nLastUpdate = 0;

    m_fMinSpeedVal = 0.f;
}

SAIL::~SAIL()
{
    if (slist != nullptr)
    {
        for (auto i = 0; i < sailQuantity; i++)
            STORM_DELETE(slist[i]);
        STORM_DELETE(slist);
    }
    if (gdata != nullptr)
    {
        for (auto i = 0; i < groupQuantity; i++)
        {
            STORM_DELETE(gdata[i].sailIdx);
        }
        delete (char *)gdata;
        gdata = nullptr;
    }

    VERTEX_BUFFER_RELEASE(RenderService, sg.vertBuf);
    INDEX_BUFFER_RELEASE(RenderService, sg.indxBuf);
    TEXTURE_RELEASE(RenderService, texl);
    TEXTURE_RELEASE(RenderService, m_nEmptyGerbTex);
    STORM_DELETE(WindVect);
    m_nMastCreatedCharacter = -1;
    STORM_DELETE(m_sMastName);
}

bool SAIL::Init()
{
    // GUARD(SAIL::SAIL())

    SetDevice();

    // UNGUARD
    return true;
}

void SAIL::SetDevice()
{
    int i;

    mtx.SetIdentity();

    // get render service
    RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!RenderService)
    {
        throw std::runtime_error("No service: dx9render");
    }

    LoadSailIni();

    if (WindVect == nullptr)
    {
        WindVect = new float[WINDVECTOR_QUANTITY];
        if (WindVect)
            // wind vector table calculation
            for (i = 0; i < WINDVECTOR_QUANTITY; i++)
                WindVect[i] = sinf(static_cast<float>(i) / static_cast<float>(WINDVECTOR_QUANTITY) * 2.f * PI);
        else
        {
            throw std::runtime_error("No memory allocation: WindVect");
        }
    }
}

bool SAIL::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool SAIL::LoadState(ENTITY_STATE *state)
{
    return true;
}

void SAIL::Execute(uint32_t Delta_Time)
{
    uint64_t rtime;
    int i;

    // test slaughter of masts
    if (gdata && core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0 &&
        core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0 && core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
    {
        long nTmpMastNum = -1;
        if (core.Controls->GetDebugAsyncKeyState('1') < 0)
            nTmpMastNum = 1;
        else if (core.Controls->GetDebugAsyncKeyState('2') < 0)
            nTmpMastNum = 2;
        else if (core.Controls->GetDebugAsyncKeyState('3') < 0)
            nTmpMastNum = 3;
        else if (core.Controls->GetDebugAsyncKeyState('4') < 0)
            nTmpMastNum = 4;
        else if (core.Controls->GetDebugAsyncKeyState('5') < 0)
            nTmpMastNum = 5;
        else if (core.Controls->GetDebugAsyncKeyState('6') < 0)
            nTmpMastNum = 6;
        else if (core.Controls->GetDebugAsyncKeyState('7') < 0)
            nTmpMastNum = 7;
        else if (core.Controls->GetDebugAsyncKeyState('0') < 0)
            nTmpMastNum = 0;
        if (nTmpMastNum >= 0)
        {
            MODEL *pTmpMdl = nullptr;
            if ((pTmpMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[0].modelEI))) != nullptr)
            {
                char pcTmpMastName[256];
                sprintf_s(pcTmpMastName, "mast%d", nTmpMastNum);
                auto *nod = pTmpMdl->FindNode(pcTmpMastName);
                if (nod)
                {
                    entid_t eiMastTmp;
                    if (eiMastTmp = EntityManager::CreateEntity("MAST"))
                    {
                        core.Send_Message(eiMastTmp, "lpii", MSG_MAST_SETGEOMETRY, nod, gdata[0].shipEI,
                                          gdata[0].modelEI);
                    }
                }
            }
        }
    }

    if (bFirstRun)
    {
        FirstRun();
        wFirstIndx = sailQuantity;
    }
    if (bDeleteState)
    {
        DeleteSailGroup();
        wFirstIndx = sailQuantity;
    }

    auto fMaxTurnAngl = Delta_Time * TURNSTEPANGL;

    auto bSailUpdate = false;
    m_nLastUpdate -= Delta_Time;
    if (m_nLastUpdate <= 0)
    {
        m_nLastUpdate = GROUP_UPDATE_TIME;
        bSailUpdate = true;
    }

    if (bUse)
    {
        // ====================================================
        // If the ini-file has been changed, read the info from it
        if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
        {
            auto ft_new = fio->_GetLastWriteTime(RIGGING_INI_FILE);
            if (ft_old != ft_new)
            {
                int oldWindQnt = WINDVECTOR_QUANTITY;
                LoadSailIni();
                if (oldWindQnt != WINDVECTOR_QUANTITY) // if changed the size of the wind table
                {
                    STORM_DELETE(WindVect);
                    WindVect = new float[WINDVECTOR_QUANTITY];
                    if (WindVect) // wind vector table calculation
                        for (i = 0; i < WINDVECTOR_QUANTITY; i++)
                            WindVect[i] =
                                sinf(static_cast<float>(i) / static_cast<float>(WINDVECTOR_QUANTITY) * 2.f * PI);
                    else
                    {
                        throw std::runtime_error("No memory allocation: WindVect");
                    }
                }
                for (i = 0; i < sailQuantity; i++)
                {
                    slist[i]->MaxSumWind = slist[i]->sailHeight * MAXSUMWIND;
                    // correct the current indices for the new size of the vector table
                    while (slist[i]->VertIdx >= WINDVECTOR_QUANTITY)
                        slist[i]->VertIdx -= WINDVECTOR_QUANTITY;
                    while (slist[i]->HorzIdx >= WINDVECTOR_QUANTITY)
                        slist[i]->HorzIdx -= WINDVECTOR_QUANTITY;
                }
            }
        }

        // get the wind value
        if (const auto ei = EntityManager::GetEntityId("weather"))
        {
            auto wb = static_cast<WEATHER_BASE *>(EntityManager::GetEntityPointer(ei));
            globalWind.ang.x = wb->GetFloat(whf_wind_angle);
            globalWind.ang.z = cosf(globalWind.ang.x);
            globalWind.ang.x = sinf(globalWind.ang.x);
            globalWind.base = wb->GetFloat(whf_wind_speed) / WIND_SPEED_MAX;
            if (globalWind.base > 1.f)
                globalWind.base = 1.f;
        }

        // set a box of zero size to the center of the groups and the parameters of wind calculation
        for (i = 0; i < groupQuantity; i++)
        {
            if (gdata[i].bDeleted)
                continue;
            MODEL *cmod;
            cmod = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[i].modelEI));
            if (cmod == nullptr)
                continue;
            gdata[i].boxCenter = gdata[i].boxSize = slist[gdata[i].sailIdx[0]]->ss.boundSphere.rc;
            // CVECTOR(0.f,0.f,0.f);
            gdata[i].speed_c = 0.f;
            gdata[i].maxSpeed = 0.f;
            gdata[i].curHole = 0;
            gdata[i].bFinalSailDoOld = gdata[i].bFinalSailDo;
            gdata[i].bFinalSailDo = false;
            VDATA *pvdat = core.Event("evntGetSRollSpeed", "l", GetCharacterForGroup(i));
            if (pvdat == nullptr)
                gdata[i].fRollingSpeed = ROLLINGSPEED;
            else
                gdata[i].fRollingSpeed = pvdat->GetFloat() * ROLLINGSPEED;
        }

        CVECTOR pos, ang;
        float perspect;
        RDTSC_B(rtime);
        RenderService->GetCamera(pos, ang, perspect);
        CMatrix tmpMtx;
        tmpMtx.BuildMatrix(ang);
        CVECTOR vCamDirect = tmpMtx * CVECTOR(0, 0, 1.f);
        float minCos = cosf(atanf(perspect));

        for (i = 0; i < sailQuantity; i++)
        {
            if (gdata[slist[i]->HostNum].bDeleted)
                continue;
            // fold/unfold sail as needed
            if (slist[i]->sroll && !slist[i]->bFreeSail)
                slist[i]->DoRollingStep(Delta_Time);
            if (slist[i]->sroll && !slist[i]->bFreeSail)
                gdata[slist[i]->HostNum].bFinalSailDo = true;
            // If the mesh on the sail has changed, then set new indices
            slist[i]->GetGrid(pos, perspect);
            // wind calculation
            slist[i]->CalculateSailWind();
            // turn the sail according to need
            if (slist[i]->ss.turningSail)
                slist[i]->TurnSail(fMaxTurnAngl);
            // calculation of the influence of the state of the sails on the speed of the ship (from 0 to 1)
            gdata[slist[i]->HostNum].speed_c += slist[i]->curSpeed;
            gdata[slist[i]->HostNum].curHole += slist[i]->GetMaxHoleCount() - slist[i]->ss.holeCount;
        }

        auto *pv = static_cast<SAILVERTEX *>(RenderService->LockVertexBuffer(sg.vertBuf));
        if (pv)
        {
            for (i = 0; i < sailQuantity; i++)
            {
                if (gdata[slist[i]->HostNum].bDeleted)
                    continue;
                // make sails sway
                slist[i]->goWave(&pv[slist[i]->ss.sVert], Delta_Time);
                // set rolling / unrolling of sails
                //                if( gdata[slist[i]->HostNum].bFinalSailDo )
                //                    if(!slist[i]->ss.rollingSail)
                //                        slist[i]->SetRolling(gdata[slist[i]->HostNum].bFinalSailUp);
                // sail bounding box calculation
                CVECTOR vtmp = slist[i]->ss.boundSphere.rc - slist[i]->ss.boundSphere.r;
                int itmp = slist[i]->HostNum;
                if (gdata[itmp].boxCenter.x > vtmp.x)
                    gdata[itmp].boxCenter.x = vtmp.x;
                if (gdata[itmp].boxCenter.y > vtmp.y)
                    gdata[itmp].boxCenter.y = vtmp.y;
                if (gdata[itmp].boxCenter.z > vtmp.z)
                    gdata[itmp].boxCenter.z = vtmp.z;

                vtmp = slist[i]->ss.boundSphere.rc + slist[i]->ss.boundSphere.r;
                if (gdata[itmp].boxSize.x < vtmp.x)
                    gdata[itmp].boxSize.x = vtmp.x;
                if (gdata[itmp].boxSize.y < vtmp.y)
                    gdata[itmp].boxSize.y = vtmp.y;
                if (gdata[itmp].boxSize.z < vtmp.z)
                    gdata[itmp].boxSize.z = vtmp.z;
            }

            RenderService->UnLockVertexBuffer(sg.vertBuf);
        }

        // resetting the rolling/unrolling sails flag for all ships and calculating the speed
        for (i = 0; i < groupQuantity; i++)
        {
            if (gdata[i].bDeleted)
                continue;

            // the sound of setting the sail
            if (gdata[i].bFinalSailDoOld != gdata[i].bFinalSailDo)
            {
                if (gdata[i].bYesShip)
                {
                    ATTRIBUTES *pA =
                        static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[i].shipEI))->GetACharacter();
                    core.Event("Ship_SailsMoveSound", "al", pA, static_cast<long>(gdata[i].bFinalSailDo));
                }
            }

            float fSP = static_cast<float>(gdata[i].maxSP);
            float curSP = (float)gdata[i].maxSP;
            float fSDmg = 0.0;
            float fSPow = 0.0;
            if (gdata[i].speed_m < 0.001f)
            {
                gdata[i].shipSpeed = 0.f;
                gdata[i].maxSpeed = 0.f;
            }
            else
            {
                gdata[i].shipSpeed = gdata[i].speed_c / gdata[i].speed_m;
                gdata[i].maxSpeed /= gdata[i].speed_m;

                if (gdata[i].maxHole != 0)
                {
                    fSP = (float)gdata[i].maxSP * gdata[i].curHole / gdata[i].maxHole;
                    curSP = 0.0;
                    for (int j = 0; j < gdata[i].sailQuantity; j++)
                    {
                        int sn = gdata[i].sailIdx[j];
                        fSPow = (float)gdata[i].maxSP * (float)slist[sn]->maxSpeed / gdata[i].speed_m;
                        fSDmg += fSPow * slist[sn]->ss.holeCount / slist[sn]->GetMaxHoleCount();
                        curSP += fSPow;
                    }
                    curSP -= fSDmg;
                }
                if (fSP > gdata[i].maxSP)
                    fSP = (float)gdata[i].maxSP;
                if (curSP > gdata[i].maxSP)
                    curSP = (float)gdata[i].maxSP;
            }
            gdata[i].boxCenter = (gdata[i].boxCenter + gdata[i].boxSize) * .5f;
            gdata[i].boxSize -= gdata[i].boxCenter;
            gdata[i].boxRadius = sqrtf(~gdata[i].boxSize);

            if (gdata[i].bYesShip)
            {
                auto *pVai = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[i].shipEI));
                if (pVai != nullptr && pVai->GetACharacter() != nullptr)
                {
                    ATTRIBUTES *pA = pVai->GetACharacter()->GetAttributeClass("Ship");
                    if (pA != nullptr)
                        pA->SetAttributeUseDword("SP", fftoi(curSP));
                    // pA->SetAttributeUseDword("SP", (long)fSP);
                }
            }
        }
        // initializing ray tracing parameters
        LastTraceGroup = 0;
        RDTSC_E(rtime);
        tm.idx = rtime;
    }
}

void SAIL::Realize(uint32_t Delta_Time)
{
    uint32_t dwOldTextureFactor;
    int i, j, idx;
    if (bUse)
    {
        bool bDraw = RenderService->TechniqueExecuteStart("ShipSail");
        if (!bDraw)
            return;
        RenderService->SetMaterial(mat);
        RenderService->TextureSet(2, texl);
        CMatrix matv, matp, matc;
        RenderService->GetTransform(D3DTS_VIEW, (D3DXMATRIX *)&matv);
        RenderService->GetTransform(D3DTS_PROJECTION, (D3DXMATRIX *)&matp);
        matc = matv * matp;
        if constexpr (false) // Delta_Time==0 )
        {
            for (j = 0; j < groupQuantity; j++)
            {
                if (gdata[j].bDeleted)
                    continue;
                RenderService->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);
                RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, gdata[j].dwSailsColor);
                for (idx = 0; idx < gdata[j].sailQuantity; idx++)
                {
                    i = gdata[j].sailIdx[idx];
                    if (slist[i]->bFreeSail)
                        continue;
                    // if(gdata[slist[i]->HostNum].bDeleted) continue;
                    RenderService->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)slist[i]->pMatWorld);
                    RenderService->TextureSet(0, slist[i]->surfaceTex);
                    if (slist[i]->m_bIsGerald)
                    {
                        if (slist[i]->m_pGeraldTex)
                            RenderService->SetTexture(1, slist[i]->m_pGeraldTex);
                        else
                            RenderService->TextureSet(1, slist[i]->m_nGeraldTex);
                    }
                    else
                        RenderService->TextureSet(1, m_nEmptyGerbTex);
                    // Draw hole sail
                    if (slist[i]->ss.Nh != 0)
                    {
                        RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf, slist[i]->ss.sVert,
                                                  slist[i]->ss.nVert, slist[i]->ss.shi, slist[i]->ss.Nh);
                    }
                    // Draw normal sail
                    if (slist[i]->ss.Nn != 0)
                    {
                        RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf, slist[i]->ss.sVert,
                                                  slist[i]->ss.nVert, slist[i]->ss.sni, slist[i]->ss.Nn);
                    }
                }
                RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);
            }
        }
        else
        {
            for (j = 0; j < groupQuantity; j++)
            {
                if (gdata[j].bDeleted)
                    continue;
                RenderService->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);
                RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, gdata[j].dwSailsColor);
                for (idx = 0; idx < gdata[j].sailQuantity; idx++)
                {
                    i = gdata[j].sailIdx[idx];
                    // if(gdata[slist[i]->HostNum].bDeleted) continue;
                    RenderService->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)slist[i]->pMatWorld);
                    RenderService->TextureSet(0, slist[i]->surfaceTex);
                    if (slist[i]->m_bIsGerald)
                    {
                        if (slist[i]->m_pGeraldTex)
                            RenderService->SetTexture(1, slist[i]->m_pGeraldTex);
                        else
                            RenderService->TextureSet(1, slist[i]->m_nGeraldTex);
                    }
                    else
                        RenderService->TextureSet(1, m_nEmptyGerbTex);
                    // Draw hole texture sail
                    uint32_t dwOld;
                    RenderService->GetSamplerState(2, D3DSAMP_ADDRESSU, &dwOld);
                    RenderService->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
                    // slist[i]->FillIndex(pt);
                    auto *pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(sg.indxBuf, D3DLOCK_DISCARD));
                    if (pt)
                        slist[i]->FillIndex(pt);
                    RenderService->UnLockIndexBuffer(sg.indxBuf);
                    if (gdata[j].bYesShip)
                    {
                        static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[j].shipEI))
                            ->SetLightAndFog(true);
                        static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[j].shipEI))->SetLights();
                    }
                    if (slist[i]->ss.nholeIndx != 0)
                    {
                        RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf, slist[i]->ss.sVert,
                                                  slist[i]->ss.nVert, slist[i]->ss.sholeIndx, slist[i]->ss.nholeIndx);
                    }
                    // Draw normal texture sail
                    RenderService->SetSamplerState(2, D3DSAMP_ADDRESSU, dwOld);
                    if (slist[i]->ss.nnormIndx != 0)
                    {
                        RenderService->DrawBuffer(sg.vertBuf, sizeof(SAILVERTEX), sg.indxBuf, slist[i]->ss.sVert,
                                                  slist[i]->ss.nVert, slist[i]->ss.sIndx, slist[i]->ss.nnormIndx);
                    }
                    if (gdata[j].bYesShip)
                    {
                        static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[j].shipEI))->UnSetLights();
                        static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[j].shipEI))
                            ->RestoreLightAndFog();
                    }
                }
                RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);
            }

            //_asm rdtsc    _asm sub eax,tm_draw _asm mov tm_draw,eax
        }
        while (RenderService->TechniqueExecuteNext())
        {
        }
    }
}

uint64_t SAIL::ProcessMessage(MESSAGE &message)
{
    // GUARD(uint32_t SAIL::ProcessMessage(MESSAGE message))
    entid_t tmpEI;
    int i;

    switch (message.Long())
    {
    case MSG_SAIL_INIT: // format "liil" (code,shipEntity,modelEntity,bSailUp)
    {
        // resize group data block
        if (gdata)
        {
            GROUPDATA *oldgdata = gdata;
            gdata = new GROUPDATA[groupQuantity + 1];
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
            oldgdata = nullptr;
            groupQuantity++;
        }
        else
        {
            gdata = new GROUPDATA[1];
            groupQuantity = 1;
            sailQuantity = 0;
            slist = nullptr;
        }
        // Set new data into buffer
        gdata[groupQuantity - 1].bDeleted = false;
        gdata[groupQuantity - 1].bYesShip = true;
        gdata[groupQuantity - 1].shipEI = message.EntityID();
        gdata[groupQuantity - 1].modelEI = message.EntityID();
        gdata[groupQuantity - 1].shipSpeed = 0.f;
        gdata[groupQuantity - 1].fSpeedMul = 1.f;
        gdata[groupQuantity - 1].speed_m = 0;
        gdata[groupQuantity - 1].speed_c = 0;
        gdata[groupQuantity - 1].fRollingSpeed = ROLLINGSPEED;
        gdata[groupQuantity - 1].dwSailsColor = 0xFFFFFFFF;
        gdata[groupQuantity - 1].maxSP = 100;
        bool bSailUp = (message.Long() != 0);
        if (bSailUp)
        {
            gdata[groupQuantity - 1].bFinalSailDoOld = gdata[groupQuantity - 1].bFinalSailDo = false;
            gdata[groupQuantity - 1].bFinalSailUp = false;
            gdata[groupQuantity - 1].curSailSet = ROLLTYPE_FAST;
        }
        else
        {
            gdata[groupQuantity - 1].bFinalSailDoOld = gdata[groupQuantity - 1].bFinalSailDo = false;
            gdata[groupQuantity - 1].bFinalSailUp = false;
            gdata[groupQuantity - 1].curSailSet = ROLLTYPE_NONE;
        }

        MODEL *mdl;
        if ((mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[groupQuantity - 1].modelEI))) != nullptr)
        {
            GEOS::INFO gi;
            GEOS::LABEL gl;
            for (int j = 0; true; j++)
            {
                NODE *nod = mdl->GetNode(j);
                if (nod == nullptr || nod->geo == nullptr)
                    break;

                nod->geo->GetInfo(gi);
                for (i = 0; i < gi.nlabels; i++)
                {
                    nod->geo->GetLabel(i, gl);

                    if (!strncmp(gl.name, "sail", 4))
                        AddSailLabel(gl, nod, bSailUp);
                }
            }
        }
        else
        {
            core.Trace("WARNING! Can`t model class pointer for ShipModel");
        }
        // Set all getting sails
        SetAllSails(groupQuantity - 1);
        bFirstRun = true;
    }
    break;

        // Do rope tie
    case MSG_SAIL_ROPE_TIE: // format "lplpl" (code,&NODE,groupNum,&position,ropenum)
        int iMax, posNum;
        float posDelta, curDelta;
        NODE *nod;
        nod = (NODE *)message.Pointer();
        int gNum;
        gNum = message.Long();
        CVECTOR *pos;
        pos = (CVECTOR *)message.Pointer();
        if (pos == nullptr)
            break;
        int ropenum;
        ropenum = message.Long();

        for (i = 0; i < sailQuantity; i++)
        {
            if (slist[i]->hostNode == nod && slist[i]->groupNum == gNum)
            {
                SAILONE *so = slist[i];
                if (so->ss.eSailType == SAIL_TREANGLE)
                    iMax = 3;
                else
                    iMax = 4;
                posDelta = 1000.f;
                for (int j = posNum = 0; j < iMax; j++)
                {
                    curDelta = ~(so->ss.hardPoints[j] - *pos);
                    if (curDelta < posDelta)
                    {
                        posDelta = curDelta;
                        posNum = j;
                    }
                }
                so->sailtrope.pnttie[posNum] = true;
                so->sailtrope.pPos[posNum] = pos;

                CVECTOR epos;
                if (so->ss.turningSail && posNum != 0) // setting for turning sails only
                    if (tmpEI = EntityManager::GetEntityId("rope"))
                        if (so->sailtrope.rrs[0] == nullptr)
                        {
                            so->sailtrope.rrs[0] = new ROTATEROPEDSAIL;
                            so->sailtrope.rrs[0]->ropenum = ropenum;
                            so->sailtrope.rrs[0]->tiePoint = posNum;
                            CVECTOR bpos;
                            switch (posNum)
                            {
                            case 1:
                                bpos = so->ss.hardPoints[0];
                                break;
                            case 2:
                                bpos = so->ss.hardPoints[0];
                                break;
                            case 3:
                                bpos = so->ss.hardPoints[1];
                                break;
                            }
                            static_cast<ROPE_BASE *>(EntityManager::GetEntityPointer(tmpEI))
                                ->GetEndPoint(&epos, so->sailtrope.rrs[0]->ropenum, gdata[so->HostNum].modelEI);
                            so->sailtrope.rrs[0]->r1 = sqrtf(~(*pos - bpos));
                            so->sailtrope.rrs[0]->r2 = sqrtf(~(*pos - epos));
                            if (so->ss.eSailType != SAIL_TREANGLE)
                            {
                                CVECTOR tmpv;
                                tmpv = so->sgeo.dVv + so->sgeo.ddVv * static_cast<float>(SAIL_ROW_MAX);
                                tmpv = so->sgeo.dVv + so->sgeo.ddVv * static_cast<float>(SAIL_ROW_MAX);
                                tmpv = (so->sailtrope.rrs[0]->r1) * (!tmpv);
                                tmpv = *pos - tmpv;

                                so->sailtrope.rrs[0]->b = tmpv;
                            }
                        }
                        else if (so->sailtrope.rrs[1] == nullptr)
                        {
                            so->sailtrope.rrs[1] = new ROTATEROPEDSAIL;
                            so->sailtrope.rrs[1]->ropenum = ropenum;
                            so->sailtrope.rrs[1]->tiePoint = posNum;
                            CVECTOR bpos;
                            switch (posNum)
                            {
                            case 1:
                                bpos = so->ss.hardPoints[0];
                                break;
                            case 2:
                                bpos = so->ss.hardPoints[0];
                                break;
                            case 3:
                                bpos = so->ss.hardPoints[1];
                                break;
                            }
                            so->sailtrope.rrs[1]->r1 = sqrtf(~(*pos - bpos));
                            static_cast<ROPE_BASE *>(EntityManager::GetEntityPointer(tmpEI))
                                ->GetEndPoint(&epos, so->sailtrope.rrs[1]->ropenum, gdata[so->HostNum].modelEI);
                            so->sailtrope.rrs[1]->r2 = sqrtf(~(*pos - epos));
                            if (so->ss.eSailType != SAIL_TREANGLE)
                            {
                                CVECTOR tmpv;
                                tmpv = so->sgeo.dVv + so->sgeo.ddVv * static_cast<float>(SAIL_ROW_MAX);
                                tmpv = (so->sailtrope.rrs[1]->r1) * (!tmpv);
                                tmpv = *pos - tmpv;

                                so->sailtrope.rrs[1]->b = tmpv;
                            }
                        }
            }
        }
        break;

        // get the speed of the ship from the sails
    case MSG_SAIL_GET_SPEED:
        tmpEI = message.EntityID();
        float *speedPtr;
        speedPtr = (float *)message.Pointer();
        float maxVal;
        maxVal = message.Float();
        if (speedPtr)
        {
            for (i = 0; i < groupQuantity; i++)
                if (tmpEI == gdata[i].shipEI)
                {
                    if (gdata[i].bDeleted)
                    {
                        *speedPtr = 0;
                        break;
                    }
                    if (gdata[i].curSailSet != ROLLTYPE_NONE && gdata[i].shipSpeed < m_fMinSpeedVal)
                        *speedPtr = m_fMinSpeedVal;
                    else
                        *speedPtr = gdata[i].shipSpeed;

                    bool yesChange = false;
                    int rt = ROLLTYPE_FAST;
                    if (maxVal < 0.3f)
                    {
                        if (gdata[i].curSailSet != ROLLTYPE_NONE)
                        {
                            yesChange = true;
                            rt = ROLLTYPE_NONE;
                        }
                    }
                    else if (maxVal > 0.7f)
                    {
                        if (gdata[i].curSailSet != ROLLTYPE_FAST)
                        {
                            yesChange = true;
                        }
                    }
                    else
                    {
                        if (gdata[i].curSailSet != ROLLTYPE_NORMAL)
                        {
                            yesChange = true;
                            rt = ROLLTYPE_NORMAL;
                        }
                    }

                    // set the selected type of sail position
                    if (yesChange)
                    {
                        float tmpf = 0.f;
                        for (int j = 0; j < gdata[i].sailQuantity; j++)
                        {
                            int snum = gdata[i].sailIdx[j];
                            if (slist[snum]->rollType > rt)
                            {
                                slist[snum]->SetRolling(true);
                                tmpf += slist[snum]->maxSpeed;
                            }
                            else
                            {
                                slist[snum]->SetRolling(false);
                            }
                        }
                        if (tmpf > 0.f)
                            gdata[i].fSpeedMul = maxVal / (tmpf / gdata[i].speed_m);
                        gdata[i].curSailSet = rt;
                    }

                    break;
                }

            if (i == groupQuantity)
            {
                *speedPtr = (maxVal > 0.3f) ? m_fMinSpeedVal : 0.f; // no ship in sail groups - speed 0
            }
        }
        break;

    case MSG_SAIL_GET_PARAMS: // "lip"
    {
        entid_t shipEI = message.EntityID();
        auto pMaxSpeed = (float *)message.Pointer();
        // find the required group of sails
        int gn;
        for (gn = 0; gn < groupQuantity; gn++)
            if (gdata[gn].shipEI == shipEI)
                break;
        // write the value for it by the parameter pointer
        if (pMaxSpeed)
            if (gn < groupQuantity)
                *pMaxSpeed = gdata[gn].maxSpeed;
            else
                *pMaxSpeed = 0.f;
    }
    break;

        // transfer the sail to another group
    case MSG_SAIL_TO_NEWHOST: {
        entid_t oldModelEI = message.EntityID();
        auto nod = (NODE *)message.Pointer();
        long groupNum = message.Long();
        entid_t newHostEI = message.EntityID();
        entid_t newModelEI = message.EntityID();
        DoSailToNewHost(newModelEI, newHostEI, groupNum, nod, oldModelEI);
    }
    break;
    case MSG_SAIL_CHECK: {
        entid_t oldHostEI = message.EntityID();
        entid_t newHostEI = message.EntityID();
        entid_t newModelEI = message.EntityID();
        DoNoRopeSailToNewHost(newModelEI, newHostEI, oldHostEI);
    }
    break;

        // delete sail group
    case MSG_SAIL_DEL_GROUP:
        tmpEI = message.EntityID(); // get the Entity of the group owner
        for (i = 0; i < groupQuantity; i++)
            if (tmpEI == gdata[i].shipEI)
            {
                gdata[i].bDeleted = true;
                bDeleteState = true;
                break;
            }
        break;

        // release a group of sails from standard inflation
    case MSG_SAIL_FREE_GROUP:
        tmpEI = message.EntityID(); // Group owner entity
        for (i = 0; i < groupQuantity; i++)
            if (tmpEI == gdata[i].shipEI)
            {
                if (gdata[i].bDeleted)
                    break;
                for (int j = 0; j < gdata[i].sailQuantity; j++)
                {
                    int sn = gdata[i].sailIdx[j];
                    slist[sn]->bFreeSail = true;
                }
                break;
            }
        break;

        // start / end attaching sails to the drop mast
    case MSG_SAIL_MAST_PROCESSING:
        STORM_DELETE(m_sMastName);
        m_nMastCreatedCharacter = message.Long();
        if (m_nMastCreatedCharacter != -1)
        {
            const std::string &param = message.String();
            if (!param.empty())
            {
                if ((m_sMastName = new char[param.size() + 1]) == nullptr)
                {
                    throw std::runtime_error("allocate memory error");
                }
                strcpy_s(m_sMastName, param.size() + 1, param.c_str());
            }
        }
        break;

        // script message
    case MSG_SAIL_SCRIPT_PROCESSING: {
        const std::string &param = message.String();
        return ScriptProcessing(param.c_str(), message);
    }
    break;
    }

    // UNGUARD
    return 0;
}

void SAIL::AddSailLabel(GEOS::LABEL &lbl, NODE *nod, bool bSailUp)
{
    SAILONE *cs;
    int grNum, i;
    grNum = atoi(&lbl.group_name[5]);
    for (i = 0; i < sailQuantity; i++)
        if (slist[i]->hostNode == nod && slist[i]->groupNum == grNum)
        {
            cs = slist[i];
            break;
        }

    if (i == sailQuantity) // there is no such sail - need to create a new one
    {
        // create new sail data and fill that
        if (slist)
        {
            SAILONE **oldslist = slist;
            slist = new SAILONE *[sailQuantity + 1];
            memcpy(slist, oldslist, sizeof(SAILONE *) * sailQuantity);
            delete oldslist;
        }
        else
            slist = new SAILONE *[1];
        cs = slist[sailQuantity] = new SAILONE;
        sailQuantity++;
        cs->pp = this;
        cs->HostNum = groupQuantity - 1;
        cs->hostNode = nod;
        cs->groupNum = grNum; // write identify info
        cs->pMatWorld = &nod->glob_mtx;
        cs->curSpeed = 0;
        cs->sailWind = CVECTOR(0.f, 0.f, 0.f);
        cs->bRolling = !bSailUp;

        cs->mastNum = 0;
        cs->rollType = ROLLTYPE_NORMAL;
        // get the number of the mast to which the sail is attached (0-no mast)
        if (!strncmp(nod->GetName(), "rey", 3))
        {
            if (nod->parent)
                if (!strncmp(nod->parent->GetName(), "mast", 4))
                    cs->mastNum = atoi(&nod->parent->GetName()[4]);
            if (nod->GetName()[4] == 'a' || cs->mastNum == 1)
                cs->rollType = ROLLTYPE_FAST;
        }

        switch (lbl.group_name[4])
        {
        case 't': // triangle sail
            cs->ss.eSailType = SAIL_TREANGLE;
            cs->ss.texNum = 1;
            cs->ss.rollingSail = false;
            break;
        case 's': // square sail
            cs->ss.eSailType = SAIL_SQUARE;
            cs->ss.texNum = 2;
            cs->ss.rollingSail = true;
            break;
        case 'g': // square sail with gerb picture
            cs->ss.eSailType = SAIL_SQUARE;
            cs->ss.texNum = 0;
            cs->ss.rollingSail = true;
            break;
        case 'f': // trapecidal sail
            cs->ss.eSailType = SAIL_TRAPECIDAL;
            cs->ss.texNum = 2;
            cs->ss.rollingSail = false;
            break;
        case 'd': // rolling square sail
            cs->ss.eSailType = SAIL_SQUARE;
            cs->ss.rollingSail = true;
            cs->ss.texNum = 2;
            break;
        case 'x': // special square sail
            cs->ss.eSailType = SAIL_SPECIAL;
            cs->ss.rollingSail = true;
            cs->ss.texNum = 2;
            break;
            // --> ugeen 08.11.10
        case 'v': // triangle sail
            cs->ss.eSailType = SAIL_TREANGLE;
            cs->ss.texNum = 1;
            cs->ss.rollingSail = false;
            cs->rollType = ROLLTYPE_FAST;
            break;
        case 'n': // trapecidal sail
            cs->ss.eSailType = SAIL_TRAPECIDAL;
            cs->ss.texNum = 2;
            cs->ss.rollingSail = false;
            cs->rollType = ROLLTYPE_FAST;
            break;
            // <-- ugeen
        }
        cs->ss.turningSail = !strncmp(nod->GetName(), "rey_", 4);
    }

    CVECTOR vtmp;
    vtmp.x = lbl.m[3][0]; //+nod->glob_mtx.matrix[3];
    vtmp.y = lbl.m[3][1]; //+nod->glob_mtx.matrix[7];
    vtmp.z = lbl.m[3][2]; //+nod->glob_mtx.matrix[11];
    switch (lbl.name[4])
    {
    case '1':
        cs->ss.hardPoints[0] = vtmp;
        break;
    case '2':
        cs->ss.hardPoints[1] = vtmp;
        break;
    case '3':
        if (cs->ss.eSailType == SAIL_TREANGLE)
            cs->ss.hardPoints[2] = vtmp;
        else
            cs->ss.hardPoints[3] = vtmp;
        break;
    case '4':
        cs->ss.hardPoints[2] = vtmp;
        break;
    case '0':
        cs->ss.bYesLimitPoint = true;
        cs->ss.LimitPoint = vtmp;
    }
}

void SAIL::SetAllSails(int groupNum)
{
    gdata[groupNum].bDeleted = false;
    gdata[groupNum].sailQuantity = 0;
    gdata[groupNum].maxHole = 0;

    for (int i = 0; i < sailQuantity; i++)
        if (slist[i]->HostNum == groupNum)
        {
            // Set sail
            slist[i]->RenderService = RenderService;
            if (slist[i]->SetSail())
            {
                gdata[groupNum].sailQuantity++;
                slist[i]->SetGeometry();
                gdata[groupNum].speed_m += slist[i]->maxSpeed;
                gdata[groupNum].maxHole += static_cast<int>(slist[i]->GetMaxHoleCount());
            }
            else
            {
                // throw std::runtime_error("SAIL: Null size");
                core.Trace("SAIL: Can`t init sail");
                STORM_DELETE(slist[i]);
                sailQuantity--;
                if (sailQuantity > 0)
                {
                    SAILONE **oldslist = slist;
                    slist = new SAILONE *[sailQuantity];
                    if (!slist)
                    {
                        slist = oldslist;
                        oldslist = nullptr;
                    }
                    if (i > 0)
                        memcpy(slist, oldslist, sizeof(SAILONE *) * i);
                    if (i < sailQuantity)
                        memcpy(&slist[i], &oldslist[i + 1], sizeof(SAILONE *) * (sailQuantity - i));
                    STORM_DELETE(oldslist);
                }
                else
                {
                    STORM_DELETE(slist);
                }
                i--;
            }
        }

    if (gdata[groupNum].sailQuantity > 0)
    {
        gdata[groupNum].sailIdx = new int[gdata[groupNum].sailQuantity];
        int idx = 0;
        for (int i = 0; i < sailQuantity; i++)
            if (slist[i]->HostNum == groupNum)
            {
                gdata[groupNum].sailIdx[idx++] = i;
            }
        // see the presence of holes
        if (gdata[groupNum].bYesShip && !gdata[groupNum].bDeleted)
        {
            ATTRIBUTES *pACh =
                static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[groupNum].shipEI))->GetACharacter();
            ATTRIBUTES *pA = nullptr;
            // start installing textures on the sails
            SetSailTextures(groupNum, core.Event("GetSailTextureData", "l", pACh->GetAttributeAsDword("index", -1)));
            if (pACh != nullptr)
            {
                pA = pACh->FindAClass(pA, "ship.sails");
                if (pA == nullptr)
                    pA = pACh->CreateSubAClass(pACh, "ship.sails");
            }
            if (pA != nullptr)
            {
                char param[256];
                sprintf_s(param, "%d", gdata[groupNum].maxHole);
                pA->SetValue(param);
                for (int i = 0; i < static_cast<int>(pA->GetAttributesNum()); i++)
                {
                    ATTRIBUTES *pAttr = pA->GetAttributeClass(i);
                    if (pAttr != nullptr)
                        for (int j = 0; j < static_cast<int>(pAttr->GetAttributesNum()); j++)
                        {
                            ATTRIBUTES *pASail = pAttr->GetAttributeClass(j);
                            if (pASail != nullptr)
                            {
                                SAILONE *ps = FindSailFromData(groupNum, pAttr->GetThisName(), pASail->GetThisName());
                                if (ps != nullptr)
                                {
                                    ps->SetAllHole(pASail->GetAttributeAsDword("hd"));
                                    /*if( (ps->ss.eSailType==SAIL_TREANGLE?10:12) == (int)ps->ss.holeCount )
                                    {// the whole sail is damaged - don't need it
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
        if (groupQuantity > 0)
        {
            GROUPDATA *oldgdata = gdata;
            gdata = new GROUPDATA[groupQuantity];
            if (gdata)
            {
                memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
                delete oldgdata;
                oldgdata = nullptr;
            }
            else
                gdata = oldgdata;
        }
        else
        {
            delete gdata;
            gdata = nullptr;
        }
    }
}

void SAIL::SetAllSails()
{
    sg.nVert = 0;
    sg.nIndx = 54;

    for (int i = 0; i < sailQuantity; i++)
    {
        // form the parameters of the vertex and index buffers
        slist[i]->SetTurnLimits();
        slist[i]->ss.sVert = sg.nVert;
        slist[i]->ss.sIndx = sg.nIndx;
        sg.nVert += slist[i]->ss.nVert;
    }
    if (sg.nVert == 0)
        return;
    sg.nIndx += 1152;

    if (texl == -1)
        texl = RenderService->TextureCreate("ships\\parus_hole.tga");
    if (m_nEmptyGerbTex == -1)
        m_nEmptyGerbTex = RenderService->TextureCreate("ships\\emptygerald.tga");

    sg.vertBuf = RenderService->CreateVertexBuffer(SAILVERTEX_FORMAT, sg.nVert * sizeof(SAILVERTEX),
                                                   D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
    sg.indxBuf = RenderService->CreateIndexBuffer(sg.nIndx * 2, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

    SAILVERTEX *pv;
    pv = static_cast<SAILVERTEX *>(RenderService->LockVertexBuffer(sg.vertBuf));
    if (pv)
    {
        for (int i = 0; i < sailQuantity; i++)
        {
            slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
            slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
        }
        RenderService->UnLockVertexBuffer(sg.vertBuf);
    }

    // Set triangle buffer for sea mirror
    uint16_t *pt;
    pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(sg.indxBuf));
    if (pt)
    {
        for (int i = 0; i < sailQuantity; i++)
        {
            slist[i]->FillIndex(pt);
        }

        // for triangle sail
        pt[0] = pt[15] = pt[24] = 0;                                                                               // 0
        pt[1] = pt[3] = pt[6] = pt[12] = pt[16] = pt[21] = pt[25] = SAIL_ROW_MAX / 2 * (SAIL_ROW_MAX / 2 + 1) / 2; // 1
        pt[2] = pt[8] = pt[9] = pt[17] = pt[18] = pt[23] = pt[26] = SAIL_ROW_MAX / 2 * (SAIL_ROW_MAX / 2 + 3) / 2; // 2
        pt[4] = pt[13] = (SAIL_ROW_MAX - 1) * SAIL_ROW_MAX / 2;                                                    // 3
        pt[5] = pt[7] = pt[10] = pt[14] = pt[19] = pt[22] =
            (SAIL_ROW_MAX - 1) * SAIL_ROW_MAX / 2 + SAIL_ROW_MAX / 2;  // 4
        pt[11] = pt[20] = (SAIL_ROW_MAX - 1) * (SAIL_ROW_MAX + 2) / 2; // 5

        // for square sail
        pt[27] = pt[42] = pt[51] = 0;                                                                             // 0
        pt[28] = pt[31] = pt[40] = pt[43] = pt[52] = SAIL_ROW_MAX * SAIL_COL_MAX - SAIL_ROW_MAX;                  // 3
        pt[29] = pt[30] = pt[33] = pt[39] = pt[44] = pt[48] = pt[53] = SAIL_ROW_MAX / 2;                          // 1
        pt[32] = pt[34] = pt[37] = pt[41] = pt[46] = pt[49] = SAIL_ROW_MAX * SAIL_COL_MAX - SAIL_ROW_MAX / 2 - 1; // 4
        pt[35] = pt[36] = pt[45] = pt[50] = SAIL_ROW_MAX - 1;                                                     // 2
        pt[38] = pt[47] = SAIL_ROW_MAX * SAIL_COL_MAX - 1;                                                        // 5

        RenderService->UnLockIndexBuffer(sg.indxBuf);
    }
}

void SAIL::LoadSailIni()
{
    // GUARD(SAIL::LoadSailIni());
    char section[256], param[256];

    if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
    {
        ft_old = fio->_GetLastWriteTime(RIGGING_INI_FILE);
    }
    auto ini = fio->OpenIniFile("resource\\ini\\rigging.ini");
    if (!ini)
    {
        throw std::runtime_error("rigging.ini file not found!");
    }

    sprintf(section, "SAILS");

    // load texture names
    texQuantity = static_cast<int>(ini->GetLong(section, "TextureCount", 1));
    if (texQuantity == 0)
    {
        texQuantity = 1;
    }
    texNumCommon = static_cast<float>(ini->GetLong(section, "TexNumCommon", 0)) / static_cast<float>(texQuantity);
    texNumEnglish = static_cast<float>(ini->GetLong(section, "TexNumEnglish", 0)) / static_cast<float>(texQuantity);
    texNumTreangle = static_cast<float>(ini->GetLong(section, "TexNumTreangle", 0)) / static_cast<float>(texQuantity);

    // load speed calculate parameters:
    g_fSailHoleDepend = ini->GetFloat(section, "fHoleDepend", 1.0f);
    ini->ReadString(section, "TreangleWindSpeed", param, sizeof(param) - 1, "0.2,0.6,0.8");
    sscanf(param, "%f,%f,%f", &ts_min, &ts_xdep, &ts_zdep);
    ini->ReadString(section, "TrapecidalWindSpeed", param, sizeof(param) - 1, "0.4,0.5,0.6");
    sscanf(param, "%f,%f,%f", &fs_min, &fs_xdep, &fs_zdep);
    ini->ReadString(section, "SquareWindSpeed", param, sizeof(param) - 1, "0.4,0.1,0.6");
    sscanf(param, "%f,%f,%f", &ss_min, &ss_xdep, &ss_zdep);

    // load wind depend parameters
    SsailWindDepend = ini->GetFloat(section, "fSsailWindDepend", .05f);
    TsailWindDepend = ini->GetFloat(section, "fTsailWindDepend", .5f);
    fWindAdding = ini->GetFloat(section, "fWindAdding", .3f);
    FLEXSPEED = ini->GetFloat(section, "FLEXSPEED", .001f);
    MAXSUMWIND = ini->GetFloat(section, "MAXSUMWIND", .02f);
    WINDVECTOR_QUANTITY = static_cast<int>(ini->GetLong(section, "WINDVECTOR_QNT", 60));
    WINDVECTOR_TINCR = static_cast<int>(ini->GetLong(section, "WINDVECTOR_TINCR", 3));
    WINDVECTOR_TADD = static_cast<int>(ini->GetLong(section, "WINDVECTOR_TADD", 3));
    WINDVECTOR_SINCR = static_cast<int>(ini->GetLong(section, "WINDVECTOR_SINCR", 6));
    WINDVECTOR_SADD = static_cast<int>(ini->GetLong(section, "WINDVECTOR_SADD", 3));

    // load rolling sail parameters
    ROLL_Z_VAL = ini->GetFloat(section, "ROLL_Z_VAL", .01f);
    ROLL_Z_DELTA = ini->GetFloat(section, "ROLL_Z_DELTA", .001f);
    ROLL_Y_VAL = ini->GetFloat(section, "ROLL_Y_VAL", .04f);
    ROLL_Y_DELTA = ini->GetFloat(section, "ROLL_Y_DELTA", .001f);

    // sail turning parameters
    WINDANGL_DISCRETE = ini->GetFloat(section, "WINDANGLDISCRETE", .01f);
    MAXTURNANGL = ini->GetFloat(section, "MAXTURNANGL", .6f);
    TURNSTEPANGL = ini->GetFloat(section, "TURNSTEPANGL", .002f);
    ROLLINGSPEED = ini->GetFloat(section, "ROLLINGSPEED", .0003f);

    // load material parameters
    ini->ReadString(section, "Diffuse", param, sizeof(param) - 1, "0.0,0.0,0.0,0.0");
    sscanf(param, "%f,%f,%f,%f", &mat.Diffuse.r, &mat.Diffuse.g, &mat.Diffuse.b, &mat.Diffuse.a);
    ini->ReadString(section, "Ambient", param, sizeof(param) - 1, "0.0,0.0,0.0,0.0");
    sscanf(param, "%f,%f,%f,%f", &mat.Ambient.r, &mat.Ambient.g, &mat.Ambient.b, &mat.Ambient.a);
    ini->ReadString(section, "Specular", param, sizeof(param) - 1, "0.0,0.0,0.0,0.0");
    sscanf(param, "%f,%f,%f,%f", &mat.Specular.r, &mat.Specular.g, &mat.Specular.b, &mat.Specular.a);
    ini->ReadString(section, "Emissive", param, sizeof(param) - 1, "0.7,0.7,0.7,0.7");
    sscanf(param, "%f,%f,%f,%f", &mat.Emissive.r, &mat.Emissive.g, &mat.Emissive.b, &mat.Emissive.a);
    mat.Power = ini->GetFloat(section, "Power", 0.f);

    // load ROLLING SAIL form table
    // load square sail form
    ini->ReadString(section, "rollSSailForm", param, sizeof(param) - 1,
                    "0.2,0.8,1.0,0.8,0.4,1.0,1.3,1.0,0.4,0.8,1.0,0.8,0.2");
    sscanf(param, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &SSailRollForm[0], &SSailRollForm[1], &SSailRollForm[2],
           &SSailRollForm[3], &SSailRollForm[4], &SSailRollForm[5], &SSailRollForm[6], &SSailRollForm[7],
           &SSailRollForm[8], &SSailRollForm[9], &SSailRollForm[10], &SSailRollForm[11], &SSailRollForm[12]);
    // load treangle sail form
    ini->ReadString(section, "rollTSailForm", param, sizeof(param) - 1, "0.1,0.6,0.3,0.8,0.2");
    sscanf(param, "%f,%f,%f,%f,%f", &TSailRollForm[0], &TSailRollForm[1], &TSailRollForm[2], &TSailRollForm[3],
           &TSailRollForm[4]);
    TR_FORM_MUL = ini->GetFloat(section, "tr_form_mul", 2.f);

    // load hole depend parameters
    fTHoleFlexDepend = ini->GetFloat(section, "fTHoleFlexDepend", .01f);
    if (fTHoleFlexDepend > .1f)
        fTHoleFlexDepend = 0.1f;
    fSHoleFlexDepend = ini->GetFloat(section, "fSHoleFlexDepend", .01f);
    if (fSHoleFlexDepend > (1.f / 12.f))
        fSHoleFlexDepend = 1.f / 12.f;

    // load parameter for sails of fall mast
    // square sails
    FALL_SSAIL_ADD_MIN = ini->GetFloat(section, "fFallSSailAddMin", 0.2f);
    FALL_SSAIL_ADD_RAND = ini->GetFloat(section, "fFallSSailAddRand", 0.2f);
    // treangle sails
    FALL_TSAIL_ADD_MIN = ini->GetFloat(section, "fFallTSailAddMin", 0.2f);
    FALL_TSAIL_ADD_RAND = ini->GetFloat(section, "fFallTSailAddRand", 0.2f);

    GROUP_UPDATE_TIME = ini->GetLong(section, "msecSailUpdateTime", GROUP_UPDATE_TIME);

    // UNGUARD
}

float SAIL::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    float retVal = 2.f;
    if (!bUse)
        return retVal;

    for (int i = 0; i < groupQuantity; i++)
    {
        if (gdata[i].bDeleted)
            continue;
        const CVECTOR minp = gdata[i].boxCenter - gdata[i].boxSize;
        const CVECTOR maxp = gdata[i].boxCenter + gdata[i].boxSize;

        if (!(src.x <= maxp.x && src.x >= minp.x && src.y <= maxp.y && src.y >= minp.y && src.z <= maxp.z &&
              src.z >= minp.z) &&
            !(dst.x <= maxp.x && dst.x >= minp.x && dst.y <= maxp.y && dst.y >= minp.y && dst.z <= maxp.z &&
              dst.z >= minp.z))
        {
            bool bYesTrace = false;
            CVECTOR vmed;
            // section by X:
            if (src.x < minp.x)
            {
                if (dst.x < minp.x)
                    continue;
                vmed = src + (dst - src) * (minp.x - src.x) / (dst.x - src.x);
                if (vmed.y >= minp.y && vmed.y <= maxp.y && vmed.z >= minp.z && vmed.z <= maxp.z)
                    bYesTrace = true;
            }
            if (!bYesTrace && src.x > maxp.x)
            {
                if (dst.x > maxp.x)
                    continue;
                vmed = src + (dst - src) * (maxp.x - src.x) / (dst.x - src.x);
                if (vmed.y >= minp.y && vmed.y <= maxp.y && vmed.z >= minp.z && vmed.z <= maxp.z)
                    bYesTrace = true;
            }
            // section by Z:
            if (!bYesTrace && src.z < minp.z)
            {
                if (dst.z < minp.z)
                    continue;
                vmed = src + (dst - src) * (minp.z - src.z) / (dst.z - src.z);
                if (vmed.y >= minp.y && vmed.y <= maxp.y && vmed.x >= minp.x && vmed.x <= maxp.x)
                    bYesTrace = true;
            }
            if (!bYesTrace && src.z > maxp.z)
            {
                if (dst.z > maxp.z)
                    continue;
                vmed = src + (dst - src) * (maxp.z - src.z) / (dst.z - src.z);
                if (vmed.y >= minp.y && vmed.y <= maxp.y && vmed.x >= minp.x && vmed.x <= maxp.x)
                    bYesTrace = true;
            }
            // section by Y:
            if (!bYesTrace && src.y < minp.y)
            {
                if (dst.y < minp.y)
                    continue;
                vmed = src + (dst - src) * (minp.y - src.y) / (dst.y - src.y);
                if (vmed.z >= minp.z && vmed.z <= maxp.z && vmed.x >= minp.x && vmed.x <= maxp.x)
                    bYesTrace = true;
            }

            if (!bYesTrace)
                continue;
        }

        LastTraceGroup = i;
        // ==============================================
        // the ray passed through the cube around the sails of the ship
        // start looking for the sail which it hit
        // ==============================================
        float tmp;
        traceSail = -1;
        for (int j = 0; j < gdata[i].sailQuantity; j++)
        {
            tmp = slist[gdata[i].sailIdx[j]]->Trace(src, dst, bCannonTrace);
            if (tmp <= 1.f)
                if (bCannonTrace)
                {
                    if (retVal > tmp)
                    {
                        retVal = tmp;
                        traceSail = gdata[i].sailIdx[j];
                    }
                }
                else
                {
                    retVal = tmp;
                    traceSail = gdata[i].sailIdx[j];
                    break;
                }
        }

        if (retVal <= 1.f)
            break; // trace only one ship at a time
    }

    return retVal;
}

const char *SAIL::GetCollideMaterialName()
{
    return "sail";
}

bool SAIL::GetCollideTriangle(struct TRIANGLE &trg)
{
    return false;
}

bool SAIL::Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly)
{
    return false;
}

void SAIL::FirstRun()
{
    int i, sn;

    if (wFirstIndx)
        SetAddSails(wFirstIndx);
    else
        SetAllSails();

    entid_t ropeEI;
    if (ropeEI = EntityManager::GetEntityId("rope"))
        // position calculation according to the position of the ropes
        for (sn = wFirstIndx; sn < sailQuantity; sn++)
        {
            bool bChange = false;
            if (slist[sn]->sroll == nullptr && !slist[sn]->bRolling)
                // recalculate the rope tension if the sail is not in lifting mode
                for (i = 0; i < 2; i++)
                    if (slist[sn]->sailtrope.rrs[i])
                    {
                        const int tieNum = slist[sn]->sailtrope.rrs[i]->tiePoint;
                        CVECTOR endVect;
                        static_cast<ROPE_BASE *>(EntityManager::GetEntityPointer(ropeEI))
                            ->GetEndPoint(&endVect, slist[sn]->sailtrope.rrs[i]->ropenum,
                                          gdata[slist[sn]->HostNum].modelEI);
                        CVECTOR medVect;
                        medVect = slist[sn]->ss.hardPoints[tieNum];
                        CVECTOR begVect;
                        switch (tieNum)
                        {
                        case 1:
                            begVect = slist[sn]->ss.hardPoints[0];
                            break;
                        case 2:
                            begVect = slist[sn]->ss.hardPoints[0];
                            break;
                        case 3:
                            begVect = slist[sn]->ss.hardPoints[1];
                            break;
                        }

                        float r1;
                        r1 = slist[sn]->sailtrope.rrs[i]->r1;
                        float r2;
                        r2 = slist[sn]->sailtrope.rrs[i]->r2;

                        if (slist[sn]->ss.eSailType != SAIL_TREANGLE)
                        {
                            medVect = (endVect * r1 + slist[sn]->sailtrope.rrs[i]->b * r2) / (r1 + r2);
                        }
                        else
                        {
                            medVect.x = (endVect.x * r1 + begVect.x * r2) / (r1 + r2);
                            medVect.z = (endVect.z * r1 + begVect.z * r2) / (r1 + r2);
                        }
                        *slist[sn]->sailtrope.pPos[tieNum] = slist[sn]->ss.hardPoints[tieNum] = medVect;

                        bChange = true;
                    }
            if (bChange)
            {
                slist[sn]->SetGeometry();
                if (slist[sn]->ss.eSailType == SAIL_TREANGLE)
                    slist[sn]->sailHeight = sqrtf(~(slist[sn]->ss.hardPoints[1] - slist[sn]->ss.hardPoints[2]));
                else
                    slist[sn]->sailHeight = sqrtf(~(slist[sn]->ss.hardPoints[0] - slist[sn]->ss.hardPoints[2]));
            }
        }

    bFirstRun = false;
    wFirstIndx = sailQuantity;
    bUse = sailQuantity > 0;
}

float SAIL::Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst)
{
    bCannonTrace = true;
    g_iBallOwnerIdx = iBallOwner;

    const float retVal = Trace(src, dst);

    bCannonTrace = false;
    g_iBallOwnerIdx = -1;

    if (retVal < 2.f)
    {
        if (!slist[traceSail]->bFreeSail && !gdata[slist[traceSail]->HostNum].bDeleted)
        {
            const CVECTOR damagePoint = src + (dst - src) * retVal;
            auto *pvai =
                static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[slist[traceSail]->HostNum].shipEI));
            ATTRIBUTES *pA = nullptr;
            if (pvai != nullptr)
                pA = pvai->GetACharacter();
            long charIdx = -1;
            if (pA != nullptr)
                charIdx = pA->GetAttributeAsDword("index", -1);
            core.Event(SHIP_SAIL_DAMAGE, "lfffl", charIdx, damagePoint.x, damagePoint.y, damagePoint.z, iBallOwner);
        }
    }

    return retVal;
}

void SAIL::DoSailToNewHost(entid_t newModelEI, entid_t newHostEI, int grNum, NODE *nod, entid_t oldModelEI)
{
    if (groupQuantity < 1 || sailQuantity < 1)
        return;

    // find the old owner
    int oldg;
    for (oldg = 0; oldg < groupQuantity; oldg++)
        if (gdata[oldg].modelEI == oldModelEI && !gdata[oldg].bDeleted)
            break;
    if (oldg == groupQuantity)
        return; // no old model - return without doing anything

    // find a sail
    int sn;
    for (sn = 0; sn < sailQuantity; sn++)
        if (slist[sn]->hostNode == nod && slist[sn]->HostNum == oldg && (grNum == 0 || slist[sn]->groupNum == grNum))
            break;
    if (sn == sailQuantity)
        return; // there is no such sail - return without result

    // find a link to our sail in the old owner
    int idx;
    for (idx = 0; idx < gdata[oldg].sailQuantity; idx++)
        if (gdata[oldg].sailIdx[idx] == sn)
            break;
    if (idx == gdata[oldg].sailQuantity)
        return; // no sail in the group - return without result

    // find a new owner
    int gn;
    for (gn = 0; gn < groupQuantity; gn++)
        if (gdata[gn].modelEI == newModelEI)
            break;
    if (gn == groupQuantity) // there is no such owner - create a new one
    {
        GROUPDATA *oldgdata = gdata;
        gdata = new GROUPDATA[groupQuantity + 1];
        if (gdata == nullptr)
            throw std::runtime_error("Not memory allocation");
        memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
        gdata[gn].bYesShip = false;
        gdata[gn].bDeleted = false;
        gdata[gn].sailQuantity = 0;
        gdata[gn].boxCenter = gdata[gn].boxSize = CVECTOR(0.f, 0.f, 0.f);
        gdata[gn].boxRadius = 0.f;
        gdata[gn].shipEI = newHostEI;
        gdata[gn].speed_m = 0.f;
        gdata[gn].modelEI = newModelEI;
        gdata[gn].dwSailsColor = 0xFFFFFFFF;
        gdata[gn].maxSP = 100;
        delete oldgdata;
        oldgdata = nullptr;
        groupQuantity++;
        bDeleteState = true;
    }

    // look for a new sail in a new group
    int i;
    for (i = 0; i < gdata[gn].sailQuantity; i++)
        if (gdata[gn].sailIdx[i] == sn)
            break;

    if (m_nMastCreatedCharacter >= 0 && slist[sn] != nullptr)
        core.Event("DoSailHole", "llssllllf", -1, m_nMastCreatedCharacter, (m_sMastName == nullptr ? "#" : m_sMastName),
                   slist[sn]->hostNode->GetName(), slist[sn]->groupNum, slist[sn]->GetMaxHoleCount(),
                   (1 << slist[sn]->GetMaxHoleCount()) - 1, slist[sn]->GetMaxHoleCount(),
                   static_cast<float>(slist[sn]->maxSpeed) / gdata[oldg].speed_m);

    if (i == gdata[gn].sailQuantity)
    {
        // add the found sail to a new group
        if (gdata[gn].sailQuantity == 0)
        {
            gdata[gn].sailIdx = new int[1];
            gdata[gn].sailQuantity = 1;
        }
        else
        {
            int *oldIdx = gdata[gn].sailIdx;
            if ((gdata[gn].sailIdx = new int[gdata[gn].sailQuantity + 1]) == nullptr)
            {
                throw std::runtime_error("Not memory allocation");
            }
            memcpy(gdata[gn].sailIdx, oldIdx, sizeof(int) * gdata[gn].sailQuantity);
            delete oldIdx;
            gdata[gn].sailQuantity++;
        }
        gdata[gn].sailIdx[gdata[gn].sailQuantity - 1] = sn;
        gdata[gn].speed_m += slist[sn]->maxSpeed;
    }
    slist[sn]->HostNum = gn;

    // remove the sail from the old group
    if (gdata[oldg].sailQuantity == 1)
    // remove the only sail together with the group
    {
        // write down the value of the state of the sails = 0 for the ship
        auto *pVai = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[oldg].shipEI));
        if (pVai && pVai->GetACharacter())
        {
            ATTRIBUTES *pA = pVai->GetACharacter()->GetAttributeClass("Ship");
            if (pA)
                pA->SetAttributeUseDword("SP", 0);
        }
        STORM_DELETE(gdata[oldg].sailIdx);
        gdata[oldg].sailQuantity = 0;
        gdata[oldg].bDeleted = true;
    }
    else
    {
        gdata[oldg].sailQuantity--;
        gdata[oldg].sailIdx[idx] = gdata[oldg].sailIdx[gdata[oldg].sailQuantity];
    }
    bDeleteState = true;

    // change some values for our sail
    STORM_DELETE(slist[sn]->sailtrope.rrs[0]);
    STORM_DELETE(slist[sn]->sailtrope.rrs[1]);
    slist[sn]->sailtrope.pnttie[0] = slist[sn]->sailtrope.pnttie[1] = slist[sn]->sailtrope.pnttie[2] =
        slist[sn]->sailtrope.pnttie[3] = false;
    slist[sn]->sailtrope.pPos[0] = slist[sn]->sailtrope.pPos[1] = slist[sn]->sailtrope.pPos[2] =
        slist[sn]->sailtrope.pPos[3] = nullptr;
    slist[sn]->ss.turningSail = false;
}

void SAIL::DeleteSailGroup()
{
    bUse = false;
    int gn, sn, i;

    const int old_sailQuantity = sailQuantity;
    const int old_groupQuantity = groupQuantity;

    // remove all sails from the deleted group //
    // ---------------------------------------//
    for (sn = 0, sailQuantity = 0; sn < old_sailQuantity; sn++)
        if (gdata[slist[sn]->HostNum].bDeleted || slist[sn]->bDeleted)
        {
            STORM_DELETE(slist[sn]);
        }
        else
            sailQuantity++;

    if (sailQuantity == 0)
        sailQuantity = 0;

    // counting the number of groups
    for (gn = 0, groupQuantity = 0; gn < old_groupQuantity; gn++)
        if (!gdata[gn].bDeleted)
            groupQuantity++;
        else
        {
            STORM_DELETE(gdata[gn].sailIdx);
            gdata[gn].sailQuantity = 0;
        }

    // new lists of sails and groups
    SAILONE **oldslist = slist;
    GROUPDATA *oldgdata = gdata;
    if (sailQuantity == 0 || groupQuantity == 0)
    {
        for (i = 0; i < old_sailQuantity; i++)
        {
            STORM_DELETE(oldslist[i]);
        }
        slist = nullptr;
        for (i = 0; i < old_groupQuantity; i++)
        {
            STORM_DELETE(oldgdata[i].sailIdx);
        }
        gdata = nullptr;
        sailQuantity = 0;
        groupQuantity = 0;
    }
    else
    {
        slist = new SAILONE *[sailQuantity];
        gdata = new GROUPDATA[groupQuantity];
        if (slist == nullptr || gdata == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }

        groupQuantity = 0;
        sailQuantity = 0;
        for (gn = 0; gn < old_groupQuantity; gn++)
        {
            STORM_DELETE(oldgdata[gn].sailIdx);
            // counting the number of sails in a group
            int nsn = 0;
            for (sn = 0; sn < old_sailQuantity; sn++)
                if (oldslist[sn] != nullptr && oldslist[sn]->HostNum == gn)
                    nsn++;
            if (nsn == 0)
                continue;
            // in a new location, create a record about the sail group
            memcpy(&gdata[groupQuantity], &oldgdata[gn], sizeof(GROUPDATA));
            if ((gdata[groupQuantity].sailIdx = new int[nsn]) == nullptr)
            {
                throw std::runtime_error("allocate memory error");
            }
            gdata[groupQuantity].sailQuantity = nsn;
            // fill in the list of sails for the group and general
            for (sn = 0, nsn = 0; sn < old_sailQuantity; sn++)
                if (oldslist[sn] != nullptr && oldslist[sn]->HostNum == gn)
                {
                    gdata[groupQuantity].sailIdx[nsn++] = sailQuantity;
                    slist[sailQuantity] = oldslist[sn];
                    slist[sailQuantity]->HostNum = groupQuantity;
                    sailQuantity++;
                }
            // transition to another group
            groupQuantity++;
        }
    }
    delete oldgdata;
    oldgdata = nullptr;
    STORM_DELETE(oldslist);

    // calculation of new parameters for vertex and index buffers
    long vIndx = 0; // index of a vertex buffer
    // passing through all groups and setting new links to buffers
    for (gn = 0; gn < groupQuantity; gn++)
    {
        for (sn = 0; sn < gdata[gn].sailQuantity; sn++)
        {
            slist[gdata[gn].sailIdx[sn]]->ss.sVert = vIndx;
            slist[gdata[gn].sailIdx[sn]]->ss.sIndx = 54;
            vIndx += slist[gdata[gn].sailIdx[sn]]->ss.nVert;
        }
    }
    sg.nVert = vIndx;

    // remove old buffers
    VERTEX_BUFFER_RELEASE(RenderService, sg.vertBuf);
    if (sg.nVert > 0)
    {
        // create new buffers
        sg.vertBuf = RenderService->CreateVertexBuffer(SAILVERTEX_FORMAT, sg.nVert * sizeof(SAILVERTEX),
                                                       D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

        SAILVERTEX *pv;
        pv = static_cast<SAILVERTEX *>(RenderService->LockVertexBuffer(sg.vertBuf));
        if (pv)
        {
            for (i = 0; i < sailQuantity; i++)
            {
                slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
                slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
            }
            RenderService->UnLockVertexBuffer(sg.vertBuf);
        }
    }

    bDeleteState = false;
    wFirstIndx = sailQuantity;
    bUse = sailQuantity > 0;
}

void SAIL::SetAddSails(int firstSail)
{
    for (int i = firstSail; i < sailQuantity; i++)
    {
        // form the parameters of the vertex and index buffers
        slist[i]->SetTurnLimits();
        slist[i]->ss.sVert = sg.nVert;
        slist[i]->ss.sIndx = 54;
        sg.nVert += slist[i]->ss.nVert;
    }

    // remove old buffers
    VERTEX_BUFFER_RELEASE(RenderService, sg.vertBuf);
    // create new buffers
    sg.vertBuf = RenderService->CreateVertexBuffer(SAILVERTEX_FORMAT, sg.nVert * sizeof(SAILVERTEX),
                                                   D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

    // fill the vertex buffer and set the texture coordinates
    SAILVERTEX *pv;
    pv = static_cast<SAILVERTEX *>(RenderService->LockVertexBuffer(sg.vertBuf));
    if (pv)
    {
        for (int i = 0; i < sailQuantity; i++)
        {
            slist[i]->FillVertex(&pv[slist[i]->ss.sVert]);
            slist[i]->SetTexGrid(&pv[slist[i]->ss.sVert]);
        }
        RenderService->UnLockVertexBuffer(sg.vertBuf);
    }
    else
        throw std::runtime_error("Vertex buffer error");
}

void SAIL::DoNoRopeSailToNewHost(entid_t newModel, entid_t newHost, entid_t oldHost)
{
    entid_t rope_id;
    if (!(rope_id = EntityManager::GetEntityId("rope")))
        return; // no ropes, no concert
    auto *rb = static_cast<ROPE_BASE *>(EntityManager::GetEntityPointer(rope_id));
    if (rb == nullptr)
        return;

    // find the group of the old owner
    int ogn;
    for (ogn = 0; ogn < groupQuantity; ogn++)
        if (gdata[ogn].bYesShip && gdata[ogn].shipEI == oldHost && !gdata[ogn].bDeleted)
            break;
    if (ogn == groupQuantity)
        return;

    // new root NODE
    auto nmdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(newModel));
    if (nmdl == nullptr)
        return;
    NODE *nroot = nmdl->GetNode(0);
    if (nroot == nullptr)
        return;

    auto *omdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[ogn].modelEI));
    if (omdl == nullptr)
        return;

    // go over the sails in the found group
    for (int si = 0; si < gdata[ogn].sailQuantity; si++)
    {
        const int sn = gdata[ogn].sailIdx[si];
        NODE *nod = nullptr;
        for (int k = 0; k < 1000; k++)
        {
            nod = omdl->GetNode(k);
            if (nod == nullptr)
                break;
            if (nod == slist[sn]->hostNode)
                break;
        }
        if (nod == nullptr)
            continue;

        GEOS *tgeo = slist[sn]->hostNode->geo;
        GEOS::INFO gi;
        tgeo->GetInfo(gi);
        for (int i = 0; i < gi.nlabels; i++)
        {
            GEOS::LABEL gl;
            tgeo->GetLabel(i, gl);
            if (!strncmp(gl.group_name, "sail", 4))                                  // this is a group of sails
                if (atoi(&gl.group_name[5]) == slist[sn]->groupNum)                  // with the correct group number
                    if (!strncmp(gl.name, "rope", 4) || !strncmp(gl.name, "fal", 3)) // and the sail is tied with a rope
                    {
                        long nRopeNum;
                        if (gl.name[0] == 'r')
                            nRopeNum = atoi(&gl.name[5]);
                        else
                            nRopeNum = atoi(&gl.name[4]) + 1000;
                        if (rb->IsAbsentRope(gdata[ogn].modelEI, nRopeNum))
                        {
                            // remove the ropes belonging to this sail
                            if (slist[sn]->sailtrope.pnttie[0] || slist[sn]->sailtrope.pnttie[1] ||
                                slist[sn]->sailtrope.pnttie[2] || slist[sn]->sailtrope.pnttie[3])
                            {
                                rb->DoDeleteUntie(gdata[ogn].modelEI, slist[sn]->hostNode, slist[sn]->groupNum);
                            }
                            // transferring the sail to a new group
                            DoSailToNewHost(newModel, newHost, slist[sn]->groupNum, slist[sn]->hostNode,
                                            gdata[ogn].modelEI);
                            // transfer this sail to another NODE
                            int iMax = 4;
                            if (slist[sn]->ss.eSailType == SAIL_TREANGLE)
                                iMax = 3;
                            for (int j = 0; j < iMax; j++)
                                nroot->glob_mtx.MulToInv(slist[sn]->hostNode->glob_mtx * slist[sn]->ss.hardPoints[j],
                                                         slist[sn]->ss.hardPoints[j]);
                            slist[sn]->hostNode = nroot;
                            slist[sn]->pMatWorld = &nroot->glob_mtx;
                        }
                    }
        }
    }
}

void sailPrint(VDX9RENDER *rs, const CVECTOR &pos3D, float rad, long line, const char *format, ...)
{
    static char buf[256];
    // print to the buffer
    long len = _vsnprintf_s(buf, sizeof(buf) - 1, format, (char *)(&format + 1));
    buf[sizeof(buf) - 1] = 0;
    // Looking for a point position on the screen
    static CMatrix mtx, view, prj;
    static D3DVIEWPORT9 vp;
    MTX_PRJ_VECTOR vrt;
    rs->GetTransform(D3DTS_VIEW, view);
    rs->GetTransform(D3DTS_PROJECTION, prj);
    mtx.EqMultiply(view, prj);
    view.Transposition();
    float dist = ~(pos3D - view.Pos());
    if (dist >= rad * rad)
        return;
    const float d = view.Vz() | view.Pos();
    if ((pos3D | view.Vz()) < d)
        return;
    rs->GetViewport(&vp);
    mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width * 0.5f, vp.Height * 0.5f, sizeof(CVECTOR),
                   sizeof(MTX_PRJ_VECTOR));
    // Looking for a position
    const long fh = rs->CharHeight(FONT_DEFAULT) / 2;
    vrt.y -= (line + 0.5f) * fh;
    // Transparency
    long color = 0xffffffff;
    const float kDist = 0.75f;
    if (dist > kDist * kDist * rad * rad)
    {
        dist = 1.0f - (sqrtf(dist) - kDist * rad) / (rad - kDist * rad);
        color = (static_cast<uint32_t>(dist * 255.0f) << 24) | 0xffffff;
    }
    rs->ExtPrint(FONT_DEFAULT, color, 0x00000000, PR_ALIGN_CENTER, false, 1.0f, 0, 0, static_cast<long>(vrt.x),
                 static_cast<long>(vrt.y), buf);
}

SAILONE *SAIL::FindSailFromData(int gn, const char *nodeName, const char *grName) const
{
    if (nodeName == nullptr || grName == nullptr)
        return nullptr;
    int grNum;
    sscanf(grName, "%d", &grNum);
    for (int i = 0; i < gdata[gn].sailQuantity; i++)
    {
        const int sn = gdata[gn].sailIdx[i];
        if (slist[sn]->hostNode != nullptr && strcmp(slist[sn]->hostNode->GetName(), nodeName) == 0 &&
            slist[sn]->groupNum == grNum)
            return slist[sn];
    }
    return nullptr;
}

void SAIL::SetSailTextures(long grNum, VDATA *pvd) const
{
    if (grNum < 0 || grNum >= groupQuantity || pvd == nullptr)
        return;

    ATTRIBUTES *pA = pvd->GetAClass();
    if (pA == nullptr)
        return;

    gdata[grNum].maxSP = pA->GetAttributeAsDword("MaxSP", gdata[grNum].maxSP);

    // main texture
    char *pcNormalName = pA->GetAttribute("normalTex");
    // coat of arms of a texture
    auto *pGeraldTexture = (IDirect3DTexture9 *)pA->GetAttributeAsPointer("geraldTexPointer", 0);
    char *pcGeraldName = pA->GetAttribute("geraldTex");
    //
    gdata[grNum].dwSailsColor = pA->GetAttributeAsDword("sailscolor", 0xFFFFFFFF);

    for (int i = 0; i < gdata[grNum].sailQuantity; i++)
    {
        SAILONE *so = slist[gdata[grNum].sailIdx[i]];
        if (so == nullptr || so->hostNode == nullptr)
            continue;
        char param[256];
        sprintf_s(param, "%s", so->hostNode->GetName());
        ATTRIBUTES *pAGerald = pA->GetAttributeClass(param);
        if (pAGerald)
        {
            if (pAGerald->GetAttribute("Gerald"))
                so->m_nGeraldTex = RenderService->TextureCreate(pAGerald->GetAttribute("Gerald"));
            if (so->m_nGeraldTex == -1 && pcGeraldName)
                so->m_nGeraldTex = RenderService->TextureCreate(pcGeraldName);
            if (so->m_nGeraldTex == -1 && pGeraldTexture)
            {
                pGeraldTexture->AddRef();
                so->m_pGeraldTex = pGeraldTexture;
            }
            if (so->m_nGeraldTex != -1 || so->m_pGeraldTex != nullptr)
            {
                so->m_bIsGerald = true;
                so->m_fHorzGeraldScale = pAGerald->GetAttributeAsFloat("hscale", 0.5f);
                so->m_fVertGeraldScale = pAGerald->GetAttributeAsFloat("vscale", so->m_fHorzGeraldScale);
                if (so->m_fHorzGeraldScale > 0.f)
                    so->m_fHorzGeraldScale = 1.f / so->m_fHorzGeraldScale;
                else
                    so->m_fHorzGeraldScale = 0.5f;
                if (so->m_fVertGeraldScale > 0.f)
                    so->m_fVertGeraldScale = 1.f / so->m_fVertGeraldScale;
                else
                    so->m_fVertGeraldScale = 0.5f;
            }
        }
        if (so->surfaceTex == -1)
            so->surfaceTex = RenderService->TextureCreate(pcNormalName);
    }
}

int SAIL::FindGroupForCharacter(int chrIdx) const
{
    for (int gn = 0; gn < groupQuantity; gn++)
    {
        if (gdata[gn].bDeleted || !gdata[gn].bYesShip)
            continue;
        ATTRIBUTES *pA = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[gn].shipEI))->GetACharacter();
        if (pA != nullptr)
            if (static_cast<int>(pA->GetAttributeAsDword("index", -1)) == chrIdx)
                return gn;
    }
    return -1;
}

int SAIL::GetCharacterForGroup(int grNum) const
{
    ATTRIBUTES *pA = nullptr;
    if (gdata[grNum].bYesShip)
        pA = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(gdata[grNum].shipEI))->GetACharacter();
    if (pA != nullptr)
        return static_cast<int>(pA->GetAttributeAsDword("index", -1));
    return -1;
}

SAILONE_BASE *SAIL::FindSailForCharacter(int chrIdx, char *nodeName, int grNum)
{
    if (nodeName == nullptr)
        return nullptr;

    const int gn = FindGroupForCharacter(chrIdx);

    if (gn >= 0 && gn < groupQuantity)
    {
        for (int idx = 0; idx < gdata[gn].sailQuantity; idx++)
        {
            const int sn = gdata[gn].sailIdx[idx];
            if (slist[sn]->hostNode != nullptr && slist[sn]->groupNum == grNum &&
                strcmp(slist[sn]->hostNode->GetName(), nodeName) == 0)
                return slist[sn];
        }
    }

    return nullptr;
}

void SAIL::LostRender()
{
    if (sg.indxBuf != -1)
        RenderService->ReleaseIndexBuffer(sg.indxBuf);
    sg.indxBuf = -1;
}

void SAIL::RestoreRender()
{
    if (sg.nVert == 0)
        return;
    sg.indxBuf = RenderService->CreateIndexBuffer(sg.nIndx * 2, D3DUSAGE_DYNAMIC);

    // Set triangle buffer for sea mirror
    auto *pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(sg.indxBuf));
    if (pt)
    {
        for (int i = 0; i < sailQuantity; i++)
        {
            slist[i]->FillIndex(pt);
        }

        // for triangle sail
        pt[0] = pt[15] = pt[24] = 0;                                                                               // 0
        pt[1] = pt[3] = pt[6] = pt[12] = pt[16] = pt[21] = pt[25] = SAIL_ROW_MAX / 2 * (SAIL_ROW_MAX / 2 + 1) / 2; // 1
        pt[2] = pt[8] = pt[9] = pt[17] = pt[18] = pt[23] = pt[26] = SAIL_ROW_MAX / 2 * (SAIL_ROW_MAX / 2 + 3) / 2; // 2
        pt[4] = pt[13] = (SAIL_ROW_MAX - 1) * SAIL_ROW_MAX / 2;                                                    // 3
        pt[5] = pt[7] = pt[10] = pt[14] = pt[19] = pt[22] =
            (SAIL_ROW_MAX - 1) * SAIL_ROW_MAX / 2 + SAIL_ROW_MAX / 2;  // 4
        pt[11] = pt[20] = (SAIL_ROW_MAX - 1) * (SAIL_ROW_MAX + 2) / 2; // 5

        // for square sail
        pt[27] = pt[42] = pt[51] = 0;                                                                             // 0
        pt[28] = pt[31] = pt[40] = pt[43] = pt[52] = SAIL_ROW_MAX * SAIL_COL_MAX - SAIL_ROW_MAX;                  // 3
        pt[29] = pt[30] = pt[33] = pt[39] = pt[44] = pt[48] = pt[53] = SAIL_ROW_MAX / 2;                          // 1
        pt[32] = pt[34] = pt[37] = pt[41] = pt[46] = pt[49] = SAIL_ROW_MAX * SAIL_COL_MAX - SAIL_ROW_MAX / 2 - 1; // 4
        pt[35] = pt[36] = pt[45] = pt[50] = SAIL_ROW_MAX - 1;                                                     // 2
        pt[38] = pt[47] = SAIL_ROW_MAX * SAIL_COL_MAX - 1;                                                        // 5

        RenderService->UnLockIndexBuffer(sg.indxBuf);
    }
}

int SAIL::GetSailStateForCharacter(int chrIdx) const
{
    const int gn = FindGroupForCharacter(chrIdx);
    if (gn < 0 || gn >= groupQuantity)
        return 0;
    switch (gdata[gn].curSailSet)
    {
    case ROLLTYPE_NONE:
        return 0;
        break;
    case ROLLTYPE_NORMAL:
        return 1;
        break;
    case ROLLTYPE_FAST:
        return 2;
        break;
    }
    return 0;
}

uint32_t SAIL::ScriptProcessing(const char *name, MESSAGE &message)
{
    if (name == nullptr)
        return 0;

    if (_stricmp(name, "RandomSailsDmg") == 0)
    {
        const long chrIdx = message.Long();
        const float fDmg = message.Float();
        const int gn = FindGroupForCharacter(chrIdx);
        if (gn >= 0 && gn < groupQuantity)
            DoRandomsSailsDmg(chrIdx, gn, fDmg);
    }

    if (_stricmp(name, "SailRollSpeed") == 0)
    {
        const long chrIdx = message.Long();
        const float fSpeed = message.Float();
        const int gn = FindGroupForCharacter(chrIdx);
        if (gn >= 0 && gn < groupQuantity)
            gdata[gn].fRollingSpeed = fSpeed * ROLLINGSPEED;
    }

    if (_stricmp(name, "GetSailStatus") == 0)
    {
        long chrIdx = message.Long();
        int gn = FindGroupForCharacter(chrIdx);
        if (gn >= 0 && gn < groupQuantity)
            GetSailStatus(chrIdx, gn);
    }

    return 0;
}

void SAIL::GetSailStatus(int chrIdx, int gn)
{
    if (gn < 0 || gn >= groupQuantity)
        return;

    for (int i = 0; i < gdata[gn].sailQuantity; i++)
    {
        int sn = gdata[gn].sailIdx[i];
        VDATA *pvd = core.Event("evntGetSailStatus", "lslfll", chrIdx, slist[sn]->hostNode->GetName(),
                                slist[sn]->groupNum, (float)slist[sn]->maxSpeed / gdata[gn].speed_m,
                                slist[sn]->ss.holeCount, slist[sn]->GetMaxHoleCount());
    }
}

void SAIL::DoRandomsSailsDmg(int chrIdx, int gn, float fDmg)
{
    if (gn < 0 || gn >= groupQuantity)
        return;

    int i;
    int _sailsIdx[256];
    for (i = 0; i < gdata[gn].sailQuantity; i++)
        _sailsIdx[i] = gdata[gn].sailIdx[i];

    for (int _sailsQnt = gdata[gn].sailQuantity; _sailsQnt > 0;)
    {
        const int tmp = rand() % _sailsQnt;
        const int sn = _sailsIdx[tmp];

        int holeIdx = rand() % slist[sn]->GetMaxHoleCount();
        for (int j = 0; j < 14 && slist[sn]->ss.hole[holeIdx]; j++)
            if (holeIdx < static_cast<int>(slist[sn]->GetMaxHoleCount()) - 1)
                holeIdx++;
            else
                holeIdx = 0;

        const bool bOldHole = slist[sn]->ss.hole[holeIdx];
        slist[sn]->ss.hole[holeIdx] = true;
        int nNewHoleCount = slist[sn]->ss.holeCount;
        if (!bOldHole)
            nNewHoleCount++;
        VDATA *pvd = core.Event("evntRandomSailDmg", "lslfflll", chrIdx, slist[sn]->hostNode->GetName(),
                                slist[sn]->groupNum, fDmg, static_cast<float>(slist[sn]->maxSpeed) / gdata[gn].speed_m,
                                nNewHoleCount, slist[sn]->GetMaxHoleCount(), slist[sn]->GetHoleDword());
        slist[sn]->ss.hole[holeIdx] = bOldHole;
        if (pvd == nullptr)
        {
            core.Trace("WARNING!!! Event evntRandomSailDmg not float return!");
            return;
        }
        float fDoDmg = pvd->GetFloat();

        if (fDoDmg > 0.f)
        {
            slist[sn]->ss.holeCount = nNewHoleCount;
            slist[sn]->ss.hole[holeIdx] = true;
            slist[sn]->CalculateMirrorSailIndex();
        }
        else
            fDoDmg = -fDoDmg;
        if (fDoDmg == 0.f)
        {
            _sailsQnt--;
            _sailsIdx[sn] = _sailsIdx[_sailsQnt];
        }
        else
        {
            fDmg -= fDoDmg;
            if (fDmg <= 0.f)
            {
                return;
            }
        }
    }
}

uint32_t SAIL::AttributeChanged(ATTRIBUTES *pAttr)
{
    if (pAttr == nullptr)
        return 0;
    if (*pAttr == "MinSpeed")
        m_fMinSpeedVal = pAttr->GetAttributeAsFloat();
    return 0;
}
