//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Lizards
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Lizards.h"
#include "EntityManager.h"

Lizards::Lizards()
{
    num = sizeof(lizard) / sizeof(Lizard);
}

Lizards::~Lizards()
{
}

//Инициализация
bool Lizards::Init()
{
    //Указатель на локацию
    const auto loc = EntityManager::GetEntityId("location");
    auto *location = (Location *)EntityManager::GetEntityPointer(loc);
    if (!location)
        return false;
    //Заводим ящерец
    for (long i = 0; i < num; i++)
        lizard[i].Init(location);
    //Исполнение
    // api->LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 100000);
    return true;
}

//Исполнение
void Lizards::Execute(uint32_t delta_time)
{
}

//Рисование
void Lizards::Realize(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f;
    for (long i = 0; i < num; i++)
        lizard[i].Update(dltTime);
}
