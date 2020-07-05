#ifndef _TSOUNDCORE_H_
#define _TSOUNDCORE_H_

#ifndef _XBOX
	#include <windows.h>
#else
	#include <xtl.h>
#endif

#include <dsound.h>
#include "SoundDefines.h"

#define VOLUME_TO_DX(v) lg[(long) ((v) * 99.0f)] // lg[0]..lg[99]
typedef enum {FADE_DISABLED, FADE_IN, FADE_OUT} eFadeType;

struct tCachedData
{
	dword lastUsedTime;
	bool uncacheable;
	unsigned long size;
	WAVEFORMATEX waveFormat;
	void *pcmData;
	DWORD flags;
	long usageCounter;
	long cacheIndex;
	tCachedData() :uncacheable(true), size(0), pcmData(0), lastUsedTime(-1), usageCounter(0) {}
};

class TSoundCore  
{
public:
	TSoundCore(eVolumeType _volumeType);
	//const TSoundCore &operator=(const TSoundCore &_source);
	//TSoundCore (const TSoundCore &_source);
	~TSoundCore();
	void SetVolume (float _volume, bool _transparent = false);
	float GetVolume (bool _absolute = false);
	void UpdateVolume();
	void SetFadeMode(eFadeType _fade, DWORD _time = 0);

	static void SetGlobalVolume(float _fxVolume, float _musicVolume, float _speechVolume);
	static void GetGlobalVolume(float *_fxVolume, float *_musicVolume, float *_speechVolume);
	static void GetStatistics(long *_dataSize, long *_maxDataSize);

	LPDIRECTSOUND8      ds8;
	LPDIRECTSOUNDBUFFER dsBuffer;
	WAVEFORMATEX        waveFormat;
	bool  looped;
	bool  playing;
	float volume;
	eFadeType fade;
	DWORD startFadeTime;
	DWORD fadeTime;
	float fadeK;
	bool  cached;
	long  loopPauseTime;
	long  maxLoopPauseTime;
	eVolumeType volumeType;
	bool  isOriginalBuffer;
	bool  onUnload;
	void  *attachedToSoundBuffer;

protected:		
	void HandleFade();
	bool CreateBuffer(DWORD _flags, int _size, const WAVEFORMATEX &_waveFormat, tCachedData *_cachedData=0);
	bool DuplicateBuffer(DWORD _flags, tCachedData *_cachedData);

private:
	float static globalFXVolume;
	float static globalMusicVolume;
	float static globalSpeechVolume;

	long static allDataSize;
	long static allMaxDataSize;
	long dataSize;
};

#endif // !defined
