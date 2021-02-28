//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmPlayerShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WdmPlayerShip.h"

#include "core.h"

#include "WdmEnemyShip.h"
#include "WdmIslands.h"
#include "WdmStorm.h"
#include "WorldMap.h"
#include "defines.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmPlayerShip::WdmPlayerShip()
{
    wdmObjects->playerShip = this;
    actionRadius = 16.0f;
    stormEventTime = 0.0f;
    goForward = false;

    drawCircle = true;
}

WdmPlayerShip::~WdmPlayerShip()
{
    wdmObjects->playerShip = nullptr;
}

void WdmPlayerShip::PushOutFromIsland()
{
    // If not in the island, then don't push out
    if (!wdmObjects->islands->CollisionTest(mtx, modelL05, modelW05, false))
    {
        return;
    }
    // spiral around the point
    auto ang = 0.0f, angStep = PI * 0.1f;
    const auto areaRad =
        0.1f * 0.707f *
        sqrtf(wdmObjects->worldSizeX * wdmObjects->worldSizeX + wdmObjects->worldSizeZ * wdmObjects->worldSizeZ);
    const auto x = mtx.Pos().x;
    const auto z = mtx.Pos().z;
    for (auto r = 0.0f; r < areaRad; r += modelRadius * 0.2f, ang += angStep)
    {
        if (ang > 2.0f * PI)
            ang -= 2.0f * PI;
        const auto _x = x + r * sinf(ang);
        const auto _z = z + r * cosf(ang);
        CMatrix m(0.0f, ay, 0.0f, _x, 0.0f, _z);
        if (!wdmObjects->islands->CollisionTest(m, modelL05, modelW05, false))
        {
            Teleport(_x, _z, ay);
            return;
        }
    }
    // Didn't work out, try to randomly move
    for (long i = 0; i < 256; i++)
    {
        const auto _x = x + areaRad * rand() * 1.0f / RAND_MAX;
        const auto _z = z + areaRad * rand() * 1.0f / RAND_MAX;
        CMatrix m(0.0f, ay, 0.0f, _x, 0.0f, _z);
        if (!wdmObjects->islands->CollisionTest(m, modelL05, modelW05, false))
        {
            Teleport(_x, _z, ay);
            return;
        }
    }
}

void WdmPlayerShip::SetActionRadius(float radius)
{
    if (radius < 0.0f)
        radius = 0.0f;
    actionRadius = radius;
}

// Calculations
void WdmPlayerShip::Update(float dltTime)
{
    WdmShip::Update(dltTime);
    Move(dltTime);
    // Test getting into our encounter area
    if (stormEventTime > 0.0f)
        stormEventTime -= dltTime;
    if (wdmObjects->isPause)
        return;
    // Storm
    auto i = TestInStorm();
    if (i >= 0)
    {
        if (stormEventTime <= 0.0f)
        {
            stormEventTime = 0.5f;
            core.Event("WorldMap_PlayerInStorm", "fffl", mtx.Pos().x, mtx.Pos().z, ay, i);
        }
    }
    wdmObjects->playarInStorm = (i == -2);
    // The ships
    wdmObjects->enableSkipEnemy = false;
    for (i = 0; i < wdmObjects->ships.size(); i++)
    {
        // skip unnecessary
        auto *const es = static_cast<WdmEnemyShip *>(wdmObjects->ships[i]);
        if (static_cast<WdmShip *>(es) == this || !es->isLive || es->killMe)
        {
            if (wdmObjects->enemyShip == es)
            {
                es->isEntryPlayer = false;
                wdmObjects->enemyShip = nullptr;
            }
            continue;
        }
        // Distance to the ship
        const auto r = ~(es->mtx.Pos() - mtx.Pos());
        // Determine the testing radius
        if (es->isEnemy)
        {
            if (r < actionRadius * actionRadius * 6.0f)
            {
                if (r < actionRadius * actionRadius)
                {
                    // Caught up
                    // ((WdmEnemyShip *)wdmObjects->ships[i])->isLive = false;
                    wdmObjects->ships[i]->isSelect = true;
                    if (es->attack)
                        es->attack->isSelect = true;
                    core.Event("WorldMap_ShipEncounter", "fffl", mtx.Pos().x, mtx.Pos().z, ay, i);
                }
                else
                {
                    if (!es->isEntryPlayer || (wdmObjects->enemyShip && !wdmObjects->enemyShip->isEnemy))
                    {
                        wdmObjects->enemyShip = es;
                        es->isEntryPlayer = true;
                    }
                    if (wdmObjects->enemyShip && r < actionRadius * actionRadius * 4.0f)
                    {
                        wdmObjects->enableSkipEnemy = static_cast<WdmEnemyShip *>(wdmObjects->ships[i])->canSkip;
                    }
                }
            }
            else
            {
                if (wdmObjects->enemyShip == es)
                {
                    es->isEntryPlayer = false;
                    wdmObjects->enemyShip = nullptr;
                    wdmObjects->enableSkipEnemy = false;
                }
            }
        }
        else
        {
            if (!wdmObjects->enemyShip || !wdmObjects->enemyShip->isEnemy)
            {
                if (r < actionRadius * actionRadius)
                {
                    // if(!es->isEntryPlayer)
                    if (wdmObjects->enemyShip != es)
                    {
                        es->isEntryPlayer = true;
                        wdmObjects->enemyShip = es;
                    }
                }
                else
                {
                    if (wdmObjects->enemyShip == es)
                    {
                        es->isEntryPlayer = false;
                        wdmObjects->enemyShip = nullptr;
                    }
                }
            }
        }
    }
    if (wdmObjects->wm && wdmObjects->wm->AttributesPointer)
    {
        wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipX", mtx.Pos().x);
        wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipZ", mtx.Pos().z);
        wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipAY", ay);
    }

    const long nOldIslandVal = wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_island", 0);
    const long nOldEncounterType = wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_type", 0);
    // note hitting the island
    if (wdmObjects->curIsland)
    {
        wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_island", 1);
    }
    else
    {
        wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_island", 0);
    }
    // mark hitting the encounter
    if (wdmObjects->enemyShip)
    {
        switch (wdmObjects->enemyShip->shipType)
        {
        case wdmest_unknow:
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", 0);
            break;
        case wdmest_merchant:
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", 1);
            break;
        case wdmest_warring:
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", 2);
            break;
        case wdmest_follow:
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", 3);
            break;
        default:
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", -1);
        }
    }
    else
    {
        // mark hitting the storm
        if (wdmObjects->playarInStorm)
        {
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", 4);
        }
        else
        {
            wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type", -1);
        }
    }
    if (nOldIslandVal != wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_island", 0) ||
        nOldEncounterType != wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_type", 0))
        core.Event("WM_UpdateCurrentAction");
}

void WdmPlayerShip::LRender(VDX9RENDER *rs)
{
    WdmShip::LRender(rs);
    if (wdmObjects->isDebug)
    {
        CMatrix mtx(CVECTOR(0.0f), mtx.Pos());
        wdmObjects->DrawCircle(mtx, actionRadius, 0x4f0000ff);
    }
}

bool WdmPlayerShip::ExitFromMap()
{
    // looking for selected
    long found = -1;
    for (long i = 0; i < wdmObjects->ships.size(); i++)
    {
        wdmObjects->ships[i]->isSelect = false;
        if (wdmObjects->ships[i] == this)
            continue;
        if (!wdmObjects->ships[i]->isLive)
            continue;
        if (wdmObjects->ships[i]->killMe)
            continue;
        if (wdmObjects->ships[i] == wdmObjects->enemyShip)
        {
            found = i;
            wdmObjects->ships[i]->isSelect = true;
            // wdmObjects->enemyShip->isLive = false;
            wdmObjects->enemyShip->isSelect = true;
        }
    }
    if (found < 0)
        return false;
    if (wdmObjects->enemyShip->attack)
        wdmObjects->enemyShip->attack->isSelect = true;
    core.Event("WorldMap_ShipEncounter", "fffl", mtx.Pos().x, mtx.Pos().z, ay, found);
    return true;
}

long WdmPlayerShip::TestInStorm() const
{
    auto inStormZone = false;
    auto isTornado = false;
    for (long i = 0; i < wdmObjects->storms.size(); i++)
    {
        if (wdmObjects->storms[i]->killMe)
            continue;
        if (wdmObjects->storms[i]->CheckIntersection(mtx.Pos().x, mtx.Pos().z, actionRadius))
        {
            wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "1");
            if (wdmObjects->storms[i]->isTornado)
            {
                wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "1");
            }
            else
            {
                wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
            }
            wdmObjects->wm->AttributesPointer->SetAttribute("stormId", wdmObjects->storms[i]->GetId());
            return i;
        }
        float x, z;
        wdmObjects->storms[i]->GetPosition(x, z);
        const auto d = (mtx.Pos().x - x) * (mtx.Pos().x - x) + (mtx.Pos().z - z) * (mtx.Pos().z - z);
        if (d < wdmObjects->stormZone * wdmObjects->stormZone)
        {
            if (wdmObjects->storms[i]->IsActive())
            {
                inStormZone = true;
                if (wdmObjects->storms[i]->isTornado)
                {
                    isTornado = true;
                }
            }
        }
    }
    if (inStormZone)
    {
        wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "1");
        if (isTornado)
        {
            wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "1");
        }
        else
        {
            wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
        }
        wdmObjects->wm->AttributesPointer->SetAttribute("stormId", "");
        return -2;
    }
    wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "0");
    wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
    return -1;
}

// Move the ship
void WdmPlayerShip::Move(float dltTime)
{
    CONTROL_STATE cs;
    // Forward
    core.Controls->GetControlState("WMapShipSailUp", cs);
    if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
        goForward = true;
    core.Controls->GetControlState("WMapShipSailUp1", cs);
    if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
        goForward = true;
    if (goForward)
        speed += WDM_SHIP_INER_ST * WDM_SHIP_MAX_SPEED * dltTime * 0.5f;
    // Back
    auto isBack = false;
    core.Controls->GetControlState("WMapShipSailDown", cs);
    if (cs.state == CST_ACTIVE)
        isBack = true;
    core.Controls->GetControlState("WMapShipSailDown1", cs);
    if (cs.state == CST_ACTIVE)
        isBack = true;
    if (isBack)
    {
        goForward = false;
        speed -= WDM_SHIP_INER_ST * WDM_SHIP_MAX_SPEED * dltTime * 0.5f;
        if (speed < 0.0f)
        {
            speed = 0.0f;
        }
    }
    core.Controls->GetControlState("WMapShipSailDown", cs);
    // Turns
    auto isTurn = false;
    core.Controls->GetControlState("WMapShipTurnLeft", cs);
    if (cs.state == CST_ACTIVE)
        isTurn = true;
    core.Controls->GetControlState("WMapShipTurnLeft1", cs);
    if (cs.state == CST_ACTIVE)
        isTurn = true;
    if (isTurn)
        turnspd -= WDM_SHIP_INER_ST * WDM_SHIP_TSPEED * dltTime;
    isTurn = false;
    core.Controls->GetControlState("WMapShipTurnRight", cs);
    if (cs.state == CST_ACTIVE)
        isTurn = true;
    core.Controls->GetControlState("WMapShipTurnRight1", cs);
    if (cs.state == CST_ACTIVE)
        isTurn = true;
    if (isTurn)
        turnspd += WDM_SHIP_INER_ST * WDM_SHIP_TSPEED * dltTime;
}

void WdmPlayerShip::Collide()
{
    goForward = false;
}
