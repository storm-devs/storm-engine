#ifndef _SOUNDDEFINES_H_
#define _SOUNDDEFINES_H_

#include "..\COMMON_H\d_types.h"
#include "..\COMMON_H\memop.h"
///////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////
//#define TRACE_INFORMATION
#ifndef _XBOX
	#define FIO(a) api->fio->_##a
#else
	#define FIO(a) a  
#endif

#ifndef _XBOX
	#define MAX_CACHED_DATA      (4*1024*1024)
#else
	#define MAX_CACHED_DATA      (2*1024*1024)
#endif

#define SOUND_INVALID_ID	 0
#define MAX_SOUND_DUPLICATES 50//8
#define COMMON_STRING_LENGTH 256
#define MAX_SOUND_DRIVERS    5
#define ERROR_POSITION       -1.0f
#define MIN_VOLUME			 0.01f

extern long lg[100];

#define MAX_SOUNDS			150
#define MAX_QUEUE_SOUNDS	30
#define MAX_DISTANCE		500.0f //distance beyond which 3D-sound isn`t created
#define MAX_NAMES_PER_ALIAS 50	   //random-chosen names within alias
#define MAX_LOOP_PAUSE_TIME 0

#define LOOP_DISABLED		-54321

#define ALIAS_DIRECTORY			"resource\\ini\\aliases\\"
#define DEFAULT_SOUND_DIRECTORY "resource\\sounds\\"
#define SOUND_SECTION_NAME      "sound"
#define SOUND_PATH_NAME		    "sound path"
#define ALIAS_INI			    "resource\\ini\\sound_alias.ini"
#define SCHEME_INI_NAME  "resource\\ini\\sound_scheme.ini"
#define SCHEME_KEY_NAME  "ch"
#define SCHEME_KEY_NAME_LOOP  "ch_loop"

#define DEFAULT_PROBABILITY 1.f
#define SCHEME_MAX_CHANNELS 100
#define SCHEME_MIN_DELAY    0
#define SCHEME_MAX_DELAY    0x7FFFFFFF

enum eSoundType {PCM_3D     = 1, 
				 MP3_STEREO = 2, 
				 MP3_3D     = 3,
				 PCM_STEREO = 4};
enum eVolumeType {VOLUME_FX     = 1,
	              VOLUME_MUSIC  = 2,
	              VOLUME_SPEECH = 3};
enum eSoundMessage
{
	SM_MAX_DISTANCE = 1,
	SM_MIN_DISTANCE = 2,
	SM_POSITION     = 3,
	/*
	SM_CONE_ANGELS,
	SM_CONE_OUTSIDE_VOLUME,
	SM_VELOCITY
	*/
};

#ifdef _XBOX
	typedef unsigned long FOURCC;
	#define D3DVALUE float

	typedef struct { 
		FOURCC ckid; 
		dword  cksize; 
		FOURCC fccType; 
		dword  dwDataOffset; 
		dword  dwFlags; 
	} MMCKINFO;
#endif
///////////////////////////////////////////////////////////////////
// DEFINES
///////////////////////////////////////////////////////////////////
#ifndef TRACE_INFORMATION
	#define SOUND_TRACE(a)    true
	#define SOUND_TRACE2(a,b) true
#else
	#define SOUND_TRACE(a)    _VSYSTEM_API->Trace(a);
	#define SOUND_TRACE2(a,b) _VSYSTEM_API->Trace(a,b);
#endif

#define THROW_STRING(s, p) \
{ \
	static char _tmpString[2048]; \
 \
	sprintf(_tmpString, s, p); \
	_VSYSTEM_API->Trace(_tmpString); \
	throw _tmpString; \
}

class SoundService;
extern SoundService *soundService;

#endif