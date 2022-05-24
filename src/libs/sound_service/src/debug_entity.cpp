#include "debug_entity.h"
#include "entity.h"
#include "sound_service.h"
#include "core.h"

#include "shared/layers.h"

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
    // core.LayerCreate("sound_debug_realize", true , false);
    core.SetLayerType(SOUND_DEBUG_REALIZE, layer_type_t::realize);
    core.AddToLayer(SOUND_DEBUG_REALIZE, GetId(), -1);
}
