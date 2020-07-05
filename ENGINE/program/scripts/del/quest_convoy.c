
int GenerateEscortQuest(aref chr, int isdelayed, string logbook, int iscontinue, int isfightpossible, int ishovernor, int isnewship)
{
	trace("Generate Escort Quest Begin");
	
	int iTradeType;
	int iTradeQuantity;
	string sTargetColony;
	string sBeginColony;
	int iQuest;
	int iChar;

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		if(sti(Quests[iQuest].l.type) == ESCORT_SHIP_QUEST)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	int iCompanionQunatity = GetCompanionQuantity(pchar);
	if(iCompanionQunatity >= COMPANION_MAX)
	{
		return -1;
	}


	if(QuestsShablons[ESCORT_SHIP_QUEST].l.taken == "1")
	{
		return -1;
	}

	if(ishovernor == 0 && GetCharacterReputation(pchar, 6) < 10)
	{
		return -1;
	}
	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		iTradeType = sti(Quests[iQuest].l.goods);
		iTradeQuantity = sti(Quests[iQuest].l.goods.quantity);
		if(GetSquadronFreeSpace(pchar, iTradeType) < iTradeQuantity)
		{
			return -1;
		}
		if(sTargetColony == "")
		{
			return -1;
		}
		/*
		isdelayed = sti(Quests[iQuest].l.isdelayed);
		if(isdelayed == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
		*/
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÀŒÕ»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	sBeginColony = Locations[FindLocation(pchar.location)].fastreload;
	sTargetColony = FindNonEnemyColonyForNationExceptColony(sBeginColony);

	if (sTargetColony == "")
	{
		return -1;
	}


	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã “Œ¬¿–
	//////////////////////////////////////////////////////////////////////////////
	if(isnewship == 0)
	{
		iTradeType = SearchForTrade(sBeginColony, sTargetColony, chr, 0);

		if(iTradeType == -1)
		{
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////////
		/// »Ÿ≈Ã  ŒÀ»◊≈—“¬Œ “Œ¬¿–¿
		//////////////////////////////////////////////////////////////////////////////
		iChar = sti(chr.index);		
		iTradeQuantity = GetSquadronFreeSpace(&characters[iChar], iTradeType);
		AddCharacterGoods(&characters[iChar], iTradeType, iTradeQuantity);
	}
	else
	{
		iChar = GenerateCharacter(iNation, WITH_SHIP, "trader", MAN, 1, TRADER);
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã ƒ»—“¿Õ÷»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;

	//////////////////////////////////////////////////////////////////////////////
	/// Œœ–≈ƒ≈Àﬂ≈Ã Õ¿√–¿ƒ”
	//////////////////////////////////////////////////////////////////////////////
	if(isnewship == 0)
	{
		int curStoreIdx = FindStore(sTargetColony);
		int iCost = GetStoreGoodsPrice(&Stores[curStoreIdx],iTradeType,PRICE_TYPE_SELL,&characters[iChar],1)
		iCost = (iTradeQuantity*iCost/sti(goods[iTradeType].units))/3.0 + iTradeQuantity*(sti(characters[iChar].skill.commerce))/5);
	}
	else
	{
		iCost = rand(50) + rand(50) + 1500;
	}

	if(isdelayed == 1)
	{
		iDistance = iDistance * 2;
	}

	//iDistance = iDistance + 2;

	iCost = iCost + iDistance * 2;

	iCost = iCost * GetQuestPriceFromDifficulty();

	/*if(isfightpossible > 0)
	{
		iCost = iCost * (isfightpossible + 1);
	}*/

	if(iCost < 3000)
	{
		iCost = 2500 + rand(500) + rand(500);
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

	Quests[iQuest].id = "ConvoyShipFor_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.shipchar = characters[iChar].id;
	Quests[iQuest].l.isnewship = isnewship;
	Quests[iQuest].l.encounters = "";
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.isdelayed = isdelayed;
	Quests[iQuest].l.time = iDistance;
	Quests[iQuest].l.logbook = logbook;
	Quests[iQuest].l.iscontinue = iscontinue;
	Quests[iQuest].l.target_colony = sTargetColony;
	Quests[iQuest].l.parent_colony = sBeginColony;
	Quests[iQuest].l.contribution = 0;
	Quests[iQuest].l.swapped = 0;
	Quests[iQuest].l.ishovernor = ishovernor;
	Quests[iQuest].l.isfight = isfightpossible;

	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.accepted = false;
	chr.CurrentQuest.type = ESCORT_SHIP_QUEST;
	Quests[iQuest].l.type = ESCORT_SHIP_QUEST;

	if(isHovernor == 0)
	{
		chr.quest.questflag = 1;
		chr.quest.questflag.model = SetModelFlag(chr);
	}

	return 1;
} 

void AcceptEscortQuest(aref chr)
{
	chr.CurrentQuest.accepted = true;
	chr.currentquest.completed = 0;
	DeleteAttribute(chr, "CurrentQuest.lastdialog");
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";
	int iQuest = sti(chr.currentquest);

	string sShipChar = Quests[iQuest].l.shipchar;
	int iShipChar = GetCharacterIndex(sShipChar);
	int isDelayed = sti(Quests[iQuest].l.isdelayed);
	int isswapped = sti(Quests[iQuest].l.swapped);

	string sLogBook = Quests[iQuest].l.logbook;

	NPCGiveQuest(chr);

	int isfightpossible = sti(Quests[iQuest].l.isfight);

	string sQuest;
	string sBeginColony = Quests[iQuest].l.parent_colony;
	string sTargetColony = Quests[iQuest].l.target_colony;
	string sIsland = colonies[FindColony(sTargetColony)].island;

	/*if(isfightpossible > 0)
	{
		sQuest = "encounter_for_escort_ship_quest_char_" + sShipChar + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "ComeToIsland";
		pchar.quest.(sQuest).win_condition = "fight_on_convoy_quest";
		pchar.quest.(sQuest).quest = iQuest;
	}*/

	characters[iShipChar].characterlock = 1;
	LAi_RemoveLoginTime(&characters[iShipChar]);
	
	if(isdelayed == 0)
	{
		SetCompanionIndex(pchar, -1, iShipChar);
		SetShipRemovable(&characters[iShipChar], 0);
		characters[iShipChar].location = "none";
	}

	if(isswapped == 0)
	{
		///////////////////////////////////////////////////////////////////
		/// Œ·‡·ÓÚÍ‡ Á‡ı‚‡Ú‡ ËÎË ÔÓÚÓÔÎÂÌËˇ ÒÛ‰Ì‡
		///////////////////////////////////////////////////////////////////
		sQuest = "capture_escort_ship_char_" + sShipChar + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "Ship_Capture";
		pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex(sShipChar);
		pchar.quest.(sQuest).win_condition = "EscortTradeShip_ship_captured";
		pchar.quest.(sQuest).fail_condition.l1 = "NPC_Death";
		pchar.quest.(sQuest).fail_condition.l1.character = GetCharacterIndex(sShipChar);
		pchar.quest.(sQuest).fail_condition = "EscortTradeShip_ship_sinked";
		pchar.quest.(sQuest).quest = iQuest;

		sQuest = "escort_ship_timer_for_char_" + sShipChar + "_" + rand(10000);
		chr.CurrentQuest.pquest = sQuest;
		int idays = sti(Quests[iQuest].l.time);
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "EscortQuestTimer_Failed";
		pchar.quest.(sQuest).quest = iQuest;

		sQuest = "escort_ship_sucess_for_char_" + sShipChar + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sTargetColony + "_tavern";
		pchar.quest.(sQuest).win_condition = "EscortTradeShip_sucess";
		pchar.quest.(sQuest).quest = iQuest;
	}
	else
	{
		sQuest = "escort_ship_sucess_for_char_" + sShipChar + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sBeginColony + "_townhall";
		pchar.quest.(sQuest).win_condition = "EscortTradeShip_sucess";
		pchar.quest.(sQuest).quest = iQuest;
	}

	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;
	int iMoney = sti(Quests[iQuest].l.money);

	if(isDelayed == 0)
	{
		if(sLogBook == "")
		{
			sLogBook = "CONVOY_SHIP_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "CONVOY_SHIP_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sShipName", characters[iShipChar].ship.name);
			Quests[iQuest].l.logbook = sLogBook;
		}
		if(isswapped == 0)
		{
			AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "1");

			AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
			AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
			AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
			AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
			AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
			AddQuestUserData(sLogBook, "period", sPeriod);
			AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
			AddQuestUserData(sLogBook, "sShipName", characters[iShipChar].ship.name);
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_CONVOY_SHIP_QUEST", "10");
		}
	}
	else
	{
		if(sLogBook == "")
		{
			sLogBook = "DELAY_CONVOY_SHIP_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELAY_CONVOY_SHIP_QUEST_FOR_");
			AddQuestUserDataForTitle(sLogBook, "sShipName", characters[iShipChar].ship.name);
			Quests[iQuest].l.logbook = sLogBook;
		}
		AddQuestRecordEx(sLogBook, "DELAY_CONVOY_SHIP_QUEST", "1");

		AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
		AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
		AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
		AddQuestUserData(sLogBook, "period", sPeriod);
		AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		AddQuestUserData(sLogBook, "sShipName", characters[iShipChar].ship.name);
	}

	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(isswapped == 0)
	{
		characters[iShipChar].quest.convoyquest.colony = sTargetColony;
		characters[iShipChar].quest.convoyquest.date = sPeriod;
		characters[iShipChar].quest.convoyquest.money = iMoney;
		if(ishovernor == 1)
		{
			AddQuestUserData(sLogBook, "who", XI_ConvertString("whohovernor"));
		}
		else
		{
			AddQuestUserData(sLogBook, "who", XI_ConvertString("whotrader"));
		}
	}
	else
	{
		characters[iShipChar].quest.convoyquest.colony = sBeginColony;
		characters[iShipChar].quest.convoyquest.date = sPeriod;
		characters[iShipChar].quest.convoyquest.money = iMoney;
	}

	QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "1";

	if(chr.id != sShipChar)
	{
		//chr.quest.questflag = 3;
		//chr.quest.questflag.model = SetModelFlag(chr);
		CheckQuestForCharacter(chr);
	}
	else
	{
		chr.quest.questflag.model = "";
		DeleteAttribute(chr, "quest.questflag");
	}
}

void FightOnConvoyQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).Quest);

	int iChar;

	string sEnemyGroup = "QuestEscortShipEnemy"+rand(100000);
	Group_CreateGroup(sEnemyGroup);
	int iNation = FindEnemyNation2Nation(iNation);
	int iEnemies = Quests[iQuest].l.isfight;
	for (int i = 0; i <iEnemies; i++)
	{ 
		iChar = GenerateCharacter(iNation, WITH_SHIP, "pirate", MAN, 0, WARRIOR);
		Group_AddCharacter(sEnemyGroup, characters[iChar].id);
	}
		
	Group_SetGroupCommander(sEnemyGroup, characters[iChar].id);
	SetCharacterRelationBoth(nMainCharacterIndex, iChar, RELATION_ENEMY);
	Group_SetPursuitGroup(sEnemyGroup, PLAYER_GROUP);
	Group_SetAddress(sEnemyGroup, pchar.location, "", "");

	Group_SetTaskAttack(sEnemyGroup, PLAYER_GROUP);
	Group_LockTask(sEnemyGroup);
	UpdateRelations();

	string sLogbook = Quests[iQuest].l.logbook;

	AddQuestRecordEx(sLogbook, "CONVOY_SHIP_QUEST", "8");
}

void FailedEscortTradeShipQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	RechargeEscortPunishment(iQuest);

	int isFirstTime = sti(Quests[iQuest].l.contribution);
	string sParentChar = Quests[iQuest].l.parent_npc;
	int iNation = sti(characters[GetCharacterIndex(sParentChar)].nation);

	string sLogBook = Quests[iQuest].l.logbook;
	if(isFirstTime == 1)
	{
		ChangeCharacterReputation(pchar, -1, iNation);
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "6");
	}
}

void RechargeEscortPunishment(int iQuest)
{
	if(sti(Quests[iQuest].l.contribution) == 19)
	{
		DestroyEscortShipQuest(iQuest, 1);
	}
	else
	{
		string sParentNPC = Quests[iQuest].l.parent_npc;

		string sQuest = "capture_escort_ship_timer_for_char_" + sParentNPC + "_rand_"+rand(10000);
		
		characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest = sQuest;

		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "EscortQuestTimer_Failed";
		pchar.quest.(sQuest).quest = iQuest;

		Quests[iQuest].l.contribution = sti(Quests[iQuest].l.contribution) + 1;
		Quests[iQuest].l.contribution.quest = sQuest;
	}
}

void DeleteEscortQuest(aref chr)
{
	int iQuest = sti(chr.currentquest);

	FoundAndDeleteAllQuestForQuestNumber(iQuest);
	Quests[iQuest].id = "-1";
	DeleteAttribute(&Quests[iQuest], "l"));
	QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "-1";
	EmptyAllQuestData(iQuest);

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	LAi_CharacterDisableDialog(chr);
	DeleteAttribute(chr, "characterlock");
	ChangeCharacterAddress(chr, "none", "");

}

void DestroyEscortShipQuest(int iQuest, int iType)
{
	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);

	if(iType == 1)
	{
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "3");
	}
	if(iType == 2)
	{
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "4");
	}
	if(iType == 3)
	{
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "5");
	}


	sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	string sShipChar = Quests[iQuest].l.shipchar;
	SetShipRemovable(characterFromID(sShipCHar), 1);
	RemoveCharacterCompanion(pchar, characterFromID(sShipChar));

	DeleteAttribute(characterFromID(sShipChar), "quest.convoyquest");

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
		int iCareerReward = sti(QuestsShablons[ESCORT_SHIP_QUEST].l.careerreward);
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
	//EmptyFantomCharacacter(characterFromID(sShipChar));

	Quests[iQuest].id = "-1";
	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
}

void SucessEscortTradeShipQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).Quest);

	int iChar;

	int isumm = sti(Quests[iQuest].l.money);
	int icontributtion = sti(Quests[iQuest].l.contribution);

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);
	string sShipChar = Quests[iQuest].l.shipchar;
	
	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	int isswapped = sti(Quests[iQuest].l.swapped);
	int isdelayed = sti(Quests[iQuest].l.isdelayed);

	if(isdelayed == 0 && isswapped == 1)
	{
		pChr.currentquest.completed = 1;
		CheckQuestForCharacter(pChr);
		/*SetShipRemovable(characterFromID(sShipChar), 1);
		RemoveCharacterCompanion(pchar, characterFromID(sShipChar));
		DeleteAttribute(characterFromID(sShipChar), "quest.convoyquest");

		if(ishovernor == 0)
		{
			if(icontributtion > 0)
			{
				isumm = isumm - (isumm*icontributtion*5/100);
				AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "7");
				AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(icontributtion));
			}
			else
			{
				AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "2");
			}

			ChangeCharacterReputation(pchar, 1, 6);

			AddMoneyToCharacter(pchar, isumm);
			AddPartyExp(pchar, isumm/10);
					
			int isContinue = sti(Quests[iQuest].l.iscontinue);
			if(isContinue == 0)
			{
				CloseQuestHeader(sLogBook);
			}

			DeleteAttribute(pChr, "CurrentQuest");
			EraseQuestFreeState(pChr);

			sShipChar = Quests[iQuest].l.shipchar;
			DeleteAttribute(characterFromID(sShipChar), "characterlock");
			ChangeCharacterAddress(characterFromID(sShipChar), "none", "");

			if(CheckAttribute(&Quests[iQuest], "l.contribution.quest"))
			{
				sQuest = Quests[iQuest].l.contribution.quest;
				pchar.quest.(sQuest).over = true;
			}

			Quests[iQuest].id = "-1";

			DeleteAttribute(&Quests[iQuest], "l"));

			EmptyAllQuestData(iQuest);
		}
		else
		{
			AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "9");
			
			iChar = GetCharacterIndex(sParentNPC);
			characters[iChar].currentquest.completed = 1;
		}

		sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
		pchar.quest.(sQuest).over = true;

		DeleteAttribute(pchar, "quests." + sQuest);

		QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "-1";
		*/
	}
	else
	{

		iChar = GetCharacterIndex(sShipChar);
		PlaceCharacter(&characters[iChar], "sit", "random");
		characters[iChar].dialog.filename = "Passenger_dialog.c";
		characters[iChar].greeting = "cit_complete_" + (rand(3) + 1);
		LAi_SetSitType(&characters[iChar]);
		//LAi_ActorDialog(&characters[iChar], pchar, "", 1000.0, 1.0);

		if(isswapped == 0 && isdelayed == 0)
		{
			characters[iChar].dialog.currentnode = "Convoy_completed";
		}
		else
		{
			characters[iChar].dialog.currentnode = "Convoy_delayed";
			Quests[iQuest].l.swapped = 1;
			Quests[iQuest].l.isdelayed = 0;
		}
		characters[iChar].quest.questflag = 2;
		characters[iChar].quest.questflag.model = SetModelFlag(&characters[iChar]);


		characters[iChar].CurrentQuest = iQuest;
		characters[iChar].CurrentQuest.type = ESCORT_SHIP_QUEST;
	}

}

void SinkedEscortTradeShipQuest(string sQuest)
{
	int iQuest = pchar.quest.(sQuest).quest;

	DestroyEscortShipQuest(iQuest, 2);
	EmptyAllQuestData(iQuest);
}

void CapturedEscortTradeShipQuest(string sQuest)
{
	int iQuest = pchar.quest.(sQuest).quest;
	
	pchar.vilecheater = sti(pchar.vilecheater) + 1;

	DestroyEscortShipQuest(iQuest, 3);
	EmptyAllQuestData(iQuest);
}

int SelectShipForEscortQuest()
{
	int iShip = sti(pchar.ship.type);
	int iClass = realships[iShip].Class;

	object PossibleShips[8];
	int PossibleShipsArray[8];

	PossibleShips[0].ship = SHIP_SLOOP;
	PossibleShips[0].rank = 6;

	PossibleShips[1].ship = SHIP_SCHOONER;
	PossibleShips[1].rank = 5;

	PossibleShips[2].ship = SHIP_BARQUE;
	PossibleShips[2].rank = 5;

	PossibleShips[3].ship = SHIP_CARAVEL;
	PossibleShips[3].rank = 5;

	PossibleShips[4].ship = SHIP_FLEUT;
	PossibleShips[4].rank = 4;

	PossibleShips[5].ship = SHIP_GALEON_L;
	PossibleShips[5].rank = 4;

	PossibleShips[6].ship = SHIP_GALEON_H;
	PossibleShips[6].rank = 3;

	PossibleShips[7].ship = SHIP_PINNACE;
	PossibleShips[7].rank = 3;

	int m = 0;
	for(int i = 0; i< 8; i++)
	{
		if(sti(PossibleShips[i].rank) >= iClass)
		{
			PossibleShipsArray[m] = sti(PossibleShips[i].ship);
			m++;
		}
	}

	int iResult = -1;
	if(m > 0)
	{
		iResult = rand(m - 1);
	}
	else
	{
		return -1;
	}

	return PossibleShipsArray[iResult];
}

int CheckForEscortShipSuccess(aref chr)
{
	if(CheckAttribute(chr, "CurrentQuest.type"))
	{
		int iQuest = sti(chr.CurrentQuest);
		int iType = sti(chr.CurrentQuest.type);
		if(iType == ESCORT_SHIP_QUEST)
		{
			string sEmployer = Quests[iQuest].l.parent_npc;
			int iChar = GetCharacterIndex(sEmployer);
			if(CheckAttribute(&characters[iChar], "currentquest.completed"))
			{
				if(sti(characters[iChar].currentquest.completed) != 1 && chr.id == sEmployer)
				{
					return 3;// disabled "take quest" flag on
				}
			}
			else
			{
				if(CheckAttribute(&characters[iChar], "CurrentQuest.accepted"))
				{
					if(characters[iChar].CurrentQuest.accepted == false)
					{
						return 1;
					}
				}
				//return 3;// disabled "take quest" flag on
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
	return - 1;

}

void HalfCompleteEscortTradeShipQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	int ishovernor = Quests[iQuest].l.ishovernor;

	if(ishovernor == 0)
	{
		CompleteEscortTradeShipQuest(chr);
		return;
	}

	string sEmployer = Quests[iQuest].l.parent_npc;

	characters[GetCharacterIndex(sEmployer)].currentquest.completed = 1;

	RemoveCharacterCompanion(pchar, chr);

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	//LAi_SetActorType(chr);
	LAi_CharacterDisableDialog(chr);
	DeleteAttribute(chr, "characterlock");
	string sLogbook = Quests[iQuest].l.logbook;
	AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "9");

	FoundAndDeleteAllQuestForQuestNumber(iQuest);
}

void CompleteEscortTradeShipQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);

	int isumm = sti(Quests[iQuest].l.money);

	AddPartyExp(pchar, isumm/2);

	int icontributtion = sti(Quests[iQuest].l.contribution);

	string sLogBook = Quests[iQuest].l.logbook;

	//string sParentNPC = Quests[iQuest].l.parent_npc;
	//ref pChr = CharacterFromID(sParentNPC);
	string sQuest;
	string sShipChar = Quests[iQuest].l.shipchar;
	
	int ishovernor = Quests[iQuest].l.ishovernor;
	int isswapped = sti(Quests[iQuest].l.swapped);
	int isdelayed = sti(Quests[iQuest].l.isdelayed);

	if(isdelayed == 0)
	{
		if(GetCharacterIndex(sShipChar) != -1)
		{
			LAi_CharacterDisableDialog(characterFromID(sShipChar));
			SetShipRemovable(characterFromID(sShipChar), 1);
			RemoveCharacterCompanion(pchar, characterFromID(sShipChar));
			DeleteAttribute(characterFromID(sShipChar), "quest.convoyquest");
			DeleteAttribute(characterFromID(sShipChar), "characterlock");
		}
	}
	if(icontributtion > 0)
	{
		isumm = isumm - (isumm*icontributtion*5/100);
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "7");
		AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
	}
	else
	{
		AddQuestRecordEx(sLogBook, "CONVOY_SHIP_QUEST", "2");
	}

	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, 1, 6);
	}
	else
	{
		int iNation = sti(chr.nation);
		ChangeCharacterReputation(pchar, 1, iNation);
		int iCareerReward = sti(QuestsShablons[ESCORT_SHIP_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, iCareerReward);
		chr.career.change = iCareer;

		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
	}

	AddMoneyToCharacter(pchar, isumm);
			
	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}

	sQuest = chr.CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(chr, "CurrentQuest");
	EraseQuestFreeState(chr);

	//sShipChar = Quests[iQuest].l.shipchar;
	//DeleteAttribute(characterFromID(sShipChar), "characterlock");
	//ChangeCharacterAddress(characterFromID(sShipChar), "none", "");

	if(CheckAttribute(&Quests[iQuest], "l.contribution.quest"))
	{
		sQuest = Quests[iQuest].l.contribution.quest;
		pchar.quest.(sQuest).over = true;
	}

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	EmptyAllQuestData(iQuest);


	DeleteAttribute(pchar, "quests." + sQuest);

	pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "-1";
	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	if(ishovernor == 1)
	{
		GenerateHovernorQuests(chr);
		chr.quest.questflag.model = SetModelFlag(chr);
	}
}

int CheckForConvoyFreeStack()
{
	
	int iCompanions = GetCompanionQuantity(pchar);

	if(iCompanions < COMPANION_MAX)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void DelayEscortQuest(aref npchar)
{
	Lai_SetSitType(npchar);
}