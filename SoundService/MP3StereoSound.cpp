#include "..\common_h\exs.h"
#include "..\common_h\defines.h"
//#include <dxerr8.h>
#include "MP3StereoSound.h"

//--------------------------------------------------------------------
MP3StereoSound::MP3StereoSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType)
	:VSound(_volumeType)
{
	GUARD(MP3StereoSound::MP3StereoSound)
	Assert(_ds8)

	AMP3Coded::core      = this;
	AStereoCapable::core = this;

	TSoundCore::cached = _cached;
	TSoundCore::looped = _looped;
	TSoundCore::ds8    = _ds8;

	UNGUARD
}

//--------------------------------------------------------------------
MP3StereoSound::~MP3StereoSound()
{
	GUARD(MP3StereoSound::~MP3StereoSound)

	Unload();

	SOUND_TRACE("delete MP3STEREO");
	UNGUARD
}

//--------------------------------------------------------------------
bool MP3StereoSound::Unload()
{
	GUARD(MP3StereoSound::Unload)

	AMP3Coded::Unload();
	AStereoCapable::Unload();

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool MP3StereoSound::Play (const char *_name, 
			   			   bool _simpleCache /*= false*/,
						   long _time/* = 0*/,
						   const CVECTOR *_startPosition/* = 0*/,
						   float _minDistance /*= -1.0f*/,
						   float _maxDistance /*= -1.0f*/,
						   long  _loopPauseTime /*= 0*/,
						   float _volume /*= 1.0f*/)
{
	GUARD(MP3StereoSound::Play)

	dword ticks;
	RDTSC_B(ticks)

	Unload();

	MMCKINFO chunkInfo;

	AMP3Coded::core->maxLoopPauseTime = _loopPauseTime;
	if (!AMP3Coded::OpenMP3File(_name, &waveFormat, &chunkInfo.cksize))
		return false;

#ifndef _XBOX
	if (!CreateBuffer(DSBCAPS_CTRLPAN |
					  DSBCAPS_CTRLVOLUME |
					  DSBCAPS_CTRLPOSITIONNOTIFY |
					  DSBCAPS_CTRLFREQUENCY | 
					  DSBCAPS_GETCURRENTPOSITION2|
					  DSBCAPS_LOCSOFTWARE,
					  chunkInfo.cksize, waveFormat))
		return false;
#else
	if (!CreateBuffer(DSBCAPS_CTRLVOLUME |
					  DSBCAPS_CTRLPOSITIONNOTIFY |
					  DSBCAPS_CTRLFREQUENCY,
					  chunkInfo.cksize, waveFormat))
		return false;
#endif

	if (!AMP3Coded::LoadDataToBuffer(dsBuffer, chunkInfo))
	{
		AStereoCapable::Unload();
		return false;
	}

	if (!_simpleCache)
	{
		
		if (!AStereoCapable::Play(true, _volume, 0/*_time*/))
			return false;

		SOUND_TRACE2("MP3_STEREO: '%s' started", _name);

		playing = true;
	}

	RDTSC_E(ticks)
	//SOUND_TRACE2("OGG: Play = %d ticks", ticks)

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void MP3StereoSound::Stop (long _time/* = 0*/)
{
	GUARD(MP3StereoSound::Stop)
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
	UNGUARD
}

//--------------------------------------------------------------------
void MP3StereoSound::Release ()
{
	GUARD(MP3StereoSound::Release)
	playing = false;
	Unload();
	UNGUARD
}

//--------------------------------------------------------------------
bool MP3StereoSound::ReleaseWithResource (tCachedData *_cachedData)
{
	return false;
}

//--------------------------------------------------------------------
bool MP3StereoSound::IsPlaying ()
{
	return playing;
}

//--------------------------------------------------------------------
float MP3StereoSound::GetPosition()
{
	return AMP3Coded::GetPosition();
}

//--------------------------------------------------------------------
void MP3StereoSound::Set3DParam (eSoundMessage _message, const void *_op)
{
}

//--------------------------------------------------------------------
bool MP3StereoSound::DoPerFrameWork(dword _dTime)
{
	GUARD(MP3StereoSound::DoPerFrameWork)

	dword ticks;
	RDTSC_B(ticks)

	TSoundCore::HandleFade();
	AMP3Coded::DoSmallDecode(dsBuffer, looped, _dTime);

	RDTSC_E(ticks)
	SOUND_TRACE2("OGG_STEREO: Position = %f", GetPosition());

	if (!looped && !IsPlaying())
	{
		//api->Trace("SOUND: Unloading non-looped OGG!");
		return false;
	}
	else
		return true;
	UNGUARD
}

//--------------------------------------------------------------------
void MP3StereoSound::SetVolume(float _volume)
{
	TSoundCore::SetVolume(_volume);
}

//--------------------------------------------------------------------
VSound *MP3StereoSound::Duplicate()
{
	//return (new MP3StereoSound(*this));
	return 0;
}

//--------------------------------------------------------------------
void MP3StereoSound::Restart()
{
	GUARD(MP3StereoSound::Restart)

	if (!dsBuffer)
		return;

	if (playing &&dsBuffer)
		dsBuffer->Stop();
	bytesPlayed = 0;
	flushFirst = true;
	_CORE_API->fio->_SetFilePointer(inFile, 0, 0, FILE_BEGIN);

	AStereoCapable::Play();
	playing = true;
	UNGUARD
}

//--------------------------------------------------------------------
void MP3StereoSound::Resume(long _time)
{
	if (_time)
		TSoundCore::SetFadeMode(FADE_IN, _time);

	if (playing || (loopPauseTime >= 0))
		return;

	if (!AStereoCapable::Play())
		return;

	playing = true;
}
