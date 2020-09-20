// DEFINES
#define MUSIC_CHANGE_TIME  3000
#define MUSIC_SILENCE_TIME 180000.0
#define SOUNDS_FADE_TIME   200

#event_handler ("LoadSceneSound", "LoadSceneSound")
// PLAY
int Play3DSound(string name, float x, float y, float z)
{
	InitSound();
	//Trace("Play3DSound : "+name);
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, false, false, 0, x, y, z);
}

int Play3DSoundCached(string name, float x, float y, float z)
{
	InitSound();
	//Trace("Play3DSoundCached : "+name);
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, false, true, 0, x, y, z);
}

int Play3DSoundComplex(string name, float x, float y, float z, bool bLooped, bool bCached)
{
	InitSound();
	return SendMessage(Sound,"lsllllllfff",MSG_SOUND_PLAY, name, SOUND_WAV_3D, VOLUME_FX, false, bLooped, bCached, 0, x, y, z);
}

int PlayStereoSound(string name)
{
	InitSound();
	//Trace("PlayStereoSound : "+name);
	return SendMessage(Sound,"lslllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_FX, false, false, false);
}

int PlayStereoSoundLooped(string name)
{
	InitSound();
	//Trace("PlayStereoSoundLooped : "+name);
	return SendMessage(Sound,"lsllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_FX, false, true, false);
}

int PlayStereoSoundLooped_JustCache(string name)
{
	InitSound();
	//Trace("PlayStereoSoundLooped : "+name);
	return SendMessage(Sound,"lslllll",MSG_SOUND_PLAY, name, SOUND_WAV_STEREO, VOLUME_FX, true, true, false);
}

int PlayStereoOGG(string name)
{
	InitSound();
	//Trace("PlayStereoSound : "+name);
	return SendMessage(Sound,"lsllllll",MSG_SOUND_PLAY, name, SOUND_MP3_STEREO, VOLUME_FX, false, false, false, 0); //fix boal
}

// OTHER METHODS
void StopSound(int id, int fade)
{
	InitSound();
	//Trace("StopSound : "+id);
	SendMessage(Sound, "lll", MSG_SOUND_STOP, id, fade);
}

void ResumeSound(int id, int fade)
{
	InitSound();
	//Trace("ResumeSound : "+id);
	SendMessage(Sound, "lll", MSG_SOUND_RESUME, id, fade);
}

void ReleaseSound(int id)
{
	InitSound();
	//Trace("ReleaseSound : "+id);
	SendMessage(Sound, "ll", MSG_SOUND_RELEASE, id);
}

// SOUND SCHEMES
void ResetSoundScheme()
{
	InitSound();
	//Trace("ResetSoundScheme");
	SendMessage(Sound,"l",MSG_SOUND_SCHEME_RESET);
}

void SetSoundScheme(string schemeName)
{
	InitSound();
	//Trace("SetSoundScheme: "+schemeName);
	SendMessage(Sound,"ls",MSG_SOUND_SCHEME_SET,schemeName);
}

void AddSoundScheme(string schemeName)
{
	InitSound();
	//Trace("AddSoundScheme: "+schemeName);
	SendMessage(Sound,"ls",MSG_SOUND_SCHEME_ADD,schemeName);
}

/* Тепер эта функция опередляет звуковые схемы в игре на СУШЕ!. Внутри проверяется
как погода, так и время. Сделано для того, что бы не было схем, которые
бы игрались независимо от погоды или времени, ибо это всё-таки неудобно
в использовании */

void SetWeatherScheme(string scheme)
{
	if (Whr_IsStorm())
	{
		if (Whr_IsNight())
				{
					SetSoundScheme(scheme+"_night_storm");
				}
			else
				{
					SetSoundScheme(scheme+"_day_storm");
				}
	}
	else 
	{ 
		if (Whr_IsRain())
		{
			if (Whr_IsNight())
				{
					SetSoundScheme(scheme+"_night_rain");
				}
			else
				{
					SetSoundScheme(scheme+"_day_rain");
				}					
		}
		else
		{
			if (Whr_IsNight())
				{
					SetSoundScheme(scheme+"_night");
				}
			else
				{
					SetSoundScheme(scheme+"_day");
				}
		}
	}
}

/* void SetTimeScheme(string scheme)
{
	if (Whr_IsNight())
	{
		AddSoundScheme(scheme+"_night");
	}
	else
	{
		AddSoundScheme(scheme+"_day");
	}
}
*/
void SetSchemeForLocation (ref loc)
{
    ref mchref = GetMainCharacter();
    mchref.GenQuest.God_hit_us = false; // нефиг воровать :)  (только в домах)
    int iColony = -1; //boal music
    
    if(CheckAttribute(loc,"type"))
	{
		ResetSoundScheme();
		SetMusicAlarm(""); // иузыка не играла, если переходили меж локациями одной схемы - багфиx boal 28.06.06
		switch (loc.type)
		{
		case "town":
			SetWeatherScheme("town");
   			if (CheckAttribute(loc,"fastreload"))
			{
			    iColony = FindColony(loc.fastreload);
			}
			if (iColony != -1)
			{
			    SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music");
			}
			else
			{
				SetMusicAlarm("music_gorod");
			}
			break;
		case "land": // дуэльное поле, лэндфорт и так далее
			SetWeatherScheme("land");
			SetMusicAlarm("music_jungle");
			break;
		case "jungle": // дуэльное поле, лэндфорт и так далее
			SetWeatherScheme("land");
			SetMusicAlarm("music_jungle");
			DeleteAttribute(pchar, "CheckStateOk"); // убрать флаг проверенности протектором
			break;
		case "mayak": //eddy. теперь надо
			ResetSoundScheme();
			SetWeatherScheme("seashore");
			SetMusicAlarm("music_jungle");
			break;
		case "seashore": 
			ResetSoundScheme();
			SetWeatherScheme("seashore");
			SetMusicAlarm("music_jungle");
			DeleteAttribute(pchar, "CheckStateOk"); // убрать флаг проверенности протектором
			break;
		case "cave": 
			SetSoundScheme("cave");
			SetMusicAlarm("music_cave");
			bMonstersGen = false; //сбросить флаг монстров
			break;
		case "dungeon": 
			SetSoundScheme("dungeon");
			SetMusicAlarm("music_cave");
			bMonstersGen = false; //сбросить флаг монстров
			break;
		case "inca_temple": // не используется
			SetSoundScheme("inca_temple");
			SetMusicAlarm("music_coridor");
			break;
		case "house":
			SetSoundScheme("house");
			SetMusicAlarm("music_gorod");
			mchref.GenQuest.God_hit_us = true; // нефиг воровать :)
			break;
		case "tavern":
			SetSoundScheme("tavern");
			SetMusicAlarm("music_tavern");
			break;
		case "shop":
			SetSoundScheme("shop");
			SetMusicAlarm("music_shop");
			break;
		case "residence":
            SetSoundScheme("residence");
			if (CheckAttribute(loc,"fastreload"))   // boal
			{
			    iColony = FindColony(loc.fastreload);
			}
			if (iColony != -1)
			{
			    SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music");
			}
			else
			{
                SetMusicAlarm("music_gubernator");
			}
			break;
		case "church":
			if (isDay()) SetSoundScheme("church");
			SetMusicAlarm("music_church");
			break;
		case "shipyard":
			SetSoundScheme("shipyard");
			SetMusicAlarm("music_shipyard");
			break;
		case "fort_attack": // атакуем форт, внутренняя локация			
			SetSoundScheme("fort_attack");
			SetMusicAlarm("music_bitva");
			break;
		case "fort": // форт для мирных прогулок
			SetWeatherScheme("seashore");
			// музыка нации, филиал города
   			if (CheckAttribute(loc, "parent_colony"))
			{
			    iColony = FindColony(loc.parent_colony);
			}
			if (iColony != -1)
			{
			    SetMusicAlarm(NationShortName(sti(Colonies[iColony].nation)) + "_music");
			}
			else
			{
                SetMusicAlarm("music_gorod");
			}
			break;
		case "deck": // мирная палуба
			SetSoundScheme("deck");
			SetMusicAlarm("music_spokplavanie");
			break;
		case "deck_fight": // боевая полуба
			SetSoundScheme("deck_fight");
			SetMusicAlarm("music_spokplavanie");
			break;
		case "slave_deck": // квестовая палуба
			SetSoundScheme("slave_deck");
			//SetMusicAlarm("music_spokplavanie");
			break;
		case "boarding_cabine":
			SetSoundScheme("cabine");
			SetMusicAlarm("music_bitva");
			break;
		case "sailing_cabine":
			SetSoundScheme("sail_cabine");
			SetMusicAlarm("music_spokplavanie");
			break;
		case "incquisitio":
			SetSoundScheme("incquisitio");
			SetMusicAlarm("music_cave");
			break;
		case "jail":
			SetSoundScheme("jail");
			SetMusicAlarm("music_cave");
			break;
		case "LostShipsCity":
			SetWeatherScheme("LostShipsCity");
			SetMusicAlarm("music_spokplavanie");
			break;
		case "LSC_inside":
			SetWeatherScheme("LSC_inside");
			SetMusicAlarm("music_spokplavanie");
			break;
		case "underwater":
			//SetWeatherScheme("LSC_inside");
			SetMusicAlarm("music_underwater");
			break;
		}
	}
	SetStaticSounds(loc);
}

void SetStaticSounds (ref loc)
{
	if(IsEntity(loc) != true) return;
	string locatorGroupName = "locators.sound";
	if(CheckAttribute(loadedLocation, locatorGroupName) == 0) return;
	aref locatorGroup;
	makearef(locatorGroup, loc.(locatorGroupName));
	int locatorCount = GetAttributesNum(locatorGroup);
	if(locatorCount <= 0) return;
	string locatorName, locatorType;
	int locatorNameLength;

	for(int i = 0; i < locatorCount; i++)
	{
		aref locator = GetAttributeN(locatorGroup, i);
		locatorName = GetAttributeName(locator);
		locatorNameLength = strlen(locatorName);
		locatorType = strcut(locatorName, 0, locatorNameLength-3);
		if ((locatorType == "nightinsects")||(locatorType == "torch"))
		{
			if (Whr_IsDay() && loc.type != "Dungeon" && loc.type != "cave" && loc.type != "fort_attack") continue;
   		}
		if (locatorType == "shipyard")
		{
			if (Whr_IsNight()) continue;
   		}
		if (locatorType == "church")
		{
			if (Whr_IsNight()) continue;
   		}
		if (locatorType == "windmill")
		{
			continue;
   		}

		//trace("Create 3D Sound <"+locatorType+ "> for locator <"+locatorName+ "> into pos:("+locator.x+","+locator.y+","+locator.z+")" );
		SendMessage(Sound, "lsllllllfff", MSG_SOUND_PLAY, locatorType, SOUND_WAV_3D, VOLUME_FX, 0, 1, 0, 0, stf(locator.x), stf(locator.y), stf(locator.z));
	}
	
}

/*void SetSchemeForAbordage ()
{
	ResetSoundScheme();
	SetMusic("music_abordaz");
	ResumeAllSounds();
	int abordageSoundID = PlayStereoSoundLooped_JustCache("abordage");
	SendMessage(Sound,"lll",MSG_SOUND_RESUME, abordageSoundID, 0);
}*/

void SetSchemeForSea ()
{
	ResetSoundScheme();

	// AddSoundScheme("sea");
	if (Whr_IsNight())
		{
			if (Whr_IsStorm())
			{
				AddSoundScheme("sea_night_storm");
				SetMusic("music_storm");
			}
			else 
			{ 
				SetMusic("music_spokplavanie");
				if (Whr_IsRain())
				{
					AddSoundScheme("sea_night_rain");
				}
				else
				{
					AddSoundScheme("sea_night");
				}
			}
		}
	else // if Whr_IsDay
		{
			if (Whr_IsStorm())
			{
				AddSoundScheme("sea_day_storm");
				SetMusic("music_storm");
			}
			else 
			{ 
				SetMusic("music_spokplavanie");
				if (Whr_IsRain())
				{
					AddSoundScheme("sea_day_rain");
				}
				else
				{
					AddSoundScheme("sea_day");
				}
			}
		}
	ResumeAllSounds();
}

void SetSchemeForMap ()
{
	ResetSoundScheme();
	AddSoundScheme("sea_map");
	SetMusic("music_map");
	ResumeAllSounds();
	bFortCheckFlagYet = false; //eddy. уберем флаг распознавания фортом врага
}

// MUSIC
int musicID = -1;
int oldMusicID = -1;
int boardM = -1;
string musicName = "";
string oldMusicName = "";
void SetMusic(string name)
{
	//return;
	InitSound();
	//Trace("SETTING MUSIC: " + name);

	if (name == musicName)
	{
		SendMessage(Sound, "lll", MSG_SOUND_RESUME, musicID, SOUNDS_FADE_TIME);
		return;
	}

	//Trace("SetMusic : "+name);
	if (oldMusicID >= 0)
	{
		SendMessage(Sound, "ll", MSG_SOUND_RELEASE, oldMusicID);
		oldMusicID = -1;
	}

	if (musicID >= 0)
	{
		SendMessage(Sound, "lll", MSG_SOUND_STOP, musicID, MUSIC_CHANGE_TIME);
		oldMusicID = musicID;
	}

	//int silenceTime = 20000 + MakeInt(frnd() * MUSIC_SILENCE_TIME);
	//musicID = SendMessage(Sound, "lslllllll", MSG_SOUND_PLAY, name, SOUND_MP3_STEREO, VOLUME_MUSIC, true, true, false, MUSIC_CHANGE_TIME, silenceTime);
	//SendMessage(Sound, "llf", MSG_SOUND_SET_VOLUME, musicID, 1.0);
	// fix поседнее - это громкость звука
	musicID = SendMessage(Sound, "lslllllllf", MSG_SOUND_PLAY, name, SOUND_MP3_STEREO, VOLUME_MUSIC, true, true, false, 0, MUSIC_CHANGE_TIME, 1.0);
	SendMessage(Sound, "lll", MSG_SOUND_RESUME, musicID, MUSIC_CHANGE_TIME);
	
	oldMusicName = musicName;
	musicName = name;
}

void FadeOutMusic(int _time)
{
	if (musicID >= 0)
	{
		StopSound(musicID, _time);
		musicID = -1;
		musicName = "";
	}
}

// RELOAD
void PauseAllSounds()
{
	//Trace("PauseAllSounds");
	SendMessage(Sound,"lll",MSG_SOUND_STOP, 0, SOUNDS_FADE_TIME);
}

void ResumeAllSounds()
{
	//Trace("ResumeAllSounds");
	SendMessage(Sound,"lll",MSG_SOUND_RESUME, musicID, SOUNDS_FADE_TIME);
}

// OLD VERSIONS
int PlaySoundDist3D(string name, float x, float y, float z)
{
	return Play3DSound(name, x,y,z);
}

int PlaySoundLoc3D(string name, float x, float y, float z)
{
	return Play3DSoundCached(name, x,y,z);
}

int PlaySound3D(string name, float x, float y, float z)
{
	return Play3DSound(name, x,y,z);
}

void Sound_SetVolume(int iSoundID, float fVolume)
{
	SendMessage(Sound, "llf", MSG_SOUND_SET_VOLUME, iSoundID, fVolume);
}

int PlaySound(string name)
{
	return PlayStereoSound(name);
}

int PlaySoundComplex(string sSoundName, bool bSimpleCache, bool bLooped, bool bCached, int iFadeTime)
{
	return SendMessage(Sound,"lsllllll",MSG_SOUND_PLAY,VOLUME_FX,sSoundName,SOUND_WAV_3D,bSimpleCache,bLooped,bCached,iFadeTime);
}

void StopMusic()
{
}

void PlayMusic(string n)
{
}
//--------------------------------------------------------------------
// Sound Section
//--------------------------------------------------------------------
object Sound;

int alarmed = 0;
int oldAlarmed = 0;
bool seaAlarmed = false;
bool oldSeaAlarmed = false;
void SetMusicAlarm(string name)
{
	if (alarmed == 0)
	{
		SetMusic(name);
	}
	else
	{
		SetMusic("music_bitva");
		
		if (LAi_boarding_process != 0)
		{
			/*if (!CheckAttribute(loadedLocation, "CabinType") && !bBettaTestMode)//pchar.location != "CaptainCabine" && pchar.location != "CaptainCabineBig")
			{   // TO_DO добавил отключку в бетатест - разрушает мозг (выбивается звуком из фона)
				//Trace("Включить шума абордажа");
				abordageSoundID = PlayStereoSoundLooped("Abordage");
				boardM = 1;
			}*/
			if (!CheckAttribute(loadedLocation, "CabinType"))
			{
				boardM = 1; // потом сбросим звук и схему
			}
		}
	}
}

void Sound_OnAllarm()
{
	Sound_OnAlarm(GetEventData());
}

int abordageSoundID;

void Sound_OnAlarm(bool _alarmed)
{
	alarmed = _alarmed;
	if (alarmed == oldAlarmed)
		return;

	if (alarmed != 0)
	{ //alarm on!
		SetMusic("music_bitva");

	}
	else
	{ //alarm off
		SetMusic(oldMusicName);
		/*
		// данный звук отключаем когда открывается релоад в кабину капитана
		if (boardM == 1) 
			{
				Trace("Выключить шума абордажа");
				StopSound(abordageSoundID, 0);
				boardM = -1;
			}*/
	}
	oldAlarmed = alarmed;
}

void Sound_OnSeaAlarm(bool _seaAlarmed)
{
	seaAlarmed = _seaAlarmed;
	if (seaAlarmed == oldSeaAlarmed)
		return;

	if (seaAlarmed)
	{ //alarm on!
		SetMusic("music_sea_battle");
	}
	else
	{ //alarm off
		SetMusic(oldMusicName);
	}
	oldSeaAlarmed = seaAlarmed;
}

// set music without any checks
void Sound_OnSeaAlarm555(bool _seaAlarmed, bool bHardAlarm)
{
	if (bHardAlarm) { oldSeaAlarmed = !_seaAlarmed; }
	
	Sound_OnSeaAlarm(_seaAlarmed);
}

void InitSound()
{
	//Trace("InitSound");
	if (!IsEntity(&Sound))
	{
		CreateEntity(&Sound, "Sound");
		SetEventHandler("eventAllarm", "Sound_OnAllarm", 0);
	}
	//SendMessage(Sound,"lf",MSG_SOUND_SET_MASTER_VOLUME,1.0);
}

void ResetSound()
{
	// fix -->
	if (musicID > 0)
	{
		StopSound(musicID,0);
	}
	if (oldMusicID > 0)
	{
		StopSound(oldMusicID,0);
	}
	ResetSoundScheme();
	// fix <--
	StopSound(0,0);
	ReleaseSound(0);
	musicName = "";
	oldMusicName = "";
	musicID = -1;    //fix boal не было нуления ИД
    oldMusicID = -1;
}

void LoadSceneSound()
{
	int i = FindLoadedLocation();
	if (i != -1)
		SetSchemeForLocation(&Locations[i]);
}