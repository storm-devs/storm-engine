
bool CheckForDefendColony(aref chr)
{
	
	int iNation = sti(chr.nation);
	string sColony;
	if(!CheckAttribute(pchar, "quest.defendcolony"))
	{
		for (int i=0; i<MAX_COLONIES; i++)
		{
			if(sti(Colonies[i].nation) == iNation && Colonies[i].agressor != "-1")
			{
				sColony = Colonies[i].id;
				if(!CheckQuestAttribute("defendcolony.colony", sColony))
				{
					chr.quest.defendcolony.colony = sColony;
					chr.quest.defendcolony.nation = Colonies[i].agressor;
					pchar.quest.defendcolony.colony = Colonies[i].id;
					pchar.quest.defendcolony.employer = chr.id;
					return true;
				}
			}
		}
	}
	return false;
}


void DefendColonyMissionAccept(aref chr)
{
	
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";
	pchar.quest.defendcolony_progress = "begin";
	
	chr.quest.work = "1";
	
	string sLogBook = "DefendColony" + chr.id + rand(100000);
	SetQuestHeaderEx(sLogBook, "DefendColony");
	AddQuestRecord(sLogBook, "1");
	
	string sIsland = chr.location;
	sIsland = Locations[FindLocation(sIsland)].fastreload;
	AddQuestUserData(sLogBook, "island", XI_ConvertString(Colonies[FindColony(sIsland)].id));
	AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
	
	sIsland = chr.quest.defendcolony.colony;
	AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("Colony"+Colonies[FindColony(sIsland)].id));
	
	string sNation = GetNationNameByType(sti(chr.quest.defendcolony.nation));
	AddQuestUserData(sLogBook, "sNationGen", XI_ConvertString(sNation + "Gen"));
	pchar.quest.defendcolony.logbook = sLogbook;
}

void FortDefendMissionCompleted(int iNation, aref chr)
{
	string career = GetNationNameByType(iNation) + "_line_counter";
	int iMoney = pchar.(career);
	AddMoneyToCharacter(pchar, iMoney*500);
	
	chr.quest.work = "0";
	DeleteAttribute(chr, "quest.defendcolony");
	DeleteAttribute(pchar, "quest.defendcolony");
	ChangeCharacterFame(pchar, 1);
	ChangeCharacterReputation(pchar, 1, iNation);

	iNation = sti(pchar.nation);
	string sPatent = GetNationNameByType(iNation) + "_career_counter";
	pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
}

void FortDefendMissionNotCompleted(int iNation, aref chr)
{
	DeleteAttribute(chr, "quest.defendcolony");
	DeleteAttribute(pchar, "quest.defendcolony");
	chr.quest.work = "0";
	ChangeCharacterReputation(pchar, -1, iNation);

	iNation = sti(pchar.nation);
	string sPatent = GetNationNameByType(iNation) + "_career_counter";
	pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
}


string GenerateColonyOccupationQuests(int iNation, string sType)
{
	int iColonyQuantity = 0;
	string sTargetFort;
	int iColony;
	int randomizer = 0;
	string subFort;
	
	int i = 0;

	
	int iEnemy = FindEnemyNation2NationWithoutPirates(iNation);
	
	if (iEnemy < 0)
	{
		iEnemy = CreateEnemyForNation(iNation);
	}
		
	
	subFort = FindEnemyColonyForNationExceptPirates(iNation);
	
	if (iEnemy != -1)
	{		
		iColony = FindColony(subFort);
		
		GenerateRumour("colony_occupation", iEnemy, iNation, iColony);
		Colonies[iColony].agressor = iNation;
		sTargetFort = Colonies[iColony].island;
				
		///Квест на появление игрока
		int iQuest = rand(1000000);

		while(iQuest == sti(pchar.time_events_counter))
		{
			iQuest = rand(1000000);
		}
		pchar.time_events_counter = iQuest;
		
		string sQuest = "colony_occupation_wait_player_quest" + iQuest;
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sTargetFort;
		pchar.quest.(sQuest).win_condition = "colony_occupation_wait_player_quest";
		pchar.quest.(sQuest).nation = iNation;
		pchar.quest.(sQuest).enemy = colonies[iColony].nation;
		pchar.quest.(sQuest).colony = sTargetFort;
		
		string sQuest2 = "wait_for_colony_occupation_quest" + iQuest;
		
		string sColony = Colonies[iColony].id;
		int iTimeForEnd = sti(characters[GetCharacterIndex(sColony + " Fort commander")].rank)/2 + sti(characters[GetCharacterIndex(sColony + " Fort commander")].skill.accuracy)/2 + sti(characters[GetCharacterIndex(sColony + " Fort commander")].skill.leadership)/2 + sti(characters[GetCharacterIndex(sColony + " Fort commander")].skill.sneak)/2;
		
		trace ("Colony " + sTargetFort + " will fall throught " + iTimeForEnd + " days.");
		
		pchar.quest.(sQuest2).win_condition.l1 = "timer";
		pchar.quest.(sQuest2).win_condition.l1.date.day = GetAddingDataDay(0, 0, iTimeForEnd);
		pchar.quest.(sQuest2).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iTimeForEnd);
		pchar.quest.(sQuest2).win_condition.l1.date.year = GetAddingDataYear(0, 0, iTimeForEnd);
		pchar.quest.(sQuest2).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest2).win_condition = "end_colonyoccupation_quest";
		pchar.quest.(sQuest2).nation = iNation;
		pchar.quest.(sQuest2).enemy = colonies[iColony].nation;
		pchar.quest.(sQuest2).colony = subFort;
		pchar.quest.(sQuest2).failedquest = sQuest;
		
		return sQuest;
	}
}

void PlayerVisitColonyOccupation(string qname)
{
	int iNation = sti(pchar.quest.(qname).nation);
	int iEnemy = sti(pchar.quest.(qname).enemy);
	string sColony = pchar.quest.(qname).colony;
	string sColonyString;
	string sColonyCommander = sColony + " Fort commander";
			
	int iChar;
	int iRand = 10;
	
	Group_CreateGroup("Colony_Squadron");
	sColonyString = "FightForOtherColony_" + sColony;
	pchar.quest.(sColonyString).colony = sColony;
	pchar.quest.(sColonyString).nation = iNation;
	pchar.quest.(sColonyString).enemy = iEnemy;
	
	string sStep;
	int i, k;
	k = 1;
	for (i=0; i <5; i++)
	{
		if (GetSummonSkillFromName(pchar, SKILL_SNEAK) < rand(2) + iRand)
		{
			iChar = GenerateCharacter(iNation, WITH_SHIP, "officer", MAN, 1, BOSS);
			
			iRand = iRand - 3;
			
			Group_AddCharacter("Colony_Squadron", characters[iChar].id);
			sStep = "l" + k;
			pchar.quest.(sColonyString).win_condition.(sStep) = "npc_death";
			pchar.quest.(sColonyString).win_condition.(sStep).character = iChar;
			pchar.quest.(sColonyString).enemy.(sStep) = iChar;
			k++;
		}
	}
	
	pchar.quest.(sColonyString).win_condition = "Fight_forOtherColony";
	
	Group_SetGroupCommander("Colony_Squadron", characters[iChar].id);
	Group_SetAddress("Colony_Squadron", sColony, "quest_ships","quest_ship_1");
	Group_SetTaskNone("Colony_Squadron");
	
	SetCharacterRelationBoth(iChar,GetCharacterIndex(sColonyCommander),RELATION_ENEMY);
	UpdateRelations();
	
	string sQuest = "escapefrombattlefrocollonymap";
	pchar.quest.(sQuest).win_condition.l1 = "MapEnter";
	pchar.quest.(sQuest).win_condition = "escapefrombattlefrocollony";
	pchar.quest.(sQuest).colony = sColony;
	pchar.quest.(sQuest).nation = iEnemy;
	
	pchar.quest.attack_other_fort.target = sColonyCommander;
	pchar.quest.attack_other_fort.nation = iNation;
	pchar.quest.attack_other_fort.enemy = iEnemy;
	pchar.quest.attack_other_fort.colony = sColony;
	pchar.quest.attack_other_fort.commander = iChar;
	DoQuestCheckDelay("attack_other_fort", 1.0);
	
	iRand = -1;
	int iFlag = 0;
	if(CheckQuestAttribute("defendcolony.type", "multi") || rand(10) > 3)
	{
		Group_CreateGroup("Colony_Defend_Squadron");
		string sStepK;
		for (i=0; i <2; i++)
		{
			if (GetSummonSkillFromName(pchar, SKILL_SNEAK) > rand(2) + iRand)
			{
				iFlag = 1;
				iChar = GenerateCharacter(iNation, WITH_SHIP, "officer", MAN, 1, WARRIOR);
				sStepK = "l"+i;
				pchar.quest.(sColonyString).nonenemy.(sStepK) = iChar;
				iRand = iRand + 3;
				
				Group_AddCharacter("Colony_Defend_Squadron", characters[iChar].id);
			}
		}
		
		if(iFlag == 1)
		{
			Group_SetGroupCommander("Colony_Defend_Squadron", characters[iChar].id);
			Group_SetAddress("Colony_Defend_Squadron", sColony, "reload","Ship_1");
		
			Group_SetTaskAttack("Colony_Defend_Squadron", "Colony_Squadron");
			Group_LockTask("Colony_Defend_Squadron");
			SetCharacterRelationBoth(iChar,GetCharacterIndex(sColonyCommander),RELATION_ENEMY);
			UpdateRelations();
		}
	}
	
	UpdateRelations();
}

void EndColonyOccupation(string qname, int iType)
{
	//iType - 0 событие произошло без участия игрока
	//iType - 1 событие произошло с участием игрока
	
	
	int iRand = rand(1);
	
	int iNation = pchar.quest.(qname).nation;
	int iEnemy = pchar.quest.(qname).enemy;
	string sColony = pchar.quest.(qname).colony;
	int iColony = FindColony(sColony);

	aref EnemyChars;
	makearef(EnemyChars, pchar.quest.(qname).enemy);
	int iChars = GetAttributesNum(EnemyChars);

	int iChar;
	int iShipType;
	string sStep;
	for(int i = 0; i<iChars; i++)
	{
		sStep = "l"+i;
		iChar = sti(pchar.quest.(qname).enemy.(sStep));
		ChangeCharacterAddress(&characters[ichar], "none", "");
		DeleteAttribute(&characters[ichar], "characterlock");
		//EmptyFantomCharacacter(&characters[ichar]);
	}

	makearef(EnemyChars, pchar.quest.(qname).nonenemy);
	iChars = GetAttributesNum(EnemyChars);
	for(i=0; i<iChars; i++)
	{
		sStep = "l"+i;
		iChar = sti(pchar.quest.(qname).enemy.(sStep));
		ChangeCharacterAddress(&characters[ichar], "none", "");
		DeleteAttribute(&characters[ichar], "characterlock");
		//EmptyFantomCharacacter(&characters[ichar]);
	}

	
	if (iType == 0)
	{
		string sFailedQuest = pchar.quest.(qname).failedquest;
		pchar.quest.(sFailedQuest).over = "yes";
	}
	
	if (iType == 0 && iRand == 0)
	{		
		int iCaptureType = rand(2);
		//0 - захватила
		//1 - разграбила
		//2 - практически уничтожила
		if(iCaptureType == 0)
		{
			Event_CaptureColonyByNation(iNation, iEnemy, sColony);
			//GenerateRumour("colony_captured", iNation, iEnemy, iColony);
			ChangeReputationForColony(Colonies[iColony].id, -2);
		}
		
		if(iCaptureType == 1)
		{
			Event_RobbingColonyByNation(iNation, sColony, iEnemy, -1);
		}

		if(iCaptureType == 2)
		{
			Event_DestroyColonyByNation(iNation, sColony, iEnemy, -1);
		}
	}

	if(iType == 1 || iRand == 1)
	{

		if (CheckQuestAttribute("defendcolony.colony", sColony))
		{
			pchar.quest.defendcolony_progress = "completed";
			GenerateRumour("colony_defend", iNation, iEnemy, iColony);
			ChangeReputationForColony(Colonies[iColony].id, 2);
		}
		else
		{
			GenerateRumour("colony_defend", iEnemy, iNation, iColony);
			ChangeReputationForColony(Colonies[iColony].id, -2);
		}
		
		
		//int help = sti(pchar.quest.(qname).help);
		
		if (CheckQuestAttribute(qname+".help", "1") && GetNationRelation2Character(iEnemy, nMainCharacterIndex) != RELATION_ENEMY && !CheckQuestAttribute("defendcolony.colony", sColony))
		{
			AddMoneyToCharacter(pchar, (10000 + GetSummonSkillFromName(pchar, SKILL_COMMERCE)*500));
			ChangeCharacterFame(pchar, 5);
			pchar.quest.(qname).help = 2;
			LaunchAskForHelp();
		}
	}
	Colonies[iColony].agressor = "-1";
	
	pchar.quest.(qname).over = "yes";
	
	pchar.quest.escapefrombattlefrocollonymap.over = "yes";
}

void EscapeFromBattleForColony(string sQuest)
{
	
	
	string sColony = pchar.quest.(sQuest).colony;
	int iEnemy = sti(pchar.quest.(sQuest).nation);
	int iNation = sti(colonies[FindColony(sColony)].nation);
	Event_CaptureColonyByNation(iEnemy, iNation, sColony);
	
	Group_SetAddress("Colony_Squadron", "none", "quest_ships","quest_ship_1");
	Group_SetAddress("Colony_Defend_Squadron", "none", "quest_ships","quest_ship_1");
	
	if (CheckAttribute(pchar, "quest.defendcolony.colony"))
	{
		if (pchar.quest.defendcolony.colony == sColony)
		{
			pchar.quest.defendcolony.colony;
			
			pchar.quest.defendcolony_progress = "captured";
		}
	}
}

void DestroyRandomBuildingsForThisLevel(int iColony)
{
	if(colonies[iColony].nation != "none")
	{
		if(sti(colonies[iColony].colonylevel) == 1)
		{
			if(sti(colonies[iColony].shipyard) == 1)
			{
				colonies[iColony].shipyard = rand(1);
			}
			if(sti(colonies[iColony].wheatfield) == 1)
			{
				colonies[iColony].wheatfield = rand(1);
			}
			if(sti(colonies[iColony].fishingpier) == 1)
			{
				colonies[iColony].fishingpier = rand(1);
			}
			if(sti(colonies[iColony].mill) == 1)
			{
				colonies[iColony].mill = rand(1);
			}
		}

		if(sti(colonies[iColony].colonylevel) == 2)
		{
			if(sti(colonies[iColony].shipyard) == 2)
			{
				colonies[iColony].shipyard = rand(1)+1;
			}
			if(sti(colonies[iColony].huts) == 1)
			{
				colonies[iColony].huts = rand(1);
			}
			if(sti(colonies[iColony].storehouse) == 1)
			{
				colonies[iColony].storehouse = rand(1);
			}
		}

		if(sti(colonies[iColony].colonylevel) == 3)
		{
			if(sti(colonies[iColony].shipyard) == 3)
			{
				colonies[iColony].shipyard = rand(2)+1;
			}
			if(sti(colonies[iColony].church) == 1)
			{
				colonies[iColony].church = rand(1);
			}
			if(sti(colonies[iColony].prison) == 1)
			{
				colonies[iColony].prison = rand(1);
			}
		}

		if(sti(colonies[iColony].colonylevel) == 4)
		{
			if(sti(colonies[iColony].academy) == 1)
			{
				colonies[iColony].academy = rand(1);
			}
		}
		BuildUpgrade(iColony, UPGRADE_PIER, false);
		BuildUpgrade(iColony, UPGRADE_MILL, false);
		BuildUpgrade(iColony, UPGRADE_SHIPYARD, false);
		BuildUpgrade(iColony, UPGRADE_HUTS, false);
		BuildUpgrade(iColony, UPGRADE_ACADEMY, false);
		BuildUpgrade(iColony, UPGRADE_STOREHOUSE, false);
		BuildUpgrade(iColony, UPGRADE_CHURCH, false);
	}
}

void DestroyRandomBuildingsWithRaiseOneLevel(int iColony)
{
	int isRaise = 0;

	if(sti(colonies[iColony].colonylevel) == 1)
	{
		if(sti(colonies[iColony].shipyard) == 1)
		{
			colonies[iColony].shipyard = rand(1);
		}
		if(sti(colonies[iColony].wheatfield) == 1)
		{
			colonies[iColony].wheatfield = rand(1);
		}
		if(sti(colonies[iColony].fishingpier) == 1)
		{
			colonies[iColony].fishingpier = rand(1);
		}
		if(sti(colonies[iColony].mill) == 1)
		{
			colonies[iColony].mill = rand(1);
		}
	}

	if(sti(colonies[iColony].colonylevel) == 2)
	{
		if(sti(colonies[iColony].shipyard) == 1)
		{
			colonies[iColony].shipyard = rand(1);
			isRaise = 1;
		}
		if(sti(colonies[iColony].wheatfield) == 1)
		{
			colonies[iColony].wheatfield = rand(1);
			isRaise = 1;
		}
		if(sti(colonies[iColony].fishingpier) == 1)
		{
			colonies[iColony].fishingpier = rand(1);
			isRaise = 1;
		}
		if(sti(colonies[iColony].mill) == 1)
		{
			colonies[iColony].mill = rand(1);
			isRaise = 1;
		}

		if(sti(colonies[iColony].shipyard) == 2)
		{
			colonies[iColony].shipyard = rand(1)+1;
		}
		if(sti(colonies[iColony].huts) == 1)
		{
			colonies[iColony].huts = rand(1);
		}
		if(sti(colonies[iColony].storehouse) == 1)
		{
			colonies[iColony].storehouse = rand(1);
		}
	}

	if(sti(colonies[iColony].colonylevel) == 3)
	{
		if(sti(colonies[iColony].shipyard) == 2)
		{
			colonies[iColony].shipyard = rand(1)+1;
			isRaise = 1;
		}
		if(sti(colonies[iColony].huts) == 1)
		{
			colonies[iColony].huts = rand(1);
			isRaise = 1;
		}
		if(sti(colonies[iColony].storehouse) == 1)
		{
			colonies[iColony].storehouse = rand(1);
			isRaise = 1;
		}

		if(sti(colonies[iColony].shipyard) == 3)
		{
			colonies[iColony].shipyard = rand(2)+1;
		}
		if(sti(colonies[iColony].church) == 1)
		{
			colonies[iColony].church = rand(1);
		}
		if(sti(colonies[iColony].prison) == 1)
		{
			colonies[iColony].prison = rand(1);
		}
	}

	if(sti(colonies[iColony].colonylevel) == 4)
	{
		if(sti(colonies[iColony].shipyard) == 3)
		{
			colonies[iColony].shipyard = rand(2)+1;
			isRaise = 1;
		}
		if(sti(colonies[iColony].church) == 1)
		{
			colonies[iColony].church = rand(1);
			isRaise = 1;
		}
		if(sti(colonies[iColony].prison) == 1)
		{
			colonies[iColony].prison = rand(1);
			isRaise = 1;
		}

		if(sti(colonies[iColony].academy) == 1)
		{
			colonies[iColony].academy = rand(1);
		}
	}

	colonies[iColony].colonylevel = sti(colonies[iColony].colonylevel) - isRaise;

	BuildUpgrade(iColony, UPGRADE_PIER, false);
	BuildUpgrade(iColony, UPGRADE_MILL, false);
	BuildUpgrade(iColony, UPGRADE_SHIPYARD, false);
	BuildUpgrade(iColony, UPGRADE_HUTS, false);
	BuildUpgrade(iColony, UPGRADE_ACADEMY, false);
	BuildUpgrade(iColony, UPGRADE_STOREHOUSE, false);
	BuildUpgrade(iColony, UPGRADE_CHURCH, false);
}

void DestroyRandomBuildingsWithRaiseAllLevels(int iColony)
{
	if(sti(colonies[iColony].colonylevel) > 1)
	{
		colonies[iColony].colonylevel = 1;
	}
	SetUpgradesForColony(iColony);
}



void AttackOtherFort()
{
	string sColonyCommander = pchar.quest.attack_other_fort.target;
	int iColonyAggressor = sti(pchar.quest.attack_other_fort.commander);
	
	int iNation = sti(pchar.quest.attack_other_fort.nation);
	int iEnemy = sti(pchar.quest.attack_other_fort.enemy);
	string sColony = pchar.quest.attack_other_fort.colony;

	trace ("sColonyCommander is " + sColonyCommander);

	for (int i =0; i <iNumShips; i++)
	{
		if (checkAttribute(&characters[Ships[i]], "SeaAI.Group.Name"))
		{
			if (characters[Ships[i]].SeaAI.Group.Name == "Colony_Squadron")
			{
				Ship_SetTaskAttack(PRIMARY_TASK, Ships[i], GetCharacterIndex(sColonyCommander));
			}
		}
	}
	
	if (GetNationRelation2Character(iEnemy, nMainCharacterIndex) != RELATION_ENEMY)
	{
		if(!CheckQuestAttribute("defendcolony.colony", sColony))
		{
			LaunchAskForHelp();
		}
	}
}

void DefendColonyMissionRefuse(aref chr)
{
	
	
	pchar.quest.defendcolony.colony = "";
	
	pchar.quest.defendcolony_progress = "";
	
	chr.quest.work = "0";
}