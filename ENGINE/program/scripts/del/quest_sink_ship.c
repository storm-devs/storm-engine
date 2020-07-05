/*
int GenerateSinkShipQuest(aref chr, string slogbook, int iscontinue, int ishovernor, int isfightpossible, int iscaptureship, int stepsquantity)
{
	trace("Generate Sink Quest Begin");

	int iQuest;

	string sItem = "-1";
	int iChar2 = -1;

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		if(sti(Quests[iQuest].l.type) == SINK_SHIP_QUEST)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	/// ”¡»–¿≈Ã 0 »«  ŒÀ»◊≈—“¬¿ ÿ¿√Œ¬
	//////////////////////////////////////////////////////////////////////////////
	stepsquantity = stepsquantity + 1;

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÀŒÕ»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	string sBeginColony = Locations[FindLocation(pchar.location)].fastreload;

	string sTargetColony = FindRandomColonyExceptColony(sBeginColony);

	int iTargetNation = FindEnemyNation2Nation(iNation);
	//int iTargetNation = sti(colonies[FindColony(sTargetColony)].nation);

	if (sTargetColony == "")
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã »— ŒÃ€…  Œ–¿¡À‹
	//////////////////////////////////////////////////////////////////////////////
	int iChar = GenerateCharacter(iTargetNation, WITH_SHIP, "citizen", MAN, 1, WARRIOR);
	
	//////////////////////////////////////////////////////////////////////////////
	/// ≈—À»  Œ–¿¡À‹ Õ¿ƒŒ «¿’¬¿“»“‹ »Ÿ≈Ã ÷≈À‹
	//////////////////////////////////////////////////////////////////////////////
	if(iscaptureship == 1)
	{
		int itype = rand(2);
		if(itype == 0)
		{
			iChar2 = GenerateCharacter(iNation, WITHOUT_SHIP, "citizen", MAN, 1, WARRIOR);
		}
		else
		{
			sItem = SelectItemForCaptureQuest(itype);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã ƒ»—“¿Õ÷»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * (1.0 + (0.3 * stepsquantity));

	//////////////////////////////////////////////////////////////////////////////
	/// Œœ–≈ƒ≈Àﬂ≈Ã Õ¿√–¿ƒ”
	//////////////////////////////////////////////////////////////////////////////

	int iCost = 500 + (iDistance * 20);


	iCost = iCost * GetQuestPriceFromDifficulty();

	if(iCost < 500)
	{
		trace("what the fuck?! Cost for sink ship quest less then 500 gil");
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
	Quests[iQuest].id = "SinkShip" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.shipchar = characters[iChar].id;
	if(iChar2 != -1)
	{
		Quests[iQuest].l.capturechar = characters[iChar2].id;
	}
	else
	{
		Quests[iQuest].l.capturechar = "-1";
	}
	Quests[iQuest].l.item = sItem;
	Quests[iQuest].l.stepsquantity = stepsquantity;
	Quests[iQuest].l.allies = isfightpossible;
	Quests[iQuest].l.enemies = rand(3);
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.taken = "-1";
	Quests[iQuest].l.time = iDistance;
	Quests[iQuest].l.logbook = slogbook;
	Quests[iQuest].l.iscontinue = iscontinue;
	Quests[iQuest].l.target_colony = sTargetColony;
	Quests[iQuest].l.target_island = colonies[FindColony(sTargetColony)].island;
	Quests[iQuest].l.parent_colony = sBeginColony;
	Quests[iQuest].l.contribution = 0;
	Quests[iQuest].l.ishovernor = ishovernor;
	Quests[iQuest].l.isfight = isfightpossible;

	chr.CurrentQuest.accepted = false;
	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = SINK_SHIP_QUEST;

	Quests[iQuest].l.type = SINK_SHIP_QUEST;

	for (int i = 0; i <isfightpossible; i++)
	{ 
		iChar = GenerateCharacter(iTargetNation, WITH_SHIP, "officer", MAN, 1, WARRIOR);
		SetCompanionIndex(CharacterFromID(Quests[iQuest].l.shipchar), -1, iChar);
	}

	return 1;// "give quest" flag on
}

void AcceptSinkShipQuest(aref chr)
{
	chr.CurrentQuest.accepted = true;
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";
	DeleteAttribute(chr, "CurrentQuest.lastdialog");

	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;
	pchar.quests.(sQuest) = iQuest;

	int idays = sti(Quests[iQuest].l.time);

	sQuest = "sink_ship_quest_timer_for_" + chr.id + "_" + rand(10000);
	chr.CurrentQuest.pquest = sQuest;
	pchar.quest.(sQuest).win_condition.l1 = "timer";
	pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
	pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
	pchar.quest.(sQuest).win_condition = "sink_ship_quest_timer_failed";
	pchar.quest.(sQuest).quest = iQuest;

	string sColony = Quests[iQuest].l.target_colony;
	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	int iMoney = sti(Quests[iQuest].l.money);
	string sIsland = Quests[iQuest].l.target_island;
	string sBeginColony = Quests[iQuest].l.parent_colony;

	string sLogBook = Quests[iQuest].l.logbook;

	sQuest = "prepare_sink_ship_quest_for_" + chr.id + "_" + rand(10000);
	pchar.quest.(sQuest).win_condition.l1 = "location";
	pchar.quest.(sQuest).win_condition.l1.location = sIsland;
	pchar.quest.(sQuest).win_condition = "PrepareForSinkShip";
	pchar.quest.(sQuest).function = "VisitShipSinkLocation";
	pchar.quest.(sQuest).quest = iQuest;

	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;

	string sCaptureChar = Quests[iQuest].l.capturechar;
	string sItem = Quests[iQuest].l.item;
	int iChar = GetCharacterIndex(Quests[iQuest].l.shipchar);

	characters[iChar].ship.upper_model = "quest_signs\ship_sign";

	int iscaptureship;
	if(sCaptureChar == "-1" && sItem == "-1")
	{
		iscaptureship = 0;
	}
	else
	{
		iscaptureship = 1;
	}

	if(iscaptureship == 0)
	{
		sQuest = "complete_sink_ship_quest_for_" + chr.id + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
		pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex(Quests[iQuest].l.shipchar);
		pchar.quest.(sQuest).win_condition = "Sink_Quest_pirate";
		pchar.quest.(sQuest).function = "ShipSinkForQuest";
		pchar.quest.(sQuest).quest = iQuest;


		if(sLogBook == "")
		{
			sLogBook = "SINK_SHIP_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "SINK_SHIP_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sShipName", characters[iChar].ship.name);
			Quests[iQuest].l.logbook = sLogBook;
		}
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "1");

		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
		AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
		AddQuestUserData(sLogBook, "period", sPeriod);
		AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		AddQuestUserData(sLogBook, "sShipName", characters[iChar].ship.name);
	}
	else
	{
		if(sCaptureChar != "-1")
		{
			characters[GetCharacterIndex(Quests[iQuest].l.shipchar)].quest.sink_quest.passenger = sCaptureChar;
		}
		if(sItem != "-1")
		{
			characters[GetCharacterIndex(Quests[iQuest].l.shipchar)].quest.sink_quest.item = sItem;
		}

		sQuest = "complete_capture_ship_quest_for_" + chr.id + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "Ship_capture";
		pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex(Quests[iQuest].l.shipchar);
		pchar.quest.(sQuest).win_condition = "Capture_Quest_pirate";
		pchar.quest.(sQuest).fail_condition.l1 = "NPC_Death";
		pchar.quest.(sQuest).fail_condition.l1.character = GetCharacterIndex(Quests[iQuest].l.shipchar);
		pchar.quest.(sQuest).fail_condition = "Capture_Quest_pirate_failed";
		pchar.quest.(sQuest).quest = iQuest;

		if(sLogBook == "")
		{
			sLogBook = "CAPTURE_SHIP_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "CAPTURE_SHIP_QUEST");
			Quests[iQuest].l.logbook = sLogBook;
			//AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
		}
		AddQuestRecordEx(sLogBook, "CAPTURE_SHIP_QUEST", "1");

		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
		//AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
		AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
		AddQuestUserData(sLogBook, "period", sPeriod);
		AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		AddQuestUserData(sLogBook, "sShipName", characters[iChar].ship.name);
	}

	if(ishovernor == 0)
	{
		AddQuestUserData(sLogBook, "who", XI_ConvertString("whotrader"));
	}
	else
	{
		AddQuestUserData(sLogBook, "who", XI_ConvertString("whohovernor"));
	}

	NPCGiveQuest(chr);
	//chr.CurrentQuest.pquest = sQuest;
	if(ishovernor == 1)
	{
		QuestsShablons[SINK_SHIP_QUEST].l.taken = "1";
	}

	//chr.quest.questflag = 3;
	//chr.quest.questflag.model = SetModelFlag(chr);
	CheckQuestForCharacter(chr);


    if(sti(Quests[iQuest].l.stepsquantity) > 0)
	{
		int iQuestDistance = idays / sti(Quests[iQuest].l.stepsquantity);
		sQuest = "Sink_Ship_travel_" + Quests[iQuest].l.shipchar + "_for_" + chr.id + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, iQuestDistance);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, iQuestDistance);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, iQuestDistance);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "sink_ship_travel";
		pchar.quest.(sQuest).function = "SinkShipTravel";
		pchar.quest.(sQuest).quest = iQuest;
	}
}

void SinkShipTravel(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	string sCharacter = Quests[iQuest].l.shipchar;
	ref chr = CharacterFromID(sCharacter);
	
	int stepsquantity = sti(Quests[iQuest].l.stepsquantity);
	if(stepsquantity > 0)
	{
		stepsquantity = stepsquantity - 1;
		string sBeginColony = Quests[iQuest].l.target_colony;
		string sTargetColony = FindRandomColonyExceptColony(sBeginColony);
		
		chr.mapEnc.type = "trade";
		chr.mapEnc.worldMapShip = "quest_ship";
		chr.mapEnc.quest = iQuest;

		Map_CreateTrader(sBeginColony, sTargetColony, chr.id);

		Quests[iQuest].l.target_colony = sTargetColony;
		Quests[iQuest].l.target_island = colonies[FindColony(sTargetColony)].island;
		Quests[iQuest].l.stepsquantity = stepsquantity;

		GenerateRumour("ship_info", GetCharacterIndex(sCharacter), FindColony(sBeginColony), FindColony(sTargetColony));
	}
	else
	{
		string sShipChar = Quests[iQuest].l.shipchar;
		int iShipChar = GetCharacterIndex(sShipChar);

		if(!LAi_IsDead(&characters[iShipChar]))
		{
			int iNation = sti(characters[iShipChar].nation);
			int iAllies = sti(Quests[iQuest].l.allies);
			
			string sGroup = "QuestSinkShip"+rand(100000);
			Group_CreateGroup(sGroup);
			Group_AddCharacter(sGroup, characters[iShipChar].id);
			string sLocator = "quest_ship_" + (rand(2) + 4);
			Group_SetAddress(sGroup, pchar.location, "quest_ships", sLocator);
			Group_SetGroupCommander(sGroup, characters[iShipChar].id);
			if(GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY)
			{
				Group_SetTaskMove(sGroup, 100000, 100000);
			}
			else
			{
				Group_SetTaskAttack(sGroup, PLAYER_GROUP);
			}
			Group_LockTask(sGroup);

			string sLogBook = Quests[iQuest].l.logbook;

			if(iAllies == 0)
			{
				AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "4");
			}
			if(iAllies > 1)
			{
				AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "6");
			}
		}
	}
}

string SelectItemForCaptureQuest(int iType)
{
	int iRandItem = rand(3);
	string sItem = "-1";
	switch (iType)
	{
		case 0: 
			sItem = "CaptureShipLetter_" + iRandItem; 
		break;
		case 1: 
			sItem = "CaptureShipWifePapers_" + iRandItem;
		break;
		case 2:
			sItem = "CaptureShipPapers_" + iRandItem;
		break;
	}
	
	return sItem;
}

void VisitShipSinkLocation(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sCurrentLocation = pchar.location;
	string sTargetLocation = Quests[iQuest].l.target_island;

	if(sCurrentLocation != sTargetLocation)
	{
		RechargeShipSinkQuest(iQuest);
	}
	else
	{
		string sShipChar = Quests[iQuest].l.shipchar;
		int iShipChar = GetCharacterIndex(sShipChar);

		if(!LAi_IsDead(&characters[iShipChar]))
		{
			int iNation = sti(characters[iShipChar].nation);
			int iAllies = sti(Quests[iQuest].l.allies);
			
			string sGroup = "QuestSinkShip"+rand(100000);
			Group_CreateGroup(sGroup);
			Group_AddCharacter(sGroup, characters[iShipChar].id);
			string sLocator = "quest_ship_" + (rand(2) + 4);
			Group_SetAddress(sGroup, pchar.location, "quest_ships", sLocator);
			Group_SetGroupCommander(sGroup, characters[iShipChar].id);
			if(GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY)
			{
				Group_SetTaskMove(sGroup, 100000, 100000);
			}
			else
			{
				Group_SetTaskAttack(sGroup, PLAYER_GROUP);
			}
			Group_LockTask(sGroup);

			string sLogBook = Quests[iQuest].l.logbook;

			if(iAllies == 0)
			{
				AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "4");
			}
			else
			{
				AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "6");
			}
		}
	}
}

void SinkShipQuestLooseBattleExitToMap(string sQuest)
{
	
	int iQuest = sti(pchar.quest.(sQuest).quest);
	
	string sShipCHar = Quests[iQuest].l.shipchar;
	int iChr = GetCharacterIndex(sShipCHar);
	DeleteAttribute(&characters[iChr], "characterlock");
	ChangeCharacterAddress(&characters[iChr], "none", "");
	//EmptyFantomCharacacter(characterFromID(sShipCHar));
	DestroySinkShipQuest(iQuest, 1);
}

void RechargeShipSinkQuest(int iQuest)
{
	string sEmployer = Quests[iQuest].l.parent_npc;

	string sQuest = "prepare_sink_ship_quest_for_" + sEmployer + "_" + rand(10000);

	string sIsland = Quests[iQuest].l.target_island;

	pchar.quest.(sQuest).win_condition.l1 = "location";
	pchar.quest.(sQuest).win_condition.l1.location = sIsland;
	pchar.quest.(sQuest).win_condition = "PrepareForSinkShip";
	pchar.quest.(sQuest).quest = iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "5");
}

void ShipSinkForQuest(string sQuest)
{
	
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sCaptureChar = Quests[iQuest].l.capturechar;
	string sItem = Quests[iQuest].l.item;

	string sLogBook = Quests[iQuest].l.logbook;

	int iContribution = sti(Quests[iQuest].l.contribution);
	if(iContribution > 0)
	{
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "15");
	}
	else
	{
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "16");
	}

	//Quests[iQuest].l.taken = 2;
	string sEmployer = Quests[iQuest].l.parent_npc;
	int iChar = GetCharacterIndex(sEmployer);
	characters[iChar].currentquest.completed = 1;

	sQuest = characters[GetCharacterIndex(sEmployer)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	pchar.quest.SinkShipQuest_LooseBattle_ExitToMap.over = "true";

	string sShipChar = Quests[iQuest].l.shipchar;
	Map_ReleaseQuestEncounter(sShipChar);

	FoundAndDeleteAllQuestForQuestNumber(iQuest);

}

void ShipFailedCaptureForQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sLogBook = Quests[iQuest].l.logbook;

	AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "13");

	//string sAntiQuest = pchar.quest.(sQuest).antiquest;

	//pchar.quest.(sAntiQuest).over = true;
	pchar.quest.SinkShipQuest_LooseBattle_ExitToMap.over = "true";
	DestroySinkShipQuest(iQuest, 0);
	string sShipChar = Quests[iQuest].l.shipchar;
	Map_ReleaseQuestEncounter(sShipChar);

	FoundAndDeleteAllQuestForQuestNumber(iQuest);
}

void ShipCaptureForQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sCaptureChar = Quests[iQuest].l.capturechar;
	string sItem = Quests[iQuest].l.item;

	string sLogBook = Quests[iQuest].l.logbook;

	string sEmployer = Quests[iQuest].l.parent_npc;
	int iChar = GetCharacterIndex(sEmployer);
	characters[iChar].currentquest.completed = 1;

	if (sCaptureChar != "-1")
	{
		int iPassenger = GetCharacterIndex(sCaptureChar);
		AddPassenger(pchar, &characters[iPassenger], 1);
		string sColony = Quests[iQuest].l.parent_colony;
		characters[iPassenger].isprisonquest = 1;
		characters[iPassenger].employer = pchar.quest.(sQuest).employer;
		characters[iPassenger].colony = sColony;
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "11");
	}

	if(sItem != "-1")
	{
		GiveItem2Character(pchar, sItem);
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "12");
	}

	sQuest = characters[GetCharacterIndex(sEmployer)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;
	pchar.quest.SinkShipQuest_LooseBattle_ExitToMap.over = "true";

	string sShipChar = Quests[iQuest].l.shipchar;
	Map_ReleaseQuestEncounter(sShipChar);

	FoundAndDeleteAllQuestForQuestNumber(iQuest);
}

void CompleteShipSinkQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	string sQuest = "l"+iQuest;

	int isumm = sti(Quests[iQuest].l.money);
	int icontributtion = sti(Quests[iQuest].l.contribution);

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);

	if(icontributtion > 0)
	{
		isumm = isumm - (isumm*icontributtion*5/100);
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "10");
		AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
	}
	else
	{
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "2");
	}

	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, 1, 6);
	}
	else
	{
		int iNation = sti(pChr.nation);
		ChangeCharacterReputation(pchar, 1, iNation);
		int iCareerReward = sti(QuestsShablons[SINK_SHIP_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, iCareerReward);
		pChr.career.change = iCareer;
		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
	}

	AddMoneyToCharacter(pchar, isumm);
	AddPartyExp(pchar, isumm/10);
	sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(pchar, "quests." + sQuest);

	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}

	DeleteAttribute(pChr, "CurrentQuest");
	EraseQuestFreeState(pChr);

	if(CheckAttribute(&Quests[iQuest], "l.contribution.quest"))
	{
		sQuest = Quests[iQuest].l.contribution.quest;
		pchar.quest.(sQuest).over = true;
	}

	string sCapturedChar = Quests[iQuest].l.capturechar;
	string sItem = Quests[iQuest].l.item;

	if(sCapturedChar != "-1")
	{
		RemovePassenger(pchar, characterFromID(sCapturedChar));
	}
	if(sItem != "-1")
	{
		TakeItemFromCharacter(pchar, sItem);
	}

	string sChr = Quests[iQuest].l.shipchar;
	int iChr = GetCharacterIndex(sChr);
	int iShip = sti(characters[iChr].ship.type);
	int iPass = GetPassengersQuantity(pchar);
	int isdelete = 1;
	for(int i = 0; i < iPass; i++)
	{
		if(iChr == GetPassenger(pchar, i))
		{
			isdelete = 0;
		}
	}
	if(isdelete == 1)
	{
		DeleteAttribute(&characters[iChr], "characterlock");
		ChangeCharacterAddress(&characters[iChr], "none", "");
		//EmptyFantomCharacacter(&characters[iChr]);
	}
	

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));
	
	QuestsShablons[SINK_SHIP_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);

	pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	if(ishovernor == 1)
	{
		GenerateHovernorQuests(chr);
		chr.quest.questflag.model = SetModelFlag(chr);
	}

}

void SinkShipTimerFailed(string sQuest)
{
	RechargeSinkShipPunishment(sQuest, 1);

	int iQuest = sti(pchar.quest.(sQuest).quest);
	int isFirstTime = sti(Quests[iQuest].l.contribution);
	string sParentChar = Quests[iQuest].l.parent_npc;
	int iNation = sti(characters[GetCharacterIndex(sParentChar)].nation);

	string sLogBook = Quests[iQuest].l.logbook;
	if(isFirstTime == 1)
	{
		ChangeCharacterReputation(pchar, -1, iNation);
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "9");
	}
}

void RechargeSinkShipPunishment(string sQuest, int iMode)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	
	if(sti(Quests[iQuest].l.contribution) == 19)
	{
		DestroySinkShipQuest(iQuest, 0);
	}
	else
	{
		string sParentNPC = Quests[iQuest].l.parent_npc;

		sQuest = "sink_ship_quest_timer_for_" + sParentNPC + "_rand_"+rand(10000);
		
		characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest = sQuest;

		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "sink_ship_quest_timer_failed";
		pchar.quest.(sQuest).function = "SinkShipTimerFailed";
		pchar.quest.(sQuest).quest = iQuest;

		Quests[iQuest].l.contribution = sti(Quests[iQuest].l.contribution) + 1;
		Quests[iQuest].l.contribution.quest = sQuest;
	}
}

void DestroySinkShipQuest(int iQuest, int iMode)
{
	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);

	if(iMode == 0)
	{
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "3");
	}
	else
	{
		AddQuestRecordEx(sLogBook, "SINK_SHIP_QUEST", "14");
	}


	sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(pchar, "quests." + sQuest);

	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}

	DeleteAttribute(pChr, "CurrentQuest");
	EraseQuestFreeState(pChr);


	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, -5, 6);
	}
	else
	{
		int iNation = sti(pChr.nation);
		ChangeCharacterReputation(pchar, -1, iNation);
		int iCareerReward = sti(QuestsShablons[SINK_SHIP_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
		pChr.career.change = iCareer;
		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
	}

	string sChr = Quests[iQuest].l.shipchar;
	int iChr = GetCharacterIndex(sChr);
	DeleteAttribute(&characters[iChr], "characterlock");
	ChangeCharacterAddress(&characters[iChr], "none", "");
	//EmptyFantomCharacacter(&characters[iChr]);

	Quests[iQuest].id = "-1";

	QuestsShablons[SINK_SHIP_QUEST].l.taken = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	EmptyAllQuestData(iQuest);
}

int CheckForSinkShipSuccess(aref chr)
{
	if(CheckAttribute(chr, "CurrentQuest.type"))
	{
		int iQuest = sti(chr.CurrentQuest);
		int iType = sti(chr.CurrentQuest.type);
		if(iType == SINK_SHIP_QUEST)
		{
			if(CheckAttribute(chr, "CurrentQuest.accepted"))
			{
				if(chr.CurrentQuest.accepted == false)
				{
					return 1;
				}
			}
			string sEmployer = Quests[iQuest].l.parent_npc;
			int iChar = GetCharacterIndex(sEmployer);
			if(CheckAttribute(&characters[iChar], "currentquest.completed"))
			{
				if(sti(characters[iChar].currentquest.completed) != 1)
				{
					return 3;// disabled "take quest" flag on
				}
			}
			else
			{
				return 3;// disabled "take quest" flag on
			}

			if(sti(Quests[iQuest].l.contribution) == 0)
			{
				return 2;// "take quest" flag on
			}
			else
			{
				chr.quest.punishment = Quests[iQuest].l.contribution;
				return 2;// "take quest" flag on
			}
		}
	}
	return -1;
}
*/
