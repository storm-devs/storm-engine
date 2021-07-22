#include "touch.h"
#include <crtdbg.h>

#include "core.h"

#include "Character.h"
#include "Entity.h"
#include "inlines.h"
#include "shared/messages.h"
#include "shared/sea_ai/Script_defines.h"

#define ISLAND_CODE -1
#define INVALID_SHIP_IDX 0xACACAC
#define DELTA_TIME 80

CREATE_CLASS(TOUCH)

void TOUCH::SetDevices()
{
    entid_t ent;

    pRS = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
}

bool TOUCH::Init()
{
    SetDevices();
    return true;
}

TOUCH::TOUCH()
{
    iNumShips = 0;
    fScale = 6.0f;
    dwDeltaTime = 0;
    pIslandBase = nullptr;
    bUseTouch = false;
}

TOUCH::~TOUCH()
{
    for (long i = 0; i < iNumShips; i++)
        STORM_DELETE(pShips[i]);
}

uint64_t TOUCH::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SHIP_CREATE:
        pShips[iNumShips] = new TOUCH_SHIP;
        PZERO(pShips[iNumShips], sizeof(TOUCH_SHIP));
        pShips[iNumShips]->eID = message.EntityID();
        iNumShips++;
        break;
        /*                case EVENT_Entity_DELETED:
                  GET_DATA(temp_long,pTemp);    GET_DATA(id,pTemp);
                  for(i=0; i<iNumShips; i++) if(pShips[i]->eID == id)
                  {
                    delete pShips[i];
                    iNumShips--;
                    pShips[i] = pShips[iNumShips];
                    break;
                  }
                break;
              }*/
        break;
    }
    return 1;
}

long MaxDepth, CurDepth;
uint64_t dwRdtsc;
CVECTOR vGlobalRecoil;

void TOUCH::Realize(uint32_t DeltaTime)
{
    DrawShips();
}

void TOUCH::Execute(uint32_t dwCoreDeltaTime)
{
    // GUARD(void TOUCH::Execute(uint32_t dwCoreDeltaTime))
    long i;
    entid_t ent;
    if (!pIslandBase)
        pIslandBase = static_cast<ISLAND_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("island")));

    // Sleep(150);

    RDTSC_B(dwRdtsc);

    if (dwCoreDeltaTime == 0)
        return;
    for (i = 0; i < iNumShips; i++)
    {
        pShips[i]->pShip = static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(pShips[i]->eID));
        if (pShips[i]->pShip && !pShips[i]->iNumVContour)
        {
            pShips[i]->pShip->BuildContour(&pShips[i]->vContour[0], pShips[i]->iNumVContour);
        }
    }

    CurDepth = 0;
    // if (!bUseTouch)
    // long iTempDeltaTime = dwCoreDeltaTime;
    // while (iTempDeltaTime>0)
    {
        // iTempDeltaTime -= DELTA_TIME;
        // iDeltaTime = (iTempDeltaTime>DELTA_TIME) ? DELTA_TIME : iTempDeltaTime;
        iDeltaTime = dwCoreDeltaTime;
        for (i = 0; i < iNumShips; i++)
            Touch(i, INVALID_SHIP_IDX, nullptr, nullptr, 0.0f, 0.0f);
    }

    FakeTouch(); // just push out the ships that are still in each other

    // if (GetAsyncKeyState('5')) fScale -= 0.1f;
    // if (GetAsyncKeyState('6')) fScale += 0.1f;

    RDTSC_E(dwRdtsc);
}

BOOL TOUCH::IsIntersectShipsRects(long idx1, long idx2)
{
    Assert(idx1 >= 0 && idx1 <= iNumShips && idx2 <= iNumShips);
    if (idx2 == ISLAND_CODE)
        return true;

    const auto pS1 = pShips[idx1]->pShip, pS2 = pShips[idx2]->pShip;

    Assert(pS1 && pS2);

    // boal fix float both_dist = SQR(pS1->State.vBoxSize.z) + SQR(pS2->State.vBoxSize.z) + 2.5f;
    const auto both_dist = SQR(pS1->State.vBoxSize.z / 2.0f) + SQR(pS2->State.vBoxSize.z / 2.0f) + 2.5f;
    const auto dist = SQR(pS1->State.vPos.x - pS2->State.vPos.x) + SQR(pS1->State.vPos.z - pS2->State.vPos.z);

    const auto bRadiusIntersects = dist <= both_dist;

    // must for optimization test rects
    return (bRadiusIntersects);
}

CVECTOR TOUCH::GetPoint(float x, float y, float xx, float yy, float xscale, float yscale, float fCos, float fSin,
                        POINT ss)
{
    CVECTOR vPos;
    vPos.x = xscale * xx;
    vPos.z = yscale * yy;
    RotateAroundY(vPos.x, vPos.z, fCos, fSin);
    vPos.x += x + ss.x / 2;
    vPos.z = -vPos.z + y + ss.y / 2;
    return vPos;
}

void TOUCH::DrawLine(std::vector<RS_LINE2D> &aLines, float x1, float y1, float x2, float y2, uint32_t dwColor)
{
    RS_LINE2D l1, l2;
    l1.rhw = 0.5f;
    l2.rhw = 0.5f;
    l1.dwColor = dwColor;
    l2.dwColor = dwColor;
    l1.vPos = CVECTOR(x1, y1, 1.0f);
    l2.vPos = CVECTOR(x2, y2, 1.0f);

    aLines.emplace_back(l1);
    aLines.emplace_back(l2);
}

void TOUCH::DrawShips()
{
    long i, j;
    CVECTOR p1, p2;
    POINT ss;
    std::vector<RS_LINE2D> aLines;

    ss.x = 800;
    ss.y = 600;

    auto *pOur = static_cast<SHIP_BASE *>(pShips[0]->pShip);
    if (!pOur)
        return;
    const auto center_x = pOur->State.vPos.x;
    const auto center_z = pOur->State.vPos.z;
    const auto delta = pOur->State.vAng.y - PI;
    const auto fCosCenter = cosf(delta);
    const auto fSinCenter = sinf(delta);

    for (i = 0; i < iNumShips; i++)
        if (pShips[i])
        {
            auto *o3d = static_cast<SHIP_BASE *>(pShips[i]->pShip);
            pShips[i]->TP[0].vPos = o3d->State.vPos;
            pShips[i]->TP[0].vAng = o3d->State.vAng;
            BuildContour(i);
            auto x = fScale * -(o3d->State.vPos.x - center_x);
            auto y = fScale * (o3d->State.vPos.z - center_z);
            RotateAroundY(x, y, fCosCenter, fSinCenter);
            const auto fCos = cosf((i == 0) ? 0.0f : -delta - PI + o3d->State.vAng.y);
            const auto fSin = sinf((i == 0) ? 0.0f : -delta - PI + o3d->State.vAng.y);
            const auto xscale = fScale;
            const auto yscale = fScale;

            if (pShips[i]->vKickPoint.x != 0.0f || pShips[i]->vKickPoint.z != 0.0f)
            {
                p1 = GetPoint(x, y, pShips[i]->vKickPoint.x, pShips[i]->vKickPoint.z, fScale, fScale, fCos, fSin, ss);
                p2 = GetPoint(x, y, 0.0f, 0.0f, fScale, fScale, fCos, fSin, ss);
                DrawLine(aLines, p1.x, p1.z, p2.x, p2.z, 0xFFFFFF);
                // p1 = pShips[i]->vKickAngle;
                // p1 = GetPoint(x,y,pShips[i]->vKick.x,pShips[i]->vKick.z,fScale,fScale,fCos,fSin,ss);
                // DrawLine(aLines, p1.x, p1.z, p2.x, p2.z, 0xFF0000);
            }
            // DrawLine(aLines, p1.x-0.5f, p1.z, p1.x+0.5f, p1.z, 0xFFFFFF);

            for (j = 0; j < pShips[i]->iNumVContour; j++)
            {
                p1 = GetPoint(x, y, pShips[i]->vContour[j].x, pShips[i]->vContour[j].z, xscale, yscale, fCos, fSin, ss);
                const auto nextj = (j == pShips[i]->iNumVContour - 1) ? 0 : j + 1;
                p2 = GetPoint(x, y, pShips[i]->vContour[nextj].x, pShips[i]->vContour[nextj].z, xscale, yscale, fCos,
                              fSin, ss);

                uint32_t dwColor = 0x00FF1F;
                DrawLine(aLines, p1.x, p1.z, p2.x, p2.z, dwColor);
                float fRes;
                // dwColor = 0xFFFFFF;
                if (pIslandBase &&
                    pIslandBase->GetDepth(pShips[i]->vContourTemp[0][j].x, pShips[i]->vContourTemp[0][j].z, &fRes))
                {
                    if (fRes == 0.0f)
                        dwColor = 0xFFFF00;
                }
                DrawLine(aLines, p1.x - 0.5f, p1.z, p1.x + 0.5f, p1.z, dwColor);
                DrawLine(aLines, p2.x - 0.5f, p2.z, p2.x + 0.5f, p2.z, dwColor);
            }
        }

    if (aLines.size())
        pRS->DrawLines2D(&aLines[0], aLines.size() / 2, "Line");
}

BOOL TOUCH::BuildContour(long ship_idx)
{
    long i, j;

    auto *pTS = pShips[ship_idx];

    for (j = 0; j < 2; j++)
    {
        const auto fCos = cosf(pTS->TP[j].vAng.y);
        const auto fSin = sinf(pTS->TP[j].vAng.y);
        const auto x = pTS->TP[j].vPos.x;
        const auto z = pTS->TP[j].vPos.z;
        for (i = 0; i < pTS->iNumVContour; i++)
        {
            auto xx = pTS->vContour[i].x;
            auto zz = pTS->vContour[i].z;
            RotateAroundY(xx, zz, fCos, fSin);
            pTS->vContourTemp[j][i].x = x + xx;
            pTS->vContourTemp[j][i].z = z + zz;
        }
    }
    return true;
}

BOOL TOUCH::IsPointInContour(CVECTOR *vP, CVECTOR *vContourTemp, long numvContourTemp)
{
    auto xx = 1.0f;
    long i;
    Assert(vP && vContourTemp);
    for (i = 0; i < numvContourTemp; i++)
    {
        auto idx1 = i;
        auto idx2 = (i == numvContourTemp - 1) ? 0 : i + 1;
        if (vContourTemp[idx1].z < vContourTemp[idx2].z)
        {
            idx1 = idx2;
            idx2 = i;
        }
        const auto z1 = vContourTemp[idx1].z - vP->z;
        const auto z2 = vContourTemp[idx2].z - vP->z;
        if (z1 * z2 > 0)
            continue;
        const auto dz = z1 - z2;
        const auto x = vP->x - (vContourTemp[idx1].x + (vContourTemp[idx2].x - vContourTemp[idx1].x) * (z1 / dz));
        xx *= x;
    }
    return (xx <= 0.0f);
}

void TOUCH::GetLineABC(CVECTOR &v1, CVECTOR &v2, float &A, float &B, float &C)
{
    A = (v2.z - v1.z);
    B = (v1.x - v2.x);
    C = -v1.x * v2.z + v1.z * v2.x;
}

CVECTOR TOUCH::GetLineIntersectPoint(CVECTOR &v1, CVECTOR &v2, CVECTOR &o1, CVECTOR &o2)
{
    auto res = CVECTOR(10000.0f, 0.0f, 10000.0f);
    float A1, B1, C1, A2, B2, C2;
    GetLineABC(v1, v2, A1, B1, C1);
    GetLineABC(o1, o2, A2, B2, C2);
    const auto z_minus = (B2 * A1 - A2 * B1);
    if (z_minus == 0.0f || A1 == 0.0f)
        return res;
    res.z = (A2 * C1 - C2 * A1) / z_minus;
    res.x = (-B1 * res.z - C1) / A1;
    return res;
}

// calculate collision point
BOOL TOUCH::IsIntersectShipsReal(long idx, long cidx, CVECTOR *vPos, CVECTOR *vAng, CVECTOR *vRecoil, float *fPower,
                                 float *fSlide)
{
    long i, j;
    long l_idx1 = -1, l_idx2 = -1;
    TOUCH_SHIP *pS1, *pS2;

    pS1 = pShips[idx];
    Assert(pS1);

    // island collision
    if (cidx == ISLAND_CODE)
    {
        float fRes;

        if (!pIslandBase)
            return false; // if island collision
        if (!pS1->pShip->TouchMove(iDeltaTime, &pS1->TP[0], &pS1->TP[1]))
            return false;
        if (!BuildContour(idx))
            return false;
        for (i = 0; i < pS1->iNumVContour; i++)
            if (pIslandBase->GetDepthFast(pS1->vContourTemp[1][i].x, pS1->vContourTemp[1][i].z, &fRes))
            {
                if (fRes < fCollisionDepth)
                    continue;
                const auto v1 = &pS1->vContourTemp[0][i], v2 = &pS1->vContourTemp[1][i];
                auto vTemp = !(*v2 - *v1);
                *vRecoil = !(*v1 - pS1->TP[1].vPos);
                *fSlide = 0.0f;
                //*vRecoil = !(*vRecoil + vTemp);
                //*vRecoil = vTemp;//!(*v1-pS1->TP[1].vPos);
                *fPower = METERS2KNOTS(sqrtf(~(*v2 - *v1))) * (1000.0f / static_cast<float>(iDeltaTime));
                *vPos = pS1->vContourTemp[0][i];
                vPos->y = 0.0f;
                return true;
            }

        return false;
    }

    pS2 = pShips[cidx];
    Assert(pS2);

    // build contours
    if (!pS1->pShip->TouchMove(iDeltaTime, &pS1->TP[0], &pS1->TP[1]))
        return false;
    if (!pS2->pShip->TouchMove(iDeltaTime, &pS2->TP[0], &pS2->TP[1]))
        return false;
    if (!BuildContour(idx))
        return false;
    if (!BuildContour(cidx))
        return false;

    // calculate intersection point
    for (i = 0; i < pS1->iNumVContour; i++)
        if (IsPointInContour(&pS1->vContourTemp[1][i], &pS2->vContourTemp[1][0], pS2->iNumVContour))
        // FIX ME and pS2->vContourTemp[1][0]!!!
        {
            auto min_dist = 10000.0f;
            CVECTOR vIP; // intersection point
            const auto v1 = &pS1->vContourTemp[0][i], v2 = &pS1->vContourTemp[1][i];
            for (j = 0; j < pS2->iNumVContour; j++)
            {
                const auto idx1 = j;
                const auto idx2 = (j == pS2->iNumVContour - 1) ? 0 : j + 1;
                const auto vP = GetLineIntersectPoint(*v1, *v2, pS2->vContourTemp[0][idx1], pS2->vContourTemp[0][idx2]);
                const auto dist = SQR(vP.x - v2->x) + SQR(vP.z - v2->z);
                if (dist < min_dist)
                {
                    l_idx1 = idx1;
                    l_idx2 = idx2;
                    vIP = vP;
                    min_dist = dist;
                }
            }
            // strange!!!
            if (l_idx1 == -1 || l_idx2 == -1)
                continue;

            // calculate power of blow, slide coeff,
            // calculate vIP in pS2 ship system
            auto vIPs = pS2->TP[1].vPos - vIP;
            RotateAroundY(vIPs.x, vIPs.z, cosf(-PI - pS2->TP[1].vAng.y), sinf(-PI - pS2->TP[1].vAng.y));

            // calculate common variables
            // blow position
            *vPos = vIP;
            // blow direction
            auto vBlow = (*v2 - *v1);
            if ((~vBlow) == 0.0f)
                continue;
            *vAng = !vBlow;

            // slide coefficient
            *fSlide = *vAng | (!(pS2->vContourTemp[1][l_idx2] - pS2->vContourTemp[1][l_idx1]));
            // recoil direction
            *vRecoil = (pS2->vContourTemp[1][l_idx2] - pS2->vContourTemp[1][l_idx1]);
            Assert((~(*vRecoil)) != 0.0f);
            *vRecoil = !(*vRecoil);
            RotateAroundY(vRecoil->x, vRecoil->z, cosf(-PId2), sinf(-PId2));
            vRecoil->y = 0.0f;
            // power of blow
            const auto vSpeed2 = (pS2->vContourTemp[1][l_idx1] - pS2->vContourTemp[0][l_idx1]);
            *fPower = METERS2KNOTS(sqrtf(~vBlow));
            if ((~vSpeed2) != 0.0f)
            {
                const auto fTemp = (*vAng | (!vSpeed2));
                *fPower -= METERS2KNOTS(fTemp * sqrtf(~vSpeed2));
            }
            // fPower must multiplied by Weight of ships
            *fPower *= (1000.0f / static_cast<float>(iDeltaTime));
            // rotate blow direction to ship system
            return true;
        }
    return false;
}

long TOUCH::ProcessImpulse(long iOurIdx, CVECTOR vPos, CVECTOR vDir, float fPowerApplied)
{
    STRENGTH strength;

    ZERO(strength);
    strength.bInertia = false;

    auto *pS1 = pShips[iOurIdx];

    const auto vSDirZ = CVECTOR(0.0f, 0.0f, 1.0f);
    const auto vSDirX = CVECTOR(1.0f, 0.0f, 0.0f);

    // calculate Z impulse
    auto fkZ = (0.1f + 1.0f - fabsf(vPos.x / (0.5f * pS1->pShip->State.vBoxSize.x)));
    fkZ = CLAMP(fkZ);
    // Assert(fkZ>=0.0f && fkZ<=1.0f)
    const auto fZMove = (vDir | vSDirZ) * fPowerApplied * fkZ;
    strength.vSpeed.z = fZMove;

    // calculate X impulse
    auto fkX = (0.1f + 1.0f - fabsf(vPos.z / (0.5f * pS1->pShip->State.vBoxSize.z)));
    fkX = CLAMP(fkX);
    // Assert(fkX>=0.0f && fkX<=1.0f)
    const auto fXMove = (vDir | vSDirX) * fPowerApplied * fkX;
    strength.vSpeed.x = fXMove;

    // calculate Y rotation impulse
    const auto vSDirY = !(-vPos);
    auto fkY = 0.1f + 1.0f - fabsf(vDir | vSDirY);
    fkY = CLAMP(fkY);
    strength.vRotate.y = fkY * fPowerApplied * 0.02f;
    if (vPos.x * vPos.z >= 0.0f)
        strength.vRotate.y = -strength.vRotate.y;

    return pS1->pShip->AddStrength(&strength);
}

long TOUCH::GetTouchPoint(long iIdx, const CVECTOR &vPos)
{
    return 1;
}

// ------------------------------- Main Cycle of TOUCH -------------------------
// vPos - blow point
// vAng - blow direction
// vRecoil - recoil vector
// fPower - blow power (without mass dependence)
// fSlide - blow slide coefficient
float TOUCH::Touch(long idx, long skip_idx, CVECTOR *vPos, CVECTOR *vAng, float fPower, float fSlide)
{
    CVECTOR vPos1, vAng1, vRecoil;
    float fPower1, fSlide1, fPowerReturn = 0.0f;
    long i;
    SHIP_BASE *pOur, *pEnemy;

    Assert(idx >= 0 && idx < iNumShips);

    CurDepth++;
    if (CurDepth >= 10)
        return fPower;
    if (MaxDepth < CurDepth)
        MaxDepth = CurDepth;

    pOur = static_cast<SHIP_BASE *>(pShips[idx]->pShip);
    auto fCos = cosf(-pShips[idx]->TP[1].vAng.y + PI);
    auto fSin = sinf(-pShips[idx]->TP[1].vAng.y + PI);

    if (IsSinked(idx))
        return 0.0f;

    // apply blow power if exist
    if (vPos && vAng && fPower != 0.0f)
    {
        pEnemy = static_cast<SHIP_BASE *>(pShips[skip_idx]->pShip);

        // calculate weight coefficient
        auto fOurWeight = pOur->State.fWeight;
        auto fEnemyWeight = pEnemy->State.fWeight;
        Assert(fOurWeight != 0.0f && fEnemyWeight != 0.0f);

        const auto fKWeight = 0.5f + (fEnemyWeight - fOurWeight) / (2 * MAX(fOurWeight, fEnemyWeight));

        // add z,x impulse and y rotate impulse
        const auto fPowerApplied = fKWeight * fPower;

        // rotate vDir and vPos to ship system
        auto vPosTemp = *vPos, vDirTemp = *vAng;
        const auto fAngle = -pShips[idx]->TP[1].vAng.y;
        vPosTemp -= pShips[idx]->TP[1].vPos;
        RotateAroundY(vPosTemp.x, vPosTemp.z, cosf(fAngle), sinf(fAngle));
        RotateAroundY(vDirTemp.x, vDirTemp.z, cosf(fAngle), sinf(fAngle));

        // script event initiate

        core.Event(SHIP_SHIP2SHIP_COLLISION, "llfflfff", GetIndex(pOur->GetACharacter()),
                   GetIndex(pEnemy->GetACharacter()), fPowerApplied, fSlide, GetTouchPoint(idx, vPosTemp), vPos->x,
                   vPos->y, vPos->z);

        ProcessImpulse(idx, vPosTemp, vDirTemp, fPowerApplied);

        fPowerReturn = 1.0f * (fPower - fPowerApplied);
    }

    // ship 2 ship collision
    for (i = ISLAND_CODE; i < iNumShips; i++)
        if (i != idx && i != skip_idx && IsIntersectShipsRects(idx, i))
        {
            if (IsSinked(i))
                continue;
            if (IsIntersectShipsReal(idx, i, &vPos1, &vAng1, &vRecoil, &fPower1, &fSlide1))
            {
                if (i != ISLAND_CODE)
                    fPower1 = Touch(i, idx, &vPos1, &vAng1, fPower1, fSlide1); // return recoil power

                auto vPosTemp = vPos1, vDirTemp = vRecoil;
                const auto fAngle = -PI - pShips[idx]->TP[1].vAng.y;
                vPosTemp -= pShips[idx]->TP[1].vPos;
                RotateAroundY(vPosTemp.x, vPosTemp.z, cosf(PI + fAngle), sinf(PI + fAngle));
                RotateAroundY(vDirTemp.x, vDirTemp.z, cosf(fAngle), sinf(fAngle));

                // script event initiate
                const auto iEnemyCharacterIndex = (i == ISLAND_CODE) ? -1 : GetIndex(pShips[i]->pShip->GetACharacter());
                core.Event((i != ISLAND_CODE) ? SHIP_SHIP2SHIP_COLLISION : SHIP_SHIP2ISLAND_COLLISION, "llfflfff",
                           GetIndex(pOur->GetACharacter()), iEnemyCharacterIndex, fPower1, fSlide1,
                           GetTouchPoint(idx, vPosTemp), vPos1.x, vPos1.y, vPos1.z);

                if (i == ISLAND_CODE)
                    vGlobalRecoil = vDirTemp;
                ProcessImpulse(idx, vPosTemp, vDirTemp, fPower1);

                fPowerReturn += fPower1; // must return projection to recoil direction!!!
                bUseTouch = true;
            }
        }

    CurDepth--;
    return fPowerReturn;
}

bool TOUCH::IsSinked(long iIndex)
{
    if (iIndex == ISLAND_CODE)
        return false;

    auto *pShip = pShips[iIndex]->pShip;
    if (pShip->isDead())
    {
        const auto fY = pShip->GetBoxsize().y;
        if (pShip->GetPos().y < -fY / 3.0f)
            return true;
    }
    return false;
}

BOOL TOUCH::FakeTouch()
{
    CVECTOR vPos1, vAng1, vRecoil;
    float fPower1, fSlide1, fPowerReturn = 0.0f;
    long i, j;
    SHIP_BASE *pOur, *pEnemy;

    for (i = 0; i < iNumShips; i++)
    {
        if (IsSinked(i))
            continue;
        for (j = ISLAND_CODE; j < iNumShips; j++)
            if (i != j && IsIntersectShipsRects(i, j))
            {
                long iCycleIndex = 0;
                // must add new simple intersection test function
                if (IsSinked(j))
                    continue;
                while (IsIntersectShipsReal(i, j, &vPos1, &vAng1, &vRecoil, &fPower1, &fSlide1))
                {
                    if (j != ISLAND_CODE)
                    {
                        // from each other move out
                        pOur = static_cast<SHIP_BASE *>(pShips[i]->pShip);
                        pEnemy = static_cast<SHIP_BASE *>(pShips[j]->pShip);
                        auto fOurWeight = pOur->State.fWeight;
                        auto fEnemyWeight = pEnemy->State.fWeight;
                        const auto k = 0.5f + (fEnemyWeight - fOurWeight) / (2 * MAX(fOurWeight, fEnemyWeight));
                        const auto vDV = !CVECTOR(pEnemy->State.vPos - pOur->State.vPos);
                        const auto fMul = 0.02f;

                        pOur->State.vPos.x -= (k * vDV.x * fMul);
                        pEnemy->State.vPos.x += ((1.0f - k) * vDV.x * fMul);
                        pOur->State.vPos.z -= (k * vDV.z * fMul);
                        pEnemy->State.vPos.z += ((1.0f - k) * vDV.z * fMul);
                    }
                    else
                    {
                        // from island move out
                        pOur = static_cast<SHIP_BASE *>(pShips[i]->pShip);
                        const auto vDV = !CVECTOR(vPos1 - pOur->State.vPos);
                        const auto fMul = 0.02f;

                        const auto fAngle = -PI - pOur->State.vAng.y;
                        auto vPosTemp = vPos1 - pOur->State.vPos;
                        RotateAroundY(vPosTemp.x, vPosTemp.z, cosf(PI + fAngle), sinf(PI + fAngle));

                        const auto fAng = 0.001f * (((vPosTemp.x * vPosTemp.z) >= 0.0f) ? 1.0f : -1.0f);
                        pOur->State.vAng.y -= fAng;
                        pOur->State.vPos.x -= (vDV.x * fMul);
                        pOur->State.vPos.z -= (vDV.z * fMul);
                        iCycleIndex++;
                    }
                    if (iCycleIndex == 1024)
                        break;
                }
            }
    }

    return true;
}

uint32_t TOUCH::AttributeChanged(ATTRIBUTES *pAttribute)
{
    if (*pAttribute == "CollisionDepth")
    {
        fCollisionDepth = pAttribute->GetAttributeAsFloat();
        return 0;
    }
    return 0;
}
