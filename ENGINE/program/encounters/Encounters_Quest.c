// Map quest encounters
#define MAX_QUEST_MAP_ENCOUNTERS	20
object QuestMapEncounters[MAX_QUEST_MAP_ENCOUNTERS];

void InitQuestMapEncounters()
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (!CheckAttribute(&QuestMapEncounters[i], "id")) { continue; }
		DeleteAttribute(&QuestMapEncounters[i], "");
	}
}

int FindQuestMapEncounter(string sGroupID)
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (!CheckAttribute(&QuestMapEncounters[i], "id")) { continue; }
		if (QuestMapEncounters[i].id == sGroupID) { return i; }
	}
	return -1;
}

ref Encounter_GetQuestMapEncounter(int iEncounterIndex)
{
	return &QuestMapEncounters[iEncounterIndex];
}

int Encounter_FindFirstQuestMapEncounter(int iEncType)
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (CheckAttribute(&QuestMapEncounters[i], "id")) 
		{ 
			if (sti(QuestMapEncounters[i].Type) == iEncType) { return i; }
		}
	}
	return -1;
}

void Encounter_DeleteQuestMapEncounter(string sGroupID)
{
	int iEncounter = FindQuestMapEncounter(sGroupID);
	if (iEncounter < 0) { return; }

	DeleteAttribute(&QuestMapEncounters[iEncounter], "");
	//рэйнжер продолжает путь
	
	/*
	if (characters[GetCharacterIndex(sGroupID)].adventure.type == "trade")
	{
		TradeQuest(sGroupID, rand(10)+1);
	}
	*/
}

void Encounter_DeleteDeadQuestMapEncounters()
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (!CheckAttribute(&QuestMapEncounters[i], "id")) { continue; }
		if (Group_isDead(QuestMapEncounters[i].id))
		{
//			CheckForDeadRanger(QuestMapEncounters[i].id);
			DeleteAttribute(&QuestMapEncounters[i], "");
		}
	}
}

int FindFreeQuestMapEncounterSlot()
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (!CheckAttribute(&QuestMapEncounters[i], "id")) { return i; }
	}
	return -1;
}
// boal -->
int FindFreeOrExistsQuestMapEncounterSlot(string newId)
{
	for (int i=0; i<MAX_QUEST_MAP_ENCOUNTERS; i++)
	{
		if (CheckAttribute(&QuestMapEncounters[i], "id") &&  QuestMapEncounters[i].id == newId) { return i; }
	}
	return FindFreeQuestMapEncounterSlot();
}
// boal <--

// не работает!!!  теперь все иначе, через Map_CreateWarrior
void Encounter_SetQuestMapEncounter(string sGroupID, int iEncType, bool bPermanent, int iRandomMeeting)
{
	int iEncounter = FindFreeOrExistsQuestMapEncounterSlot(sGroupID);//fix 09/01/05 boal
	if (iEncounter == -1)
	{
		Trace("Encounter_SetQuestMapEncounter: Not enought Quest Map Encounters slots = " + MAX_QUEST_MAP_ENCOUNTERS);
		return;
	}
	ref rEnc = &QuestMapEncounters[iEncounter];

    DeleteAttribute(rEnc, ""); //fix
    
	if (iRandomMeeting > 100)	{ iRandomMeeting = 100; }
	if (iRandomMeeting < 0)		{ iRandomMeeting = 0; }

	rEnc.ID = sGroupID;
	rEnc.Type = iEncType;
	rEnc.Permanent = bPermanent;
	rEnc.Rand = iRandomMeeting;
}