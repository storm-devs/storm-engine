#ifndef _PCMSTEREOSOUND_H_
#define _PCMSTEREOSOUND_H_

#include "..\common_h\vmodule_api.h"
#include "APCMCoded.h"
#include "AStereoCapable.h"
#include "SoundDefines.h"
#include "TSoundCore.h"
#include "VSound.h"
#include <dsound.h>

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class PCMStereoSound : public VSound, public virtual AStereoCapable, public virtual APCMCoded
{
  public:
    PCMStereoSound(LPDIRECTSOUND8 _ds8, bool _looped, bool _cached, eVolumeType _volumeType);
    PCMStereoSound(const PCMStereoSound &_source);
    virtual ~PCMStereoSound();

    virtual bool Play(const char *_name, bool _simpleCache = false, long _time = 0, const CVECTOR *_startPosition = 0,
                      float _minDistance = -1.0f, float _maxDistance = -1.0f, long _loopPauseTime = 0,
                      float _volume = 1.0f);
    virtual void Stop(long _time = 0);
    virtual void Release();
    virtual bool ReleaseWithResource(tCachedData *_cachedData);
    virtual bool IsPlaying();
    virtual float GetPosition();
    virtual void Set3DParam(eSoundMessage _message, const void *_op);
    virtual void SetVolume(float _volume);
    virtual bool DoPerFrameWork(dword _dTime);
    virtual void Restart();
    virtual VSound *Duplicate();
    virtual void Resume(long _time = 0);

  protected:
    virtual bool Unload();
    bool IsAlreadyCached(const tCachedData &_cachedData, bool _startCaching);

  private:
    int nCopy;
};

#endif // !defined
