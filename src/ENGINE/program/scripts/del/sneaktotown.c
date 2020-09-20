// to_do
/*
void SneakToTown(string Island, string group, string locName)
{
	
	float x = stf(pchar.ship.pos.x);
	float z = stf(pchar.ship.pos.z);
	
	pchar.sneak.x = x;
	pchar.sneak.z = z;
	
	
	pchar.sneak.success = 0;

	pchar.sneak.locName = locName;
	pchar.sneak.islandname = Island;
	pchar.sneak.reload = bCanEnterToLand;
	
	bCanEnterToLand = true;
	Sea_LandLoad(SNEAK_RELOAD);	
	bCanEnterToLand = pchar.sneak.reload;

	int iIsland = FindIsland(Island);
	aref aIsland, aLocators;
	ref rIsland = &islands[iIsland];
	makearef(aIsland, rIsland.reload);
	int iNum =  GetAttributesNum(aIsland);
	string sColony = "-1";
	for(int i = 0; i < iNum; i++)
	{
		aLocators = GetAttributeN(aIsland, i);
		if(aLocators.name == locName)
		{
			sColony = aLocators.go;
			sColony = locations[FindLocation(sColony)].fastreload;
		}
	}
	int iColony = FindColony(sColony);
	if(sColony != "-1" && colonies[iColony].disease == "0")
	{
		CheckForColonyContamination(sColony);
	}

	if(colonies[iColony].nation != "none")
	{
		int iNation = sti(colonies[iColony].nation);
		string sNation = GetNationNameByType(iNation);
		string sSoldiersGroup = sNation + "_CITIZENS";
		string sCitizensGroup = sNation + "_CITIZENS";

		LAi_group_SetRelation(sSoldiersGroup, LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
		LAi_group_SetRelation(sCitizensGroup, LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
	}
} 

void CheckForSuccessSneak(int iColony)
{
	int iLuck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
	int iFame = sti(pchar.fame);
	int iRand = rand(100) + iLuck;

	string sLoc = Colonies[iColony].id + "_Town";
	string sQuest = "";
	if(iFame > iRand)
	{
		sQuest = "FailSneakToColony_" + colonies[iColony].id;
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sLoc;
		pchar.quest.(sQuest).win_condition = "FailSneakToColony";
		pchar.quest.(sQuest).function = "SneakFailed";
	}
	else
	{
		sQuest = "SuccessSneakToColony_" + colonies[iColony].id;
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = sLoc;
		pchar.quest.(sQuest).win_condition = "SuccessSneakToColony";
		pchar.quest.(sQuest).function = "SneakSuccesed";
	}
}

void SneakSuccesed(string sQuest)
{
	string sColony = pchar.location;
	sColony = Locations[FindLocation(sColony)].fastreload;
	
	int iColony = FindColony(sColony);
	int iNation = sti(colonies[iColony].nation);

	if(iNation != PIRATE && GetNationRelation2MainCharacter(iNation) == RELATION_ENEMY)
	{
	
		string sNation = GetNationNameByType(iNation);
		string sSoldiersGroup = sNation + "_CITIZENS";
		string sCitizensGroup = sNation + "_CITIZENS";

		LAi_group_SetRelation(sSoldiersGroup, LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
		LAi_group_SetRelation(sCitizensGroup, LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);

		pchar.sneak.success = 1;

		//LAi_group_SetAlarm(LAI_GROUP_PLAYER, sSoldiersGroup, 10.0);
		//LAi_group_SetAlarm(LAI_GROUP_PLAYER, sCitizensGroup, 10.0);
	}
}

void SneakFailed(string sQuest)
{
	string sColony = pchar.location;
	sColony = Locations[FindLocation(sColony)].fastreload;
	
	int iColony = FindColony(sColony);
	int iNation = sti(colonies[iColony].nation);

	if(iNation != PIRATE && GetNationRelation2MainCharacter(iNation) == RELATION_ENEMY)
	{
	
		string sNation = GetNationNameByType(iNation);
		string sSoldiersGroup = sNation + "_CITIZENS";
		string sCitizensGroup = sNation + "_CITIZENS";

		LAi_group_SetRelation(sSoldiersGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
		LAi_group_SetRelation(sCitizensGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);

		pchar.sneak.success = 0;

		LAi_group_SetAlarm(LAI_GROUP_PLAYER, sSoldiersGroup, 10.0);
		LAi_group_SetAlarm(LAI_GROUP_PLAYER, sCitizensGroup, 10.0);
	}

}
*/
