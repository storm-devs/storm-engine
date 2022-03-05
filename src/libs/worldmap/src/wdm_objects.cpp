//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmObjects
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_objects.h"

#include <chrono>

#include "defines.h"
#include "geometry.h"

//============================================================================================

WdmObjects *wdmObjects = nullptr;

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmObjects::WdmObjects()
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    Assert(!wdmObjects);
    srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    wdmObjects = this;
    wm = nullptr;
    rs = nullptr;
    gs = nullptr;
    camera = nullptr;
    islands = nullptr;
    playerShip = nullptr;
    isDebug = false;
    Clear();
    enemyShip = nullptr;
    enableSkipEnemy = false;
    playarInStorm = false;
    worldSizeX = 2000.0f;
    worldSizeZ = 2000.0f;
    for (int32_t i = 0; i < sizeof(entryModels) / sizeof(entryModels[0]); i++)
    {
        entryModels[i] = -1;
    }
    strcpy(stCoordinate, "Coordinates");
    resizeRatio = 1.0f;
}

WdmObjects::~WdmObjects()
{
    wdmObjects = nullptr;
    for (int32_t i = 0; i < models.size(); i++)
    {
        delete models[i].geo;
    }
}

void WdmObjects::SetWorldSize(float x, float z)
{
    worldSizeX = x;
    worldSizeZ = z;
    x *= 1.2f;
    z *= 1.2f;
    windField.Init(-0.5f * x, -0.5f * z, 0.5f * x, 0.5f * z);
}

void WdmObjects::Clear()
{
    wm = nullptr;
    rs = nullptr;
    gs = nullptr;
    islands = nullptr;
    isDebug = false;
    isPause = false;
    shipSpeedOppositeWind = 0.8f;
    shipSpeedOverWind = 1.2f;
    enemyshipViewDistMin = 20.0f; // The distance at which the ship begins to disappear
    enemyshipViewDistMax = 50.0f; // The distance at which the ship disappears completely
    enemyshipDistKill = 100.0f;   // The distance at which the ship is killed
    enemyshipBrnDistMin = 40.0f;  // The minimum distance at which a ship is spawned
    enemyshipBrnDistMax = 70.0f;  // The maximum distance at which a ship is spawned
    stormViewDistMin = 120.0f;    // The distance at which the storm begins to fade
    stormViewDistMax = 160.0f;    // The distance at which the storm disappears completely
    stormDistKill = 200.0f;       // The distance at which the storm is killed
    stormBrnDistMin = 70.0f;      // The minimum distance at which a storm is spawned
    stormBrnDistMax = 160.0f;     // The maximum distance at which a storm is spawned
    stormZone = 50.0f;            // Total storm radius
    enableSkipEnemy = false;
    playarInStorm = false;
    isNextDayUpdate = true;
    nationFlagIndex.reset();
    SetWorldSize(2000.0f, 2000.0f);
}

void WdmObjects::AddShip(WdmShip *ship)
{
    Assert(ship);
    ships.push_back(ship);
}

void WdmObjects::DelShip(WdmShip *ship)
{
    Assert(ship);
    for (auto &it : ships)
    {
        if (it == ship)
        {
            it = ships.back();
            ships.pop_back();
            break;
        }
    }
}

void WdmObjects::AddStorm(WdmStorm *storm)
{
    Assert(storm);
    storms.push_back(storm);
}

void WdmObjects::DelStorm(WdmStorm *storm)
{
    for (auto &it : storms)
    {
        if (it == storm)
        {
            it = storms.back();
            storms.pop_back();
            break;
        }
    }
}

// Create geometry
GEOS *WdmObjects::CreateGeometry(const char *path)
{
    if (!path || !path[0] || !gs)
        return nullptr;
    // Looking among added
    const uint32_t hash = TOREMOVE::HashNoCase(path);
    int32_t i = hash & (sizeof(entryModels) / sizeof(entryModels[0]) - 1);
    for (i = entryModels[i]; i >= 0; i = models[i].next)
    {
        if (models[i].hash == hash)
        {
            if (storm::iEquals(models[i].path, path))
            {
                return models[i].geo;
            }
        }
    }
    // Loading geometry
    modelPath = "WorldMap\\";
    modelPath += path;
    gs->SetTexturePath("WorldMap\\Geometry\\");
    auto *const geo = gs->CreateGeometry(modelPath.c_str(), "", 0);
    gs->SetTexturePath("");
    // Add to the table
    // Model & m = models[models.Add()];
    // m.path = path;
    // m.hash = hash;
    // m.next = -1;
    // m.geo = geo;
    models.push_back(Model{geo, path, hash, -1});
    int32_t index = hash & (sizeof(entryModels) / sizeof(entryModels[0]) - 1);
    if (entryModels[index] < 0)
    {
        entryModels[index] = i;
    }
    else
    {
        for (index = entryModels[index]; models[index].next >= 0; index = models[index].next)
            ;
        models[index].next = i;
    }
    return geo;
}

//============================================================================================
// Debug functions
//============================================================================================

#define WdmObjects_myPI 3.1415926535897932384626433832795f

WdmObjects::Vertex WdmObjects::vertex[1024];

void WdmObjects::DrawCircle(const CVECTOR &pos, float radius, uint32_t color) const
{
    static CMatrix mtx;
    mtx.SetPosition(pos.x, pos.y, pos.z);
    DrawCircle(mtx, radius, color);
}

void WdmObjects::DrawCircle(CMatrix &mtx, float radius, uint32_t color) const
{
    for (int32_t i = 0; i < 64; i++)
    {
        vertex[i].v.x = radius * sinf(2.0f * WdmObjects_myPI * i / 63);
        vertex[i].v.y = 0.0f;
        vertex[i].v.z = radius * cosf(2.0f * WdmObjects_myPI * i / 63);
        vertex[i].c = color;
    }
    rs->TextureSet(0, -1);
    rs->TextureSet(1, -1);
    rs->SetTransform(D3DTS_WORLD, mtx);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, D3DFVF_XYZ | D3DFVF_DIFFUSE, 62, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawVector(const CVECTOR &start, const CVECTOR &end, uint32_t color) const
{
    auto dir = end - start;
    auto len = ~dir;
    if (len == 0.0f)
        return;
    len = sqrtf(len);
    dir *= 1.0f / len;
    const auto p = 0.9f * len;
    const auto r = 0.03f * len;

    int32_t t = 0;
    for (int32_t i = 0, imax = 8; i < imax; i++)
    {
        const auto y1 = r * sinf(2.0f * WdmObjects_myPI * i / static_cast<float>(imax));
        const auto x1 = r * cosf(2.0f * WdmObjects_myPI * i / static_cast<float>(imax));
        const auto y2 = r * sinf(2.0f * WdmObjects_myPI * (i + 1) / static_cast<float>(imax));
        const auto x2 = r * cosf(2.0f * WdmObjects_myPI * (i + 1) / static_cast<float>(imax));
        // Add triangles of the 1st line
        // 0,0,0
        vertex[t * 3 + 0].v.x = 0.0f;
        vertex[t * 3 + 0].v.y = 0.0f;
        vertex[t * 3 + 0].v.z = len;
        vertex[t * 3 + 0].c = color;
        // 0,1,1
        vertex[t * 3 + 1].v.x = x1;
        vertex[t * 3 + 1].v.y = y1;
        vertex[t * 3 + 1].v.z = p;
        vertex[t * 3 + 1].c = color;
        // 0,2,2
        vertex[t * 3 + 2].v.x = x2;
        vertex[t * 3 + 2].v.y = y2;
        vertex[t * 3 + 2].v.z = p;
        vertex[t * 3 + 2].c = color;
        // 1,0,3
        vertex[t * 3 + 3] = vertex[t * 3 + 2];
        // 1,1,4
        vertex[t * 3 + 4] = vertex[t * 3 + 1];
        // 1,2,5
        vertex[t * 3 + 5].v.x = x1 * 0.3f;
        vertex[t * 3 + 5].v.y = y1 * 0.3f;
        vertex[t * 3 + 5].v.z = p;
        vertex[t * 3 + 5].c = color;
        // 2,0,6
        vertex[t * 3 + 6] = vertex[t * 3 + 2];
        // 2,1,7
        vertex[t * 3 + 7] = vertex[t * 3 + 5];
        // 2,2,8
        vertex[t * 3 + 8].v.x = x2 * 0.3f;
        vertex[t * 3 + 8].v.y = y2 * 0.3f;
        vertex[t * 3 + 8].v.z = p;
        vertex[t * 3 + 8].c = color;
        // 3,0,9
        vertex[t * 3 + 10] = vertex[t * 3 + 5];
        // 3,1,10
        vertex[t * 3 + 9] = vertex[t * 3 + 8];
        // 3,2,11
        vertex[t * 3 + 11].v.x = x1 * 0.5f;
        vertex[t * 3 + 11].v.y = y1 * 0.5f;
        vertex[t * 3 + 11].v.z = 0.0f;
        vertex[t * 3 + 11].c = color;
        // 4,0,12
        vertex[t * 3 + 12] = vertex[t * 3 + 8];
        // 4,1,13
        vertex[t * 3 + 13] = vertex[t * 3 + 11];
        // 4,2,14
        vertex[t * 3 + 14].v.x = x2 * 0.5f;
        vertex[t * 3 + 14].v.y = y2 * 0.5f;
        vertex[t * 3 + 14].v.z = 0.0f;
        vertex[t * 3 + 14].c = color;
        // 5,0,15
        vertex[t * 3 + 15] = vertex[t * 3 + 14];
        // 5,1,16
        vertex[t * 3 + 16] = vertex[t * 3 + 11];
        // 5,2,17
        vertex[t * 3 + 17].v.x = 0.0f;
        vertex[t * 3 + 17].v.y = 0.0f;
        vertex[t * 3 + 17].v.z = 0.0f;
        vertex[t * 3 + 17].c = color;

        t += 6;
    }
    Assert(t * 3 * 6 < sizeof(vertex) / sizeof(Vertex));
    rs->TextureSet(0, -1);
    rs->TextureSet(1, -1);
    auto yBs = dir ^ CVECTOR(0.0f, 1.0f, 0.0f);
    if (~yBs == 0.0f)
    {
        yBs = dir ^ CVECTOR(1.0f, 0.0f, 0.0f);
        if (~yBs == 0.0f)
            yBs = dir ^ CVECTOR(0.0f, 0.0f, 1.0f);
        if (~yBs == 0.0f)
            return;
    }
    CMatrix mtx;
    mtx.Vx() = yBs ^ dir;
    mtx.Vy() = yBs;
    mtx.Vz() = dir;
    mtx.SetPosition(start.x, start.y, start.z);
    rs->SetTransform(D3DTS_WORLD, mtx);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, t, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawLine(const CVECTOR &start, const CVECTOR &end, uint32_t color) const
{
    vertex[0].v = start;
    vertex[0].c = color;
    vertex[1].v = end;
    vertex[1].c = color;
    const CMatrix mtx;
    rs->SetTransform(D3DTS_WORLD, mtx);
    rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, 1, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawBox2D(CMatrix &mtx, float l, float w, uint32_t color) const
{
    vertex[0].v = CVECTOR(-w, 0.2f, l);
    vertex[0].c = color;
    vertex[1].v = CVECTOR(w, 0.2f, l);
    vertex[1].c = color;
    vertex[2].v = CVECTOR(w, 0.2f, l);
    vertex[2].c = color;
    vertex[3].v = CVECTOR(w, 0.2f, -l);
    vertex[3].c = color;
    vertex[4].v = CVECTOR(w, 0.2f, -l);
    vertex[4].c = color;
    vertex[5].v = CVECTOR(-w, 0.2f, -l);
    vertex[5].c = color;
    vertex[6].v = CVECTOR(-w, 0.2f, -l);
    vertex[6].c = color;
    vertex[7].v = CVECTOR(-w, 0.2f, l);
    vertex[7].c = color;
    rs->SetTransform(D3DTS_WORLD, mtx);
    rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, 4, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

// Get vp dimensions
void WdmObjects::GetVPSize(float &w, float &h) const
{
    // Get the current vp sizes
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);
    w = static_cast<float>(vp.Width);
    h = static_cast<float>(vp.Height);
}

// Get wind direction and strength
float WdmObjects::GetWind(float x, float z, CVECTOR &dir)
{
    windField.GetWind(x, z, dir.x, dir.z);
    dir.y = 0.0f;
    auto len = sqrtf(dir.x * dir.x + dir.z * dir.z);
    if (len > 1e-20f)
    {
        dir.x /= len;
        dir.z /= len;
    }
    else
    {
        dir.x = 0.0f;
        dir.z = 1.0f;
    }
    if (len < 0.0f)
        len = 0.0f;
    if (len > 1.0f)
        len = 1.0f;
    return len;
}

// Update wind state
void WdmObjects::UpdateWind(float dltTime)
{
    dltTime = dltTime / 2.0f;
    windField.Step(dltTime);

    /*
    const int32_t size = 64;
    for(int32_t z = 0; z < size; z++)
    {
      for(int32_t x = 0; x < size; x++)
      {
        float px = worldSizeX*(x/(float)size - 0.5f);
        float pz = worldSizeZ*(z/(float)size - 0.5f);
        float wx, wz;
        windField.GetWind(px, pz, wx, wz);
        DrawLine(CVECTOR(px, 1.0f, pz), CVECTOR(px, 1.0f, pz) + CVECTOR(wx, 0.0f, wz)*30.0f, 0xff00ff00);
      }
    }
    */
}

// Get save string
const char *WdmObjects::GetWindSaveString(std::string &windData)
{
    windData = "v02_";
    const int32_t size = sizeof(windField);
    AddDataToString(windData, static_cast<uint8_t>(size >> 0));
    AddDataToString(windData, static_cast<uint8_t>(size >> 8));
    AddDataToString(windData, static_cast<uint8_t>(size >> 16));
    AddDataToString(windData, static_cast<uint8_t>(size >> 24));
    const auto *buf = (const uint8_t *)&windField;
    for (int32_t i = 0; i < size; i++)
    {
        AddDataToString(windData, buf[i]);
    }
    return windData.c_str();
}

// Set save string
void WdmObjects::SetWindSaveString(const char *str)
{
    if (!str)
    {
        windField.Reinit();
        return;
    }
    if (str[0] != 'v' || str[1] != '0' || str[2] != '2' || str[3] != '_')
    {
        windField.Reinit();
        return;
    }
    str += 4;
    const int32_t size = sizeof(windField);
    int32_t testSize = 0;
    testSize |= static_cast<uint32_t>(GetDataFromString(str)) << 0;
    testSize |= static_cast<uint32_t>(GetDataFromString(str)) << 8;
    testSize |= static_cast<uint32_t>(GetDataFromString(str)) << 16;
    testSize |= static_cast<uint32_t>(GetDataFromString(str)) << 24;
    if (size != testSize)
    {
        windField.Reinit();
        return;
    }
    auto *buf = (uint8_t *)&windField;
    for (int32_t i = 0; i < size; i++)
    {
        const auto data = GetDataFromString(str);
        if (data < 0)
        {
            windField.Reinit();
            return;
        }
        buf[i] = static_cast<uint8_t>(data);
    }
}

// Add float to string
void WdmObjects::AddDataToString(std::string &str, uint8_t d)
{
    char hex[] = "0123456789ABCDEF";
    str += hex[(d >> 4) & 0xf];
    str += hex[(d >> 0) & 0xf];
}

// Get float from string
int32_t WdmObjects::GetDataFromString(const char *&cur)
{
    uint32_t tmp = 0;
    for (int32_t cnt = 0; cnt < 2; cnt++)
    {
        if (!*cur)
            return -1;
        uint32_t v;
        switch (*cur++)
        {
        case '0':
            v = 0x0;
            break;
        case '1':
            v = 0x1;
            break;
        case '2':
            v = 0x2;
            break;
        case '3':
            v = 0x3;
            break;
        case '4':
            v = 0x4;
            break;
        case '5':
            v = 0x5;
            break;
        case '6':
            v = 0x6;
            break;
        case '7':
            v = 0x7;
            break;
        case '8':
            v = 0x8;
            break;
        case '9':
            v = 0x9;
            break;
        case 'a':
        case 'A':
            v = 0xA;
            break;
        case 'b':
        case 'B':
            v = 0xB;
            break;
        case 'c':
        case 'C':
            v = 0xC;
            break;
        case 'd':
        case 'D':
            v = 0xD;
            break;
        case 'e':
        case 'E':
            v = 0xE;
            break;
        case 'f':
        case 'F':
            v = 0xF;
            break;
        default:
            return -1;
        }
        tmp = (tmp << 4) | v;
    }
    return tmp;
}
