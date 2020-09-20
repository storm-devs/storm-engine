/*
int GeneratePassengerQuest(aref chr, bool isdelayed, string logbook, int iscontinue, int isfightpossible, int ishovernor, int ischarCreate)
{
	trace("Generate Passenger Quest Begin");
	
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

	if(checkAttribute(pchar, "vilecheater") && ishovernor == 0)
	{
		if(sti(pchar.vilecheater) > sti(pchar.reputation)/2 && rand(5) > 2)
		{
			return -1;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÆÅÍÙÈÍÛ ÍÅ ÌÎÃÓÒ ÏÓÒÅØÅÑÒÂÎÂÀÒÜ
	//////////////////////////////////////////////////////////////////////////////
	if(ischarCreate == 0 && chr.sex == "woman")
	{
		return -1
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÊÎËÎÍÈÞ
	//////////////////////////////////////////////////////////////////////////////
	int iNation = sti(chr.nation);
	string sBeginColony = Locations[FindLocation(pchar.location)].fastreload;

	string sTargetColony = FindRandomColonyExceptColony(sBeginColony);
	int iTargetNation = sti(colonies[FindColony(sTargetColony)].nation);

	if(GetNationRelation(iNation, iTargetNation) == RELATION_ENEMY)
	{
		isfightpossible = 1;
	}
	else
	{
		isfightpossible = 0;
	}

	if (sTargetColony == "")
	{
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// ÈÙÅÌ ÏÀÑÑÀÆÈÐÀ
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
	/// ÈÙÅÌ ÄÈÑÒÀÍÖÈÞ
	//////////////////////////////////////////////////////////////////////////////
	int iDistance = CalculateDistance(FindColony(sTargetColony));
	iDistance = iDistance * 1.3;

	//////////////////////////////////////////////////////////////////////////////
	/// ÎÏÐÅÄÅËßÅÌ ÍÀÃÐÀÄÓ
	//////////////////////////////////////////////////////////////////////////////

	if(isdelayed == 1)
	{
		iDistance = iDistance * 2;
	}
	iDistance = iDistance + 2;

	int iCost = iDistance * 30;

	iCost = iCost * GetQuestPriceFromDifficulty();

	characters[iChar].money = iCost + rand(iCost);
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

	chr.CurrentQuest.accepted = false;
	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.type = DELIVERY_PASSENGER_QUEST;

	Quests[iQuest].l.type = DELIVERY_PASSENGER_QUEST;
	return 1;// "give quest" flag on
}

void AcceptDeliveryPassengerQuest(aref chr)
{
	chr.CurrentQuest.accepted = true;
	DeleteAttribute(chr, "CurrentQuest.lastdialog");
	
	string sRechargeQuest = "RechargeHovernorQuests_" + chr.id;
	pchar.quest.(sRechargeQuest).over = "yes";

	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;
	pchar.quests.(sQuest) = iQuest;

	int idays = sti(Quests[iQuest].l.time);
	int isswapped = sti(Quests[iQuest].l.swapped);
	
	string sPassenger = Quests[iQuest].l.passenger;
	int iPassenger = GetCharacterIndex(sPassenger);
	string sColony = Quests[iQuest].l.target_colony;
	string sParentColony = Quests[iQuest].l.parent_colony;
	
	int isdelayed = sti(Quests[iQuest].l.isdelayed);
	int ishovernor = sti(Quests[iQuest].l.ishovernor);

	int iMoney = sti(Quests[iQuest].l.money);

	if(isswapped == 0)
	{
		sQuest = "delivery_passenger_quest_timer_for_" + chr.ID + "_rand_" + rand(10000);;
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, idays);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "DeliveryPassengerTimerFailed";
		pchar.quest.(sQuest).quest = iQuest;
		Quests[iQuest].l.contribution.quest = sQuest;

		sQuest = "delivery_passenger_quest_for_" + chr.ID + "_rand_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sColony + "_town";
		pchar.quest.(sQuest).win_condition = "DeliveryPassenger";
		pchar.quest.(sQuest).Quest = iQuest;
	}
	else
	{
		if(ishovernor == 0)
		{
			sQuest = "delivery_passenger_quest_for_" + chr.ID + "_rand_" + rand(10000);
			pchar.quest.(sQuest).win_condition.l1 = "location";
			pchar.quest.(sQuest).win_condition.l1.location = sParentColony + "_town";
			pchar.quest.(sQuest).win_condition = "DeliveryPassenger";
			pchar.quest.(sQuest).Quest = iQuest;
		}
	}

	if(isdelayed == 0)
	{
		AddPassenger(pchar, characterFromID(sPassenger), -1);
		characters[iPassenger].quest.deliverymoney = iMoney;
		characters[iPassenger].quest.colony = Quests[iQuest].l.target_colony;
	}

	characters[GetCharacterIndex(sPassenger)].isquest = 1;

    string sLogBook = Quests[iQuest].l.logbook;

	string sIsland = colonies[FindColony(sColony)].island;
	string sBeginColony = Quests[iQuest].l.parent_colony;

	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sPeriod = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;

	if(isDelayed == 0)
	{
		if(sLogBook == "")
		{
			sLogBook = "DELIVERY_PASSENGER_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELIVERY_PASSENGER_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sTargetCharName", GetCharacterFullName(sPassenger));
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		if(isswapped == 0)
		{
			if(ishovernor == 1)
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "1");
				AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
			}
			else
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "6");
			}

			AddQuestUserData(sLogBook, "sTargetCharName", GetCharacterFullName(sPassenger));
			AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
			AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
			AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
			AddQuestUserData(sLogBook, "period", sPeriod);
			AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
		}
		else
		{
			AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_PASSENGER_QUEST", "5");
		}
	}
	else
	{
		if(sLogBook == "")
		{
			sLogBook = "DELAY_DELIVERY_PASSENGER_QUEST_FOR_" + chr.id + "_" + rand(100000);
			SetQuestHeaderEx(sLogBook, "DELAY_DELIVERY_PASSENGER_QUEST");
			AddQuestUserDataForTitle(sLogBook, "sTargetCharName", GetCharacterFullName(sPassenger));
			AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
			Quests[iQuest].l.logbook = sLogBook;
		}
		AddQuestRecordEx(sLogBook, "DELAY_DELIVERY_PASSENGER_QUEST", "1");
		AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));

		AddQuestUserData(sLogBook, "sTargetCharName", GetCharacterFullName(sPassenger));
		AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
		AddQuestUserData(sLogBook, "island", XI_ConvertString(sIsland));
		AddQuestUserData(sLogBook, "sNextColony", XI_ConvertString("colony"+sBeginColony));
		AddQuestUserData(sLogBook, "sTime", FindRussianDaysString(idays));
		AddQuestUserData(sLogBook, "period", sPeriod);
		AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(iMoney));
	}

	if(isswapped == 0)
	{
		if(ishovernor == 1)
		{
			AddQuestUserData(sLogBook, "who", XI_ConvertString("whohovernor"));
		}
	}

	if(characters[iPassenger].id != chr.id)
	{
		NPCGiveQuest(chr);
	}

	if(ishovernor == 0)
	{
		LAi_SetActorType(chr);
		LAi_ActorGoToLocation(chr, "reload", "reload1", "none", "reload", "reload1", "", 30.0);
		chr.quest.questflag.model = "";
		DeleteAttribute(chr, "quest.questflag"));
	}
	else
	{
		//chr.quest.questflag = 3;
		//chr.quest.questflag.model = SetModelFlag(chr);
		CheckQuestForCharacter(chr);

		LAi_SetActorType(&characters[iPassenger]);
		LAi_ActorGoToLocation(&characters[iPassenger], "reload", "reload1", "none", "reload", "reload1", "", 30.0);
		characters[iPassenger].quest.questflag.model = "";
		DeleteAttribute(&characters[iPassenger], "quest.questflag"));
	}

	sQuest = "Kill_Psssenger_" + characters[iPassenger].id + "_" + rand(10000);
	pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
	pchar.quest.(sQuest).win_condition.l1.character = iPassenger;
	pchar.quest.(sQuest).win_condition = "Kill_passenger";
	//pchar.quest.(sQuest).passenger = characters[iPassenger].id;
	pchar.quest.(sQuest).quest = iQuest;
}

void Kill_passenger(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);

	AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "9");

	sQuest = Quests[iQuest].l.contribution.quest;
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
		int iCareerReward = sti(QuestsShablons[DELIVERY_PASSENGER_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
		pChr.career.change = iCareer;
		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
	}

	ref chr = characterFromID(Quests[iQuest].l.passenger);
	DeleteAttribute(chr, "characterlock");
	DeleteAttribute(Chr, "CurrentQuest");

	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
	FoundAndDeleteAllQuestForQuestNumber(iQuest);
}

void DeliveryPassengerTimerFailed(string sQuest)
{
	RechargeDeliveryPassengerPunishment(sQuest, 1);

	int iQuest = sti(pchar.quest.(sQuest).quest);
	string sLogBook = Quests[iQuest].l.logbook;

	AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "3");
}

void RechargeDeliveryPassengerPunishment(string sQuest, int iMode)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	
	if(sti(Quests[iQuest].l.contribution) == 19)
	{
		DestroyDeliveryPassengerQuest(iQuest);
	}
	else
	{
		string sParentNPC = Quests[iQuest].l.parent_npc;

		sQuest = "delivery_passenger_quest_timer_for_" + sParentNPC + "_rand_"+rand(10000);
		
		//characters[GetCharacterIndex(sParentNPC)].CurrentQuest.pquest = sQuest;

		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 1);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "Punishment_for_delivery_passenger_quest";
		pchar.quest.(sQuest).quest = iQuest;

		Quests[iQuest].l.contribution = sti(Quests[iQuest].l.contribution) + 1;
		Quests[iQuest].l.contribution.quest = sQuest;
	}
}

void PrepareCompleteDeliveryPassengerQuest(string sQuest)
{
	int iQuest = sti(pchar.quest.(sQuest).quest);
	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	string sPassenger;
	ref refPassenger;
	if(ishovernor == 0)
	{
		//ïîìåùàåì ïåðñîíàæà â ãîðîä íà ñâîáîäíûé ëîêàòîð
		sPassenger = Quests[iQuest].l.passenger;
		refPassenger = CharacterFromID(sPassenger);
		RemovePassenger(pchar, refPassenger);
		LAi_RemoveLoginTime(refPassenger);
		PlaceCharacter(refPassenger, "goto", "nonrandom");
		//LAi_SetActorType(refPassenger);
		//LAi_ActorFollow(refPassenger, pchar, "", -1);
		LAi_CharacterEnableDialog(refPassenger);
		//RemovePassengerWeapon(refPassenger);
		LAi_SetFollowerType(refPassenger);
		refPassenger.greeting = "cit_complete_" + (rand(3) + 1);
		refPassenger.currentquest = iQuest;
		refPassenger.currentquest.type = DELIVERY_PASSENGER_QUEST;
		refPassenger.quest.questflag = 2;
		refPassenger.quest.questflag.model = SetModelFlag(refPassenger);
		int iContribution = sti(Quests[iQuest].l.contribution);
		if(iContribution > 0)
		{
			refPassenger.dialog.currentnode = "Failed_Travel";
		}
		else
		{
			refPassenger.dialog.currentnode = "Successful_Travel";
		}
		refPassenger.dialog.filename = "Passenger_dialog.c";
		refPassenger.greeting = "cit_complete_" + (rand(3) + 1);
		sQuest = Quests[iQuest].l.contribution.quest;
		pchar.quest.(sQuest).over = "yes";
	}
	else
	{
		int isDelayed = sti(Quests[iQuest].l.isdelayed);
		if(isDelayed == 0)
		{
			sPassenger = Quests[iQuest].l.passenger;
			refPassenger = CharacterFromID(sPassenger);
			RemovePassenger(pchar, refPassenger);
			LAi_RemoveLoginTime(refPassenger);
			PlaceCharacter(refPassenger, "goto", "nonrandom");
			//LAi_SetOfficerType(refPassenger);
			//RemovePassengerWeapon(refPassenger);
			LAi_SetFollowerType(refPassenger);
			refPassenger.greeting = "cit_complete_" + (rand(3) + 1);
			//LAi_ActorFollow(refPassenger, pchar, "", -1);
			LAi_CharacterEnableDialog(refPassenger);
			refPassenger.quest.questflag = 2;
			refPassenger.quest.questflag.model = SetModelFlag(refPassenger);
			refPassenger.dialog.currentnode = "Successful_Travel";
			refPassenger.dialog.filename = "Passenger_dialog.c";
			refPassenger.greeting = "cit_complete_" + (rand(3) + 1);
			refPassenger.currentquest = iQuest;
			refPassenger.currentquest.type = DELIVERY_PASSENGER_QUEST;
		}
		else
		{
			Quests[iQuest].l.isdelayed = 0;
			Quests[iQuest].l.swapped = 1;
			string sTargetColony = Quests[iQuest].l.target_colony;
			Quests[iQuest].l.target_colony = Quests[iQuest].l.parent_colony;
			Quests[iQuest].l.parent_colony = sTargetColony;
			sPassenger = Quests[iQuest].l.passenger;
			refPassenger = CharacterFromID(sPassenger);
			RemovePassenger(pchar, refPassenger);
			LAi_RemoveLoginTime(refPassenger);
			PlaceCharacter(refPassenger, "goto", "nonrandom");
			//LAi_SetOfficerType(refPassenger);
			//RemovePassengerWeapon(refPassenger);
			//LAi_SetActorType(refPassenger);
			//LAi_ActorFollow(refPassenger, pchar, "", -1);
			LAi_CharacterEnableDialog(refPassenger);
			LAi_SetFollowerType(refPassenger);
			refPassenger.greeting = "cit_quest_" + (rand(3) + 1);
			refPassenger.quest.questflag = 2;
			refPassenger.quest.questflag.model = SetModelFlag(refPassenger);
			refPassenger.dialog.currentnode = "Passenger_delayed";
			refPassenger.dialog.filename = "Passenger_dialog.c";
			refPassenger.greeting = "cit_quest_" + (rand(3) + 1);
			refPassenger.currentquest = iQuest;
			refPassenger.currentquest.type = DELIVERY_PASSENGER_QUEST;
		}
	}
}

void CompletePassengerQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	string sLogBook = Quests[iQuest].l.logbook;
	int icontributtion = sti(Quests[iQuest].l.contribution);
	int isumm = sti(Quests[iQuest].l.money);

	string sLocator;
	int isContinue;

	string sPatent;

	if(ishovernor == 0)
	{
		chr.quest.questflag.model = "";
		DeleteAttribute(chr, "quest.questflag");

		if(icontributtion == 0)
		{
			AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "2");
		}
		else
		{
			isumm = isumm - (isumm*icontributtion*5/100);
			AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "4");
			AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
		}

		AddMoneyToCharacter(pchar, isumm);
		AddPartyExp(pchar, isumm/10);

		isContinue = sti(Quests[iQuest].l.iscontinue);
		if(isContinue == 0)
		{
			CloseQuestHeader(sLogBook);
		}
		DeleteAttribute(&Quests[iQuest], "l"));
		EmptyAllQuestData(iQuest);
		FoundAndDeleteAllQuestForQuestNumber(iQuest);

		LAi_SetActorType(chr);
		sLocator = FindNearestFreeLocator("reload");
		LAi_ActorGoToLocation(chr, "reload", sLocator, "none", "", "", "", 20.0);
		DeleteAttribute(chr, "characterlock");
		DeleteAttribute(chr, "currentquest");
		chr.quest.questflag.model = "";
		DeleteAttribute(chr, "quest.questflag");
	}
	else
	{
		string sStep = "";
		int isSwapped = sti(Quests[iQuest].l.swapped);
		//int isDelayed = sti(Quests[iQuest].l.isdelayed);
		if(isSwapped == 0)
		{
			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag");

			if(icontributtion == 0)
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "7");
			}
			else
			{
				isumm = isumm - (isumm*icontributtion*5/100);
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "8");
				AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
			}
			FoundAndDeleteAllQuestForQuestNumber(iQuest);
			Quests[iQuest].l.swapped = 2;
			LAi_SetActorType(chr);
			sLocator = FindNearestFreeLocator("reload");
			LAi_ActorGoToLocation(chr, "reload", sLocator, "none", "reload", "reload1", "", 15.0);
			DeleteAttribute(chr, "characterlock");
			DeleteAttribute(chr, "currentquest");
			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag");
		}
		if(isSwapped == 1)
		{
			//âàðèàíò, êîãäà ìû âåðíóëèñü çà íàãðàäîé
			string sPassenger = Quests[iQuest].l.passenger;
			ref refPassenger = CharacterFromID(sPassenger);
			RemovePassenger(pchar, refPassenger);

			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag");

			isumm = isumm - (isumm*icontributtion*5/100);

			AddMoneyToCharacter(pchar, isumm);
			if(!CheckAttribute(&Quests[iQuest], "l.subtype"))
			{
				AddPartyExp(pchar, isumm/10);
			}
			else
			{
				if(Quests[iQuest].l.subtype == "story")
				{
					sStep = "l" + sti(chr.FranceQuestProgress);
					chr.FranceQuestProgress.(sStep).iscompleted = 1;
					if(!CheckAttribute(chr, "FranceQuestProgress"))
					{
						chr.FranceQuestProgress = 1;
					}
					AddPartyExp(pchar, sti(chr.FranceQuestProgress) * 500);
				}
			}

			
			if(icontributtion == 0)
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "2");
			}
			else
			{
				isumm = isumm - (isumm*icontributtion*5/100);
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "4");
				AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
			}

			isContinue = sti(Quests[iQuest].l.iscontinue);
			if(isContinue == 0)
			{
				CloseQuestHeader(sLogBook);
			}

			int iNation = sti(chr.nation);
			ChangeCharacterReputation(pchar, 1, iNation);
			int iCareerReward = sti(QuestsShablons[DELIVERY_PASSENGER_QUEST].l.careerreward);
			chr.career.change = ChangeCharacterCareerProgress(iNation, iCareerReward);

			DeleteAttribute(&Quests[iQuest], "l"));
			EmptyAllQuestData(iQuest);
			FoundAndDeleteAllQuestForQuestNumber(iQuest);

			EraseQuestFreeState(chr);
			GenerateHovernorQuests(chr);
			//CheckQuestForCharacter(chr);
			chr.quest.questflag.model = SetModelFlag(chr);

			sPatent = GetNationNameByType(sti(chr.nation)) + "_career_counter";
			pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
		}

		if(isSwapped == 2)
		{
			//âàðèàíò, êîãäà ìû âåðíóëèñü çà íàãðàäîé

			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag");

			isumm = isumm - (isumm*icontributtion*5/100);

			AddMoneyToCharacter(pchar, isumm);
			if(!CheckAttribute(&Quests[iQuest], "l.subtype"))
			{
				AddPartyExp(pchar, isumm/10);
			}
			else
			{
				if(Quests[iQuest].l.subtype == "story")
				{
					sStep = "l" + sti(chr.FranceQuestProgress);
					chr.FranceQuestProgress.(sStep).iscompleted = 1;
					if(!CheckAttribute(chr, "FranceQuestProgress"))
					{
						chr.FranceQuestProgress = 1;
					}
					AddPartyExp(pchar, sti(chr.FranceQuestProgress) * 500);
				}
			}

			isContinue = sti(Quests[iQuest].l.iscontinue);
			if(isContinue == 0)
			{
				CloseQuestHeader(sLogBook);
			}

			if(icontributtion == 0)
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "2");
			}
			else
			{
				AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "4");
				AddQuestUserData(sLogBook, "sTardiness", FindRussianDaysString(icontributtion));
			}

			DeleteAttribute(&Quests[iQuest], "l"));
			EmptyAllQuestData(iQuest);
			FoundAndDeleteAllQuestForQuestNumber(iQuest);

			EraseQuestFreeState(chr);
			GenerateHovernorQuests(chr);
			
			chr.quest.questflag.model = SetModelFlag(chr);

			sPatent = GetNationNameByType(sti(chr.nation)) + "_career_counter";
			pchar.(sPatent).tasks.completed = sti(pchar.(sPatent).tasks.completed) + 1;
		}
	}
}

void DestroyDeliveryPassengerQuest(int iQuest)
{
	string sQuest = "l"+iQuest;

	string sLogBook = Quests[iQuest].l.logbook;

	string sParentNPC = Quests[iQuest].l.parent_npc;
	ref pChr = CharacterFromID(sParentNPC);

	AddQuestRecordEx(sLogBook, "DELIVERY_PASSENGER_QUEST", "5");

	sQuest = Quests[iQuest].l.contribution.quest;
	pchar.quest.(sQuest).over = true;

	DeleteAttribute(pchar, "quests." + sQuest);

	int isContinue = sti(Quests[iQuest].l.iscontinue);
	if(isContinue == 0)
	{
		CloseQuestHeader(sLogBook);
	}


	int ishovernor = sti(Quests[iQuest].l.ishovernor);
	if(ishovernor == 0)
	{
		ChangeCharacterReputation(pchar, -5, 6);
	}
	else
	{
		int iNation = sti(pChr.nation);
		ChangeCharacterReputation(pchar, -1, iNation);
		int iCareerReward = sti(QuestsShablons[DELIVERY_PASSENGER_QUEST].l.careerreward);
		int iCareer = ChangeCharacterCareerProgress(iNation, -iCareerReward);
		pChr.career.change = iCareer;
		iNation = sti(pchar.nation);
		string sPatent = GetNationNameByType(iNation) + "_career_counter";
		pchar.(sPatent).tasks.ruined = sti(pchar.(sPatent).tasks.ruined) + 1;
	}

	ref chr = characterFromID(Quests[iQuest].l.passenger);
	
	if(chr.id != pChr.id)
	{
		DeleteAttribute(pChr, "CurrentQuest");
		EraseQuestFreeState(pChr);
	}
	
	string sRemovePassQuest = "EnterToAnyTown_for_character" + chr.id + "_" + rand(1000);
	pchar.quest.(sRemovePassQuest).win_condition.l1 = "town_location";
	pchar.quest.(sRemovePassQuest).function = "EraseFailedPassengerQuest";
	pchar.quest.(sRemovePassQuest).character = chr.id;

	//DeleteAttribute(chr, "characterlock");
	//DeleteAttribute(Chr, "CurrentQuest");


	Quests[iQuest].id = "-1";

	DeleteAttribute(&Quests[iQuest], "l"));

	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.taken = "-1";

	EmptyAllQuestData(iQuest);
	FoundAndDeleteAllQuestForQuestNumber(iQuest);
}

void EraseFailedPassengerQuest(string sQuest)
{
	string sCharacter = pchar.quest.(sQuest).character;
	int iChar = GetCharacterIndex(sCharacter);

	if(isPrisonedChar(&characters[iChar]) == false)
	{
		RemovePassenger(pchar, &characters[iChar]);
		DeleteAttribute(&characters[iChar], "characterlock");
		DeleteAttribute(&characters[iChar], "CurrentQuest");
	}
}

void DeletePassengerQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);	

	EmptyAllQuestData(iQuest);
	FoundAndDeleteAllQuestForQuestNumber(iQuest);

	int ishovernor = sti(Quests[iQuest].l.ishovernor);

	if(ishovernor == 0)
	{
		Quests[iQuest].id = "-1";
		DeleteAttribute(&Quests[iQuest], "l"));
		QuestsShablons[DELIVERY_PASSENGER_QUEST].l.taken = "-1";

		LAi_SetActorType(chr);
		string sLocator = FindNearestFreeLocator("reload");
		LAi_ActorGoToLocation(chr, "reload", sLocator, "none", "reload", "reload1", "", 15.0);
		DeleteAttribute(chr, "characterlock");
		DeleteAttribute(chr, "currentquest");
		chr.quest.questflag.model = "";
		DeleteAttribute(chr, "quest.questflag");
	}
}

int CheckForSuccesDeliveryPassenger(aref chr)
{
	if(CheckAttribute(chr, "CurrentQuest.type"))
	{
		int iQuest = sti(chr.currentquest);
		int iType = sti(chr.CurrentQuest.type);
		if(iType == DELIVERY_PASSENGER_QUEST)
		{
			int iPassenger = GetCharacterIndex(Quests[iQuest].l.passenger);
			int isSwapped = sti(Quests[iQuest].l.swapped);
			int isDelayed = sti(Quests[iQuest].l.isdelayed);
			int iconrtibution = 0;
			if(sti(chr.index) != iPassenger)
			{
				if(isSwapped == 2 && isDelayed == 0)
				{
					if(GetPassengerNumber(pchar, iPassenger) == -1)
					{
						iconrtibution = sti(Quests[iQuest].l.contribution);
						if(iconrtibution > 0)
						{
							chr.quest.punishment = Quests[iQuest].l.contribution;
						}
						return 2; // "take quest" flag on
					}
				}
				if(isSwapped == 1 && isDelayed == 0)
				{
					if(GetPassengerNumber(pchar, iPassenger) != -1)
					{
						iconrtibution = sti(Quests[iQuest].l.contribution);
						if(iconrtibution > 0)
						{
							chr.quest.punishment = Quests[iQuest].l.contribution;
						}
						return 2; // "take quest" flag on
					}
				}
				if(CheckAttribute(chr,"CurrentQuest.accepted"))
				{
					if(chr.CurrentQuest.accepted == false)
					{
						return 1;
					}
				}
				return 3;// disabled "take quest" flag on
			}
			else
			{
				int iLoc = FindLocation(pchar.location);
				if (iLoc != -1)
				{
					if(CheckAttribute(&Locations[iLoc], "fastreload"))
					{
						string sColony = Locations[iLoc].fastreload;
						//int isSwapped = sti(Quests[iQuest].l.swapped);
						if(sColony == Quests[iQuest].l.target_colony)
						{
							iconrtibution = sti(Quests[iQuest].l.contribution);
							if(iconrtibution > 0)
							{
								chr.quest.punishment = Quests[iQuest].l.contribution;
							}
							return 2;
						}
						if(isSwapped == 1)
						{
							if(sColony == Quests[iQuest].l.parent_colony)
							{
								return 2;
							}
						}
					}
				}
				return 1;
			}
		}
	}
	return -1; // no quest flag
}

void RemovePassengerWeapon(aref refPassenger)
{
	LAi_SetFightMode(refPassenger, false);
	LAi_LockFightMode(refPassenger, true);
	//DeleteAttribute(refPassenger, "equip");
	//DeleteAttribute(refPassenger, "items");
	//RemoveCharacterEquip(refPassenger, BLADE_ITEM_TYPE);
	//RemoveCharacterEquip(refPassenger, GUN_ITEM_TYPE);
}

*/
