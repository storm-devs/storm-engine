
#include "Loc_ai\LAi_defines.c"
#include "Loc_ai\LAi_events.c"
#include "Loc_ai\LAi_utils.c"
#include "Loc_ai\LAi_login.c"
#include "Loc_ai\LAi_character.c"
#include "Loc_ai\LAi_templates.c"
#include "Loc_ai\LAi_types.c"
#include "Loc_ai\LAi_groups.c"
#include "Loc_ai\LAi_fightparams.c"
#include "Loc_ai\LAi_boarding.c"
#include "Loc_ai\LAi_monsters.c"
#include "Loc_ai\LAi_location.c"
#include "Loc_ai\LAi_equip.c"
#include "Loc_ai\LAi_sounds.c"
#include "Loc_ai\LAi_utilites.c"


bool LAi_IsInitedAI = false;
bool LAi_IsCapturedLocation = false;
bool LAi_restoreStates = false;
bool LAi_IsBoarding = false;

//Инициализация интелекта
void LocAi_Init(ref loc)
{
	LAi_numloginedcharacters = 0;
	if(LAi_group_Init() == false) Trace("Error: Characters group logic not inited!!!");
	SetEventHandler("CharacterUpdate", "LAi_CharacterUpdate", 1);
	SetEventHandler("CharactersStateUpdate", "LAi_Character_FrameUpdate", 1);
	SetEventHandler("Location_CharacterEndTask", "LAi_CharacterEndTask", 0);
	SetEventHandler("Location_CharacterTaskFailure", "LAi_CharacterTaskFailure", 0);
	SetEventHandler("Location_CharacterBusyPos", "LAi_CharacterBusyPos", 0);
	SetEventHandler("Location_CharacterFollowGo", "LAi_CharacterFollowGo", 0);
	SetEventHandler("Location_CharacterFollowStay", "LAi_CharacterFollowStay", 0);
	SetEventHandler("Location_CharacterFightGo", "LAi_CharacterFightGo", 0);
	SetEventHandler("Location_CharacterFightStay", "LAi_CharacterFightStay", 0);
	SetEventHandler("Location_CharacterAttack", "LAi_CharacterAttack", 0);
	SetEventHandler("Location_CharacterFire", "LAi_CharacterFire", 0);
	SetEventHandler("Location_CharacterIsFire", "LAi_CharacterIsFire", 0);
	SetEventHandler("Location_CharacterIsFight", "LAi_CharacterIsFight", 0);
	SetEventHandler("Location_CharacterEscapeSlide", "LAi_CharacterEscapeSlide", 0);
	SetEventHandler("Location_CharacterColThreshold", "LAi_CharacterColThreshold", 0);
	SetEventHandler("Location_Character_EndAction", "LAi_Character_EndAction", 0);
	SetEventHandler("Location_CharacterItemAction", "LAi_CharacterItemAction", 0);
	LAi_IsInitedAI = true;
	LAi_IsCapturedLocation = IsLocationCaptured(loc.id);
	if(!actLoadFlag)
	{
		//Очистим фантомных персонажей
		for(int i = 0; i < MAX_CHARS_IN_LOC; i++)
		{
			DeleteAttribute(&Characters[LOC_FANTOM_CHARACTERS + i], "");
			Characters[LOC_FANTOM_CHARACTERS + i].index = LOC_FANTOM_CHARACTERS + i;
			Characters[LOC_FANTOM_CHARACTERS + i].id = "";
			Characters[LOC_FANTOM_CHARACTERS + i].location = "none";
			Characters[LOC_FANTOM_CHARACTERS + i].location.locator = "";
			Characters[LOC_FANTOM_CHARACTERS + i].location.group = "";
		}
	}
	//Выставим адреса офицерам главного персонажа
	bool isBoarding = false;
	if(CheckAttribute(loc, "boarding") == true)
	{
		if(loc.boarding == "true") isBoarding = true;
		if(loc.boarding == "fort") isBoarding = true; // boal 21.01.05 fix!!!
	}
	LAi_IsBoarding = isBoarding;
	if(!isBoarding)
	{
		int locIndex = 1;
		if(LAi_restoreStates)
		{
			LAi_SetCurHPMax(pchar);
			LAi_UseAtidoteBottle(pchar);
		}
		int idx;
		//проверяем таймер запрета генерации офицеров в локацию. eddy
		if (CheckAttribute(loc, "DisableOfficers.QtyDays") && GetNpcQuestPastDayParam(loc, "DisableOfficers") > sti(loc.DisableOfficers.QtyDays))
		{
			DeleteAttribute(loc, "DisableOfficers"); //если есть и истек срок - сносим флаг запрета
		}
		if (!CheckAttribute(loc, "DisableOfficers")) //eddy. офицерский флаг
		{
			for(i = 1; i < 4; i++)
			{
				idx = GetOfficersIndex(pchar, i);
				if(idx < 0) continue;
				if(LAi_restoreStates)
				{
					LAi_SetCurHPMax(&Characters[idx]);
					LAi_UseAtidoteBottle(&Characters[idx]);
				}

				if (Characters[idx].location == pchar.location)) continue; // fix
				DeleteAttribute(&Characters[idx], "location");
				Characters[idx].location = pchar.location;
				Characters[idx].location.group = "officers";
				Characters[idx].location.locator = pchar.location.locator + "_" + locIndex;

				if(CheckAttribute(loc, "locators.officers"))
				{
	    			locIndex = locIndex + 1;
				}else{
					Characters[idx].location = "none";
					Characters[idx].location.group = "";
					Characters[idx].location.locator = "";
				}
			}
		}
		else
		{
		    for(i = 1; i < 4; i++)
			{
				idx = GetOfficersIndex(pchar, i);
				if(idx < 0) continue;

				Characters[idx].location = "none";
				Characters[idx].location.group = "";
				Characters[idx].location.locator = "";
			}
		}
	}else{
		LAi_restoreStates = false;
	}
	LAi_IsCapturedLocation = IsLocationCaptured(loc.id);
}

//Инициализация интелекта
void LocAi_Release()
{
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			LAi_CharacterLogoff(&Characters[idx]);
		}
	}
	LAi_group_Release();
	LAi_IsInitedAI = false;
	LAi_numloginedcharacters = 0;
	DelEventHandler("CharacterUpdate", "LAi_CharacterUpdate");
	DelEventHandler("CharactersStateUpdate", "LAi_Character_FrameUpdate");
	DelEventHandler("Location_CharacterEndTask", "LAi_CharacterEndTask");
	DelEventHandler("Location_CharacterTaskFailure", "LAi_CharacterTaskFailure");
	DelEventHandler("Location_CharacterBusyPos", "LAi_CharacterBusyPos");
	DelEventHandler("Location_CharacterFollowGo", "LAi_CharacterFollowGo");
	DelEventHandler("Location_CharacterFollowStay", "LAi_CharacterFollowStay");
	DelEventHandler("Location_CharacterFightGo", "LAi_CharacterFightGo");
	DelEventHandler("Location_CharacterFightStay", "LAi_CharacterFightStay");
	DelEventHandler("Location_CharacterAttack", "LAi_CharacterAttack");
	DelEventHandler("Location_CharacterFire", "LAi_CharacterFire");
	DelEventHandler("Location_CharacterIsFire", "LAi_CharacterIsFire");
	DelEventHandler("Location_CharacterIsFight", "LAi_CharacterIsFight");
	DelEventHandler("Location_CharacterEscapeSlide", "LAi_CharacterEscapeSlide");
	DelEventHandler("Location_CharacterColThreshold", "LAi_CharacterColThreshold");
	DelEventHandler("Location_Character_EndAction", "LAi_Character_EndAction");	
	DelEventHandler("Location_CharacterItemAction", "LAi_CharacterItemAction");
	//Очистим фантомных персонажей
	for(i = 0; i < MAX_CHARS_IN_LOC; i++)
	{
		DeleteAttribute(&Characters[LOC_FANTOM_CHARACTERS + i], "");
		Characters[LOC_FANTOM_CHARACTERS + i].index = LOC_FANTOM_CHARACTERS + i;
		Characters[LOC_FANTOM_CHARACTERS + i].id = "";
		Characters[LOC_FANTOM_CHARACTERS + i].location = "none";
		Characters[LOC_FANTOM_CHARACTERS + i].location.locator = "";
		Characters[LOC_FANTOM_CHARACTERS + i].location.group = "";
		// убираем остатки захвата в городе boal
		LAi_LoginInCaptureTown(&Characters[LOC_FANTOM_CHARACTERS + i], false);
	}
}


void LocAi_PostInit(ref loc)
{
	if(!LAi_IsBoarding) LAi_CharacterPostLogin(loc);
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			LAi_PostLoginInit(&Characters[idx]);
		}
	}
	LAi_restoreStates = false;
}

//Сохранение информации
void LAi_SaveInfo()
{
	LAi_group_SaveInfo();
}

//Инициализация перед новой игрой
void LAi_NewGame()
{
	//Задержанные квесты
	DeleteAttribute(&Lai_questdelays, "");
	//Группы
	DeleteAttribute(&LAi_grp_relations, "");
	LAi_grp_playeralarm = 0;
	LAi_grp_alarmactive = false;
	//Абордаж
	boarding_location = -1;
	boarding_location_type = -1;
	boarding_enemy_crew = 0;
	boarding_player_crew = 0;
	boarding_officers = 0;
	boarding_player_crew_per_chr = 1.0;
	boarding_exp = 0;
	LAi_boarding_process = false;
	//Разное
	LAi_IsInitedAI = false;
	LAi_IsCapturedLocation = false;
	LAi_restoreStates = true;
	//DeleteAttribute(&LAi_MonsterInfo, "");
	LAi_numloginedcharacters = 0;
	DeleteAttribute(&LAi_QuestFader, "");
	globalSGMode = false;
	//Ставим тип игрока главному персонажу
	LAi_SetPlayerType(pchar);
}
