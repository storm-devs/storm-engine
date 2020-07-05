
int GenerateSlavesQuest(aref chr)
{
	trace("Generate Slaves Quest Begin");

	int iQuest;

	if(CheckAttribute(chr, "CurrentQuest"))
	{
		iQuest = sti(chr.CurrentQuest);
		if(sti(Quests[iQuest].l.type) == HUNT_FOR_SLAVES_QUEST)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	iQuest = FindFirstEmptyQuest();
	
	if(iQuest == -1)
	{
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////
	/// ÎÁÜßÂËßÅÌ ÊÎËÈ×ÅÑÒÂÎ ÍÅÎÁÕÎÄÈÌÛÕ ÐÀÁÎÂ
	//////////////////////////////////////////////////////////////////////////////

	int iSlaves = rand(100) + 20;

	string sHomeColony = FindColonyByCharacter(chr);
	if(sHomeColony == "-1")
	{
		return -1;
	}
	int curStoreIdx = FindStore(sHomeColony);
	int iCost = GetStoreGoodsPrice(&Stores[curStoreIdx],GOOD_SLAVES,PRICE_TYPE_BUY,pchar,1);
	iCost = iCost * iSlaves;

	Quests[iQuest].id = "HuntForSlaves_" + chr.id + "_" + rand(10000);
	Quests[iQuest].l.slaves = iSlaves;
	Quests[iQuest].l.money = iCost;
	Quests[iQuest].l.target_colony = sHomeColony;
	Quests[iQuest].l.parent_npc = chr.id;
	Quests[iQuest].l.type = HUNT_FOR_SLAVES_QUEST;

	chr.CurrentQuest = iQuest;
	chr.CurrentQuest.state = QUEST_NOT_ACCEPTED;
	chr.CurrentQuest.type = HUNT_FOR_SLAVES_QUEST;

	chr.quest.questflag = 1;
	chr.quest.questflag.model = SetModelFlag(chr);

	return 1; // "give quest" flag on
}

void AcceptSlavesQuest(aref chr)
{
	chr.CurrentQuest.state = QUEST_ACCEPTED;
	DeleteAttribute(chr, "CurrentQuest.lastdialog");
	int iQuest = sti(chr.CurrentQuest);

	string sQuest = "l"+iQuest;
	pchar.quests.(sQuest) = iQuest;

	string sLogBook = "HUNT_FOR_SLAVES_QUEST_FOR_" + chr.id + "_" + rand(100000);
	SetQuestHeaderEx(sLogBook, "HUNT_FOR_SLAVES_QUEST");
	AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+Quests[iQuest].l.target_colony));
	AddQuestRecordEx(sLogBook, "HUNT_FOR_SLAVES_QUEST", "1");
	AddQuestUserData(sLogBook, "sCharName", GetCharacterFullName(chr.id));
	AddQuestUserData(sLogBook, "slaves", FindRussianSlavesString(sti(Quests[iQuest].l.slaves), "Acc"));
	AddQuestUserData(sLogBook, "sMoney", FindRussianMoneyString(sti(Quests[iQuest].l.money)));
	AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony" + Quests[iQuest].l.target_colony));
	AddQuestUserData(sLogBook, "island", XI_ConvertString(colonies[FindColony(Quests[iQuest].l.target_colony)].island));
	Quests[iQuest].l.logbook = sLogBook;

	NPCGiveQuest(chr);

	//string sModel;
	chr.quest.questflag = CheckSlaveQuest(chr);
}

int CheckSlaveQuest(aref chr)
{
	if(CheckAttribute(chr, "CurrentQuest.type"))
	{
		if(!CheckAttribute(chr, "currentquest.state"))
		{
			return 1;
		}
		if(sti(chr.CurrentQuest.state) == QUEST_NOT_ACCEPTED)
		{
			return 1;
		}
		if(sti(chr.CurrentQuest.type) == HUNT_FOR_SLAVES_QUEST)
		{
			int iQuest = sti(chr.CurrentQuest);
			int iNeedSlaves = sti(Quests[iQuest].l.slaves);
			int iCurSlaves = GetCargoGoods(pchar, GOOD_SLAVES);
			if(iCurSlaves >= iNeedSlaves)
			{
				return 2; // "take quest" flag on
			}
			return 3; // disabled "take quest" flag on
		}
	}
	return -1; // no quest flag
}

void CompleteSlavesQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	string sLogBook = Quests[iQuest].l.logbook;

	AddQuestRecordEx(sLogBook, "HUNT_FOR_SLAVES_QUEST", "2");
	CloseQuestHeader(sLogBook);

	AddMoneyToCharacter(pchar, sti(Quests[iQuest].l.money));
	RemoveCharacterGoods(pchar, GOOD_SLAVES, sti(Quests[iQuest].l.slaves));

	//chr.quest.questflag.model = "";
	DeleteSlavesQuest(chr);
}

void DeleteSlavesQuest(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	Quests[iQuest].id = "-1";
	DeleteAttribute(&Quests[iQuest], "l"));
	EmptyAllQuestData(iQuest);

	EraseQuestFreeState(chr);
	LAi_SetActorType(chr);
	string sLocator = FindNearestFreeLocator("reload");
	LAi_ActorGoToLocation(chr, "reload", sLocator, "NONE", "", "", "", 20.0);
	DeleteAttribute(chr, "characterlock");
	EmptyFantomCharacacter(chr);

	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");
}