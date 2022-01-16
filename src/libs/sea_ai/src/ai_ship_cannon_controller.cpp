#include "ai_balls.h"
#include "ai_ship.h"
#include "inlines.h"

float AIShipCannonController::fMaxCannonDamageDistance = 1.0f;
float AIShipCannonController::fMaxCannonDamageRadiusPoint = 1.0f;

AIShipCannonController::AIShipCannonController(AIShip *_pShip) : bReload(false), bNotEnoughBalls(false)
{
    SetAIShip(_pShip);
}

bool AIShipCannonController::Fire2Position(AISHIP_BORT &bort, const CVECTOR &vFirePos, float fFireHeight)
{
    const auto bEnoughBalls = isHaveEnoughBallsForBort(bort);
    if (!bEnoughBalls)
        bNotEnoughBalls = true;
    if (!bort.isCharged() || bort.aCannons.empty() || bort.isBortDamaged())
        return false;
    
    auto vTempFirePos = vFirePos + CVECTOR(0.0f, fFireHeight, 0.0f);

    auto *pVData = core.Event(SHIP_GET_BORT_FIRE_DELTA, "afff", GetAIShip()->GetACharacter(), vTempFirePos.x,
                              vTempFirePos.y, vTempFirePos.z);
    const auto fRadius = pVData->GetFloat();
    const auto fAng = FRAND(PIm2);
    vTempFirePos += CVECTOR(fRadius * sinf(fAng), 0.0f, fRadius * cosf(fAng));

    const auto vTempFireDir = !(vTempFirePos - pOurAIShip->GetPos());

    core.Event(SHIP_BORT_FIRE, "lisffffff", GetIndex(GetAIShip()->GetACharacter()), GetAIShip()->GetShipEID(),
               const_cast<char *>(bort.sName.c_str()), vTempFirePos.x, vTempFirePos.y, vTempFirePos.z, vTempFireDir.x,
               vTempFireDir.y, vTempFireDir.z);

    for (auto& cannon : bort.aCannons)
    {
        if (!cannon.isDamaged())
        {
            cannon.SetRechargeEnable(bEnoughBalls);

            if (debugDrawToggle)
            {
                debugFirePositions.emplace_back(vTempFirePos, ARGB(0xFF, 0xFF, 0xFF, 0x00), float{});
            }

            cannon.Fire(bort.fSpeedV0, vTempFirePos);
        }
    }
    bort.ClearCharge(); // FIX-ME

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

    auto bortIt = GetFirstFireBort(vTempFirePos);
    while (IsValid(bortIt))
    {
        const float fMaxDist = AICannon::CalcMaxFireDistance(bortIt->fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

        CVECTOR vFirePos = vFireCamPos + fMaxDist * !vFireDir;
        vFirePos.y = 0.0f;

        Fire2Position(*bortIt, vFirePos, 0.0f);

        bortIt = GetNextFireBort(bortIt, vTempFirePos);
    }

    return true;
}

bool AIShipCannonController::Fire(const CVECTOR &vFirePos)
{
    auto bortIt = GetFirstFireBort(vFirePos);
    while (IsValid(bortIt))
    {
        Fire2Position(*bortIt, vFirePos, -1.0f);
        bortIt = GetNextFireBort(bortIt, vFirePos);
    }
    return true;
}

bool AIShipCannonController::Fire(AIShip *pEnemy)
{
    Assert(pEnemy);
    Assert(AIBalls::pAIBalls);

    VDATA *pVData = core.Event(CANNON_GET_FIRE_HEIGHT, "aa", GetAIShip()->GetACharacter(), pEnemy->GetACharacter());
    Assert(pVData);
    const float fFireHeight = pVData->GetFloat();

    pVData = core.Event(CANNON_GET_FIRE_TIME, "a", GetAIShip()->GetACharacter());
    Assert(pVData);
    const float fFireTime = pVData->GetFloat();

    const float fDistance = GetAIShip()->GetDistance(*pEnemy);
    const float fSpeedV0 = GetSpeedV0();
    const float fRealSpeedV0 = fSpeedV0 * AIBalls::pAIBalls->GetMultiplier();
    const float fSpeedZ = pEnemy->GetShipBasePointer()->GetCurrentSpeed();
    const float fAng = pEnemy->GetShipBasePointer()->GetAng().y;

    const auto vEnemyPos = pEnemy->GetPos();
    const auto vTargetEnemyPos = vEnemyPos + CVECTOR{0.0f, fFireHeight, 0.0f};

    // rough estimation
    auto vFirePos = fSpeedZ * (fDistance / fRealSpeedV0 + fFireTime) * CVECTOR(sinf(fAng), 0.0f, cosf(fAng)) + vEnemyPos;

    auto bortIt = GetFirstFireBort(vFirePos);
    while (IsValid(bortIt))
    {
        const auto &cannons = bortIt->aCannons;
        if (!cannons.empty())
        {
            const auto &midCannon = cannons[cannons.size() / 2];

            // calc average real speed
            const float fRealAng = midCannon.CalcHeightFireAngle(fSpeedV0, !(vTargetEnemyPos - midCannon.GetPos()), vTargetEnemyPos);
            const float fSpeedV = fRealSpeedV0 * cosf(fRealAng);

            // calc precise pos
            TOUCH_PARAMS touchParamsNew;
            pEnemy->GetShipBasePointer()->TouchMove(static_cast<uint32_t>((fDistance / fSpeedV + fFireTime) * 1000.0f),
                                                    nullptr, &touchParamsNew);
            vFirePos = touchParamsNew.vPos;

            if (debugDrawToggle)
            {
                debugFirePositions.emplace_back(vEnemyPos, ARGB(0xFF, 0xFF, 0x00, 0x00), float{});
                debugFirePositions.emplace_back(vFirePos + CVECTOR{0.0f, fFireHeight, 0.0f},
                                                ARGB(0xFF, 0x00, 0xFF, 0x00), float{});
            }

            Fire2Position(*bortIt, vFirePos, fFireHeight);
            bortIt = GetNextFireBort(bortIt, vFirePos);
        }
    }
    return true;
}

void AIShipCannonController::Execute(float fDeltaTime)
{
    UpdateParameters();

    const CVECTOR vOurPos = GetAIShip()->GetPos();

    if (bReload)
    {
        bNotEnoughBalls = false;

        for (auto &bort : aShipBorts)
        {
            const bool bEnoughBalls = isHaveEnoughBallsForBort(bort);
            if (!bEnoughBalls)
                bNotEnoughBalls = true;
            for (auto &aCannon : bort.aCannons)
            {
                AICannon *pC = &aCannon;
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
        for (auto &bort : aShipBorts)
        {
            if (isHaveEnoughBallsForBort(bort))
                continue;
            bNotEnoughBalls = true;
            break;
        }
        core.Event(SHIP_NOT_ENOUGH_BALLS, "l", bNotEnoughBalls);
    }

    for (auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            // set maximum MaxFireDistance for all cannons
            const float fMaxBortFireDistance =
                AICannon::CalcMaxFireDistance(bort.fOurBortFireHeight + vOurPos.y, GetSpeedV0(), bort.fFireAngMax);
            for (auto &aCannon : bort.aCannons)
            {
                aCannon.SetMaxFireDistance(fMaxBortFireDistance);
            }

            int32_t dwReadyCannonsAfter = 0, dwReadyCannonsBefore = 0;

            bort.fChargePercent = 0.0f;
            for (uint32_t j = 0; j < bort.aCannons.size(); j++)
            {
                AICannon *pC = &bort.aCannons[j];

                if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty()))
                    dwReadyCannonsBefore++;

                if (pC->isDamaged())
                    continue;

                pC->Execute(fDeltaTime);
                bort.fChargePercent += pC->GetRechargePercent();

                if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty()))
                    dwReadyCannonsAfter++;
            }
            // prevent division by zero (mitrocosta)
            const auto intactCannons = GetBortIntactCannonsNum(bort);
            if (intactCannons > 0)
            {
                bort.fChargePercent /= intactCannons;
            }
            else
            {
                bort.fChargePercent = .0f;
            }

            if (dwReadyCannonsBefore != dwReadyCannonsAfter && dwReadyCannonsAfter == GetBortIntactCannonsNum(bort))
            // aShipBorts[i].fChargePercent >= 0.999f)
            {
                core.Event(SHIP_BORT_RELOADED, "as", GetAIShip()->GetACharacter(), bort.sName.c_str());
                // core.Trace("%s bort is reloaded", aShipBorts[i].sName.c_str());
            }

            // update borts parameters for script
            // if (GetAIShip()->isMainCharacter())
            {
                ATTRIBUTES *pAPlayer = GetAIShip()->GetACharacter();
                ATTRIBUTES *pABorts = pAPlayer->FindAClass(pAPlayer, "Ship.Cannons.Borts");
                Assert(pABorts);
                pABorts->SetAttribute(bort.sName.c_str(), "");
                ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, bort.sName.c_str());
                Assert(pACurBort);

                pACurBort->SetAttributeUseFloat("MaxFireDistance", bort.fMaxFireDistance);
                pACurBort->SetAttributeUseFloat("ChargeRatio", bort.fChargePercent);
                pACurBort->SetAttributeUseFloat("DamageRatio",
                                                1.0f - (static_cast<float>(GetBortIntactCannonsNum(bort)) +
                                                        static_cast<float>(GetBortDisabledCannonsNum(bort))) /
                                                static_cast<float>(bort.aCannons.size()));
                //        pACurBort->SetAttributeUseFloat("DamageRatio",
                //                                        1.0f - static_cast<float>(GetBortIntactCannonsNum(i)) /
                //                                        static_cast<float>(pBort
                //                                          ->aCannons.size()));
            }
        }
    }
}

// return (fire distance) angle
float AIShipCannonController::GetBortHeightAngle(const AISHIP_BORT &bort) const
{
    CMatrix m;
    GetAIShip()->GetMatrix()->Get3X3(&m);

    CVECTOR v;
    v.y = 0.0f;
    v.x = sinf(bort.fFireDir);
    v.z = cosf(bort.fFireDir);

    return atan2f((m * v).y, 1.0f);
}

float AIShipCannonController::GetFireDistance(bool bMaxFireDistance) const
{
    float fDistance = (bMaxFireDistance) ? 0.0f : 1e10f;
    const float fSpeedV0 = GetSpeedV0();

    const CVECTOR vOurPos = GetAIShip()->GetPos();

    for (const auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            const float fMaxFireDistance =
                AICannon::CalcMaxFireDistance(bort.fOurBortFireHeight + vOurPos.y, fSpeedV0,
                                              GetBortHeightAngle(bort) + bort.fFireAngMax);
            if (bMaxFireDistance && fDistance < fMaxFireDistance)
                fDistance = fMaxFireDistance;
            if (!bMaxFireDistance && fDistance > fMaxFireDistance)
                fDistance = fMaxFireDistance;
        }
    }
    return fDistance;
}

bool AIShipCannonController::UpdateParameters()
{
    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const CVECTOR vZ = !(GetAIShip()->GetMatrix()->Vz());

    // updates borts parameters
    const float fSpeedV0 = GetSpeedV0();
    for (auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            bort.fSpeedV0 = fSpeedV0;
            bort.fMaxFireDistance = AICannon::CalcMaxFireDistance(bort.fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(
                                                                           bort) + bort.fFireAngMax);
            const float fDir = bort.fFireDir;
            float fZone = bort.fFireZone;

            auto v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
            RotateAroundY(v.x, v.z, vZ.z, vZ.x);
            bort.vDirection = v;

            /*v = CVECTOR(sinf(fDir - fZone/2.0f), 0.0f, cosf(fDir - fZone/2.0f));
            RotateAroundY(v.x,v.z,vZ.z,vZ.x);
            aShipBorts[i].vAngleVectors[0] = v;

            v = CVECTOR(sinf(fDir + fZone/2.0f), 0.0f, cosf(fDir + fZone/2.0f));
            RotateAroundY(v.x,v.z,vZ.z,vZ.x);
            aShipBorts[i].vAngleVectors[1] = v;*/
        }
    }
    return true;
}

CVECTOR AIShipCannonController::GetBortDirection(const AISHIP_BORT &bort) const
{
    const float fDir = bort.fFireDir;
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

    const auto bortIt = GetFirstFireBort(vTempFirePos);
    if (!IsValid(bortIt))
    {
        return {1e5f, 0.0f, 1e5f};
    }

    const float fMaxDist = AICannon::CalcMaxFireDistance(bortIt->fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);
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

    const auto bortIt = GetFirstFireBort(vTempFirePos);
    if (!IsValid(bortIt)) // FIX-ME
    {
        return {1e5f, 0.0f, 1e5f};
    }

    const float fMaxDist = AICannon::CalcMaxFireDistance(bortIt->fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

    return vOurPos + fMaxDist * !vFireDir;
}

bool AIShipCannonController::isCanFire(const CVECTOR &vCamDir) const
{
    const CVECTOR vEnemyDir = !CVECTOR(vCamDir.x, 0.0f, vCamDir.z);
    for (const auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            if (bort.isBortDamaged())
                continue;

            const float fY0 = vEnemyDir | bort.vDirection;
            // fY0 = (vEnemyDir ^ aShipBorts[i].vAngleVectors[1]).y;
            // fY1 = (vEnemyDir ^ aShipBorts[i].vAngleVectors[1]).y;
            if (fY0 <= bort.fCosFireZone)
                continue;
            // if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) continue;

            // check fire height angle
            CVECTOR v1 = bort.vDirection; //(aShipBorts[i].vAngleVectors[0] + aShipBorts[i].vAngleVectors[1]) / 2.0f;
            v1 = !(CVECTOR(v1.x, vCamDir.y, v1.z));
            CVECTOR v2 = !(CVECTOR(v1.x, sinf(GetBortHeightAngle(bort)), v1.z));
            const float fSub = v2.y - v1.y;
            const float fAng = acosf(Clamp(v1 | v2));
            if (fSub > 0.0f && fAng > fabsf(bort.fFireAngMin))
                continue;
            if (fSub < 0.0f && fAng > fabsf(bort.fFireAngMax))
                continue;

            return true;
        }
    }
    return false;
}

bool AIShipCannonController::isCanFireBort(const AISHIP_BORT &bort, const CVECTOR &vFirePos,
                                           float *pfZapasDistance) const
{
    if (bort.aCannons.empty())
        return false;
    if (bort.isBortDamaged())
        return false;

    CVECTOR vFireDir = (vFirePos - pOurAIShip->GetPos());
    vFireDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
    const float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));

    // check distance limit
    if (fFireDist > bort.fMaxFireDistance)
        return false;

    const float fY0 = vFireDir | bort.vDirection;
    if (fY0 <= bort.fCosFireZone)
        return false;
    /*fY0 = (vFireDir ^ aShipBorts[i].vAngleVectors[0]).y;
    fY1 = (vFireDir ^ aShipBorts[i].vAngleVectors[1]).y;
    if (bTempFlag)
    {
      core.Trace("AIShipCannonController: bort = %d, fY0 = %.8f, fY1 = %.8f", dwBort, fY0, fY1);
    }
    if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) return false;*/

    if (pfZapasDistance)
        *pfZapasDistance = bort.fMaxFireDistance - fFireDist;

    return true;
}

auto AIShipCannonController::GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance)
    -> decltype(aShipBorts)::iterator
{
    const float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));
    auto bestBortIt = std::end(aShipBorts);
    const CVECTOR vOurPos = GetAIShip()->GetPos();
    const auto vOurDir = CVECTOR(sinf(GetAIShip()->GetAng().y), 0.0f, cosf(GetAIShip()->GetAng().y));
    float fBestAng = 1e8f;
    for (auto bort = std::begin(aShipBorts); bort != std::end(aShipBorts); ++bort)
    {
        if (!bort->aCannons.empty())
        {
            if (!bort->isCharged() || bort->isBortDamaged())
                continue;

            const float fSpeedV0 = GetSpeedV0();
            const float fMaxFireDistance = AICannon::CalcMaxFireDistance(bort->fOurBortFireHeight + vOurPos.y, fSpeedV0,
                                                                         GetBortHeightAngle(*bort) + bort->fFireAngMax);
            if (fFireDist > fMaxFireDistance)
                continue;
            if (fMaxFireDistance - fFireDist < fZapasDistance)
                continue;

            CVECTOR vBortDir = GetBortDirection(*bort);
            const float fAng = acosf(Clamp(vOurDir | vBortDir));
            if (fAng < fBestAng)
            {
                fBestAng = fAng;
                bestBortIt = bort;
            }
        }
    }
    return bestBortIt;
}

auto AIShipCannonController::GetFirstFireBort(const CVECTOR &vFirePos, float *pfZapasDistance)
    -> decltype(aShipBorts)::iterator
{
    for (auto it = std::begin(aShipBorts); it != std::end(aShipBorts); ++it)
    {
        if (!it->aCannons.empty() && isCanFireBort(*it, vFirePos, pfZapasDistance) && !it->isBortDamaged())
            return it;
    }

    return std::end(aShipBorts);
}

auto AIShipCannonController::GetNextFireBort(const decltype(aShipBorts)::iterator bortIt, const CVECTOR &vFirePos,
                                             float *pfZapasDistance)
    -> decltype(aShipBorts)::iterator
{
    if (bortIt != std::end(aShipBorts))
    {
        for (auto it = bortIt + 1; it != std::end(aShipBorts); ++it)
        {
            if (!it->aCannons.empty() && isCanFireBort(*it, vFirePos, pfZapasDistance) && !it->isBortDamaged())
                return it;
        }
    }
    return std::end(aShipBorts);
}

bool AIShipCannonController::IsValid(const decltype(aShipBorts)::const_iterator bortIt) const
{
    return bortIt != std::cend(aShipBorts);
}

bool AIShipCannonController::isCanFirePos(const CVECTOR &vFirePos)
{
    return IsValid(GetFirstFireBort(vFirePos));
}

bool AIShipCannonController::isCanFire(AIShip *pEnemy)
{
    Assert(pEnemy);
    return IsValid(GetFirstFireBort(pEnemy->GetPos()));
}

float AIShipCannonController::GetSpeedV0() const
{
    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pACannons = pACharacter->FindAClass(pACharacter, "Ship.Cannons");
    Assert(pACannons);
    return pACannons->GetAttributeAsFloat("SpeedV0");
}

void AIShipCannonController::Realize(float fDeltaTime)
{
    using namespace std::chrono_literals;

    if (core.Controls->GetDebugAsyncKeyState('H') < 0)
    {
        debugDrawToggle = !debugDrawToggle;
        std::this_thread::sleep_for(100ms);
    }

    if (!debugDrawToggle)
    {
        return;
    }

    for (auto it = debugFirePositions.begin(); it != debugFirePositions.end();)
    {
        constexpr auto kDebugFadeTime = 3.0f;
        if (auto &[pos, color, dt] = *it; dt <= kDebugFadeTime)
        {
            dt += fDeltaTime;

            CVECTOR cPos;
            CVECTOR cAng;
            float cFov;
            AIHelper::pRS->GetCamera(cPos, cAng, cFov);

            const auto dist = (pos - cPos).GetLength();

            float radius = 0.3f;
            if (dist > 1.0f)
            {
                radius *= sqrtf(dist);
            }
            AIHelper::pRS->DrawSphere(pos, radius, color);
            ++it;
        }
        else
        {
            it = debugFirePositions.erase(it);
        }
    }

    static std::vector<RS_LINE> Lines;
    Lines.clear();

    for (const auto &bort : aShipBorts)
    {
        for (const auto &cannon : bort.aCannons)
        {
            if (!cannon.isDamaged())
            {
                constexpr auto red = ARGB(0xFF, 0xDC, 0x14, 0x3C);
                constexpr auto green = ARGB(0xFF, 0x7C, 0xFC, 0x00);

                const auto &&vPos = cannon.GetPos();
                Lines.emplace_back(vPos, red);
                Lines.emplace_back(vPos + 5.0f * cannon.GetDir(), red);
                Lines.emplace_back(vPos, green);
                Lines.emplace_back(vPos + CVECTOR{0.0f, cannon.GetDirY(), 0.0f}, green);
            }
        }
    }

    if (!Lines.empty())
    {
        AIHelper::pRS->SetTransform(D3DTS_WORLD, CMatrix());
        AIHelper::pRS->DrawLines(Lines.data(), Lines.size() / 2, "Line");
    }

    if (GetAIShip()->isMainCharacter())
    {
        std::string buf;
        for (const auto &bort : aShipBorts)
        {
            buf += fmt::format("{:.3f} ", GetBortHeightAngle(bort));
        }
        AIHelper::pRS->Print(200, 20, buf.c_str());
    }

    struct tr_vertex
    {
        CVECTOR vPos;
        uint32_t dwColor;
    };

    static std::vector<tr_vertex> Verts;
    Verts.clear();

    CMatrix m;
    GetAIShip()->GetMatrix()->Get3X3(m);
    const CVECTOR vZ = !(m.Vz());
    const CVECTOR vPosTemp = GetAIShip()->GetPos();
    for (const auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            // calc fire range
            float fDist = SQR(GetSpeedV0()) / AIHelper::fGravity;
            fDist *= sinf(2.0f * (GetBortHeightAngle(bort) + aShipBorts[0].fFireAngMax));
            // draw baba
            const float fDir = bort.fFireDir;
            const float fRastr = bort.fFireZone;
            const float fDA = PId2 / 36.0f;
            for (float fAng = fDir - fRastr / 2.0f; fAng < fDir + fRastr / 2.0f; fAng += fDA)
            {
                CVECTOR v[3];
                ZERO(v);
                const float R = fDist;
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

                constexpr auto color = ARGB(0x0F, 0x90, 0xEE, 0x90);
                Verts.emplace_back(v[0], color);
                Verts.emplace_back(v[1], color);
                Verts.emplace_back(v[2], color);
            }
        }
    }

    if (!Verts.empty())
    {
        AIHelper::pRS->SetTransform(D3DTS_WORLD, CMatrix());
        AIHelper::pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, Verts.size() / 3, &Verts[0],
                                       sizeof(tr_vertex), "ShipCannonAngles");
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

        aShipBorts.emplace_back();
        // AISHIP_BORT * pBort = &aShipBorts[aShipBorts.Add()];
        AISHIP_BORT &bort = aShipBorts.back();
        bort.sName = pBortAttribute->GetThisName();
        bort.fFireZone = pBortAttribute->GetAttributeAsFloat("FireZone");
        bort.fFireAngMin = pBortAttribute->GetAttributeAsFloat("FireAngMin");
        bort.fFireAngMax = pBortAttribute->GetAttributeAsFloat("FireAngMax");
        bort.fFireDir = pBortAttribute->GetAttributeAsFloat("FireDir");
        bort.dwNumDamagedCannons = 0;

        bort.fCosFireZone = cosf(bort.fFireZone / 2.0f);
        dwIdx++;

        // create damages
        char buf[512];
        sprintf_s(buf, "%s.damages", bort.sName.c_str());
        pABorts->CreateSubAClass(pABorts, buf);
    }

    return ScanShipForCannons();
}

bool AIShipCannonController::ScanShipForCannons()
{

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);
    MODEL *pModel = GetAIShip()->GetModel();
    Assert(pModel);

    // search and add cannons
    uint32_t dwIdx = 0;
    NODE *pNode;
    while ((pNode = pModel->GetNode(dwIdx)))
    {
        GEOS::INFO info;
        pNode->geo->GetInfo(info);
        for (size_t i = 0; i < static_cast<uint32_t>(info.nlabels); i++)
        {
            GEOS::LABEL label;
            pNode->geo->GetLabel(i, label);
            for (auto &bort : aShipBorts)
                //       if (aShipBorts[j] == label.group_name) {
                if (storm::iEquals(bort.sName, label.group_name))
                {
                    bort.aCannons.emplace_back();
                    AICannon *pCannon = &bort.aCannons.back();
                    const CMatrix m;
                    memcpy(m, label.m, sizeof(m));
                    bort.fOurBortFireHeight += m.Pos().y;
                    pCannon->Init(GetAIShip(), GetAIShip()->GetShipEID(), label);

                    char buf[512];
                    sprintf_s(buf, "%s.damages", label.group_name);
                    ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, buf);
                    sprintf_s(buf, "c%zd", bort.aCannons.size() - 1);
                    const float fDamage = pADamages->GetAttributeAsFloat(buf, 0.0f);
                    pADamages->SetAttributeUseFloat(buf, fDamage);
                    pCannon->SetDamage(fDamage);
                    if (pCannon->isDamaged())
                        bort.dwNumDamagedCannons++;

                    break;
                }
        }
        dwIdx++;
    }

    // calculate medium fire point for this ship
    for (auto &bort : aShipBorts)
    {
        if (!bort.aCannons.empty())
        {
            bort.fOurBortFireHeight /= static_cast<float>(bort.aCannons.size());

            const bool bHaveEnoughBalls = isHaveEnoughBallsForBort(bort);
            if (!bHaveEnoughBalls)
                bNotEnoughBalls = true;

            for (auto &aCannon : bort.aCannons)
                aCannon.QuickRecharge(bHaveEnoughBalls);
        }
    }

    // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    /*core.Trace("NumBorts = %d",aShipBorts.size());
    for (uint32_t i=0;i<aShipBorts.size();i++)
      core.Trace("Bort = %s, Num cannons = %d",aShipBorts[i].sName.c_str(),aShipBorts[i].aCannons.size());*/
    // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    return true;
}

uint32_t AIShipCannonController::GetCannonsNum() const
{
    uint32_t dwCannonsNum = 0;
    for (const auto &bort : aShipBorts)
        dwCannonsNum += GetBortIntactCannonsNum(bort);
    return dwCannonsNum;
}

uint32_t AIShipCannonController::GetBortIntactCannonsNum(const AISHIP_BORT& bort) const
{
    uint32_t dwCannons = 0;
    for (const auto &aCannon : bort.aCannons)
        if (!aCannon.isDamaged())
            dwCannons++;
    return dwCannons;
}

// ugeen : need for correct ship cannons random
uint32_t AIShipCannonController::GetBortDisabledCannonsNum(const AISHIP_BORT &bort) const
{
    uint32_t dwCannons = 0;
    for (const auto &aCannon : bort.aCannons)
        if (aCannon.isDisabled())
            dwCannons++;
    return dwCannons;
}

bool AIShipCannonController::isHaveEnoughBallsForBort(const AISHIP_BORT &bort) const
{
    VDATA *pvData = core.Event(SHIP_GET_CURRENT_BALLS_NUM, "a", GetAIShip()->GetACharacter());
    Assert(pvData);
    if (static_cast<int32_t>(GetBortIntactCannonsNum(bort)) > pvData->GetInt())
        return false;
    return true;
}

void AIShipCannonController::Unload()
{
    for (auto &bort : aShipBorts)
    {
        for (auto &aCannon : bort.aCannons)
        {
            AICannon *pC = &aCannon;
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

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);

    for (auto &bort : aShipBorts)
    {
        sprintf_s(str, "%s.damages", bort.sName.c_str());
        ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, str);
        Assert(pADamages);
        ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, bort.sName.c_str());
        Assert(pACurBort);

        for (uint32_t j = 0; j < bort.aCannons.size(); j++)
        {
            AICannon *pC = &bort.aCannons[j];
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
                                                1.0f - (static_cast<float>(GetBortIntactCannonsNum(bort)) +
                                                        static_cast<float>(GetBortDisabledCannonsNum(bort))) /
                                                           static_cast<float>(bort.aCannons.size()));
                // aShipBorts[i].dwNumDamagedCannons++;
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

    ATTRIBUTES *pACharacter = GetAIShip()->GetACharacter();
    ATTRIBUTES *pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts");
    Assert(pABorts);

    for (auto &aShipBort : aShipBorts)
    {
        AISHIP_BORT *pBort = &aShipBort;

        sprintf_s(str, "%s.damages", aShipBort.sName.c_str());
        ATTRIBUTES *pADamages = pABorts->FindAClass(pABorts, str);
        Assert(pADamages);
        ATTRIBUTES *pACurBort = pABorts->FindAClass(pABorts, aShipBort.sName.c_str());
        Assert(pACurBort);
        aShipBort.dwNumDamagedCannons = 0; // not used anywhere, maybe a rudiment?

        for (uint32_t j = 0; j < aShipBort.aCannons.size(); j++)
        {
            AICannon *pC = &aShipBort.aCannons[j];
            sprintf_s(str, "c%d", j);
            const float fDamage = pADamages->GetAttributeAsFloat(str, 0.0f);
            pC->SetDamage(fDamage);
            pADamages->SetAttributeUseFloat(str, pC->GetDamage());
            if (pC->isDamaged())
            {
                aShipBort.dwNumDamagedCannons++;
                pACurBort->SetAttributeUseFloat("DamageRatio",
                                                1.0f - static_cast<float>(GetBortIntactCannonsNum(aShipBort)) /
                                                                          static_cast<float>(aShipBort.aCannons.size()));
            }
        }
    }
}

// boal 08.08.06 method of recalculation of guns on the ship <--

void AIShipCannonController::Save(CSaveLoad *pSL)
{
    pSL->SaveDword(bReload);
    pSL->SaveDword(bNotEnoughBalls);
    pSL->SaveDword({}); // TODO: $core-state-legacy

    pSL->SaveDword(aShipBorts.size());
    for (auto &B : aShipBorts)
    {
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
        for (auto &aCannon : B.aCannons) 
        {
            aCannon.Save(pSL);
        }
    }
}

void AIShipCannonController::Load(CSaveLoad *pSL)
{
    bReload = pSL->LoadDword() != 0;
    bNotEnoughBalls = pSL->LoadDword() != 0;
    pSL->LoadDword(); // TODO: $core-state-legacy

    const uint32_t dwNum = pSL->LoadDword();
    for (uint32_t i = 0; i < dwNum; i++)
    {
        aShipBorts.emplace_back();
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
            B.aCannons.emplace_back();
            B.aCannons.back().Load(pSL, GetAIShip(), GetAIShip()->GetShipEID());
        }
    }
}
