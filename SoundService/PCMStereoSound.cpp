#include "..\common_h\exs.h"
//#include <dxerr8.h>
#include "PCMStereoSound.h"

#ifndef _XBOX
	DWORD BUFFER_CAPS = DSBCAPS_CTRLPAN |
						DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLPOSITIONNOTIFY |
						DSBCAPS_CTRLFREQUENCY | 
						DSBCAPS_GETCURRENTPOSITION2 |
						DSBCAPS_LOCSOFTWARE;
#else
	DWORD BUFFER_CAPS = DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLPOSITIONNOTIFY |
						DSBCAPS_CTRLFREQUENCY;
#endif

//--------------------------------------------------------------------
PCMStereoSound::PCMStereoSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType)
	:VSound(_volumeType)
{
	GUARD(SoundPCM::SoundPCM)
	Assert(_ds8)

	APCMCoded::core  = this;
	AStereoCapable::core = this;

	TSoundCore::cached = _cached;
	TSoundCore::looped = _looped;
	TSoundCore::ds8    = _ds8;

	UNGUARD
}
//--------------------------------------------------------------------
PCMStereoSound::PCMStereoSound (const PCMStereoSound &_source)
:VSound(_source.volumeType)
{
	nCopy = _source.nCopy + 1;
	APCMCoded::core  = this;
	AStereoCapable::core = this;
	APCMCoded::bufferSize = _source.bufferSize;
}
//--------------------------------------------------------------------
PCMStereoSound::~PCMStereoSound()
{
	GUARD(SoundPCM::~SoundPCM)
	SOUND_TRACE("SoundService: PCMStereo::dtor {");

	Unload();

	SOUND_TRACE("SoundService: PCMStereo::dtor }");
	UNGUARD
}

//--------------------------------------------------------------------
bool PCMStereoSound::Unload()
{
	GUARD(SoundPCM::Unload)
	SOUND_TRACE("SoundService: PCMStereo::Unload {");

	AStereoCapable::Unload();

	SOUND_TRACE("SoundService: PCMStereo::Unload }");
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool PCMStereoSound::Play (const char *_name, 
		   			   bool _simpleCache /* = false*/,
					   long _time/* = 0*/,
					   const CVECTOR *_startPosition/* = 0*/,
					   float _minDistance /*= -1.0f*/,
					   float _maxDistance /*= -1.0f*/,
					   long  _loopPauseTime /*= 0*/,
					   float _volume /*= 1.0f*/)
{
	GUARD(PCMStereoSound::Play)
	SOUND_TRACE("SoundService: PCMStereo::Play {");
	Unload();

	HANDLE		inFile = 0;
	DWORD		dataSize;
	tCachedData *cachedData;
	bool		startCaching;
	bool		logError;

	SOUND_TRACE2("PCM_3D: trying to start '%s'", _name);
	if (!APCMCoded::OpenWAVFile(_name, &inFile, &waveFormat, &dataSize, &cachedData, &startCaching, &logError))
	{
		if (logError)
			api->Trace("Unable to open %s", _name);
		return false;
	}
	bufferSize = dataSize;

	if (IsAlreadyCached(*cachedData, startCaching))
	{
		if (!DuplicateBuffer(BUFFER_CAPS, cachedData))
			return false;
	}
	else
	{
		if (!CreateBuffer(BUFFER_CAPS,
						  dataSize, waveFormat, cachedData))
			return false;

		if(dsBuffer == 0) return false;

		if (!APCMCoded::LoadDataToBuffer(dsBuffer, inFile, dataSize, cachedData))
		{
			AStereoCapable::Unload();
			api->fio->_CloseHandle(inFile);
			return false;
		}

		api->fio->_CloseHandle(inFile);
	}

	if (!_simpleCache)
	{
		if (_time)
			TSoundCore::SetFadeMode(FADE_IN, _time);
		if (!AStereoCapable::Play(this->looped, _volume))
			return false;

		SOUND_TRACE2("PCM_3D: '%s' started", _name);

		playing = true;
	}
	SOUND_TRACE("SoundService: PCMStereo::Play }");
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool PCMStereoSound::IsAlreadyCached(const tCachedData &_cachedData, bool _startCaching)
{
	return (_cachedData.pcmData && !_startCaching);
}

//--------------------------------------------------------------------
void PCMStereoSound::Stop (long _time/* = 0*/)
{
	GUARD(PCMStereoSound::Stop)
	SOUND_TRACE("SoundService: PCMStereo::Stop {");
	if (_time)
	{
		TSoundCore::SetFadeMode(FADE_OUT, _time);
	}
	else
	{
		if (dsBuffer)
			dsBuffer->Stop();
		playing = false;
	}

	SOUND_TRACE("SoundService: PCMStereo::Stop }");
	UNGUARD
}

//--------------------------------------------------------------------
void PCMStereoSound::Release ()
{
	SOUND_TRACE("SoundService: PCMStereo::Release {");
	playing = false;
	Unload();
	SOUND_TRACE("SoundService: PCMStereo::Release }");
}

//--------------------------------------------------------------------
bool PCMStereoSound::ReleaseWithResource (tCachedData *_cachedData)
{
	SOUND_TRACE("SoundService: PCMStereo::ReleaseWithResource {");
	if (APCMCoded::UnloadIfThisCached(_cachedData))
	{
		APCMCoded::DeleteCachedData();
		Release();
		return true;
	}

	SOUND_TRACE("SoundService: PCMStereo::ReleaseWithResource }");
	return false;
}

//--------------------------------------------------------------------
bool PCMStereoSound::IsPlaying ()
{
	GUARD(PCMStereoSound::IsPlaying)
	if (!playing || !dsBuffer)
		return false;

	DWORD status;
	dsBuffer->GetStatus(&status);
	playing = status & DSBSTATUS_PLAYING;

	return playing;
	UNGUARD
}

//--------------------------------------------------------------------
float PCMStereoSound::GetPosition()
{
	GUARD(PCMStereoSound::GetPosition)
	DWORD playPosition, writePosition;
	dsBuffer->GetCurrentPosition(&playPosition, &writePosition);
	return bufferSize ? (((float) playPosition) / bufferSize) : ERROR_POSITION;
	UNGUARD
}

//--------------------------------------------------------------------
void PCMStereoSound::Set3DParam (eSoundMessage _message, const void *_op)
{
	return;
}

//--------------------------------------------------------------------
bool PCMStereoSound::DoPerFrameWork(dword _dTime)
{
	GUARD(PCMStereoSound::DoPerFrameWork)

	if (this->onUnload)
		return false;

	TSoundCore::HandleFade();

	if (!playing)
		return false;

	// DEBUG!!!
	//if (playing)
	{
		if (!IsPlaying())
			return false;
	}

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void PCMStereoSound::SetVolume(float _volume)
{
	SOUND_TRACE("SoundService: PCMStereo::SetVolume {");
	TSoundCore::SetVolume(_volume);
	SOUND_TRACE("SoundService: PCMStereo::SetVolume }");
}

//--------------------------------------------------------------------
VSound *PCMStereoSound::Duplicate()
{
	GUARD(PCMStereoSound::Duplicate)
	if (nCopy >= MAX_SOUND_DUPLICATES)
		return this;

	SOUND_TRACE("duplicated PCMStereo");
	return (NEW PCMStereoSound(*this));
	UNGUARD
}

//--------------------------------------------------------------------
void PCMStereoSound::Restart()
{
	GUARD(PCMStereoSound::Restart)
	SOUND_TRACE("SoundService: PCMStereo::Restart {");
	if (!dsBuffer)
		return;

	if (playing && dsBuffer)
		dsBuffer->Stop();
	AStereoCapable::Play(this->looped);
	playing = true;
	SOUND_TRACE("SoundService: PCMStereo::Restart }");
	UNGUARD
}
 
//--------------------------------------------------------------------
void PCMStereoSound::Resume(long _time)
{
	if (playing)
		return;

	SOUND_TRACE("SoundService: PCMStereo::Resume {");
	if (_time)
		TSoundCore::SetFadeMode(FADE_IN, _time);
	if (!AStereoCapable::Play(this->looped))
		return;

	playing = true;
	SOUND_TRACE("SoundService: PCMStereo::Resume }");
}
