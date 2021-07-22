#include "AIFort.h"
#include "AIShip.h"

AIShipCameraController::AIShipCameraController(AIShip *_pAIShip)
{
    dwTarget = RELATION_UNKNOWN;
    pTargetAPointer = nullptr;
    fDelta = 0.0f;
    iCrosshairTex = -1;
    pACrosshair = nullptr;
    SetAIShip(_pAIShip);
}

AIShipCameraController::~AIShipCameraController()
{
    if (iCrosshairTex >= 0)
        AIHelper::pRS->TextureRelease(iCrosshairTex);
}

bool AIShipCameraController::Init()
{
    pACrosshair = AIHelper::pASeaCameras->GetAttributeClass("Crosshair");
    Assert(pACrosshair);
    iCrosshairTex = AIHelper::pRS->TextureCreate(pACrosshair->GetAttribute("Texture"));
    dwSubTexturesX = pACrosshair->GetAttributeAsDword("SubTexX");
    dwSubTexturesY = pACrosshair->GetAttributeAsDword("SubTexY");
    auto *pAColors = pACrosshair->GetAttributeClass("colors");
    if (pAColors)
    {
        Colors[RELATION_UNKNOWN] = pAColors->GetAttributeAsDword("default");
        Colors[RELATION_ENEMY] = pAColors->GetAttributeAsDword("enemy");
        Colors[RELATION_FRIEND] = pAColors->GetAttributeAsDword("friend");
        Colors[RELATION_NEUTRAL] = pAColors->GetAttributeAsDword("neutral");
    }
    else
        core.Trace("AIShipCameraController:: Attributes Crosshair.Colors not found!");
    return true;
}

void AIShipCameraController::Execute(float fDeltaTime)
{
    bCameraOutside = pACrosshair->GetAttributeAsDword("OutsideCamera") != 0;

    pTargetAPointer = nullptr;
    dwTarget = RELATION_UNKNOWN;
    auto vOurPos = GetAIShip()->GetPos();

    if (isCameraOutside())
    {
        fDelta = 0.0f;
    }
    else
    {
        uint32_t i, j, iMax;
        CMatrix m;

        AIHelper::pRS->GetTransform(D3DTS_VIEW, m);
        m.Transposition();
        const auto vZ = m.Vz();

        auto bEnemy = false;
        if (GetAIShip()->GetCannonController()->isCanFire(vZ))
        {
            auto fBestDistance = 1e8f;
            auto vFirePos = GetAIShip()->GetCannonController()->GetFirePos(vZ);

            const auto vFireDir = !(vFirePos - vOurPos);
            const auto fFireDist = sqrtf(~(vFirePos - vOurPos));
            vFirePos.y = 0.0f;
            // check Ships
            for (i = 0; i < AIShip::AIShips.size(); i++)
                if (GetAIShip() != AIShip::AIShips[i])
                {
                    auto vFakeShipPos = AIShip::AIShips[i]->GetPos() + (!(AIShip::AIShips[i]->GetPos() - vOurPos)) *
                                                                           AIShip::AIShips[i]->GetBoxsize().z * 0.8f;
                    AIHelper::pRS->DrawSphere(vFakeShipPos, 2.0f, 0xFF00FF00);
                    const auto fDistance = (vFakeShipPos - vFirePos).GetLength2D();
                    // AIShip::AIShips[i]->GetDistance(vFirePos);
                    if (fDistance <= 0.8f * AIShip::AIShips[i]->GetBoxsize().z)
                    {
                        if (fDistance > fBestDistance)
                            continue;
                        fBestDistance = fDistance;

                        pTargetAPointer = AIShip::AIShips[i]->GetACharacter();
                        dwTarget =
                            Helper.GetRelation(GetAIShip()->GetACharacter(), AIShip::AIShips[i]->GetACharacter());
                    }
                }

            // check Forts cannons
            if (AIFort::pAIFort)
                for (i = 0; i < AIFort::pAIFort->GetNumForts(); i++)
                {
                    auto *pF = AIFort::pAIFort->GetFort(i);
                    iMax = pF->GetAllCannonsNum(); // boal
                    for (j = 0; j < iMax; j++)
                    {
                        auto *const pC = pF->GetCannon(j);
                        if (pC->isDamaged())
                            continue;
                        const auto fDistance = pC->GetDistance(vOurPos);
                        if (fDistance > fFireDist)
                            continue;
                        if (fDistance > fBestDistance)
                            continue;
                        const auto fDot = vFireDir | (!(pC->GetPos() - vOurPos));
                        if (fDot < 0.0f)
                            continue;

                        auto vFire2CannonPos = GetAIShip()->GetCannonController()->GetFirePos(vZ, fDistance);
                        const auto fRealDistance = ~(vFire2CannonPos - pC->GetPos());
                        if (fRealDistance > SQR(8.0f))
                            continue;

                        pTargetAPointer = pF->GetACharacter();
                        dwTarget = Helper.GetRelation(GetAIShip()->GetACharacter(), pF->GetACharacter());
                    }
                }

            if (dwTarget != RELATION_UNKNOWN)
            {
                const auto fRealDeltaTime =
                    (core.GetDeltaTime() == 0) ? 0.0f : static_cast<float>(core.GetRDeltaTime()) * 0.001f;
                fDelta += fRealDeltaTime * 5.0f;
                while (fDelta > 2.0f)
                {
                    fDelta -= static_cast<float>(static_cast<long>(fDelta / 2.0f) * 2);
                }
            }
        }
    }
}

bool AIShipCameraController::Fire()
{
    CMatrix m;

    AIHelper::pRS->GetTransform(D3DTS_VIEW, m);
    m.Transposition();
    const auto vZ = m.Vz();
    // CVECTOR vZ = m * CVECTOR(0.0f, 0.0f, 1.0f);
    // vZ = !CVECTOR(vZ.x, 0.0f, vZ.z);
    if (!GetAIShip()->GetCannonController()->isCanFire(vZ))
        return false;

    if (dwTarget != RELATION_UNKNOWN && pTargetAPointer)
    {
        auto *const pMainGroupCharacter = Helper.GetMainCharacter(pTargetAPointer);
        core.Event(SHIP_FIRE_ACTION, "aal", pTargetAPointer, pMainGroupCharacter, dwTarget);
    }

    // if (pTargetAPointer) AIGroup::GroupSetAttack(GetAIShip()->GetACharacter(), pTargetAPointer);
    return GetAIShip()->GetCannonController()->Fire(m.Pos(), vZ);
}

void AIShipCameraController::Realize(float fDeltaTime)
{
    if (isCameraOutside())
        return;

    CMatrix m;
    RS_RECT rCam;

    // AIHelper::pRS->GetTransform(D3DTS_VIEW, m); m.Transposition();    CVECTOR vZ = m.Vz();
    const auto mOldView = m;
    AIHelper::pRS->GetTransform(D3DTS_VIEW, mOldView);
    m = mOldView;
    m.Transposition();
    const auto vZ = m.Vz();
    /*CVECTOR campos,camang;
    float campersp;
    AIHelper::pRS->GetCamera(campos,camang,campersp);
    m.BuildMatrix(camang);
    CVECTOR vZ = m.Vz();*/
    if (!GetAIShip()->GetCannonController()->isCanFire(vZ))
        return;

    m.SetIdentity();
    AIHelper::pRS->SetTransform(D3DTS_VIEW, m);

    rCam.dwColor = Colors[dwTarget];

    // AIHelper::pRS->Print(0,0,"%d",rCam.dwColor);

    rCam.fAngle = 0.0f;
    const auto fSizeMultiply = 0.9f + (0.2f * ((fDelta > 1.0f) ? fDelta : (2.0f - fDelta)));
    rCam.fSize = fSizeMultiply * pACrosshair->GetAttributeAsFloat("Size");
    // rCam.vPos = campos + m * CVECTOR(0.0f,0.0f,1.0f);
    rCam.vPos = m * CVECTOR(0.0f, 0.0f, 1.0f);
    rCam.dwSubTexture = 0;

    AIHelper::pRS->TextureSet(0, iCrosshairTex);
    AIHelper::pRS->DrawRects(&rCam, 1, pACrosshair->GetAttribute("Technique"), dwSubTexturesX, dwSubTexturesY);

    AIHelper::pRS->SetTransform(D3DTS_VIEW, mOldView);
}

void AIShipCameraController::Save(CSaveLoad *pSL) const
{
    pSL->SaveDword(dwTarget);
    pSL->SaveFloat(fDelta);
    pSL->SaveDword(bCameraOutside);
}

void AIShipCameraController::Load(CSaveLoad *pSL)
{
    Init();

    dwTarget = pSL->LoadDword();
    fDelta = pSL->LoadFloat();
    bCameraOutside = pSL->LoadDword() != 0;

    pACrosshair->SetAttributeUseDword("OutsideCamera", bCameraOutside);
}
