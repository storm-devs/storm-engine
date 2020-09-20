// to_do  del
/*
int GenerateCaptureFortQuest(aref chr)
{
	trace("Fort Quest Begin");

	int iQuest;

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		if(sti(Quests[iQuest].l.type) == CAPTURE_FORT_QUEST && sti(Quests[iQuest].l.target_colony) != -1)
		{
			return 1;
		}
		else
		{
			return -1;
		}
		
	}

	string slogbook = "Career";

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÀŒÕ»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	string sBeginColony = Locations[FindLocation(pchar.location)].fastreload;

	string sTargetColony = FindEnemyColonyForNationExceptPirates(iNation);

	if (sTargetColony == "-1")
	{
		sTargetColony = FindNonEnemyColonyForNationExceptColonyAndNation(sBeginColony, iNation);
	}

	if (sTargetColony == "-1")
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÃ¿Õƒ»–¿ ‘Œ–“¿
	//////////////////////////////////////////////////////////////////////////////
	string sChar = sTargetColony + " Fort Commander";

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã ƒ»—“¿Õ÷»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;
	iDistance = iDistance + iGameArcade * 4;

	//////////////////////////////////////////////////////////////////////////////
	/// Œœ–≈ƒ≈Àﬂ≈Ã Õ¿√–¿ƒ”
	//////////////////////////////////////////////////////////////////////////////

	int iCost = 1000 + (iDistance * 50) + sti(characters[sChar].Fort.Cannons.Quantity) * 500;

	iCost = iCost * GetQuestPriceFromDifficulty();

	if(iCost < 5000)
	{
		iCost = 5000;
		trace("what the fuck?! Cost for fort quest less then 5000 gil");
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã —¬Œ¡ŒƒÕ€…  ¬≈—“
	//////////////////////////////////////////////////////////////////////////////
	iQuest = FindFirstEmptyQuest();
	if(iQuest == -1)
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// «¿œŒÀÕﬂ≈Ã œŒÀﬂ
	//////////////////////////////////////////////////////////////////////////////
	Quests[iQuest].id = "FortQuest_For_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.fortchar = sChar;
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.condition = "-1";
	Quests[iQuest].l.time = iDistance;
	Quests[iQuest].l.logbook = slogbook;
	Quests[iQuest].l.target_colony = sTargetColony;
	Quests[iQuest].l.parent_colony = sBeginColony;
	Quests[iQuest].l.contribution = 0;
	
	int iLuck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
	int iRand = rand(12);
	if(iLuck > iRand)
	{
		Quests[iQuest].l.allies = rand(3); //‰‡. ÚÓÊÂ ÏÓÊÂÚ ·˚Ú¸ 0
	}
	else
	{
		Quests[iQuest].l.allies = 0;
	}

	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = CAPTURE_FORT_QUEST;

	Quests[iQuest].l.type = CAPTURE_FORT_QUEST;

	return 1;
}


void AcceptCaptureFortQuest(aref chr)
{
	DeleteAttribute(chr, "CurrentQuest.lastdialog");
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";

	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;
	pchar.quests.(sQuest) = iQuest;

	Quests[iQuest].l.condition = "taken"; 

	int idays = sti(Quests[iQuest].l.time);

	sQuest = "capture_fort_quest_timer_for_" + chr.id + "_" + rand(10000);
	chr.CurrentQuest.pquest = sQuest;
	pchar.quest.(sQuest).win_condition.l1 = "timer";
	pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
	pchar.quest.(sQuest).win_condition = "capture_fort_quest_timer_failed";
	pchar.quest.(sQuest).function = "CaptureFortQuestFailed";
	pchar.quest.(sQuest).quest = iQuest;

	string sColony = Quests[iQuest].l.target_colony;
	int iMoney = sti(Quests[iQuest].l.money);
	string sIsland = colonies[FindColony(sColony)].island;
	string sBeginColony = Quests[iQuest].l.parent_colony;
	string sTargetColony = Quests[iQuest].l.target_colony;

	string sLogBook = Quests[iQuest].l.logbook;

	sQuest = "prepare_capture_fort_quest_for_" + chr.id + "_" + rand(10000);
	pchar.quest.(sQuest).win_condition.l1 = "location";
	pchar.quest.(sQuest).win_condition.l1.location = sIsland;
	pchar.quest.(sQuest).win_condition = "PrepareCaptureFort";
	pchar.quest.(sQuest).function = "PrepareCaptureFort";
	pchar.quest.(sQuest).quest = iQuest;

	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;

	//int iChar = GetCharacterIndex(Quests[iQuest].l.fortchar);

	sQuest = "complete_capture_fort_quest_for_" + chr.id + "_" + rand(10000);
	pchar.quest.(sQuest).win_condition.l1 = "Fort_capture";
	pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex(Quests[iQuest].l.fortchar);
	pchar.quest.(sQuest).win_condition = "CaptureFort";
	pchar.quest.(sQuest).function = "FortCapturedForQuest";
	pchar.quest.(sQuest).quest = iQuest;

	AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "1");

	AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
	AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
	AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
	AddQuestUserData(sLogBook, "period", sPeriod);
	AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));	

	AddQuestUserData(sLogBook, "who", XI_ConvertString("whohovernor"));
	string sNationGen = GetNationNameByType(sti(chr.id));
	AddQuestUserData(sLogBook, "sNationGen", XI_ConvertString(sNationGen + "Gen"));

	NPCGiveQuest(chr);

	//chr.quest.questflag = 3;
	//chr.quest.questflag.model = SetModelFlag(chr);
	CheckQuestForCharacter(chr);
}

void FortCapturedForQuest(string sQuestName)
{
	int iQuest = sti(pchar.quest.(sQuestName).quest);
	pchar.quest.fortinterface.colony = Quests[iQuest].l.target_colony;
	pchar.quest.fortinterface.colony.nation = characters[GetCharacterIndex(Quests[iQuest].l.parent_npc)].nation;
	pchar.quest.fortinterface.colony.logbook = Quests[iQuest].l.logbook;
	pchar.quest.fortinterface.colony.quest = iQuest;
	Quests[iQuest].l.condition = "done";
}

void FortCapturedForQuestFromInterface(int iNation, string sColony)
{
	if(CheckAttribute(pchar, "quest.fortinterface.colony"))
	{
		if(sColony == pchar.quest.fortinterface.colony)
		{
			if(iNation == sti(pchar.quest.fortinterface.colony.nation))
			{
				string slogbook = pchar.quest.fortinterface.colony.logbook;
				AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "2");
				ChangeCharacterFame(pchar, 7);
				ChangeReputationForColony(sColony, -2);
			}
		}
	}
}

bool CheckForQuest(string sColony, int iNation)
{
	if(CheckAttribute(pchar, "quest.fortinterface.colony"))
	{
		if(sColony == pchar.quest.fortinterface.colony)
		{
			if(iNation == sti(pchar.quest.fortinterface.colony.nation))
			{
				return true;
			}
		}
	}
	return false;
}

void PrepareCaptureFort(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	int iAllies = sti(Quests[iQuest].l.allies);
	int ichar = GetCharacterIndex(Quests[iQuest].l.parent_npc);
	int iNation = sti(characters[iChar].nation);
	int iFortCharacter = GetCharacterIndex(Quests[iQuest].l.fortchar);
	
	string sGroup = "AlliesForCaptureFortQuest"+rand(100000);
	Group_CreateGroup(sGroup);

	for (int i = 0; i <iAllies; i++)
	{ 
		iChar = GenerateCharacter(iNation, WITH_SHIP, "officer", MAN, 0, WARRIOR);
		Group_AddCharacter(sGroup, characters[iChar].id);
		Ship_SetTaskAttack(PRIMARY_TASK, iChar, iFortCharacter);
		int iRelation = GetNationRelation2MainCharacter(iNation);
		SetCharacterRelationBoth(pchar, iChar, iRelation);
		SetCharacterRelationBoth(iFortCharacter, iChar, RELATION_ENEMY);
	}
	Group_SetGroupCommander(sGroup, characters[iChar].id);
	UpdateRelations();
}

void CompleteCaptureFortQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	string sQuest = "l"+iQuest;

	int isumm = sti(Quests[iQuest].l.money);
	int icontributtion = sti(Quests[iQuest].l.contribution);

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = chr.id;

	if(icontributtion > 0)
	{
		isumm = isumm - (isumm*icontributtion*5/100);
		AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "5");
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(sParentNPC));
	}
	else
	{
		AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "2");
	}

	int iNation = sti(chr.nation);
	ChangeCharacterReputation(pchar, 1, iNation);
	int iCareerReward = sti(QuestsShablons[CAPTURE_FORT_QUEST].l.careerreward);
	int iCareer = ChangeCharacterCareerProgress(iNation, iCareerReward);
	chr.career.change = iCareer;

	AddMoneyToCharacter(pchar, isumm);
	AddPartyExp(pchar, isumm/10);
	sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(pchar, "quests." + sQuest);

	DeleteAttribute(chr, "CurrentQuest");
	EraseQuestFreeState(chr);

	if(CheckAttribute(&Quests[iQuest], "l.contribution.quest"))
	{
		sQuest = Quests[iQuest].l.contribution.quest;
		pchar.quest.(sQuest).over = true;
	}


	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));
	
	QuestsShablons[CAPTURE_FORT_QUEST].l.taken = "-1";
	QuestsShablons[CAPTURE_FORT_QUEST].l.condition = "-1";

	EmptyAllQuestData(iQuest);

	pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

	iNation = sti(pchar.nation);
	string sPatent = GetNationNameByType(iNation) + "_career_counter";
	pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;

	GenerateHovernorQuests(chr);
	chr.quest.questflag.model = SetModelFlag(chr);

	if(sti(pchar.nation) != PIRATE)
	{
		bool isGameDone = true;
		for(int i = 0; i < MAX_COLONIES; i++)
		{
			if(colonies[i].nation == "none")
			{
				continue;
			}
			if(sti(colonies[i].nation) != sti(pchar.nation))
			{
				isGameDone = false;
			}
		}

		if(isGameDone == true)
		{
			sQuest = "EndGame";
			pchar.quest.(sQuest).win_condition.l1 = "nation_location";
			pchar.quest.(sQuest).win_condition.l1.nation = sti(pchar.nation);
			pchar.quest.(sQuest).win_condition = "EndGame";
		}
	}
}

int CheckForCaptureFortSuccess(aref chr)
{
	int iResult = -1;
	if(CheckAttribute(chr, "CurrentQuest.type"))
	{
		int iQuest = sti(chr.CurrentQuest);
		int iQuestType = sti(chr.CurrentQuest.type);
		if(iQuestType == CAPTURE_FORT_QUEST && sti(Quests[iQuest].l.type) == CAPTURE_FORT_QUEST)
		{
			if(Quests[iQuest].l.condition == "done" || Quests[iQuest].l.condition == "failed")
			{
				iResult = 2;//success
			}
			if(Quests[iQuest].l.condition == "-1")
			{
				iResult = 1;
			}
			if(Quests[iQuest].l.condition == "taken")
			{
				iResult = 3;
			}
		}
	}
	return iResult;
}

void CaptureFortQuestFailed(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sLogBook = Quests[iQuest].l.logbook;
	AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "3");

	Quests[iQuest].l.condition = "failed";
	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	int iNation = sti(pchar.nation);
	string sPatent = GetNationNameByType(iNation) + "_career_counter";
	pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
}

void DeleteCaptureFortQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = chr.id;

	AddQuestRecordEx(sLogBook, "CAPTURE_FORT_QUEST", "4");
	AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(sParentNPC));

	DeleteAttribute(pchar, "quests." + sQuest);

	DeleteAttribute(chr, "CurrentQuest");
	EraseQuestFreeState(chr);
	
	int iNation = sti(chr.nation);
	ChangeCharacterReputation(pchar, -1, iNation);
	int iCareerReward = sti(QuestsShablons[CAPTURE_FORT_QUEST].l.careerreward);
	int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
	chr.career.change = iCareer;

	Quests[iQuest].id = "-1";

	QuestsShablons[CAPTURE_FORT_QUEST].l.taken = "-1";
	QuestsShablons[CAPTURE_FORT_QUEST].l.condition = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	EmptyAllQuestData(iQuest);

	GenerateHovernorQuests(chr);
	chr.quest.questflag.model = SetModelFlag(chr);
}
*/
