#ifndef _SOUNDSERVICE_H_
#define _SOUNDSERVICE_H_

//#include <mmreg.h>
//#include <dxerr8.h>
#include <dsound.h>
#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
#include "VSoundService.h"
#include "VSound.h"
#include "..\common_h\CVECTOR.h"
#include ".\FMOD4\api\inc\fmod.hpp"
#include ".\FMOD4\api\inc\fmod_errors.h"
#include "..\common_h\templates.h"



#define MAX_SOUNDS_SLOTS 512


//debug....
class SoundVisualisationEntity;

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class SoundService: public VSoundService
{
	bool bShowDebugInfo;
	FMOD_RESULT status;

	bool bInited;

	FMOD::System* fmod_system;

	FMOD::Sound* OGG_sound[2];

	struct tSoundCache
	{
		DWORD dwNameHash;
		string Name;
		FMOD::Sound* sound;
		float fTimeFromLastPlay;
		eSoundType type;

		tSoundCache()
		{
			dwNameHash = 0;
			sound = NULL;
			fTimeFromLastPlay = 0.0f;
		}
	};

	struct tPlayedSound
	{
		float fFaderNeedVolume;
		float fFaderCurrentVolume;
		float fFaderDeltaInSec;


		bool bFree;
		FMOD::Channel* channel;
		eVolumeType type;
		eSoundType sound_type;
		float fSoundVolume;

		//temp
		string Name;
		

		tPlayedSound()
		{
			bFree = true;
			channel = NULL;
			type = VOLUME_FX;

			fFaderNeedVolume = 0;
			fFaderCurrentVolume = 0;
			fFaderDeltaInSec = 0;

		}

	};

	tPlayedSound PlayingSounds[MAX_SOUNDS_SLOTS];



	struct PlayedOGG
	{
		string Name;
		dword dwHash;
		unsigned int position;
	};

	array<PlayedOGG> OGGPosition;


	unsigned int GetOGGPosition (const char* szName);
	void SetOGGPosition (const char* szName, unsigned int pos);
	int GetOGGPositionIndex (const char* szName);


	array<tSoundCache> SoundCache;


	int GetFromCache (const char* szName, eSoundType _type);


	bool FaderParity;




	FMOD_VECTOR vListenerPos;
	FMOD_VECTOR vListenerForward;
	FMOD_VECTOR vListenerTop;


	void CreateEntityIfNeed ();
// Aliases ------------------------------------------------------------

	struct tAliasSound
	{
		string FileName;
		float fProbability;
	};

	struct tAlias
	{
		string Name;
		dword dwNameHash;

		float fMaxProbabilityValue;
		float fMinDistance;
		float fMaxDistance;
		long iPrior;
		float fVolume;
		array<tAliasSound> SoundFiles;

		tAlias() : SoundFiles (_FL_)
		{
		}
	};

	array<tAlias> Aliases;

	const char* GetRandomName (tAlias *_alias);
	int GetAliasIndexByName (const char *szAliasName);
	void AnalyseNameStringAndAddToAlias(tAlias *_alias, const char *in_string);
	void AddAlias(INIFILE *_iniFile, char *_sectionName);
	void LoadAliasFile (const char *_filename);
	void InitAliases ();

// Sound Schemes------------------------------------------------------------
	struct tSoundSchemeChannel
	{
		TSD_ID SoundID;
		string soundName;
		long minDelayTime;
		long maxDelayTime;
		float volume;
		long timeToNextPlay;
		bool looped; //not working

		tSoundSchemeChannel()
		{
			SoundID = 0;
		}
	};

	array<tSoundSchemeChannel> SoundSchemeChannels;

	bool AddSoundSchemeChannel (char *in_string, bool _looped = false);
	void ProcessSoundSchemes ();

//----------------------------------------------------------------------------

	int FindEmptySlot ();



	float fFXVolume;
	float fMusicVolume;
	float fSpeechVolume;


public:

	SoundService();
	virtual ~SoundService();
	bool __declspec(dllexport) __cdecl SFLB_SetScheme(const char *_schemeName);
	virtual bool Init();
	virtual dword RunSection()
	{
		return SECTION_EXECUTE;
	}

	virtual void  RunStart();
	virtual void  RunEnd();

	virtual TSD_ID SoundPlay		(const char *_name,
									 eSoundType _type,
									 eVolumeType _volumeType,
									 bool _simpleCache = false,
									 bool _looped = false,
									 bool _cached = false,
									 long _time = 0,
									 CVECTOR *_startPosition = 0,
									 float _minDistance = -1.0f,
									 float _maxDistance = -1.0f,
									 long  _loopPauseTime = 0,
			 					   float _volume = 1.0f,
									 long _prior = 128);

	virtual TSD_ID SoundDuplicate   (TSD_ID _sourceID);
	virtual void   SoundSet3DParam  (TSD_ID _id, eSoundMessage _message, const void *_op);
	virtual void   SoundStop	    (TSD_ID _id, long _time = 0);
	virtual void   SoundRelease	    (TSD_ID _id);
	virtual void   SoundSetVolume   (TSD_ID _id, float _volume);
	virtual bool   SoundIsPlaying   (TSD_ID _id);
	virtual float  SoundGetPosition (TSD_ID _id);
	virtual void   SoundRestart     (TSD_ID _id);
	virtual void   SoundResume      (TSD_ID _id, long _time = 0);

	// Service functions
	virtual void  SetMasterVolume      (float _fxVolume, float _musicVolume, float _speechVolume);
	virtual void  GetMasterVolume      (float *_fxVolume, float *_musicVolume, float *_speechVolume);
	virtual void  SetCameraPosition    (const CVECTOR &_cameraPosition);
	virtual void  SetCameraOrientation (const CVECTOR &_nose, const CVECTOR &_head);

	//schemes` routines
	virtual void ResetScheme();
	virtual bool SetScheme (const char *_schemeName);
	virtual bool AddScheme (const char *_schemeName);
	virtual void SetEnabled (bool _enabled);



	void DebugDraw ();

	void _cdecl DebugPrint3D(const CVECTOR & pos3D, float rad, long line, float alpha, dword color, float scale, const char * format, ...);
	void Draw2DCircle (const CVECTOR& center, DWORD dwColor, float fRadius, DWORD dwColor2, float fRadius2);


	void ProcessFader (int idx);


};


//API_SERVICE_START("sound service")
//	DECLARE_MAIN_SERVICE(SoundService)	
//API_SERVICE_END(SoundService)

#endif // !defined

