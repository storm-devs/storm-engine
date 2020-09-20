/*
int GenerateDeliveryItemQuest(aref chr, bool isdelayed, string logbook, int iscontinue, int ishovernor, int isfightpossible)
{
	//iType = 0 - äîñòàâêà ñóíäóêà
	//iType = 1 - äîñòàâêà ïèñüìà ñ ñîþçîì ïðîòèâ âðàãà
	//iType = 2 - äîñòàâêà ïèñüìà ñ ïðåäëîæåíèåì ïåðåìèðèÿ

	trace("Generate DeliveryItem Quest Begin");

	
	int iTradeType;
	int iTradeQuantity;
	string sTargetColony;
	string sBeginColony;
	int iQuest;
	string sItem = "-1";

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		//isdelayed = sti(Quests[iQuest].l.isdelayed);
		if(sti(Quests[iQuest].l.type) == DELIVERY_ITEM_QUEST)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÊÎËÎÍÈÞ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	sBeginColony = Locations[FindLocation(pchar.location)].fastreload;

	int isEnemyPresent = 0;
	if(isfightpossible > 0)
	{
		sTargetColony = FindEnemyColonyForNation(iNation);
		if(FindColony(sTargetColony) == -1)
		{
			sTargetColony = FindNonEnemyColonyForNationExceptColony(sBeginColony);
		}
		else
		{
			isEnemyPresent = 1;
		}
	}
	else
	{
		sTargetColony = FindNonEnemyColonyForNationExceptColony(sBeginColony);
	}

	if (sTargetColony == "")
	{
		return -1;
	}
	if(FindColony(sTargetColony) == -1)
	{
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÏÐÅÄÌÅÒ
	//////////////////////////////////////////////////////////////////////////////
	sItem = SelectItemForDeliveryItemQuest(isEnemyPresent);

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÄÈÑÒÀÍÖÈÞ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;

	if(isdelayed == 1)
	{
		iDistance = iDistance*2;
	}

	iDistance = iDistance + 2;

	int iCost = iCost + iDistance * 20;
	iCost = iCost * GetQuestPriceFromDifficulty();
	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÑÂÎÁÎÄÍÛÉ ÊÂÅÑÒ
	//////////////////////////////////////////////////////////////////////////////
	iQuest = FindFirstEmptyQuest();
	if(iQuest == -1)
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÇÀÏÎËÍßÅÌ ÏÎËß
	//////////////////////////////////////////////////////////////////////////////

	Quests[iQuest].id = "DeliveryItem_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.money = iCost;
	
	int iChar = GenerateCharacter(iNation, WITHOUT_SHIP, "citizen", MAN, 1, WARRIOR);
	characters[iChar].CurrentQuest = iQuest;
	Quests[iQuest].l.target_npc = characters[iChar].id;
	
	Quests[iQuest].l.isfightpossible = isfightpossible;
	Quests[iQuest].l.item = sItem;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.isdelayed = isdelayed;
	Quests[iQuest].l.taken = "-1";
	Quests[iQuest].l.time = iDistance;
	Quests[iQuest].l.logbook = logbook;
	Quests[iQuest].l.iscontinue = iscontinue;
	Quests[iQuest].l.target_colony = sTargetColony;
	Quests[iQuest].l.parent_colony = sBeginColony;
	Quests[iQuest].l.contribution = 0;
	Quests[iQuest].l.swapped = 0;
	Quests[iQuest].l.ishovernor = ishovernor;
	Quests[iQuest].l.questtype = DELIVERY_ITEM_QUEST;

	chr.CurrentQuest.accepted = false;
	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = DELIVERY_ITEM_QUEST;

	Quests[iQuest].l.type = DELIVERY_ITEM_QUEST;

	return 1; 
}

void AcceptDeliveryItemQuest(aref chr)
{
	chr.CurrentQuest.accepted = true;
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";
	DeleteAttribute(chr, "CurrentQuest.lastdialog");
	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;
	pchar.quests.(sQuest) = iQuest;

	int idays = sti(Quests[iQuest].l.time);
	int isswapped = sti(Quests[iQuest].l.swapped);
	if(isswapped == 0)
	{
		sQuest = "delivery_item_quest_timer_for_" + chr.id + "_" + rand(10000);
		chr.CurrentQuest.pquest = sQuest;
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "deliveryitemquesttimer_failed";
		pchar.quest.(sQuest).quest = iQuest;
	}

	string sTargetColony = Quests[iQuest].l.target_colony;
	string sBeginColony = Quests[iQuest].l.parent_colony;
	
	int isdelayed = sti(Quests[iQuest].l.isdelayed);
	int ishovernor = sti(Quests[iQuest].l.ishovernor);

	string sItem = Quests[iQuest].l.item;

    string sLogBook = Quests[iQuest].l.logbook;

	int iMoney = sti(Quests[iQuest].l.money);
	string sIsland = colonies[FindColony(sTargetColony)].island;
	
	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;

	if(isdelayed == 0)
	{
		GiveItem2Character(pchar, sItem);
		if(isswapped == 0)
		{
			pchar.items.(sItem).isquest.colony = sTargetColony;
			pchar.items.(sItem).isquest.period = sPeriod;
		}
		else
		{
			pchar.items.(sItem).isquest.colony = sBeginColony;
			pchar.items.(sItem).isquest.period = sPeriod;
		}
	}

	int lngFilePerksID = LanguageOpenFile("ItemsDescribe.txt");
	string sLogookItem = items[FindItem(sItem)].name;
	sLogookItem = LanguageConvertString(lngFilePerksID,sLogookItem);
	LanguageCloseFile(lngFilePerksID);

	if(isDelayed == 0)
	{
		if(sLogBook == "")
		{
			sLogBook = "DELIVERY_ITEM_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELIVERY_ITEM_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sItem", sLogookItem);
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		if(isswapped == 0)
		{
			AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "1");

			AddQuestUserData(sLogBook, "sItem", sLogookItem);
			AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
			AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
			AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
			AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
			AddQuestUserData(sLogBook, "period", sPeriod);
			AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_ITEM_QUEST", "5");
		}
	}
	else
	{
		if(sLogBook == "")
		{
			sLogBook = "DELAY_DELIVERY_ITEM_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELAY_DELIVERY_ITEM_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sItem", sLogookItem);
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
			AddQuestUserDataForTitle(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_ITEM_QUEST", "1");

		AddQuestUserData(sLogBook, "sItem", sLogookItem);
		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sTargetColony));
		AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
		AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
		AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
		AddQuestUserData(sLogBook, "period", sPeriod);
		AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
	}

	if(isswapped == 0)
	{
		if(ishovernor == 0)
		{
			AddQuestUserData(sLogBook, "who", XI_ConvertString("whotrader"));
		}
		else
		{
			AddQuestUserData(sLogBook, "who", XI_ConvertString("whohovernor"));
		}
	}

	NPCGiveQuest(chr);

	if(ishovernor == 1)
	{
		QuestsShablons[DELIVERY_ITEM_QUEST].l.taken = "1";
	}
	//chr.quest.questflag = 3;
	//chr.quest.questflag.model = SetModelFlag(chr);
	CheckQuestForCharacter(chr);

	sQuest = "Delivery_Item_Quest_for_" + chr.id + "_" + rand(10000);
	pchar.quest.(sQuest).win_condition.l1 = "location";
	pchar.quest.(sQuest).win_condition.l1.location = sTargetColony + "_tavern";
	pchar.quest.(sQuest).win_condition = "DeliveryItemQuestReachedLocation";
	pchar.quest.(sQuest).quest = iQuest;
}

void FailedDeliveryItemQuest(string sQuest)
{
	RechargeDeliveryItemPunishment(sQuest, 1);

	int iQuest = sti(pchar.quest.(sQuest).quest);
	int isFirstTime = sti(Quests[iQuest].l.contribution);
	int isDelayed = sti(Quests[iQuest].l.isdelayed);

	if(isFirstTime == 1)
	{
		ChangeCharacterReputation(pchar, -1, 6);
		string sLogBook = Quests[iQuest].l.logbook;
		if(isDelayed == 0)
		{
			AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "3");
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_ITEM_QUEST", "3");
		}
	}

}

void RechargeDeliveryItemPunishment(string sQuest, int iMode)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	
	if(sti(Quests[iQuest].l.contribution) == 19)
	{
		//pchar.quest.(sQuest).over = "yes";
		DestroyDeliveryItemQuest(iQuest);
	}
	else
	{
		string sParentNPC = Quests[iQuest].l.parent_npc;

		sQuest = "delivery_item_quest_timer_for_" + sParentNPC + "_rand_"+rand(10000);

		characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest = sQuest;

		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "Punishment_for_delivery_item_quest";
		pchar.quest.(sQuest).quest = iQuest;

		Quests[iQuest].l.contribution = sti(Quests[iQuest].l.contribution) + 1;
		Quests[iQuest].l.contribution.quest = sQuest;
	}
}

int CheckForDeliveryItemSuccess(aref chr)
{
	if(CheckAttribute(chr, "CurrentQuest.accepted"))
	{
		if(chr.CurrentQuest.accepted == false)
		{
			return 1;
		}
	}

	int iQuest = sti(chr.currentquest);

	int isDelayed = sti(Quests[iQuest].l.isdelayed);
	string sTargetCharacter = Quests[iQuest].l.target_npc;

	string sItem = Quests[iQuest].l.item;

	if(isDelayed == 1)
	{
		if(!CheckCharacterItem(pchar, sItem))
		{
			return 3;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		if(CheckCharacterItem(pchar, sItem))
		{
			if(chr.id == sTargetCharacter)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
		else
		{
			if(chr.id == sTargetCharacter)
			{
				return 3;
			}
			else
			{
				return 2;
			}
		}
	}


	return -1;
}

void DestroyDeliveryItemQuest(int iQuest)
{	
	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	string sTargetNPC = Quests[iQuest].l.target_npc;
	ref pChr = CharacterFromID(sParentNPC);
	ref tChr = CharacterFromID(sTargetNPC);

	AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "5");


	sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;
	sQuest = characters[GetCharacterIndex(sTargetNPC)].CurrentQuest.pquest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(pchar, "quests." + sQuest);

	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}

	string sItem = Quests[iQuest].l.item;

	DeleteAttribute(pchar, "items." +(sItem));
	DeleteAttribute(pchar, "items." +(sItem));

	DeleteAttribute(pChr, "CurrentQuest");
	DeleteAttribute(tChr, "CurrentQuest");
	EraseQuestFreeState(pChr);
	EraseQuestFreeState(tChr);


	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, -5, 6);
	}
	else
	{
		int iNation = sti(pChr.nation);
		ChangeCharacterReputation(pchar, -1, iNation);
		int iCareerReward = sti(QuestsShablons[DELIVERY_ITEM_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
		pChr.career.change = iCareer;

		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
	}

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_ITEM_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
}

void CompleteDeliveryItemQuest(aref chr)
{	
	int iQuest = sti(chr.currentquest);
	string sItem =  Quests[iQuest].l.item;
	int isumm = sti(Quests[iQuest].l.money);
	int icontributtion = sti(Quests[iQuest].l.contribution);
	string sLogBook = Quests[iQuest].l.logbook;

	if(icontributtion > 0)
	{
		isumm = isumm - (isumm*icontributtion*5/100);
		AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "4");
		AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
	}
	else
	{
		AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "2");
	}

	int ishovernor = sti(Quests[iQuest].l.ishovernor);

	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, 1, 6);
	}
	else
	{
		int iNation = sti(chr.nation);
		ChangeCharacterReputation(pchar, 1, iNation);
		int iCareerReward = sti(QuestsShablons[DELIVERY_ITEM_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, iCareerReward);
		chr.career.change = iCareer;

		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
	}

	int isDelayed = sti(Quests[iQuest].l.isdelayed);

	if(isDelayed == 1)
	{
		TakeItemFromCharacter(pchar, sItem);
	}
	AddMoneyToCharacter(pchar, isumm);
	AddPartyExp(pchar, isumm/5);

	EraseQuestFreeState(chr);
	DeleteAttribute(chr, "CurrentQuest");

	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}

	if(CheckAttribute(&Quests[iQuest], "l.contribution.quest"))
	{
		sQuest = Quests[iQuest].l.contribution.quest;
		pchar.quest.(sQuest).over = true;
	}

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_ITEM_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
	
	pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

	if(ishovernor == 1)
	{
		GenerateHovernorQuests(chr);
		chr.quest.questflag.model = SetModelFlag(chr);
	}

	string sQuest = chr.CurrentQuest.pquest;
	pchar.quest.(sQuest).over = "yes";
}

string SelectItemForDeliveryItemQuest(int isEnemy)
{
	int iRandItem;
	string sItem = "-1";
	switch (isEnemy)
	{
		case 0: 
			iRandItem = rand(9);
			sItem = "DeliveryItemQuest_Peace_" + iRandItem; 
		break;
		case 1: 
			iRandItem = rand(2);
			sItem = "DeliveryItemQuest_War_" + iRandItem;
		break;
	}
	
	return sItem;
}


void DeliveryItemQuestReachedLocation(string sQuest)
{
	int iQuest = pchar.quest.(sQuest).quest;

	string sTargetCharacter = Quests[iQuest].l.target_npc;
	int isDelayed = Quests[iQuest].l.isdelayed;

	int iChar = GetCharacterIndex(sTargetCharacter);

	characters[iChar].CurrentQuest = iQuest;
	characters[iChar].CurrentQuest.type = DELIVERY_ITEM_QUEST;

	LAi_RemoveLoginTime(&characters[iChar]);
	PlaceCharacter(&characters[iChar], "sit", "random");
	characters[iChar].dialog.filename = "ItemHolder_dialog.c";

	if(isDelayed == 1)
	{
		characters[iChar].dialog.currentnode = "Take_item";
		characters[iChar].greeting = "cit_quest_" + (rand(3) + 1);
	}
	else
	{
		characters[iChar].dialog.currentnode = "Give_item";
		characters[iChar].greeting = "cit_complete_" + (rand(3) + 1);
	}
	LAi_SetSitType(&characters[iChar]);

	characters[iChar].quest.questflag = 2;
	characters[iChar].quest.questflag.model = SetModelFlag(&characters[iChar]);

}

void PrepareCompleteDeliveryItemQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	int isDelayed = sti(Quests[iQuest].l.isdelayed);
	string sItem = Quests[iQuest].l.item;
	string sLogBook = Quests[iQuest].l.logbook;

	if(isDelayed == 0)
	{
		TakeItemFromCharacter(pchar, sItem);
		
		AddQuestRecordEx(sLogBook, "DELIVERY_ITEM_QUEST", "2");
	}
	else
	{
		GiveItem2Character(pchar, sItem);
		AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_ITEM_QUEST", "5");
	}

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	DeleteAttribute(chr, "characterlock");

}
*/
