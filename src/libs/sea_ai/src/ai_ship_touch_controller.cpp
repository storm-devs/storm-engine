#include "ai_ship.h"
#include "inlines.h"

AIShipTouchController::AIShipTouchController(AIShip *_pAIShip)
{
    SetAIShip(_pAIShip);
    fRaySize = 50.0f;

    dtTouchTime.Setup(0.0f, 1.0f, 1.2f);

    for (uint32_t i = 0; i < 15; i++)
        aTouchRays.push_back(ray_t{0.0f, 1.0f, 0.0f}); //[aTouchRays.Add()].fDist = 1.0f;
}

AIShipTouchController::~AIShipTouchController()
{
}

bool AIShipTouchController::Init()
{
    return true;
}

bool AIShipTouchController::isCollision2D(const CVECTOR &vSrc, const CVECTOR &vDst)
{
    CVECTOR vRes;

    auto *const vPentaBox = GetPentagonBox();

    for (uint32_t j = 0; j < 5; j++)
        if (IntersectLines2D(vSrc, vDst, vPentaBox[j], vPentaBox[(j == 4) ? 0 : j + 1], vRes))
            return true;

    return false;
}

CVECTOR *AIShipTouchController::GetPentagonBox()
{
    CMatrix m;
    const auto vOurPos = GetAIShip()->GetPos();

    const auto vBoxSize = GetAIShip()->GetBoxsize() / 2.0f;
    m.BuildRotateY(GetAIShip()->GetAng().y);
    vBox[0] = CVECTOR(-vBoxSize.x, 0.0f, -vBoxSize.z);
    vBox[1] = CVECTOR(vBoxSize.x, 0.0f, -vBoxSize.z);
    vBox[2] = CVECTOR(vBoxSize.x, 0.0f, vBoxSize.z / 1.5f);
    vBox[3] = CVECTOR(0.0f, 0.0f, vBoxSize.z);
    vBox[4] = CVECTOR(-vBoxSize.x, 0.0f, vBoxSize.z / 1.5f);
    for (auto &i : vBox)
        i = vOurPos + m * i;
    return &vBox[0];
}

void AIShipTouchController::TestCollision(AIShip *pOtherShip)
{
    Assert(pOtherShip);
    if (GetAIShip() == pOtherShip)
        return;

    uint32_t i;
    CMatrix m;
    CVECTOR vBox[5], vBoxSize;
    CVECTOR vOurPos, vEnemyPos, vEnemyPredictionPos;

    const auto fSpeed = GetAIShip()->GetShipBasePointer()->State.vSpeed.z;
    if (fSpeed < 0.1f)
        return;

    vOurPos = GetAIShip()->GetPos();
    vEnemyPos = pOtherShip->GetPos();
    pOtherShip->GetPrediction(10.0f, &vEnemyPredictionPos, nullptr);
    vEnemyPos = vEnemyPredictionPos;

    vBoxSize = pOtherShip->GetBoxsize() / 2.0f;
    m.BuildRotateY(pOtherShip->GetAng().y);
    vBox[0] = CVECTOR(-vBoxSize.x, 0.0f, -vBoxSize.z);
    vBox[1] = CVECTOR(vBoxSize.x, 0.0f, -vBoxSize.z);
    vBox[2] = CVECTOR(vBoxSize.x, 0.0f, vBoxSize.z / 1.5f);
    vBox[3] = CVECTOR(0.0f, 0.0f, vBoxSize.z);
    vBox[4] = CVECTOR(-vBoxSize.x, 0.0f, vBoxSize.z / 1.5f);
    for (i = 0; i < 5; i++)
        vBox[i] = vEnemyPos + m * vBox[i];

    for (i = 0; i < aTouchRays.size(); i++)
    {
        const auto fAng =
            GetAIShip()->GetAng().y + static_cast<float>(i) / static_cast<float>(aTouchRays.size()) * PIm2;
        auto vPos = vOurPos + fRaySize * CVECTOR(sinf(fAng), 0.0f, cosf(fAng));
        for (uint32_t j = 0; j < 5; j++)
        {
            CVECTOR vRes;

            if (IntersectLines2D(vOurPos, vPos, vBox[j], vBox[(j == 4) ? 0 : j + 1], vRes))
            {
                vOurPos.y = vRes.y = 0.0f;
                const auto fDist = sqrtf(~(vOurPos - vRes));
                if (fDist < aTouchRays[i].fMinDist)
                {
                    aTouchRays[i].fMinDist = fDist;
                    aTouchRays[i].fDist = fDist / fRaySize;
                }
            }
        }
    }
}

void AIShipTouchController::Execute(float fDeltaTime)
{
    uint32_t i, j;
    CMatrix m;
    CVECTOR vBoxSize; // , v[4];

    if (GetAIShip()->isMainCharacter())
        return;

    auto *pShip = static_cast<SHIP_BASE *>(GetAIShip()->GetShipPointer());
    Assert(pShip);

    vBoxSize = GetAIShip()->GetBoxsize() / 2.0f;
    const auto fSpeed = KNOTS2METERS(pShip->State.vSpeed.z);
    fRaySize = vBoxSize.z * 5.0f + fSpeed * 10.0f;

    /*vBoxSize = GetAIShip()->GetBoxsize() / 2.0f;
    m.BuildRotateY(GetAIShip()->GetAng().y);
    v[0] = m * CVECTOR(-vBoxSize.x, 0.0f, -vBoxSize.z);
    v[1] = m * CVECTOR(vBoxSize.x, 0.0f, -vBoxSize.z);
    v[2] = m * CVECTOR(vBoxSize.x, 0.0f, vBoxSize.z);
    v[3] = m * CVECTOR(-vBoxSize.x, 0.0f, vBoxSize.z);*/

    // reset
    if (dtTouchTime.Update(fDeltaTime))
    {
        for (i = 0; i < aTouchRays.size(); i++)
        {
            aTouchRays[i].fMinDist = 10000.0f;
            aTouchRays[i].fDist = 1.0f;
        }

        // test collision with ships
        for (i = 0; i < AIShip::AIShips.size(); i++)
        {
            TestCollision(AIShip::AIShips[i]);
        }
        // test collision with island
        const auto vOurPos = GetAIShip()->GetPos();
        if (AIHelper::pIsland)
        {
            for (i = 0; i < aTouchRays.size(); i++)
            {
                const auto fAng =
                    GetAIShip()->GetAng().y + static_cast<float>(i) / static_cast<float>(aTouchRays.size()) * PIm2;
                auto vPos = vOurPos + fRaySize * CVECTOR(sinf(fAng), 0.0f, cosf(fAng));
                const auto fRes = static_cast<COLLISION_OBJECT *>(AIHelper::pIsland)->Trace(vOurPos, vPos);
                if (fRes < 1.0f)
                {
                    aTouchRays[i].fDist *= fRes;
                }
            }
        }

        // rotate calculate section
        uint32_t dwMaxIdx = 0;
        auto fMaxDist = -100.0f;
        uint32_t ii, i1, i2;
        for (i = 0; i < aTouchRays.size() / 2; i++)
        {
            for (j = 0; j < 2; j++)
            {
                ii = (j == 0) ? i : (aTouchRays.size() - i - 1);
                i1 = (ii == 0) ? aTouchRays.size() - 1 : (ii - 1);
                i2 = (ii == aTouchRays.size() - 1) ? 0 : (ii + 1);
                const auto fTriRotator = (aTouchRays[ii].fDist + aTouchRays[i1].fDist + aTouchRays[i2].fDist) / 3.0f;
                if (fTriRotator - fMaxDist > 0.001f)
                {
                    fMaxDist = fTriRotator;
                    dwMaxIdx = ii;
                }
            }
        }

        // stop calculate section
        auto fClosed = 0.0f;
        if (aTouchRays[0].fDist < 1.0f)
            fClosed = aTouchRays.size() / 4.0f;
        for (i = 1; i < aTouchRays.size() / 4; i++)
        {
            i1 = i;
            i2 = aTouchRays.size() - i;
            if (aTouchRays[i1].fDist < 1.0f)
                fClosed += 0.38f;
            if (aTouchRays[i2].fDist < 1.0f)
                fClosed += 0.38f;
        }

        // get out from my road section
        for (i = 1; i < aTouchRays.size() / 4; i++)
        {
            // i1 = ;
        }

        // search new free direction

        fRotateFactor = 0.0f;
        if (dwMaxIdx != 0)
            fRotateFactor = (dwMaxIdx < aTouchRays.size() / 2) ? 2.5f : -2.5f;
        // float fRotate = GetBestRotateDirection() * 2.5f;
        GetAIShip()->GetRotateController()->AddRotate(fRotateFactor);

        fClosed *= 1.6f;
        fSpeedFactor = 1.0f - fClosed / (aTouchRays.size() / 2.0f);
        fSpeedFactor = CLAMP(fSpeedFactor);
        // pShip->SetSpeed(fSpeedFactor);
    }
    else
    {
        GetAIShip()->GetRotateController()->AddRotate(fRotateFactor);
        GetAIShip()->GetSpeedController()->MulSpeed(fSpeedFactor);
    }
}

float AIShipTouchController::GetBestRotateDirection()
{
    auto fLeft = 0.0f, fRight = 0.0f;
    const uint32_t iBortNum = (aTouchRays.size() - 1) / 2;

    for (uint32_t i = 0; i < aTouchRays.size(); i++)
        if (i != 0)
        {
            if (i <= iBortNum)
                fRight += aTouchRays[i].fDist;
            else
                fLeft += aTouchRays[i].fDist;
        }

    if (fabsf(fLeft - fRight) < 0.5f)
        return 0.0f;

    return (fLeft > fRight) ? -1.0f : 1.0f;
}

void AIShipTouchController::Realize(float fDeltaTime)
{
}

void AIShipTouchController::Save(CSaveLoad *pSL)
{
    pSL->SaveDword(aTouchRays.size());
    for (auto &aTouchRay : aTouchRays)
    {
        pSL->SaveFloat(aTouchRay.fMinDist);
        pSL->SaveFloat(aTouchRay.fDist);
        pSL->SaveFloat(aTouchRay.fEarthDist);
    }
    pSL->SaveFloat(fLeftRaysFree);
    pSL->SaveFloat(fRightRaysFree);
    pSL->SaveFloat(fRaySize);
    pSL->SaveFloat(fSpeedFactor);
    pSL->SaveFloat(fRotateFactor);
    for (auto i : vBox)
        pSL->SaveVector(i);
}

void AIShipTouchController::Load(CSaveLoad *pSL)
{
    const auto dwNum = pSL->LoadDword();
    for (uint32_t i = 0; i < dwNum; i++)
        aTouchRays.push_back(ray_t{pSL->LoadFloat(), pSL->LoadFloat(), pSL->LoadFloat()});
    fLeftRaysFree = pSL->LoadFloat();
    fRightRaysFree = pSL->LoadFloat();
    fRaySize = pSL->LoadFloat();
    fSpeedFactor = pSL->LoadFloat();
    fRotateFactor = pSL->LoadFloat();
    for (auto &i : vBox)
        i = pSL->LoadVector();
}
