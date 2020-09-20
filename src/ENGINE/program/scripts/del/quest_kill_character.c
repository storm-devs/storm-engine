
bool GenerateKillQuest(int iNation, aref chr, int iType)
{
	//iType = 0 - на улице
	//iType = 1 - в доме
	
	
	LAi_SetHp(characterFromID("Capture_Quest_pirate"), 80.0, 80.0);
	LAi_SetHp(characterFromID("Capture_Quest_officer_01"), 80.0, 80.0);
	LAi_SetHp(characterFromID("Capture_Quest_officer_02"), 80.0, 80.0);
	
	string model = "pirat"+(rand(9)+1);
	characters[getCharacterIndex("Capture_Quest_pirate")].model = model;
	characters[getCharacterIndex("Capture_Quest_pirate")].headModel = "h_"+model;
	characters[getCharacterIndex("Capture_Quest_pirate")].dialog.filename = "Quest kill character_dialog.c";
	model = "man"+(rand(5)+1);
	characters[getCharacterIndex("Capture_Quest_officer_01")].model = model;
	characters[getCharacterIndex("Capture_Quest_officer_01")].headModel = "h_"+model;
	characters[getCharacterIndex("Capture_Quest_pirate")].dialog.filename = "Quest kill character_dialog.c";
	model = "sailor"+(rand(5)+1);
	characters[getCharacterIndex("Capture_Quest_officer_02")].model = model;
	characters[getCharacterIndex("Capture_Quest_officer_02")].headModel = "h_"+model;
	characters[getCharacterIndex("Capture_Quest_pirate")].dialog.filename = "Quest kill character_dialog.c";
	
	SetRandomNameToCharacter(characterFromiD("Capture_Quest_pirate"));
	//SetRandomSkillsForCharacter(characterFromiD("Capture_Quest_pirate"));
	SetRandomNameToCharacter(characterFromiD("Capture_Quest_officer_01"));
	//SetRandomSkillsForCharacter(characterFromiD("Capture_Quest_officer_01"));
	SetRandomNameToCharacter(characterFromiD("Capture_Quest_officer_02"));
	//SetRandomSkillsForCharacter(characterFromiD("Capture_Quest_officer_02"));
	
	string sQuest = "kill_" + GetNationNameByType(iNation) + "_Quest_pirate";
	pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
	pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex("Capture_Quest_pirate");
	pchar.quest.(sQuest).win_condition = sQuest;
	pchar.quest.(sQuest).character = chr.id;
	pchar.quest.(sQuest).item = SelectItemForKillQuest();
	pchar.quest.killcharacter.item = pchar.quest.(sQuest).item;
	
	chr.quest.killcharacter = "Capture_Quest_pirate";
	chr.quest.killcharacter.island = SelectKillDestination(iNation);
	chr.quest.killcharacter.condition = "accept";
	chr.quest.killcharacter.item = pchar.quest.(sQuest).item;
	chr.quest.killcharacter.nation = iNation;
	
	if (iType == 0)
	{
		pchar.quest.prepare_kill_character.win_condition.l1 = "location";
		pchar.quest.prepare_kill_character.win_condition.l1.location = chr.quest.killcharacter.island + "_port";
		pchar.quest.prepare_kill_character.win_condition = "prepare_kill_character";
		pchar.quest.prepare_kill_character.group = Colonies[sti(chr.quest.killcharacter.island)].nation;
	}
	else
	{
		pchar.quest.prepare_home_kill_character.win_condition.l1 = "location";
		pchar.quest.prepare_home_kill_character.win_condition.l1.location = chr.quest.killcharacter.island + "_town";
		pchar.quest.prepare_home_kill_character.win_condition = "prepare_home_kill_character";
		pchar.quest.kill_character.parent_location = chr.quest.killcharacter.island + "_town";
		
		pchar.quest.prepare_kill_character.win_condition.l1 = "location";
		pchar.quest.prepare_kill_character.win_condition.l1.location = "Kill_house";
		pchar.quest.prepare_kill_character.win_condition = "prepare_kill_character";
		pchar.quest.prepare_kill_character.group = Colonies[sti(chr.quest.killcharacter.island)].nation;
	}
	
	DeleteQuestHeader("killcharacter");
	SetQuestHeader("killcharacter");
	AddQuestRecord("killcharacter", "1");
	AddQuestUserData("killcharacter", "sCharName", GetCharacterFullName(chr.id));
	AddQuestUserData("killcharacter", "sTargetCharName", GetCharacterFullName("Capture_Quest_pirate"));
	string nation = GetNationNameByType(iNation) + "Gen";
	AddQuestUserData("killcharacter", "sNationGen", XI_ConvertString(nation));
	AddQuestUserData("killcharacter", "island", GlobalStringConvert(chr.quest.killcharacter.island));
	
	characters[getCharacterIndex("Capture_Quest_pirate")].money = rand(5000)+1000;
	
	pchar.quest.work = 1;
	
	return true;
}

string SelectItemForKillQuest()
{
	int iRandItem = rand(3);
	string sItem;
	
	sItem = "Quest_Ring_" + iRandItem;
	
	return sItem;
}

void KillCharacterForQuest(int iNation, string sQuestname)
{
	
	string sCharacter = pchar.quest.(sQuestname).character;
	string sItem = pchar.quest.(sQuestname).item;
		
	characters[GetCharacterIndex(sCharacter)].quest.killcharacter.condition = "completed";
	GiveItem2Character(pchar, sItem);
	
	AddQuestRecord("KillCharacter", "2");
	Locations[FindLocation(pchar.quest.kill_character.parent_location)].reload.l1000.disable = 1;
}

void KillCharacterMissionCompleted(int iNation, aref chr)
{
	
	
	
	
	string career = GetNationNameByType(iNation) + "_line_counter";
	int iMoney = pchar.(career);
	AddMoneyToCharacter(pchar, iMoney*500);
	TakeItemFromCharacter(pchar, chr.quest.killcharacter.item);
	DeleteAttribute(chr, "quest.killcharacter");
	deleteAttribute(pchar, "quest.killcharacter");
	pchar.quest.work = 0;
	CloseQuestHeader("KillCharacter");
	ChangeCharacterAddress(characterFromID("Capture_Quest_pirate"), "none", "");
	ChangeCharacterAddress(characterFromID("Capture_Quest_officer_01"), "none", "");
	ChangeCharacterAddress(characterFromID("Capture_Quest_officer_02"), "none", "");
	ChangeCharacterFame(pchar, 1);
	ChangeCharacterReputation(pchar, 1, iNation);
	Locations[FindLocation(pchar.quest.kill_character.parent_location)].reload.l1000.disable = 1;
}

void KillCharacterMissionFailed(aref chr)
{
	
	
	TakeItemFromCharacter(pchar, chr.quest.killcharacter.item);
	int iNation = sti(chr.quest.killcharacter.nation);
	
	DeleteAttribute(chr, "quest.killcharacter");
	deleteAttribute(pchar, "quest.killcharacter");
	pchar.quest.work = 0;
	CloseQuestHeader("KillCharacter");
	ChangeCharacterAddress(characterFromID("Capture_Quest_pirate"), "none", "");
	ChangeCharacterAddress(characterFromID("Capture_Quest_officer_01"), "none", "");
	ChangeCharacterAddress(characterFromID("Capture_Quest_officer_02"), "none", "");
	ChangeCharacterReputation(pchar, -1, iNation);
	Locations[FindLocation(pchar.quest.kill_character.parent_location)].reload.l1000.disable = 1;
}

string SelectKillDestination(int iNation)
{
	
	string sIsland = "";
	string sTempIsland = "";
	int iEnemy = -1;
	int iColonyQuantity = 0;


	int iColonies[MAX_COLONIES];
	int m = 0;
	
	for(int i = 0; i <MAX_COLONIES; i++)
	{
		if (colonies[i].nation != "none")
		{
			if (GetNationRelation(sti(Colonies[i].nation), iNation) != RELATION_ENEMY && Colonies[i].id != locations[FindLocation(pchar.location)].fasterload)
			{
					iColonies[m] = i;
					m++;
			}
		}
	}
	
	if (m == 0)
	{
		return "-1";
	}
	
	int iRand = rand(m-1);
	return colonies[iRand].id;
}

string FindNonEnemyRandomIsland(int iRand, int iType, int iNation);
{
	
	int n = 0;
	
	if (iRand <0)
	{
		iRand = 0;
	}
	
	string sTargetIsland = "";
	string sPcharLocation = pchar.location;
	string sIsland;
	if (sti(iType) == 0)
	{
		n = 0;
		int iColonies[MAX_COLONIES];
		for (int k=0; k < MAX_COLONIES; k++)
		{
			if (colonies[i].nation != "none")
			{
				if (GetNationRelation(sti(Colonies[k].nation), iNation) != RELATION_ENEMY && Colonies[k].id != locations[FindLocation(sPcharLocation)].fastreload && CheckAttribute(Colonies[k], "capitol"))
				{
					iColonies[n] = i;
					n = n + 1;
				}
			}
		}
	}
	else
	{
		n = 0;
		for (int i=0; i < MAX_COLONIES; i++)
		{
			if (colonies[i].nation != "none")
			{
				if (GetNationRelation(sti(Colonies[i].nation), iNation) != RELATION_ENEMY && Colonies[i].id != locations[FindLocation(pchar.location)].fastreload && colonies[i].nation != "none")
				{
					iColonies[n] = i;
					n++
				}
			}
		}
	}
	if (n == 0)
	{
		return "-1";
	}
	
	iRand = rand(n-1);
	sTargetIsland = colonies[iRand].id;
		
	return sTargetIsland;
}

string SelectHome(string sColony)
{
	
	string Home = "";
	string sLocator = FindFreeReloadLocator("reload", sColony);
	
	if (sLocator != "-1")
	{
		Home = "Kill_house";
		Locations[FindLocation("Kill_house")].reload.l1.name = "reload1";
		Locations[FindLocation("Kill_house")].reload.l1.go = sColony;
		Locations[FindLocation("Kill_house")].reload.l1.emerge = sLocator;
	}
	else
	{
		trace ("Упс. Не смогли найти дом для убийства в локации " + sColony + ".");
	}
	return Home;
}

string FindFreeReloadLocator(string group, string sColony)
{
	if(IsEntity(loadedLocation) != true) return "";
	string sTarget = "";
	string at = "locators." + group;
	if(CheckAttribute(LoadedLocation, at) == 0) return "";
	aref grp;
	makearef(grp, LoadedLocation.(at));
	int num = GetAttributesNum(grp);
	if(num <= 0) return "";
	
	int tryquantity = 0;
	string sAtr, sAtr2;
	
	while (sTarget == "")
	{
		int result = rand(num - 1);
		string sLocator = GetAttributeName(GetAttributeN(grp, result));
		
		for (int i = 1; i <num; i++)
		{
			sAtr = "reload.l" + i;
			sAtr2 = sAtr + ".go";
			if (CheckAttribute(Locations[FindLocation(sColony)], sAtr) && CheckAttribute(Locations[FindLocation(sColony)], sAtr2))
			{
				sTarget = "";
			}
			else
			{
				sTarget = "1";
				break;
			}
		}
		tryquantity = tryquantity + 1;
		if (tryquantity == 50)
		{
			break;
		}
	}
	
	LoadedLocation.reload.l1000.name = sLocator;
	LoadedLocation.reload.l1000.go = "Kill_house";
	LoadedLocation.reload.l1000.emerge = "reload1";
	LoadedLocation.reload.l1000.label = "House";
	LoadedLocation.reload.l1000.disable = 0;
	
	
	if (sTarget  == "0")
	{
		return "-1";
	}
	else
	{
		return sLocator;
	}
}