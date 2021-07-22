#include "AIBalls.h"
#include "AIFort.h"
#include "inlines.h"
#include "shared/messages.h"

#include <algorithm>

AIBalls *AIBalls::pAIBalls = nullptr;

AIBalls::AIBalls()
{
    pSail = nullptr;
    pSea = nullptr;
    pFort = nullptr;
    pIsland = nullptr;
    pAIBalls = this;

    fBallFlySoundDistance = 1.0f;

    fDeltaTimeMultiplyer = 1.0f;
    dwFireBallFromCameraTime = 0;
}

AIBalls::~AIBalls()
{
    uint32_t i, j;

    pAIBalls = nullptr;

    AIHelper::pRS->TextureRelease(dwTextureIndex);

    for (i = 0; i < aBallTypes.size(); i++)
    {
        auto *pBallsType = &aBallTypes[i];
        for (j = 0; j < pBallsType->Balls.size(); j++)
        {
            auto *pBall = &pBallsType->Balls[j];
            if (pBall->pParticle)
            {
                STORM_DELETE(pBall->pParticle);
            }
            pBall->sBallEvent[0] = '\0';
        }
        aBallTypes[i].Balls.clear();
    }
    aBallTypes.clear();
}

bool AIBalls::Init()
{
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    SetDevice();
    return true;
}

void AIBalls::SetDevice()
{
}

void AIBalls::FireBallFromCamera()
{
    auto *pMainCharIndex = static_cast<VDATA *>(core.GetScriptVariable("nMainCharacterIndex"));
    if (!pMainCharIndex)
        return;
    auto iMainCharIndex = pMainCharIndex->GetLong();
    if (iMainCharIndex < 0)
        return;
    auto *pMainCharacter = static_cast<VDATA *>(core.GetScriptVariable("Characters"));
    if (!pMainCharacter)
        return;
    auto *pAMainCharacter = pMainCharacter->GetAClass(iMainCharIndex);
    if (!pAMainCharacter)
        return;
    auto *pACannonType = pAMainCharacter->FindAClass(pAMainCharacter, "Ship.Cannons.Type");
    if (!pACannonType)
        return;
    auto dwCannonType = pACannonType->GetAttributeAsDword();

    auto *pABall = pAMainCharacter->CreateAttribute("_err324__", "");
    if (!pABall)
        return;

    /*AIHelper::pRS->GetTransform(D3DTS_VIEW, mView);
    CMatrix mIView = mView;
    mIView.Transposition3X3();
    float fY = atan2f(mIView.Vz().x, mIView.Vz().z);
    float fX = SIGN(mIView.Vz().y) * acosf(mIView.Vz() | CVECTOR(mIView.Vz().x, 0.0f, mIView.Vz().z));*/

    CVECTOR vCamPos, vCamAng;
    float fFov;
    AIHelper::pRS->GetCamera(vCamPos, vCamAng, fFov);

    CMatrix mView(vCamAng, vCamPos);
    auto mIView = mView;
    // mIView.Transposition3X3();
    auto fY = atan2f(mIView.Vz().x, mIView.Vz().z);
    auto fX = SIGN(mIView.Vz().y) * acosf(mIView.Vz() | CVECTOR(mIView.Vz().x, 0.0f, mIView.Vz().z));

    pABall->SetAttribute("Type", "Balls");
    pABall->SetAttributeUseDword("CharacterIndex", static_cast<uint32_t>(iMainCharIndex));
    pABall->SetAttributeUseFloat("x", mIView.Pos().x);
    pABall->SetAttributeUseFloat("y", mIView.Pos().y);
    pABall->SetAttributeUseFloat("z", mIView.Pos().z);
    pABall->SetAttributeUseFloat("SpdV0", 100.0f);
    pABall->SetAttributeUseFloat("HeightMultiply", 1.0f);
    pABall->SetAttributeUseFloat("SizeMultiply", 1.0f);
    pABall->SetAttributeUseFloat("TimeSpeedMultiply", 1.0f);
    pABall->SetAttributeUseDword("CannonType", dwCannonType);
    pABall->SetAttributeUseFloat("MaxFireDistance", 15000.0f);

    pABall->SetAttributeUseFloat("Ang", fX);
    pABall->SetAttributeUseFloat("Dir", fY);

    AddBall(pABall);

    pAMainCharacter->DeleteAttributeClassX(pABall);
}

void AIBalls::AddBall(ATTRIBUTES *pABall)
{
    auto *const pBallName = pABall->GetAttribute("Type");
    Assert(pBallName);

    uint32_t i;
    for (i = 0; i < aBallTypes.size(); i++)
        if (_stricmp(aBallTypes[i].sName.c_str(), pBallName) == 0)
            break;
    if (i == aBallTypes.size())
        return;

    aBallTypes[i].Balls.push_back(BALL_PARAMS{});
    // BALL_PARAMS * pBall = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];
    auto *pBall = &aBallTypes[i].Balls.back();

    pBall->iBallOwner = pABall->GetAttributeAsDword("CharacterIndex");

#define GetAFloat(x) pABall->GetAttributeAsFloat(x)
#define GetADword(x) pABall->GetAttributeAsDword(x)

    pBall->fTime = 0.0f;
    pBall->vPos = pBall->vFirstPos = CVECTOR(GetAFloat("x"), GetAFloat("y"), GetAFloat("z"));
    pBall->fSpeedV0 = GetAFloat("SpdV0");
    pBall->fHeightMultiply = GetAFloat("HeightMultiply");
    pBall->fSizeMultiply = GetAFloat("SizeMultiply");
    pBall->fTimeSpeedMultiply = GetAFloat("TimeSpeedMultiply");
    pBall->dwCannonType = GetADword("CannonType");
    pBall->fMaxFireDistance = GetAFloat("MaxFireDistance");
    const auto fAngle = GetAFloat("Ang");
    pBall->fCosAngle = cosf(fAngle);
    pBall->fSinAngle = sinf(fAngle);
    const auto fDir = GetAFloat("Dir");
    pBall->fDirX = cosf(fDir);
    pBall->fDirZ = sinf(fDir);
    pBall->pParticle = nullptr;

    // pBall->sBallEvent = pABall->GetAttribute("Event");
    const auto *event_str = pABall->GetAttribute("Event");
    const auto len = std::min(strlen(event_str), static_cast<size_t>(TSE_MAX_EVENT_LENGTH));
    std::copy_n(event_str, len, pBall->sBallEvent);
    pBall->sBallEvent[len] = '\0';

    if (aBallTypes[i].sParticleName.size())
    {
        entid_t eidParticle;
        if (eidParticle = EntityManager::GetEntityId("particles"))
        {
            pBall->pParticle = (VPARTICLE_SYSTEM *)core.Send_Message(
                eidParticle, "lsffffffl", PS_CREATE_RIC, (char *)aBallTypes[i].sParticleName.c_str(), pBall->vPos.x,
                pBall->vPos.y, pBall->vPos.z, 0.0f, 1.0f, 0.0f, 100000);
        }
    }
}

void AIBalls::Execute(uint32_t Delta_Time)
{
    uint32_t i, j;
    CVECTOR vSrc, vDst;
    entid_t EID;

    if (!pIsland && (EID = EntityManager::GetEntityId("island")))
        pIsland = static_cast<CANNON_TRACE_BASE *>(EntityManager::GetEntityPointer(EID));
    if (!pSail && (EID = EntityManager::GetEntityId("sail")))
        pSail = static_cast<CANNON_TRACE_BASE *>(EntityManager::GetEntityPointer(EID));
    if (!pSea && (EID = EntityManager::GetEntityId("sea")))
        pSea = static_cast<CANNON_TRACE_BASE *>(EntityManager::GetEntityPointer(EID));

    aBallRects.clear();

    // if (!pVWForts) pVWForts = (VIDWALKER*)core.LayerGetWalker("fort_cannon_trace");

    auto mView = rs->GetView();

    auto fDeltaTime = 0.001f * static_cast<float>(Delta_Time);

    for (i = 0; i < aBallTypes.size(); i++)
    {
        auto *pBallsType = &aBallTypes[i];

        AttributesPointer->SetAttributeUseDword("CurrentBallType", pBallsType->dwGoodIndex);

        for (j = 0; j < pBallsType->Balls.size(); j++)
        {
            auto *pBall = &pBallsType->Balls[j];

            vSrc = pBall->vPos;

            AttributesPointer->SetAttributeUseDword("CurrentBallCannonType", pBall->dwCannonType);
            AttributesPointer->SetAttributeUseFloat("CurrentBallDistance", sqrtf(~(pBall->vPos - pBall->vFirstPos)));
            AttributesPointer->SetAttributeUseFloat("CurrentMaxBallDistance", pBall->fMaxFireDistance);

            // update ball time
            pBall->fTime += fDeltaTime * fDeltaTimeMultiplyer * pBall->fTimeSpeedMultiply;
            // update positions
            float fsX = pBall->fSpeedV0 * pBall->fTime * pBall->fCosAngle;
            float fsY = pBall->fHeightMultiply * (pBall->fSpeedV0 * pBall->fTime * pBall->fSinAngle -
                                                  AIHelper::fGravity * SQR(pBall->fTime) / 2.0f);
            pBall->vPos = CVECTOR(0.0f, fsY, fsX);
            RotateAroundY(pBall->vPos.x, pBall->vPos.z, pBall->fDirX, pBall->fDirZ);
            pBall->vPos += pBall->vFirstPos;

            vDst = pBall->vPos;

            if (pBall->sBallEvent[0] != '\0')
                core.Event(pBall->sBallEvent, "lllffffffs", pBall->iBallOwner, static_cast<uint32_t>(1),
                           pBallsType->dwGoodIndex, pBall->vPos.x, pBall->vPos.y, pBall->vPos.z, vSrc.x, vSrc.y,
                           vSrc.z);

            if (pBall->pParticle)
            {
                CVECTOR vDir = !(vSrc - vDst);
                pBall->pParticle->SetEmitter(pBall->vPos, vDir);
            }

            float fRes = 2.0f;

            CVECTOR v1 = mView * vSrc;
            CVECTOR v2 = mView * vDst;

            if (SIGN(v1.z) != SIGN(v2.z))
            {
                float fDelta = fabsf(v1.z / (v2.z - v1.z));
                float x = v1.x + (v2.x - v1.x) * fDelta;
                float y = v1.y + (v2.y - v1.y) * fDelta;

                if (Sqr(x) + Sqr(y) <= Sqr(fBallFlySoundDistance))
                {
                    CVECTOR vRes, v = fBallFlySoundStereoMultiplyer * CVECTOR(x, y, 0.0f);
                    mView.MulToInv(v, vRes);

                    core.Event(BALL_FLY_NEAR_CAMERA, "fff", vRes.x, vRes.y, vRes.z);
                }
            }

            // sail trace
            if (pSail)
                pSail->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

            const auto its = EntityManager::GetEntityIdIterators(SHIP_CANNON_TRACE);
            for (auto it = its.first; it != its.second; ++it)
            {
                auto *pShip = static_cast<CANNON_TRACE_BASE *>(EntityManager::GetEntityPointer(it->second));
                fRes = pShip->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
                if (fRes <= 1.0f)
                    break;
            }

            // fort trace
            if (fRes > 1.0f && AIFort::pAIFort)
            {
                fRes = AIFort::pAIFort->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
            }

            // island trace
            if (fRes > 1.0f && pIsland)
                fRes = pIsland->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

            // sea trace
            if (fRes > 1.0f && pSea)
                fRes = pSea->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

            // delete ball
            if (fRes <= 1.0f)
            {
                if (pBall->sBallEvent[0] != '\0')
                {
                    core.Event(pBall->sBallEvent, "lllffffff", pBall->iBallOwner, static_cast<uint32_t>(0),
                               pBallsType->dwGoodIndex, pBall->vPos.x, pBall->vPos.y, pBall->vPos.z, vSrc.x, vSrc.y,
                               vSrc.z);
                    pBall->sBallEvent[0] = '\0';
                }

                if (pBall->pParticle)
                {
                    pBall->pParticle->Stop();
                    STORM_DELETE(pBall->pParticle);
                }
                // pBallsType->Balls.ExtractNoShift(j);
                pBallsType->Balls[j] = pBallsType->Balls.back();
                pBallsType->Balls.pop_back();
                j--;

                continue;
            }

            if (pBall->sBallEvent[0] == '\0')
            {
                aBallRects.push_back(RS_RECT{});
                // RS_RECT * pRSR = &aBallRects[aBallRects.Add()];
                RS_RECT *pRSR = &aBallRects.back();
                pRSR->vPos = pBall->vPos;
                pRSR->dwColor = 0xFFFFFF;
                pRSR->dwSubTexture = pBallsType->dwSubTexIndex;
                pRSR->fAngle = pBall->fTime * 3.0f;
                pRSR->fSize = pBallsType->fSize * pBall->fSizeMultiply;
            }
        }
    }
}

void AIBalls::Realize(uint32_t Delta_Time)
{
    if (aBallRects.size())
    {
        AIHelper::pRS->TextureSet(0, dwTextureIndex);
        AIHelper::pRS->DrawRects(&aBallRects[0], aBallRects.size(), "Cannonballs", dwSubTexX, dwSubTexY);
        aBallRects.clear();
    }

    dwFireBallFromCameraTime += Delta_Time;
    /*
  #ifndef _XBOX
    if (core.Controls->GetDebugAsyncKeyState('C') < 0 && dwFireBallFromCameraTime > 30 &&
  AttributesPointer->GetAttributeAsDword("FireBallFromCamera", 0) != 0)
    {
      dwFireBallFromCameraTime = 0;
      FireBallFromCamera();
    }
  #endif
  */
}

uint32_t AIBalls::AttributeChanged(ATTRIBUTES *pAttributeChanged)
{
    if (*pAttributeChanged == "clear")
    {
        for (uint32_t i = 0; i < aBallTypes.size(); i++)
        {
            BALL_TYPE *pBallsType = &aBallTypes[i];

            for (uint32_t j = 0; j < pBallsType->Balls.size(); j++)
            {
                BALL_PARAMS *pBall = &pBallsType->Balls[j];

                pBall->sBallEvent[0] = '\0';

                if (pBall->pParticle)
                {
                    pBall->pParticle->Stop();
                    STORM_DELETE(pBall->pParticle);
                }
            }

            pBallsType->Balls.clear();
        }

        return 0;
    }

    if (*pAttributeChanged == "add")
    {
        AddBall(AttributesPointer);
        return 0;
    }

    if (*pAttributeChanged == "isDone")
    {
        // load common parameters
        fBallFlySoundDistance = AttributesPointer->GetAttributeAsFloat("BallFlySoundDistance");
        fBallFlySoundStereoMultiplyer = AttributesPointer->GetAttributeAsFloat("BallFlySoundStereoMultiplyer");
        fDeltaTimeMultiplyer = AttributesPointer->GetAttributeAsFloat("SpeedMultiply");
        sTextureName = AttributesPointer->GetAttribute("Texture");
        dwSubTexX = AttributesPointer->GetAttributeAsDword("SubTexX");
        dwSubTexY = AttributesPointer->GetAttributeAsDword("SubTexY");

        dwTextureIndex = AIHelper::pRS->TextureCreate(sTextureName.c_str());

        // install balls
        ATTRIBUTES *pAPBalls = AttributesPointer->GetAttributeClass("Balls");
        uint32_t dwIdx = 0;
        while (pAPBalls && true)
        {
            const char *pName = pAPBalls->GetAttributeName(dwIdx);
            if (!pName)
                break;
            ATTRIBUTES *pAP = pAPBalls->GetAttributeClass(pName);
            if (!pAP)
                break;

            BALL_TYPE ballType;
            ballType.sName = pName;
            ballType.dwSubTexIndex = pAP->GetAttributeAsDword("SubTexIndex");
            ballType.dwGoodIndex = pAP->GetAttributeAsDword("GoodIndex");
            ballType.fSize = pAP->GetAttributeAsFloat("Size");
            ballType.fWeight = pAP->GetAttributeAsFloat("Weight");

            if (pAP->GetAttribute("Particle"))
                ballType.sParticleName = pAP->GetAttribute("Particle");

            aBallTypes.push_back(ballType);

            dwIdx++;
        }
        return 0;
    }
    return 0;
}

uint64_t AIBalls::ProcessMessage(MESSAGE &message)
{
    if (message.Long() == MSG_MODEL_RELEASE)
    {
        for (uint32_t i = 0; i < aBallTypes.size(); i++)
            for (uint32_t j = 0; j < aBallTypes[i].Balls.size(); j++)
            {
                BALL_PARAMS *pBall = &aBallTypes[i].Balls[j];

                if (pBall->pParticle)
                {
                    pBall->pParticle->Stop();
                    STORM_DELETE(pBall->pParticle);
                }
            }
    }
    return 0;
}

void AIBalls::Save(CSaveLoad *pSL)
{
    for (uint32_t i = 0; i < aBallTypes.size(); i++)
    {
        pSL->SaveDword(aBallTypes[i].Balls.size());

        for (uint32_t j = 0; j < aBallTypes[i].Balls.size(); j++)
        {
            pSL->SaveBuffer((const char *)&aBallTypes[i].Balls[j], sizeof(BALL_PARAMS));
        }
    }
}

void AIBalls::Load(CSaveLoad *pSL)
{
    for (uint32_t i = 0; i < aBallTypes.size(); i++)
    {
        const uint32_t dwNum = pSL->LoadDword();

        auto balls_size = std::size(aBallTypes[i].Balls);
        aBallTypes[i].Balls.resize(balls_size + dwNum);
        for (uint32_t j = 0; j < dwNum; j++)
        {
            // BALL_PARAMS * pB = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];
            BALL_PARAMS &pB = aBallTypes[i].Balls[balls_size + j];
            pSL->Load2Buffer(&pB);
            if (pB.pParticle)
            {
                pB.pParticle = nullptr;
                if (auto eidParticle = EntityManager::GetEntityId("particles"))
                {
                    pB.pParticle = (VPARTICLE_SYSTEM *)core.Send_Message(
                        eidParticle, "lsffffffl", PS_CREATE_RIC, (char *)aBallTypes[i].sParticleName.c_str(), pB.vPos.x,
                        pB.vPos.y, pB.vPos.z, 0.0f, 1.0f, 0.0f, 100000);
                }
            }
        }
    }
}
