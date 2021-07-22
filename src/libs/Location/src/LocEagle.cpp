//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocEagle
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LocEagle.h"
#include "Entity.h"
#include "Location.h"
#include "core.h"
#include "geometry.h"
#include "shared/messages.h"

LocEagle::LocEagle()
{
    time = 1.0f;
    kTime = 1.0f;
    y = 0.0f;
    dltY = 0.0f;
    kRad = 1.0f;
    dltRad = 0.0f;
    kAy = 1.0f;
    timeAy = 0.0f;
    ay = rand() * 6.5f / RAND_MAX;
}

LocEagle::~LocEagle()
{
    EntityManager::EraseEntity(mdl);
}

// Initialization
bool LocEagle::Init()
{
    // The point we fly around
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = static_cast<Location *>(EntityManager::GetEntityPointer(loc));
    if (!location)
        return false;
    cnt = location->GetPtcData().middle + CVECTOR(0.0f, 30.0f, 0.0f);
    // Path for textures
    auto *gs = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    if (!gs)
    {
        core.Trace("Can't create geometry service!");
        return false;
    }
    // Model
    if (!(mdl = EntityManager::CreateEntity("modelr")))
        return false;
    EntityManager::AddToLayer(REALIZE, mdl, 20);
    gs->SetTexturePath("Animals\\");
    if (!core.Send_Message(mdl, "ls", MSG_MODEL_LOAD_GEO, "Animals\\eagle"))
    {
        gs->SetTexturePath("");
        return false;
    }
    gs->SetTexturePath("");
    // Animation
    if (!core.Send_Message(mdl, "ls", MSG_MODEL_LOAD_ANI, "eagle"))
        return false;
    // Start playing the animation
    auto *m = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdl));
    if (!m)
        return false;
    auto *ani = m->GetAnimation();
    if (!ani)
        return false;
    if (!ani->Player(0).SetAction("flight"))
        return false;
    if (!ani->Player(0).Play())
        return false;
    // include in the execution list
    // core.LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 10);
    return true;
}

// Execution
void LocEagle::Execute(uint32_t delta_time)
{
    // Model
    auto *m = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdl));
    if (!m)
        return;
    // Updating position
    const auto dltTime = delta_time * 0.001f;
    time += kTime * dltTime;
    if (time >= 1.0f)
    {
        dltY = ((rand() & 15) * (2.0f / 15.0f) - 1.0f) * 1.0f - y;
        time = 0.0f;
        kTime = 1.0f / (2.0f + rand() * (20.0f / RAND_MAX));
        dltRad = (0.5f + ((rand() & 7) * (1.0f / 7.0f) * 0.7f)) - kRad;
    }
    timeAy -= dltTime;
    if (timeAy <= 0.0f)
    {
        if (rand() & 3)
        {
            // Fast
            timeAy = 10.0f + rand() * (10.0f / RAND_MAX);
            kAy = 1.0f;
        }
        else
        {
            // Slow
            timeAy = 1.0f + rand() * (2.0f / RAND_MAX);
            kAy = 0.4f;
            time = 0.0f;
            kTime = 1.0f / (1.0f + rand() * (1.0f / RAND_MAX));
            dltY = -1.0f - y;
        }
    }
    ay += dltTime * kAy * 0.1f / kRad;
    y += dltY * kTime * dltTime;
    // Set position
    m->mtx.BuildMatrix(0.0f, ay + 1.57f, 0.0f);
    m->mtx.Pos().x = cnt.x + kRad * 20.0f * sinf(ay);
    m->mtx.Pos().y = cnt.y + y;
    m->mtx.Pos().z = cnt.z + kRad * 20.0f * cosf(ay);
}
