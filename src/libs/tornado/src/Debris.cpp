//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Debris
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Debris.h"
#include "Entity.h"
#include "core.h"
#include "geometry.h"
#include "shared/messages.h"
#include "ship_base.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

Debris::Debris(Pillar &_pillar) : pillar(_pillar)
{
    numModels = 0;
    flyCounter = 0;
    SetGlobalAlpha(1.0f);
    soundService = nullptr;
    lastPlayTime = 0.0f;
}

Debris::~Debris()
{
    for (long i = 0; i < numModels; i++)
        EntityManager::EraseEntity(mdl[i].mdl->GetId());
}

void Debris::Init()
{
    AddModel("Tornado\\palka01", 500, 1.5f);
    AddModel("Tornado\\palka02", 400, 1.7f);
    AddModel("Tornado\\palka03", 100, 1.0f);
    AddModel("Tornado\\palka04", 500, 1.1f);
    AddModel("Tornado\\palka05", 25, 0.9f);
    AddModel("Tornado\\Flotsam1", 5, 0.6f);
    AddModel("Tornado\\Flotsam2", 1, 0.4f);
    AddModel("Tornado\\Flotsam3", 20, 0.8f);
    AddModel("Tornado\\Flotsam4", 15, 0.5f);
    AddModel("Tornado\\Flotsam5", 10, 0.85f);
    AddModel("Tornado\\Flotsam6", 5, 1.1f);
    AddModel("Tornado\\Flotsam7", 5, 1.2f);
    NormalazedModels();
    soundService = static_cast<VSoundService *>(core.CreateService("SoundService"));
}

void Debris::Update(float dltTime)
{
    if (numModels == 0)
        return;
    if (lastPlayTime > 0.0f)
        lastPlayTime -= dltTime;
    // Position on the water
    // Initialization
    if (flyCounter < sizeof(fly) / sizeof(ModelInfo))
    {
        if ((rand() & 1) == 1)
        {
            // check nearby ships
            if (IsShip())
            {
                // need to add a new model
                fly[flyCounter].mdl = SelectModel(fly[flyCounter].maxSpeed);
                fly[flyCounter].r = rand() * 10.0f / RAND_MAX;
                fly[flyCounter].y = 0.0f + (rand() & 7);
                fly[flyCounter].a = rand() * 2.0f * TRND_PI / RAND_MAX;
                fly[flyCounter].ay = rand() * (3.0f / RAND_MAX);
                fly[flyCounter].ang = 0.0f;
                fly[flyCounter].scale = 1.0f + (rand() & 3) / 4.0f;
                flyCounter++;
                if (soundService)
                {
                    if (lastPlayTime <= 0.0f)
                    {
                        const auto pos = CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
                        soundService->SoundPlay("TornadoCrackSound", PCM_3D, VOLUME_FX, false, false, true, 0, &pos);
                        lastPlayTime = 0.2f + rand() * (0.2f / RAND_MAX);
                    }
                }
            }
        }
    }
    // Flight
    const auto h = pillar.GetHeight();
    for (long i = 0; i < flyCounter; i++)
    {
        // Updating the height position
        fly[i].ay += dltTime * fly[i].maxSpeed;
        if (fly[i].ay > 20.0f * fly[i].maxSpeed)
            fly[i].ay = 20.0f * fly[i].maxSpeed;
        fly[i].y += dltTime * fly[i].ay;
        if (fly[i].y > h * 0.75f)
        {
            fly[i] = fly[--flyCounter];
            continue;
        }
        // Updating the radius
        auto k = dltTime * 1.0f;
        if (k > 1.0f)
            k = 1.0f;
        fly[i].r += (pillar.GetRaduis(fly[i].y) - fly[i].r) * k;
        // Updating angle
        k = pillar.GetKHeight(fly[i].y) - 0.5f;
        k = 1.0f - k * k * 4.0f;
        k = dltTime * (3.0f + 5.0f * k);
        fly[i].a += k;
        if (fly[i].a > 2.0f * TRND_PI)
            fly[i].a -= 2.0f * TRND_PI;
        fly[i].ang.y += k * 1.01f;
        if (fly[i].ang.y > 2.0f * TRND_PI)
            fly[i].ang.y -= 2.0f * TRND_PI;
        fly[i].ang.x += dltTime * 2.21f;
        if (fly[i].ang.x > 2.0f * TRND_PI)
            fly[i].ang.x -= 2.0f * TRND_PI;
        fly[i].ang.z += dltTime * 3.37f;
        if (fly[i].ang.z > 2.0f * TRND_PI)
            fly[i].ang.z -= 2.0f * TRND_PI;
        // Transparency
        fly[i].alpha = 1.0f;
        if (fly[i].y < 3.0f)
            fly[i].alpha *= fly[i].y / 3.0f;
        if (fly[i].y > h * 0.5f)
            fly[i].alpha *= (fly[i].y - h * 0.5f) / (h * 0.75f - h * 0.5f);
        if (fly[i].alpha < 0.0f)
            fly[i].alpha = 0.0f;
    }
}

void Debris::Draw(VDX9RENDER *rs)
{
    for (long i = 0; i < flyCounter; i++)
    {
        // Model position
        CVECTOR pos;
        pos.x = pillar.GetX(fly[i].y) + fly[i].r * sinf(fly[i].a);
        pos.y = fly[i].y;
        pos.z = pillar.GetZ(fly[i].y) + fly[i].r * cosf(fly[i].a);
        fly[i].mdl->mtx.BuildMatrix(fly[i].ang, pos);
        for (long a = 0; a < 3; a++)
            for (long b = 0; b < 3; b++)
                fly[i].mdl->mtx.m[a][b] *= fly[i].scale;
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, (static_cast<long>(fly[i].alpha * galpha) << 24) | 0xffffff);
        fly[i].mdl->ProcessStage(Entity::Stage::realize, 10);
    }
}

void Debris::AddModel(const char *modelName, float prt, float spd)
{
    if (numModels > _countof(mdl))
        return;
    // Create a model
    entid_t id;
    if (!(id = EntityManager::CreateEntity("modelr")))
        return;
    auto *m = static_cast<MODEL *>(EntityManager::GetEntityPointer(id));
    if (!m)
        return;
    // Path to textures
    auto *gs = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    if (!gs)
        return;
    gs->SetTexturePath("Tornado\\");
    // Loading
    try
    {
        core.Send_Message(id, "ls", MSG_MODEL_LOAD_GEO, modelName);
    }
    catch (...)
    {
        gs->SetTexturePath("");
        EntityManager::EraseEntity(id);
        return;
    }
    gs->SetTexturePath("");
    // Configuring
    auto *node = m->GetNode(0);
    if (node)
        node->SetTechnique("TornadoDebris");
    // save
    mdl[numModels].mdl = m;
    mdl[numModels].prt = prt;
    mdl[numModels++].maxSpeed = spd;
}

void Debris::NormalazedModels()
{
    auto sum = 0.0f;
    for (long i = 0; i < numModels; i++)
        sum += mdl[i].prt;
    for (long i = 0; i < numModels; i++)
        mdl[i].prt /= sum;
}

MODEL *Debris::SelectModel(float &maxSpd)
{
    const auto rnd = rand() * (1.0f / RAND_MAX);
    auto sum = 0.0f;
    long i;
    for (i = 0; i < numModels - 1; i++)
    {
        sum += mdl[i].prt;
        if (rnd < sum)
        {
            maxSpd = mdl[i].maxSpeed * (0.5f + rand() * (1.4f / RAND_MAX));
            return mdl[i].mdl;
        }
    }
    maxSpd = mdl[i].maxSpeed;
    return mdl[i].mdl;
}

bool Debris::IsShip()
{
    const CVECTOR p(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
    CVECTOR pos;
    const auto &entities = EntityManager::GetEntityIdVector("ship");
    for (auto id : entities)
    {
        // Object pointer
        auto *ship = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(id));
        if (!ship)
            break;
        // Tornado position in the ship system
        Assert(ship->GetMatrix());
        ship->GetMatrix()->MulToInv(p, pos);
        // Check the box
        const auto s = ship->GetBoxsize();
        if (pos.x < -s.x - 6.0f || pos.x > s.x + 6.0f)
            continue;
        if (pos.z < -s.z - 6.0f || pos.z > s.z + 6.0f)
            continue;

        return true;
    }
    return false;
}

void Debris::SetGlobalAlpha(float a)
{
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;
    galpha = a * 255.0f;
}
