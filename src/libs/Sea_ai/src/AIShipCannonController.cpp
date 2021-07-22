#include "AIShip.h"
#include "inlines.h"

float AIShipCannonController::fMaxCannonDamageDistance = 1.0f;
float AIShipCannonController::fMaxCannonDamageRadiusPoint = 1.0f;

AIShipCannonController::AIShipCannonController(AIShip *_pShip)
{
    SetAIShip(_pShip);
    bReload = false;
    bTempFlag = false;
    bNotEnoughBalls = false;

    rs.dwColor = 0xFFFFFF;
    rs.vPos = CVECTOR(10000.0f, 0.0f, 0.0f);
    rs.dwSubTexture = -1;
    rs.fAngle = 0.0f;
    rs.fSize = 1.0f;
}

AIShipCannonController::~AIShipCannonController()
{
}

bool AIShipCannonController::Fire2Position(uint32_t dwBort, const CVECTOR &vFirePos, float fFireHeight)
{
    Assert(dwBort < aShipBorts.size());
    auto *pBort = &aShipBorts[dwBort];

    const auto bEnoughBalls = isHaveEnoughtBallsForBort(dwBort);
    if (!bEnoughBalls)
        bNotEnoughBalls = true;
    if (!pBort->isCharged() || !pBort->aCannons.size() || pBort->isBortDamaged())
        return false;

    auto vFireDir = !(vFirePos - pOurAIShip->GetPos());
    auto fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));

    auto vTempFirePos = vFirePos + CVECTOR(0.0f, fFireHeight, 0.0f);

    auto *pVData = core.Event(SHIP_GET_BORT_FIRE_DELTA, "afff", GetAIShip()->GetACharacter(), vTempFirePos.x,
                              vTempFirePos.y, vTempFirePos.z);
    const auto fRadius = pVData->GetFloat();
    const auto fAng = FRAND(PIm2);
    vTempFirePos += CVECTOR(fRadius * sinf(fAng), 0.0f, fRadius * cosf(fAng));

    const auto vTempFireDir = !(vTempFirePos - pOurAIShip->GetPos());

    core.Event(SHIP_BORT_FIRE, "lisffffff", GetIndex(GetAIShip()->GetACharacter()), GetAIShip()->GetShipEID(),
               (char *)aShipBorts[dwBort].sName.c_str(), vTempFirePos.x, vTempFirePos.y, vTempFirePos.z, vTempFireDir.x,
               vTempFireDir.y, vTempFireDir.z);

    for (uint32_t j = 0; j < pBort->aCannons.size(); j++)
    {
        auto *pCannon = &pBort->aCannons[j];
        if (pCannon->isDamaged())
            continue;
        pCannon->SetRechargeEnable(bEnoughBalls);
        pCannon->Fire(pBort->fSpeedV0, vTempFirePos);
    }
    pBort->ClearCharge(); // FIX-ME

    return true;
}

bool AIShipCannonController::Fire(const CVECTOR &vFireCamPos, const CVECTOR &vFireDir)
{
    const auto vOurPos = GetAIShip()->GetPos();
    const auto fSpeedV0 = GetSpeedV0();

    const auto vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
    float fAngle = acosf(Clamp(vTempDir | vFireDir));
    if (vFireDir.y < 0)
        fAngle = -fAngle;

    CVECTOR vTempFirePos = vOurPos + !vTempDir;
    vTempFirePos.y = 0.0f;

    uint32_t dwBort = GetFirstFireBort(vTempFirePos);
    if (dwBort != INVALID_BORT_INDEX)
        do
        {
            const float fMaxDist =
                AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

            CVECTOR vFirePos = vFireCamPos + fMaxDist * !vFireDir;
            vFirePos.y = 0.0f;

            Fire2Position(dwBort, vFirePos, 0.0f);
        } while ((dwBort = GetNextFireBort(dwBort, vTempFirePos)) != INVALID_BORT_INDEX);

    return true;
}

bool AIShipCannonController::Fire(const CVECTOR &vFirePos)
{
    uint32_t dwBort = GetFirstFireBort(vFirePos);
    if (dwBort != INVALID_BORT_INDEX)
        do
        {
            Fire2Position(dwBort, vFirePos, -1.0f);
        } while ((dwBort = GetNextFireBort(dwBort, vFirePos)) != INVALID_BORT_INDEX);
    return true;
}

bool AIShipCannonController::Fire(AIShip *pEnemy)
{
    Assert(pEnemy);

    float fFireHeight;

    VDATA *pVData = core.Event(CANNON_GET_FIRE_HEIGHT, "aa", pOurAIShip->GetACharacter(), pEnemy->GetACharacter());
    Assert(pVData);
    fFireHeight = pVData->GetFloat();

    const float fDistance = GetAIShip()->GetDistance(*pEnemy);
    const float fSpeedZ = pEnemy->GetShipBasePointer()->GetCurrentSpeed();
    const float fAng = pEnemy->GetShipBasePointer()->GetAng().y;
    CVECTOR vFirePos = (fSpeedZ * fDistance / GetSpeedV0()) * CVECTOR(sinf(fAng), 0.0f, cosf(fAng));
    vFirePos += pEnemy->GetPos();

    rs.vPos = vFirePos;

    uint32_t dwBort = GetFirstFireBort(vFirePos);
    if (dwBort != INVALID_BORT_INDEX)
        do
        {
            Fire2Position(dwBort, vFirePos, fFireHeight);
        } while ((dwBort = GetNextFireBort(dwBort, vFirePos)) != INVALID_BORT_INDEX);
    return true;
}

void AIShipCannonController::Execute(float fDeltaTime)
{
    uint32_t i, j;

    UpdateParameters();

    const CVECTOR vOurPos = GetAIShip()->GetPos();

    if (bReload)
    {
        bNotEnoughBalls = false;

        for (i = 0; i < aShipBorts.size(); i++)
        {
            const bool bEnoughBalls = isHaveEnoughtBallsForBort(i);
            if (!bEnoughBalls)
                bNotEnoughBalls = true;
            for (j = 0; j < aShipBorts[i].aCannons.size(); j++)
            {
                AICannon *pC = &aShipBorts[i].aCannons[j];
                if (pC->isDamaged())
                    continue;
                pC->SetRechargeEnable(bEnoughBalls);
                pC->Recharge();
            }
        }
        bReload = false;
    }

    if (GetAIShip()->isMainCharacter())
    {
        bNotEnoughBalls = false;
        for (i = 0; i < aShipBorts.size(); i++)
        {
            if (isHaveEnoughtBallsForBort(i))
                continue;
            bNotEnoughBalls = true;
            break;
        }
        core.Event(SHIP_NOT_ENOUGH_BALLS, "l", bNotEnoughBalls);
    }

    for (i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            AISHIP_BORT *pBort = &aShipBorts[i];

            // set maximum MaxFireDistance for all cannons
            const float fMaxBortFireDistance =
                AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, GetSpeedV0(), pBort->fFireAngMax);
            for (uint32_t j = 0; j < pBort->aCannons.size(); j++)
            {
                pBort->aCannons[j].SetMaxFireDistance(fMaxBortFireDistance);
            }

            bool bReloaded = pBort->fChargePercent < 0.999f;

            long dwReadyCannonsAfter = 0, dwReadyCannonsBefore = 0;

            pBort->fChargePercent = 0.0f;
            for (uint32_t j = 0; j < pBort->aCannons.size(); j++)
            {
                AICannon *pC = &pBort->aCannons[j];

                if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty()))
                    dwReadyCannonsBefore++;

                if (pC->isDamaged())
                    continue;

                pC->Execute(fDeltaTime);
                pBort->fChargePercent += pC->GetRechargePercent();

                if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty()))
                    dwReadyCannonsAfter++;
            }
            // prevent division by zero (mitrocosta)
            auto intactCannons = GetBortIntactCannonsNum(i);
            if (intactCannons > 0)
            {
                pBort->fChargePercent /= intactCannons;
            }
            else
            {
                pBort->fChargePercent = .0f;
            }

            if (dwReadyCannonsBefore != dwReadyCannonsAfter && dwReadyCannonsAfter == GetBortIntactCannonsNum(i))
            // pBort->fChargePercent >= 0.999f)
            {
                core.Event(SHIP_BORT_RELOADED, "as", GetAIShip()->GetACharacter(), pBort->sName.c_str());
                // core.Trace("%s bort is reloaded", pBort->sName.c_str());
            }

            // update borts parameters for script
            // if (GetAIShip()->isMainCharacter())
            {
                ATTRIBUTES *pAPlayer = GetAIShip()->GetACharacter();
                ATTRIBUTES *pABorts = pAPlayer->FindAClass(pAPlayer, "Ship.Cannons.Borts");
                Assert(pABorts);
                pABorts->SetAttribute((char *)pBort->sName.c_str(), "");
                ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, (char *)pBort->sName.c_str());
                Assert(pACurBort);

                pACurBort->SetAttributeUseFloat("MaxFireDistance", pBort->fMaxFireDistance);
                pACurBort->SetAttributeUseFloat("ChargeRatio", pBort->fChargePercent);
                pACurBort->SetAttributeUseFloat("DamageRatio",
                                                1.0f - (static_cast<float>(GetBortIntactCannonsNum(i)) +
                                                        static_cast<float>(GetBortDisabledCannonsNum(i))) /
                                                           static_cast<float>(pBort->aCannons.size()));
                //        pACurBort->SetAttributeUseFloat("DamageRatio",
                //                                        1.0f - static_cast<float>(GetBortIntactCannonsNum(i)) /
                //                                        static_cast<float>(pBort
                //                                          ->aCannons.size()));
            }
        }
}

struct tr_vertex
{
    CVECTOR vPos;
    uint32_t dwColor;
};

std::vector<tr_vertex> Verts;
#define TR_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE)

void AIShipCannonController::AddTrg(CVECTOR *pVerts, uint32_t dwColor)
{
    for (uint32_t i = 0; i < 3; i++)
        Verts.push_back(tr_vertex{pVerts[i], dwColor});
}

// return (fire distance) angle
float AIShipCannonController::GetBortHeightAngle(long iBortIndex)
{
    CMatrix m;
    CVECTOR v;

    GetAIShip()->GetMatrix()->Get3X3(&m);

    v.y = 0.0f;
    v.x = sinf(aShipBorts[iBortIndex].fFireDir);
    v.z = cosf(aShipBorts[iBortIndex].fFireDir);

    return atan2f((m * v).y, 1.0f);
}

float AIShipCannonController::GetFireDistance(bool bMaxFireDistance)
{
    float fDistance = (bMaxFireDistance) ? 0.0f : 1e10f;
    const float fSpeedV0 = GetSpeedV0();

    const CVECTOR vOurPos = GetAIShip()->GetPos();

    for (uint32_t i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            AISHIP_BORT *pBort = &aShipBorts[i];
            const float fMaxFireDistance = AICannon::CalcMaxFireDistance(
                pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
            if (bMaxFireDistance && fDistance < fMaxFireDistance)
                fDistance = fMaxFireDistance;
            if (!bMaxFireDistance && fDistance > fMaxFireDistance)
                fDistance = fMaxFireDistance;
        }
    return fDistance;
}

bool AIShipCannonController::UpdateParameters()
{
    CVECTOR v;

    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const CVECTOR vZ = !(GetAIShip()->GetMatrix()->Vz());

    // updates borts parameters
    const float fSpeedV0 = GetSpeedV0();
    for (uint32_t i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            AISHIP_BORT *pBort = &aShipBorts[i];

            pBort->fSpeedV0 = fSpeedV0;
            pBort->fMaxFireDistance = AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0,
                                                                    GetBortHeightAngle(i) + pBort->fFireAngMax);

            const float fDir = pBort->fFireDir;
            float fZone = pBort->fFireZone;

            v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
            RotateAroundY(v.x, v.z, vZ.z, vZ.x);
            pBort->vDirection = v;

            /*v = CVECTOR(sinf(fDir - fZone/2.0f), 0.0f, cosf(fDir - fZone/2.0f));
            RotateAroundY(v.x,v.z,vZ.z,vZ.x);
            pBort->vAngleVectors[0] = v;

            v = CVECTOR(sinf(fDir + fZone/2.0f), 0.0f, cosf(fDir + fZone/2.0f));
            RotateAroundY(v.x,v.z,vZ.z,vZ.x);
            pBort->vAngleVectors[1] = v;*/
        }
    return true;
}

CVECTOR AIShipCannonController::GetBortDirection(uint32_t dwBort)
{
    Assert(dwBort != INVALID_BORT_INDEX && dwBort < aShipBorts.size());
    AISHIP_BORT *pBort = &aShipBorts[dwBort];

    const float fDir = pBort->fFireDir;
    auto v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
    const CVECTOR vZ = !(GetAIShip()->GetMatrix()->Vz()); // FIX-ME?
    RotateAroundY(v.x, v.z, vZ.z, vZ.x);
    return v;
}

CVECTOR AIShipCannonController::GetFirePos(const CVECTOR &vFireDir, float fDistance)
{
    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const float fSpeedV0 = GetSpeedV0();

    const CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
    float fAngle = acosf(Clamp(vTempDir | vFireDir));
    if (vFireDir.y < 0)
        fAngle = -fAngle;

    CVECTOR vTempFirePos = vOurPos + !vTempDir;
    vTempFirePos.y = 0.0f;

    uint32_t dwBort = GetFirstFireBort(vTempFirePos);
    if (dwBort == INVALID_BORT_INDEX)
    {
        return CVECTOR(1e5f, 0.0f, 1e5f);
        Assert(dwBort != INVALID_BORT_INDEX);
    }

    const float fMaxDist =
        AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);
    if (fDistance > fMaxDist)
        fDistance = fMaxDist;

    const float t = fDistance / (fSpeedV0 * cosf(fAngle));
    const float y = fSpeedV0 * t * sinf(fAngle) - AIHelper::fGravity * SQR(t) / 2.0f;

    const CVECTOR vPos = vOurPos + fDistance * vTempDir + CVECTOR(0.0f, y, 0.0f);

    return vPos;
}

CVECTOR AIShipCannonController::GetFirePos(const CVECTOR &vFireDir)
{
    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const float fSpeedV0 = GetSpeedV0();

    const CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
    float fAngle = acosf(Clamp(vTempDir | vFireDir));
    if (vFireDir.y < 0)
        fAngle = -fAngle;

    CVECTOR vTempFirePos = vOurPos + !vTempDir;
    vTempFirePos.y = 0.0f;

    uint32_t dwBort = GetFirstFireBort(vTempFirePos);
    if (dwBort == INVALID_BORT_INDEX) // FIX-ME
    {
        return CVECTOR(1e5f, 0.0f, 1e5f);

        __debugbreak();
        bTempFlag = true;
        dwBort = GetFirstFireBort(vTempFirePos);
        Assert(dwBort != INVALID_BORT_INDEX);
    }

    const float fMaxDist =
        AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

    return vOurPos + fMaxDist * !vFireDir;
}

bool AIShipCannonController::isCanFire(const CVECTOR &vCamDir)
{
    const CVECTOR vEnemyDir = !CVECTOR(vCamDir.x, 0.0f, vCamDir.z);
    for (uint32_t i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            if (aShipBorts[i].isBortDamaged())
                continue;

            AISHIP_BORT *pBort = &aShipBorts[i];

            // check angle limit
            float fY0; //, fY1;
            fY0 = vEnemyDir | pBort->vDirection;
            // fY0 = (vEnemyDir ^ pBort->vAngleVectors[1]).y;
            // fY1 = (vEnemyDir ^ pBort->vAngleVectors[1]).y;
            if (fY0 <= pBort->fCosFireZone)
                continue;
            // if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) continue;

            // check fire height angle
            CVECTOR v1 = pBort->vDirection; //(pBort->vAngleVectors[0] + pBort->vAngleVectors[1]) / 2.0f;
            v1 = !(CVECTOR(v1.x, vCamDir.y, v1.z));
            CVECTOR v2 = !(CVECTOR(v1.x, sinf(GetBortHeightAngle(i)), v1.z));
            const float fSub = v2.y - v1.y;
            const float fAng = acosf(Clamp(v1 | v2));
            if (fSub > 0.0f && fAng > fabsf(pBort->fFireAngMin))
                continue;
            if (fSub < 0.0f && fAng > fabsf(pBort->fFireAngMax))
                continue;

            return true;
        }
    return false;
}

bool AIShipCannonController::isCanFireBort(uint32_t dwBort, const CVECTOR &vFirePos, float *pfZapasDistance)
{
    AISHIP_BORT *pBort = &aShipBorts[dwBort];

    if (!pBort->aCannons.size())
        return false;
    if (pBort->isBortDamaged())
        return false;

    CVECTOR vFireDir = (vFirePos - pOurAIShip->GetPos());
    vFireDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
    const float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));

    // check distance limit
    if (fFireDist > pBort->fMaxFireDistance)
        return false;

    // check angle limit
    float fY0; //,fY1;
    fY0 = vFireDir | pBort->vDirection;
    if (fY0 <= pBort->fCosFireZone)
        return false;
    /*fY0 = (vFireDir ^ pBort->vAngleVectors[0]).y;
    fY1 = (vFireDir ^ pBort->vAngleVectors[1]).y;
    if (bTempFlag)
    {
      core.Trace("AIShipCannonController: bort = %d, fY0 = %.8f, fY1 = %.8f", dwBort, fY0, fY1);
    }
    if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) return false;*/

    if (pfZapasDistance)
        *pfZapasDistance = pBort->fMaxFireDistance - fFireDist;

    return true;
}

uint32_t AIShipCannonController::GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance)
{
    const float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));
    uint32_t dwBestBort = INVALID_BORT_INDEX;
    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const auto vOurDir = CVECTOR(sinf(GetAIShip()->GetAng().y), 0.0f, cosf(GetAIShip()->GetAng().y));
    float fBestAng = 1e8f;
    for (uint32_t i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            AISHIP_BORT *pBort = &aShipBorts[i];

            if (!pBort->isCharged() || pBort->isBortDamaged())
                continue;

            const float fSpeedV0 = GetSpeedV0();
            const float fMaxFireDistance = AICannon::CalcMaxFireDistance(
                pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
            if (fFireDist > fMaxFireDistance)
                continue;
            if (fMaxFireDistance - fFireDist < fZapasDistance)
                continue;

            CVECTOR vBortDir = GetBortDirection(i);
            const float fAng = acosf(Clamp(vOurDir | vBortDir));
            if (fAng < fBestAng)
            {
                fBestAng = fAng;
                dwBestBort = i;
            }
        }
    return dwBestBort;
}

uint32_t AIShipCannonController::GetFirstFireBort(const CVECTOR &vFirePos, float *pfZapasDistance)
{
    for (uint32_t i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size() && isCanFireBort(i, vFirePos, pfZapasDistance) &&
            !aShipBorts[i].isBortDamaged())
            return i;

    bTempFlag = false;
    return INVALID_BORT_INDEX;
}

uint32_t AIShipCannonController::GetNextFireBort(uint32_t dwPrevBort, const CVECTOR &vFirePos, float *pfZapasDistance)
{
    Assert(dwPrevBort != INVALID_BORT_INDEX && dwPrevBort < aShipBorts.size());
    for (uint32_t i = dwPrevBort + 1; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size() && isCanFireBort(i, vFirePos, pfZapasDistance) &&
            !aShipBorts[i].isBortDamaged())
            return i;

    return INVALID_BORT_INDEX;
}

bool AIShipCannonController::isCanFirePos(const CVECTOR &vFirePos)
{
    return (GetFirstFireBort(vFirePos) != INVALID_BORT_INDEX);
}

bool AIShipCannonController::isCanFire(AIShip *pEnemy)
{
    Assert(pEnemy);
    return (GetFirstFireBort(pEnemy->GetPos()) != INVALID_BORT_INDEX);
}

float AIShipCannonController::GetSpeedV0()
{
    ATTRIBUTES *pACannon;
    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    pACannon = pACharacter->FindAClass(pACharacter, "Ship.Cannons");
    Assert(pACannon);
    return pACannon->GetAttributeAsFloat("SpeedV0");
}

void AIShipCannonController::Realize(float fDeltaTime)
{
    uint32_t i, j;

    return;

    for (i = 0; i < aShipBorts.size(); i++)
    {
        AISHIP_BORT *pBort = &aShipBorts[i];

        for (j = 0; j < pBort->aCannons.size(); j++)
        {
            AICannon *pC = &pBort->aCannons[j];
            if (pC->isDamaged())
                continue;
            CVECTOR vCPos = pC->GetPos() - pC->GetDir() * 1.0f;
            AIHelper::pRS->DrawSphere(vCPos, fMaxCannonDamageDistance, 0xFFFFFF);
        }
    }
    // AIHelper::pRS->DrawRects(&rs, 1, "Line");
    /*char tmp[256], str[256];
    str[0] = 0;
    for (uint32_t i=0;i<aShipBorts.size();i++)
    {
      sprintf_s(tmp,"%.3f, ",GetBortHeightAngle(i));
      strcat_s(str,tmp);
    }
    pRS->Print(0,20,str);*/
    if (core.Controls->GetDebugAsyncKeyState('H') < 0)
    {
        Verts.clear();
        CMatrix m;

        CVECTOR vPosTemp = GetAIShip()->GetPos();
        GetAIShip()->GetMatrix()->Get3X3(m);
        CVECTOR vZ = !(m.Vz());
        for (uint32_t i = 0; i < aShipBorts.size(); i++)
            if (aShipBorts[i].aCannons.size())
            {
                AICannon *pCannon = &aShipBorts[i].aCannons[0];
                // calc fire angle
                float fDist = SQR(GetSpeedV0()) / AIHelper::fGravity;
                fDist *= sinf(2.0f * (GetBortHeightAngle(i) + aShipBorts[i].fFireAngMax));
                // draw baba
                float fDir = aShipBorts[i].fFireDir;
                float fRastr = aShipBorts[i].fFireZone;
                float fDA = PId2 / 36.0f;
                for (float fAng = fDir - fRastr / 2.0f; fAng < fDir + fRastr / 2.0f; fAng += fDA)
                {
                    CVECTOR v[3];
                    ZERO(v);
                    float R = fDist;
                    v[0].x = vPosTemp.x;
                    v[0].z = vPosTemp.z;

                    v[1].x = R * sinf(fAng);
                    v[1].z = R * cosf(fAng);
                    RotateAroundY(v[1].x, v[1].z, vZ.z, vZ.x);
                    v[1] += v[0];

                    v[2].x = R * sinf(fAng + fDA);
                    v[2].z = R * cosf(fAng + fDA);
                    RotateAroundY(v[2].x, v[2].z, vZ.z, vZ.x);
                    v[2] += v[0];
                    AddTrg(&v[0], 0x0F007F00);
                }

                CMatrix m;
                AIHelper::pRS->SetTransform(D3DTS_WORLD, m);
                if (Verts.size())
                    AIHelper::pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, TR_FORMAT, Verts.size() / 3, &Verts[0],
                                                   sizeof(tr_vertex), "ShipCannonAngles");
            }
    }
}

bool AIShipCannonController::Init(ATTRIBUTES *_pAShip)
{
    pAShip = _pAShip;

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);

    // init borts from ShipsTypes attributes
    ATTRIBUTES *pAP = pAShip->FindAClass(pAShip, "Cannons.Borts");
    if (!pAP)
        return false;
    uint32_t dwIdx = 0;
    while (true)
    {
        ATTRIBUTES *pBortAttribute = pAP->GetAttributeClass(dwIdx);
        if (!pBortAttribute)
            break;

        aShipBorts.push_back(AISHIP_BORT{});
        // AISHIP_BORT * pBort = &aShipBorts[aShipBorts.Add()];
        AISHIP_BORT *pBort = &aShipBorts.back();
        pBort->sName = pBortAttribute->GetThisName();
        pBort->fFireZone = pBortAttribute->GetAttributeAsFloat("FireZone");
        pBort->fFireAngMin = pBortAttribute->GetAttributeAsFloat("FireAngMin");
        pBort->fFireAngMax = pBortAttribute->GetAttributeAsFloat("FireAngMax");
        pBort->fFireDir = pBortAttribute->GetAttributeAsFloat("FireDir");
        pBort->dwNumDamagedCannons = 0;

        pBort->fCosFireZone = cosf(pBort->fFireZone / 2.0f);
        dwIdx++;

        // create damages
        char str[512];
        sprintf_s(str, "%s.damages", (char *)pBort->sName.c_str());
        pABorts->CreateSubAClass(pABorts, str);
    }

    return ScanShipForCannons();
}

bool AIShipCannonController::ScanShipForCannons()
{
    char str[512];
    uint32_t i, j;
    CMatrix m;
    GEOS::LABEL label;
    GEOS::INFO info;
    NODE *pNode;

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);
    MODEL *pModel = GetAIShip()->GetModel();
    Assert(pModel);

    // search and add cannons
    uint32_t dwIdx = 0;
    while (pNode = pModel->GetNode(dwIdx))
    {
        pNode->geo->GetInfo(info);
        for (i = 0; i < static_cast<uint32_t>(info.nlabels); i++)
        {
            pNode->geo->GetLabel(i, label);
            for (j = 0; j < aShipBorts.size(); j++)
                //       if (aShipBorts[j] == label.group_name) {
                if (_strcmpi(aShipBorts[j].sName.c_str(), label.group_name) == 0)
                {
                    // ugeen : akella bug fix
                    aShipBorts[j].aCannons.push_back(AICannon{});
                    // AICannon * pCannon = &aShipBorts[j].aCannons[aShipBorts[j].aCannons.Add()];
                    AICannon *pCannon = &aShipBorts[j].aCannons.back();
                    memcpy(m, label.m, sizeof(m));
                    aShipBorts[j].fOurBortFireHeight += m.Pos().y;
                    pCannon->Init(GetAIShip(), GetAIShip()->GetShipEID(), label);

                    sprintf_s(str, "%s.damages", label.group_name);
                    ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, str);
                    sprintf_s(str, "c%zd", aShipBorts[j].aCannons.size() - 1);
                    const float fDamage = pADamages->GetAttributeAsFloat(str, 0.0f);
                    pADamages->SetAttributeUseFloat(str, fDamage);
                    pCannon->SetDamage(fDamage);
                    if (pCannon->isDamaged())
                        aShipBorts[j].dwNumDamagedCannons++;

                    break;
                }
        }
        dwIdx++;
    }

    // calculate medium fire point for this ship
    for (i = 0; i < aShipBorts.size(); i++)
        if (aShipBorts[i].aCannons.size())
        {
            aShipBorts[i].fOurBortFireHeight /= static_cast<float>(aShipBorts[i].aCannons.size());

            const bool bHaveEnoughBalls = isHaveEnoughtBallsForBort(i);
            if (!bHaveEnoughBalls)
                bNotEnoughBalls = true;

            for (j = 0; j < aShipBorts[i].aCannons.size(); j++)
                aShipBorts[i].aCannons[j].QuickRecharge(bHaveEnoughBalls);
        }

    // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    /*core.Trace("NumBorts = %d",aShipBorts.size());
    for (uint32_t i=0;i<aShipBorts.size();i++)
      core.Trace("Bort = %s, Num cannons = %d",aShipBorts[i].sName.c_str(),aShipBorts[i].aCannons.size());*/
    // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    return true;
}

uint32_t AIShipCannonController::GetCannonsNum()
{
    uint32_t i, dwCannonsNum = 0;
    for (i = 0; i < aShipBorts.size(); i++)
        dwCannonsNum += GetBortIntactCannonsNum(i);
    return dwCannonsNum;
}

uint32_t AIShipCannonController::GetBortIntactCannonsNum(uint32_t dwBortIdx)
{
    Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.size());
    uint32_t dwCannons = 0;
    for (uint32_t j = 0; j < aShipBorts[dwBortIdx].aCannons.size(); j++)
        if (!aShipBorts[dwBortIdx].aCannons[j].isDamaged())
            dwCannons++;
    return dwCannons;
}

// ugeen : need for correct ship cannons random
uint32_t AIShipCannonController::GetBortDisabledCannonsNum(uint32_t dwBortIdx)
{
    Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.size());
    uint32_t dwCannons = 0;
    for (uint32_t j = 0; j < aShipBorts[dwBortIdx].aCannons.size(); j++)
        if (aShipBorts[dwBortIdx].aCannons[j].isDisabled())
            dwCannons++;
    return dwCannons;
}

bool AIShipCannonController::isHaveEnoughtBallsForBort(uint32_t dwBortIdx)
{
    Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.size());
    VDATA *pvData = core.Event(SHIP_GET_CURRENT_BALLS_NUM, "a", GetAIShip()->GetACharacter());
    Assert(pvData);
    if (static_cast<long>(GetBortIntactCannonsNum(dwBortIdx)) > pvData->GetLong())
        return false;
    return true;
}

void AIShipCannonController::Unload()
{
    uint32_t i, j;
    for (i = 0; i < aShipBorts.size(); i++)
    {
        for (j = 0; j < aShipBorts[i].aCannons.size(); j++)
        {
            AICannon *pC = &aShipBorts[i].aCannons[j];
            if (!pC->isDamaged())
                pC->Unload();
        }
    }
}

void AIShipCannonController::Reload()
{
    Unload();
    bReload = true;
}

void AIShipCannonController::CheckCannonsBoom(float fTmpCannonDamage, const CVECTOR &vPnt)
{
    char str[512];
    uint32_t i, j;

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);

    for (i = 0; i < aShipBorts.size(); i++)
    {
        AISHIP_BORT *pBort = &aShipBorts[i];

        sprintf_s(str, "%s.damages", (char *)pBort->sName.c_str());
        ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, str);
        Assert(pADamages);
        ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, (char *)pBort->sName.c_str());
        Assert(pACurBort);

        for (j = 0; j < pBort->aCannons.size(); j++)
        {
            AICannon *pC = &pBort->aCannons[j];
            if (pC->isDamaged())
                continue;
            CVECTOR vCPos = pC->GetPos() - pC->GetDir() * fMaxCannonDamageRadiusPoint;
            const float fDistance = sqrtf(~(vPnt - vCPos));

            if (fDistance > fMaxCannonDamageDistance)
                continue;
            VDATA *pVData = core.Event(CANNON_DAMAGE, "affffff", GetAIShip()->GetACharacter(), fTmpCannonDamage,
                                       pC->GetDamage(), fDistance, vPnt.x, vPnt.y, vPnt.z);

            sprintf_s(str, "c%d", j);

            pC->SetDamage(pVData->GetFloat());
            pADamages->SetAttributeUseFloat(str, pC->GetDamage());
            if (pC->isDamaged())
            {
                pACurBort->SetAttributeUseFloat("DamageRatio",
                                                1.0f - (static_cast<float>(GetBortIntactCannonsNum(i)) +
                                                        static_cast<float>(GetBortDisabledCannonsNum(i))) /
                                                           static_cast<float>(pBort->aCannons.size()));
                // pBort->dwNumDamagedCannons++;
                // pACurBort->SetAttributeUseFloat("DamageRatio",
                //                                1.0f - static_cast<float>(GetBortIntactCannonsNum(i)) /
                //                                static_cast<float>(pBort
                //                                  ->aCannons.size()));
            }
        }
    }
}

// boal 08.08.06 method of recalculating guns on the ship -->
void AIShipCannonController::ResearchCannons()
{
    char str[512];
    uint32_t i, j;

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);

    for (i = 0; i < aShipBorts.size(); i++)
    {
        AISHIP_BORT *pBort = &aShipBorts[i];

        sprintf_s(str, "%s.damages", (char *)pBort->sName.c_str());
        ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, str);
        Assert(pADamages);
        ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, (char *)pBort->sName.c_str());
        Assert(pACurBort);
        pBort->dwNumDamagedCannons = 0; // not used anywhere, maybe a rudiment?

        for (j = 0; j < pBort->aCannons.size(); j++)
        {
            AICannon *pC = &pBort->aCannons[j];
            sprintf_s(str, "c%d", j);
            const float fDamage = pADamages->GetAttributeAsFloat(str, 0.0f);
            pC->SetDamage(fDamage);
            pADamages->SetAttributeUseFloat(str, pC->GetDamage());
            if (pC->isDamaged())
            {
                pBort->dwNumDamagedCannons++;
                pACurBort->SetAttributeUseFloat("DamageRatio", 1.0f - static_cast<float>(GetBortIntactCannonsNum(i)) /
                                                                          static_cast<float>(pBort->aCannons.size()));
            }
        }
    }
}

// boal 08.08.06 method of recalculation of guns on the ship <--

void AIShipCannonController::Save(CSaveLoad *pSL)
{
    pSL->SaveDword(bReload);
    pSL->SaveDword(bNotEnoughBalls);
    pSL->SaveDword(bTempFlag);

    pSL->SaveDword(aShipBorts.size());
    for (uint32_t i = 0; i < aShipBorts.size(); i++)
    {
        AISHIP_BORT &B = aShipBorts[i];

        pSL->SaveString(B.sName);
        pSL->SaveFloat(B.fFireZone);
        pSL->SaveFloat(B.fFireAngMin);
        pSL->SaveFloat(B.fFireAngMax);
        pSL->SaveFloat(B.fFireDir);
        pSL->SaveFloat(B.fOurBortFireHeight);
        pSL->SaveFloat(B.fChargePercent);
        pSL->SaveFloat(B.fCosFireZone);

        pSL->SaveDword(B.dwNumDamagedCannons);
        pSL->SaveFloat(B.fSpeedV0);
        pSL->SaveFloat(B.fMaxFireDistance);
        pSL->SaveVector(B.vDirection);

        pSL->SaveDword(B.aCannons.size());
        for (uint32_t j = 0; j < B.aCannons.size(); j++)
            B.aCannons[j].Save(pSL);
    }
}

void AIShipCannonController::Load(CSaveLoad *pSL)
{
    bReload = pSL->LoadDword() != 0;
    bNotEnoughBalls = pSL->LoadDword() != 0;
    bTempFlag = pSL->LoadDword() != 0;

    const uint32_t dwNum = pSL->LoadDword();
    for (uint32_t i = 0; i < dwNum; i++)
    {
        aShipBorts.push_back(AISHIP_BORT{});
        // AISHIP_BORT & B = aShipBorts[aShipBorts.Add()];
        AISHIP_BORT &B = aShipBorts.back();

        B.sName = pSL->LoadString();
        B.fFireZone = pSL->LoadFloat();
        B.fFireAngMin = pSL->LoadFloat();
        B.fFireAngMax = pSL->LoadFloat();
        B.fFireDir = pSL->LoadFloat();
        B.fOurBortFireHeight = pSL->LoadFloat();
        B.fChargePercent = pSL->LoadFloat();
        B.fCosFireZone = pSL->LoadFloat();

        B.dwNumDamagedCannons = pSL->LoadDword();
        B.fSpeedV0 = pSL->LoadFloat();
        B.fMaxFireDistance = pSL->LoadFloat();
        B.vDirection = pSL->LoadVector();

        const uint32_t dwNumCannons = pSL->LoadDword();
        for (uint32_t j = 0; j < dwNumCannons; j++)
        {
            B.aCannons.push_back(AICannon{});
            B.aCannons.back().Load(pSL, GetAIShip(), GetAIShip()->GetShipEID());
        }
    }
}
