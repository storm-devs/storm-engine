#define MAX_SHIP_GROUPS  350

object	AIGroups[MAX_SHIP_GROUPS];

#event_handler(GROUP_CHECKTASKEVENT, "Group_CheckTask");

void SeaAIGroupsInit()
{
	for (int i=0;i<MAX_SHIP_GROUPS;i++)
	{
		DeleteAttribute(&AIGroups[i],"");
	}
}

// delete group by index
void Group_DeleteGroupIndex(int iGroupIndex)
{
	if (iGroupIndex < 0 || iGroupIndex >= MAX_SHIP_GROUPS)
	{
		Trace("Group_DeleteGroupIndex iGroupIndex is out of range = " + iGroupIndex);
		return;
	}
	// boal super fix 03/02/2005 -->
	for (int i=0; i<MAX_SHIP_GROUPS; i++)
	{
		if (CheckAttribute(&AIGroups[i], "id") && CheckAttribute(&AIGroups[i], "Task.Target"))
		{
			if (AIGroups[i].Task.Target == AIGroups[iGroupIndex].id)
			{
				Group_SetTaskNone(AIGroups[i].id);
			}
		}
	}
	// boal super fix 03/02/2005 <--

	DeleteAttribute(&AIGroups[iGroupIndex], "");
}

// delete group by ID
void Group_DeleteGroup(string sGroupID)
{
	int iGroupIndex = Group_FindGroup(sGroupID);
	if (iGroupIndex < 0)
	{
		if (sGroupID != PLAYER_GROUP && MOD_BETTATESTMODE == "On") { Trace("Group_DeleteGroup sGroupID = " + sGroupID + ", can't find group"); }
		return;
	}
	Group_DeleteGroupIndex(iGroupIndex);
}

// if you call Group_DeleteUnusedGroup() all group marked this will be deleted
void Group_DeleteAtEnd(string sGroupID)
{
	ref rGroup = Group_GetGroupByID(sGroupID);
	rGroup.DelAtEnd = true;
}

// return group type
string Group_GetTypeR(ref rGroup) { return rGroup.Type; }
string Group_GetType(string sGroupID)
{
	ref rGroup = Group_GetGroupByID(sGroupID);
	return Group_GetTypeR(rGroup);
}

// set group type: "war" | "trade" | "pirate"
void Group_SetType(string sGroupID, string sGroupType)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Type = sGroupType;

	if (isEntity(&AISea))
	{
		SendMessage(&AISea, "lss", AI_MESSAGE_GROUP_SET_TYPE, sGroupID, sGroupType);
	}
}

// create a new AI group for ships, return -1 if can't create
int Group_CreateGroup(string sGroupID)
{
	for (int i=0;i<MAX_SHIP_GROUPS;i++)
	{
		if (!CheckAttribute(&AIGroups[i],"id"))
		{
			//Trace("Group_CreateGroup create = " + sGroupID);
			AIGroups[i].id = sGroupID;
			Group_SetTaskNone(sGroupID);
			Group_SetType(sGroupID, "war");
			AIGroups[i].MainCharacter = "";
			return i;
		}
	}
	Trace("All slots in AIGroups are used, need to resize array!!!");
	if (MOD_BETTATESTMODE == "On") Log_Info("Error: Group_CreateGroup нет места в массиве");
	return -1;
}
// boal потереть мертвые
void Group_FreeAllDead()
{
	if (!bSeaActive)
	{
		for (int i=0;i<MAX_SHIP_GROUPS;i++)
		{
			if (CheckAttribute(&AIGroups[i], "id"))
			{
				if (Group_isDeadR(&AIGroups[i]))
				{
				    Group_DeleteGroupIndex(i);
				}
				else
				{
				    // таймер to_do
				}
			}
		}
	}
}

ref Group_GetGroupByID(string sGroupID)
{
	int	iGroupIndex = Group_FindGroup(sGroupID);
	// if (iGroupIndex < 0)
	return Group_GetGroupByIndex(iGroupIndex);
}

ref	Group_GetGroupByIndex(int iGroupIndex)
{
    if (iGroupIndex < 0) // fix
    {
        if (MOD_BETTATESTMODE == "On") Log_Info("Error: Group_GetGroupByIndex нет группы");
        return &NullCharacter;
    }
    return &AIGroups[iGroupIndex];
}

// find AI group for ships
int	Group_FindGroup(string sGroupID)
{
	for (int i=0;i<MAX_SHIP_GROUPS;i++)
	{
		if (!CheckAttribute(&AIGroups[i],"id")) { continue; }
		if (sGroupID == AIGroups[i].id) { return i; }
	}
	//Trace("Can't find SeaAI group = " + sGroupID);
	//if (bQuestLogShow) Log_Info("Error: Group_FindGroup нет группы "+sGroupID);
	return -1;
}

ref	Group_FindOrCreateGroup(string sGroupID)
{
	if(strlen(sGroupID) == 0)
	{
		Trace("sGroupID have no name!");
		//AIGroups[iGroupIndex].Task.Lock = false;
		return &AIGroups[0];
	}
	int iGroupIndex = Group_FindGroup(sGroupID);
	if (iGroupIndex < 0)
	{
		iGroupIndex = Group_CreateGroup(sGroupID);
		if (iGroupIndex < 0) 
		{
			Trace("Group_FindOrCreateGroup sGroupID = " + sGroupID + ", can't create group ");
			if (MOD_BETTATESTMODE == "On") Log_Info("Error: Group_FindOrCreateGroup не создалась группа " + sGroupID);
			// бред - просто плодили еррор AIGroups[iGroupIndex].Task.Lock = false;
			return &AIGroups[0];
		}
	}

	return &AIGroups[iGroupIndex];
}

// set group address to none
void Group_SetAddressNone(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Location = "none";
}

// set location near other group
void Group_SetPursuitGroup(string sGroupID, string sPursuitGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Location.neargroup = sPursuitGroupID;

	if (isEntity(&AISea))
	{
		SendMessage(&AISea, "lss", AI_MESSAGE_GROUP_SET_LOCATION_OTHER_GROUP, sGroupID, sPursuitGroupID);
	}
}

// set new address for group
void Group_SetAddress(string sGroupID, string sLocationID, string sLocationGroup, string sLocationLocator)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Location = sLocationID;
	rGroup.Location.group = sLocationGroup;
	rGroup.Location.locator = sLocationLocator;
}

void Group_DeleteUnusedGroup()
{
	for (int i=0;i<MAX_SHIP_GROUPS;i++)	
	{
		ref rGroup = Group_GetGroupByIndex(i);
		if (CheckAttribute(rGroup, "DelAtEnd"))
		{
			Group_DeleteGroupIndex(i);
		}
	}
}

// ======================================================================================================
// =										CHARACTER MANAGER											=
// ======================================================================================================

// Get main character for group
ref Group_GetGroupCommanderR(ref rGroup) { return GetCharacter(GetCharacterIndex(rGroup.MainCharacter)); }

int Group_GetGroupCommanderIndexR(ref rGroup) { return GetCharacterIndex(rGroup.MainCharacter); }
int Group_GetGroupCommanderIndex(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	return Group_GetGroupCommanderIndexR(rGroup);
}

ref Group_GetGroupCommander(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	if (!CheckAttribute(rGroup, "MainCharacter") && MOD_BETTATESTMODE == "On")
	{
		trace("ERROR: Group_GetGroupCommander нет коммандера для " + sGroupID);
		Log_Info("ERROR: Group_GetGroupCommander нет коммандера для " + sGroupID);
	}
	return Group_GetGroupCommanderR(rGroup);
}

// Set group commander
void Group_SetGroupCommander(string sGroupID, string sCharacterID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.MainCharacter = sCharacterID;

	if (bSeaActive)
	{
		ref rCharacter = GetCharacter(GetCharacterIndex(sCharacterID));
		SendMessage(&AISea, "lsa", AI_MESSAGE_GROUP_SET_COMMANDER, sGroupID, rCharacter);
	}
}

// delete character from others group, add character to new group
void Group_ChangeCharacter(string sGroupID, string sCharacterID)
{
	for (int i=0;i<MAX_SHIP_GROUPS;i++)	
	{
		ref rOldGroup = Group_GetGroupByIndex(i);
		if (CheckAttribute(rOldGroup, "id"))
		{
			Group_DelCharacter(rOldGroup.id, sCharacterID);
		}
	}

	Group_AddCharacter(sGroupID, sCharacterID);
}

// add new character to group
void Group_AddCharacter(string sGroupID, string sCharacterID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	
	// add new character to list
	int iCharacterIndex = GetCharacterIndex(sCharacterID);
	string sTemp = "id_" + iCharacterIndex;
	rGroup.Quest.(sTemp) = sCharacterID;
	rGroup.Quest.(sTemp).index = iCharacterIndex;

	// DANGER 
	Characters[iCharacterIndex].SeaAI.Group.Name = sGroupID;
}
// boal 03/08/06 метод числа живых в группе
int Group_GetLiveCharactersNum(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	return (Group_GetCharactersNumR(rGroup) - Group_GetDeadCharactersNumR(rGroup));
}

int Group_GetDeadCharactersNum(string sGroupID) { ref rGroup = Group_FindOrCreateGroup(sGroupID); return Group_GetDeadCharactersNumR(rGroup); }
int Group_GetDeadCharactersNumR(ref rGroup)
{
	int iDeads = 0;
	int iNum = Group_GetCharactersNumR(rGroup);
	for (int i=0; i<iNum; i++)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, i);
		if (iCharacterIndex < 0) { iDeads++; }
		if (sti(Characters[iCharacterIndex].Ship.Type) == SHIP_NOTUSED || LAi_IsDead(&Characters[iCharacterIndex])) { iDeads++; }  // fix фантомов
	}
	return iDeads;
}

int Group_GetCharactersNum(string sGroupID) { ref rGroup = Group_FindOrCreateGroup(sGroupID); return Group_GetCharactersNumR(rGroup); }
int Group_GetCharactersNumR(ref rGroup)
{
	aref arQuest; 
	if (!CheckAttribute(rGroup,"quest")) { return 0; }
	makearef(arQuest,rGroup.Quest);
	return GetAttributesNum(arQuest);
}

int Group_GetCharacterIndexR(ref rGroup, int iIndex)
{
	aref arQuest, arAttr; 
	if (!CheckAttribute(rGroup,"quest")) { return -1; }
	makearef(arQuest,rGroup.Quest);
	int iNumAttributes = GetAttributesNum(arQuest);
	if (iIndex >= iNumAttributes) { return -1; } 
	arAttr = GetAttributeN(arQuest, iIndex);
	return sti(arAttr.index);
}

// delete character from group
void Group_DelCharacter(string sGroupID, string sCharacterID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	string sTemp = "Quest.id_" + GetCharacterIndex(sCharacterID);

	if (CheckAttribute(rGroup, sTemp))
	{
		DeleteAttribute(rGroup, sTemp);
	}
}

// ======================================================================================================
// =											GROUP UTILS												=
// ======================================================================================================

bool Group_isDeadR(ref rGroup)
{
	int i = 0;
	int iNumDeadCharacters = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, i);
		if (iCharacterIndex < 0) { break; }
		if (sti(Characters[iCharacterIndex].Ship.Type) == SHIP_NOTUSED || LAi_IsDead(&Characters[iCharacterIndex])) { iNumDeadCharacters++; }  // стертый тоже труп
		i++;
	}
	if (iNumDeadCharacters == i) { return true; }
	return false;
}

bool Group_isDead(string sGroupID)
{
	int	iGroupIndex = Group_FindGroup(sGroupID);
	if (iGroupIndex < 0) { return true; } // fc! вот тут был БАГ!!! нет групы - она мертва - а возвращали false..

	ref rGroup = Group_GetGroupByIndex(iGroupIndex);
	return Group_isDeadR(rGroup);
}

// set start position
void Group_SetXZ_AY(string sGroupID, float x, float z, float ay)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	
	rGroup.Pos.x = x;
	rGroup.Pos.z = z;
	rGroup.Pos.ay = ay;

	if (bSeaActive)
	{
		SendMessage(&AISea, "lsffff", AI_MESSAGE_GROUP_SET_XYZ_AY, sGroupID, x, 0.0, z, ay);
	}
}

float Group_GetAttackHPDistance_R(ref rGroup, float fDistance)
{
	float fHP = 0.0;
	if (bSeaActive)
	{
		SendMessage(&AISea, "lsfe", AI_MESSAGE_GROUP_GET_ATTACK_HP, rGroup.id, fDistance, &fHP);
	}
	return fHP;
}

float Group_GetAttackHP(string sGroupID) { ref rG = Group_FindOrCreateGroup(sGroupID); return Group_GetAttackHP_R(rG); }
float Group_GetAttackHP_R(ref rGroup)
{
	return Group_GetAttackHPDistance_R(rGroup, -1.0);
}

float Group_GetPowerHP(string sGroupID) { ref rG = Group_FindOrCreateGroup(sGroupID); return Group_GetPowerHP_R(rG); }
float Group_GetPowerHP_R(ref rGroup)
{
	float fHP = 0.0;
	int i = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, i); i++;
		if (iCharacterIndex < 0) { break; }
		if (CheckAttribute(&Characters[iCharacterIndex], "Ship.HP")) // fix может быть убитый фантом
		{
			fHP = fHP + stf(Characters[iCharacterIndex].Ship.HP);
		}
		else
		{
		  // не факт что это хорошо, сам написал сам пока убрал (boal)  rGroup.DelAtEnd = true; // стереть, тк командера уже нет
		}
	}	
	return fHP;
}

// ======================================================================================================
// =											TASK SECTION											=
// ======================================================================================================

bool Group_isTaskLockR(ref rGroup) { return sti(rGroup.Task.Lock); }
bool Group_isTaskLock(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	return Group_isTaskLockR(rGroup);
}

// lock task, group can't change task before end current
void Group_LockTask(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Task.Lock = true;
}

// unlock task, group can change tasks 
void Group_UnlockTask(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Task.Lock = false;
}

// Task: Defend
void Group_SetTaskDefend(string sGroupID, string sDefendedGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Task = AITASK_DEFEND;
	rGroup.Task.Target = sDefendedGroupID;
}

// Task: Runaway
void Group_SetTaskRunaway(string sGroupID, string sGroupAwayFrom)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Task = AITASK_RUNAWAY;
	rGroup.Task.Target = "";
	if (bSeaActive)	{ AIRunaway_GroupRunaway(sGroupID, sGroupAwayFrom); }
}

// Task: Attack
void Group_SetTaskAttack(string sGroupID, string sAttackedGroup) { Group_SetTaskAttackEx(sGroupID, sAttackedGroup, true); }
void Group_SetTaskAttackEx(string sGroupID, string sAttackedGroup, bool bTaskBoth)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Task = AITASK_ATTACK;
	rGroup.Task.Target = sAttackedGroup;
	// fix --> //navy тогда уж так ;)
	if (bTaskBoth)
	{
		rGroup = Group_FindOrCreateGroup(sAttackedGroup);
		rGroup.Task = AITASK_ATTACK;
		rGroup.Task.Target = sGroupID;
	}
	// fix <--
	if (bSeaActive) { AIAttack_GroupAttack(sGroupID, sAttackedGroup, bTaskBoth); }
}
// boal иначе вылеты, если в каюте дела
void Group_SetTaskAttackInMap(string sGroupID, string sAttackedGroup)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Task = AITASK_ATTACK;
	rGroup.Task.Target = sAttackedGroup;
	// fix -->
	rGroup = Group_FindOrCreateGroup(sAttackedGroup);

	rGroup.Task = AITASK_ATTACK;
	rGroup.Task.Target = sGroupID;
	// fix <--
}

// Task: Move
void Group_SetTaskMove(string sGroupID, float x, float z)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);
	rGroup.Task = AITASK_MOVE;
	rGroup.Task.Target = "";
	rGroup.Task.Target.Pos.x = x;
	rGroup.Task.Target.Pos.y = 0.0;
	rGroup.Task.Target.Pos.z = z;

	int iIndex = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, iIndex);
		if (iCharacterIndex < 0) { break; }
		iIndex++;
		Ship_SetTaskMove(SECONDARY_TASK,iCharacterIndex,x,z);
	}
}

// Task: Reset to none
void Group_SetTaskNone(string sGroupID)
{
	ref rGroup = Group_FindOrCreateGroup(sGroupID);

	rGroup.Task = AITASK_NONE;
	rGroup.Task.Target = "";
	rGroup.Task.Lock = false;
	
	// boal fix!! 03/02/2005
	int iIndex;
	int iCharacterIndex;
	iIndex = 0;
	while (true)
	{
		iCharacterIndex = Group_GetCharacterIndexR(rGroup, iIndex);
		if (iCharacterIndex < 0) { break; }
		iIndex++;
		Ship_SetTaskNone(SECONDARY_TASK, iCharacterIndex);
	}
}

void Group_SetEnemyToCharacter(string sGroupID, int iEnemyToCharacter)
{
	int iGroupIndex = Group_FindGroup(sGroupID);
	if (iGroupIndex < 0) { return; }

	ref rGroup = Group_GetGroupByIndex(iGroupIndex);

	int i = 0;
	int iNumDeadCharacters = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, i);
		if (iCharacterIndex < 0) { break; }
		SetCharacterRelationBoth(iEnemyToCharacter, iCharacterIndex, RELATION_ENEMY);
		i++;
	}
}

void Group_SetNeutralToCharacter(string sGroupID, int iEnemyToCharacter)
{
	int iGroupIndex = Group_FindGroup(sGroupID);
	if (iGroupIndex < 0) { return; }

	ref rGroup = Group_GetGroupByIndex(iGroupIndex);

	int i = 0;
	int iNumDeadCharacters = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rGroup, i);
		if (iCharacterIndex < 0) { break; }
		SetCharacterRelationBoth(iEnemyToCharacter, iCharacterIndex, RELATION_NEUTRAL);
		i++;
	}
}


void Group_CheckTask()
{
	string sGroupID = GetEventData();
	if (sGroupID == PLAYER_GROUP) { return; }
	ref rGroup = Group_GetGroupByID(sGroupID);
    if (!CheckAttribute(rGroup, "Task")) // fix
    {
        if (MOD_BETTATESTMODE == "On") Log_Info("Error: Group_CheckTask нет группы");
        return;
    }
	switch (sti(rGroup.Task))
	{
		case AITASK_DEFEND:
			AIDefend_CheckTask(sGroupID);
		break;
		case AITASK_ATTACK:
			AIAttack_CheckTask(sGroupID);
		break;
	}
}