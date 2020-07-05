
string SelectStoryFranceQuest(aref chr)
{
	string sLink = "";
	if(!CheckAttribute(chr, "FranceQuestProgress"))
	{
		chr.FranceQuestProgress = 0;
	}

	chr.FranceQuestProgress = sti(chr.FranceQuestProgress) + 1;

	switch(sti(chr.FranceQuestProgress))
	{
		case 1:
			sLink = "FranceFirstQuest";
		break;
		case 2:
			sLink = "FranceSecondQuest";
		break;
		case 3:
			sLink = "FranceThirdQuest";
		break;
	}

	return sLink;
}

void GenerateStoryHovernorQuests(aref chr)
{
	switch(sti(chr.FranceQuestProgress))
	{
		case 1:
			chr.quest.questflag = GenerateDeliveryTradeQuestEX(chr, 0, "career", 1, 1, -1, -1);
		break;

		case 2:
			chr.quest.questflag = GeneratePassengerQuestEx(chr, 0, "career", 1, 1, 1, 1, 1);
		break;

		case 3:
			chr.quest.questflag = GenerateDeliveryTradeQuestEX(chr, 1, "career", 1, 1, -1, 1);
		break;
	}
}

string SelectHovernorQuestTextEx(aref chr)
{
	string sResult = "-1";
	if(CheckAttribute(chr, "CurrentQuest"))
	{
		int iQuestType = sti(chr.Currentquest.type);
		switch(iQuestType)
		{
			case DELIVERY_PASSENGER_QUEST:
				sResult = SelectDeliveryPassengerDialogEx(chr);
			break;

			case DELIVERY_TRADE_QUEST:
				sResult = SelectDeliveryTradeDialogEx(chr);
			break;

			case SINK_SHIP_QUEST:
				//sResult = SelectSinkShipDialogEx(chr);
			break;

			case ESCORT_SHIP_QUEST:
				//sResult = SelectEscortShipDialogEx(chr);
			break;

			case DELIVERY_ITEM_QUEST:
				//sResult = SelectDeliveryItemDialogEx(chr);
			break;

			case CAPTURE_FORT_QUEST:
				//sResult = SelectCaptureFortDialogEx(chr);
			break;
		}
	}
	return sResult;
}

int GenerateDeliveryTradeQuestEX(aref chr, bool isdelayed, string logbook, int iscontinue, int ishovernor, int iTradeType, int isFightEncounter)
{
	trace("Generate DeliveryTradeEX Quest Begin");

	int iTradeQuantity;
	string sTargetColony;
	string sBeginColony;
	int iQuest;

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
	/// »Ÿ≈Ã  ŒÀŒÕ»ﬁ
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
	/// »Ÿ≈Ã “Œ¬¿–
	//////////////////////////////////////////////////////////////////////////////
	if(iTradeType == -1)
	{
		iTradeType = rand(GOOD_OIL - GOOD_WHEAT) + GOOD_WHEAT;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÀ»◊≈—“¬Œ “Œ¬¿–¿
	//////////////////////////////////////////////////////////////////////////////
	iTradeQuantity = GetSquadronFreeSpace(pchar, iTradeType);

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã ƒ»—“¿Õ÷»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.2;

	//////////////////////////////////////////////////////////////////////////////
	/// Œœ–≈ƒ≈Àﬂ≈Ã Õ¿√–¿ƒ”
	//////////////////////////////////////////////////////////////////////////////
	int iCost = sti(chr.FranceQuestProgress) * 500;

	if(isdelayed == 1)
	{
		iDistance = iDistance * 2;
	}

	iDistance = iDistance + 2;

	iCost = iCost + iDistance * 10;

	iCost = iCost * GetQuestPriceFromDifficulty();

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

	Quests[iQuest].id = "DeliveryTrade_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.goods = iTradeType;
	Quests[iQuest].l.goods.quantity = iTradeQuantity;
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.target_npc = sTargetColony + "_trader";
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
	Quests[iQuest].l.isfight = isFightEncounter;

	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = DELIVERY_TRADE_QUEST;

	Quests[iQuest].l.type = DELIVERY_TRADE_QUEST;
	Quests[iQuest].l.subtype = "story";

	return 1;
}

string SelectStoryAnswersText(aref chr, aref sLink)
{
	int iQuest = sti(chr.CurrentQuest);

	string sStringID = "";
	string sParam = "";

	int iGood = -1;
	string sCharName = "";
	int iColony = -1;

	if(CheckAttribute(&Quests[iQuest], "l.goods"))
	{
		iGood = sti(Quests[iQuest].l.goods);
		FindRussianGoodString(iGood, &sParam, 1);
	}
	if(CheckAttribute(&Quests[iQuest], "l.passenger"))
	{
		sCharName = GetCharacterFullName(Quests[iQuest].l.passenger);
		sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	}
	if(CheckAttribute(&Quests[iQuest], "l.target_colony"))
	{
		iColony = FindColony(Quests[iQuest].l.target_colony);
		FillRussianColonyString(iColony, 1, &sParam);
	}


	if(!CheckAttribute(&Quests[iQuest], "l.contribution"))
	{
		Quests[iQuest].l.contribution = 0;
	}
	string sDays = "";
	switch(sti(chr.FranceQuestProgress))
	{
		case 1:
			if(sti(Quests[iQuest].l.contribution) > 0)
			{
				sStringID = "SelectFirstFranceLineDialogAnswerPunishment";
				sDays = FindRussianDaysString(sti(Quests[iQuest].l.contribution));
				sParam = DialogAddParamToStr(sParam, "sTime", sDays);
				sLink = "FranceFirstQuest_punishment_complete";
			}
			else
			{
				sStringID = "SelectFirstFranceLineDialogAnswer";
				sLink = "FranceFirstQuest_complete";
			}
		break;

		case 2:
			if(sti(Quests[iQuest].l.contribution) > 0)
			{
				sStringID = "SelectSecondFranceLineDialogAnswerPunishment";
				sDays = FindRussianDaysString(sti(Quests[iQuest].l.contribution));
				sParam = DialogAddParamToStr(sParam, "sTime", sDays);
				sLink = "FranceSecondQuest_punishment_complete";
			}
			else
			{
				sStringID = "SelectSecondFranceLineDialogAnswer";
				sLink = "FranceSecondQuest_complete";
			}
		break;
	}

	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

void CompleteStoryTradeQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	DeleteAttribute(chr, "CurrentQuest");
	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");
	EraseQuestFreeState(chr);

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);

	pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;

	GenerateHovernorQuests(chr);
	chr.quest.questflag.model = SetModelFlag(chr);
}

int GeneratePassengerQuestEx(aref chr, bool isdelayed, string logbook, int iscontinue, int isfightpossible, int ishovernor, int ischarCreate, int isEnemyColony)
{
	trace("Generate Passenger Quest EX Begin");
	
	int iQuest;

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		if(sti(Quests[iQuest].l.type) == DELIVERY_PASSENGER_QUEST)
		{
			//isdelayed = sti(Quests[iQuest].l.isdelayed);

			//return isdelayed;
			return 1;
		}
		else
		{
			return -1;
		}
	}


	//////////////////////////////////////////////////////////////////////////////
	/// ∆≈ÕŸ»Õ€ Õ≈ ÃŒ√”“ œ”“≈ÿ≈—“¬Œ¬¿“‹
	//////////////////////////////////////////////////////////////////////////////
	if(ischarCreate == 0 && chr.sex == "woman")
	{
		return -1
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã  ŒÀŒÕ»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	string sBeginColony = Locations[FindLocation(pchar.location)].fastreload;

	string sTargetColony = "";
	if(isEnemyColony == 1)
	{
		sTargetColony = FindEnemyColonyForNation(sBeginColony);
	}
	else
	{
		sTargetColony = FindRandomColonyExceptColony(sBeginColony);
	}

	int iTargetNation = -1;
	if(colonies[FindColony(sTargetColony)].nation == "none")
	{
		iTargetNation = PIRATE;
	}
	else
	{
		iTargetNation = sti(colonies[FindColony(sTargetColony)].nation);
	}


	if (sTargetColony == "")
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã œ¿——¿∆»–¿
	//////////////////////////////////////////////////////////////////////////////
	int iChar;
	if(ischarCreate == 1)
	{
		iChar = GenerateCharacter(iNation, WITHOUT_SHIP, "citizen", MAN, 1, WARRIOR);
	}
	else
	{
		iChar = sti(chr.index);
		characters[iChar].characterlock = 1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// »Ÿ≈Ã ƒ»—“¿Õ÷»ﬁ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;

	//////////////////////////////////////////////////////////////////////////////
	/// Œœ–≈ƒ≈Àﬂ≈Ã Õ¿√–¿ƒ”
	//////////////////////////////////////////////////////////////////////////////

	if(isdelayed == 1)
	{
		iDistance = iDistance * 2;
	}
	iDistance = iDistance + 2;

	int iCost = sti(chr.FranceQuestProgress) * 500;

	iCost = iCost + iDistance * 10;

	iCost = iCost * GetQuestPriceFromDifficulty();

	characters[iChar].money = iCost + rand(iCost);
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
	Quests[iQuest].id = "DeliveryPassenger_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.passenger = characters[iChar].id;
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.isdelayed = isdelayed;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.taken = "-1";
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
	chr.CurrentQuest.type = DELIVERY_PASSENGER_QUEST;

	Quests[iQuest].l.type = DELIVERY_PASSENGER_QUEST;
	Quests[iQuest].l.subtype = "story";
	return 1;// "give quest" flag on
}