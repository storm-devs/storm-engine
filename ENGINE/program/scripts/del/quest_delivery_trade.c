

int GenerateDeliveryTradeQuest(aref chr, bool isdelayed, string logbook, int iscontinue, int ishovernor)
{
	trace("Generate DeliveryTrade Quest Begin");

	int iTradeType;
	int iTradeQuantity;
	string sTargetColony;
	string sBeginColony;
	int iQuest;

	if(ishovernor == 0 && GetCharacterReputation(pchar, 6) < 10)
	{
		return -1;
	}
	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		if(sti(Quests[iQuest].l.type) == DELIVERY_TRADE_QUEST)
		{
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
	sTargetColony = FindNonEnemyColonyForNationExceptColony(sBeginColony);

	if (sTargetColony == "")
	{
		return -1;
	}

	string sTargetNPC = sTargetColony + "_trader";
	if(CheckAttribute(&characters[GetCharacterIndex(sTargetNPC)], "currentquest"))
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÒÎÂÀÐ
	//////////////////////////////////////////////////////////////////////////////
	iTradeType = SearchForTrade(sBeginColony, sTargetColony, chr, 0);

	if(iTradeType == -1)
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÊÎËÈ×ÅÑÒÂÎ ÒÎÂÀÐÀ
	//////////////////////////////////////////////////////////////////////////////
	iTradeQuantity = GetSquadronFreeSpace(pchar, iTradeType);

	if(iTradeQuantity > 200)
	{
		iTradeQuantity = 190 - rand(10) + rand(10);
	}
	else
	{
		if (iTradeQuantity < 100)
		{
			return -1;
		}
		else
		{
			iTradeQuantity = 90 - rand(10) + rand(10);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÄÈÑÒÀÍÖÈÞ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;

	//////////////////////////////////////////////////////////////////////////////
	/// ÎÏÐÅÄÅËßÅÌ ÍÀÃÐÀÄÓ
	//////////////////////////////////////////////////////////////////////////////
	int curStoreIdx = FindStore(sTargetColony);
	int iCost = GetStoreGoodsPrice(&Stores[curStoreIdx],iTradeType,PRICE_TYPE_SELL,pchar,1)
	iCost = (iTradeQuantity*iCost/sti(goods[iTradeType].units))/2.0 + iTradeQuantity*(GetSummonSkillFromName(pchar, SKILL_COMMERCE)/5);

	if(isdelayed == 1)
	{
		iDistance = iDistance * 2;
	}

	iDistance = iDistance + 2;

	iCost = iCost + iDistance * 10;

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

	Quests[iQuest].id = "DeliveryTrade_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.goods = iTradeType;
	Quests[iQuest].l.goods.quantity = iTradeQuantity;
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.target_npc = sTargetNPC;
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
	Quests[iQuest].l.freespace = 0;

	chr.CurrentQuest.state = QUEST_NOT_ACCEPTED;
	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = DELIVERY_TRADE_QUEST;

	Quests[iQuest].l.type = DELIVERY_TRADE_QUEST;

	return 1;
}

void PrepareAcceptDeliveryQuest(aref chr)
{
	string parentNPC;
	int isDelayed, isHovernor;
	for(int i = 0; i < MAX_QUESTS; i++)
	{
		if(Quests[i].id != "-1")
		{
			if(!CheckAttribute(&Quests[i], "l.target_npc"))
			{
				continue;
			}
			if(Quests[i].l.target_npc == chr.id && sti(Quests[i].l.type) == DELIVERY_TRADE_QUEST)
			{
				isDelayed = sti(Quests[i].l.isdelayed);
				isHovernor = sti(Quests[i].l.isHovernor);
				if(isDelayed == 1)
				{
					Quests[i].l.isdelayed = 0;
					//parentNPC = Quests[i].l.parent_colony + "_Trader";
					parentNPC = Quests[i].l.parent_npc;
					if(isHovernor == 1)
					{
						Quests[i].l.hovernor.id = Quests[i].l.parent_npc;
					}
					Quests[i].l.parent_npc = chr.id;
					Quests[i].l.target_npc = parentNPC;
					Quests[i].l.swapped = 1;
					chr.CurrentQuest = i;
					chr.CurrentQuest.type = DELIVERY_TRADE_QUEST;
					AcceptDeliveryTradeQuest(chr);
					DeleteAttribute(chr, "CurrentQuest");
					chr.quest.questflag.model = "";
					DeleteAttribute(chr, "quest.questflag");
				}
			}
		}
	}
}

void AcceptDeliveryTradeQuest(aref chr)
{
	chr.CurrentQuest.state = QUEST_ACCEPTED;
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
		sQuest = "delivery_trade_quest_timer_for_" + chr.id + "_" + rand(10000);
		chr.CurrentQuest.pquest = sQuest;
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "deliverytradequesttimer_failed";
		pchar.quest.(sQuest).quest = iQuest;
	}

	int iTradeType = Quests[iQuest].l.goods;
	int iTradeQuantity = Quests[iQuest].l.goods.quantity;
	string sColony = Quests[iQuest].l.target_colony;
	string sBeginColony = Quests[iQuest].l.parent_colony;
	
	int isdelayed = sti(Quests[iQuest].l.isdelayed);
	int ishovernor = sti(Quests[iQuest].l.ishovernor);

	if(isdelayed == 0)
	{
		AddCharacterGoods(pchar, iTradeType, iTradeQuantity);
		string sGoodName = goods[iTradeType].name;
		if(isswapped == 0)
		{
			pchar.ship.cargo.goods.(sGoodName).isquest.quantity = iTradeQuantity;
			pchar.ship.cargo.goods.(sGoodName).isquest.colony = sColony;
		}
		else
		{
			pchar.ship.cargo.goods.(sGoodName).isquest.quantity = iTradeQuantity;
			pchar.ship.cargo.goods.(sGoodName).isquest.colony = sBeginColony;
		}
	}

    string sLogBook = Quests[iQuest].l.logbook;

	int iMoney = sti(Quests[iQuest].l.money);
	string sIsland = colonies[FindColony(sColony)].island;
	string sTradeType = Goods[iTradeType].name + "Gen";

	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;


	if(isDelayed == 0)
	{
		if(sLogBook == "")
		{
			sLogBook = "DELIVERY_TRADE_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELIVERY_TRADE_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sGoodGen", XI_ConvertString(sTradeType));
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		if(isswapped == 0)
		{
			AddQuestRecordEx(sLogBook, "DELIVERY_TRADE_QUEST", "1");

			AddQuestUserData(sLogBook, "sGoodGen", XI_ConvertString(sTradeType));
			AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
			AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
			AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
			AddQuestUserData(sLogBook, "period", sPeriod);
			AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_TRADE_QUEST", "5");
		}
	}
	else
	{
		if(sLogBook == "")
		{
			sLogBook = "DELAY_DELIVERY_TRADE_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELAY_DELIVERY_TRADE_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sGoodGen", XI_ConvertString(sTradeType));
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			AddQuestUserDataForTitle(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_TRADE_QUEST", "1");

		AddQuestUserData(sLogBook, "sGoodGen", XI_ConvertString(sTradeType));
		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
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
	else
	{
		if(CheckAttribute(&Quests[iQuest], "l.isfight"))
		{
			if(sti(Quests[iQuest].l.isfight) == 1)
			{
				int iEncounterChar = GenerateCharacter(PIRATE, WITH_SHIP, "pirate", MAN, 1, WARRIOR);
				//characters[iEncounterChar].adventure.type = "war";
				characters[iEncounterChar].mapEnc.type = "war";
				characters[iEncounterChar].mapEnc.worldMapShip = "pirates_frigate";
				Map_CreateWarrior(sColony, characters[iEncounterChar].id, 40);
			}
		}
	}

	NPCGiveQuest(chr);

	if(ishovernor == 1)
	{
		QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "1";
		//chr.quest.questflag = 3;
		//chr.quest.questflag.model = SetModelFlag(chr);
		CheckQuestForCharacter(chr);
	}
	else
	{
		if(isdelayed == 0)
		{
			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag"));
		}
		else
		{
			//chr.quest.questflag = 3;
			//chr.quest.questflag.model = SetModelFlag(chr);
			CheckQuestForCharacter(chr);
		}
	}

	string sTargetNPC = Quests[iQuest].l.target_npc;
	int iTargetNPC = GetCharacterIndex(sTargetNPC);
	characters[iTargetNPC].quest.questflag = 2;
	characters[iTargetNPC].quest.questflag.model = SetModelFlag(chr);
	characters[iTargetNPC].currentquest = iQuest;
	characters[iTargetNPC].currentquest.type = DELIVERY_TRADE_QUEST;
	NPCGiveQuest(&characters[iTargetNPC]);

	if(CheckAttribute(&Quests[iQuest], "l.subtype"))
	{
		if(Quests[iQuest].l.subtype == "story")
		{
			if(CheckAttribute(chr, "FranceQuestProgress"))
			{
				string sStep = "l" + sti(chr.FranceQuestProgress);
				chr.FranceQuestProgress.(sStep).isgived = 1;
				CheckQuestForCharacter(chr);
			}
		}
	}
}

void FailedDeliveryTradeQuest(string sQuest)
{
	RechargeDeliveryTradePunishment(sQuest, 1);

	int iQuest = sti(pchar.quest.(sQuest).quest);
	int isFirstTime = sti(Quests[iQuest].l.contribution);
	int isDelayed = sti(Quests[iQuest].l.isdelayed);

	if(isFirstTime == 1)
	{
		ChangeCharacterReputation(pchar, -1, 6);
		string sLogBook = Quests[iQuest].l.logbook;
		if(isDelayed == 0)
		{
			AddQuestRecordEx(sLogBook, "DELIVERY_TRADE_QUEST", "3");
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_TRADE_QUEST", "3");
		}
	}

}

void RechargeDeliveryTradePunishment(string sQuest, int iMode)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	
	if(sti(Quests[iQuest].l.contribution) == 19)
	{
		DestroyDeliveryTradeQuest(iQuest);
	}
	else
	{
		string sParentNPC = Quests[iQuest].l.parent_npc;

		sQuest = "delivery_trade_quest_timer_for_" + sParentNPC + "_rand_"+rand(10000);

		characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest = sQuest;

		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "Punishment_for_delivery_trade_quest";
		pchar.quest.(sQuest).quest = iQuest;

		Quests[iQuest].l.contribution = sti(Quests[iQuest].l.contribution) + 1;
		Quests[iQuest].l.contribution.quest = sQuest;
	}
}

int CheckForDeliverySuccess(aref chr, string sType)
{
	if(sType == "story")
	{
		if(CheckAttribute(chr, "FranceQuestProgress"))
		{
			string sStep = "l" + sti(chr.FranceQuestProgress);
			if(CheckAttribute(chr, "FranceQuestProgress." + sStep + ".iscompleted"))
			{
				return 2;
			}
			if(CheckAttribute(chr, "FranceQuestProgress." + sStep + ".isgived"))
			{
				return 3;
			}
			return 1;
		}
	}

	int iGoodsQuantity, iGoods, isDelayed;
	for(int i = 0; i < MAX_QUESTS; i++)
	{
		if(Quests[i].id != "-1")
		{
			if(!CheckAttribute(chr, "currentquest"))
			{
				continue;
			}
			if(sti(chr.currentquest) != i)
			{
				continue;
			}
			if(CheckAttribute(&Quests[i], "l.target_npc") && sti(Quests[i].l.type) == DELIVERY_TRADE_QUEST)
			{
				if(Quests[i].l.target_npc == chr.id)
				{
					if(CheckAttribute(&Quests[i], "l.isdelayed"))
					{
						isDelayed = sti(Quests[i].l.isdelayed);
						if(isDelayed == 0)
						{
							iGoods = sti(Quests[i].l.goods);
							iGoodsQuantity = sti(Quests[i].l.goods.quantity);
							if(GetSquadronGoods(pchar, iGoods) >= iGoodsQuantity)
							{
								chr.quest.deliveryquest = i;
								return 2; // "take quest" flag on
							}
							else
							{
								return 3;// disabled "take quest" flag on
							}
						}
						else
						{
							iGoods = sti(Quests[i].l.goods);
							iGoodsQuantity = sti(Quests[i].l.goods.quantity);
							int iSpaceNeeded = GetSquadronFreeSpace(pchar, iGoods);
							int iSpaceHave = iGoodsQuantity * sti(Goods[iGoods].units);
							if(iSpaceNeeded < iSpaceHave)
							{
								Quests[i].l.freespace = iSpaceHave - iSpaceNeeded;
								return 3; // disabled "take quest" flag on
							}
							return 2;// "take quest" flag on
						}
					}
				}
				else
				{
					if(CheckAttribute(chr, "CurrentQuest.state"))
					{
						if(sti(chr.CurrentQuest.state) == QUEST_NOT_ACCEPTED)
						{
							return 1;
						}
						if(sti(chr.CurrentQuest.state) == QUEST_COMPLETED)
						{
							return 2;
						}
					}
					return 3;
				}
			}
		}
		//return 3; // disabled "take quest" flag on
	}
	return -1;// no quest flag
}

void DestroyDeliveryTradeQuest(int iQuest)
{
	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	string sTargetNPC = Quests[iQuest].l.target_npc;
	ref pChr = CharacterFromID(sParentNPC);
	ref tChr = CharacterFromID(sTargetNPC);

	AddQuestRecordEx(sLogBook, "DELIVERY_TRADE_QUEST", "5");


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

	string sGoodName = goods[sti(Quests[iQuest].l.goods)].name;

	DeleteAttribute(pchar, "ship.cargo.goods." +(sGoodName) + ".isquest");
	DeleteAttribute(pchar, "ship.cargo.goods." +(sGoodName) + ".isquest.quantity");
	DeleteAttribute(pchar, "ship.cargo.goods." +(sGoodName) + ".isquest.colony");

	DeleteAttribute(pChr, "CurrentQuest");
	DeleteAttribute(tChr, "CurrentQuest");
	EraseQuestFreeState(pChr);
	EraseQuestFreeState(tChr);

	pChr.quest.questflag.model = "";
	tChr.quest.questflag.model = "";
	DeleteAttribute(pChr, "quest.questflag");
	DeleteAttribute(tChr, "quest.questflag");


	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, -5, 6);
	}
	else
	{
		int iNation = sti(pChr.nation);
		ChangeCharacterReputation(pchar, -1, iNation);
		int iCareerReward = sti(QuestsShablons[DELIVERY_TRADE_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
		pChr.career.change = iCareer;
		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
	}

	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
}

void CompleteDeliveryTradeQuest(aref chr)
{
	if(CheckAttribute(chr, "quest.type"))
	{
		if(!CheckAttribute(chr, "quest.deliveryquest") && chr.quest.type != "hovernor")
		{
			return;
		}
	}
	int iQuest = -1;
	if(CheckAttribute(chr, "quest.deliveryquest"))
	{
		iQuest = sti(chr.quest.deliveryquest);
	}
	else
	{
		iQuest = sti(chr.currentquest);
	}
	if(sti(Quests[iQuest].l.type) != DELIVERY_TRADE_QUEST)
	{
		return;
	}
	string sQuest = "l"+iQuest;

	int iGoods = sti(Quests[iQuest].l.goods);
	int iGoodsQuantity = sti(Quests[iQuest].l.goods.quantity);

	int isumm = sti(Quests[iQuest].l.money);
	int icontributtion = sti(Quests[iQuest].l.contribution);

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC;
	if(CheckAttribute(&Quests[iQuest], "l.hovernor.id"))
	{
		sParentNPC = Quests[iQuest].l.hovernor.id;
	}
	else
	{
		sParentNPC = Quests[iQuest].l.parent_npc;
	}
	string sTargetNPC = Quests[iQuest].l.target_npc;
	ref pChr = CharacterFromID(sParentNPC);
	ref tChr = CharacterFromID(sTargetNPC);

	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(icontributtion > 0)
	{
		isumm = isumm - (isumm*icontributtion*5/100);
		AddQuestRecordEx(sLogBook, "DELIVERY_TRADE_QUEST", "4");
		AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
	}
	else
	{
		if(ishovernor == 0)
		{
			ChangeCharacterReputation(pchar, 1, 6);
		}
		else
		{
			int iNation = sti(pChr.nation);
			ChangeCharacterReputation(pchar, 1, iNation);
			int iCareerReward = sti(QuestsShablons[DELIVERY_TRADE_QUEST].l.careerreward);
			int iCareer = ChangeCharacterCareerProgress(iNation, iCareerReward);
			pChr.career.change = iCareer;
			iNation = sti(pchar.nation);
			string sPatent = GetNationNameByType(iNation) + "_career_counter";
			pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
		}
		AddQuestRecordEx(sLogBook, "DELIVERY_TRADE_QUEST", "2");
	}

	RemoveCharacterGoods(pchar, iGoods, iGoodsQuantity);
	AddMoneyToCharacter(pchar, isumm);
	pChr.currentquest.state = QUEST_COMPLETED;

	string sGoodName = goods[sti(Quests[iQuest].l.goods)].name;

	DeleteAttribute(pchar, "ship.cargo.goods." +(sGoodName) + ".isquest");

	if(CheckAttribute(characterFromID(sParentNPC), "CurrentQuest.pquest"))
	{
		sQuest = characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest;
		pchar.quest.(sQuest).over = true;
	}
	if(CheckAttribute(characterFromID(sTargetNPC), "CurrentQuest.pquest"))
	{
		sQuest = characters[GetCharacterIndex(sTargetNPC)].CurrentQuest.pquest;
		pchar.quest.(sQuest).over = true;
	}

	DeleteAttribute(pchar, "quests." + sQuest);

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

	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	if(!CheckAttribute(&Quests[iQuest], "l.subtype"))
	{
		if(ishovernor == 1)
		{
			if(chr.id == pchr.id)
			{
				DeleteAttribute(pChr, "CurrentQuest");
				pChr.quest.questflag.model = "";
				DeleteAttribute(pChr, "quest.questflag");
				EraseQuestFreeState(pChr);
			}
		}
		else
		{
			Quests[iQuest].id = "-1";
			DeleteAttribute(&Quests[iQuest], "l"));
			QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "-1";
			EmptyAllQuestData(iQuest);

		}
		DeleteAttribute(tChr, "CurrentQuest");
		tChr.quest.questflag.model = "";
		DeleteAttribute(tChr, "quest.questflag");
		EraseQuestFreeState(tChr);

		pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

		if(CheckAttribute(chr, "quest.type"))
		{
			if(chr.quest.type == "hovernor")
			{
				GenerateHovernorQuests(chr);
				chr.quest.questflag.model = SetModelFlag(chr);
			}
		}
		AddPartyExp(pchar, isumm/10);
	}
	else
	{
		if(!CheckAttribute(pChr, "FranceQuestProgress"))
		{
			pChr.FranceQuestProgress = 1;
		}
		if(pchr.id == chr.id)
		{
			DeleteAttribute(pChr, "CurrentQuest");
			pChr.quest.questflag.model = "";
			DeleteAttribute(pChr, "quest.questflag");
			EraseQuestFreeState(pChr);

			Quests[iQuest].id = "-1";

			DeleteAttribute(&Quests[iQuest], "l"));

			QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "-1";

			EmptyAllQuestData(iQuest);

			pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

			if(CheckAttribute(chr, "quest.type"))
			{
				if(chr.quest.type == "hovernor")
				{
					GenerateHovernorQuests(chr);
					chr.quest.questflag.model = SetModelFlag(chr);
				}
			}

			AddPartyExp(pchar, sti(pChr.FranceQuestProgress) * 500);
		}

		if(Quests[iQuest].l.subtype == "story")
		{
			string sStep = "l" + sti(pChr.FranceQuestProgress);
			pChr.FranceQuestProgress.(sStep).iscompleted = 1;
		}

		DeleteAttribute(tChr, "CurrentQuest");
		tChr.quest.questflag.model = "";
		DeleteAttribute(tChr, "quest.questflag");
		EraseQuestFreeState(tChr);


			
		//}
	}	
}