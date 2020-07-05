#include "DebugEntity.h"
#include "SoundService.h"




SoundVisualisationEntity::SoundVisualisationEntity()
{
	pSound = NULL;
}

SoundVisualisationEntity::~SoundVisualisationEntity()
{
}

bool SoundVisualisationEntity::Init()
{
	return true;
}

void SoundVisualisationEntity::Realize(dword _dTime)
{
	if (!pSound) return;
	pSound->DebugDraw();
}

void SoundVisualisationEntity::Execute(dword _dTime)
{
}

void SoundVisualisationEntity::SetMasterSoundService (SoundService* pSoundService)
{
	pSound = pSoundService;
}

void SoundVisualisationEntity::Wakeup()
{
	api->LayerCreate("sound_debug_realize", true , false);
	api->LayerSetRealize("sound_debug_realize", true);
	api->LayerAdd("sound_debug_realize", GetID(), -1);
}