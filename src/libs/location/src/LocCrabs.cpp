//============================================================================================
//    LocCrabs
//============================================================================================

#include "LocCrabs.h"
#include "entity.h"
#include "core.h"

//============================================================================================

LocCrabs::LocCrabs()
{
    num = sizeof(crab) / sizeof(LocCrab);
}

LocCrabs::~LocCrabs()
{
}

//============================================================================================

// Initialization
bool LocCrabs::Init()
{
    // Location Pointer
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return false;
    // Execution
    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 100000);
    return true;
}

// Messages
uint64_t LocCrabs::ProcessMessage(MESSAGE &message)
{
    long num = message.Long();
    if (num < 1)
        num = 1;
    if (num > sizeof(crab) / sizeof(LocCrab))
        num = sizeof(crab) / sizeof(LocCrab);
    // Location Pointer
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return 0;
    // start crabs
    for (long i = 0; i < num; i++)
        crab[i].Init(location);
    return 1;
}

// Execution
void LocCrabs::Execute(uint32_t delta_time)
{
}

// Drawing
void LocCrabs::Realize(uint32_t delta_time)
{
    float dltTime = delta_time * 0.001f;
    for (long i = 0; i < num; i++)
        crab[i].Update(dltTime);
}
