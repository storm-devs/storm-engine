#ifndef __VSOUND_H__
#define __VSOUND_H__

#include "SoundDefines.h"

#include "..\common_h\CVECTOR.h"
///////////////////////////////////////////////////////////////////
// INTERFACE DEFINITION
///////////////////////////////////////////////////////////////////
class VSound
{

public:
	VSound(eVolumeType _volumeType)	{}
	virtual ~VSound() {}
	
	virtual bool Play (const char *_name,
					   bool _simpleCache = false,
					   long _time = 0,
					   const CVECTOR *_startPosition = 0,
					   float _minDistance = -1.0f,
					   float _maxDistance = -1.0f,
					   long  _loopPauseTime = 0,
					   float _volume = 1.0f ) = 0;
	virtual void Stop (long _time = 0) = 0;
	virtual void Release () = 0;
	virtual bool IsPlaying () = 0;
	virtual float GetPosition() = 0;
	virtual void SetVolume(float _volume) = 0;
	virtual void Set3DParam (eSoundMessage _message, const void *_op) = 0;
	virtual bool DoPerFrameWork(dword _dTime) = 0; //return false forces service to delete sound
	virtual void Restart() = 0;
	virtual VSound *Duplicate () = 0;
	virtual void Resume (long _time = 0) = 0;

};

#endif 
