#include "..\common_h\exs.h"
//#include <dxerr8.h>
#include "MP33DSound.h"

//--------------------------------------------------------------------
MP33DSound::MP33DSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType)
	:VSound(_volumeType)
{
	GUARD(SoundMP3::SoundMP3)
	Assert(_ds8)

	AMP3Coded::core = this;
	A3DCapable::core = this;

	TSoundCore::cached = _cached;
	TSoundCore::looped = _looped;
	TSoundCore::ds8    = _ds8;

	UNGUARD
}

//--------------------------------------------------------------------
MP33DSound::~MP33DSound()
{
	GUARD(SoundMP3::~SoundMP3)

	Unload();

	SOUND_TRACE("delete MP33D");
	UNGUARD
}

//--------------------------------------------------------------------
bool MP33DSound::Unload()
{
	GUARD(SoundMP3::Unload)

	A3DCapable::Unload();

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool MP33DSound::Play (const char *_name, 
		   			   bool _simpleCache /*= false*/,
					   long _time/* = 0*/,
					   const CVECTOR *_startPosition/* = 0*/,
					   float _minDistance /*= -1.0f*/,
					   float _maxDistance /*= -1.0f*/,
					   long  _loopPauseTime /*= 0*/,
					   float _volume /*= 1.0f*/)
{
	GUARD(MP33DSound::Play)
	Unload();

	MMCKINFO chunkInfo;

	if (!AMP3Coded::OpenMP3File(_name, &waveFormat, &chunkInfo.cksize))
		return false;

#ifndef _XBOX
	if (!CreateBuffer(DSBCAPS_CTRL3D |
					  DSBCAPS_CTRLVOLUME |
   					  DSBCAPS_CTRLPOSITIONNOTIFY |
					  DSBCAPS_GETCURRENTPOSITION2 |
					  DSBCAPS_MUTE3DATMAXDISTANCE,
					  chunkInfo.cksize, waveFormat))
		return false;
#else
	if (!CreateBuffer(DSBCAPS_CTRL3D |
					  DSBCAPS_CTRLVOLUME |
   					  DSBCAPS_CTRLPOSITIONNOTIFY,
					  chunkInfo.cksize, waveFormat))
		return false;
#endif

	if (!AMP3Coded::LoadDataToBuffer(dsBuffer, chunkInfo))
	{
		A3DCapable::Unload();
		return false;
	}

	A3DCapable::Init();
	if (!_simpleCache)
	{
		if (_time)
			TSoundCore::SetFadeMode(FADE_IN, _time);
		if (!A3DCapable::Play(true, _startPosition, _minDistance, _maxDistance, _volume))
			return false;

		SOUND_TRACE2("MP3_3D: '%s' started", _name);

		playing = true;
	}
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void MP33DSound::Stop (long _time/* = 0*/)
{
	GUARD(MP33DSound::Stop)
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
void MP33DSound::Release ()
{
	GUARD(MP33DSound::Release)

	playing = false;
	Unload();

	UNGUARD
}

//--------------------------------------------------------------------
bool MP33DSound::ReleaseWithResource (tCachedData *_cachedData)
{
	return false;
}

//--------------------------------------------------------------------
bool MP33DSound::IsPlaying ()
{
	return playing;
}

//--------------------------------------------------------------------
float MP33DSound::GetPosition()
{
	return AMP3Coded::GetPosition();
}

//--------------------------------------------------------------------
void MP33DSound::Set3DParam (eSoundMessage _message, const void *_op)
{
	A3DCapable::Set3DParameter(_message, _op);
}

//--------------------------------------------------------------------
bool MP33DSound::DoPerFrameWork(dword _dTime)
{
	GUARD(MP33DSound::DoPerFrameWork)

	TSoundCore::HandleFade();
	AMP3Coded::DoSmallDecode(dsBuffer, looped, _dTime);

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void MP33DSound::SetVolume(float _volume)
{
	TSoundCore::SetVolume(_volume);
}

//--------------------------------------------------------------------
VSound *MP33DSound::Duplicate()
{
	//return (new MP33DSound(*this));
	return 0;
}

//--------------------------------------------------------------------
void MP33DSound::Restart()
{
	GUARD(MP33DSound::Restart)
	if (!dsBuffer)
		return;

	if (playing && dsBuffer)
		dsBuffer->Stop();
	bytesPlayed = 0;
	flushFirst = true;
	_CORE_API->fio->_SetFilePointer(inFile, 0, 0, FILE_BEGIN);

	if (!A3DCapable::Play(/*TSoundCore::looped*/true))
		return;
	playing = true; 
	UNGUARD
}

//--------------------------------------------------------------------
void MP33DSound::Resume(long _time)
{
	if (playing)
		return;

	if (_time)
		TSoundCore::SetFadeMode(FADE_IN, _time);
	if (!A3DCapable::Play(/*TSoundCore::looped*/true))
		return;

	playing = true;
}
