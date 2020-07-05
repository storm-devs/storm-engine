#include <math.h>
#include "..\common_h\exs.h"
#include "..\common_h\matrix.h"
#include "..\common_h\defines.h"
#include "..\common_h\rands.h"
#include "SoundService.h"


//для debug
#include "..\common_h\dx8render.h"
#include "DebugEntity.h"
VDX8RENDER* pRS = NULL;

INTERFACE_FUNCTION
CREATE_SERVICE(SoundService)	
CREATE_CLASS(SoundVisualisationEntity)


#define DISTANCEFACTOR 1.0f

//#define FADE_TIME 4.0f



void FMOD_ERROR(const char* szFromWhere, FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		char* szTxtError = FMOD_ErrorString(result);
		api->Trace("%s error! (%d) %s", szFromWhere, result, szTxtError);
	}
}


//////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION
//////////////////////////////////////////////////////////////////////
SoundService::SoundService() : Aliases (_FL_),
															 SoundCache(_FL_),
															 SoundSchemeChannels(_FL_),
															 OGGPosition(_FL_)
{
	fFXVolume = 0.5f;
	fMusicVolume = 0.5f;
	fSpeechVolume = 0.5f;


	bShowDebugInfo = false;
	OGG_sound[0] = NULL;
	OGG_sound[1] = NULL;
	fmod_system = NULL;
	bInited = false;

	vListenerPos.x = 0.0f;
	vListenerPos.y = 0.0f;
	vListenerPos.z = 0.0f;
	
	vListenerForward.x = 0.0f;
	vListenerForward.y = 0.0f;
	vListenerForward.z = 0.0f;
	
	vListenerTop.x = 0.0f;
	vListenerTop.y = 0.0f;
	vListenerTop.z = 0.0f;

	FaderParity = false;

}

//--------------------------------------------------------------------
SoundService::~SoundService()
{

	if (bInited)
	{
		status = fmod_system->close();
		FMOD_ERROR("FMOD:Close", status);
		status = fmod_system->release();
		FMOD_ERROR("FMOD:Release", status);
	}
	bInited = false;
}


//--------------------------------------------------------------------
//static TSD_ID i5 = 0;
bool SoundService::Init()
{
	pRS = (VDX8RENDER *) _CORE_API->CreateService("DX8RENDER");

	status = FMOD::System_Create(&fmod_system);
	FMOD_ERROR("FMOD:System_Create", status);

	unsigned int version;
	status = fmod_system->getVersion(&version);
	FMOD_ERROR("FMOD:getVersion", status);
	

	bInited = false;
	if (version < FMOD_VERSION)
	{
		api->Trace("Error : You are using old FMOD version %08x !\n", version);
		return false;
	} else
	{
		api->Trace("Using FMOD %08x", FMOD_VERSION);
	}

	//32 звука мы слышим
	status = fmod_system->setSoftwareChannels(32);
	FMOD_ERROR("FMOD:setSoftwareChannels", status);


	//fmod_system->setDriver()

	//status = fmod_system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
	//FMOD_ERROR("FMOD:setSpeakerMode", status);

	//всего MAX_SOUNDS_SLOTS можно звуков завести

	INIFILE * pEngineIni = fio->OpenIniFile(api->EngineIniFileName());
	if (pEngineIni)
	{
		bool bUseMMDevice = pEngineIni->GetLong("Sound", "UseMM", 0) == 1;
		if (bUseMMDevice)
	{
	status = fmod_system->setOutput(FMOD_OUTPUTTYPE_WINMM);
	FMOD_ERROR("FMOD:setOutput", status);
	}
		DELETE(pEngineIni);
	}

	status = fmod_system->init(MAX_SOUNDS_SLOTS, FMOD_INIT_NORMAL, api->GetAppHWND());
	FMOD_ERROR("FMOD:init", status);
	status = fmod_system->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
	FMOD_ERROR("FMOD:set3DSettings", status);

	bInited = true;
	api->Trace("Sound inited ok !!!");


	FMOD_SPEAKERMODE speaker_mode;
	status = fmod_system->getSpeakerMode(&speaker_mode);
	FMOD_ERROR("FMOD:getSpeakerMode", status);

	string TextSpkMode = "RAW";
	switch (speaker_mode)
	{
	case FMOD_SPEAKERMODE_MONO:
		TextSpkMode = "MONO";
		break;
	case FMOD_SPEAKERMODE_STEREO:
		TextSpkMode = "STEREO";
		break;
	case FMOD_SPEAKERMODE_5POINT1:
		TextSpkMode = "5:1";
		break;
	case FMOD_SPEAKERMODE_7POINT1:
		TextSpkMode = "7:1";
		break;
	case FMOD_SPEAKERMODE_PROLOGIC:
		TextSpkMode = "PROLOGIC";
		break;

	}
	api->Trace("FMOD: Speaker mode %s", TextSpkMode.c_str());

	InitAliases();


	//FMOD_REVERB_PROPERTIES revProp = FMOD_PRESET_CONCERTHALL;
	//status = fmod_system->setReverbProperties(&revProp);
	//FMOD_ERROR("FMOD:setReverbProperties",status);


	CreateEntityIfNeed();

	return true;
}

//--------------------------------------------------------------------
void  SoundService::RunEnd()
{
	CreateEntityIfNeed();
	//Внутреннее обновление FMOD
	status = fmod_system->update();
	FMOD_ERROR("FMOD:update",status);
}


//--------------------------------------------------------------------
void SoundService::ProcessFader (int idx)
{
	if (PlayingSounds[idx].bFree) return;
	if (fabsf (PlayingSounds[idx].fFaderCurrentVolume - PlayingSounds[idx].fFaderNeedVolume) < 0.001f)
	{
		if (PlayingSounds[idx].fFaderDeltaInSec < 0)
		{
			unsigned int OGGpos;
			PlayingSounds[idx].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
			SetOGGPosition(PlayingSounds[idx].Name.c_str(), OGGpos);

			PlayingSounds[idx].channel->stop();
		}

		return;
	}

	float fTime = api->GetDeltaTime() * 0.001f;

	PlayingSounds[idx].fFaderCurrentVolume += PlayingSounds[idx].fFaderDeltaInSec * fTime;

	//FADE IN
	if (PlayingSounds[idx].fFaderDeltaInSec > 0)
	{
		if (PlayingSounds[idx].fFaderCurrentVolume > PlayingSounds[idx].fFaderNeedVolume)
		{
			PlayingSounds[idx].fFaderCurrentVolume = PlayingSounds[idx].fFaderNeedVolume;
		}
	}

	// FADE OUT
	if (PlayingSounds[idx].fFaderDeltaInSec < 0)
	{
		if (PlayingSounds[idx].fFaderCurrentVolume < PlayingSounds[idx].fFaderNeedVolume)
		{
			PlayingSounds[idx].fFaderCurrentVolume = PlayingSounds[idx].fFaderNeedVolume;


			unsigned int OGGpos;
			PlayingSounds[idx].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
			SetOGGPosition(PlayingSounds[idx].Name.c_str(), OGGpos);

			PlayingSounds[idx].channel->stop();
		}
	}


	PlayingSounds[idx].channel->setVolume(PlayingSounds[idx].fFaderCurrentVolume);

}

void  SoundService::RunStart()
{
	CreateEntityIfNeed();

	if (pRS)
	{
		static CVECTOR pos, ang, nose, head;
		static CMatrix view;

		pRS->GetTransform(D3DTS_VIEW, view);
		view.Transposition();
		nose = view.Vz();
		head = view.Vy();
		pos = view.Pos();

		SetCameraPosition(pos);
		SetCameraOrientation(nose, head);
	}


	ProcessFader(0);
	ProcessFader(1);



	
	//Освобождаем звуки которые проигрались...
	for (int i = 0; i < MAX_SOUNDS_SLOTS; i++)
	{
		if (PlayingSounds[i].bFree) continue;

		//Если просто на паузе стоит, трогать его не надо...

		bool paused;
		status = PlayingSounds[i].channel->getPaused(&paused);
		if (status != FMOD_OK)
		{
			PlayingSounds[i].bFree = true;
			continue;
		}

		//FMOD_ERROR(status);

		if (paused) continue;


		bool is_playing;
		status = PlayingSounds[i].channel->isPlaying(&is_playing);
		FMOD_ERROR("FMOD_SOUND:isplaying", status);

		if (!is_playing || status != FMOD_OK)
		{
			PlayingSounds[i].bFree = true;
			PlayingSounds[i].channel = NULL;

			if (i == 0 && OGG_sound[0] != NULL)
			{
				status = OGG_sound[0]->release();
				FMOD_ERROR("FMOD_SOUND:release", status);
				OGG_sound[0] = NULL;
			}
			if (i == 1 && OGG_sound[1] != NULL)
			{
				status = OGG_sound[1]->release();
				FMOD_ERROR("FMOD_SOUND:release", status);
				OGG_sound[1] = NULL;
			}
			
		}
	}

	ProcessSoundSchemes();


}

int SoundService::FindEmptySlot ()
{
	for (int i = 2; i < MAX_SOUNDS_SLOTS; i++)
	{
		if (PlayingSounds[i].bFree) return i;
	}

	return -1;
}



//--------------------------------------------------------------------
const char * SoundService::GetRandomName (tAlias *_alias)
{
	float randomFloat = rand(_alias->fMaxProbabilityValue);
	int currentNameIndex = 0;
	float currentFloat = 0.f;

	for (;;)
	{
		if (currentFloat > randomFloat) break;
		if ((currentNameIndex+1) >= (int)_alias->SoundFiles.Size()) break;

		currentFloat += _alias->SoundFiles[currentNameIndex].fProbability;
		currentNameIndex++;
	}

	if (currentNameIndex >= (int)_alias->SoundFiles.Size())
	{
		currentNameIndex = (int)_alias->SoundFiles.Size() - 1;
	}

	return _alias->SoundFiles[currentNameIndex].FileName.c_str();
		//->fileNames + COMMON_STRING_LENGTH*(currentNameIndex - 1));
}


int SoundService::GetAliasIndexByName (const char *szAliasName)
{
	dword dwSearchHash = string::HashNoCase(szAliasName);
	for (dword i = 0; i < Aliases.Size(); i++)
	{
		if (Aliases[i].dwNameHash == dwSearchHash)
		{
			if (Aliases[i].Name == szAliasName)
			{
				return i;
			}
		}
	}

	return -1;
}

TSD_ID  SoundService::SoundPlay (const char *_name,
								eSoundType _type,
								eVolumeType _volumeType,
								bool _simpleCache  /*=false*/,
								bool _looped	   /*= false*/,
								bool _cached	   /*= false*/,
								long _time		   /* = 0*/,
								CVECTOR *_startPosition/* = 0*/,
								float _minDistance /* = -1.0f*/,
								float _maxDistance /* = -1.0f*/,
								long  _loopPauseTime /*= 0*/,
		 					  float _volume, /*= 1.0f*/
								long _prior)
{
	string FileName = _name;

	// aliases don`t contain `\`
	if (!strchr(_name, '\\')) 
	{
		//Пробуем найти в алиасах
		int AliasIdx = GetAliasIndexByName (_name);
		if (AliasIdx >= 0 && Aliases[AliasIdx].SoundFiles.Size() > 0)
		{
			//Играем из алиаса звук...
			FileName = GetRandomName(&Aliases[AliasIdx]);
			//api->Trace("Play sound from alias %s", FileName.c_str());

			_minDistance = Aliases[AliasIdx].fMinDistance;
			_maxDistance = Aliases[AliasIdx].fMaxDistance;
			_prior = Aliases[AliasIdx].iPrior;


			if (Aliases[AliasIdx].fVolume > 0.0f)
			{
				_volume = Aliases[AliasIdx].fVolume;
			}

		}
	}

	string SoundName = "resource\\sounds\\";
	SoundName += FileName;


	FMOD::Sound* sound = NULL;
	int SoundIdx = 0;
	if (_type == MP3_STEREO)
	{
		//Стримленые играем сразу, без кеширования и всегда в 0 слоте....


		try
		{

			dword dwMode = FMOD_LOOP_OFF;
			if (_looped) dwMode = FMOD_LOOP_NORMAL;
			status = fmod_system->createStream(SoundName.c_str(), FMOD_HARDWARE | FMOD_2D | dwMode, 0, &sound);
			FMOD_ERROR("FMOD_SOUND:createStream", status);
			if (status != FMOD_OK)
			{
				api->Trace("fmod_system->createStream(%s, FMOD_HARDWARE | FMOD_2D | dwMode, 0, &sound)", SoundName.c_str());
				return 0;
			}

		}

		catch(...)
		{
			api->Trace("Internal FMOD error, when create stream. File '%s'", SoundName.c_str());
			return 0;
		}




		int MusicIdx = 0;
		int OldMusicIdx = 1;
		if (FaderParity)
		{
			MusicIdx = 1;
			OldMusicIdx = 0;
		}
		FaderParity = !FaderParity;




		if (OGG_sound[OldMusicIdx])
		{
			SoundStop(OldMusicIdx+1, _time);
			/*
			status = PlayingSounds[OldMusicIdx].channel->stop();
			FMOD_ERROR("FMOD_SOUND:stop",status);
			PlayingSounds[OldMusicIdx].channel = NULL;
			status = OGG_sound[OldMusicIdx]->release();
			FMOD_ERROR("FMOD_SOUND:release",status);
			*/
//			PlayingSounds[OldMusicIdx].fFaderNeedVolume = 0.0f ;
//			PlayingSounds[OldMusicIdx].fFaderCurrentVolume = _volume;
//			PlayingSounds[OldMusicIdx].fFaderDeltaInSec = -_volume / (_time * 0.001f);
		}

		SoundIdx = MusicIdx;
		OGG_sound[SoundIdx] = sound;
		PlayingSounds[SoundIdx].fFaderNeedVolume = _volume * fMusicVolume;
		PlayingSounds[SoundIdx].fFaderCurrentVolume = 0.0f;
		PlayingSounds[SoundIdx].fFaderDeltaInSec = (_volume * fMusicVolume) / (_time * 0.001f);





	} else
	{
		//Для всех остальных звуков берем из кеша
		int CacheIdx = GetFromCache(SoundName.c_str(), _type);
		if (CacheIdx < 0)
		{
			return 0;
		}

		SoundIdx = FindEmptySlot();
		if (SoundIdx < 0)
		{
			api->Trace("Can't find empty slots for sound !!!!");
			return 0;
		}

		 sound = SoundCache[CacheIdx].sound;
	}



	

	//--------
	TSD_ID SoundID = SoundIdx+1;

	PlayingSounds[SoundIdx].type = _volumeType;

	PlayingSounds[SoundIdx].fSoundVolume = _volume;

	//Начинаем проигрывать звук, правда запауженный...
	status = fmod_system->playSound(FMOD_CHANNEL_FREE, sound, true, &PlayingSounds[SoundIdx].channel);
	FMOD_ERROR("FMOD_SOUND:playSound", status);

	if (status != FMOD_OK)
	{
		api->Trace("fmod_system->playSound(FMOD_CHANNEL_FREE, sound, true, &PlayingSounds[%d].channel)", SoundIdx);
	}




	if (SoundIdx <= 1 )
	{
		unsigned int OGGpos = GetOGGPosition(SoundName.c_str());
		PlayingSounds[SoundIdx].channel->setPosition(OGGpos, FMOD_TIMEUNIT_MS);

		_prior = 0;
	}


	if (_prior < 0) _prior = 0;
	if (_prior > 255) _prior = 255;


	//Приоритетность ставим....
	PlayingSounds[SoundIdx].channel->setPriority(_prior);



	//3D каналу параметры настраиваем...
	if (_type == PCM_3D)
	{
		//Если надо ставим параметры по дефолту...
		if (_minDistance < 0.0f) _minDistance = 0.0f;
		if (_maxDistance < 0.0f) _maxDistance = 0.0f;
		//FMOD_Sound_Set3DMinMaxDistance(PlayingSounds[SoundIdx].sound, _minDistance * DISTANCEFACTOR, _maxDistance * DISTANCEFACTOR);
		status = PlayingSounds[SoundIdx].channel->set3DMinMaxDistance(_minDistance * DISTANCEFACTOR, _maxDistance * DISTANCEFACTOR);
		FMOD_ERROR("FMOD_SOUND:set3DMinMaxDistance",status);

		FMOD_VECTOR vVelocity = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR vPosition;
		vPosition.x = _startPosition->x;
		vPosition.y = _startPosition->y;
		vPosition.z = _startPosition->z;
		status = PlayingSounds[SoundIdx].channel->set3DAttributes(&vPosition, &vVelocity);
		FMOD_ERROR("FMOD_SOUND:set3DAttributes",status);
	}

	switch (_volumeType)
	{
	case VOLUME_FX: 
		_volume *= fFXVolume;
		break;
	case VOLUME_MUSIC: 
		_volume *= fMusicVolume;
		break;
	case VOLUME_SPEECH: 
		_volume *= fSpeechVolume;
		break;
	default:
		_volume *= 1.0f;
		break;
	}


	if (_time <= 0)
	{
		status = PlayingSounds[SoundIdx].channel->setVolume(_volume);
		FMOD_ERROR("FMOD_SOUND:setVolume",status);
	} else
	{
		status = PlayingSounds[SoundIdx].channel->setVolume(0);
		FMOD_ERROR("FMOD_SOUND:setVolume",status);
	}

	PlayingSounds[SoundIdx].Name = SoundName;
	PlayingSounds[SoundIdx].sound_type = _type;


	//На OGG убираем все EAX эффекты...
	//if (SoundIdx == 0)
	//{
	//}


	//Если не просто кеширование.... то снимаем с паузы...
	if (!_simpleCache)
	{
		status = PlayingSounds[SoundIdx].channel->setPaused(false);
		FMOD_ERROR("FMOD_SOUND:setPaused",status);
	} else
	{
		status = PlayingSounds[SoundIdx].channel->setPaused(true);
		FMOD_ERROR("FMOD_SOUND:setPaused",status);
	}

	PlayingSounds[SoundIdx].bFree = false;


	//Лупинг задаем..
	if (_looped)
	{
		status = PlayingSounds[SoundIdx].channel->setMode(FMOD_LOOP_NORMAL);
	}
	else
	{
		status = PlayingSounds[SoundIdx].channel->setMode(FMOD_LOOP_OFF);
	}

	FMOD_ERROR("FMOD_SOUND:setMode",status);





	//---------- пробегаем по всем звукам ищем с таким же channel --------------

	for (dword j = 0; j < MAX_SOUNDS_SLOTS; j++)
	{
		if (j == SoundIdx) continue;
		
		if (PlayingSounds[j].channel == PlayingSounds[SoundIdx].channel)
		{
			//Помечаем, что выкинулся звук....
			//что бы его не стопить....
			PlayingSounds[j].bFree = true;
		}
	}
//


	//Возвращаем ID звука...
	return SoundID;
}

//--------------------------------------------------------------------
void  SoundService::SoundSet3DParam  (TSD_ID _id, eSoundMessage _message, const void *_op)
{
	if (_id == 0) return;
	_id--;

	if (PlayingSounds[_id].bFree) return;


	CVECTOR vVelocity = CVECTOR(0.0f, 0.0f, 0.0f);

	switch (_message)
	{
	case SM_MAX_DISTANCE:
		{
			float maxDistance;
			maxDistance= *((float *) _op);
			status = PlayingSounds[_id].channel->set3DMinMaxDistance(NULL, maxDistance);
			FMOD_ERROR("FMOD_SOUND:set3DMinMaxDistance",status);
			break;
		}
	
	case SM_MIN_DISTANCE:
		{
			float minDistance;
			minDistance= *((float *) _op);
			status = PlayingSounds[_id].channel->set3DMinMaxDistance(minDistance, NULL);
			FMOD_ERROR("FMOD_SOUND:set3DMinMaxDistance",status);
			break;
		}

	case SM_POSITION:
		{
			FMOD_VECTOR pos;
			float* fPtr = (float *) _op;
			pos.x = *(fPtr + 0);
			pos.y = *(fPtr + 1);
			pos.z = *(fPtr + 2);
			FMOD_VECTOR vVelocity = { 0.0f, 0.0f, 0.0f };
			status = PlayingSounds[_id].channel->set3DAttributes(&pos, &vVelocity);
			FMOD_ERROR("FMOD_SOUND:set3DAttributes",status);
			break;
		}
	}


}


//--------------------------------------------------------------------
void  SoundService::SoundRestart (TSD_ID _id)
{
	if (_id == 0) return;
	_id--;
	// boal fix api->Trace("Sound restart !");
}

//--------------------------------------------------------------------
void  SoundService::SoundRelease (TSD_ID _id)
{
	if (_id == 0) return;
	_id--;


	//boal fix api->Trace("Sound release !");
}

//--------------------------------------------------------------------
void  SoundService::SoundSetVolume (TSD_ID _id, float _volume)
{
	if (_id == 0)
	{
		for (int i = 0; i < MAX_SOUNDS_SLOTS; i++)
		{
			if (PlayingSounds[i].bFree) continue;

			PlayingSounds[i].fSoundVolume = _volume;

			switch (PlayingSounds[i].type)
			{
			case VOLUME_FX: 
				_volume *= fFXVolume;
				break;
			case VOLUME_MUSIC: 
				_volume *= fMusicVolume;
				break;
			case VOLUME_SPEECH: 
				_volume *= fSpeechVolume;
				break;
			default:
				_volume *= 1.0f;
				break;
			}


			if (i <= 1)
			{
				PlayingSounds[i].fFaderNeedVolume = _volume;
				PlayingSounds[i].fFaderCurrentVolume = PlayingSounds[i].fFaderNeedVolume;
			}

			status = PlayingSounds[i].channel->setVolume(_volume);
			FMOD_ERROR("FMOD_SOUND:setVolume",status);
		}
		return;
	}

	_id--;
//	api->Trace("Sound set volume !");


	if (_id <= 1)
	{
		if (fabsf (PlayingSounds[_id].fFaderCurrentVolume - PlayingSounds[_id].fFaderNeedVolume) > 0.001f)
		{
			//PlayingSounds[_id].fFaderNeedVolume = _volume;
			//PlayingSounds[_id].fFaderCurrentVolume = PlayingSounds[_id].fFaderNeedVolume;

			return;
		}
	}



	if (PlayingSounds[_id].bFree) return;

	PlayingSounds[_id].fSoundVolume = _volume;

	switch (PlayingSounds[_id].type)
	{
	case VOLUME_FX: 
		_volume *= fFXVolume;
		break;
	case VOLUME_MUSIC: 
		_volume *= fMusicVolume;
		break;
	case VOLUME_SPEECH: 
		_volume *= fSpeechVolume;
		break;
	default:
		_volume *= 1.0f;
		break;
	}


	status = PlayingSounds[_id].channel->setVolume(_volume);
	FMOD_ERROR("FMOD_SOUND:setVolume",status);

}

//--------------------------------------------------------------------
bool  SoundService::SoundIsPlaying (TSD_ID _id)
{
	if (_id == 0) return false;
	_id--;
	return !PlayingSounds[_id].bFree;
}

//--------------------------------------------------------------------
void SoundService::SoundResume (TSD_ID _id, long _time/* = 0*/)
{
	if (_id == 0 || _id == 1)
	{
		for (int i = 0; i < MAX_SOUNDS_SLOTS; i++)
		{
			if (PlayingSounds[i].bFree) continue;
			status = PlayingSounds[i].channel->setPaused(false);
			FMOD_ERROR("FMOD_SOUND:setPaused",status);
		}
		return;

	}

	_id--;
	if (PlayingSounds[_id].bFree)
	{
		api->Trace("Can't sound resume %d !!!! Sound is not playig !!!!", _id);
		return;
	}
	
	status = PlayingSounds[_id].channel->setPaused(false);
	FMOD_ERROR("FMOD_SOUND:setPaused",status);
	//PlayingSounds[_id].bCacheOnly = false;
}

//--------------------------------------------------------------------
float SoundService::SoundGetPosition (TSD_ID _id)
{
	if (_id == 0) return 0;
	_id--;
	if (PlayingSounds[_id].bFree) return 0;

	unsigned int SoundPositionInMilisecond;
	status = PlayingSounds[_id].channel->getPosition(&SoundPositionInMilisecond, FMOD_TIMEUNIT_MS);
	FMOD_ERROR("FMOD_SOUND:getPosition",status);
	return 0;
}


//--------------------------------------------------------------------
void SoundService::SetCameraPosition (const CVECTOR &_cameraPosition)
{
	vListenerPos.x = _cameraPosition.x;
	vListenerPos.y = _cameraPosition.y;
	vListenerPos.z = _cameraPosition.z;

	status = fmod_system->set3DListenerAttributes(0, &vListenerPos, NULL, &vListenerForward, &vListenerTop);
	FMOD_ERROR("FMOD:set3DListenerAttributes",status);
}

//--------------------------------------------------------------------
void SoundService::SetCameraOrientation (const CVECTOR &_nose, const CVECTOR &_head)
{
	CVECTOR nose = !_nose;
	CVECTOR head = !_head;

	vListenerForward.x = nose.x;
	vListenerForward.y = nose.y;
	vListenerForward.z = nose.z;
	
	vListenerTop.x = head.x;
	vListenerTop.y = head.y;
	vListenerTop.z = head.z;

	status = fmod_system->set3DListenerAttributes(0, &vListenerPos, NULL, &vListenerForward, &vListenerTop);
	FMOD_ERROR("FMOD:set3DListenerAttributes",status);
}

//--------------------------------------------------------------------
void  SoundService::SetMasterVolume (float _fxVolume, float _musicVolume, float _speechVolume)
{
	if (_fxVolume < 0.0f) _fxVolume = 0.0f;
	if (_fxVolume > 1.0f) _fxVolume = 1.0f;

	if (_musicVolume < 0.0f) _musicVolume = 0.0f;
	if (_musicVolume > 1.0f) _musicVolume = 1.0f;

	if (_speechVolume < 0.0f) _speechVolume = 0.0f;
	if (_speechVolume > 1.0f) _speechVolume = 1.0f;

	fFXVolume = _fxVolume;
	fMusicVolume = _musicVolume;
	fSpeechVolume = _speechVolume;

	for (int i = 0; i < MAX_SOUNDS_SLOTS; i++)
	{
		if (PlayingSounds[i].bFree) 
		{
			continue;
		}

		//Vano: Check if sound exist
		/*bool bPlaying;
		status = PlayingSounds[i].channel->isPlaying(&bPlaying);
		if (status != FMOD_OK)
		{
			PlayingSounds[i].bFree = true;
			continue;
		}*/

		//update volume.....

		float _volume = PlayingSounds[i].fSoundVolume;

		switch (PlayingSounds[i].type)
		{
		case VOLUME_FX: 
			_volume *= fFXVolume;
			break;
		case VOLUME_MUSIC: 
			_volume *= fMusicVolume;
			break;
		case VOLUME_SPEECH: 
			_volume *= fSpeechVolume;
			break;
		default:
			_volume *= 1.0f;
			break;
		}

		status = PlayingSounds[i].channel->setVolume(_volume);
		if (status != FMOD_OK)
		{
			PlayingSounds[i].bFree = true;
			continue;
		}
		//FMOD_ERROR("FMOD_SOUND:setVolume",status);

		
	}

	//api->Trace("Set master volume");
}

//--------------------------------------------------------------------
void  SoundService::GetMasterVolume (float *_fxVolume, float *_musicVolume, float *_speechVolume)
{
	*_fxVolume = fFXVolume;
	*_musicVolume = fMusicVolume;
	*_speechVolume = fSpeechVolume;

	api->Trace("Get master volume");
}


//--------------------------------------------------------------------
TSD_ID SoundService::SoundDuplicate (TSD_ID _sourceID)
{
	_sourceID--;
	api->Trace("Sound duplicate");
	return 0;
}

void SoundService::SetEnabled (bool _enabled)
{
}

void SoundService::SoundStop (TSD_ID _id, long _time)
{
    bool is_playing; // boal fix
    
	if (_id == 0)
	{
		//--------- удаляем все звуки нах. -----------------------------------------

		int start = 0;
		if (_time > 0)
		{
			float fVol = 0.0f;
			PlayingSounds[0].channel->getVolume(&fVol);
			PlayingSounds[0].fFaderNeedVolume = 0.0f ;
			PlayingSounds[0].fFaderCurrentVolume = fVol;
			PlayingSounds[0].fFaderDeltaInSec = -fVol / (_time * 0.001f);

			if (PlayingSounds[0].fFaderDeltaInSec <= 0.00001f)
			{
				status = OGG_sound[0]->release();
				OGG_sound[0] = NULL;
			}


			PlayingSounds[1].channel->getVolume(&fVol);
			PlayingSounds[1].fFaderNeedVolume = 0.0f ;
			PlayingSounds[1].fFaderCurrentVolume = fVol;
			PlayingSounds[1].fFaderDeltaInSec = -fVol / (_time * 0.001f);

			if (PlayingSounds[1].fFaderDeltaInSec <= 0.00001f)
			{
				status = OGG_sound[1]->release();
				OGG_sound[1] = NULL;
			}

			start = 2;
		} else
		{
			start = 0;
		}



		for (int i = start; i < MAX_SOUNDS_SLOTS; i++)
		{
			if (PlayingSounds[i].bFree) continue;

			if (i <= 1)
			{
				unsigned int OGGpos;
				PlayingSounds[i].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
				SetOGGPosition(PlayingSounds[i].Name.c_str(), OGGpos);
			}


			status = PlayingSounds[i].channel->isPlaying(&is_playing);
			if (!is_playing || status != FMOD_OK) // boal fix
			{
			    PlayingSounds[i].bFree = true;
			}
			else
			{
				status = PlayingSounds[i].channel->stop();
				FMOD_ERROR("FMOD_SOUND:stop",status);
			}
		}

		if (_time <= 0)
		{
			if (OGG_sound[0])
			{
				status = OGG_sound[0]->release();
				FMOD_ERROR("FMOD_SOUND:release",status);
			}
			OGG_sound[0] = NULL;

			if (OGG_sound[1])
			{
				status = OGG_sound[1]->release();
				FMOD_ERROR("FMOD_SOUND:release",status);
			}
			OGG_sound[1] = NULL;
		}

		//--------- удаляем все звуки нах. -----------------------------------------
		return;
	}

	_id--;

	//Удаляем выбранный........................

	if (PlayingSounds[_id].bFree) return;

	if (_time > 0)
	{
		float fVol = 0.0f;
		PlayingSounds[_id].channel->getVolume(&fVol);
		PlayingSounds[_id].fFaderNeedVolume = 0.0f ;
		PlayingSounds[_id].fFaderCurrentVolume = fVol;
		PlayingSounds[_id].fFaderDeltaInSec = -fVol / (_time * 0.001f);
	} else
	{
		if (_id <= 1)
		{
			unsigned int OGGpos;
			PlayingSounds[_id].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
			SetOGGPosition(PlayingSounds[_id].Name.c_str(), OGGpos);
		}

        if (!PlayingSounds[_id].bFree) // boal fix
        {
            status = PlayingSounds[_id].channel->isPlaying(&is_playing);
			if (!is_playing || status != FMOD_OK) // boal fix
			{
			    PlayingSounds[_id].bFree = true;
			}
			else
			{
				status = PlayingSounds[_id].channel->stop();
				FMOD_ERROR("FMOD_SOUND:stop",status);
			}
		}

		if (_id == 0 && OGG_sound[0] && status == FMOD_OK)
		{
			status = OGG_sound[0]->release();
			FMOD_ERROR("FMOD_SOUND:release",status);
			OGG_sound[0] = NULL;
		}

		if (_id == 1 && OGG_sound[1] && status == FMOD_OK)
		{
			status = OGG_sound[1]->release();
			FMOD_ERROR("FMOD_SOUND:release",status);
			OGG_sound[1] = NULL;
		}
	}


}


void SoundService::AnalyseNameStringAndAddToAlias(tAlias *_alias, const char *in_string)
{
	static char tempString2[COMMON_STRING_LENGTH];
	strncpy (tempString2, in_string, COMMON_STRING_LENGTH);

	tAliasSound snd;

	char * col = strchr(tempString2, ',');
	if (!col)
	{ // only name, without delays
		_alias->fMaxProbabilityValue += DEFAULT_PROBABILITY;
		snd.fProbability = DEFAULT_PROBABILITY;
		snd.FileName = tempString2;

		//api->Trace("  -> sound %s", snd.FileName.c_str());

		_alias->SoundFiles.Add(snd);

		return;
	}

	// try to convert forthcoming number
	float n1;
	if (sscanf(++col, "%f", &n1) == 1)
	{
		snd.fProbability = n1;
		_alias->fMaxProbabilityValue += n1;
	}

	*(--col) = 0; // truncate at first ','
	snd.FileName = tempString2;

	//api->Trace("  -> sound %s, %f", snd.FileName.c_str(), snd.fProbability);

	_alias->SoundFiles.Add(snd);
}

void SoundService::AddAlias(INIFILE* _iniFile, char* _sectionName)
{
	static char tempString[COMMON_STRING_LENGTH];
	if (!_iniFile || !_sectionName) return;

	//api->Trace("Add sound alias %s", _sectionName);


	tAlias* pAlias = &Aliases[Aliases.Add()];

	pAlias->Name = _sectionName;
	pAlias->dwNameHash = string::HashNoCase(pAlias->Name.c_str());
	pAlias->fMaxDistance = _iniFile->GetFloat(_sectionName, "maxDistance", -1.0f);
	pAlias->fMinDistance = _iniFile->GetFloat(_sectionName, "minDistance", -1.0f);
	pAlias->fVolume = _iniFile->GetFloat(_sectionName, "volume", -1.0f);
	pAlias->iPrior = _iniFile->GetLong(_sectionName, "prior", 128);

	pAlias->fMaxProbabilityValue = 0.0f;


	if (_iniFile->ReadString(_sectionName, "name", tempString , COMMON_STRING_LENGTH, ""))
	{
		AnalyseNameStringAndAddToAlias(pAlias, tempString);
		while (_iniFile->ReadStringNext(_sectionName, "name", tempString, COMMON_STRING_LENGTH))
			AnalyseNameStringAndAddToAlias(pAlias, tempString);
	}


}


void SoundService::LoadAliasFile (const char *_filename)
{
	const static int SECTION_NAME_LENGTH = 128;
	static char sectionName[SECTION_NAME_LENGTH];


	string iniName = ALIAS_DIRECTORY;
	iniName += _filename;

	//api->Trace("Find sound alias file %s", iniName.c_str());

	INIFILE *aliasIni;
	aliasIni = api->fio->OpenIniFile(iniName);
	if (!aliasIni) return;

	if (aliasIni->GetSectionName(sectionName, SECTION_NAME_LENGTH))
	{
		AddAlias(aliasIni, sectionName);
		while (aliasIni->GetSectionNameNext(sectionName, SECTION_NAME_LENGTH))
		{
			AddAlias(aliasIni, sectionName);
		}
	}

	DELETE(aliasIni);
}

void SoundService::InitAliases ()
{
	string iniName = ALIAS_DIRECTORY;
	iniName += "*.ini";

	HANDLE foundFile;
	WIN32_FIND_DATA findData;
	if ((foundFile = api->fio->_FindFirstFile(iniName, &findData)) != INVALID_HANDLE_VALUE)
	{
		do
		LoadAliasFile(findData.cFileName);
		while (api->fio->_FindNextFile(foundFile, &findData) == TRUE);
	}
}



void SoundService::CreateEntityIfNeed ()
{
	ENTITY_ID DebugEntity_id;
	bool bResult = api->FindClass(&DebugEntity_id, "SoundVisualisationEntity", 0);
	if (!bResult)
	{
		api->CreateEntity(&DebugEntity_id, "SoundVisualisationEntity");
	}

	SoundVisualisationEntity* pDebugEntity = (SoundVisualisationEntity*)api->GetEntityPointer(&DebugEntity_id);
	pDebugEntity->SetMasterSoundService(this);
	pDebugEntity->Wakeup();

}

void SoundService::DebugDraw ()
{
	if (api->Controls->GetDebugAsyncKeyState('J') < 0)
	{
		bShowDebugInfo = !bShowDebugInfo;
		Sleep(200);
	}

	if (!bShowDebugInfo) return;
//	pRS->Print(0, 0, "sound debug draw !!!!");


	bool bShowDistances = true;

	int Ypos = 38+16;
	int Count = 0;
	int RealCount = 0;

	FMOD_VECTOR lpos, lvel, lforward, lup; 
	fmod_system->get3DListenerAttributes(0, &lpos, &lvel, &lforward, &lup);

	CVECTOR vListener = CVECTOR (lpos.x, lpos.y, lpos.z);


	float fTotal;
	fmod_system->getCPUUsage(NULL, NULL, NULL, &fTotal);
	int CurrentAlloc, PeakAlloc;
	FMOD::Memory_GetStats(&CurrentAlloc, &PeakAlloc);

	CVECTOR list_pos;
	list_pos.x = lpos.x;
	list_pos.y = lpos.y;
	list_pos.z = lpos.z;
	//pRS->DrawSphere(list_pos, 4.0f, 0xFF008000);

	pRS->Print(0, 0, "CPU Usage %3.2f Mem: %3.2f Kb, MemPeak %3.2f Kb, Cached %d sounds", fTotal, CurrentAlloc/1024.0f, PeakAlloc/1024.0f, SoundCache.Size());
	pRS->Print(0, 16, "position  %3.2f, %3.2f, %3.2f, forward %3.2f, %3.2f, %3.2f, up %3.2f, %3.2f, %3.2f", lpos.x, lpos.y, lpos.z, lforward.x, lforward.y, lforward.z, lup.x, lup.y, lup.z);

	CMatrix ind;
	ind.SetIdentity();
	pRS->SetWorld(ind);



	for (int i = 0; i < MAX_SOUNDS_SLOTS; i++)
	{
		if (PlayingSounds[i].bFree) continue;

		bool bVirtual;
		PlayingSounds[i].channel->isVirtual(&bVirtual);

		bool paused;
		PlayingSounds[i].channel->getPaused(&paused);

		float fMin, fMax;
		PlayingSounds[i].channel->get3DMinMaxDistance(&fMin, &fMax);

		FMOD_MODE sound_mode;
		PlayingSounds[i].channel->getMode(&sound_mode);
		bool bIsLooped = false;
		if ((sound_mode & FMOD_LOOP_NORMAL) != 0) bIsLooped = true;


		float fVol;
		PlayingSounds[i].channel->getVolume(&fVol);

		bool bPlaying;
		PlayingSounds[i].channel->isPlaying(&bPlaying);

		unsigned int position;
		PlayingSounds[i].channel->getPosition(&position, FMOD_TIMEUNIT_MS);


		if (!bVirtual) RealCount++;
		Count++;

		FMOD_VECTOR pos, vel; 
		PlayingSounds[i].channel->get3DAttributes(&pos, &vel);

		float audib = 0;
		PlayingSounds[i].channel->getAudibility(&audib);


		int prior;
		PlayingSounds[i].channel->getPriority(&prior);




		if (PlayingSounds[i].sound_type == PCM_3D)
		{
			//0xFFFFFF00 играет но не слышно
			//0xFFFF0000 не играет, отклипировался
			//0xFF00FF00 играет

			Color drawColor = Color(0xFFFF0000L);

			if (!bVirtual)
			{
				//морфируем между желтым audib 0 и зеленыи audib 1 цветами
				Color Zero = Color(0xFFFFFF00L);
				Color Full = Color(0xFF00FF00L);
				drawColor.Lerp(Zero, Full, audib);
			}


			CVECTOR vec_pos;
			vec_pos.x = pos.x;
			vec_pos.y = pos.y;
			vec_pos.z = pos.z;
			pRS->DrawSphere(vec_pos, 0.2f, drawColor.GetDword());

			if (bShowDistances && audib > 0.001f)
			{
				pRS->SetWorld(ind);
				Draw2DCircle(vec_pos, drawColor.GetDword(), fMin, drawColor.GetDword(), fMax);
			}

			float fDistance = CVECTOR (vListener-vec_pos).GetLength();

			DebugPrint3D(vec_pos, 30.0f, 2, 1.0f, drawColor.GetDword(), 1.0f, "%s", PlayingSounds[i].Name.c_str());
			DebugPrint3D(vec_pos, 30.0f, 0, 1.0f, drawColor.GetDword(), 1.0f, "%3.2f m", fDistance);
			DebugPrint3D(vec_pos, 30.0f, 4, 1.0f, drawColor.GetDword(), 1.0f, "prior: %d", prior);
		} else
		{
			pRS->Print(0, Ypos, "[%d] [vol:%f] [Pos:%d] [Loop:%d] [Play:%d] sound [Pause:%d][Vir:%d][Aud:%f] '%s' 0x%08X", i, fVol, position, bIsLooped, bPlaying, paused, bVirtual, audib, PlayingSounds[i].Name.c_str(), PlayingSounds[i].channel);
			Ypos += 16;
		}

		//pRS->Print(0, Ypos, "pos %f, %f, %f", pos.x, pos.y, pos.z);
		//Ypos += 16;
		//pRS->Print(0, Ypos, "min %f,  max %f", fMin, fMax);
		//Ypos += 16;

		

		//pRS->DrawSphere(vec_pos, fMax, 0xFFFF0000);
	}

	pRS->Print(0, 32, "Real sounds %d, Total sounds %d", RealCount, Count);


	pRS->Print(500, 0, "Sound schemes %d", SoundSchemeChannels.Size());
	Ypos = 16;
	for (dword i = 0; i < SoundSchemeChannels.Size(); i++)
	{
		pRS->Print(510, Ypos, "[%d] %s", i, SoundSchemeChannels[i].soundName);
		Ypos += 16;
	}

}


int SoundService::GetFromCache (const char* szName, eSoundType _type)
{
	dword dwSearchHash = string::HashNoCase(szName);

	for (dword i = 0; i < SoundCache.Size(); i++)
	{
		if (SoundCache[i].type != _type) continue;
		if (SoundCache[i].dwNameHash == dwSearchHash)
		{
			if (SoundCache[i].Name == szName)
			{
				SoundCache[i].fTimeFromLastPlay = 0.0;
				return i;
			}
		}
	}


	FMOD_MODE mode =  FMOD_SOFTWARE;
	if (_type == PCM_3D)
	{
		mode = mode | FMOD_3D | FMOD_3D_LINEARROLLOFF;
	}

	if (_type == PCM_STEREO)
	{
		mode = mode | FMOD_2D;
	}


	tSoundCache Cache;
	status = fmod_system->createSound(szName, mode, 0, &Cache.sound);
	//FMOD_ERROR("FMOD_SOUND:createSound",status);

	if (Cache.sound == NULL)
	{
		//api->Trace("Problem with sound loading !!! '%s'", szName);
		return -1;
	}
	Cache.type = _type;
	Cache.Name = szName;
	Cache.dwNameHash = dwSearchHash;
	Cache.fTimeFromLastPlay = 0.0f;

	SoundCache.Add(Cache);

	return (SoundCache.Size()-1);
}


//Написать текст
void _cdecl SoundService::DebugPrint3D(const CVECTOR & pos3D, float rad, long line, float alpha, dword color, float scale, const char * format, ...)
{

	static char buf[256];
	//Печатаем в буфер
	long len = _vsnprintf(buf, sizeof(buf) - 1, format, (char *)(&format + 1));
	buf[sizeof(buf) - 1] = 0;
	//Ищем позицию точки на экране
	static CMatrix mtx, view, prj;
	static D3DVIEWPORT8 vp;
	MTX_PRJ_VECTOR vrt;
	pRS->GetTransform(D3DTS_VIEW, view);
	pRS->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	view.Transposition();
	float dist = ~(pos3D - view.Pos());
	if(dist >= rad*rad) return;
	float d = view.Vz() | view.Pos();
	if((pos3D | view.Vz()) < d) return;
	pRS->GetViewport(&vp);
	mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width*0.5f, vp.Height*0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
	//Ищем позицию
	long fh = pRS->CharHeight(FONT_DEFAULT)/2;
	vrt.y -= (line + 0.5f)*fh;
	//Прозрачность	
	const float kDist = 0.75f;
	if(alpha < 0.0f) alpha = 0.0f;
	if(alpha > 1.0f) alpha = 1.0f;
	if(dist > kDist*kDist*rad*rad)
	{
		dist = 1.0f - (sqrtf(dist) - kDist*rad)/(rad - kDist*rad);
		alpha *= dist;
	}
	if(alpha <= 0.0f) return;
	color = (dword(alpha*255.0f) << 24) | (color & 0xffffff);
	//Печатаем текст
	pRS->ExtPrint(FONT_DEFAULT, color, 0x00000000, ALIGN_CENTER, 0, scale, 0, 0, long(vrt.x), long(vrt.y), buf);
}

void SoundService::Draw2DCircle (const CVECTOR& center, DWORD dwColor, float fRadius, DWORD dwColor2, float fRadius2)
{
	float fDelta = 0.2f;
	RS_LINE line[2];
	CVECTOR vStart;
	CVECTOR vEnd;
	CVECTOR vStartPoint;

	vStartPoint = CVECTOR(cosf(0)*fRadius, 0.0f, sinf(0)*fRadius);
	vStartPoint += center;
	for (float Angle = fDelta; Angle <= (PI*2); Angle += fDelta)
	{
		CVECTOR vPoint = CVECTOR(cosf(Angle)*fRadius, 0.0f, sinf(Angle)*fRadius);
		vPoint += center;

		vStart = vStartPoint;
		vEnd = vPoint;

		line[0].vPos = vStart;
		line[0].dwColor = dwColor;
		line[1].vPos = vEnd;
		line[1].dwColor = dwColor;
		pRS->DrawLines(line, 1, "AILine");
		//pRS->DrawVector(vStart, vEnd, dwColor);
		vStartPoint = vPoint;
	}

	vStart = vStartPoint;
	vEnd = CVECTOR(cosf(0)*fRadius, 0.0f, sinf(0)*fRadius);
	vEnd += center;

	line[0].vPos = vStart;
	line[0].dwColor = dwColor;
	line[1].vPos = vEnd;
	line[1].dwColor = dwColor;
	pRS->DrawLines(line, 1, "AILine");


	vStartPoint = CVECTOR(cosf(0)*fRadius2, 0.0f, sinf(0)*fRadius2);
	vStartPoint += center;
	for (float Angle = fDelta; Angle <= (PI*2); Angle += fDelta)
	{
		CVECTOR vPoint = CVECTOR(cosf(Angle)*fRadius2, 0.0f, sinf(Angle)*fRadius2);
		vPoint += center;

		vStart = vStartPoint;
		vEnd = vPoint;

		line[0].vPos = vStart;
		line[0].dwColor = dwColor2;
		line[1].vPos = vEnd;
		line[1].dwColor = dwColor2;
		pRS->DrawLines(line, 1, "AILine");
		//pRS->DrawVector(vStart, vEnd, dwColor);
		vStartPoint = vPoint;
	}

	vStart = vStartPoint;
	vEnd = CVECTOR(cosf(0)*fRadius2, 0.0f, sinf(0)*fRadius2);
	vEnd += center;

	line[0].vPos = vStart;
	line[0].dwColor = dwColor2;
	line[1].vPos = vEnd;
	line[1].dwColor = dwColor2;
	pRS->DrawLines(line, 1, "AILine");


	line[0].vPos = CVECTOR (fRadius, 0.0f, 0.0f) + center;
	line[0].dwColor = dwColor;
	line[1].vPos = CVECTOR (fRadius2, 0.0f, 0.0f) + center;
	line[1].dwColor = dwColor;
	pRS->DrawLines(line, 1, "AILine");


	line[0].vPos = CVECTOR (-fRadius, 0.0f, 0.0f) + center;
	line[0].dwColor = dwColor;
	line[1].vPos = CVECTOR (-fRadius2, 0.0f, 0.0f) + center;
	line[1].dwColor = dwColor;
	pRS->DrawLines(line, 1, "AILine");


	line[0].vPos = CVECTOR (0.0f, 0.0f, fRadius) + center;
	line[0].dwColor = dwColor;
	line[1].vPos = CVECTOR (0.0f, 0.0f, fRadius2) + center;
	line[1].dwColor = dwColor;
	pRS->DrawLines(line, 1, "AILine");

	line[0].vPos = CVECTOR (0.0f, 0.0f, -fRadius) + center;
	line[0].dwColor = dwColor;
	line[1].vPos = CVECTOR (0.0f, 0.0f, -fRadius2) + center;
	line[1].dwColor = dwColor;
	pRS->DrawLines(line, 1, "AILine");

	//pRS->DrawVector(vStart, vEnd, dwColor);
}


//--------------------------------------------------------------------
bool SoundService::SFLB_SetScheme(const char *_schemeName)
{
	ResetScheme();
	return AddScheme(_schemeName);
}

bool SoundService::SetScheme (const char *_schemeName)
{
	return SFLB_SetScheme(_schemeName);
}

//--------------------------------------------------------------------
void SoundService::ResetScheme()
{
	SoundSchemeChannels.DelAll();
}

//--------------------------------------------------------------------
bool SoundService::AddScheme (const char *_schemeName)
{
	INIFILE *ini;
	static char tempString[COMMON_STRING_LENGTH];
	ini = api->fio->OpenIniFile(SCHEME_INI_NAME);

	if (!ini)
		return false;

	if (ini->ReadString(const_cast<char *>(_schemeName), SCHEME_KEY_NAME, tempString, COMMON_STRING_LENGTH, ""))
	{
		AddSoundSchemeChannel(tempString);
		while (ini->ReadStringNext(const_cast<char *>(_schemeName), SCHEME_KEY_NAME, tempString, COMMON_STRING_LENGTH))
			AddSoundSchemeChannel(tempString);
	}

	if (ini->ReadString(const_cast<char *>(_schemeName), SCHEME_KEY_NAME_LOOP, tempString, COMMON_STRING_LENGTH, ""))
	{
		AddSoundSchemeChannel(tempString,true);
		while (ini->ReadStringNext(const_cast<char *>(_schemeName), SCHEME_KEY_NAME_LOOP, tempString, COMMON_STRING_LENGTH))
			AddSoundSchemeChannel(tempString,true);
	}

	DELETE (ini);
	return true;
}

bool SoundService::AddSoundSchemeChannel (char *in_string, bool _looped /*= false*/)
{
	static char tempString2[COMMON_STRING_LENGTH];
	strncpy (tempString2, in_string, COMMON_STRING_LENGTH);

	char * col = strchr(tempString2, ',');
	if (!col)
	{ // only name, without delays
		tSoundSchemeChannel NewChannel;
		NewChannel.soundName = tempString2;
		NewChannel.minDelayTime = SCHEME_MIN_DELAY;
		NewChannel.maxDelayTime = SCHEME_MAX_DELAY;
		NewChannel.timeToNextPlay = (long) rand((float) (NewChannel.maxDelayTime-NewChannel.minDelayTime));
		NewChannel.volume = 1.f;
		NewChannel.looped = _looped;
		if (_looped) NewChannel.timeToNextPlay = 0;

		SoundSchemeChannels.Add(NewChannel);
		return true;
	}


	tSoundSchemeChannel NewChannel;

	// try to convert forthcoming numbers
	int n1, n2;
	float f1;
	int numbersConverted = sscanf(++col, "%d, %d, %f", &n1, &n2, &f1);
	switch (numbersConverted)
	{
	case 1:
		NewChannel.minDelayTime = SCHEME_MIN_DELAY;
		NewChannel.maxDelayTime = n1 * 1000;
		NewChannel.volume       = 1.f;
		break;
	case 2:
		NewChannel.minDelayTime = n1 * 1000;
		NewChannel.maxDelayTime = n2 * 1000;
		NewChannel.volume       = 1.f;
		break;
	case 3:
		NewChannel.minDelayTime = n1 * 1000;
		NewChannel.maxDelayTime = n2 * 1000;
		NewChannel.volume       = f1;
		break;
	};
	*(--col) = 0; // truncate at first ','
	NewChannel.soundName = tempString2;
	NewChannel.looped = _looped;
	
	if (_looped)
		NewChannel.timeToNextPlay = 0;
	else
		NewChannel.timeToNextPlay = (long) rand((float) (NewChannel.maxDelayTime - NewChannel.minDelayTime));

	SoundSchemeChannels.Add(NewChannel);
	return true;
}


void SoundService::ProcessSoundSchemes ()
{
	// handle schemes
	//if (!schemesEnabled) return;

	dword dTime = api->GetDeltaTime();

	for (dword i = 0; i < SoundSchemeChannels.Size(); i++)
	{
		if (SoundSchemeChannels[i].looped)
		{
			if (SoundSchemeChannels[i].timeToNextPlay) continue;
			SoundSchemeChannels[i].timeToNextPlay = -1;
			SoundPlay(SoundSchemeChannels[i].soundName, PCM_STEREO, VOLUME_FX, false, true, false, 0, 0, -1.f, -1.f, 0, SoundSchemeChannels[i].volume);
		}
		else
		{
			if (SoundSchemeChannels[i].timeToNextPlay > 0)
			{
				SoundSchemeChannels[i].timeToNextPlay -= dTime;
			} else
				{
					SoundSchemeChannels[i].timeToNextPlay = SoundSchemeChannels[i].minDelayTime 
						+ (long) rand((float) (SoundSchemeChannels[i].maxDelayTime - SoundSchemeChannels[i].minDelayTime));
					SoundPlay(SoundSchemeChannels[i].soundName, PCM_STEREO, VOLUME_FX, false, false, false, 0, 0, -1.f, -1.f, 0, SoundSchemeChannels[i].volume);
				}
		}//looped
	}

}


int SoundService::GetOGGPositionIndex (const char* szName)
{
	dword dwHash = string::HashNoCase(szName);

	for (dword i = 0; i < OGGPosition.Size(); i++)
	{
		if (OGGPosition[i].dwHash == dwHash)
		{
			if (OGGPosition[i].Name == szName)
			{
				return i;
			}
		}
	}

	return -1;
}

unsigned int SoundService::GetOGGPosition (const char* szName)
{
	int idx = GetOGGPositionIndex(szName);
	if (idx >= 0)
	{
		return OGGPosition[idx].position;
	}

	return 0;
}

void SoundService::SetOGGPosition (const char* szName, unsigned int pos)
{
	int idx = GetOGGPositionIndex(szName);
	if (idx >= 0)
	{
		OGGPosition[idx].position = pos;
		return;
	}

	PlayedOGG ogg;
	ogg.dwHash = string::HashNoCase(szName);
	ogg.Name = szName;
	ogg.position = pos;

	OGGPosition.Add(ogg);
}


