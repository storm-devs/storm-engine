//============================================================================================
//	LocCrabs
//============================================================================================

#include "LocCrabs.h"
#include "Entity.h"
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

//Инициализация
bool LocCrabs::Init()
{
    //Указатель на локацию
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return false;
    //Исполнение
    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 100000);
    return true;
}

//Сообщения
uint64_t LocCrabs::ProcessMessage(MESSAGE &message)
{
    long num = message.Long();
    if (num < 1)
        num = 1;
    if (num > sizeof(crab) / sizeof(LocCrab))
        num = sizeof(crab) / sizeof(LocCrab);
    //Указатель на локацию
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return 0;
    //Заводим крабов
    for (long i = 0; i < num; i++)
        crab[i].Init(location);
    return 1;
}

//Исполнение
void LocCrabs::Execute(uint32_t delta_time)
{
}

//Рисование
void LocCrabs::Realize(uint32_t delta_time)
{
    float dltTime = delta_time * 0.001f;
    for (long i = 0; i < num; i++)
        crab[i].Update(dltTime);
}
