#include "ai_balls.h"
#include "ai_fort.h"
#include "math_inlines.h"
#include "shared/messages.h"


AIBalls *AIBalls::pAIBalls = nullptr;

AIBalls::AIBalls()
    : pSail(nullptr), pSea(nullptr), pFort(nullptr), pIsland(nullptr),
      fDeltaTimeMultiplier(1.0f), fBallFlySoundDistance(1.0f), dwFireBallFromCameraTime(0)
{
    pAIBalls = this;
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
            pBall->sBallEvent.clear();
        }
        aBallTypes[i].Balls.clear();
    }
    aBallTypes.clear();
}

bool AIBalls::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
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
    auto iMainCharIndex = pMainCharIndex->GetInt();
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
    const char *pBallName = pABall->GetAttribute("Type");
    Assert(pBallName);

    uint32_t i;
    for (i = 0; i < aBallTypes.size(); i++)
        if (storm::iEquals(aBallTypes[i].sName, pBallName))
            break;
    if (i == aBallTypes.size())
        return;

    aBallTypes[i].Balls.push_back(BALL_PARAMS{});
    // BALL_PARAMS * pBall = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];
    auto *pBall = &aBallTypes[i].Balls.back();

    pBall->iBallOwner = pABall->GetAttributeAsDword("CharacterIndex");

    pBall->fTime = 0.0f;
    pBall->vPos = pBall->vFirstPos =
        CVECTOR(pABall->GetAttributeAsFloat("x"), pABall->GetAttributeAsFloat("y"), pABall->GetAttributeAsFloat("z"));
    pBall->fSpeedV0 = pABall->GetAttributeAsFloat("SpdV0");
    pBall->fHeightMultiply = pABall->GetAttributeAsFloat("HeightMultiply");
    pBall->fSizeMultiply = pABall->GetAttributeAsFloat("SizeMultiply");
    pBall->fTimeSpeedMultiply = pABall->GetAttributeAsFloat("TimeSpeedMultiply");
    pBall->dwCannonType = pABall->GetAttributeAsDword("CannonType");
    pBall->fMaxFireDistance = pABall->GetAttributeAsFloat("MaxFireDistance");
    const auto fAngle = pABall->GetAttributeAsFloat("Ang");
    pBall->fCosAngle = cosf(fAngle);
    pBall->fSinAngle = sinf(fAngle);
    const auto fDir = pABall->GetAttributeAsFloat("Dir");
    pBall->fDirX = cosf(fDir);
    pBall->fDirZ = sinf(fDir);
    pBall->pParticle = nullptr;

    pBall->sBallEvent = to_string(pABall->GetAttribute("Event"));
    
    if (aBallTypes[i].sParticleName.size())
    {
        entid_t eidParticle;
        if (eidParticle = core.GetEntityId("particles"))
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

    if (!pIsland && (EID = core.GetEntityId("island")))
        pIsland = static_cast<CANNON_TRACE_BASE *>(core.GetEntityPointer(EID));
    if (!pSail && (EID = core.GetEntityId("sail")))
        pSail = static_cast<CANNON_TRACE_BASE *>(core.GetEntityPointer(EID));
    if (!pSea && (EID = core.GetEntityId("sea")))
        pSea = static_cast<CANNON_TRACE_BASE *>(core.GetEntityPointer(EID));

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
            pBall->fTime += fDeltaTime * fDeltaTimeMultiplier * pBall->fTimeSpeedMultiply;
            // update positions
            float fsX = pBall->fSpeedV0 * pBall->fTime * pBall->fCosAngle;
            float fsY = pBall->fHeightMultiply * (pBall->fSpeedV0 * pBall->fTime * pBall->fSinAngle -
                                                  AIHelper::fGravity * SQR(pBall->fTime) / 2.0f);
            pBall->vPos = CVECTOR(0.0f, fsY, fsX);
            RotateAroundY(pBall->vPos.x, pBall->vPos.z, pBall->fDirX, pBall->fDirZ);
            pBall->vPos += pBall->vFirstPos;

            vDst = pBall->vPos;

            if (!pBall->sBallEvent.empty())
                core.Event(pBall->sBallEvent.c_str(), "lllffffffs", pBall->iBallOwner, static_cast<uint32_t>(1),
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
                    CVECTOR vRes, v = fBallFlySoundStereoMultiplier * CVECTOR(x, y, 0.0f);
                    mView.MulToInv(v, vRes);

                    core.Event(BALL_FLY_NEAR_CAMERA, "fff", vRes.x, vRes.y, vRes.z);
                }
            }

            // sail trace
            if (pSail)
                pSail->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
            
            auto &&entities = core.GetEntityIds(SHIP_CANNON_TRACE);
            for (auto ent_id : entities)
            {
                if (auto *pShip = static_cast<CANNON_TRACE_BASE *>(core.GetEntityPointer(ent_id)))
                {
                    fRes = pShip->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
                    if (fRes <= 1.0f)
                        break;
                }
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
                if (!pBall->sBallEvent.empty())
                {
                    core.Event(pBall->sBallEvent.c_str(), "lllffffff", pBall->iBallOwner, static_cast<uint32_t>(0),
                               pBallsType->dwGoodIndex, pBall->vPos.x, pBall->vPos.y, pBall->vPos.z, vSrc.x, vSrc.y,
                               vSrc.z);
                    pBall->sBallEvent.clear();
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

            if (pBall->sBallEvent.empty())
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
        for (auto &aBallType : aBallTypes)
        {
            BALL_TYPE *pBallsType = &aBallType;

            for (auto &Ball : pBallsType->Balls)
            {
                BALL_PARAMS *pBall = &Ball;

                pBall->sBallEvent.clear();

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
        fBallFlySoundStereoMultiplier = AttributesPointer->GetAttributeAsFloat("BallFlySoundStereoMultiplyer");
        fDeltaTimeMultiplier = AttributesPointer->GetAttributeAsFloat("SpeedMultiply");
        sTextureName = to_string(AttributesPointer->GetAttribute("Texture"));
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
                ballType.sParticleName = to_string(pAP->GetAttribute("Particle"));

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
        for (auto &aBallType : aBallTypes)
            for (uint32_t j = 0; j < aBallType.Balls.size(); j++)
            {
                BALL_PARAMS *pBall = &aBallType.Balls[j];

                if (pBall->pParticle)
                {
                    pBall->pParticle->Stop();
                    STORM_DELETE(pBall->pParticle);
                }
            }
    }
    return 0;
}

void BALL_PARAMS::Save(CSaveLoad *pSL)
{
    pSL->SaveVector(vFirstPos);
    pSL->SaveVector(vPos);
    // pSL->SaveDword(reinterpret_cast<uint32_t>(pParticle));
    pSL->SaveQword(reinterpret_cast<uint64_t>(pParticle));
    pSL->SaveString(sBallEvent);
    pSL->SaveLong(iBallOwner);
    pSL->SaveFloat(fTime);
    pSL->SaveFloat(fSpeedV0);
    pSL->SaveFloat(fDirX);
    pSL->SaveFloat(fDirZ);
    pSL->SaveFloat(fSinAngle);
    pSL->SaveFloat(fCosAngle);
    pSL->SaveFloat(fHeightMultiply);
    pSL->SaveFloat(fSizeMultiply);
    pSL->SaveFloat(fTimeSpeedMultiply);
    pSL->SaveFloat(fMaxFireDistance);
    pSL->SaveDword(dwCannonType);
}

void AIBalls::Save(CSaveLoad *pSL)
{
    for (auto &aBallType : aBallTypes)
    {
        pSL->SaveDword(aBallType.Balls.size());

        for (uint32_t j = 0; j < aBallType.Balls.size(); j++)
        {
            aBallType.Balls[j].Save(pSL);
        }
    }
}

void BALL_PARAMS::Load(CSaveLoad *pSL)
{
    vFirstPos = pSL->LoadVector();
    vPos = pSL->LoadVector();
    //pParticle = reinterpret_cast<VPARTICLE_SYSTEM *>(pSL->LoadDword());
    pParticle = reinterpret_cast<VPARTICLE_SYSTEM *>(pSL->LoadQword());
    sBallEvent = pSL->LoadString();
    iBallOwner = pSL->LoadLong();
    fTime = pSL->LoadFloat();
    fSpeedV0 = pSL->LoadFloat();
    fDirX = pSL->LoadFloat();
    fDirZ = pSL->LoadFloat();
    fSinAngle = pSL->LoadFloat();
    fCosAngle = pSL->LoadFloat();
    fHeightMultiply = pSL->LoadFloat();
    fSizeMultiply = pSL->LoadFloat();
    fTimeSpeedMultiply = pSL->LoadFloat();
    fMaxFireDistance = pSL->LoadFloat();
    dwCannonType = pSL->LoadDword();
}

void AIBalls::Load(CSaveLoad *pSL)
{
    for (auto &aBallType : aBallTypes)
    {
        const uint32_t dwNum = pSL->LoadDword();

        auto balls_size = std::size(aBallType.Balls);
        aBallType.Balls.resize(balls_size + dwNum);
        for (uint32_t j = 0; j < dwNum; j++)
        {
            // BALL_PARAMS * pB = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];
            BALL_PARAMS &pB = aBallType.Balls[balls_size + j];
            pB.Load(pSL);
            if (pB.pParticle)
            {
                pB.pParticle = nullptr;
                if (auto eidParticle = core.GetEntityId("particles"))
                {
                    pB.pParticle = (VPARTICLE_SYSTEM *)core.Send_Message(
                        eidParticle, "lsffffffl", PS_CREATE_RIC, (char *)aBallType.sParticleName.c_str(), pB.vPos.x,
                        pB.vPos.y, pB.vPos.z, 0.0f, 1.0f, 0.0f, 100000);
                }
            }
        }
    }
}
