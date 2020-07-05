#include "..\common_h\exs.h"
#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
//#include <dxerr8.h>
#include "TSoundCore.h"

float TSoundCore::globalFXVolume     = 1.f;
float TSoundCore::globalMusicVolume  = 1.f;
float TSoundCore::globalSpeechVolume = 1.f;

long  TSoundCore::allDataSize        = 0;
long  TSoundCore::allMaxDataSize     = 0;

//--------------------------------------------------------------------
TSoundCore::TSoundCore(eVolumeType _volumeType)
	:ds8(0)
	,looped(false)
	,volume (1.0f)
	,playing (false)
	,fade(FADE_DISABLED)
	,dsBuffer(0)
	,loopPauseTime(LOOP_DISABLED)
	,maxLoopPauseTime(0)
	,volumeType(_volumeType)
	,isOriginalBuffer(true)
	,onUnload(false)
{
	dataSize = 0;
}

//--------------------------------------------------------------------
TSoundCore::~TSoundCore()
{
	if (dsBuffer)
	{
		dsBuffer->Release();
		dsBuffer = 0;

	#ifndef _XBOX
		allDataSize -= dataSize;
	#else
		if (isOriginalBuffer && !cached)
			allDataSize -= dataSize;
	#endif
	}
}

//--------------------------------------------------------------------
void TSoundCore::SetVolume(float _volume, bool _transparent /*=false*/)
{
	if (!dsBuffer)
		return;

	if (!_transparent)
	{
		if (_volume < MIN_VOLUME)
			_volume = MIN_VOLUME;
		if (_volume > 1.0f)
			_volume = 1.0f;
		
		volume = _volume;
	}

	switch (volumeType)
	{
	case VOLUME_FX: 
		dsBuffer->SetVolume(VOLUME_TO_DX(_volume*globalFXVolume));
		break;
	case VOLUME_MUSIC: 
		dsBuffer->SetVolume(VOLUME_TO_DX(_volume*globalMusicVolume));
		break;
	case VOLUME_SPEECH: 
		dsBuffer->SetVolume(VOLUME_TO_DX(_volume*globalSpeechVolume));
		break;
	default:
		dsBuffer->SetVolume(VOLUME_TO_DX(1.f));
		break;
	}
}

//--------------------------------------------------------------------
void TSoundCore::UpdateVolume()
{
	SetVolume(volume);
}

//--------------------------------------------------------------------
bool TSoundCore::DuplicateBuffer(DWORD _flags, tCachedData *_cachedData)
{
	return CreateBuffer(_flags, 0, _cachedData->waveFormat, _cachedData);
}

//--------------------------------------------------------------------
bool TSoundCore::CreateBuffer(DWORD _flags, int _size, const WAVEFORMATEX &_waveFormat, tCachedData *_cachedData)
{
	GUARD(AStereoCapable::CreateBuffer)

	Assert(ds8)

	WAVEFORMATEX myWF = _waveFormat;
	HRESULT outDSCode;

#ifndef _XBOX
	long finalSize = _size;
	if (_cachedData && !_size)
		finalSize = _cachedData->size; 
	DSBUFFERDESC bufferDescriptor={sizeof(bufferDescriptor), 
								   _flags,
								   finalSize,
								   0,
								   &myWF};
	bufferDescriptor.guid3DAlgorithm = DS3DALG_DEFAULT;

#else
	XBOXADPCMWAVEFORMAT xWF;
	xWF.wfx = myWF;
	xWF.wSamplesPerBlock = 64;

	DSBUFFERDESC bufferDescriptor={sizeof(bufferDescriptor), 
								   _flags,
								   0,                                                  
								   (LPWAVEFORMATEX) &xWF,
								   0,
								   0};

#endif

	if (!_cachedData)
	{
		bufferDescriptor.dwBufferBytes = _size;
		attachedToSoundBuffer = 0;
	}
	else
	{
		attachedToSoundBuffer = _cachedData->pcmData;
	}

	outDSCode = ds8->CreateSoundBuffer(&bufferDescriptor, &dsBuffer, 0);
	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("Can`t create sound buffer");
		return false;
	}
	
	if (_cachedData)
	{
#ifndef _XBOX
		void *buf1, *buf2;
		dword buf1Size, buf2Size;
		dsBuffer->Lock(0, finalSize, &buf1, &buf1Size, &buf2, &buf2Size, DSBLOCK_ENTIREBUFFER);
		Assert(buf1Size == _cachedData->size)
		memcpy(buf1, _cachedData->pcmData, _cachedData->size);
		dsBuffer->Unlock(buf1, buf1Size, buf2, buf2Size);
#else
		dsBuffer->SetBufferData(_cachedData->pcmData, _cachedData->size);
		_cachedData->flags = _flags;
#endif
	}

	//statistics
	dataSize += bufferDescriptor.dwBufferBytes;
	allDataSize += dataSize;
	if (allDataSize > allMaxDataSize)
		allMaxDataSize = allDataSize;

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void TSoundCore::HandleFade()
{
	if (fade == FADE_DISABLED)
		return; //quick out

	if (!dsBuffer)
		return;

	DWORD nowTime = GetTickCount();
	if (nowTime > (startFadeTime + fadeTime))
	{
		if (fade == FADE_OUT)
		{
			dsBuffer->Stop();
			playing = false;
			//SetVolume(0.f);
		}
		else 
		if (fade == FADE_IN)
		{
			SetVolume(volume);
		}

		SetFadeMode(FADE_DISABLED);
	}

	switch (fade)
	{
	case FADE_IN:
		SetVolume((nowTime - startFadeTime) * fadeK, true);
		break;
	case FADE_OUT:
		SetVolume(volume - (nowTime - startFadeTime) * fadeK, true);
		break;
	}
}

//--------------------------------------------------------------------
void TSoundCore::SetFadeMode(eFadeType _fade, DWORD _time/* = 0*/)
{
	if (!dsBuffer)
		return;

	startFadeTime = GetTickCount();
	fadeTime = _time;
	if (_time)
		fadeK = volume / _time;
	else
		fadeK = 1.0f;
	switch (_fade)
	{
	case FADE_IN:
		SetVolume(0.0f, true);
		fade = FADE_IN;
		break;
	case FADE_OUT:
		SetVolume(volume, true);
		fade = FADE_OUT;
		break;
	default:
	case FADE_DISABLED: // switch fading off
		SetVolume(volume); 
		fade = FADE_DISABLED;
		break;
	}
}

//--------------------------------------------------------------------
/*
//const TSoundCore &TSoundCore::operator= (const TSoundCore &_source)
TSoundCore::TSoundCore (const TSoundCore &_source)
{
	ds8 = _source.ds8;
	waveFormat = _source.waveFormat;
	looped = _source.looped;
	cached = _source.cached;
	volume = 1.0f;
	playing = false;
	fade = FADE_DISABLED;
	dsBuffer = 0;
#ifndef _XBOX
	if (_source.ds8 && _source.dsBuffer)
		ds8->DuplicateSoundBuffer(_source.dsBuffer, &dsBuffer);
#else
	if (_source.ds8 && _source.dsBuffer)
	{
		dsBuffer = _source.dsBuffer;
		dsBuffer->AddRef();
	}
#endif

	//return *this;
}
*/
//--------------------------------------------------------------------
void TSoundCore::GetStatistics(long *_dataSize, long *_maxDataSize)
{
	*_dataSize    = allDataSize;
	*_maxDataSize = allMaxDataSize;
}

//--------------------------------------------------------------------
void TSoundCore::SetGlobalVolume(float _fxVolume, float _musicVolume, float _speechVolume)
{
	if (_fxVolume < 0.f)
	{
		_fxVolume = 0.f;
		api->Trace("SoundService: FX volume is too low! (setting to minimum)");
	}
	if (_fxVolume > 1.f)
	{
		_fxVolume = 1.f;
		api->Trace("SoundService: FX volume is too high! (setting to maximum)");
	}
	if (_musicVolume < 0.f)
	{
		_musicVolume = 0.f;
		api->Trace("SoundService: Music volume is too low! (setting to minimum)");
	}
	if (_musicVolume > 1.f)
	{
		_musicVolume = 1.f;
		api->Trace("SoundService: Music volume is too high! (setting to maximum)");
	}
	if (_speechVolume < 0.f)
	{
		_speechVolume = 0.f;
		api->Trace("SoundService: Speech volume is too low! (setting to minimum)");
	}
	if (_speechVolume > 1.f)
	{
		_speechVolume = 1.f;
		api->Trace("SoundService: Speech volume is too high! (setting to maximum)");
	}

	globalFXVolume = _fxVolume;
	globalMusicVolume = _musicVolume;
	globalSpeechVolume = _speechVolume;
}

//--------------------------------------------------------------------
void TSoundCore::GetGlobalVolume(float *_fxVolume, float *_musicVolume, float *_speechVolume)
{
	*_fxVolume = globalFXVolume;
	*_musicVolume = globalMusicVolume;
	*_speechVolume = globalSpeechVolume;
}

//--------------------------------------------------------------------
float TSoundCore::GetVolume (bool _absolute/*= false*/)
{
	if (!_absolute)
	{
		return volume;
	}
	else
	{
		switch (volumeType)
		{
			case VOLUME_FX: 
				return volume*globalFXVolume;
			case VOLUME_MUSIC: 
				return volume*globalMusicVolume;
			case VOLUME_SPEECH: 
				return volume*globalSpeechVolume;
			default:
				return volume;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

