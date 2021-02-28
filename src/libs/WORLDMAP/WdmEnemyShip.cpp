//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmEnemyShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WdmEnemyShip.h"

#include "core.h"

#include "WdmIslands.h"

//============================================================================================

#define WDM_ENEMYSHIP_BRNSPD (1.0f / 3.0f) // Spawn time 1 / sec
#define WDM_ENEMYSHIP_DELSPD (1.0f / 2.0f) // Removal time 1 / sec
#define WDM_ENEMYSHIP_MINTIME 60.0f        // Minimum lifetime in sec
#define WDM_ENEMYSHIP_MAXTIME 180.0f       // Maximum lifetime in sec

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmEnemyShip::WdmEnemyShip()
{
    saveAttribute = nullptr;
    mx = mz = 0.0f;
    ix = iz = 0.0f;
    sx = sz = 0.0f;
    dx = dz = 0.0f;
    isEnableKill = true;
    isLookOnPlayer = false;
    isEnemy = false;
    isEntryPlayer = false;
    deleteAlpha = 1.0f;
    slowingAlfa = 1.0f;
    brnAlpha = 0.0f;
    alpha = 0.0f;
    type = -1;
    attack = nullptr;
    // Lifetime
    liveTime = WDM_ENEMYSHIP_MINTIME + rand() * ((WDM_ENEMYSHIP_MAXTIME - WDM_ENEMYSHIP_MINTIME) / RAND_MAX);
    shipType = wdmest_unknow;
    canSkip = false;
}

WdmEnemyShip::~WdmEnemyShip()
{
    /*
    if(saveAttribute)
    {
      ATTRIBUTES * atr = saveAttribute->GetParent();
      atr->DeleteAttributeClassX(saveAttribute);
      core.Event("WorldMap_DeleteShipEncounter", "l", type);
    }*/
}

// Calculations
void WdmEnemyShip::Update(float dltTime)
{
    if (!wdmObjects->playerShip)
        return;
    // Moving
    Move(dltTime);
    // Distance from us to the player
    const auto dx = wdmObjects->playerShip->mtx.Pos().x - mtx.Pos().x;
    const auto dz = wdmObjects->playerShip->mtx.Pos().z - mtx.Pos().z;
    const auto d = sqrtf(dx * dx + dz * dz);
    // Visibility
    // depending on the distance from player
    alpha = 1.0f - (d - wdmObjects->enemyshipViewDistMin) /
                       (wdmObjects->enemyshipViewDistMax - wdmObjects->enemyshipViewDistMin);
    // depending on the time of appearance
    brnAlpha += WDM_ENEMYSHIP_BRNSPD * dltTime;
    if (brnAlpha > 1.0f)
        brnAlpha = 1.0f;
    alpha *= brnAlpha;
    // depending on the time of removal
    alpha *= slowingAlfa; // deleteAlpha;
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    // Life completion check
    if (isEnableKill)
    {
        if (deleteAlpha >= 1.0f)
        {
            // Remote check
            if (d > wdmObjects->enemyshipDistKill)
                deleteAlpha = 0.999999f;
            // Lifetime check
            liveTime -= dltTime;
            if (liveTime < 0.0f)
                deleteAlpha = 0.999999f;
            // External verification
            if (KillTest())
                deleteAlpha = 0.999999f;
        }
        else
        {
            // Removal process
            deleteAlpha -= WDM_ENEMYSHIP_DELSPD * dltTime;
            slowingAlfa = deleteAlpha;
            if (deleteAlpha < 0.0f)
            {
                const auto *delEnc = "";
                if (saveAttribute)
                {
                    delEnc = saveAttribute->GetThisName();
                }
                VDATA *pVDat = nullptr;
                if (!killMe && delEnc && delEnc[0])
                {
                    pVDat = core.Event("WorldMap_EncounterDelete", "s", delEnc);
                }
                deleteAlpha = 0;
                if (!pVDat || pVDat->GetLong() != 0)
                {
                    killMe = true;
                }
                else
                {
                    slowingAlfa = 1.f;
                }
            }
        }
    }
    //
    WdmShip::Update(dltTime);
    // Updating parameters
    UpdateSaveData();
}

void WdmEnemyShip::LRender(VDX9RENDER *rs)
{
    if (isWMRender && wdmObjects->isDebug)
    {
        auto a = static_cast<long>(alpha * 255.0f);
        if (a < 60)
            a = 60;
        if (a > 255)
            a = 255;
        a <<= 24;
        wdmObjects->DrawVector(mtx.Pos(), mtx.Pos() + CVECTOR(mx, 0.0f, mz) * 10.0f, 0x00ff00 | a);
        wdmObjects->DrawVector(mtx.Pos(), mtx.Pos() + CVECTOR(ix, 0.0f, iz) * 10.0f, 0xff0000 | a);
        wdmObjects->DrawVector(mtx.Pos(), mtx.Pos() + CVECTOR(sx, 0.0f, sz) * 10.0f, 0x0000ff | a);
        wdmObjects->DrawVector(mtx.Pos(), mtx.Pos() + CVECTOR(dx, 0.0f, dz) * 10.0f, 0xffffff | a);
    }
    WdmShip::LRender(rs);
}

// Find the pulling force in the desired direction
void WdmEnemyShip::FindMoveForce()
{
    mx = 0.0f;
    mz = 0.0f;
}

// Find the force that repels from the islands
void WdmEnemyShip::FindIslandForce()
{
    CVECTOR reaction;
    if (wdmObjects->islands)
    {
        wdmObjects->islands->FindReaction(mtx.Pos(), reaction);
        ix = reaction.x * 1.5f;
        iz = reaction.z * 1.5f;
    }
    else
    {
        ix = 0.0f;
        iz = 0.0f;
    }
}

// Find the force that repels from ships
void WdmEnemyShip::FindShipsForce()
{
    sx = 0.0f;
    sz = 0.0f;
    for (const auto &ship : wdmObjects->ships)
    {
        if (!ship)
            continue;
        if (ship == this)
            continue;
        if (ship == wdmObjects->playerShip && !isLookOnPlayer)
            continue;
        // Vertor from the ship to us
        auto fx = mtx.Pos().x - ship->mtx.Pos().x;
        auto fz = mtx.Pos().z - ship->mtx.Pos().z;
        // Distance
        auto fl = fx * fx + fz * fz - 25.0f * 25.0f;
        if (fl > 25.0f * 25.0f)
            continue;
        if (fl < 0.1f)
            fl = 0.1f;
        fl = 0.2f / powf(fl, 0.5f);
        fx *= fl;
        fz *= fl;
        sx += fx;
        sz += fz;
        // Deviation
        fl = fx * fx + fz * fz;
        fx *= ship->mtx.Vz().x;
        fz *= ship->mtx.Vz().z;
        sx += -fz * 0.01f;
        sz += fx * 0.01f;
    }
    auto sl = sx * sx + sz * sz;
    if (sl > 1.0f)
    {
        sl = 1.0f / sqrtf(sl);
        sx *= sl;
        sz *= sl;
    }
}

// All move calculations together
void WdmEnemyShip::Move(float dltTime)
{
    // Calculating the turn response
    // Where to sail
    FindMoveForce();
    // Repulsion the islands
    FindIslandForce();
    // Repulsion from ships
    FindShipsForce();
    // The resulting
    dx = 1.0f * mx + 1.5f * ix + 1.1f * sx;
    dz = 1.0f * mz + 1.5f * iz + 1.1f * sz;
    const auto dl = dx * dx + dz * dz;
    // Our direction
    const auto vx = mtx.Vz().x;
    const auto vz = mtx.Vz().z;
    const auto vl = vx * vx + vz * vz;
    // The sine of the angle between this pair of vectors
    auto sn = vz * dx - vx * dz;
    if (dl * vl > 0.0f)
        sn /= sqrtf(dl * vl);
    // If the player is behind, then make a full turn
    const auto cs = vx * dx + vz * dz;
    if (cs < 0.0f)
    {
        if (sn < 0.0f)
            sn = -1;
        else
            sn = 1.0f;
    }
    // Turning reaction
    turnspd += 2.0f * WDM_SHIP_TSPEED * sn * dltTime;
    // Speed control
    if (cs >= 0.0f)
    {
        speed += kMaxSpeed * WDM_SHIP_INER_ST * WDM_SHIP_SPEED * dltTime;
    }
    else
    {
        if (speed > 0.1f)
        {
            speed -= kMaxSpeed * WDM_SHIP_INER_ST * WDM_SHIP_SPEED * dltTime;
            if (speed < 0.1f)
                speed = 0.1f;
        }
    }
    speed *= slowingAlfa;
    turnspd *= slowingAlfa;
}

// Completion check
bool WdmEnemyShip::KillTest()
{
    return !isLive;
}

// Updating stored data
void WdmEnemyShip::UpdateSaveData()
{
    if (!saveAttribute)
        return;
    saveAttribute->SetAttributeUseFloat("brnAlpha", brnAlpha);
    saveAttribute->SetAttributeUseFloat("deleteAlpha", deleteAlpha);
    saveAttribute->SetAttributeUseFloat("liveTime", liveTime);
    saveAttribute->SetAttributeUseFloat("x", mtx.Pos().x);
    saveAttribute->SetAttributeUseFloat("y", mtx.Pos().y);
    saveAttribute->SetAttributeUseFloat("z", mtx.Pos().z);
    saveAttribute->SetAttributeUseFloat("ax", ax);
    saveAttribute->SetAttributeUseFloat("ay", ay);
    saveAttribute->SetAttributeUseFloat("az", az);
    saveAttribute->SetAttributeUseFloat("dltAx", dltAx);
    saveAttribute->SetAttributeUseFloat("dltAz", dltAz);
    saveAttribute->SetAttributeUseFloat("speed", speed);
    saveAttribute->SetAttributeUseFloat("turnspd", turnspd);
    saveAttribute->SetAttributeUseFloat("kMaxSpeed", kMaxSpeed);
    saveAttribute->SetAttributeUseFloat("rspeed_x", rspeed.x);
    saveAttribute->SetAttributeUseFloat("rspeed_y", rspeed.y);
    saveAttribute->SetAttributeUseFloat("rspeed_z", rspeed.z);
    saveAttribute->SetAttributeUseFloat("slope", slope);
    saveAttribute->SetAttributeUseDword("enc_id", type);
    saveAttribute->SetAttributeUseDword("isEnableKill", isEnableKill);
    saveAttribute->SetAttributeUseDword("isEnemy", isEnemy);
    saveAttribute->SetAttributeUseDword("killMe", killMe);
    saveAttribute->SetAttributeUseFloat("canSkip", canSkip);
    saveAttribute->SetAttribute("sec", wdmObjects->attrSec);
    saveAttribute->SetAttribute("min", wdmObjects->attrMin);
    saveAttribute->SetAttribute("hour", wdmObjects->attrHour);
    saveAttribute->SetAttribute("day", wdmObjects->attrDay);
    saveAttribute->SetAttribute("month", wdmObjects->attrMonth);
    saveAttribute->SetAttribute("year", wdmObjects->attrYear);
}

void WdmEnemyShip::SetSaveAttribute(ATTRIBUTES *save)
{
    saveAttribute = save;
    if (!saveAttribute)
        return;
    brnAlpha = saveAttribute->GetAttributeAsFloat("brnAlpha", brnAlpha);
    deleteAlpha = saveAttribute->GetAttributeAsFloat("deleteAlpha", deleteAlpha);
    liveTime = saveAttribute->GetAttributeAsFloat("liveTime", liveTime);
    mtx.Pos().x = saveAttribute->GetAttributeAsFloat("x", mtx.Pos().x);
    mtx.Pos().y = saveAttribute->GetAttributeAsFloat("y", mtx.Pos().y);
    mtx.Pos().z = saveAttribute->GetAttributeAsFloat("z", mtx.Pos().z);
    ax = saveAttribute->GetAttributeAsFloat("ax", ax);
    ay = saveAttribute->GetAttributeAsFloat("ay", ay);
    az = saveAttribute->GetAttributeAsFloat("az", az);
    dltAx = saveAttribute->GetAttributeAsFloat("dltAx", dltAx);
    dltAz = saveAttribute->GetAttributeAsFloat("dltAz", dltAz);
    speed = saveAttribute->GetAttributeAsFloat("speed", speed);
    turnspd = saveAttribute->GetAttributeAsFloat("turnspd", turnspd);
    kMaxSpeed = saveAttribute->GetAttributeAsFloat("kMaxSpeed", kMaxSpeed);
    rspeed.x = saveAttribute->GetAttributeAsFloat("rspeed_x", rspeed.x);
    rspeed.y = saveAttribute->GetAttributeAsFloat("rspeed_y", rspeed.y);
    rspeed.z = saveAttribute->GetAttributeAsFloat("rspeed_z", rspeed.z);
    slope = saveAttribute->GetAttributeAsFloat("slope", slope);
    type = saveAttribute->GetAttributeAsDword("enc_id", type);
    isEnableKill = saveAttribute->GetAttributeAsDword("isEnableKill", isEnableKill) != 0;
    isEnemy = saveAttribute->GetAttributeAsDword("isEnemy", isEnemy) != 0;
    killMe = saveAttribute->GetAttributeAsDword("killMe", killMe) != 0;
    canSkip = saveAttribute->GetAttributeAsDword("canSkip", canSkip) != 0;
    if (killMe)
    {
        deleteAlpha = 0.001f;
    }
    UpdateSaveData();
}

void WdmEnemyShip::DeleteUpdate()
{
    if (!saveAttribute)
        return;
    const char *pnt = saveAttribute->GetAttribute("needDelete");
    if (pnt)
    {
        isEnableKill = true;
        deleteAlpha = 0.999999f;
        saveAttribute = nullptr;
    }
}

// Find the position for the ship relative to the player
bool WdmEnemyShip::GeneratePosition(float objRadius, float brnDltAng, float &x, float &z)
{
    // Player position
    if (!wdmObjects->playerShip)
        return false;
    const auto psx = wdmObjects->playerShip->mtx.Pos().x;
    const auto psz = wdmObjects->playerShip->mtx.Pos().z;
    const auto psay = static_cast<WdmEnemyShip *>(wdmObjects->playerShip)->ay;
    // Choice fields
    uint8_t field[32];
    for (long i = 0; i < 32; i++)
        field[i] = 0;
    // Attempts to setups
    while (true)
    {
        // define the angle
        const long ang = rand() & 31;
        if (field[ang] != 0xff)
        {
            const auto angle = psay + brnDltAng * (0.5f - ang / 31.0f);
            // Determine the radius
            long rad;
            for (rad = rand() & 7; field[ang] & (1 << rad); rad = rand() & 7)
                ;
            const auto radius = wdmObjects->enemyshipBrnDistMin +
                                (wdmObjects->enemyshipBrnDistMax - wdmObjects->enemyshipBrnDistMin) * rad / 7.0f;
            // Coordinates
            x = psx + radius * sinf(angle);
            z = psz + radius * cosf(angle);
            // check for the possibility of being in the found place
            if (CheckPosition(x, z, objRadius))
                break;
            field[ang] |= 1 << rad;
        }
        // Checking for the possibility of continuing
        long i;
        for (i = 0; i < 32 && field[i] == 0xff; i++)
            ;
        if (i == 32)
            return false;
    }
    return true;
}

// Set life time
void WdmEnemyShip::SetLiveTime(float time)
{
    if (time < 1.0f)
        time = 1.0f;
    liveTime = time;
}

// Get attribute name
const char *WdmEnemyShip::GetAttributeName() const
{
    if (saveAttribute)
    {
        return saveAttribute->GetThisName();
    }
    return "";
}
