#include "..\common_h\exs.h"
//#include <dxerr8.h>
#include "AStereoCapable.h"

//--------------------------------------------------------------------
AStereoCapable::AStereoCapable()
{

}

//--------------------------------------------------------------------
AStereoCapable::~AStereoCapable()
{

}

//--------------------------------------------------------------------
bool AStereoCapable::Unload()
{
	GUARD(AStereoCapable::Unload)

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool AStereoCapable::Play(bool _looped, float _volume, dword _time)
{
	GUARD(AStereoCapable::Unload)

	if (!core->dsBuffer)
		return false;

	if (_volume != -1.f)
		core->SetVolume(_volume);
	if (_time)
		core->SetFadeMode(FADE_IN, _time);

	if (_looped)
	{
		return (core->dsBuffer->Play(0, 0, DSBPLAY_LOOPING) == DS_OK);
	}
	else
		return (core->dsBuffer->Play(0, 0, 0) == DS_OK);

	SOUND_TRACE("STEREO - play started");
	UNGUARD
}

//--------------------------------------------------------------------
bool AStereoCapable::Stop() 
{
	GUARD(AStereoCapable::Stop)
	if (core->dsBuffer)
		core->dsBuffer->Stop();

	return true;
	UNGUARD
}
