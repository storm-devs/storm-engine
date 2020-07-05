#include "..\common_h\exs.h"
#include "PCM3DSound.h"

#ifndef _XBOX
	#define CACHED_BUFFER_CAPS DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_MUTE3DATMAXDISTANCE /*| DSBCAPS_LOCSOFTWARE*/
#else
	#define CACHED_BUFFER_CAPS DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_MUTE3DATMAXDISTANCE
#endif

//--------------------------------------------------------------------
PCM3DSound::PCM3DSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType)
	:VSound(_volumeType)
{
	GUARD(SoundPCM::SoundPCM)
	Assert(_ds8)

	APCMCoded::core  = this;
	A3DCapable::core = this;

	TSoundCore::cached = _cached;
	TSoundCore::looped = _looped;
	TSoundCore::ds8    = _ds8;

	UNGUARD
}

//--------------------------------------------------------------------
PCM3DSound::~PCM3DSound()
{
	GUARD(SoundPCM::~SoundPCM)
	SOUND_TRACE("SoundService: PCM3D::dtor {");

	Unload();

	SOUND_TRACE("SoundService: PCM3D::dtor }");
	UNGUARD
	//api->Trace("PCM_3D: deleted");
}

//--------------------------------------------------------------------
bool PCM3DSound::Unload()
{
	GUARD(SoundPCM::Unload)
	SOUND_TRACE("SoundService: PCM3D::Unload {");
	A3DCapable::Unload();

	SOUND_TRACE("SoundService: PCM3D::Unload }");
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool PCM3DSound::Play (const char *_name, 
		   			   bool _simpleCache /* = false*/,
					   long _time/* = 0*/,
					   const CVECTOR *_startPosition/* = 0*/,
					   float _minDistance /*= -1.0f*/,
					   float _maxDistance /*= -1.0f*/,
					   long  _loopPauseTime /*= 0*/,
					   float _volume /*= 1.0f*/)
{
	GUARD(PCM3DSound::Play)
	SOUND_TRACE2("SoundService: PCM3D::Play (%s) {", _name);
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
		if (!DuplicateBuffer(CACHED_BUFFER_CAPS, cachedData))
			return false;
	}
	else
	{
		if (waveFormat.nChannels > 1)
		{
			api->Trace("Unable to play stereo file %s as 3D", _name);
			return false;
		}

		if (!CreateBuffer(CACHED_BUFFER_CAPS,  
						  dataSize, waveFormat, cachedData))
			return false;

		if(dsBuffer == 0) return false;

		if (!APCMCoded::LoadDataToBuffer(dsBuffer, inFile, dataSize, cachedData))
		{
			A3DCapable::Unload();
			api->fio->_CloseHandle(inFile);
			return false;
		}

		api->fio->_CloseHandle(inFile);
	}

	A3DCapable::Init();
	if (!_simpleCache)
	{
		if (_time)
			TSoundCore::SetFadeMode(FADE_IN, _time);
		if (!A3DCapable::Play(this->looped, _startPosition, _minDistance, _maxDistance, _volume))
			return false;

		SOUND_TRACE2("PCM_3D: '%s' started", _name);

		playing = true;
	}
	SOUND_TRACE("SoundService: PCM3D::Play }");
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool PCM3DSound::IsAlreadyCached(const tCachedData &_cachedData, bool _startCaching)
{
	return (_cachedData.pcmData && !_startCaching);
}

//--------------------------------------------------------------------
void PCM3DSound::Stop (long _time/* = 0*/)
{
	GUARD(PCM3DSound::Stop)
	SOUND_TRACE("SoundService: PCM3D::Stop {");
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
	SOUND_TRACE("SoundService: PCM3D::Stop {");
	UNGUARD
}

//--------------------------------------------------------------------
void PCM3DSound::Release ()
{
	SOUND_TRACE("SoundService: PCM3D::Release {");
	playing = false;
	Unload();
	SOUND_TRACE("SoundService: PCM3D::Release }");
}

//--------------------------------------------------------------------
bool PCM3DSound::ReleaseWithResource (tCachedData *_cachedData)
{
	SOUND_TRACE("SoundService: PCM3D::ReleaseWithResource {");
	if (APCMCoded::UnloadIfThisCached(_cachedData))
	{
		APCMCoded::DeleteCachedData();
		this->onUnload = true;
		return true;
	}

	SOUND_TRACE("SoundService: PCM3D::ReleaseWithResource }");
	return false;
}

//--------------------------------------------------------------------
bool PCM3DSound::IsPlaying ()
{
	GUARD(PCM3DSound::IsPlaying)
	if (!playing || !dsBuffer)
		return false;

	DWORD status;
	dsBuffer->GetStatus(&status);
	playing = status & DSBSTATUS_PLAYING;

	return playing;
	UNGUARD
}

//--------------------------------------------------------------------
float PCM3DSound::GetPosition()
{
	GUARD(PCM3DSound::GetPosition)
	DWORD playPosition, writePosition;
	dsBuffer->GetCurrentPosition(&playPosition, &writePosition);
	return bufferSize ? (((float) playPosition) / bufferSize) : ERROR_POSITION;
	UNGUARD
}

//--------------------------------------------------------------------
void PCM3DSound::Set3DParam (eSoundMessage _message, const void *_op)
{
	A3DCapable::Set3DParameter(_message, _op);
}

//--------------------------------------------------------------------
bool PCM3DSound::DoPerFrameWork(dword _dTime)
{
	GUARD(PCM3DSound::DoPerFrameWork)

	if (this->onUnload)
		return false;

	TSoundCore::HandleFade();

	if (!IsPlaying())
		return false;
	
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void PCM3DSound::SetVolume(float _volume)
{
	SOUND_TRACE("SoundService: PCM3D::SetVolume {");
	TSoundCore::SetVolume(_volume);
	SOUND_TRACE("SoundService: PCM3D::SetVolume }");
}

//--------------------------------------------------------------------
VSound *PCM3DSound::Duplicate()
{
	GUARD(PCM3DSound::Duplicate)
	if (nCopy >= MAX_SOUND_DUPLICATES)
		return this;

	SOUND_TRACE("duplicated PCM3D");
	return (NEW PCM3DSound(*this));
	UNGUARD
}

//--------------------------------------------------------------------
PCM3DSound::PCM3DSound (const PCM3DSound &_source)
	:VSound(_source.volumeType)
{
	nCopy = _source.nCopy + 1;
	APCMCoded::core  = this;
	A3DCapable::core = this;
	APCMCoded::bufferSize = _source.bufferSize;
	A3DCapable::ds3DBuffer = 0;
}

//--------------------------------------------------------------------
void PCM3DSound::Restart()
{
	GUARD(PCM3DSound::Restart)
	if (!dsBuffer)
		return;
	SOUND_TRACE("SoundService: PCM3D::Restart {");

	if (playing && dsBuffer)
		dsBuffer->Stop();
	A3DCapable::Play(this->looped);
	playing = true;
	SOUND_TRACE("SoundService: PCM3D::Restart }");
	UNGUARD
}
 
//--------------------------------------------------------------------
void PCM3DSound::Resume(long _time)
{
	if (playing)
		return;

	SOUND_TRACE("SoundService: PCM3D::Resume {");

	if (_time)
		TSoundCore::SetFadeMode(FADE_IN, _time);
	if (!A3DCapable::Play(this->looped))
		return;

	playing = true;
	SOUND_TRACE("SoundService: PCM3D::Resume }");
}
