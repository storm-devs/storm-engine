//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmStorm
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WdmStorm.h"
#include "WdmCamera.h"
#include "WdmCloud.h"
#include "WdmRenderModel.h"
#include "WorldMap.h"

//============================================================================================

#define WDM_STORM_LIVETIME_MIN 60.0f  // Minimum storm lifetime (sec)
#define WDM_STORM_LIVETIME_MAX 120.0f // Maximum storm lifetime (sec)

#define WDM_STORM_ALPHA_BRN 0.1f // Spawn rate per second (0 to 1)
#define WDM_STORM_ALPHA_KL 0.3f  // Die rate per second (from 1 to 0)

#define WDM_STORM_DIR_DIS 0.6f     // Directional spread (Orthogonal vectors addition, 1.0f = pi / 2)
#define WDM_STORM_DIR_SPD_MIN 5.0f // Minimum travel speed
#define WDM_STORM_DIR_SPD_MAX 8.0f // Maximum travel speed
#define WDM_STORM_DIR_CHTIME                                                                                           \
    0.03f // 1 / time_in_sec time of changing the direction of the cloud from the current one to the ship

#define WDM_STORM_CLDRADIUS 30.0f // The radius of the spread of clouds in a storm

#define WDM_STORM_CLOUDRAD 20.0f // Cloud size to activate

// ============================================================================================
// Construction, destruction
// ============================================================================================

char WdmStorm::cloudPosName[16];
char WdmStorm::rotSpdName[16];

WdmStorm::WdmStorm()
{
    isTornado = false;
    strcpy_s(cloudPosName, "cloudPos  ");
    strcpy_s(rotSpdName, "rotSpd ");
    saveAttribute = nullptr;
    // Storm position, travel direction, life time
    isActiveTime = 2.0f;
    // Angle relative to ship
    auto ang = rand() * 2.0f * 3.141592653589793f / (RAND_MAX + 1);
    // Radius to ship
    auto r =
        wdmObjects->stormBrnDistMin + rand() * (wdmObjects->stormBrnDistMax - wdmObjects->stormBrnDistMin) / RAND_MAX;
    // Position
    pos = CVECTOR(((WdmRenderModel *)wdmObjects->playerShip)->mtx.Pos().x + r * sinf(ang), 30.0f,
                  ((WdmRenderModel *)wdmObjects->playerShip)->mtx.Pos().z + r * cosf(ang));
    // Direction of movement
    // Direction to ship
    dir = ((WdmRenderModel *)wdmObjects->playerShip)->mtx.Pos() - pos;
    dir.y = 0.0f;
    r = sqrtf(~dir);
    if (r > 0.0f)
        dir /= r;
    // Arbitrary deviation from course
    r = WDM_STORM_DIR_DIS * (1.0f - rand() * 2.0f / RAND_MAX);
    ang = dir.x;
    dir.x -= r * dir.z;
    dir.z += r * ang;
    // Ship direction
    r = rand() * 0.1f * WDM_STORM_DIR_DIS / RAND_MAX;
    dir.x += ((WdmRenderModel *)wdmObjects->playerShip)->mtx.Vz().x * r;
    dir.z += ((WdmRenderModel *)wdmObjects->playerShip)->mtx.Vz().z * r;
    // Normalize
    r = sqrtf(~dir);
    if (r > 0.0f)
        dir /= r;
    // set the speed
    speed = WDM_STORM_DIR_SPD_MIN + rand() * (WDM_STORM_DIR_SPD_MAX - WDM_STORM_DIR_SPD_MIN) / RAND_MAX;
    // Lifetime
    liveTime = WDM_STORM_LIVETIME_MIN + rand() * (WDM_STORM_LIVETIME_MAX - WDM_STORM_LIVETIME_MIN) / RAND_MAX;
    liveAlpha = 0.0f;
    isBrn = true;
    isKl = false;
    // create clouds
    // Number of clouds
    // num = 3 + (rand () &amp; 3);
    num = 6;
    // Cloud location bitfield
    uint8_t w[8];
    for (long i = 0; i < 8; i++)
        w[i] = 0;
    // Spawn the clouds
    long x, z;
    const auto globSign = (rand() & 1) != 0;
    for (long i = 0; i < num; i++)
    {
        cloud[i] = static_cast<WdmCloud *>(wdmObjects->wm->AddObject(new WdmCloud(), 101));
        if (cloud[i])
        {
            wdmObjects->wm->AddLObject(cloud[i], 900);
            do
            {
                x = rand() & 7;
                z = rand() & 7;
            } while (((w[z] >> x) & 1) != 0);
            w[z] |= 1 << x;
            cloudPos[i].x = 2.0f * WDM_STORM_CLDRADIUS * (0.5f - x / 7.0f);
            cloudPos[i].y = 5.0f * (0.5f - rand() * 1.0f / RAND_MAX);
            cloudPos[i].z = 2.0f * WDM_STORM_CLDRADIUS * (0.5f - z / 7.0f);
            // Rotational speed
            rotSpd[i] = 0.8f * (cloudPos[i].x * cloudPos[i].x + cloudPos[i].z * cloudPos[i].z) /
                        (WDM_STORM_CLDRADIUS * WDM_STORM_CLDRADIUS);
            if (rotSpd[i] > 1.0f)
                rotSpd[i] = 1.0f;
            rotSpd[i] = 0.1f + 1.0f - rotSpd[i];
            rotSpd[i] *= 0.2f + rand() * 0.1f / RAND_MAX;
            if (globSign)
                rotSpd[i] = -rotSpd[i];
            rotSpd[i] *= 1.0f;
        }
    }
    // add to objects
    wdmObjects->AddStorm(this);
    rainTexture = wdmObjects->rs->TextureCreate("\\WorldMap\\rain.tga");
}

WdmStorm::~WdmStorm()
{
    if (rainTexture >= 0)
        wdmObjects->rs->TextureRelease(rainTexture);
    rainTexture = -1;
    for (long i = 0; i < num; i++)
    {
        wdmObjects->wm->DeleteObject(cloud[i]);
    }
    // add to objects
    wdmObjects->DelStorm(this);
}

bool WdmStorm::CheckIntersection(float x, float z, float r)
{
    // Crossing distance
    r = (r + WDM_STORM_CLOUDRAD) * (r + WDM_STORM_CLOUDRAD);
    // loop through all the clouds
    for (long i = 0; i < num; i++)
        if (cloud[i])
        {
            const auto cx = pos.x + cloudPos[i].x;
            const auto cz = pos.z + cloudPos[i].z;
            const auto d = (cx - x) * (cx - x) + (cz - z) * (cz - z);
            if (d < r)
                return true;
        }
    return false;
}

// Calculations
void WdmStorm::Update(float dltTime)
{
    if (isActiveTime >= 0.0f)
        isActiveTime -= dltTime;
    // Direction vector to the ship
    CVECTOR v; // = ((WdmRenderModel *)wdmObjects->playerShip)->mtx.Pos() - pos; v.y = 0.0f;
    wdmObjects->GetWind(pos.x, pos.z, v);
    float r = sqrtf(~v);
    // Change the direction slightly towards the direction of the ship
    // New direction vector
    float x = v.x;
    float z = v.z;
    if (r > 0.0f)
    {
        x /= r;
        z /= r;
    }
    /*x += 0.1f*((WdmRenderModel *)wdmObjects->playerShip)->mtx.Vz().x;
    z += 0.1f*((WdmRenderModel *)wdmObjects->playerShip)->mtx.Vz().z;
    float k = x*x + z*z;
    if(k > 0.0f){ k = 1.0f/sqrtf(k); x *= k; z *= k; }*/
    // Vector blending coefficient
    float k = dltTime * WDM_STORM_DIR_CHTIME;
    if (k > 1.0f)
        k = 1.0f;
    // Looking for a new direction
    dir.x += (x - dir.x) * k;
    dir.z += (z - dir.z) * k;
    // Normalize
    k = dir.x * dir.x + dir.z * dir.z;
    if (k > 0.0f)
    {
        k = 1.0f / sqrtf(k);
        dir.x *= k;
        dir.z *= k;
    }
    // Moving
    pos += dir * dltTime * speed;
    isKl |= (pos.x < -wdmObjects->worldSizeX * 1.4f || pos.x > wdmObjects->worldSizeX * 1.4f);
    isKl |= (pos.z < -wdmObjects->worldSizeZ * 1.4f || pos.z > wdmObjects->worldSizeZ * 1.4f);
    // Alpha from camera distance
    float alpha = (wdmObjects->camera->realHeight - pos.y - 10.0f) * 0.01f;
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    // Alpha from ship distance
    if (r < wdmObjects->stormViewDistMin)
        r = wdmObjects->stormViewDistMin;
    if (r > wdmObjects->stormViewDistMax)
        r = wdmObjects->stormViewDistMax;
    alpha *= 1.0f - (r - wdmObjects->stormViewDistMin) / (wdmObjects->stormViewDistMax - wdmObjects->stormViewDistMin);
    // live alpha
    alpha *= liveAlpha;
    // Clamp
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    // life
    liveTime -= dltTime;
    isKl |= (r >= wdmObjects->stormDistKill) | (liveTime <= 0.0f);
    isBrn &= !isKl;
    if (isBrn)
    {
        liveAlpha += WDM_STORM_ALPHA_BRN * dltTime;
        if (liveAlpha >= 1.0f)
        {
            liveAlpha = 1.0f;
            isBrn = false;
        }
    }
    if (isKl)
    {
        liveAlpha -= WDM_STORM_ALPHA_KL * dltTime;
        if (liveAlpha < 0.0f)
        {
            liveAlpha = 0.0f;
            isKl = false;
            killMe = true; // Remove ourselves
        }
    }
    // Setting positions to clouds
    for (long i = 0; i < num; i++)
        if (cloud[i])
        {
            // Rotate the cloud around the center
            const float rotAng = rotSpd[i] * dltTime;
            CMatrix m(0.0f, rotAng, 0.0f);
            cloudPos[i] = m * CVECTOR(cloudPos[i]);
            // Set alpha
            cloud[i]->globalAlpha = alpha;
            // Set position
            cloud[i]->pos = pos + cloudPos[i];
        }
    UpdateSaveData();
}

void WdmStorm::LRender(VDX9RENDER *rs)
{
    if (wdmObjects->isDebug)
    {
        CMatrix mtr;
        mtr.Pos() = pos;
        mtr.Pos().y = 0.1f;
        wdmObjects->DrawCircle(mtr, wdmObjects->stormZone, 0x2f202040);
        for (long i = 0; i < num; i++)
            if (cloud[i])
            {
                mtr.Pos() = pos + cloudPos[i];
                mtr.Pos().y = 0.1f;
                wdmObjects->DrawCircle(mtr, WDM_STORM_CLOUDRAD, 0x4f000000);
            }
    }

    long count = 0;
    for (long i = 0; i < num; i++)
    {
        if (cloud[i])
        {
            count = cloud[i]->FillRain(rainRect, count);
        }
    }
    if (count)
    {
        rs->TextureSet(0, rainTexture);
        rs->DrawRects(rainRect, count, "WdmRain", 1, 1);
    }
}

// Setting parameters
void WdmStorm::SetSaveAttribute(ATTRIBUTES *save)
{
    saveAttribute = save;
    if (!saveAttribute)
        return;
    pos.x = saveAttribute->GetAttributeAsFloat("px", pos.x);
    pos.y = saveAttribute->GetAttributeAsFloat("py", pos.y);
    pos.z = saveAttribute->GetAttributeAsFloat("pz", pos.z);
    dir.x = saveAttribute->GetAttributeAsFloat("dx", dir.x);
    dir.y = saveAttribute->GetAttributeAsFloat("dy", dir.y);
    dir.z = saveAttribute->GetAttributeAsFloat("dz", dir.z);
    isActiveTime = saveAttribute->GetAttributeAsFloat("isActiveTime", isActiveTime);
    liveTime = saveAttribute->GetAttributeAsFloat("liveTime", liveTime);
    liveAlpha = saveAttribute->GetAttributeAsFloat("liveAlpha", liveAlpha);
    speed = saveAttribute->GetAttributeAsFloat("speed", speed);

    isBrn = saveAttribute->GetAttributeAsDword("isBrn", isBrn) != 0;
    isKl = saveAttribute->GetAttributeAsDword("isKl", isKl) != 0;
    isTornado = saveAttribute->GetAttributeAsDword("isTornado", isTornado) != 0;

    num = saveAttribute->GetAttributeAsDword("num", num);

    for (char i = 0; i < 8; i++)
    {
        cloudPosName[8] = '0' + i;
        cloudPosName[9] = 'x';
        cloudPos[i].x = saveAttribute->GetAttributeAsFloat(cloudPosName, cloudPos[i].x);
        cloudPosName[9] = 'y';
        cloudPos[i].y = saveAttribute->GetAttributeAsFloat(cloudPosName, cloudPos[i].y);
        cloudPosName[9] = 'z';
        cloudPos[i].z = saveAttribute->GetAttributeAsFloat(cloudPosName, cloudPos[i].z);
        rotSpdName[6] = '0' + i;
        rotSpd[i] = saveAttribute->GetAttributeAsFloat(rotSpdName, rotSpd[i]);
    }
    UpdateSaveData();
}

void WdmStorm::DeleteUpdate()
{
    if (!saveAttribute)
        return;
    const char *pnt = saveAttribute->GetAttribute("needDelete");
    if (pnt)
    {
        isKl = true;
    }
}

// Updating stored data
void WdmStorm::UpdateSaveData()
{
    if (!saveAttribute)
        return;
    saveAttribute->SetAttributeUseFloat("px", pos.x);
    saveAttribute->SetAttributeUseFloat("py", pos.y);
    saveAttribute->SetAttributeUseFloat("pz", pos.z);
    saveAttribute->SetAttributeUseFloat("dx", dir.x);
    saveAttribute->SetAttributeUseFloat("dy", dir.y);
    saveAttribute->SetAttributeUseFloat("dz", dir.z);
    saveAttribute->SetAttributeUseFloat("isActiveTime", isActiveTime);
    saveAttribute->SetAttributeUseFloat("liveTime", liveTime);
    saveAttribute->SetAttributeUseFloat("liveAlpha", liveAlpha);
    saveAttribute->SetAttributeUseFloat("speed", speed);

    saveAttribute->SetAttributeUseDword("isBrn", isBrn);
    saveAttribute->SetAttributeUseDword("isKl", isKl);
    saveAttribute->SetAttributeUseDword("isTornado", isTornado);

    saveAttribute->SetAttributeUseDword("num", num);

    for (char i = 0; i < 8; i++)
    {
        cloudPosName[8] = '0' + i;
        cloudPosName[9] = 'x';
        saveAttribute->SetAttributeUseFloat(cloudPosName, cloudPos[i].x);
        cloudPosName[9] = 'y';
        saveAttribute->SetAttributeUseFloat(cloudPosName, cloudPos[i].y);
        cloudPosName[9] = 'z';
        saveAttribute->SetAttributeUseFloat(cloudPosName, cloudPos[i].z);
        rotSpdName[6] = '0' + i;
        saveAttribute->SetAttributeUseFloat(rotSpdName, rotSpd[i]);
    }
    saveAttribute->SetAttribute("sec", wdmObjects->attrSec);
    saveAttribute->SetAttribute("min", wdmObjects->attrMin);
    saveAttribute->SetAttribute("hour", wdmObjects->attrHour);
    saveAttribute->SetAttribute("day", wdmObjects->attrDay);
    saveAttribute->SetAttribute("month", wdmObjects->attrMonth);
    saveAttribute->SetAttribute("year", wdmObjects->attrYear);
}

const char *WdmStorm::GetId() const
{
    if (saveAttribute)
    {
        return saveAttribute->GetThisName();
    }
    return "";
}
