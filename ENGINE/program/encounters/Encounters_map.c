#define ENCOUNTER_GROUP		"egroup__"

int FindFreeMapEncounterSlot()
{
	for (int i=0;i<MAX_MAP_ENCOUNTERS;i++) 
	{
		if (sti(MapEncounters[i].bUse) == false) 
		{
			return i;
		}

		if(i > 0)
		{
			if (CheckAttribute(&MapEncounters[i], "groupname") && CheckAttribute(&MapEncounters[i-1], "groupname"))
			{
				if(MapEncounters[i].realencountertype == MapEncounters[i - 1].realencountertype &&
					MapEncounters[i].groupname == MapEncounters[i - 1].groupname &&
					MapEncounters[i].characterid == MapEncounters[i - 1].characterid &&
					MapEncounters[i].nation == MapEncounters[i - 1].nation)
				{
					MapEncounters[i].bUse = false;
					return i;
				}
			}
		}

	}
	//Trace("FindFreeMapEncounterSlot: Can't find free slot!!!");
	return -1;
}

void ManualReleaseMapEncounter(int iEncounterSlot)
{
	DeleteAttribute(&MapEncounters[iEncounterSlot], "");
	MapEncounters[iEncounterSlot].bUse = false;
}

void ReleaseMapEncounters()
{
	for (int i=0;i<MAX_MAP_ENCOUNTERS;i++) 
	{
		DeleteAttribute(&MapEncounters[i],"");
		MapEncounters[i].bUse = false;
	}
}

void ReleaseMapEncounter(int iEncounterSlot)
{
	//Trace("Release encounter with slot " + iEncounterSlot);
	if (sti(MapEncounters[iEncounterSlot].bUse) == true)
	{
		DeleteAttribute(&MapEncounters[iEncounterSlot],"");
		MapEncounters[iEncounterSlot].bUse = false;
		return;
	}
	//Trace("ReleaseMapEncounter: ERROR: Encounter not used idx = " + iEncounterSlot);
}

ref GetMapEncounterRef(int iEncounterSlot)
{
	return &MapEncounters[iEncounterSlot];
}

ref GetMapEncounterNationRef(int iEncounterSlot)
{
	if (sti(MapEncounters[iEncounterSlot].bUse) == true)
	{
		return GetNationByType(sti(MapEncounters[iEncounterSlot].Nation));
	}
	//Trace("GetMapEncounterNationString: error: not found use slot iEncounterSlot = " + iEncounterSlot);
	return GetNationByType(ENGLAND);
}

bool GenerateMapEncounter(int iMapEncounterType, string sIslandID, ref iEncounter1, ref iEncounter2)
{
	iEncounter1 = -1;
	iEncounter2 = -1;

	bool bReturn = false;
	int iNearIslandNation = PIRATE;

	if (sIslandID != "" && !Island_IsEncountersEnable(sIslandID)) {	return false; }

	switch(iMapEncounterType)
	{
		case WDM_ETYPE_MERCHANT:		// merchant ship
			bReturn = GenerateMapEncounter_Merchant(sIslandID, iEncounter1);
		break;
		case WDM_ETYPE_FOLLOW:			// war ship
			bReturn = GenerateMapEncounter_War(sIslandID, iEncounter1, GetMainCharacterIndex());
		break;
		case WDM_ETYPE_WARRING:			// 2 war (or 1 war and 1 trade, or 2 trade) ships in battle
			bReturn = GenerateMapEncounter_Battle(sIslandID, iEncounter1, iEncounter2);
		break;
	}

	if (!bReturn) 
	{
		return false;
	}

	ref rEncounter1, rEncounter2;

	if (iEncounter1 != -1)
	{
		rEncounter1 = &MapEncounters[iEncounter1];
		rEncounter1.GroupName = ENCOUNTER_GROUP + iEncounter1;
		if(sti(rEncounter1.nation) == PIRATE)
		{
			if(sti(rEncounter1.RealEncounterType) < ENCOUNTER_TYPE_PIRATE_SMALL || sti(rEncounter1.RealEncounterType) > ENCOUNTER_TYPE_PIRATE_LARGE)
			{
				iEncounter1 = -1;
				iEncounter2 = -1;
				return false;
			}
		}
	}
	if (iEncounter2 != -1)
	{
		rEncounter2 = &MapEncounters[iEncounter2];
		rEncounter2.GroupName = ENCOUNTER_GROUP + iEncounter2;
		if(sti(rEncounter2.nation) == PIRATE)
		{
			if(sti(rEncounter2.RealEncounterType) < ENCOUNTER_TYPE_PIRATE_SMALL || sti(rEncounter2.RealEncounterType) > ENCOUNTER_TYPE_PIRATE_LARGE)
			{
				iEncounter1 = -1;
				iEncounter2 = -1;
				return false;
			}
		}
	}


	if(iEncounter1 != -1 && iEncounter2 != -1)
	{
		if(GetNationRelation(sti(rEncounter1.nation), sti(rEncounter2.nation)) != RELATION_ENEMY)
		{
			iEncounter1 = -1;
			iEncounter2 = -1;
			return false;
		}
	}

	int i1 = iEncounter1;
	int i2 = iEncounter2;

	//Trace("Create encounter with slot " + iEncounter1 + ", Real = " + i1);
	//Trace("Create encounter with slot " + iEncounter2 + ", Real = " + i2);

	return true;
}

bool GenerateMapEncounter_SetMapShipModel(ref rEncounter)
{
	string sLastName;
	if(!CheckAttribute(rEncounter, "Nation"))
	{
		trace("У ЭНКАУНТЕРА НЕТ НАЦИИИ!!!");
		return false;
	}
	string sFirstName = Nations[sti(rEncounter.Nation)].worldMapShip;

	if (sti(rEncounter.RealEncounterType) == ENCOUNTER_TYPE_ALONE)
	{
		if(!CheckAttribute(rEncounter, "CharacterID"))
		{
			trace("Квестовому энкаунтеру не назначен персонаж!");
			return false;
		}
		int iCharIndex = GetCharacterIndex(rEncounter.CharacterID);
		if (iCharIndex < 0) 
		{ 
			trace("Квестовому энкаунтеру назначен персонаж, но его нет!");
			return false; 
		}

		ref rCharacter = GetCharacter(iCharIndex);
		if (CheckAttribute(rCharacter, "MapEnc.worldMapShip")) // boal fix
		{
			rEncounter.worldMapShip = rCharacter.MapEnc.worldMapShip;
		}
		else
		{
		    rEncounter.worldMapShip = sFirstName + "_bark";  // boal new
		}
	}
	else
	{
		sLastName = EncountersTypes[sti(rEncounter.RealEncounterType)].worldMapShip;
		rEncounter.worldMapShip = sFirstName + "_" + sLastName;
		//Trace(" rEncounter.worldMapShip = " + rEncounter.worldMapShip);
	}

	return true;
}

bool GenerateMapEncounter_WriteNumShips(ref rEncounter, int iEncounterType, int iMaxShipNum)
{
	aref	aWar,aMerc;
	ref		rEnc;

	makeref(rEnc,EncountersTypes[iEncounterType]);
	makearef(aWar,rEnc.War);
	makearef(aMerc,rEnc.Merchant);

	int iNumMerchantShips, iNumWarShips;

	iNumMerchantShips = sti(aMerc.ShipsMin) + rand(sti(aMerc.ShipsMax) - sti(aMerc.ShipsMin));
	iNumWarShips = sti(aWar.ShipsMin) + rand(sti(aWar.ShipsMax) - sti(aWar.ShipsMin));

	int iTotalShips = iNumMerchantShips + iNumWarShips;
	if (iTotalShips == 0) 
	{
		//Trace("GenerateMapEncounter_WriteNumShips: iTotalShips = 0, iEncounterType = " + iEncounterType);
		return false;
	}

	while (iTotalShips > iMaxShipNum)
	{
		if (iNumWarShips) 
		{ 
			iNumWarShips--; 
			iTotalShips--; 
		}
		if (iTotalShips <= iMaxShipNum) 
		{
			break;
		}

		if (iNumMerchantShips) 
		{ 
			iNumMerchantShips--; 
			iTotalShips--; 
		}
	}

	if(iNumMerchantShips > 0)
	{
		rEncounter.NumMerchantShips = iNumMerchantShips;
	}
	else
	{
		DeleteAttribute(rEncounter, "NumMerchantShips");
	}
	if(iNumWarShips > 0)
	{
		rEncounter.NumWarShips = iNumWarShips;
	}
	else
	{
		DeleteAttribute(rEncounter, "NumWarShips");
	}

	return true;
}

bool GenerateMapEncounter_Merchant(string sIslandID, ref iEncounter)
{
	// find free slot in dynamic encounter table for map
	int iEncounterSlot = FindFreeMapEncounterSlot();
	if (iEncounterSlot == -1) return false;
	ManualReleaseMapEncounter(iEncounterSlot);

	ref rEncounter = &MapEncounters[iEncounterSlot];
	
	// find real encounter
	int iEncounterType = FindMerchantEncounter();
	if (iEncounterType == -1) return false;
	rEncounter.RealEncounterType = iEncounterType;

	GenerateMapEncounter_WriteNumShips(rEncounter, iEncounterType, 8); //boal

	// nation find
	int iNation = GetRandomNationForMapEncounter(sIslandID, true);
	if (iNation < 0) 
	{ 
		return false; 
	}
	rEncounter.nation = iNation;
	//trace("iNation is " + iNation);

	iEncounter = iEncounterSlot;
	rEncounter.bUse = true;
	rEncounter.Type = "trade";

	rEncounter.Task = AITASK_MOVE;
	// create move point coordinates here

	return GenerateMapEncounter_SetMapShipModel(rEncounter);

	//return true;
}

bool GenerateMapEncounter_War(string sIslandID, ref iEncounter, int iCharacterIndexAsEnemy)
{
	int iEncounterType = -1;

	// find free slot in dynamic encounter table for map
	int iEncounterSlot = FindFreeMapEncounterSlot();
	if (iEncounterSlot == -1) return false;
	ManualReleaseMapEncounter(iEncounterSlot);

	ref rEncounter = &MapEncounters[iEncounterSlot];

	// try to find quest encounters
	if (iCharacterIndexAsEnemy != -1)
	{
		// try to find Quest encounter
		int iQEncounter = Encounter_FindFirstQuestMapEncounter(ENCOUNTER_WAR);
		if (iQEncounter >= 0)
		{
			ref rQEncounter = Encounter_GetQuestMapEncounter(iQEncounter);
			if (rand(99) < sti(rQEncounter.Rand))
			{
				string sGroupID = rQEncounter.ID;
				if (!sti(rQEncounter.Permanent)) { Encounter_DeleteQuestMapEncounter(sGroupID);	}
				ref rGCommander = Group_GetGroupCommander(sGroupID);
				
				rEncounter.Nation = sti(rGCommander.Nation);
				rEncounter.bUse = true;
				rEncounter.qID = sGroupID;
				rEncounter.GroupName = sGroupID;
				rEncounter.GeraldSails = true;

				iEncounter = iEncounterSlot;
				return true;
			}
		}
	}

	int iNation = -1;
	

	// find real encounter if not punitive
	if (iEncounterType == -1)
	{
		iEncounterType = FindWarEncounter(); if (iEncounterType == -1) return false;
	}

	rEncounter.RealEncounterType = iEncounterType;

	GenerateMapEncounter_WriteNumShips(rEncounter, iEncounterType, 8); //boal

	// nation find
	if (iNation == -1)
	{
		iNation = GetRandomNationForMapEncounter(sIslandID, false);
	}

	if (iNation < 0) 
	{ 
		return false; 
	}

	//trace("iNation is " + iNation);

	if(iEncounterType > 11 && iEncounterType < 15)
	{
		iNation = PIRATE;
	}

	rEncounter.Nation = iNation;

	if(GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY)
	{
		rEncounter.Task = AITASK_MOVE;
		DeleteAttribute(rEncounter, "Task.Target");
		rEncounter.Type = "trade";
	}
	else
	{
		rEncounter.Task = AITASK_ATTACK;
		rEncounter.Task.Target = PLAYER_GROUP; 
		rEncounter.Type = "war";
	}
	iEncounter = iEncounterSlot;

	if (sti(rEncounter.Nation) == PIRATE) 
	{
		rEncounter.Type = "pirate";
	}

	rEncounter.bUse = true;

	GenerateMapEncounter_SetMapShipModel(rEncounter);

	return true;
}

bool GenerateMapEncounter_Alone(string sCharacterID, ref iEncounterIndex)
{
	iEncounterIndex = -1;
	int iCharIndex = GetCharacterIndex(sCharacterID);
	if (iCharIndex < 1) { return false; }  //fix

	ref rCharacter = GetCharacter(iCharIndex);

	int iEncounterSlot = FindFreeMapEncounterSlot();
	if (iEncounterSlot == -1) return false;
	ManualReleaseMapEncounter(iEncounterSlot);

	ref rEncounter = &MapEncounters[iEncounterSlot];

	rEncounter.RealEncounterType = ENCOUNTER_TYPE_ALONE;

	rEncounter.GroupName = sCharacterID;
	rEncounter.CharacterID = sCharacterID;
	rEncounter.Nation = rCharacter.Nation;

	iEncounterIndex = iEncounterSlot;
	
	rEncounter.bUse = true;

	if(!CheckAttribute(rCharacter, "MapEnc.type"))
	{
		trace ("Character "+ rCharacter.id + " have no MapEnc.type field");
		return false;
	}
	else
	{
		if (rCharacter.MapEnc.Type == "war")
		{
			rEncounter.NumWarShips = 1;
			rEncounter.NumMerchantShips = 0;
			rEncounter.Type = "war";
			rEncounter.Task = AITASK_ATTACK;
			rEncounter.Task.Target = PLAYER_GROUP;
		}
		else
		{
			rEncounter.NumMerchantShips = 1;
			rEncounter.NumWarShips = 0;
			rEncounter.Type = "trade";
			rEncounter.Task = AITASK_MOVE;
			DeleteAttribute(rEncounter, "Task.Target");
		}
	}

	GenerateMapEncounter_SetMapShipModel(rEncounter);

	// create move point coordinates here

	return true;
	
}

bool GenerateMapEncounter_Battle(string sIslandID, ref iEncounter1, ref iEncounter2)
{
	//check for trade opposition
	int iIsTrade = rand(3);
	
	// generate first encounter
	if (iIsTrade != 0)
	{
		if (!GenerateMapEncounter_War(sIslandID, iEncounter1, -1))
		{
			iEncounter1 = -1; iEncounter2 = -1;
			return false;
		}
	}
	else
	{
		if (!GenerateMapEncounter_Merchant(sIslandID, iEncounter1))
		{
			iEncounter1 = -1; iEncounter2 = -1;
			return false;
		}

		float fAngle = frnd() * PIm2;
		MapEncounters[iEncounter1].Task.Pos.x = 10000.0 * sin(fAngle);
		MapEncounters[iEncounter1].Task.Pos.z = 10000.0 * cos(fAngle);
	}

	// generate second encounter
	if (!GenerateMapEncounter_War(-1, iEncounter2, -1))
	{
		ManualReleaseMapEncounter(iEncounter1);
		iEncounter1 = -1; iEncounter2 = -1;
		return false;
	}

	ref rEncounter1 = &MapEncounters[iEncounter1];
	ref rEncounter2 = &MapEncounters[iEncounter2];

	int iRealEncounterType1 = rEncounter1.RealEncounterType;
	int iRealEncounterType2 = rEncounter2.RealEncounterType;

	// find nations for battle between two encounters

	int iNationsCanBe[MAX_NATIONS]; 
	int iNumNationsCanBe = 0;

	int iNation1 = sti(rEncounter1.Nation);
	int iNation2 = sti(rEncounter2.Nation);
	
	// if we hit the target with nations - return
	if (GetNationRelation(iNation1, iNation2) != RELATION_ENEMY)
	{
		// search enemy for iNation1
		for (int i=0; i<MAX_NATIONS; i++)
		{
			bool bCanBe = Encounter_CanNation(iRealEncounterType2, i);
			if (!bCanBe) continue;
			if (GetNationRelation(i, iNation1) != RELATION_ENEMY) continue;
			iNationsCanBe[iNumNationsCanBe] = i;
			iNumNationsCanBe++;
		}

		if (iNumNationsCanBe == 0) 
		{
			ManualReleaseMapEncounter(iEncounter1);
			ManualReleaseMapEncounter(iEncounter2);
			iEncounter1 = -1; iEncounter2 = -1;
			return false;
		}

		rEncounter2.Nation = iNationsCanBe[rand(iNumNationsCanBe - 1)];
	}

	if(iRealEncounterType1 > 12 && iRealEncounterType1 < 15)
	{
		rEncounter1.Nation = PIRATE;
	}
	if(iRealEncounterType2 > 12 && iRealEncounterType2 < 15)
	{
		rEncounter2.Nation = PIRATE;
	}

	rEncounter1.Task = AITASK_ATTACK;	rEncounter1.Task.Target = ENCOUNTER_GROUP + iEncounter2;
	rEncounter2.Task = AITASK_ATTACK;	rEncounter2.Task.Target = ENCOUNTER_GROUP + iEncounter1;

	rEncounter1.Lock = true;	rEncounter2.Lock = true;
	rEncounter1.bUse = true;	rEncounter2.bUse = true;

	GenerateMapEncounter_SetMapShipModel(rEncounter1);
	GenerateMapEncounter_SetMapShipModel(rEncounter2);

	return true;
}