///////////////////////////////////////////////////////////////////
// PCM wave sound class. 
// + immediate load data into memory
// + 3D control
// + low-level reading from .wav
///////////////////////////////////////////////////////////////////

#ifndef _PCM3DSOUND_H_
#define _PCM3DSOUND_H_

#include <dsound.h>
#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
#include "VSound.h"
#include "A3DCapable.h"
#include "APCMCoded.h"
#include "TSoundCore.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class PCM3DSound : public VSound,
				   public virtual A3DCapable,
		 		   public virtual APCMCoded
{
public:
	PCM3DSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType);
	PCM3DSound (const PCM3DSound &_source);
	virtual ~PCM3DSound();

	virtual bool Play (const char *_name, 
					   bool _simpleCache = false,
					   long _time = 0,
					   const CVECTOR *_startPosition = 0,
					   float _minDistance = -1.0f,
					   float _maxDistance = -1.0f,
					   long  _loopPauseTime = 0,
					   float _volume = 1.0f );
	virtual void Stop (long _time = 0);
	virtual void Release ();
	virtual bool ReleaseWithResource (tCachedData *_cachedData);
	virtual bool IsPlaying ();
	virtual float GetPosition();
	virtual void Set3DParam (eSoundMessage _message, const void *_op);
	virtual void SetVolume(float _volume);
	virtual bool DoPerFrameWork(dword _dTime);
	virtual void Restart();
	virtual VSound *Duplicate ();
	virtual void Resume (long _time = 0);

protected:
	virtual bool Unload();
	bool IsAlreadyCached(const tCachedData &_cachedData, bool _startCaching);

private:
	int nCopy;
};

#endif // !defined
