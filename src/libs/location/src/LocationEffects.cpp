//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocationEffects
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LocationEffects.h"
#include "core.h"
#include "dx9render.h"

#define LFX_SPLASHES_NUM (sizeof(chrSplash) / sizeof(ChrSplash))
#define LFX_SPLASHES_P_NUM 64 // fix boal for intel cpp (sizeof(LocationEffects::ChrSplash::prt)/sizeof(ParticleSplash))
#define LFX_SPLASHES_SRAD 0.14f

#define LFX_PI 3.141592654f

// ============================================================================================
// Construction, destruction
// ============================================================================================

LocationEffects::LocationEffects()
    : rs(nullptr), smoke{}, flinders{}, blood{}, buffer{}
{
    // Splash
    chrSplashRefCounter = 0;
    for (long i = 0; i < LFX_SPLASHES_NUM; i++)
        chrSplash[i].time = -1.0f;
    splashesTxt = -1;
    // Flies
    numFlys = 0;
    maxFlys = 0;
    numFly = 0;
    flyTex = -1;
    // Shotgan
    numSmoke = 0;
    numFlinders = 0;
    numBlood = 0;
    texSmoke = -1;
    texFlinders = -1;
    texBlood = -1;
    texHor = -1;
    isShgInited = false;
}

LocationEffects::~LocationEffects()
{
    if (rs)
    {
        if (splashesTxt >= 0)
            rs->TextureRelease(splashesTxt);
        if (texSmoke >= 0)
            rs->TextureRelease(texSmoke);
        if (texFlinders >= 0)
            rs->TextureRelease(texFlinders);
        if (texBlood >= 0)
            rs->TextureRelease(texBlood);
        if (texHor >= 0)
            rs->TextureRelease(texHor);
        if (flyTex >= 0)
            rs->TextureRelease(flyTex);
    }
}

// Initialization
bool LocationEffects::Init()
{
    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    // core.LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 10);

    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 1000000);

    splashesTxt = rs->TextureCreate("LocEfx\\chrsplprt.tga");
    flyTex = rs->TextureCreate("LocEfx\\firefly.tga");

    return true;
}

// Execution
void LocationEffects::Execute(uint32_t delta_time)
{
}

void LocationEffects::Realize(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f;
    ProcessedFlys(dltTime);
    ProcessedChrSplash(dltTime);
    ProcessedShotgun(dltTime);
}

// Messages
uint64_t LocationEffects::ProcessMessage(MESSAGE &message)
{
    const std::string &buf = message.String();
    CVECTOR pos, dir;
    if (_stricmp(buf.c_str(), "Splashes") == 0)
    {
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        CreateSplash(pos, message.Float());
    }
    else if (_stricmp(buf.c_str(), "SGFireParticles") == 0)
    {
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        dir.x = message.Float();
        dir.y = message.Float();
        dir.z = message.Float();
        SGFirePrt(pos, dir);
    }
    else if (_stricmp(buf.c_str(), "SGBloodParticles") == 0)
    {
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        dir.x = message.Float();
        dir.y = message.Float();
        dir.z = message.Float();
        SGBldPrt(pos, dir);
    }
    else if (_stricmp(buf.c_str(), "SGEnvParticles") == 0)
    {
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        dir.x = message.Float();
        dir.y = message.Float();
        dir.z = message.Float();
        SGEnvPrt(pos, dir);
    }
    else if (_stricmp(buf.c_str(), "SGInited") == 0)
    {
        SGInited();
    }
    else if (_stricmp(buf.c_str(), "SGRelease") == 0)
    {
        SGRelease();
    }
    else if (_stricmp(buf.c_str(), "AddFly") == 0)
    {
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        AddLampFlys(pos);
    }
    else if (_stricmp(buf.c_str(), "DelFlys") == 0)
    {
        numFlys = 0;
        numFly = 0;
    }
    return 0;
}

inline void LocationEffects::DrawParticles(void *prts, long num, long size, long texture, const char *tech, bool isEx,
                                           long numU)
{
    if (num <= 0)
        return;
    CMatrix camMtx;
    rs->GetTransform(D3DTS_VIEW, camMtx);
    rs->SetTransform(D3DTS_VIEW, CMatrix());
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    rs->TextureSet(0, texture);
    long n = 0;
    for (long i = 0; i < num; i++)
    {
        auto *parts = static_cast<Particle *>(prts);
        prts = static_cast<char *>(prts) + size;
        auto pos = camMtx * parts->pos;
        const auto size = parts->size * 0.5f;
        const auto sn = sinf(parts->angle);
        const auto cs = cosf(parts->angle);
        auto color = (static_cast<long>(parts->alpha) << 24);
        if (!isEx)
            color |= 0x00ffffff;
        else
            color |= 0x00ffffff & static_cast<ParticleEx *>(parts)->color;
        auto u1 = 0.0f;
        auto u2 = 1.0f;
        if (isEx && numU)
        {
            u2 = 1.0f / static_cast<float>(numU);
            u1 = static_cast<long>(static_cast<ParticleEx *>(parts)->frame) * u2;
            u2 += u1;
        }
        buffer[n * 6 + 0].pos = pos + CVECTOR(size * (-cs + sn), size * (sn + cs), 0.0f);
        buffer[n * 6 + 0].color = color;
        buffer[n * 6 + 0].u = u1;
        buffer[n * 6 + 0].v = 0.0f;
        buffer[n * 6 + 1].pos = pos + CVECTOR(size * (-cs - sn), size * (sn - cs), 0.0f);
        buffer[n * 6 + 1].color = color;
        buffer[n * 6 + 1].u = u1;
        buffer[n * 6 + 1].v = 1.0f;
        buffer[n * 6 + 2].pos = pos + CVECTOR(size * (cs + sn), size * (-sn + cs), 0.0f);
        buffer[n * 6 + 2].color = color;
        buffer[n * 6 + 2].u = u2;
        buffer[n * 6 + 2].v = 0.0f;
        buffer[n * 6 + 3].pos = buffer[n * 6 + 2].pos;
        buffer[n * 6 + 3].color = color;
        buffer[n * 6 + 3].u = u2;
        buffer[n * 6 + 3].v = 0.0f;
        buffer[n * 6 + 4].pos = buffer[n * 6 + 1].pos;
        buffer[n * 6 + 4].color = color;
        buffer[n * 6 + 4].u = u1;
        buffer[n * 6 + 4].v = 1.0f;
        buffer[n * 6 + 5].pos = pos + CVECTOR(size * (cs - sn), size * (-sn - cs), 0.0f);
        buffer[n * 6 + 5].color = color;
        buffer[n * 6 + 5].u = u2;
        buffer[n * 6 + 5].v = 1.0f;
        n++;
        if (n * 2 == 256)
        {
            rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n * 2, buffer,
                                sizeof(Vertex), (char *)tech);
            n = 0;
        }
    }
    if (n > 0)
    {
        rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n * 2, buffer,
                            sizeof(Vertex), (char *)tech);
    }
    rs->SetTransform(D3DTS_VIEW, camMtx);
}

void LocationEffects::CreateSplash(const CVECTOR &pos, float power)
{
    // Select the freed block
    long i;
    for (i = 0; i < LFX_SPLASHES_NUM; i++)
        if (chrSplash[i].time < 0.0f)
            break;
    if (i >= LFX_SPLASHES_NUM)
        return;
    if (power < 0.0f)
        power = 0.0f;
    if (power > 1.0f)
        power = 1.0f;
    // initialize
    auto &spl = chrSplash[i];
    spl.time = 0.0f;
    spl.kTime = 1.4f;
    for (i = 0; i < LFX_SPLASHES_P_NUM; i++)
    {
        const auto ang = rand() * (LFX_PI * 2.0f / RAND_MAX);
        const auto r = rand() * (2.0f * LFX_SPLASHES_SRAD / static_cast<float>(RAND_MAX));
        const auto s = 1.0f + rand() * (3.0f / static_cast<float>(RAND_MAX));
        spl.prt[i].dir = CVECTOR(0.3f * sinf(ang), s, 0.3f * cosf(ang));
        spl.prt[i].pos = pos + CVECTOR(r * sinf(ang), 0.0f, r * cosf(ang));
        spl.prt[i].angle = 0.0f;
        spl.prt[i].alpha = 0.0f;
        spl.prt[i].dAng = ((rand() & 7) * (1.0f / 7.0f) - 0.5f) * 0.2f;
        spl.prt[i].size = 0.5f + rand() * (0.5f / static_cast<float>(RAND_MAX));
    }
    if (chrSplashRefCounter < 0)
        chrSplashRefCounter = 0;
    chrSplashRefCounter++;
}

void LocationEffects::ProcessedChrSplash(float dltTime)
{
    dltTime *= 0.9f;
    if (chrSplashRefCounter <= 0)
        return;
    for (long i = 0; i < LFX_SPLASHES_NUM; i++)
    {
        auto &spl = chrSplash[i];
        // Watching life time
        if (spl.time < 0.0f)
            continue;
        if (spl.time >= 1.0f)
        {
            spl.time = -1.0f;
            chrSplashRefCounter--;
            continue;
        }
        // Calculate the alpha value
        float aprt = spl.time;
        if (aprt < 0.1f)
            aprt *= 10.0f;
        else if (aprt > 0.5f)
            aprt = 1.0f - (aprt - 0.5f) / (1.0f - 0.5f);
        else
            aprt = 1.0f;
        aprt = aprt * 255.0f * 0.2f;
        // Particles
        for (long j = 0; j < LFX_SPLASHES_P_NUM; j++)
        {
            spl.prt[j].pos += spl.prt[j].dir * dltTime;
            spl.prt[j].alpha = aprt;
            spl.prt[j].dir.y -= 9.8f * dltTime;
            spl.prt[i].angle += spl.prt[i].dAng * dltTime;
        }
        // increase the life time
        spl.time += dltTime * spl.kTime;
        // Draw
        DrawParticles(spl.prt, LFX_SPLASHES_P_NUM, sizeof(ParticleSplash), splashesTxt, "LocCharacterSplashes");
    }
}

// ---------------------------------------------------
// Flies near laterns
// ---------------------------------------------------

void LocationEffects::AddLampFlys(CVECTOR &pos)
{
    // occupy the array
    if (numFlys >= maxFlys)
    {
        maxFlys += 8;
        flys.resize(maxFlys);
    }
    // Fill in the parameters
    // common
    flys[numFlys].pos = pos;
    flys[numFlys].radius = 0.6f;
    flys[numFlys].start = numFly;
    flys[numFlys].num = 1 + (rand() & 7);
    numFly += flys[numFlys].num;
    fly.resize(numFly);
    // Every fly
    for (long i = 0; i < flys[numFlys].num; i++)
    {
        ParticleFly &f = fly[flys[numFlys].start + i];
        f.ax = rand() * 2.0f * LFX_PI / RAND_MAX;
        f.ay = rand() * 2.0f * LFX_PI / RAND_MAX;
        f.kx = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.kx = -f.kx;
        f.ky = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.ky = -f.ky;
        f.a = rand() * 2.0f * LFX_PI / RAND_MAX;
        f.k = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.k = -f.k;
        f.angle = 0.0f;
        f.size = 0.03f;
        f.alpha = 1.0f;
        f.frame = static_cast<float>(rand() & 3);
    }
    numFlys++;
}

void LocationEffects::ProcessedFlys(float dltTime)
{
    CMatrix view;
    rs->GetTransform(D3DTS_VIEW, view);
    view.Transposition();
    const CVECTOR cam = view.Pos();
    const float dax = dltTime * 1.3f;
    const float day = dltTime * 1.4f;
    const float da = dltTime * 5.6f;
    // calculate
    for (long i = 0; i < numFlys; i++)
    {
        // Coefficient of visibility
        CVECTOR dir = cam - flys[i].pos;
        float k = ~dir;
        if (k > 400.0f)
            continue;
        k = sqrtf(k);
        if (k > 0.0f)
            dir *= 1.0f / k;
        k = k / 20.0f;
        k = 3.0f * (1.0f - k);
        if (k > 1.0f)
            k = 1.0f;
        // Updating flies
        ParticleFly *fl = &fly[flys[i].start];
        for (long j = 0; j < flys[i].num; j++)
        {
            ParticleFly &f = fl[j];
            // Angles
            f.ax += dax * f.kx;
            f.ay += day * f.ky;
            f.a += da * f.k;
            // Radius
            float r = 1.0f + 0.5f * sinf(f.a) + 0.2f * cosf(f.a * f.k * 2.1f);
            r *= flys[i].radius;
            // Position
            f.pos.x = flys[i].pos.x + r * sinf(f.ax) * sinf(f.ay);
            f.pos.y = flys[i].pos.y + r * cosf(f.ax) * cosf(f.ay);
            f.pos.z = flys[i].pos.z + r * sinf(f.ax) * cosf(f.ay);
            // Transparency
            f.alpha = k * 255.0f;
            // Colour
            CVECTOR tmp = f.pos - flys[i].pos;
            float dst = sqrtf(~tmp);
            if (dst > 0.0f)
                tmp *= 1.0f / dst;
            float cs = -(tmp | dir) + 0.4f;
            if (dst > flys[i].radius)
            {
                dst = 3.0f * (dst - flys[i].radius) / flys[i].radius;
                if (dst > 1.0f)
                    dst = 1.0f;
                cs *= 1.0f - dst;
            }
            if (cs < 0.0f)
                cs = 0.0f;
            if (cs > 1.0f)
                cs = 1.0f;
            f.color = static_cast<long>(cs * 255.0f);
            f.color |= (f.color << 16) | (f.color << 8);
            // Frame
            f.frame += dltTime * f.k * 25.0f;
            if (f.frame >= 4.0f)
                f.frame -= 4.0f;
            // Angle
            f.angle += dltTime * f.k * 3.0f;
        }
    }
    // Draw
    DrawParticles(fly.data(), numFly, sizeof(ParticleFly), flyTex, "LocFly", true, 4);
}

//---------------------------------------------------------------------------------

void LocationEffects::SGInited()
{
    if (!isShgInited)
    {
        texSmoke = rs->TextureCreate("LocEfx\\sgsmoke.tga");
        texFlinders = rs->TextureCreate("LocEfx\\sgflinders.tga");
        texBlood = rs->TextureCreate("LocEfx\\sgblood.tga");
        texHor = rs->TextureCreate("LocEfx\\sghor.tga");
        isShgInited = true;
    }
}

void LocationEffects::SGRelease()
{
    if (texSmoke >= 0)
        rs->TextureRelease(texSmoke);
    if (texFlinders >= 0)
        rs->TextureRelease(texFlinders);
    if (texBlood >= 0)
        rs->TextureRelease(texBlood);
    if (texHor >= 0)
        rs->TextureRelease(texHor);
    isShgInited = false;
    texSmoke = -1;
    texFlinders = -1;
    texBlood = -1;
    texHor = -1;
}

void LocationEffects::SGEnvPrt(const CVECTOR &pos, const CVECTOR &ndir)
{
    SGInited();
    const long max = sizeof(flinders) / sizeof(flinders[0]);
    const long num = 4 + (rand() & 3);
    for (long i = 0; i < num && numFlinders < max; i++)
    {
        flinders[numFlinders].pos = pos;
        flinders[numFlinders].spd = ndir;
        float r = rand() * 0.7f / RAND_MAX;
        const float a = rand() * 6.283185307f / (RAND_MAX + 1);
        flinders[numFlinders].spd.y += r * sinf(a);
        r *= cosf(a);
        flinders[numFlinders].spd.x += r * ndir.z;
        flinders[numFlinders].spd.z -= r * ndir.x;
        flinders[numFlinders].spd *= 1.0f + (rand() & 15) * 2.0f / 15.0f;
        flinders[numFlinders].angle = a;
        flinders[numFlinders].dang = 0.3f - 0.6f * rand() / RAND_MAX;
        if (flinders[numFlinders].dang >= 0.0f)
        {
            flinders[numFlinders].dang += 0.1f;
        }
        else
        {
            flinders[numFlinders].dang -= 0.1f;
        }
        flinders[numFlinders].size = 0.1f;
        flinders[numFlinders].alpha = 255.0f;
        flinders[numFlinders].time = 0.0f;
        flinders[numFlinders].ktime = 2.0f;
        numFlinders++;
    }
}

void LocationEffects::SGBldPrt(const CVECTOR &pos, const CVECTOR &ndir)
{
    SGInited();
    const long max = sizeof(blood) / sizeof(blood[0]);
    const long num = 16 + (rand() & 7);
    for (long i = 0; i < num && numBlood < max; i++)
    {
        blood[numBlood].pos = pos;
        blood[numBlood].spd = ndir;
        float r = rand() * 0.7f / RAND_MAX;
        const float a = rand() * 6.283185307f / (RAND_MAX + 1);
        blood[numBlood].spd.y += r * sinf(a);
        r *= cosf(a);
        blood[numBlood].spd.x += r * ndir.z;
        blood[numBlood].spd.z -= r * ndir.x;
        blood[numBlood].spd *= 1.0f + (rand() & 15) * 2.0f / 15.0f;
        blood[numBlood].angle = 0.0f;
        blood[numBlood].dang = 0.3f - 0.6f * rand() / RAND_MAX;
        if (blood[numBlood].dang >= 0.0f)
        {
            blood[numBlood].dang += 0.1f;
        }
        else
        {
            blood[numBlood].dang -= 0.1f;
        }
        blood[numBlood].size = 0.3f;
        blood[numBlood].alpha = 1.0f;
        blood[numBlood].time = 0.0f;
        blood[numBlood].ktime = 1.0f;
        numBlood++;
    }
}

void LocationEffects::SGFirePrt(const CVECTOR &pos, const CVECTOR &ndir)
{
    SGInited();
    const long max = sizeof(smoke) / sizeof(smoke[0]);
    const long num = 5 + (rand() & 3);
    for (long i = 0; i < num && numSmoke < max; i++)
    {
        smoke[numSmoke].pos = pos;
        smoke[numSmoke].spd = ndir;
        float r = rand() * 0.03f / RAND_MAX;
        const float a = rand() * 6.283185307f / (RAND_MAX + 1);
        smoke[numSmoke].spd.y += r * sinf(a);
        r *= cosf(a);
        smoke[numSmoke].spd.x += r * ndir.z;
        smoke[numSmoke].spd.z -= r * ndir.x;
        smoke[numSmoke].spd *= 2.0f + (rand() & 15) * 20.0f / 15.0f;
        smoke[numSmoke].angle = a;
        smoke[numSmoke].dang = 0.3f - 0.6f * rand() / RAND_MAX;
        if (smoke[numSmoke].dang >= 0.0f)
        {
            smoke[numSmoke].dang += 0.1f;
        }
        else
        {
            smoke[numSmoke].dang -= 0.1f;
        }
        smoke[numSmoke].size = 0.001f;
        blood[numBlood].alpha = 0.0f;
        smoke[numSmoke].time = 0.0f;
        smoke[numSmoke].ktime = 1.0f + (rand() & 15) * 0.2f / 15.0f;
        numSmoke++;
    }
}

void LocationEffects::ProcessedShotgun(float dltTime)
{
    if (!isShgInited)
        return;
    CVECTOR winDir = 0.0f;
    VDATA *param = core.Event("EWhr_GetWindAngle", nullptr);
    if (param)
    {
        float ang;
        if (!param->Get(ang))
            ang = 0.0f;
        winDir.x = sinf(ang);
        winDir.z = cosf(ang);
    }
    param = core.Event("EWhr_GetWindSpeed", nullptr);
    if (param)
    {
        float spd;
        if (!param->Get(spd))
            spd = 0.0f;
        if (spd < 0.0f)
            spd = 0.0f;
        if (spd > 20.0f)
            spd = 20.0f;
        winDir *= powf(0.001f + spd / 20.0f, 10.0f) * 10.0f;
    }
    else
    {
        winDir *= 0.05f;
    }
    long i, j;
    for (i = 0, j = 0; i < numSmoke; i++)
    {
        smoke[i].pos += (smoke[i].spd + winDir) * dltTime;
        float k = dltTime * 10.0f;
        if (k > 1.0f)
            k = 1.0f;
        smoke[i].spd -= smoke[i].spd * k;
        smoke[i].angle += smoke[i].dang * dltTime;
        if (smoke[i].size < 0.5f)
        {
            smoke[i].size += dltTime * 10.0f;
        }
        else
        {
            if (smoke[i].size < 2.0f)
            {
                smoke[i].size += dltTime * 1.0f;
            }
            else
                smoke[i].size = 2.0f;
        }
        if (smoke[i].time < 0.1f)
        {
            smoke[i].alpha = smoke[i].time / 0.1f;
        }
        else
        {
            smoke[i].alpha = 1.0f - (smoke[i].time - 0.1f) / (1.0f - 0.1f);
        }
        smoke[i].alpha *= 0.4f * 255.0f;
        smoke[i].time += smoke[i].ktime * dltTime;
        if (smoke[i].time < 1.0f)
        {
            if (i != j)
                smoke[j] = smoke[i];
            j++;
        }
    }
    numSmoke = j;
    for (i = 0, j = 0; i < numBlood; i++)
    {
        blood[i].pos += blood[i].spd * dltTime;
        float k = dltTime * 5.0f;
        if (k > 1.0f)
            k = 1.0f;
        blood[i].spd.x -= blood[i].spd.x * k;
        blood[i].spd.z -= blood[i].spd.z * k;
        blood[i].spd.y -= dltTime * 8.8f;
        if (blood[i].time > 0.9f)
            blood[i].alpha = 1.0f - (blood[i].time - 0.9f) * 10.0f;
        blood[i].alpha = 0.4f * 255.0f;
        blood[i].time += blood[i].ktime * dltTime;
        if (blood[i].time < 1.0f)
        {
            if (i != j)
                blood[j] = blood[i];
            j++;
        }
    }
    numBlood = j;
    for (i = 0, j = 0; i < numFlinders; i++)
    {
        flinders[i].pos += flinders[i].spd * dltTime;
        float k = dltTime * 5.0f;
        if (k > 1.0f)
            k = 1.0f;
        flinders[i].spd.x -= flinders[i].spd.x * k;
        flinders[i].spd.z -= flinders[i].spd.z * k;
        flinders[i].spd.y -= dltTime * 8.8f;
        if (flinders[i].time > 0.9f)
            flinders[i].alpha = 1.0f - (flinders[i].time - 0.9f) * 10.0f;
        flinders[i].time += flinders[i].ktime * dltTime;
        if (flinders[i].time < 1.0f)
        {
            if (i != j)
                flinders[j] = flinders[i];
            j++;
        }
    }
    numFlinders = j;
    //
    if (numFlinders)
    {
        DrawParticles(flinders, numFlinders, sizeof(flinders[0]), texFlinders, "ShootParticles");
    }
    if (numBlood)
    {
        DrawParticles(blood, numBlood, sizeof(blood[0]), texBlood, "ShootParticles");
    }
    if (numSmoke)
    {
        DrawParticles(smoke, numSmoke, sizeof(smoke[0]), texSmoke, "ShootParticles");
    }
    CMatrix mtx;
    rs->GetTransform(D3DTS_VIEW, mtx);
    mtx.Transposition();
    Particle prt;
    prt.pos = mtx.Pos() + mtx.Vz() * 2.0f;
    prt.angle = 0.0f;
    prt.size = 0.1f;
    prt.alpha = 255.0f;
    DrawParticles(&prt, 1, sizeof(prt), texHor, "ShootParticlesNoZ");
}
