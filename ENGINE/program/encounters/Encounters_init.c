void Enc_ExcludeNation(ref rEnc, int iNation)
{
	string sNation = Nations[iNation].Name;
	rEnc.Nation.Exclude.(sNation) = true;
}

void Enc_AddShips(ref rEnc, string sEType, int iShipsMin, int iShipsMax)
{
	rEnc.(sEType).ShipsMin = iShipsMin;		
	rEnc.(sEType).ShipsMax = iShipsMax;
}

void Enc_AddClasses(ref rEnc, int iRank, int iMClassMin, int iMClassMax, int iWClassMin, int iWClassMax)
{
	string sRank = "Rank." + iRank;
	rEnc.(sRank).0 = iMClassMin; rEnc.(sRank).1 = iMClassMax;
	rEnc.(sRank).2 = iWClassMin; rEnc.(sRank).3 = iWClassMax;
}

void InitEncounters()
{
	int i;
	ref rEnc;

	for (i=0; i<MAX_ENCOUNTER_TYPES; i++)
	{
		makeref(rEnc, EncountersTypes[i]);
		rEnc.Index = i;
		rEnc.Chance = 100;
		rEnc.Skip = false;
		rEnc.MinRank = 1;
		rEnc.MaxRank = 1000;
		rEnc.Merchant.ShipsMin = 0;		rEnc.Merchant.ShipsMax = 0;
		rEnc.War.ShipsMin = 0;			rEnc.War.ShipsMax = 0;
		rEnc.Type = ENCOUNTER_TRADE;
	}
	////  boal баг фикс для квестовых, а то генерились в К3
    makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_ALONE]);
 	rEnc.Chance = 0;
	rEnc.Skip   = true;
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 1-2 торговца 5-6 класса.
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_SMALL]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "sloop";
	Enc_AddShips(rEnc, "Merchant", 1, 2);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 4, 7, 0, 0);
	Enc_AddClasses(rEnc, 1000,4, 7, 0, 0);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-4 торговца 5-6 класса.
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_MEDIUM]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "bark";
	Enc_AddShips(rEnc, "Merchant", 2, 4);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 4, 6, 0, 0);
	Enc_AddClasses(rEnc, 1000,4, 6, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-5 торговца 4-6 класса.
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_LARGE]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "galleon";
	Enc_AddShips(rEnc, "Merchant", 3, 5);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 3, 6, 0, 0);
	Enc_AddClasses(rEnc, 1000,3, 6, 0, 0);
		
	///////////////////////////////////////////////////////////////////////////////////////
	/// 1-3 торговца 5-6 класса плюс 1 корабль охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_GUARD_SMALL]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "sloop";
	Enc_AddShips(rEnc, "Merchant", 1, 3);
	Enc_AddShips(rEnc, "War", 1, 1);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 4, 6, 4, 5);
	Enc_AddClasses(rEnc, 1000,4, 6, 4, 5);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-3 торговца 5-6 класса плюс 1-2 корабля охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_GUARD_MEDIUM]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "bark";
	Enc_AddShips(rEnc, "Merchant", 2, 3);
	Enc_AddShips(rEnc, "War", 1, 2);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 3, 6, 2, 5);
	Enc_AddClasses(rEnc, 1000,3, 6, 2, 5);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-5 торговца 4-6 класса плюс 1-2 корабля охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_MERCHANT_GUARD_LARGE]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 5;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "galleon";
	Enc_AddShips(rEnc, "Merchant", 3, 5);
	Enc_AddShips(rEnc, "War", 1, 2);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 2, 6, 1, 5);
	Enc_AddClasses(rEnc, 1000,2, 6, 1, 5);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-3 торговца 3-4 класса плюс 1-2 корабля охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_ESCORT_SMALL]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "bark";
	Enc_AddShips(rEnc, "Merchant", 2, 3);
	Enc_AddShips(rEnc, "War", 1, 2);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 2, 3, 4, 3, 4);
	Enc_AddClasses(rEnc, 1000,3, 4, 3, 4);
	
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-4 торговца 3-4 класса плюс 2-3 корабля охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_ESCORT_MEDIUM]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 3;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "galleon";
	Enc_AddShips(rEnc, "Merchant", 2, 4);
	Enc_AddShips(rEnc, "War", 2, 3);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 3, 3, 4, 2, 4);
	Enc_AddClasses(rEnc, 1000,3, 4, 2, 4);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-5 торговца 3-4 класса плюс 2-4 корабля охраны
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_ESCORT_LARGE]);
	rEnc.Type = ENCOUNTER_TRADE;
	rEnc.MinRank = 4;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "manowar";
	Enc_AddShips(rEnc, "Merchant", 3, 5);
	Enc_AddShips(rEnc, "War", 2, 4);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 4, 3, 4, 1, 3);
	Enc_AddClasses(rEnc, 1000,3, 4, 1, 3);

	// ======================== WAR ENCOUNTERS ========================
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 1-2 военных корабля 4-6 класса
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PATROL_SMALL]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "bark";
	Enc_AddShips(rEnc, "War", 1, 3);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 1, 0, 0, 3, 5);
	Enc_AddClasses(rEnc, 1000,0, 0, 3, 5);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-4 военных корабля 3-5 класса
	///////////////////////////////////////////////////////////////////////////////////////

	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PATROL_MEDIUM]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 5;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "frigate";
	Enc_AddShips(rEnc, "War", 2, 4);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 2, 0, 0, 2, 5);
	Enc_AddClasses(rEnc, 1000,0, 0, 2, 5);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-5 военных корабля 3-4 класса
	/////////////////////////////////////////////////////////////////////////////////////// 
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PATROL_LARGE]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 7;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "frigate";
	Enc_AddShips(rEnc, "War", 3, 5);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 3, 0, 0, 2, 4);
	Enc_AddClasses(rEnc, 1000,0, 0, 2, 4);
		
	///////////////////////////////////////////////////////////////////////////////////////
	/// 1-2 пирата
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PIRATE_SMALL]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 1;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "sloop";
	Enc_AddShips(rEnc, "War", 1, 2);
	Enc_ExcludeNation(rEnc, ENGLAND);
	Enc_ExcludeNation(rEnc, FRANCE);
	Enc_ExcludeNation(rEnc, SPAIN);
	Enc_ExcludeNation(rEnc, HOLLAND);

	Enc_AddClasses(rEnc, 1, 0, 0, 5, 7);
	Enc_AddClasses(rEnc, 1000,0, 0, 5, 7);

	///////////////////////////////////////////////////////////////////////////////////////
	/// 1-4 пирата
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PIRATE_MEDIUM]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 3;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "sloop";
	Enc_AddShips(rEnc, "War", 1, 4);
	Enc_ExcludeNation(rEnc, ENGLAND);
	Enc_ExcludeNation(rEnc, FRANCE);
	Enc_ExcludeNation(rEnc, SPAIN);
	Enc_ExcludeNation(rEnc, HOLLAND);

	Enc_AddClasses(rEnc, 2, 0, 0, 3, 6);
	Enc_AddClasses(rEnc, 1000,0, 0, 3, 6);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-6 пирата
	///////////////////////////////////////////////////////////////////////////////////////
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PIRATE_LARGE]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 4;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "bark";
	Enc_AddShips(rEnc, "War", 2, 6);
	Enc_ExcludeNation(rEnc, ENGLAND);
	Enc_ExcludeNation(rEnc, FRANCE);
	Enc_ExcludeNation(rEnc, SPAIN);
	Enc_ExcludeNation(rEnc, HOLLAND);

	Enc_AddClasses(rEnc, 3, 0, 0, 2, 6);
	Enc_AddClasses(rEnc, 1000,0, 0, 2, 6);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-3 военных корабля 3-4 класса
	/////////////////////////////////////////////////////////////////////////////////////// 
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_SQUADRON]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 4;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "frigate";
	Enc_AddShips(rEnc, "War", 2, 3);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 4, 0, 0, 2, 4);
	Enc_AddClasses(rEnc, 1000,0, 0, 2, 4);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-5 военных корабля 2-4 класса
	/////////////////////////////////////////////////////////////////////////////////////// 
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_FLEET]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 6;
	rEnc.MaxRank = 1000;
	rEnc.worldMapShip = "manowar";
	Enc_AddShips(rEnc, "War", 3, 5);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 5, 0, 0, 1, 4);
	Enc_AddClasses(rEnc, 1000,0, 0, 1, 4);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 3-7 военных корабля 1-3 класса
	/////////////////////////////////////////////////////////////////////////////////////// 
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_ARMADA]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 9;
	rEnc.MaxRank = 1000;
	rEnc.Chance = 30;
	rEnc.worldMapShip = "manowar";
	Enc_AddShips(rEnc, "War", 3, 7);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 6, 0, 0, 1, 3);
	Enc_AddClasses(rEnc, 1000,0, 0, 1, 3);
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 2-4 военных корабля 1-3 класса
	/////////////////////////////////////////////////////////////////////////////////////// 
	makeref(rEnc, EncountersTypes[ENCOUNTER_TYPE_PUNITIVE_SQUADRON]);
	rEnc.Type = ENCOUNTER_WAR;
	rEnc.MinRank = 7;
	rEnc.MaxRank = 1000;
	rEnc.Chance = 30;
	rEnc.worldMapShip = "manowar";
	Enc_AddShips(rEnc, "War", 2, 4);
	Enc_ExcludeNation(rEnc, PIRATE);

	Enc_AddClasses(rEnc, 6, 0, 0, 1, 3);
	Enc_AddClasses(rEnc, 1000,0, 0, 1, 3);	
		
	Trace("Init encounters complete.");
}