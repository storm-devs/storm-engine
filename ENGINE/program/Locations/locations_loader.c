

#include "Locations\particles.c"


#define EVENT_LOCATION_LOAD		"EventLoadLocation"
#define EVENT_LOCATION_UNLOAD	"EventUnloadLocation"

#define MAX_SHIPS_IN_LOCATION	32

ref loadedLocation;
object locWeather;
object locSea;
object locCamera;
object locShips[MAX_SHIPS_IN_LOCATION];

int	locNumShips = 0;

float locTmpTime = 0.0;
bool locDisableUpdateTime = false;


int FindLocation(string id)
{
	/*
	for(int i = 0; i < nLocationsNum; i++)
	{
		if(Locations[i].id == id)
		{
			return i;
		}
	}

	return -1;
	*/
	
	return NativeFindLocation(&locations, id);
	
}

void SetCamShuttle(ref loc) // boal вынес в метод
{
    if(CheckAttribute(loc, "camshuttle") == true)
	{
		if (CheckAttribute(pchar, "SystemInfo.ScaleSeaHeight"))
		{
			float sp = 0.95 - pow(0.91, stf(pchar.SystemInfo.ScaleSeaHeight));
			Log_TestInfo("Волна = "+pchar.SystemInfo.ScaleSeaHeight + "  качка = " + sp);
			//скорость, время, мин, макс, скорость наклона, макс наклон
			SendMessage(&LocCamera, "lffffff", -1, 0.5, -1.0, 1.0, 1.0, (0.07 * sp), (0.045 + sp/15.0)); // зависимость от волнения 14.09.06
		}
		else
		{
			//скорость, время, мин, макс, скорость наклона, макс наклон
			SendMessage(&LocCamera, "lffffff", -1, 0.5, -1.0, 1.0, 1.0, 0.05, 0.1);
		}
	}
	else
	{
		//скорость, время, мин, макс, скорость наклона, макс наклон
        SendMessage(&LocCamera, "lffffff", -1, 0.5, 1.0, 1.0, 1.0, 0.00, 0.0);
        //SendMessage(&LocCamera, "lffffff", -1, 0.0, 0, 0.0, 0.0, 0.00, 0.0);
	}
}

bool LoadLocation(ref loc)
{
	//trace("LoadLocation(ref loc) " + loc.id);
	PostEvent(EVENT_LOCATION_LOAD,0);

	int i;
	bool res;
	
	//Time update==========================================================================
	locTmpTime += 5;
	CalcLocalTime(3);
	//Environment==========================================================================
	chrWaitReloadLocator = ""; // fix перехода в таверну при клики на диалог
	chrWaitReloadIsNoLink = false;
		
    loadedLocation = loc; // первое присвоение boal
	//Environment==========================================================================

	if(CheckAttribute(loc, "type"))
	{
		res = true;
		switch(loc.type)
		{
		case "cave":			// пещера
			ExecuteTechnique("amb_cave");
			res = false;
		break;
		case "inca_temple":		// храм инков
			ExecuteTechnique("amb_inca_temple");
			res = false;
			break;
		case "house":			// дом
			ExecuteTechnique("amb_house");
			res = false;
			break;
		case "tavern":			// таверна
			ExecuteTechnique("amb_tavern");
			res = false;
			break;
		case "shop":			// магазин
			ExecuteTechnique("amb_shop");
			res = false;
			break;
		case "residence":		// резиденция
			ExecuteTechnique("amb_residence");
			res = false;
			break;
		case "church":			// церковь
			ExecuteTechnique("amb_church");
			res = false;
			break;
		case "fort_attack":			// тюрьма
			ExecuteTechnique("amb_jail");
			res = false;
			break;
		case "dungeon":
			ExecuteTechnique("amb_dungeon");
			res = false;
			break;
		}
		if(res)
		{
			ExecuteTechnique("amb_other");
		}
	}
	//
	
	if(CheckAttribute(loc, "QuestlockWeather"))
	{		
		//SetNextWeather(loc.QuestlockWeather);
		if (CheckAttribute(loc, "QuestlockWeather.hours") && CheckAttribute(loc, "QuestlockWeather.minutes"))
		{
			SetCurrentTime(sti(loc.QuestlockWeather.hours), sti(loc.QuestlockWeather.minutes));
			DeleteAttribute(loc, "QuestlockWeather.hours");
			DeleteAttribute(loc, "QuestlockWeather.minutes");
		}
	}

	
	bool isNoBoarding = true;
	bool isFort = false;
	if(CheckAttribute(loc, "boarding") == true)
	{
		if(loc.boarding == "true") isNoBoarding = false;
		if(loc.boarding == "fort")
		{
			isNoBoarding = false;
			isFort = true;
		}
	}
	if(isNoBoarding)
	{
		//Sea
		if(loc.environment.sea == "true") 
		{
			CreateSea("execute","realize");
			CreateCoastFoamEnvironment(loc.id, "execute", "realize");
		}
		//Weather
		if(loc.environment.weather == "true") CreateWeather("execute","realize");//CreateEntity(&locWeather, "weather");
		//Ship env
		CreateShipEnvironment();
	}
	if(isFort)
	{
		//Sea
		if(loc.environment.sea == "true") CreateSea("execute","realize");//CreateEntity(&locSea, "sea");
		//Weather
		if(loc.environment.weather == "true") CreateWeather("execute","realize");//CreateEntity(&locWeather, "weather");
	}

	ReloadProgressUpdate();
	//SendMessage(Sound,"lf",MSG_SOUND_SET_MASTER_VOLUME,0.0);

	//Create location======================================================================
	if(CreateEntity(&loc, "location") == 0) return 0;
	//Set models path
	if(CheckAttribute(loc, "filespath.models"))
	{		
		SendMessage(loc, "ls", MSG_LOCATION_MODELSPATH, loc.filespath.models);
	}
	//Set textures path
	if(CheckAttribute(loc, "filespath.textures"))
	{
		SendMessage(loc, "ls", MSG_LOCATION_TEXTURESPATH, loc.filespath.textures);
	}
	//Set lighting path
	SendMessage(loc, "ls", MSG_LOCATION_LIGHTPATH, GetLightingPath());
	SendMessage(loc, "ls", MSG_LOCATION_SHADOWPATH, GetLmLightingPath());
	//Loading always models================================================================
	aref st, at, lit, lit1;
	string sat;
	makearef(st, loc.models.always);
	int num = GetAttributesNum(st);
	for(i = 0; i < num; i++)
	{
		at = GetAttributeN(st, i);
		sat = GetAttributeName(at);
        if(sat != "grassPatch")
		{
			sat = "models.always." + sat;
			res = LocLoadModel(loc, sat, "");
			if(res == 0)
			{
				Trace("LocationLoader: not loaded model location." + sat + ", id = " + loc.id);
				Trace("Unload location.");
				UnloadLocation(loc);
				return 0;
			}
		}else{
			LocLoadGrass(loc, "models.always." + sat);
		}
	}
	//Loading background model=============================================================
	if(CheckAttribute(loc, "models.back") == true)
	{
		if(LocLoadModel(loc, "models.back", Whr_GetInsideBack()) == 0)
		{
			Trace("LocationLoader: not loaded back model location." + sat + ", id = " + loc.id);
		}		
	}

	ReloadProgressUpdate();

	//Loading day/night models=============================================================
	//--> проверяем наличие контры для установки нужного патча
	string smg = "";
	if (loc.type == "seashore" || loc.type == "mayak") 
	{
		makearef(st, loc.models.always);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);	
			sat = GetAttributeName(at);
			if(st.(sat) == "smg")
			{
				smg = "smg";
			}
		}			
	}
	//<-- проверяем наличие контры для установки нужного патча
	if(Whr_IsDay() != 0)
	{
		//Day
		makearef(st, loc.models.day);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			sat = GetAttributeName(at);
			if((sat != "charactersPatch") && (sat != "jumpPatch") && (sat != "lights"))
			{
				sat = "models.day." + sat;
				res = LocLoadModel(loc, sat, "");
				if(res == 0)
				{
                    Trace("LocationLoader: not loaded model location." + sat + ", id = " + loc.id);
					Trace("Unload location.");
					UnloadLocation(loc);
					return 0;
				}
			}
		}
		//Loading patches
		if(CheckAttribute(loc, "models.day.charactersPatch") != 0)
		{
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_CHRS_PATCH, loc.models.day.charactersPatch + smg);
			if(res == 0)
			{
				Trace("");
				Trace("\");
				Trace("Character patch not loaded!");
				Trace("/");
				Trace("");
				UnloadLocation(loc);
				return 0;
			}
		}else{
			Trace("");
			Trace("\");
			Trace("Character patch not setting for location.models.day.charactersPatch!");
			Trace("/");
			Trace("");
			UnloadLocation(loc);
			return 0;
		}
		if(CheckAttribute(loc, "models.day.jumpPatch") != 0)
		{
			SendMessage(loc, "ls", MSG_LOCATION_SET_JMP_PATCH, loc.models.day.jumpPatch);
		}
	}else{
		//Night
		makearef(st, loc.models.night);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			sat = GetAttributeName(at);
			if((sat != "charactersPatch") && (sat != "jumpPatch") && (sat != "lights"))
			{
				sat = "models.night." + sat;
				res = LocLoadModel(loc, sat, "");
				if(res == 0)
				{
                    Trace("LocationLoader: not loaded model location." + sat + ", id = " + loc.id);
					Trace("Unload location.");
					UnloadLocation(loc);
					return 0;
				}
			}
		}
		
		//Loading patches
		if(CheckAttribute(loc, "models.night.charactersPatch") != 0)
		{			
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_CHRS_PATCH, loc.models.night.charactersPatch + smg);
			if(res == 0)
			{
				Trace("");
				Trace("\");
				Trace("Character patch not loaded!");
				Trace("/");
				Trace("");
				UnloadLocation(loc);
				return 0;
			}
		}else{
			Trace("");
			Trace("\");
			Trace("Character patch not setting for location.models.night.charactersPatch!");
			Trace("/");
			Trace("");
			UnloadLocation(loc);
			return 0;
		}
		if(CheckAttribute(loc, "models.night.jumpPatch") != 0)
		{
			SendMessage(loc, "ls", MSG_LOCATION_SET_JMP_PATCH, loc.models.night.jumpPatch);
		}


	}
	//Entry models=========================================================================
	if(LocIsEntryLocation(loc) == true)
	{
		makearef(st, loc.models.entry);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			sat = "models.entry." + GetAttributeName(at);
			res = LocLoadModel(loc, sat, "");
			if(res == 0)
			{
                Trace("LocationLoader: not loaded model location." + sat + ", id = " + loc.id);
				Trace("Unload location.");
				UnloadLocation(loc);
				return 0;
			}
		}
	}

	ReloadProgressUpdate();

	//Locators=============================================================================
	Sea.MaxSeaHeight = 0.5;
	if(CheckAttribute(loc, "MaxWaveHeigh"))
	{
		if(stf(loc.MaxWaveHeigh) > 0.0)
		{
			Sea.MaxSeaHeight = stf(loc.MaxWaveHeigh);
		}
	}
	//Locator's radiuses
	int j, k, gnum, lnum;
	aref locator_rad;
	if(CheckAttribute(loc, "locators_radius") != 0)
	{
		makearef(locator_rad, loc.locators_radius);
		gnum = GetAttributesNum(locator_rad);
		for(j = 0; j < gnum; j++)
		{
			//Group radius
			aref rdgrp = GetAttributeN(locator_rad, j);
			string rdgname = GetAttributeName(rdgrp);			
			float rad = MakeFloat("" + rdgrp);
			SetLocatorGroupRadius(loc, rdgname, rad);
			//Some locators radius
			lnum = GetAttributesNum(rdgrp);
			for(k = 0; k < lnum; k++)
			{
				aref rdloc = GetAttributeN(rdgrp, k);
				string rdlname = GetAttributeName(rdloc);
				rad = MakeFloat("" + rdloc);
				SetLocatorRadius(loc, rdgname, rdlname, rad);
			}
		}
	}
	//Updatelocators=======================================================================
	SendMessage(loc, "l", MSG_LOCATION_UPDATELOCATORS);
	loadedLocation = loc;

	ReloadProgressUpdate();

	//Lights===============================================================================
	LocationSetLights(loc);
	//Particles============================================================================
	CreateParticles(loc);
	//Ships================================================================================
	if (isNoBoarding && loc.environment.sea == "true") LocLoadShips(loc); // boal fix!!!!!!!!
	//Animals===========================================================================
	CreateAnimals(loc);
	//Characters===========================================================================
	LoginCharactersInLocation(loc);
	//CheckEnemyLocators(loc);
	//Main character options===============================================================
	ref mainCharacter = GetMainCharacter();
	if(IsEntity(mainCharacter) == 0)
	{
		Trace("Main character not loaded!");
		UnloadLocation(loc);
		return 0;		
	}
	AddCharacterLocatorGroup(mainCharacter, "reload");
	AddCharacterLocatorGroup(mainCharacter, "camdetector");
	AddCharacterLocatorGroup(mainCharacter, "item");
	AddCharacterLocatorGroup(mainCharacter, "randitem");
	AddCharacterLocatorGroup(mainCharacter, "box");
	AddCharacterLocatorGroup(mainCharacter, "teleport");

	ReloadProgressUpdate();

 	//Camera===============================================================================
	CreateEntity(&locCamera, "locationcamera");
	SendMessage(&locCamera, "li", MSG_CAMERA_SETTARGET, mainCharacter);
	locCameraFollow();
	pchar.CameraHoldPos = false; //boal
	if(CheckAttribute(loc, "lockCamAngle") == true)
	{
		float lockCamAngle = stf(loc.lockCamAngle);
		if(lockCamAngle < -1.5) lockCamAngle = -1.5;
		if(lockCamAngle > 1.5) lockCamAngle = 1.5;
		SendMessage(&locCamera, "lf", MSG_CAMERA_MOVE, lockCamAngle);
	}	
    SetCamShuttle(loc);

	SetEventHandler("Control Activation","locCameraSwitch",1);
	/*if(isNoBoarding) мешало релоду на абордаже и каюте*/ SetEventHandler("Control Activation","chrCharacterKeys",1);


	bool crtAnimals;

	if(CheckAttribute(loc, "type"))
	{
		if(loc.type == "jungle")
		{
			//Орёл
			if(rand(100) < 80)
			{
				crtAnimals = true;
				if(!IsDay()) crtAnimals = false;
				if(Whr_GetWindSpeed() > 10.0) crtAnimals = false;
				if(Whr_IsRain()) crtAnimals = false;
				if(Whr_IsStorm()) crtAnimals = false;
				if(Whr_GetFogDensity() > 0.008) crtAnimals = false;
				if(crtAnimals)
				{
					SendMessage(loc, "ls", MSG_LOCATION_EX_MSG, "AddEagle");
				}
			}
			//Ящерецы
			if(rand(100) < 95)
			{
				crtAnimals = true;
				if(Whr_IsStorm()) crtAnimals = false;
				if(Whr_GetFogDensity() > 0.008) crtAnimals = false;
				if(Whr_GetWindSpeed() > 10.0) crtAnimals = false;
				if(crtAnimals)
				{
					SendMessage(loc, "ls", MSG_LOCATION_EX_MSG, "AddLizards");
				}
			}
			//Крысы
			if(rand(100) < 50)
			{
				if(IsDay())
				{
					if(rand(100) < 50)
					{
						SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 1 + rand(2));
					}
				}else{
					SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 1 + rand(4));
				}
			}
		}
		if(loc.type == "Dungeon")
		{
			//Крысы
			if(rand(100) < 95)
			{
				SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 16 + rand(16));
			}
		}
		if(loc.type == "jail")
		{
			//Крысы
			if(rand(100) < 75)
			{
				SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 1 + rand(3));
			}
		}
		if(loc.type == "town")
		{
			//Крысы
			if(rand(100) < 35)
			{
				crtAnimals = true;
				if(Whr_IsStorm()) crtAnimals = false;
				if(Whr_GetFogDensity() > 0.008) crtAnimals = false;
				if(Whr_GetWindSpeed() > 10.0) crtAnimals = false;
				if(crtAnimals)
				{
					if(IsDay())
					{
						SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 1 + rand(2));
					}else{
						SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 2 + rand(4));
					}
				}
			}
		}
		// boal -->
        if (loc.id == "My_Deck_Medium" || loc.id == "My_Deck")
        {
            SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "AddRats", 2 + rand(3));
        }
        // boal <--
	}

    // boal debug info -->
	if (bLocatorShow)
	{
        ShowAllLocators();
    }
    // boal debug info <--
    
	//DumpAttributes(loc);
	//SendMessage(Sound,"lf",MSG_SOUND_SET_MASTER_VOLUME,1.0);
	
	//VisibleLocatorsGroup("torchlightes_o", 0.1, 15.0, 55, 0, 0, 255);
	/*
	VisibleLocatorsGroup("candles", 1.0, 15.0, 255, 255, 255, 55);
	VisibleLocatorsGroup("candles_medium", 1.0, 15.0, 255, 255, 255, 55);
	VisibleLocatorsGroup("chandeliers", 1.0, 15.0, 255, 128, 0, 128);
	VisibleLocatorsGroup("torchlightes", 1.0, 15.0, 255, 255, 155, 0);
	VisibleLocatorsGroup("outside", 1.0, 15.0, 255, 155, 155, 255);	
	VisibleLocatorsGroup("fonar", 1.0, 15.0, 255, 155, 255, 155);
	VisibleLocatorsGroup("heaters", 1.0, 15.0, 255, 155, 255, 155);
	VisibleLocatorsGroup("incas_light", 1.0, 15.0, 255, 155, 255, 255);
	VisibleLocatorsGroup("incas_sky", 1.0, 15.0, 255, 0, 255, 255);
	*/
	
	Item_OnLoadLocation(loc.id);

	ReloadProgressUpdate();

	//ResumeAllSounds();
	//SetSchemeForLocation(loc);

	if (isNoBoarding) { Island_Start(); }

	if(CheckAttribute(&mainCharacter, "lastFightMode") != 0)
	{
		if(sti(mainCharacter.lastFightMode) != 0)
		{
			if(CheckAttribute(loc, "noFight") && loc.noFight != "1") //eddy. для переходов в режиме боя, чтобы не було эррорлога. счас только при читовых телепортах вылазит, сам смотри.
			{
				SendMessage(&mainCharacter, "lsl", MSG_CHARACTER_EX_MSG, "SetFightMode", 1);
			}
		}
	}
	if (loc.type == "residence") //eddy. в резиденции нельзя дебоширить
	{
		if (!IsLocationCaptured(loc.id) && !CheckAttribute(loc, "boarding"))// захват города
		{
			LAi_LockFightMode(Pchar, true);
		}
	}

	// ADDED BY VANO
	// сбрасываем цвет фона на 0
	Render.BackColor = 0;
	// выключаем эффект моря
	Render.SeaEffect = false;
	// включаем плавание в локации
	loc.Swimming = true;
	// 14.07.2007 - отключаем подводную часть
	Sea.UnderWater = false;

	// если мы под водой
	if(CheckAttribute(loc, "underwater") != 0)
	{
		if (sti(loc.underwater))
		{
			aref arWeather = GetCurrentWeather();

			// отключаем плавание - персонаж ходит в воде
			loc.Swimming = false;

			// включаем эффект воды
			Render.SeaEffect = true;
			// я не помню, тут 0.02 или 0.002 должно быть, если сильно колбасит - то надо уменьшить :)
			Render.SeaEffectSize = 0.002;
			Render.SeaEffectSpeed = 3.0;
			// ставим цвет фона равным цвету тумана
			Render.BackColor = arWeather.Fog.Color;

			// 14.07.2007 - включаем подводную часть в море
			Sea.UnderWater = true;

			Loc.Caustic.scale = 1.0;
			Loc.Caustic.fogdensity = 0.001;
			Loc.Caustic.color = argb(128, 255, 255, 255);
			Loc.Caustic.speed = 0.3;
			
			SendMessage(loc, "ls", MSG_LOCATION_EX_MSG, "LoadCaustic");
			SendMessage(loc, "lsl", MSG_LOCATION_EX_MSG, "EnableCaustic", 1);
		}
	}
	// ADDED BY VANO

	return 1;
}

void LocationSetLights(ref loc)
{
	SendMessage(loc, "ls", MSG_LOCATION_EX_MSG, "DelAllLights");

	string lightPath,lightName,lightGroupName;
	string sat;
	aref st,at,lit,lit1;
	int i,num, lnum,j;

	if(Whr_IsLight() == 0)
	{
		lightPath = "models.day.lights";
	}else{
		lightPath = "models.night.lights";
	}

	if(CheckAttribute(loc, lightPath) != 0)
	{
		makearef(st, loc.(lightPath));
		num = GetAttributesNum(st);
		//Trace("numLights = " + num);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			lightGroupName = GetAttributeName(at);
			sat = lightPath + "." + lightGroupName;
			lightName = loc.(sat);
			sat = "locators." + lightGroupName;
			if(CheckAttribute(loc, sat) != 0)
			{
				makearef(lit, loc.(sat));
				lnum = GetAttributesNum(lit);					
				for(j = 0; j < lnum; j++)
				{
					lit1 = GetAttributeN(lit, j);
					float litX = stf(lit1.x);
					float litY = stf(lit1.y);
					float litZ = stf(lit1.z);
					//Trace("     AddLight: " + lightName + " (" + litX + ", " + litY + ", " + litZ);
					SendMessage(loc, "lsfff", MSG_LOCATION_ADD_LIGHT, lightName, litX, litY, litZ);
					if(lightName == "lamp")
					{
						SendMessage(loc, "lsfff", MSG_LOCATION_EX_MSG, "AddFlys", litX, litY, litZ);
					}
				}
			}
		}
	}
}

bool UnloadLocation(aref loc)
{
	// ADDED BY VANO
	// сбрасываем цвет фона на 0
	Render.BackColor = 0;
	// выключаем эффект моря
	Render.SeaEffect = false;
	// ADDED BY VANO
	// 14.07.2007 - отключаем подводную часть
	Sea.UnderWater = false;

	//trace("UnloadLocation(aref loc) " + loc.id);
	DialogExit();

	ref mainCharacter = GetMainCharacter();
	if(SendMessage(&mainCharacter, "ls", MSG_CHARACTER_EX_MSG, "IsFightMode") != 0)
	{
		mainCharacter.lastFightMode = "1";
	}else{
		mainCharacter.lastFightMode = "0";
	}

	Event(EVENT_LOCATION_UNLOAD,"");

	int n;

	DeleteCoastFoamEnvironment();

	bool isNoBoarding = true;
	bool isFort = false;
	if(CheckAttribute(loc, "boarding") == true)
	{
		if(loc.boarding == "true") isNoBoarding = false;
		if(loc.boarding == "fort")
		{
			isNoBoarding = false;
			isFort = true;
		}
	}
	if(isNoBoarding)
	{
		DeleteParticles();
		//DeleteClass(&Island);
		DeleteAnimals();
		DeleteSea();
		DeleteWeather();
		DeleteShipEnvironment(); 
		for(n = 0; n < locNumShips; n++) {DeleteClass(locShips[n]);}
		locNumShips = 0;
	}
	else
	{
        //DeleteParticles();  // фикс костров
		SendMessage(&Particles,"l", PS_CLEAR_CAPTURED);
	}
	if(isFort)
	{
		DeleteSea();
		DeleteWeather();
	}

	DelEventHandler("Control Activation","locCameraSwitch");
	/*if(isNoBoarding) fix 31.03.05 */ DelEventHandler("Control Activation","chrCharacterKeys");
	Item_OnUnLoadLocation();
	LocAi_Release();
	DeleteClass(&loc);
	DeleteClass(&locCamera);
	return true;
}

bool LocLoadModel(aref loc, string sat, string addition)
{
	//Trace("Load model: " + loc.(sat) + " from:" + sat);

	//Пропустим пустое имя
	if(loc.(sat) == "") return true;
	//Считываем параметры модельки
	string attr, attr1;
	bool res;
	string tech = "";
	int level = 10;
	attr = sat + ".tech";
	if(CheckAttribute(loc, attr)) tech = loc.(attr);
    attr = sat + ".level";
    if(CheckAttribute(loc, attr)) level = MakeInt(loc.(attr));
    attr = sat + ".lights";
    int dynamicLightsOn = 0;
    if(CheckAttribute(loc, attr)) dynamicLightsOn = MakeInt(loc.(attr));
    //Грузим модельку
    res = SendMessage(loc, "lssll", MSG_LOCATION_ADD_MODEL, loc.(sat) + addition, tech, level, dynamicLightsOn);
	if(res == 0) return 0;
	//Устанавливаем флаги
	object mdl;
	if(SendMessage(loc, "le", MSG_LOCATION_GET_MODEL, &mdl) != 0)
	{
		string sAreal = GiveArealByLocation(loc);
		if (sAreal != "none")
		{
			string sCity = GetCityNameByIsland(sAreal);
			if (sCity != "none")
			{
				SendMessage(&Flag, "lil", MSG_FLAG_INIT, &mdl, GetCityNation(sCity));
			}
		}
	}
	// Проверяем на пену
	attr = sat + ".foam";
	if(CheckAttribute(loc, attr) != 0)
	{
		if (loc.(attr) == "1")
		{
			/*if (!isEntity(&Island))
			{
				CreateEntity(&Island, "Island");
				LayerAddObject("realize", &Island, 65529);
			}

			SendMessage(&Island, "liss", MSG_LOCATION_ADD_MODEL, &mdl, loc.id, loc.filespath.models);*/
			//Trace("<> Model " + attr + ", have a foam!!!");
		}
	}
	//Ставим модификаторы
	//locator
	attr = sat + ".locator.group";
	//Trace("> " + attr);
	if(CheckAttribute(loc, attr) != 0)
	{
		res = 0;
		attr1 = sat + ".locator.name";
		//Trace("> " + attr1);
		if(CheckAttribute(loc, attr1) != 0)
		{
			res = SendMessage(loc, "lss", MSG_LOCATION_MODEL_SET_POS, loc.(attr), loc.(attr1));
		}
		if(res == 0) 
		{
			Trace("Can't set locator modifier to model: " + loc.(sat));
		}
	}
	//rotate
	attr = sat + ".rotate";
	if(CheckAttribute(loc, attr) != 0)
	{
		float x, y, z;
		attr1 = attr + ".x";
		if(CheckAttribute(loc, attr1) != 0) x = MakeFloat(loc.(attr1));
		attr1 = attr + ".y";
		if(CheckAttribute(loc, attr1) != 0) y = MakeFloat(loc.(attr1));
		attr1 = attr + ".z";
		if(CheckAttribute(loc, attr1) != 0) z = MakeFloat(loc.(attr1));
		res = SendMessage(loc, "lfff", MSG_LOCATION_MODEL_SET_ROT, x, y, z);
		if(res == 0) Trace("Can't set rotate modifier to model: " + loc.(sat));
	}	
	//uvslide
	attr = sat + ".uvslide";
	if(CheckAttribute(loc, attr) != 0)
	{
		float u0, v0, u1, v1;
		u0 = 0; v0 = 0; u1 = 0; v1 = 0;
		attr1 = attr + ".u0";
		if(CheckAttribute(loc, attr1) != 0) u0 = MakeFloat(loc.(attr1));
		attr1 = attr + ".v0";
		if(CheckAttribute(loc, attr1) != 0) v0 = MakeFloat(loc.(attr1));
		attr1 = attr + ".u1";
		if(CheckAttribute(loc, attr1) != 0) u1 = MakeFloat(loc.(attr1));
		attr1 = attr + ".v1";
		if(CheckAttribute(loc, attr1) != 0) v1 = MakeFloat(loc.(attr1));
		res = SendMessage(loc, "lffff", MSG_LOCATION_MODEL_SET_UVS, u0, v0, u1, v1);
		if(res == 0) Trace("Can't set uvslide modifier to model: " + loc.(sat));
	}
	//rotate
	attr = sat + ".lamps";
	if(CheckAttribute(loc, attr) != 0)
	{
		if(loc.(attr) != "0")
		{
			SendMessage(loc, "l", MSG_LOCATION_MODEL_LAMPS);
		}
	}
	//reflection
	attr = sat + ".reflection";
	if(CheckAttribute(loc, attr) != 0)
	{
		SendMessage(loc, "lf", MSG_LOCATION_MODEL_REFLECTION, stf(loc.(attr)));
	}
	//reflection
	attr = sat + ".sea_reflection";
	if(CheckAttribute(loc, attr) != 0)
	{
		LayerAddObject("sea_reflection2", &mdl, 4);
	}
	return 1;
}

float GetAngleY(float x, float z)
{
	return atan2(x,z);
}

void LocLoadShips(ref Location)
{
	int		i, j;
	int		iShips[MAX_SHIPS_IN_LOCATION];
	int		iShipsType[MAX_SHIPS_IN_LOCATION];
	int		iShipType, iMainCharacterShipType;
	aref	locator_ships,locator_otherships;
	aref	locator;

	locNumShips = 0;

	bool bMainCharacterHere = LocIsEntryLocation(Location);

	ref rPlayer = GetMainCharacter();
	
	if (!CheckAttribute(Location, "locators")) 
	{
		//Trace("LocLoadShips: Can't find Location.locators in location: " + Location.id);
		return;
	}

	int iMCI = GetMainCharacterIndex();

	if (!CheckAttribute(Location, "locators.ships"))
	{
		Trace("LocLoadShips: Can't find Location.locators.ships in location: " + Location.id); return;
	}
	if (!CheckAttribute(Location, "locators.ships_other"))
	{
		Trace("LocLoadShips: Can't find Location.locators.ships_other in location: " + Location.id); return;
	}
	makearef(locator_ships, location.locators.ships);
	makearef(locator_otherships, location.locators.ships_other);

	iMainCharacterShipType = sti(Characters[iMCI].Ship.Type);

	// create our characters ships
	if (bMainCharacterHere)
	{
		for (i=0;i<COMPANION_MAX;i++)
		{
			if (locNumShips >= MAX_SHIPS_IN_LOCATION) continue;
			int iCompanionIndex = GetCompanionIndex(&Characters[iMCI],i);
			if (iCompanionIndex==-1) continue;
			SetCharacterShipLocation(&Characters[iCompanionIndex],Location.id);
			iShipType = sti(Characters[iCompanionIndex].Ship.Type);
			if (iShipType == SHIP_NOTUSED) continue;
			iShips[locNumShips] = iCompanionIndex;
			iShipsType[locNumShips] = 0;
			locNumShips++;
			Characters[iCompanionIndex].Ship.Stopped = true; // boal fix
		}
	}

	// create other quest characters ships
	for (i=1;i<MAX_CHARACTERS;i++)  // начало не с 0
	{
		if (i == iMCI) continue;
		if (locNumShips >= MAX_SHIPS_IN_LOCATION) continue;
		if (!Character_LocIsEntryLocation(&Characters[i],Location)) continue;
		bool bExist = false;
		for (j=0;j<locNumShips;j++) 
		{
			if (i == iShips[j]) 
			{
				bExist = true; break;
			}
		}
		if (bExist) continue;
		iShipType = sti(Characters[i].Ship.Type);
		if (iShipType == SHIP_NOTUSED) continue;
		iShips[locNumShips] = i; locNumShips++;
		iShipsType[locNumShips] = 1;
		Characters[i].Ship.Stopped = true; // boal fix
	}

	int iNumShips = GetAttributesNum(locator_ships);
	int iNumOtherShips = GetAttributesNum(locator_otherships);
	int iCurNumShips = 0;
	int iCurNumOtherShips = 0;
	for(int n = 0; n < locNumShips; n++)
	{
		bool bContinue = false;
		switch(iShipsType[locNumShips])
		{
			case 0:
				if (iCurNumShips >= iNumShips) 
				{ 
					Trace("LocLoadShips: need (ships): " + iCurNumShips + ", but max_num_ships: " + iNumShips);
					bContinue = true; 
				}
				locator = GetAttributeN(locator_ships, iCurNumShips);
				iCurNumShips++;
			break;
			case 1:
				if (iCurNumOtherShips >= iNumOtherShips) 
				{ 
					Trace("LocLoadShips: need (ships_other): " + iCurNumOtherShips + ", but max_num_ships_other: " + iNumOtherShips);
					bContinue = true; 
				}
				locator = GetAttributeN(locator_otherships, iCurNumOtherShips);
				iCurNumOtherShips++;
			break;
		}
		if (bContinue) continue;
		if (CreateEntity(&locShips[n], "ship") == 0) break;
		//if (iShips[n] == 0) Log_info("Bad index");
		ref rCharacter = GetCharacter(iShips[n]);
		ref rShip = GetRealShip(sti(rCharacter.Ship.Type));
		//SetTextureForShip(rShip, rCharacter);
		Ship_SetLightsAndFlares(rCharacter);
		//rCharacter.Ship.TexturePath = "";
		rCharacter.Ship.Pos.x = stf(locator.x);
		rCharacter.Ship.Pos.z = stf(locator.z);
		rCharacter.Ship.Ang.y = GetAngleY(stf(locator.vZ.x),stf(locator.vZ.z));
		rCharacter.Ship.stopped = true;
		rCharacter.Ship.Speed.z = 0.0;
		Ship_PrepareShipForLocation(rCharacter);
		SendMessage(&locShips[n],"laa",MSG_SHIP_CREATE,&rCharacter,&rShip);
	}

	// load boat 2 location
	if (CheckAttribute(Location, "locators.reload.boat"))
	{
		if (bMainCharacterHere && iMainCharacterShipType != SHIP_NOTUSED)
		{
			if (CreateEntity(&locShips[n], "ship"))
			{
				makearef(locator,Location.locators.reload.boat);

				rCharacter = GetCharacter(BOAT_CHARACTER); // boal
				rShip = GetRealShip(sti(rCharacter.Ship.Type));
				rCharacter.Ship.TexturePath = "";
				rCharacter.Ship.Pos.x = stf(locator.x);
				rCharacter.Ship.Pos.z = stf(locator.z);
				rCharacter.Ship.Ang.y = GetAngleY(stf(locator.vZ.x),stf(locator.vZ.z));
				rCharacter.Ship.stopped = true;
				rCharacter.Ship.Speed.z = 0.0;
				Ship_SetLightsAndFlares(rCharacter);
				Ship_PrepareShipForLocation(rCharacter);
				SendMessage(&locShips[n],"laa",MSG_SHIP_CREATE,&rCharacter,&rShip);
				locNumShips++;
			}
		}
	}
	else 
	{
		if (bMainCharacterHere)
		{
			Trace("LocLoadShips: Can't find boat locator in location: " + Location.id);
		}
	}
	
}

bool Character_LocIsEntryLocation(ref rCharacter, ref rLocation)
{
	if (CheckAttribute(rCharacter,"location.from_sea") && !LAi_IsDead(rCharacter))  // boal superfix 26/05/06
	{
		if (rCharacter.location.from_sea == rLocation.id) return true;
	}
	return false;
}

bool LocIsEntryLocation(ref Location)
{
	ref rPlayer = GetMainCharacter();
	//Trace("LocIsEntryLocation = " + rPlayer.location.from_sea);
	if(rPlayer.location.from_sea != Location.id) return false;
	return true;
}

bool LocLoadGrass(aref loc, string sat)
{
	string grs = loc.(sat);
	string tex = "";
	sat = sat + ".texture";
	if(CheckAttribute(loc, sat) != 0)
	{
		tex = loc.(sat);
	}
	return SendMessage(loc, "lss", MSG_LOCATION_SET_GRS_PATCH, grs, tex);
}


#event_handler("LocationTimeUpdate", "LocationTimeUpdateFunc");
void LocationTimeUpdateFunc()
{
	float dltTime = GetEventData();
	if(locDisableUpdateTime) return;
	locTmpTime = locTmpTime + dltTime;
	//if(locTmpTime > 100000.0) locTmpTime = 100000.0;
	// boal -->
	if (locTmpTime > 9)//14)
	{
		/*if (LAi_IsFightMode(GetMainCharacter()) && LAi_group_IsActivePlayerAlarm())
		{
			if (locTmpTime > 29)
			{
				CalcLocalTime(6);
				RefreshLandTime();
			}
		}
		else
		{ */
		    CalcLocalTime(3);
		    RefreshLandTime();
		//}
	}
	// boal <--
}

void CheckAndDisableLocator(ref loc, string locatorname, string checkdestination, bool bDisable)
{
	string sLocTo = "";
	if( CheckAttribute(loc,locatorname) ) {
		sLocTo = loc.fastreload + checkdestination;
		if( loc.(locatorname).go == sLocTo ) {
			if( CheckAttribute(loc,locatorname+".storyloc_disable") ) {
				loc.(locatorname).disable = loc.(locatorname).storyloc_disable;
			} else {
				loc.(locatorname).disable = bDisable;
			}
		}
	}
}

void StoryLineEnableLocator(string locID, string locname, bool bEnable)
{
	int nLoc = FindLocation(locID);
	if( nLoc >= 0 ) {
		if( CheckAttribute(&Locations[nLoc],"reload."+locname) ) {
			chrEnableReloadLocator(locID,Locations[nLoc].reload.(locname).name,bEnable);
		}
		if( CheckAttribute(&Locations[nLoc],"reload."+locname+".disable") ) {
			Locations[nLoc].reload.(locname).storyloc_disable = Locations[nLoc].reload.(locname).disable;
		} else {
			DeleteAttribute(&Locations[nLoc],"reload."+locname+".storyloc_disable");
		}
	}
}

void ShowAllLocators()
{
    VisibleLocatorsGroup("rld", 1.0, 15.0, 255, 128, 200, 120);
    VisibleLocatorsGroup("reload", 1.0, 15.0, 255, 0, 255, 0);
    VisibleLocatorsGroup("goto", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("sit", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("item", 1.0, 15.0, 255, 255, 0, 255);
    VisibleLocatorsGroup("randitem", 1.0, 15.0, 255, 255, 0, 255);
    VisibleLocatorsGroup("characters", 1.0, 15.0, 155, 255, 0, 0);
    VisibleLocatorsGroup("Merchant", 1.0, 15.0, 105, 0, 255, 125);
    VisibleLocatorsGroup("box", 1.0, 15.0, 255, 0, 255, 255);
    VisibleLocatorsGroup("encdetector", 1.0, 15.0, 255, 0, 255, 255);
    VisibleLocatorsGroup("outside", 1.0, 15.0, 255, 155, 155, 255);
    VisibleLocatorsGroup("officers", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("waitress", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("barmen", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("teleport", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("magsteleport", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("greenteleport", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("redteleport", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("blueteleport", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("monsters", 1.0, 15.0, 255, 255, 200, 0);
    VisibleLocatorsGroup("Smugglers", 1.0, 15.0, 255, 255, 0, 0);
    VisibleLocatorsGroup("camera", 1.0, 15.0, 155, 0, 255, 255);
    VisibleLocatorsGroup("soldiers", 1.0, 15.0, 255, 255, 100, 200);
    VisibleLocatorsGroup("quest", 1.0, 15.0, 255, 255, 200, 200);
    VisibleLocatorsGroup("patrol", 1.0, 15.0, 255, 255, 100, 200);
    VisibleLocatorsGroup("tables", 1.0, 15.0, 255, 255, 0, 0);
}

void HideAllLocators()
{
    HideLocatorsGroup("rld");
    HideLocatorsGroup("reload");
    HideLocatorsGroup("goto");
    
    HideLocatorsGroup("sit");
    HideLocatorsGroup("item");
    HideLocatorsGroup("randitem");
    HideLocatorsGroup("characters");
    HideLocatorsGroup("Merchant");
    HideLocatorsGroup("box");
    HideLocatorsGroup("encdetector");
    HideLocatorsGroup("outside");
    HideLocatorsGroup("officers");
    HideLocatorsGroup("waitress");
    HideLocatorsGroup("barmen");
    HideLocatorsGroup("teleport");
    HideLocatorsGroup("magsteleport");
    HideLocatorsGroup("greenteleport");
    HideLocatorsGroup("redteleport");
    HideLocatorsGroup("blueteleport");
    HideLocatorsGroup("monsters");
    HideLocatorsGroup("Smugglers");
    HideLocatorsGroup("camera");
    HideLocatorsGroup("soldiers");
    HideLocatorsGroup("quest");
    HideLocatorsGroup("patrol");
    HideLocatorsGroup("tables");
}