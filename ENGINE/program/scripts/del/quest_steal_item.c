
///////////////////////////////
// саму схему воровства переделать
// есть возможность воровать укаждого
// есть перки повышающие шанс своровать
// есть перки позволяющие запугать (чтобы не звал на помощь)
// плюс проверка удачи и репутации
// соответственно воровать - иконка
// запугивание - опция в диалоге
///////////////////////////////
/*
bool GenerateStealItem(int iNation, aref chr, int iType)
{
	//iType = 0 - дипломатические бумаги. губернатор выдает только этот квест.
	//iType = 1 - долговые бумаги (векселя)
	//iType = 2 - драгоценность
	
	
	
	string sIsland = "";
	sIsland = SelectStealItemDestination(iNation, iType);
	if (sIsland == "")
	{
		return false;
	}
	
	string sQuest = "steal_item_for_" + chr.id;
	
	chr.quest.stealitem.island = sIsland;
	chr.quest.stealitem.Nation = iNation;
	
	pchar.quest.(sQuest).win_condition.l1 = "item";
	pchar.quest.(sQuest).win_condition.l1.item = SelectItemForStealQuest(iType, chr);
	pchar.quest.(sQuest).win_condition = "steal_item";
	pchar.quest.(sQuest).character = chr.id;
	chr.quest.stealitem.item = pchar.quest.(sQuest).win_condition.l1.item;
	pchar.quest.stealitem.item = chr.quest.stealitem.item;
	
	pchar.quest.work = 1;
	return true;
}

string SelectStealItemDestination(int iNation, int iType)
{
	
	string sIsland = "";
	string sTempIsland = "";
	int iEnemy = -1;
	int iColonyQuantity = 0;
	int iColonies[MAX_COLONIES];

	for(int i = 0; i <MAX_COLONIES; i++)
	{
		if (colonies[i].nation != "none")
		{
			if (GetNationRelation(sti(Colonies[i].nation), iNation) != RELATION_ENEMY && Colonies[i].id != locations[FindLocation(pchar.location)].fastreload)
			{
					iColonies[iColonyQuantity] = i;
					iColonyQuantity = iColonyQuantity + 1;
			}
		}
	}
	if (iColonyQuantity == 0)
	{
		return "-1";
	}
	int iRand = rand(iColonyQuantity-1);
	
	sTempIsland = iColonies[iRand].id;
		
	return sTempIsland;
}

string SelectItemForStealQuest(int iType, aref chr)
{
	
	int iRandItem;
	string sItem;
	string location;
	
	switch (iType)
	{
		case 0:
			iRandItem = rand(3);	
			sItem = "StealPapers_" + iRandItem;	
			location = chr.quest.stealitem.island + "_townhall";
			SetEnterLocationQuest(location, "steal_townhall_check", 0);
			pchar.quest.stealitem.location = location;
			return sItem;
		break;
		
		case 1:
			sItem = "StealVeksel";
			location = chr.quest.stealitem.island + "_steal_home";
			SetEnterLocationQuest(location, "steal_townhall_check", 0);
			pchar.quest.stealitem.location = location;
			return sItem;
		break;
		
		case 2:
			//их можно продать!
			iRandItem = rand(2);	
			location = chr.quest.stealitem.island + "_steal_home";
			SetEnterLocationQuest(location, "steal_townhall_check", 0);
			pchar.quest.stealitem.location = location;
			sItem = "StealJewelry_" + iRandItem;	
			return sItem;
		break;
	}
}

void ItemStealCheck()
{
	
	ref chr;
	
	int luck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
	if(IsCharacterPerkOn(pchar, "thief"))
	{
		luck = luck + 2;
	}
	luck = luck - rand(12);
	
	string sColony, sNation, sType;
	int iNation, iCharacter;
	
	bool bSteal = false;
	bool bStealQuest = false;
	
	string sStealItem;
	
	for(int i = 0; i <10; i++)
	{
		sStealItem = "Stealitem" + i;
		if (checkAttribute(pchar, sStealItem))
		{
			bSteal = true;
		}
			
	}		
	
	if (sti(pchar.stealmoney) > 0)
	{
		bSteal = true;
	}
	
	if (luck < 0 && bSteal == true && CheckAttribute(&Locations[FindLocation(pchar.location)], "fastreload"))
	{
		sColony = Locations[FindLocation(pchar.location)].fastreload;
		iNation = sti(Colonies[FindColony(sColony)].nation);
		sType = Locations[FindLocation(pchar.location)].type;
		
		if (iNation != PIRATE && sType != "port" && sType != "town")
		{
			iCharacter = LAi_FindNearestNotCompanionCharacter(pchar, 25.0);
			if (iCharacter > 1)
			{
				chr = &characters[iCharacter];
				chr.old.chr_ai.type = chr.chr_ai.type;
				chr.old.dialog.filename = chr.dialog.filename;
				chr.old.dialog.currentnode = chr.dialog.currentnode;
				chr.dialog.filename = "Steal soldier dialog.c";
				for(i = 0; i <10; i++)
				{
					sStealItem = "Stealitem" + i;
					if (checkAttribute(pchar, sStealItem))
					{
						if (IsQuestUsedItem(pchar.(sStealItem)))
						{
							bStealQuest = true;
						}
					}
				}
				if (bStealQuest == true)
				{
					chr.dialog.currentnode = "Steal Quest Item";
				}
				else
				{
					chr.dialog.currentnode = "First Time";
				}
				LAi_SetActorType(chr);
				LAi_ActorDialogNow(chr, pchar, "", 1.0);
			}
		}
		else
		{
			pchar.stealmoney = "0";
			for(i = 0; i <10; i++)
			{
				sStealItem = "Stealitem" + i;
				if (checkAttribute(pchar, sStealItem))
				{
					DeleteAttribute(pchar, sStealItem);
				}
			}
			pchar.quest.check_steal.over = "yes";
		}
	}
	else
	{
		pchar.stealmoney = "0";
		for(int k = 0; k <10; k++)
		{
			sStealItem = "Stealitem" + k;
			if (checkAttribute(pchar, sStealItem))
			{
				DeleteAttribute(pchar, sStealItem);
			}
		}
		pchar.quest.check_steal.over = "yes";
	}
}

string GetShortNationName(int iNation)
{
	string sResult = "";
	
	switch (iNation)
	{
		case ENGLAND:
			sResult = "eng";
		break;
		
		case SPAIN:
			sResult = "spa";
		break;
		
		case FRANCE:
			sResult = "fra";
		break;
		
		case HOLLAND:
			sResult = "hol";
		break;
	}
	
	return sResult;
}

void ReturnStealCharacter(aref chr)
{
	chr.chr_ai.type = chr.old.chr_ai.type;
	chr.dialog.filename = chr.old.dialog.filename;
	chr.dialog.currentnode = chr.old.dialog.currentnode;
}

void CheckSteal()
{
	
	ref chr;
	
	bool bSteal = false;
	string sModel, sNation, sColony;
	int iNation;
	
	sColony = Locations[FindLocation(pchar.location)].townsack;
	iNation = Islands[FindIsland(sColony)].nation;
	
	string sStealItem;
	
	for(int i = 0; i <10; i++)
	{
		sStealItem = "Stealitem" + i;
		if (checkAttribute(pchar, sStealItem))
		{
			bSteal = true;
		}
			
	}		
	
	if (sti(pchar.stealmoney) > 0)
	{
		bSteal = true;
	}
	
	sModel = "soldier_" + GetShortNationName(iNation) + (rand(3)+2);
	chr = LAi_CreateFantomCharacter(sModel, "reload", LAi_FindFreeRandomLocator("goto"));
	chr.id = "Steal_soldier";
	chr.dialog.filename = "Steal soldier dialog.c";
	chr.dialog.currentnode = "uuups";
	chr.nation = iNation;
	PlaceCharacter(chr, "goto", "random");
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, pchar, "", 1.0, 1.0);
	
	sNation = GetNationNameByType(iNation) + "_CITIZENS";
	LAi_group_MoveCharacter(chr, sNation);
}

void StealMoney()
{
	
	int iCost = (sti(pchar.rank) + 10 - sti(pchar.reputation)) * 10;
	string sStealItem;
	
	AddMoneyToCharacter(pchar, -iCost);
	
	for(int i = 0; i <10; i++)
	{
		sStealItem = "Stealitem" + i;
		if (checkAttribute(pchar, sStealItem))
		{
			TakeItemFromCharacter(pchar, pchar.(sStealItem));
		}	
	}
	
	ChangeCharacterReputation(pchar, -1, 7);
}

void StealFight(int iNation, aref chr)
{
	
	
	string sNation = GetNationNameByType(iNation) + "_CITIZENS";
	
	Lai_SetWarriorType(chr);
	LAi_group_MoveCharacter(chr, sNation);
	
	LAI_group_SetRelation(sNation, LAI_GROUP_PLAYER, RELATION_ENEMY);
	LAi_group_FightGroups(sNation, LAI_GROUP_PLAYER, true);
	SetNationRelation2MainCharacter(iNation, RELATION_ENEMY);
	ChangeCharacterReputation(pchar, -3, iNation);
}

void StealItemMissionCompleted(int iNation, aref chr, int iType)
{
	
	if (iType == 0)
	{
		
		
		string career = GetNationNameByType(iNation) + "_line_counter";
		int iMoney = pchar.(career);
		AddMoneyToCharacter(pchar, iMoney);
		TakeItemFromCharacter(pchar, chr.quest.stealitem.item);
		DeleteAttribute(chr, "quest.stealitem");
		DeleteAttribute(pchar, "quest.steal_item");
		pchar.quest.work = 0;
		ChangeCharacterReputation(pchar, 1, iNation);
		ChangeCharacterFame(pchar, 1);
		
		int iPcharNation = sti(pchar.nation);
		SetNationRelationBoth(iNation, iPcharNation, RELATION_ENEMY);
	}
}

void StealItemMissionFailed(aref chr);
{
	
	
	int iNation = sti(chr.quest.StealItem);
	DeleteAttribute(chr, "quest.StealItem");
	DeleteAttribute(pchar, "quest.steal_item");
	pchar.quest.work = 0;
	ChangeCharacterReputation(pchar, -1, iNation);
}

void PlaceItem()
{
	
	string sItem = pchar.quest.stealitem.item
	LoadedLocation.box1.items.(sitem) = 1;
}

void StealQuestItem(aref chr)
{
	
	
	string sNation, sColony;
	int iNation;
	
	ReturnStealCharacter(chr);
	
	sColony = Locations[FindLocation(pchar.location)].townsack;
	iNation = sti(Islands[FindIsland(sColony)].nation);
	
	sNation = GetNationNameByType(iNation) + "_CITIZENS";
	LAi_group_SetRelation(LAI_GROUP_PLAYER, sNation, LAI_GROUP_ENEMY);
	
	SetNationRelation2MainCharacter(iNation, RELATION_ENEMY);
	LAi_group_SetAlarm(sNation, LAI_GROUP_PLAYER, 1.0);
	ChangeCharacterReputation(pchar, -3, iNation);
	
	sNation = GetNationNameByType(iNation) + "_citizens";
	LAi_group_MoveCharacter(chr, sNation);
	LAi_group_SetRelation(LAI_GROUP_PLAYER, sNation, LAI_GROUP_ENEMY);	
	LAi_group_SetAlarm(sNation, LAI_GROUP_PLAYER, 1.0);
}
*/
