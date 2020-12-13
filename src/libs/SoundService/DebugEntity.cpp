#include "DebugEntity.h"
#include "EntityManager.h"
#include "SoundService.h"

SoundVisualisationEntity::SoundVisualisationEntity()
{
    pSound = nullptr;
}

SoundVisualisationEntity::~SoundVisualisationEntity()
{
}

bool SoundVisualisationEntity::Init()
{
    return true;
}

void SoundVisualisationEntity::Realize(uint32_t dTime)
{
    if (!pSound)
        return;
    pSound->DebugDraw();
}

void SoundVisualisationEntity::Execute(uint32_t dTime)
{
}

void SoundVisualisationEntity::SetMasterSoundService(SoundService *pSoundService)
{
    pSound = pSoundService;
}

void SoundVisualisationEntity::Wakeup() const
{
    // api->LayerCreate("sound_debug_realize", true , false);
    EntityManager::SetLayerType(SOUND_DEBUG_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(SOUND_DEBUG_REALIZE, GetId(), -1);
}
