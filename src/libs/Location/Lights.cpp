//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lights
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Lights.h"

#define LIGHTS_DEBUG

// ============================================================================================
// Construction, destruction
// ============================================================================================

Lights::Lights()
{
    rs = nullptr;
    collide = nullptr;
    numTypes = 0;
    maxTypes = 0;
    numLights = 0;
    maxLights = 0;
    for (long i = 0; i < 8; i++)
    {
        lt[i].light = -1;
        lt[i].set = false;
    }
    numLampModels = 0;
}

Lights::~Lights()
{
    aMovingLight.clear();
    for (long i = 0; i < numTypes; i++)
    {
        if (types[i].corona >= 0 && rs)
            rs->TextureRelease(types[i].corona);
        delete types[i].name;
    }
    if (rs)
        for (long i = 1; i < 8; i++)
            rs->LightEnable(i, false);
}

// Initialization
bool Lights::Init()
{
    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::exception("No service: dx9render");
    collide = static_cast<COLLIDE *>(core.CreateService("COLL"));
    // read the parameters
    auto ini = fio->OpenIniFile("RESOURCE\\Ini\\lights.ini");
    if (!ini)
    {
        core.Trace("Location lights not inited -> RESOURCES\\Ini\\lights.ini not found");
        return false;
    }
    char lName[256];
    auto res = ini->GetSectionName(lName, sizeof(lName) - 1);
    while (res)
    {
        lName[sizeof(lName) - 1] = 0;
        long i;
        for (i = 0; i < numTypes; i++)
        {
            if (_stricmp(lName, types[i].name) == 0)
            {
                core.Trace("Location lights redefinition light: %s", lName);
                break;
            }
        }
        if (i == numTypes)
        {
            // Add a new source
            if (numTypes >= maxTypes)
            {
                maxTypes += 16;
                types.resize(maxTypes);
            }
            // Zero down
            memset(&types[numTypes], 0, sizeof(types[numTypes]));
            // Save the name
            const auto len = strlen(lName) + 1;
            types[numTypes].name = new char[len];
            memcpy(types[numTypes].name, lName, len);
            // Reading parameters
            types[numTypes].color.b = ini->GetFloat(lName, "b", 1.0f);
            types[numTypes].color.g = ini->GetFloat(lName, "g", 1.0f);
            types[numTypes].color.r = ini->GetFloat(lName, "r", 1.0f);
            types[numTypes].color.a = 1.0f;
            types[numTypes].dxLight.Type = D3DLIGHT_POINT;
            types[numTypes].dxLight.Diffuse = types[numTypes].color;
            types[numTypes].dxLight.Range = ini->GetFloat(lName, "range", 10.0f);
            types[numTypes].dxLight.Attenuation0 = ini->GetFloat(lName, "att0", 0.0f);
            types[numTypes].dxLight.Attenuation1 = ini->GetFloat(lName, "att1", 0.0f);
            types[numTypes].dxLight.Attenuation2 = ini->GetFloat(lName, "att2", 1.0f);
            types[numTypes].flicker = ini->GetFloat(lName, "flicker", 0.0f);
            types[numTypes].freq = ini->GetFloat(lName, "freq", 0.0f);
            types[numTypes].flickerSlow = ini->GetFloat(lName, "flickerSlow", 0.0f);
            types[numTypes].freqSlow = ini->GetFloat(lName, "freqSlow", 0.0f);
            types[numTypes].corona = -1;
            types[numTypes].coronaRange = ini->GetFloat(lName, "coronaRange", 20.0f);
            types[numTypes].coronaSize = ini->GetFloat(lName, "coronaSize", 1.0f);
            if (types[numTypes].coronaRange > 0.0f && types[numTypes].coronaSize > 0.0f)
            {
                types[numTypes].invCoronaRange = 1.0f / types[numTypes].coronaRange;
                char texture[256];
                if (ini->ReadString(lName, "corona", texture, sizeof(texture), ""))
                {
                    if (texture[0])
                    {
                        types[numTypes].corona = rs->TextureCreate(texture);
                    }
                }
            }
            types[numTypes].coronaRange2 = types[numTypes].coronaRange * types[numTypes].coronaRange;
            if (types[numTypes].flicker <= 0.0f)
                types[numTypes].freq = 0.0f;
            if (types[numTypes].flicker > 1.0f)
                types[numTypes].flicker = 1.0f;
            if (types[numTypes].freq <= 0.0f)
                types[numTypes].flicker = 0.0f;
            if (types[numTypes].freq > 0.0f)
                types[numTypes].p = 1.0f / types[numTypes].freq;
            if (types[numTypes].flickerSlow <= 0.0f)
                types[numTypes].freqSlow = 0.0f;
            if (types[numTypes].flickerSlow > 1.0f)
                types[numTypes].flickerSlow = 1.0f;
            if (types[numTypes].freqSlow <= 0.0f)
                types[numTypes].flickerSlow = 0.0f;
            if (types[numTypes].freqSlow > 0.0f)
                types[numTypes].pSlow = 1.0f / types[numTypes].freqSlow;
            numTypes++;
        }
        res = ini->GetSectionNameNext(lName, sizeof(lName) - 1);
    }
    if (numTypes == 0)
    {
        core.Trace("Location lights not inited -> 0 light types");
        return false;
    }
    // start executing
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 10);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 1001000);
    return true;
}

// Execution
void Lights::Execute(uint32_t delta_time)
{
#ifdef LIGHTS_DEBUG
    if (core.Controls->GetDebugAsyncKeyState(VK_F11) < 0)
    {
        for (long i = 0; i < numTypes; i++)
            UpdateLightTypes(i);
    }
#endif
    for (long i = 0; i < numLights; i++)
    {
        // See what is there
        auto &l = types[lights[i].type];
        if (l.flicker == 0.0f && l.flickerSlow == 0.0f)
            continue;
        // Updating the state
        auto &ls = lights[i];
        // Frequent flickering
        ls.time += delta_time * 0.001f;
        if (ls.time > l.p)
        {
            ls.time -= l.p;
            ls.itens = (1.0f - rand() * 2.0f / RAND_MAX) * l.flicker;
        }
        // Smooth flickering
        ls.timeSlow += delta_time * 0.001f;
        auto k = ls.timeSlow * l.freqSlow;
        if (k < 0.0f)
            k = 0.0f;
        if (k >= 1.0f)
        {
            k = 0.0f;
            ls.timeSlow -= l.pSlow;
            ls.itensDlt = ls.itensSlow;
            ls.itensSlow = (1.0f - rand() * 2.0f / RAND_MAX) * l.flickerSlow;
            ls.itensDlt = ls.itensSlow - ls.itensDlt;
        }
        ls.i = ls.itensSlow - ls.itensDlt + k * ls.itensDlt + ls.itens;
        k = 1.0f + ls.i;
        if (k < 0.0f)
            k = 0.0f;
        lights[i].color.b = k * l.color.b;
        lights[i].color.g = k * l.color.g;
        lights[i].color.r = k * l.color.r;
    }
}

// Drawing crowns
void Lights::Realize(uint32_t delta_time)
{
    // Camera position
    CVECTOR pos, ang;
    rs->GetCamera(pos, ang, ang.x);
    CMatrix camMtx;
    rs->GetTransform(D3DTS_VIEW, camMtx);
    rs->SetTransform(D3DTS_VIEW, CMatrix());
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    const auto camPDist = -(pos.x * camMtx.Vx().z + pos.y * camMtx.Vy().z + pos.z * camMtx.Vz().z);
    for (long i = 0, n = 0; i < numLights; i++)
    {
        // Source
        auto &ls = lights[i];
        auto &l = types[ls.type];
        if (l.corona < 0)
            continue;
        // in the foreground
        auto dist = ls.pos.x * camMtx.Vx().z + ls.pos.y * camMtx.Vy().z + ls.pos.z * camMtx.Vz().z + camPDist;
        if (dist < -2.0f * l.coronaSize)
            continue;
        // Distance
        const auto dx = ls.pos.x - pos.x;
        const auto dy = ls.pos.y - pos.y;
        const auto dz = ls.pos.z - pos.z;
        auto d = dx * dx + dy * dy + dz * dz;
        auto isVisible = d < l.coronaRange2;
        if (!isVisible)
            continue;
        // Visibility
        if (collide)
        {
            const auto dist = collide->Trace(EntityManager::GetEntityIdIterators(SUN_TRACE), pos,
                                             CVECTOR(ls.pos.x, ls.pos.y, ls.pos.z), lampModels, numLampModels);
            isVisible = dist > 1.0f;
        }
        ls.corona += isVisible ? 0.008f * delta_time : -0.008f * delta_time;
        if (ls.corona <= 0.0f)
        {
            ls.corona = 0.0f;
            continue;
        }
        if (ls.corona > 1.0f)
            ls.corona = 1.0f;
        // Distance
        dist = sqrtf(d);
        d = dist * l.invCoronaRange;
        if (d > 1.0f)
            d = 1.0f;
        auto alpha = 1.0f;
        if (d < 0.3f)
            alpha *= 0.2f + 0.8f * d / 0.3f;
        if (d > 0.4f)
        {
            alpha *= 1.0f - (d - 0.4f) / 0.6f;
            alpha *= alpha;
        }
        alpha *= ls.corona * 255.0f;
        // Deviation coefficient
        d = ls.i * 0.4f;
        if (d < -0.1f)
            d = -0.1f;
        if (d > 0.1f)
            d = 0.1f;
        d += 1.0f;
        // Current size
        const auto size = d * l.coronaSize;
        // Transparency
        alpha *= d;
        if (alpha < 0.0f)
            alpha = 0.0f;
        if (alpha > 255.0f)
            alpha = 255.0f;
        // Position
        auto pos = camMtx * CVECTOR(ls.pos.x, ls.pos.y, ls.pos.z);
        // Colour
        auto c = static_cast<uint32_t>(alpha);
        c |= (c << 24) | (c << 16) | (c << 8);
        // Angle of rotation
        float cs, sn;
        if (dist > 0.0f)
        {
            auto _cs = (dx * camMtx.Vx().z + dz * camMtx.Vz().z);
            auto _sn = (dx * camMtx.Vz().z - dz * camMtx.Vx().z);
            auto kn = _cs * _cs + _sn * _sn;
            if (kn > 0.0f)
            {
                kn = 1.0f / sqrtf(kn);
                _cs *= kn;
                _sn *= kn;
                cs = (_cs * _cs - _sn * _sn);
                sn = 2.0f * _cs * _sn;
            }
            else
            {
                cs = 1.0f;
                sn = 0.0f;
            }
        }
        else
        {
            cs = 1.0f;
            sn = 0.0f;
        }
        // fill in
        buf[n * 6 + 0].pos = pos + CVECTOR(size * (-cs + sn), size * (sn + cs), 0.0f);
        buf[n * 6 + 0].color = c;
        buf[n * 6 + 0].u = 0.0f;
        buf[n * 6 + 0].v = 0.0f;
        buf[n * 6 + 1].pos = pos + CVECTOR(size * (-cs - sn), size * (sn - cs), 0.0f);
        buf[n * 6 + 1].color = c;
        buf[n * 6 + 1].u = 0.0f;
        buf[n * 6 + 1].v = 1.0f;
        buf[n * 6 + 2].pos = pos + CVECTOR(size * (cs + sn), size * (-sn + cs), 0.0f);
        buf[n * 6 + 2].color = c;
        buf[n * 6 + 2].u = 1.0f;
        buf[n * 6 + 2].v = 0.0f;
        buf[n * 6 + 3].pos = buf[n * 6 + 2].pos;
        buf[n * 6 + 3].color = c;
        buf[n * 6 + 3].u = 1.0f;
        buf[n * 6 + 3].v = 0.0f;
        buf[n * 6 + 4].pos = buf[n * 6 + 1].pos;
        buf[n * 6 + 4].color = c;
        buf[n * 6 + 4].u = 0.0f;
        buf[n * 6 + 4].v = 1.0f;
        buf[n * 6 + 5].pos = pos + CVECTOR(size * (cs - sn), size * (-sn - cs), 0.0f);
        buf[n * 6 + 5].color = c;
        buf[n * 6 + 5].u = 1.0f;
        buf[n * 6 + 5].v = 1.0f;
        n++;
        rs->TextureSet(0, l.corona);
        rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n * 2, buf, sizeof(Vertex),
                            "Coronas");
        n = 0;
    }
    rs->SetTransform(D3DTS_VIEW, camMtx);
}

// Find source index
long Lights::FindLight(const char *name)
{
    if (!name || !name[0])
        return -1;
    for (long i = 0; i < numTypes; i++)
        if (_stricmp(name, types[i].name) == 0)
            return i;
    return -1;
}

// Add source to location
void Lights::AddLight(long index, const CVECTOR &pos)
{
    if (index == -1)
        return;
    Assert(index >= 0 && index < numTypes);
    if (numLights >= maxLights)
    {
        maxLights += 32;
        lights.resize(maxLights);
    }
    lights[numLights].color = types[index].color;
    lights[numLights].pos.x = pos.x;
    lights[numLights].pos.y = pos.y;
    lights[numLights].pos.z = pos.z;
    lights[numLights].time = 0.0f;
    lights[numLights].itens = 0.0f;
    lights[numLights].itensSlow = 0.0f;
    lights[numLights].itensDlt = 0.0f;
    lights[numLights].timeSlow = 0.0f;
    lights[numLights].type = index;

    // Send a message to the lighter
    if (const auto eid = EntityManager::GetEntityId("Lighter"))
    {
        core.Send_Message(eid, "sffffffffffs", "AddLight", pos.x, pos.y, pos.z, types[index].color.r,
                          types[index].color.g, types[index].color.b, types[index].dxLight.Attenuation0,
                          types[index].dxLight.Attenuation1, types[index].dxLight.Attenuation2,
                          types[index].dxLight.Range, types[index].name);
    }

    numLights++;
}

// Add lantern model
bool Lights::AddLampModel(const entid_t lampModel)
{
    if (numLampModels >= sizeof(lampModels) / sizeof(entid_t))
        return false;
    lampModels[numLampModels++] = lampModel;
    return true;
}

void Lights::DelAllLights()
{
    aMovingLight.clear();
    numLights = 0;
}

// Add portable source
long Lights::AddMovingLight(const char *type, const CVECTOR &pos)
{
    long idx;
    for (idx = 0; idx < 1000; idx++)
    {
        long n;
        for (n = 0; n < aMovingLight.size(); n++)
            if (aMovingLight[n].id == idx)
                break;
        if (n == aMovingLight.size())
            break;
    }
    if (idx == 1000)
        return -1;

    const auto nType = FindLight(type);
    if (nType < 0)
        return -1;

    MovingLight movingLight;
    movingLight.id = idx;
    movingLight.light = numLights;
    aMovingLight.push_back(movingLight);
    // long i = aMovingLight.Add();
    // aMovingLight[i].id = idx;
    // aMovingLight[i].light = numLights;
    AddLight(nType, pos);
    return idx;
}

// Put portable source in new position
void Lights::UpdateMovingLight(long id, const CVECTOR &pos)
{
    for (long n = 0; n < aMovingLight.size(); n++)
        if (aMovingLight[n].id == id)
        {
            const auto i = aMovingLight[n].light;
            if (i >= 0 && i < numLights)
                lights[i].pos = *(D3DVECTOR *)&pos;
            return;
        }
}

// Remove portable source
void Lights::DelMovingLight(long id)
{
    for (long n = 0; n < aMovingLight.size(); n++)
        if (aMovingLight[n].id == id)
        {
            numLights--;
            for (auto i = aMovingLight[n].light; i < numLights; i++)
                lights[i] = lights[i + 1];
            aMovingLight.erase(aMovingLight.begin() + n);
            return;
        }
}

// Set light sources for the character
void Lights::SetCharacterLights(const CVECTOR *const pos)
{
    // Filling the source array
    long i;
    long n;
    if (pos)
    {
        std::vector<long> aLightsSort;
        for (i = 0; i < numLights; i++)
            aLightsSort.push_back(i);
        for (i = 0; i < aMovingLight.size(); i++)
        {
            const auto it = std::find(aLightsSort.begin(), aLightsSort.end(), aMovingLight[i].light);
            if (it != aLightsSort.end())
                aLightsSort.erase(it);
            aLightsSort.insert(aLightsSort.begin(), aMovingLight[i].light);
        }

        // Sort by distance
        aLightsDstSort.clear();
        for (n = 0; n < aLightsSort.size(); n++)
        {
            i = aLightsSort[n];

            // Checking the distance
            const auto dx = (pos->x - lights[i].pos.x);
            const auto dy = (pos->y - lights[i].pos.y);
            const auto dz = (pos->z - lights[i].pos.z);
            const auto dst = dx * dx + dy * dy + dz * dz + 2.0f;
            const float rng = types[lights[i].type].dxLight.Range;

            if (dst <= rng * rng)
            {
                int j;
                for (j = 0; j < aLightsDstSort.size(); j++)
                {
                    if (dst < aLightsDstSort[j].dst)
                        break;
                }

                lt_elem le = {i, dst};
                if (j == aLightsDstSort.size())
                    aLightsDstSort.push_back(le);
                else
                    aLightsDstSort.insert(aLightsDstSort.begin() + j, le);

                if (aLightsDstSort.size() == 8)
                    aLightsDstSort.erase(aLightsDstSort.begin() + 7);
            }
        }
    }

    for (n = 0; n < aLightsDstSort.size(); n++)
    {
        i = aLightsDstSort[n].idx;

        // Setting the source
        LightType &l = types[lights[i].type];
        l.dxLight.Diffuse = lights[i].color;
        l.dxLight.Position = lights[i].pos;
        rs->SetLight(n + 1, &l.dxLight);
        rs->LightEnable(n + 1, true);
        lt[n + 1].light = i;
        lt[n + 1].set = true;
    }

    // Turn off the rest
    while (++n < 8)
    {
        lt[n].light = -1;
        lt[n].set = false;
    }
}

// Disable light sources set for the character
void Lights::DelCharacterLights()
{
    for (long i = 1; i < 8; i++)
    {
        if (!lt[i].set)
            continue;
        lt[i].set = false;
        rs->LightEnable(i, false);
    }
}

// Update source types
void Lights::UpdateLightTypes(long i)
{
    auto ini = fio->OpenIniFile("RESOURCE\\Ini\\lights.ini");
    if (!ini)
        return;
    // Source name
    char *lName = types[i].name;
    // Reading parameters
    types[i].color.b = ini->GetFloat(lName, "b", 1.0f);
    types[i].color.g = ini->GetFloat(lName, "g", 1.0f);
    types[i].color.r = ini->GetFloat(lName, "r", 1.0f);
    types[i].color.a = 1.0f;
    types[i].dxLight.Type = D3DLIGHT_POINT;
    types[i].dxLight.Diffuse = types[i].color;
    types[i].dxLight.Range = ini->GetFloat(lName, "range", 10.0f);
    types[i].dxLight.Attenuation0 = ini->GetFloat(lName, "att0", 0.0f);
    types[i].dxLight.Attenuation1 = ini->GetFloat(lName, "att1", 0.0f);
    types[i].dxLight.Attenuation2 = ini->GetFloat(lName, "att2", 1.0f);
    types[i].flicker = ini->GetFloat(lName, "flicker", 0.0f);
    types[i].freq = ini->GetFloat(lName, "freq", 0.0f);
    types[i].flickerSlow = ini->GetFloat(lName, "flickerSlow", 0.0f);
    types[i].freqSlow = ini->GetFloat(lName, "freqSlow", 0.0f);
    types[i].corona = -1;
    types[i].coronaRange = ini->GetFloat(lName, "coronaRange", 20.0f);
    types[i].coronaSize = ini->GetFloat(lName, "coronaSize", 1.0f);
    if (types[i].coronaRange > 0.0f && types[i].coronaSize > 0.0f)
    {
        types[i].invCoronaRange = 1.0f / types[i].coronaRange;
        char texture[256];
        if (ini->ReadString(lName, "corona", texture, sizeof(texture), ""))
        {
            if (texture[0])
            {
                types[i].corona = rs->TextureCreate(texture);
            }
        }
    }
    types[i].coronaRange2 = types[i].coronaRange * types[i].coronaRange;
    if (types[i].flicker <= 0.0f)
        types[i].freq = 0.0f;
    if (types[i].flicker > 1.0f)
        types[i].flicker = 1.0f;
    if (types[i].freq <= 0.0f)
        types[i].flicker = 0.0f;
    if (types[i].freq > 0.0f)
        types[i].p = 1.0f / types[i].freq;
    if (types[i].flickerSlow <= 0.0f)
        types[i].freqSlow = 0.0f;
    if (types[i].flickerSlow > 1.0f)
        types[i].flickerSlow = 1.0f;
    if (types[i].freqSlow <= 0.0f)
        types[i].flickerSlow = 0.0f;
    if (types[i].freqSlow > 0.0f)
        types[i].pSlow = 1.0f / types[i].freqSlow;
    for (long c = 0; c < numLights; c++)
    {
        if (lights[c].type != i)
            continue;
        lights[c].color = types[i].color;
    }
}
