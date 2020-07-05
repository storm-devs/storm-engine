#ifndef _SOUND_H_
#define _SOUND_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\object.h"
#include "..\common_h\messages.h"
#include "..\common_h\entity.h"
#include "..\common_h\dx8render.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
// SoundPlay (..., type, ...)
#define SOUND_WAV_3D     1
#define SOUND_MP3_STEREO 2
#define SOUND_MP3_3D     3
#define SOUND_WAV_STEREO 4
//SoundSet3DParam(..., type, ...)
#define SOUND_PARAM_MAX_DISTANCE    1
#define SOUND_PARAM_MIN_DISTANCE    2
#define SOUND_PARAM_POSITION        3
#define SOUND_STRING_SIZE         512

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class SOUND: public ENTITY
{
public:
	SOUND();
	virtual ~SOUND();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);

private:
	VSoundService *soundService;
	VDX8RENDER	  *renderer;
};

//API_MODULE_START("Sound")
//	CREATE_CLASS(0, SOUND)
//API_MODULE_END

#endif
