#include "scripts\colony.c"
#include "scripts\food.c"
#include "scripts\time_events.c"
#include "scripts\Rumour_Init.c" // homo 23/06/06
#include "scripts\islandships.c"
#include "scripts\officers.c"
#include "scripts\companions.c"
#include "scripts\ShipsUtilites.c"
#include "scripts\Crew.c"
#include "scripts\bountyhunters.c"
#include "scripts\tavern.c"
#include "scripts\idioms.c"
#include "scripts\smuggling.c"
#include "scripts\PsHero.c" // navy
#include "scripts\duel.c" // navy
#include "scripts\MerchantOnMap.c" // homo 29/07/06
#include "scripts\Siege.c" // homo 09/09/06 Осады
#include "scripts\Treasure.c"  // boal 02.09.06 клады

#define MAN 0
#define WOMAN 1

#define WITHOUT_SHIP 0
#define WITH_SHIP 1

#define WARRIOR 0
#define CITIZEN 1
#define TRADER 2
#define BOSS	3
#define FORT_COMMANDER	4
#define BOUNTY_HUNTER	5
#define SHAMAN	6
#define SOLDIER	7
#define OFFICER	8

#define QUEST_NOT_ACCEPTED	0
#define QUEST_ACCEPTED	1
#define QUEST_COMPLETED	2

void SetNames()
{
	for(int i = 1; i <MAX_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "name") || Characters[i].name == "none")
		{
			SetRandomNameToCharacter(&Characters[i]);
		}
	}
}

void SetNationRelations()
{
    Nations_SetAllFriends();

	SetNationRelationBoth(ENGLAND, PIRATE, RELATION_ENEMY);
	SetNationRelationBoth(ENGLAND, FRANCE, RELATION_NEUTRAL);

	SetNationRelationBoth(HOLLAND, PIRATE, RELATION_ENEMY);
	SetNationRelationBoth(FRANCE, PIRATE, RELATION_ENEMY);

	SetNationRelationBoth(SPAIN, ENGLAND, RELATION_ENEMY);
    SetNationRelationBoth(SPAIN, PIRATE, RELATION_ENEMY);
    SetNationRelationBoth(SPAIN, FRANCE, RELATION_ENEMY);
    SetNationRelationBoth(SPAIN, HOLLAND, RELATION_NEUTRAL);

    SetNationRelationBoth(HOLLAND, ENGLAND, RELATION_ENEMY);
    SetNationRelationBoth(HOLLAND, FRANCE, RELATION_FRIEND);
    
	// ГГ отношения от нации
	pchar = GetMainCharacter();
	SetNationRelation2MainCharacter(ENGLAND, GetNationRelation(sti(pchar.nation), ENGLAND));
	SetNationRelation2MainCharacter(FRANCE,  GetNationRelation(sti(pchar.nation), FRANCE));
	SetNationRelation2MainCharacter(SPAIN,   GetNationRelation(sti(pchar.nation), SPAIN));
	SetNationRelation2MainCharacter(PIRATE,  GetNationRelation(sti(pchar.nation), PIRATE));
    SetNationRelation2MainCharacter(HOLLAND, GetNationRelation(sti(pchar.nation), HOLLAND));
}

// метод установки отношений по дипломату
void ChangeNationRelationFromRelationAgent(aref chr)
{
	int iNation = sti(chr.quest.relation);

    int iDays = rand(10) + 5;
	string sQuest = "Change_Relation_for_Nation_" + GetNationNameByType(iNation) + "_by_relation_agent_" + iDays;
	
	SetTimerCondition(sQuest, 0, 0, iDays, false);
	pchar.quest.(sQuest).function = "ChangeNationRelationFromRelationAgentComplete";
	pchar.quest.(sQuest).nation = iNation;
}

void ChangeNationRelationFromRelationAgentComplete(string sQuest)
{
	int iNation = sti(pchar.quest.(sQuest).nation);
    string sNation = "RelationAgentRate" + GetNationNameByType(iNation);
    
	SetNationRelation2MainCharacter(iNation, RELATION_NEUTRAL);
	ChangeCharacterNationReputation(pchar, iNation, sti(Pchar.GenQuest.(sNation)));
	sti(Pchar.GenQuest.(sNation)) = 0; // чтоб второй раз не было
	sNation = "RelationAgent" + GetNationNameByType(iNation);
    Pchar.GenQuest.(sNation) = false;
    
	Log_Info("Дипломат выполнил свою работу.");
}

int CalculateRelationSum(int iNation)
{
	string sNation = "RelationAgentRate" + GetNationNameByType(iNation);
	Pchar.GenQuest.(sNation) = abs(ChangeCharacterNationReputation(pchar, iNation, 0))
	int iSumm = sti(Pchar.GenQuest.(sNation)) * 1500 +  makeint(stf(Pchar.rank)/stf(Pchar.reputation)*100000);

	//iSumm = iSumm * (1.0 + (0.1 * MOD_SKILL_ENEMY_RATE));
	
	return iSumm;
}
// to_do del нигде не используется
bool CreateParticleSystemOnLocator(string _location_id, string _locatorName, string _particleName)
{
	aref locator;
	if (!FindLocator(_location_id, _locatorName, &locator, true))
		return false;

	CreateParticleSystemX(_particleName,stf(locator.x),stf(locator.y),stf(locator.z), stf(locator.vz.x),stf(locator.vz.y),stf(locator.vz.z),0);
	return true;
}

void GiveItemToTrader(aref ch)
{
	/*int irand;
	
	DeleteAttribute(ch, "items");
	
	TakeNItems(ch,"blade1", Rand(1)+1);
	TakeNItems(ch,"pistol1", Rand(1)+1);

	float fChance;
	fChance = FRAND(100.0);
	int iLuck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
	for(int i = ITEMS_QUANTITY-1; i >= 0, i--)
	{
		if(CheckAttribute(&items[i], "picTexture"))
		{
			if(items[i].picTexture == "WEAPONS")
			{
				if(sti(items[i].uniq) == 0)
				{
					if((stf(items[i].rare) + iLuck) >= fChance && stf(items[i].rare) > 10.0)
					{
						TakeNItems(ch,items[i].id, 1);
					}
				}
			}
		}
	} */
	int irand;

	DeleteAttribute(ch, "items");
	// boal зачем они в продаже?  ДЛЯ ОТЛАДКИ  -->
	if (bBettaTestMode)
	{
    	for(irand=0; irand < ITEMS_QUANTITY; irand++)
    	{
            TakeNItems(ch, Items[irand].id, 1);
    	}
	}

	// издевательство -->
	irand = rand(3);
    if (sti(pchar.rank) == 1 && irand == 1) {TakeNItems(ch,"sculMa3", 1);}
    // издевательство <--

	irand = rand(3);
    if (irand == 1) {TakeNItems(ch,"blade5", Rand(2)+1);} // кинжал
	irand = rand(3);
    if (irand == 1) {TakeNItems(ch,"blade1", Rand(2)+1);}
	irand = rand(3);
    if (irand == 1) {TakeNItems(ch,"blade2", Rand(2)+1);}
	irand = rand(3);
    if (irand == 1) {TakeNItems(ch,"blade3", 1);}
	irand = rand(4);
    if (irand == 1) {TakeNItems(ch,"blade4", 1);}
    if (irand == 2) { TakeNItems(ch,"blade10", Rand(1)+1);}
    if (irand == 3 && makeint(pchar.rank) >=2) {TakeNItems(ch,"blade35", 1);}

    irand = rand(3);
    if (irand == 1 && makeint(pchar.rank) >=3) {TakeNItems(ch,"topor3", 1);}
    if (irand == 2 && makeint(pchar.rank) >=4) { TakeNItems(ch,"blade11", 1);}

    irand = rand(4);
    if (irand == 1 && makeint(pchar.rank) >=8) {TakeNItems(ch,"blade30", 1);}

    irand = rand(6);
    if (irand == 1 && makeint(pchar.rank) >=7) {TakeNItems(ch,"topor1", 1);}

	if (makeint(pchar.rank) >=5)
	{
	    irand = rand(6);
		if (irand == 1) { TakeNItems(ch,"blade6", 1);}
		if (irand == 2) { TakeNItems(ch,"blade7", Rand(1)+1);}
		if (irand == 3) { TakeNItems(ch,"blade9", Rand(1)+1);}
		if (irand == 4) { TakeNItems(ch,"blade13", Rand(1)+1);}
	}
	irand = rand(8);
    if (irand == 1) { TakeNItems(ch,"cirass1", 1);}

    if (makeint(pchar.rank) >=6)
	{
		irand = rand(8);

        if (irand == 3) { TakeNItems(ch,"blade12", Rand(1)+1);}
		if (irand == 0) { TakeNItems(ch,"cirass2", 1);}
	}
	if (makeint(pchar.rank) >=13)
	{
		irand = rand(10);
	    if (irand == 1) { TakeNItems(ch,"cirass4", 1);}
	}
	if (makeint(pchar.rank) >=10)
	{
		irand = rand(12);
		if (irand == 1) { TakeNItems(ch,"blade15", 1);}
		if (irand == 2) { TakeNItems(ch,"blade16", 1);}
        if (irand == 3) { TakeNItems(ch,"cirass3", 1);}
	}

	if (makeint(pchar.rank) >= 12)
	{
		irand = rand(5);
		if (irand == 1) { TakeNItems(ch,"blade17", 1);}
        if (irand == 2) { TakeNItems(ch,"blade18", 1);}
		if (irand == 3) { TakeNItems(ch,"blade19", 1);}

	}
    if (makeint(pchar.rank) >= 15)
	{
		irand = rand(10);
		if (irand == 1) { TakeNItems(ch,"blade20", 1);}
        if (irand == 2) { TakeNItems(ch,"blade21", 1);}
		if (irand == 3) { TakeNItems(ch,"blade22", 1);}
        if (irand == 4) { TakeNItems(ch,"blade23", 1);}
	}

    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >=13) {TakeNItems(ch,"blade24", 1);} // ураган
    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >=11) {TakeNItems(ch,"blade31", 1);}
    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >=17) {TakeNItems(ch,"blade33", 1);}
    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >=16) {TakeNItems(ch,"blade25", 1);}
    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >=18) {TakeNItems(ch,"blade34", 1);}
    irand = rand(20);
    if (irand == 1 && makeint(pchar.rank) >= 50) {TakeNItems(ch,"topor2", 1);}

    //irand = rand(8);
    //if (irand == 1 && makeint(pchar.rank) >=16) {TakeNItems(ch,"blade28", 1);} // Танат


    //irand = rand(20);
    //if (irand == 1 && makeint(pchar.rank) >=14) {TakeNItems(ch,"blade26", 1);} // цербер
    //irand = rand(40);
    //if (irand == 1 && makeint(pchar.rank) >=35) {TakeNItems(ch,"blade32", 1);} // Килидж
	// boal gun bullet -->
	TakeNItems(ch,"bullet", Rand(20)+10);
    // boal <--


    irand = rand(4);
    if (irand == 1)
    {
	    TakeNItems(ch,"pistol1", Rand(2)+1);
    }

	if (makeint(pchar.rank) >= 2)
	{
		irand = rand(5);
		if (irand == 1) { TakeNItems(ch,"pistol2", Rand(1)+1);}
	}
	if (makeint(pchar.rank) >= 5)
	{
		irand = rand(7);
		if (irand == 1) { TakeNItems(ch,"pistol3", Rand(1)+1);}
	}
	if (makeint(pchar.rank) >= 10)
	{
		irand = rand(10);
		if (irand == 1) { TakeNItems(ch,"pistol4", 1);}
	}
	if (makeint(pchar.rank) >= 8)
	{
		irand = rand(8);
		if (irand == 1) { TakeNItems(ch,"pistol5", 1);}
	}
	if (makeint(pchar.rank) >= 12)
	{
		irand = rand(10);
		if (irand == 1) { TakeNItems(ch,"pistol6", 1);}
	}
    irand = rand(10);
    if (irand == 1)
    {
	    TakeNItems(ch,"spyglass1", 1);
	}

	irand = rand(8);
	if (irand == 1 && makeint(pchar.rank) >=3) { TakeNItems(ch,"spyglass2", 1);}
	irand = rand(8);
	if (irand == 1  && makeint(pchar.rank) >=7) { TakeNItems(ch,"spyglass3", 1);}
	irand = rand(20);
	if (irand == 1 && makeint(pchar.rank) >=10) { TakeNItems(ch,"spyglass4", 1);}

    TakeNItems(ch,"potion1", Rand(11)+1);
    irand = rand(3);
    if (irand == 1)
    {
	    TakeNItems(ch,"potion2", Rand(7)+1);
	}
	irand = rand(5);
    if (irand == 1)
    {
	    TakeNItems(ch,"potion3", Rand(3)+1);
	}
	irand = rand(5);
	if (irand == 1)
    {
	    TakeNItems(ch,"potion4", Rand(5)+1);
	}
	irand = rand(5);
	if (irand == 1)
    {
        TakeNItems(ch,"potion5", Rand(5)+1);
    }

	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry1", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry2", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry3", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"jewelry4", Rand(5)+1);} // itmname_jewelry4	{Изумруд} (+1 авторитет)
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry5", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry6", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry7", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"jewelry8", Rand(5)+1);}  // itmname_jewelry8	{Bronze Ring} +1 удача
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"jewelry9", Rand(5)+1);} // бронзовый крест +1 удача
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry10", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry11", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry12", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry13", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry14", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"jewelry15", Rand(5)+1);} // Emerald Pendant +1 удача
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"jewelry16", Rand(2)+1);}
	irand = rand(9);
    if (irand == 1) {TakeNItems(ch,"jewelry17", Rand(2)+1);}
	irand = rand(9);
    if (irand == 1) {TakeNItems(ch,"jewelry18", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian1", Rand(5)+1);} //itmname_indian1	{Идол Иргантахула} (+1 авторитет и удача, -2 пистолеты).
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian2", Rand(5)+1);} // Weird Animal Figurine (-1 авторитет)
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian3", Rand(5)+1);} // Small Mosaic (+1 торговля)
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian4", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian5", Rand(5)+1);} // itmname_indian5	{Маска Маркуханкату} (+1 удача).
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian6", Rand(5)+1);}// indian6	{Бог войны} (+1 фехтование)
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian7", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian8", Rand(2)+1);}
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian9", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian10", Rand(5)+1);} // itmname_indian10	{Идол Йкстоикхатлу} (+2 пистолеты, +1 меткость, -2 удача).
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian11", Rand(2)+1);} // бог крыс
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian12", Rand(5)+1);} // itmname_indian12	{Идол Морхахтулака} (+1 фехтование).
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian13", Rand(2)+1);}
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian14", Rand(5)+1);} // itmname_indian14	{Идол Ихтилокстумба} (+2 торговля, -2 авторитет).
	//irand = rand(9);
	//if (irand == 1) {TakeNItems(ch,"indian15", Rand(5)+1);} // itmname_indian15	{Ритуальная ванна} (+1 авторитет и защита, -1 удача).
	irand = rand(9);
	if (irand == 1) {TakeNItems(ch,"indian16", Rand(2)+1);}

	// boal 270904 карты -->
	irand = rand(9);
	if (irand == 1) { TakeNItems(ch,"map_bad", 1);}
	irand = rand(16);
	if (irand == 1 && makeint(pchar.rank) >=3) { TakeNItems(ch,"map_normal", 1);}

	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_jam", 1);}
	irand = rand(39);
	if (irand == 2) {TakeNItems(ch,"map_cayman", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_barbados", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_trinidad", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_Curacao", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_martiniqua", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_dominica", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_puerto", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_cuba", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_hisp", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_nevis", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_guad", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_antigua", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_TORTUGA", 1);}
	irand = rand(39);
	if (irand == 2) {TakeNItems(ch,"map_terks", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_sm", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_bermudas", 1);}
	irand = rand(39);
	if (irand == 2) {TakeNItems(ch,"map_Pearl", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_beliz", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_santa", 1);}
	irand = rand(39);
	if (irand == 2) {TakeNItems(ch,"map_maine_1", 1);}
	irand = rand(39);
	if (irand == 2) {TakeNItems(ch,"map_maine_2", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_panama", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_maracaibo", 1);}
	irand = rand(19);
	if (irand == 2) {TakeNItems(ch,"map_cumana", 1);}
	// boal 270904 карты <--
}

string PlaceCharacter(aref ch, string group, string location) //boal change
{  // location = "random"  "random_free"  и "random_must_be" - должен быть, даже если все занято  random_must_be_near - рядом
	float locx;
	float locy;
	float locz;
	string homelocator;
	bool ok;
	
	GetCharacterPos(GetMainCharacter(), &locx, &locy, &locz);
	if (location == "random_free") location = "random"; // совместимость с пред. правкой
	
	if (location == "random" || location == "random_must_be")
	{
		homelocator = LAi_FindFreeRandomLocator(group);
		ChangeCharacterAddressGroup(ch, pchar.location, group, homelocator);  // всегда, где ГГ
	}
	else
	{
		homelocator = LAi_FindNearestFreeLocator(group, locx, locy, locz);
		if (location == "random_must_be_near")
		{
		    ChangeCharacterAddressGroup(ch, pchar.location, group, homelocator);  // всегда, где ГГ
		}
		else
		{
			ChangeCharacterAddressGroup(ch, location, group, homelocator);
		}
	}

	if (homelocator == "" && location == "random_must_be") // дело плохо, свободных уже нет, но НПС-то нужен, ищем затяный
	{
	    homelocator = LAi_FindFarLocator(group, locx, locy, locz);
		ChangeCharacterAddressGroup(ch, pchar.location, group, homelocator);  // всегда, где ГГ
	}
	if (homelocator == "" && location == "random_must_be_near") // дело плохо, свободных уже нет, но НПС-то нужен, ищем затяный
	{
	    homelocator = LAi_FindNearestLocator(group, locx, locy, locz);
		ChangeCharacterAddressGroup(ch, pchar.location, group, homelocator);  // всегда, где ГГ
	}
    return homelocator; // boal нужно проверять куда его занесло
}

void PrepareVisitCity()
{
	string sQuest;
	for (int i=0; i<MAX_COLONIES; i++)
	{
		if (Colonies[i].nation != "none")
		{
			sQuest = "VisitColony_" + Colonies[i].id;
			pchar.quest.(sQuest).win_condition.l1 = "location";
			pchar.quest.(sQuest).win_condition.l1.location = Colonies[i].id + "_town";
			pchar.quest.(sQuest).win_condition = "FirstVisitCity";
			pchar.quest.(sQuest).function      = "FirstVisitCity";
			pchar.quest.(sQuest).colonyId = Colonies[i].id;
		}
	}
}

void FirstVisitCity(string sQuest)
{
	string sColony = pchar.quest.(sQuest).colonyId;
	Colonies[FindColony(sColony)].visited = true;
}

// переделал 25.09.06 метод не зависит от ГГ, просто ранд корабли для порта
int SearchForMaxShip(aref chr, int isLock, int _tmp)
{
	int iType;
	
	if (sti(chr.nation) != PIRATE)
	{
		if (rand(100) > 70)
		{
			iType = rand(SHIP_FRIGATE);	
		}
		else
		{
			iType = rand(SHIP_BRIG);	
		}
	}
	else
	{
	 	if (rand(100) > 70)
		{
			iType = rand(SHIP_BATTLESHIP);	
		}
		else
		{
			iType = rand(SHIP_GALEON_L);	
		}
	}
	iType = GenerateShip(iType, isLock);
	return iType;
}

int FindFirstEmptyCharacter()
{
	for(int i = GlobalCharacters; i<TOTAL_CHARACTERS; i++)
	{
		if (characters[i].id == "0")
		{
			if (i >= MAX_CHARACTERS) MAX_CHARACTERS = i+1; //сдвигаем планку НПС
			return i;
		}
	}
	
	return -1;
}

void AddGeometryToLocation(string LocationID, string ModelName)
{
	ref LocationRef;
	int n = 1;
	string str;
	locationRef = &locations[FindLocation(LocationID)];

	for(n = 1; n < 10; n++)
	{
		str = "l" + n;
		if(!CheckAttribute(LocationRef, "models.always." + str))
		{
			LocationRef.models.always.(str) = ModelName;
			LocationRef.models.always.(str).tech = "dLightModel";
			break;
		}
	}
}	

void RemoveGeometryFromLocation(string LocationID, string ModelName)
{
	ref LocationRef;
	int n = 1;
	string str;
	//trace("FUNCTION RemoveGeometryFromLocation reports: Current location ID is " + LocationID + " and geometry " + ModelName + " should be removed");
	if(FindLocation(LocationID) != -1)
	{
		locationRef = &locations[FindLocation(LocationID)];
	}
	else
	{
		return;
	}

	for(n = 1; n < 10; n++)
	{
		str = "l" + n;
		//trace("FUNCTION RemoveGeometryFromLocation reports: Checking Geometry name for n = " + str + ". It is " + LocationRef.models.always.(str));
		if(CheckAttribute(LocationRef, "models.always." + str) && LocationRef.models.always.(str) == ModelName)
		{
			DeleteAttribute(LocationRef, "models.always." + str);
			break;
		}
	}
}	


void CreateModel(int iChar, string sType, int iSex)
{
	int iNation = sti(characters[iChar].nation);
	
	string sBody = "";
	string sPrefix = "";
	int iNumber = -1;
		
	switch (sType)
	{
		case "pofficer":
			sBody = "officer";
			iNumber = rand(11)+1;
		break;

		case "officer":
			sBody = "off";
			iNumber = rand(1)+1;
		break;
		
		case "soldier":
			sBody = "sold";
			iNumber = rand(7)+1;
		break;
		
		case "pirate":
			sBody = "pirate";
			iNumber = rand(9)+1;
		break;

		case "trader":
			sBody = "trader";
			iNumber = rand(15)+1;
		break;
		
		case "citizen":
			if(iSex == MAN)
			{
				sBody = "citiz";
				iNumber = rand(11)+1;
			}
			else
			{
				sBody = "girl";
				iNumber = rand(9)+1;
			}
		break;

		case "whore":
			sBody = "whore";
			iNumber = rand(3) + 1;
		break;
	}

	sPrefix = "_";
	
	if(sType == "officer" || sType == "soldier")
	{
		switch (iNation)
		{
			case ENGLAND:
				sPrefix = "_eng_";
			break;
			
			case FRANCE:
				sPrefix = "_fra_";
			break;
			
			case SPAIN:
				sPrefix = "_spa_";
			break;
			
			case HOLLAND:
				sPrefix = "_hol_";
			break;
			
			case PIRATE:
				sPrefix = "";
			break;
		}
	}
	
	string sResult = "";
	
	sResult = sBody+sPrefix+iNumber;
	
	characters[iChar].model = sResult;

	FaceMaker(&characters[iChar]);
}

// метод вернет случайный дружественный iNation город, неравный  sBeginColony _checkPort - Проверка порта
int FindNonEnemyColonyForAdventure(int iNation, string sBeginColony, bool _checkPort)
{
	int iArray[MAX_COLONIES];
	int m = 0;
	
	for (int i=0; i<MAX_COLONIES; i++)
	{
		if (colonies[i].nation != "none")
		{
			if (_checkPort && CheckAttribute(&Colonies[i], "HasNoFort")) continue;  // без форта не берем
			
			if (GetNationRelation(sti(Colonies[i].nation), iNation) != RELATION_ENEMY && Colonies[i].id != sBeginColony)
			{
				iArray[m] = i;
				m++;
			}
		}
	}
	
	if (m == 0)
	{
		return -1;
	}
	
	m = rand(m-1);
	m = iArray[m];
	return m;
}

int FindNonEnemyColonyForNation(int iNation, bool _checkPort)
{
	return FindNonEnemyColonyForAdventure(iNation, "NOT_IN_USE", _checkPort);
}

string FindAlliedColonyForNation(int iNation, bool _checkPort)
{
	int iArray[MAX_COLONIES];
	int m = 0;
	string sColony = "";
	
	for (int i = 0; i<MAX_COLONIES; i++)
	{
		if (colonies[i].nation != "none" && colonies[i].id != "Panama")
		{
			if (_checkPort && CheckAttribute(&Colonies[i], "HasNoFort")) continue;  // без форта не берем
			if (sti(Colonies[i].nation)== iNation)
			{
				iArray[m] = i;
				m++;
			}
		}
	}
	
	if (m == 0)
	{
		return "-1";
	}
	
	m = rand(m-1);
	m = iArray[m];
	sColony =  Colonies[m].id;
	
	return sColony;
}

string FindAlliedColonyForNationExceptColony(string sHomeColony)
{
	int iArray[MAX_COLONIES];
	int m = 0;
	string sColony = "";

	int iNation = sti(colonies[FindColony(sHomeColony)].nation);
	
	for (int i = 0; i<MAX_COLONIES; i++)
	{
		if(colonies[i].nation != "none" && colonies[i].id != sHomeColony)
		{
			if (sti(Colonies[i].nation) == iNation)
			{
				iArray[m] = i;
				m++;
			}
		}
	}
	
	if (m == 0)
	{
		return "-1";
	}
	
	m = rand(m-1);
	m = iArray[m];
	sColony =  Colonies[m].id;
	
	return sColony;
}
/*
string SelectQuestDestinationAnyNationExceptTargetNation(int iNation)
{
	string sArray[MAX_COLONIES];
	int m = 0;

	for (int i = 0; i<MAX_COLONIES; i++)
	{
		if (sti(Colonies[i].nation) != iNation)
		{
			sArray[m] = Colonies[i].id;
			m++;
		}
	}

	if (m > 0)
	{
		m = rand(m-1);
		return sArray[m];
	}
	else
	{
		return "Error in select Destination for capture ship";
	}
}

string SelectQuestDestinationAnyNationExceptIsland(string sIsland)
{
	int m = 0;

	string sTempIsland = sIsland;

	while(sTempIsland == sisland)
	{
		m = rand(MAX_COLONIES-1);
		if(colonies[m].nation != "none")
		{
			sTempIsland = Colonies[m].island;
		}
	}
	string sTempColony  = Colonies[m].id
	return sTempColony;
}
*/
// to_do
string SelectQuestDestinationAnyNationExceptColony(string sColony)
{
	int m;

	string sTempColony = sColony;

	while(sTempColony == sColony)
	{
		m = rand(MAX_COLONIES-1);
		if(colonies[m].nation != "none")
		{
			sTempColony = Colonies[m].id;
		}
	}

	return sTempColony;
}

void EmptyAllFantomCharacter()
{
	if (bAbordageStarted) return; // иначе трутся НПС при переходе в каюте
	int cn = -1;
	for (int i=GlobalCharacters; i<MAX_CHARACTERS; i++)
	{
		// отдельный код зачистки boal -->
		if (LAi_IsDead(&characters[i]) && !CheckAttribute(&characters[i], "RebirthPhantom"))
		{
      			if(GetCompanionQuantity(&characters[i]) > 1) // Хм, а у фантомов есть компаньоны как у взрослых ГГ? оставлю на всяк случай (для ПГГ)
				{
					for(int k = 0; k < COMPANION_MAX; k++)
					{
						cn = GetCompanionIndex(&characters[i], k);
						if(cn != -1)
						{
							InitCharacter(&characters[cn], cn);
						}
					}
				}
				else
				{
     				InitCharacter(&characters[i], i);
				}
		}
		else
		{
			if (CheckAttribute(&characters[i], "LifeDay") &&
			    GetNpcQuestPastDayParam(&characters[i], "LifeTimeCreate") >= sti(characters[i].LifeDay) &&
				characters[i].location != pchar.location) // не трем, если ГГ в локации,иначе горожане пропадают на лету
			{ // время вышло
				InitCharacter(&characters[i], i);  // тут проверку на компаньонов не нужно, тк они все одинаковые по времени
			}
		}
		// boal <--
	}
}

// метод оставлен для совместимости со старым кодом К3
int GenerateCharacter(int iNation, int isShip, string sModel, int iSex, int isLock, int CharacterType)
{
    int iChar = NPC_GeneratePhantomCharacter(sModel, iNation, iSex, -isLock); // -isLock) == -1 || 0

	if (IsShip == WITH_SHIP)
	{
		// to_do del
		if(CharacterType == FORT_COMMANDER)
		{
			characters[iChar].ship.type = GenerateShip(SHIP_FORT, isLock);
			Fantom_SetRandomCrewExp(&characters[iChar], "war");
		}
		else
		{
			characters[iChar].ship.type = SearchForMaxShip(&characters[iChar], isLock, CharacterType);
			Fantom_SetRandomCrewExp(&characters[iChar], "war");
			
		}
		
		SetBaseShipData(&characters[iChar]);
		
		SetRandomNameToShip(&characters[iChar]);
	}
	else
	{
		characters[iChar].ship.type = SHIP_NOTUSED;
		DeleteAttribute(&characters[iChar], "ship.upgrades");
	}
	return iChar;
}

int GetRandomNationForMapEncounter(string sIslandID, bool bMerchant)
{   // boal: метод соверненно не понятен, убрал из него массив НатионСтайт, заменив на 1.0, раз работало - пусть будет как было, но логика чумовая
	int iNation = -1;
	if(sIslandID != "")
	{
		int iIsland = FindIsland(sIslandID);
		for(int i = 0; i < MAX_COLONIES; i++)
		{
			if(colonies[i].island == sIslandID)
			{
				if (colonies[i].nation != "none")
				{
					iNation = sti(colonies[i].nation);
					break;
				}
			}
		}
	}

	float fEngland  = 1.0;
	float fFrance   = 1.0;
	float fSpain    = 1.0;
	float fHolland  = 1.0;
	float fPirate   = 1.0;

	if(iNation != -1)
	{
	    switch (iNation )
		{
			case ENGLAND:
				fEngland += 0.2;
			break;
			case FRANCE:
    			fFrance += 0.2;
			break;
			case SPAIN:
    			fSpain += 0.2;
			break;
			case HOLLAND:
    			fHolland += 0.2;
			break;
			case PIRATE:
    			fPirate += 0.2;
			break;
		}
	}

	float fProbablyNation;
	if(bMerchant)
	{
		fProbablyNation = fEngland + fFrance + fSpain + fHolland;
	}
	else
	{
		fProbablyNation = fEngland + fFrance + fSpain + fHolland + fPirate;
	}

	fProbablyNation = frand(fProbablyNation);

	fFrance   = fFrance  + fEngland;
	fSpain    = fFrance  + fSpain;
	fHolland  = fSpain   + fHolland;
	fPirate   = fHolland + fPirate;

	if(bMerchant == 0)
	{
		if(fProbablyNation >= fFrance && fProbablyNation < fSpain)
		{
			return FRANCE;
		}

		if(fProbablyNation >= fSpain && fProbablyNation < fHolland)
		{
			return SPAIN;
		}

		if(fProbablyNation >= fHolland && fProbablyNation < fPirate)
		{
			return HOLLAND;
		}

		if(fEngland <= fProbablyNation)
		{
			return ENGLAND;
		}
	}
	else
	{
		if(fProbablyNation >= fFrance && fProbablyNation < fSpain)
		{
			return FRANCE;
		}

		if(fProbablyNation >= fSpain && fProbablyNation < fHolland)
		{
			return SPAIN;
		}

		if(fProbablyNation >= fHolland && fProbablyNation < fPirate)
		{
			return HOLLAND;
		}

		if(fEngland <= fProbablyNation)
		{
			return ENGLAND;
		}
	}

	return PIRATE;
}


string CheckingTranslate(int idLngFile, string idString)
{
	string retString = LanguageConvertString ( idLngFile, idString );
	if(retString=="")
	{
		trace("Warning! No text for lable <" + idString + "> into language file <LocLables.txt>");
	}
	return retString;
}

// to_do
void LaunchMoneyGraphCollect()
{
	string sYear = "year" + worldMap.date.year;
	string sMonth = "month" + worldMap.date.month;

	pchar.MoneyGraph.(sYear).(sMonth) = pchar.money;
}

string FindNearestFreeLocator(string group)
{
	float locx;
	float locy;
	float locz;
	string homelocator;
	
	GetCharacterPos(pchar, &locx, &locy, &locz);
	homelocator = LAi_FindNearestFreeLocator(group, locx, locy, locz);
	
	return homelocator;
}

string SetModelFlag(aref chr)
{
	string sResult = "";

	if(chr.chr_ai.type == LAI_TYPE_ACTOR)
	{
		return "";
	}

	if(CheckAttribute(&InterfaceStates,"EnabledQuestsMarks") )
	{
		if(sti(InterfaceStates.EnabledQuestsMarks) == 0)
		{
			return "";
		}
	}

	if(CheckAttribute(chr, "quest.questflag"))
	{
		switch(sti(chr.quest.questflag))
		{
			case 1:
				sResult = "exclamationmarkY";
			break;
			
			case 2:
				sResult = "questionmarkY";
			break;

			case 3:
				sResult = "questionmarkW";
			break; 
		}
	}

	chr.quest.questflag.technique = GetTechNameForSign();

	return sResult;
}

string GetTechNameForSign()
{
	return "RandItem";
}

void CheckQuestForCharacter(aref chr)
{

}

////////////////////////// boal набор методов для генерации фантомов
// boal -->
void InitCharacter(ref ch, int n)
{
    //DeleteAttribute(ch,"act");
	DeleteAttribute(ch,"");
	//Logic data
	ch.id = "0";
	ch.index = n;
	ch.name 	= "none";
	ch.lastname = "none";
	ch.sex = "man";
	//Model data
	ch.model = "none";
	ch.model.entity = "NPCharacter";
	ch.model.animation = "";
	//Address
	ch.location = "none";
	ch.location.group = "";
	ch.location.locator = "";
	ch.location.from_sea = "";
	//Ship data
	ch.Ship.Type = SHIP_NOTUSED;
	//Quest data
	ch.Dialog.CurrentNode = "First time";
	ch.Dialog.TempNode = "First time";
	ch.quest.meeting = "0";
	ch.quest = "True";
	ch.quest_date = 0;
	//Misc data
	ch.nation = ENGLAND;
	ch.skill.freeskill = 0;
	ch.perks.freepoints = 0;
	ch.rank = 1;
	ch.reputation = REPUTATION_NEUTRAL;
	ch.reputation.france = 0;
	ch.reputation.england = 0;
	ch.reputation.spain = 0;
	ch.reputation.holland = 0;
	ch.reputation.pirate = 0;
	ch.reputation.smuggler = 0;
	ch.reputation.trader = 0;
	ch.fame = 1;
	// boal -->
    SetRandSPECIAL(ch);
    //InitStartParam(ch);
    SetSelfSkill(ch, 1, 1, 1, 1, 1);
    SetShipSkill(ch, 1, 1, 1, 1, 1, 1, 1, 1, 1);

	ch.Skill.FreeSPECIAL  = 0;
    ch.Health.HP        = 60.0; // отличное
	ch.Health.maxHP     = 60.0;
	ch.Health.Damg      = 0.0;
	ch.Health.weekDamg  = 0.0;
	ch.Health.TotalDamg = 0.0;
	// <--
	ch.rank = 1;
	ch.experience = 0;

	ch.Fellows.Passengers.id0 = n; // свой пассажир

	ch.Fellows.Passengers.boatswain = "-1";
	ch.Fellows.Passengers.navigator = "-1";
	ch.Fellows.Passengers.doctor = "-1";
	ch.Fellows.Passengers.cannoner = "-1";
	ch.Fellows.Passengers.treasurer = "-1";
	ch.Fellows.Passengers.carpenter = "-1";

	//ch.prisoned = false;

	ch.money = "0";
	// battle hp
	ch.headModel = "";
	//LAi_SetLoginTime(ch, 6.0, 21.98333);
	LAi_SetLoginTime(ch, 0.0, 24.0); // круглосуточно
 	LAi_SetHP(ch, LAI_DEFAULT_HP, LAI_DEFAULT_HP_MAX);
}
// boal <--
// метод ищет или создает НПС с указанным ID. Для последуешего применения
int NPC_FindOrCreateCharacter(string _id)
{
	int ci = GetCharacterIndex(_id);
	if (ci == -1)
    { // не нашли
        ci = FindFirstEmptyCharacter();
        if (ci != -1)
		{
			InitCharacter(&Characters[ci], ci);
			Characters[ci].id = _id;
		}
    }
    return ci;
}
// _ani нужно для двух рас пкм + к3
// _LifeDay - сколько дней живет, если -1, то вечный, если 0, то трется при выходе из локации
// _equip - экипировка саблями, перки все равно раздадутся
int NPC_GenerateCharacter(string _id, string _model, string _sex, string _ani, int _rank, int _nation, int _LifeDay, bool _equip)
{
    int iChar = NPC_FindOrCreateCharacter(_id);
	ref ch;
	if (iChar == -1) return -1;
	
    ch = &Characters[iChar];
	ch.rank 	= _rank;
    ch.nation   = _nation;
    ch.sex      = _sex;
    ch.model    = _model;
    if(ch.sex == "man")
	{
		ch.model.height = 1.8;
	}else{
		ch.model.height = 1.75;
	}
	ch.model.animation = _ani;
	FaceMaker(ch);
	SetRandomNameToCharacter(ch);
	SetFantomParamFromRank(ch, _rank, _equip);
	if (_LifeDay >= 0)
	{
	    ch.LifeDay = _LifeDay;
	    SaveCurrentNpcQuestDateParam(ch, "LifeTimeCreate");
	}
	else
	{
     	DeleteAttribute(ch, "LifeDay");
	}
	return  iChar;
}
// сгенерим разового фантома - нашего моряка (в  форме, если патент)
ref GetOurSailor(string _id) // моежт быть нужно несколько
{
    string  smodel;
    ref     CrOur;
    string  ani;
    int     i;

    smodel = LAi_GetBoardingModel(GetMainCharacter(), &ani);
    
	i = NPC_GenerateCharacter(_id, smodel, "man", ani, 10, sti(pchar.nation), 0, true);
	CrOur = GetCharacter(i);
	CrOur.name     = "Матрос";
	CrOur.lastname = "";
	
	return CrOur;
}

int NPC_GeneratePhantomCharacter(string sType, int iNation, int iSex, int _LifeDay)//, int CharacterType)
{
    int iChar = FindFirstEmptyCharacter();
    ref ch;

	if (iChar == -1) return -1;
	ch = &Characters[iChar];
    InitCharacter(ch, iChar);
    
	if (_LifeDay >= 0)
	{
	    ch.LifeDay = _LifeDay;
	    SaveCurrentNpcQuestDateParam(ch, "LifeTimeCreate");
	}
	else
	{
     	DeleteAttribute(ch, "LifeDay");
	}
	
	ch.PhantomType = sType; // не нужно пока для дела, впрок

	if(iSex == MAN)
	{
		ch.sex = "man";
		ch.model.animation = "man";
		ch.model.height = 1.80
	}
	else
	{
		ch.sex = "woman";
		ch.model.animation = "towngirl";
		ch.model.height = 1.70
	}

	ch.nation = iNation;

	SetRandomNameToCharacter(ch);
    ch.reputation = (1 + rand(44) + rand(44));// репа всем горожанам
	ch.id = "GenChar_" + iChar;
	
    CreateModel(iChar, sType, iSex);
    SetFantomParam(ch);
    
    if (sType == "citizen")
	{
		LAi_NPC_Equip(ch, sti(ch.rank), false, false);
	}
	else
	{
	    LAi_NPC_Equip(ch, sti(ch.rank), true, true);
	}
	
	return  iChar;
}

// boal prison count -->
int GetPrisonerQty()
{
    ref offref;
    int i, cn, iMax;
    ref mchr = GetMainCharacter();
    int qty = 0;

    iMax = GetPassengersQuantity(mchr);
    for(i=0; i < iMax; i++)
    {
        cn = GetPassenger(mchr,i);
        if(cn != -1)
        {
            offref = GetCharacter(cn);
            if(CheckAttribute(offref,"prisoned"))
            {
	            if(sti(offref.prisoned)==true && GetRemovable(offref)) // ставим только фантомов
	            {
                    qty++;
                }
            }
        }
    }
    return qty;
}
int SetCharToPrisoner(ref refEnemyCharacter)
{
    ref  rChTo, refMyCharacter;

    refMyCharacter =  GetMainCharacter();

    int  iNextPrisoner = FindFirstEmptyCharacter();

	if (iNextPrisoner != -1)
	{
	    rChTo = &Characters[iNextPrisoner];
	    InitCharacter(rChTo, iNextPrisoner);

		ChangeAttributesFromCharacter(rChTo, refEnemyCharacter, false);   // было четкое копирование, но что-то наследовалось от той жижни и в море вел по АИ убегания
	    rChTo.index = iNextPrisoner;
	    rChTo.id = "prisoner_" + iNextPrisoner;
	    rChTo.Dialog.Filename = "Ransack_captain_dialog.c";
	    rChTo.Dialog.CurrentNode = "First time";
	    DeleteAttribute(rChTo, "items");
		DeleteAttribute(rChTo, "equip");
		DeleteAttribute(rChTo, "LifeDay"); // постоянный
		DeleteAttribute(rChTo, "ship");
		DeleteAttribute(rChTo, "ShipSails.gerald_name");

		DeleteAttribute(rChTo, "AlwaysEnemy");
		DeleteAttribute(rChTo, "ShipTaskLock");
		DeleteAttribute(rChTo, "WatchFort");
		DeleteAttribute(rChTo, "AnalizeShips");

	    rChTo.ship.type = SHIP_NOTUSED;
		GiveItem2Character(rChTo, "unarmed");
	    EquipCharacterByItem(rChTo, "unarmed");

	    rChTo.greeting = "Gr_Ransack";
	    LAi_SetCitizenTypeNoGroup(rChTo);
	    LAi_group_MoveCharacter(rChTo, "Prisoner");
	    LAi_SetLoginTime(rChTo, 0.0, 24.0);
	    LAi_NoRebirthEnable(rChTo);

	    SetCharacterRemovable(rChTo, true);

	    AddPassenger(refMyCharacter,rChTo,true);
    }
    return iNextPrisoner;
}

void ReleasePrisoner(ref NPChar)
{
    DeleteAttribute(NPChar,"prisoned"); // освободили пленника
    NPChar.LifeDay = 0; // трем
	RemovePassenger(PChar, NPChar);
}

void FreeSitLocator(string location, string locator)
{	
	ref rCharacter; //ищем
	int n;

	for (n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(rCharacter,Characters[n]);
		if (CheckAttribute(rCharacter, "location"))
		{
            if (rCharacter.location == location && rCharacter.location.locator == locator)
            {
            	ChangeCharacterAddressGroup(rCharacter, "none", "none", "none"); // в никуда
            }
            else
            {
            	if (rCharacter.location == "none") // офы, которые уже наняты, но сидят
            	{
            		//Если загруженны, то выйдем из локации
					LogoffCharacter(rCharacter);
            	}
            }
		}
    }
}

bool CheckFreeLocator(string location, string locator, int idxDontSee)
{
	return LAi_CheckLocatorFree("sit", locator);
	/*ref rCharacter; //ищем
	int n;

	for (n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(rCharacter,Characters[n]);
		if (CheckAttribute(rCharacter, "location") && n != idxDontSee) // не смотрим самого себя, если нужно
		{
            if (rCharacter.location == location && rCharacter.location.locator == locator)
            {
				return false;
            }
		}
    }
    return true;*/
}
// boal <--

int RandFromThreeDight(int _Num1, int _Num2, int _Num3)
{
	switch (rand(2))
	{
		case 0: return _Num1; break;
		case 1: return _Num2; break;
		case 2: return _Num3; break;
	}
}