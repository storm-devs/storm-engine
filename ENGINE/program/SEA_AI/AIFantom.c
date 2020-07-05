
#define INVALID_SHIP_TYPE			-1

ref Fantom_GetNextFantom()
{
	iNumFantoms++;
	return &Characters[FANTOM_CHARACTERS + iNumFantoms];
}

int Fantom_GenerateEncounter(string sGroupName, object oResult, int iEType, int iNumWarShips, int iNumMerchantShips) 
{
	aref	aWar, aMerchant;
	ref		rEnc;
	int		i;
	int		iWarClassMax, iWarClassMin, iMerchantClassMax, iMerchantClassMin;

	makeref(rEnc, EncountersTypes[iEType]);
	makearef(aWar, rEnc.War);
	makearef(aMerchant, rEnc.Merchant);

	//iNumMerchantShips = MakeInt(aMerchant.ShipsMin) + rand(MakeInt(aMerchant.ShipsMax) - MakeInt(aMerchant.ShipsMin));
	//iNumWarShips = MakeInt(aWar.ShipsMin) + rand(MakeInt(aWar.ShipsMax) - MakeInt(aWar.ShipsMin));
	
	int iRank = sti(pchar.Rank);
	Encounter_GetClassesFromRank(iEType, iRank, &iMerchantClassMin, &iMerchantClassMax, &iWarClassMin, &iWarClassMax);
	
	int iFantomIndex, iShipType;

	for (i=0; i<iNumMerchantShips; i++)
	{
		iShipType = Fantom_GetShipType(iMerchantClassMin, iMerchantClassMax, "Merchant");
		if (iShipType == INVALID_SHIP_TYPE) continue;
		Fantom_AddFantomCharacter(sGroupName, iShipType, "Trade", iEType);
	}

	for (i=0; i<iNumWarShips; i++)
	{
		iShipType = Fantom_GetShipType(iWarClassMin, iWarClassMax, "War");
		if (iShipType == INVALID_SHIP_TYPE) continue;
		Fantom_AddFantomCharacter(sGroupName, iShipType, "War", iEType);
	}

	return iNumWarShips + iNumMerchantShips;
}

int Fantom_GetShipType(int iClassMin, int iClassMax, string sShipType)
{
	int iShips[50];
	int i, iShipsNum;
	iShipsNum = 0;

	for (i=SHIP_TARTANE; i<=SHIP_MANOWAR; i++)  //энкаунтеры только до мановара, квестовые корабли отдельно
	{
		object rShip = GetShipByType(i);
		if (!checkAttribute(rship, "class"))
		{
			trace ("bad ship is: " + rship.name);
		}
		int iClass = MakeInt(rShip.Class);
		
		if (iClass > iClassMin) { continue; }
		if (iClass < iClassMax) { continue; }
		if (sti(rShip.CanEncounter) != true) { continue; }
		if (sti(rShip.Type.(sShipType)) != true) { continue; }

		iShips[iShipsNum] = i;
		iShipsNum++;
	}
	if (iShipsNum==0) 
	{
		Trace("Can't find ship type '" + sShipType + "' with ClassMin = " + iClassMin + " and ClassMax = " + iClassMax);
		return INVALID_SHIP_TYPE;
	}

	int iBaseShipType = iShips[rand(iShipsNum - 1)];
	//trace(ShipsTypes[iBaseShipType].name);
	int iRealShipType = GenerateShip(iBaseShipType, 0);
	return iRealShipType;
}

// return new added fantom character
// мктод этот походу левый, тк перекрываетс€ в сиа.с
void Fantom_AddFantomCharacter(string sGroupName, int iShipType, string sFantomType, int iEncounterType)
{
	ref rFantom = GetFantomCharacter(iNumFantoms);



	DeleteAttribute(rFantom, "relation");
	DeleteAttribute(rFantom, "abordage_twice");
	DeleteAttribute(rFantom, "QuestDate");
	DeleteAttribute(rFantom, "ransom");

	/*rFantom.fame = rand(70);

	
	int iNation = sti(rFantom.nation);
	//trace("Nation of Fantom is " + GetNationNameByType(iNation));
	iNation = FindEnemyNation2NationWithoutPirates(iNation);
	if(iNation != -1)
	{
		string sNation = GetNationNameByType(iNation);
		
		rFantom.reputation.(sNation) = 10;
		
		//trace("Nation of Enemy of Fantom is " + sNation);
		rFantom.bounty.(sNation).money = sti(rFantom.fame)*rand(500);
	}	
	*/
	rFantom.SeaAI.Group.Name = sGroupName;
	rFantom.Ship.Type = iShipType;
	rFantom.Ship.Mode = sFantomType;
	rFantom.Charge.Type = GOOD_BALLS;
	
	iNumFantoms++;
}
// на деле этот метод бесполезен, тк золото в сундуке генеритс€ в др месте, а то что, в к3 тут были распределени€ опыта и команды вообще позорище.
void Fantom_SetRandomMoney(ref rFantom, string sFantomType)
{
	// clear money from character
	rFantom.Money = 0;

	//GenerateBootyItems(rFantom); // to_do del
	if (!CheckAttribute(rFantom, "ship.type")) return; // fix
	
    int iShip = sti(rFantom.ship.type);
	if (iShip == SHIP_NOTUSED) return; // fix
	
	// clear money from character
	rFantom.Money = 0;

	// ship class
	int iSClass = GetCharacterShipClass(rFantom);

	// add money
// boal 20.01.2004 -->
	switch (sFantomType)
	{
		case "trade":
			rFantom.Money = (10 - iSClass) * 100 + rand((10 - iSClass) * 800);
			return;
		break;
		case "war":
			rFantom.Money = (10 - iSClass) * 40 + rand((10 - iSClass) * 350);
			return;
		break;
		case "pirate":
		    rFantom.Money = (10 - iSClass) * 80 + rand((10 - iSClass) * 500);
			return;
		break;
	}
// boal 20.01.2004 <--
	//Trace("Fantom index = " + rFantom.index + ", group id = " + rFantom.SeaAI.Group.Name + ", have invalid encounter type = " + sFantomType);
}

// boal перенес из ¬ћЋ
int Fantom_CalcSkill(ref rMainCharacter, string sSkill, int iShipClass, int iSkillMin, int iSkillMax, int iSClass7, int iSClass6, int iSClass5, int iSClass4, int iSClass3, int iSClass2, int iSClass1)
// Stone-D : It was all backwards!
{
	int iSkill = GetSummonSkillFromName(rMainCharacter, sSkill);
    // boal 15.03.04 -->
    int minSkillLevel = 0;
	iSkill = iSkill + iSkillMin + rand(iSkillMax - iSkillMin);
	switch (iShipClass)
	{
		case 1: minSkillLevel =  iSClass1; break;
		case 2: minSkillLevel =  iSClass2; break;
		case 3: minSkillLevel =  iSClass3; break;
		case 4: minSkillLevel =  iSClass4; break;
		case 5: minSkillLevel =  iSClass5; break;
		case 6: minSkillLevel =  iSClass6; break;
		case 7: minSkillLevel =  iSClass7; break;
	}
	minSkillLevel = minSkillLevel*10; // приведение скила к 1-100
	
    if (iSkill < minSkillLevel)	iSkill = minSkillLevel;
    // boal 15.03.04 <--
	if (iSkill < 1)		    	iSkill = 1;
	if (iSkill > SKILL_MAX)     iSkill = SKILL_MAX;
	return iSkill;
}

void Fantom_SetCannons(ref rFantom, string sFantomType)
{
	int iSClass = GetCharacterShipClass(rFantom);
	ref rShip = GetRealShip(GetCharacterShipType(rFantom));

 	int iCannonsType = sti(rShip.Cannon);
	string sCannonType = "cannon";
	int iCaliber = sti(rShip.MaxCaliber);
    // boal 03.02.2004 -->
    switch(iCaliber)
	{
		case 12:
			iCaliber = 0;
		break;
		case 16:
			iCaliber = 1;
		break;
		case 24:
			iCaliber = 2;
		break;
		case 32:
			iCaliber = 4;
		break;
		case 42:
			iCaliber = 5;
		break;
	}
	if (iCaliber > 0)
	{
	   iCaliber = iCaliber - rand(1);
	}
	switch(iCaliber)
	{
        case 0:
			iCaliber = 12;
		break;
		case 1:
			iCaliber = 16;
		break;
		case 2:
			iCaliber = 24;
		break;
		case 3:
			iCaliber = 32;
		break;
		case 4:
			iCaliber = 42;
		break;
	}
	if (iCaliber < 12)
	{
	   iCaliber = 12;
	}
    // boal 03.02.2004 <--
	if (iCaliber > sti(rShip.MaxCaliber)) { iCaliber=sti(rShip.MaxCaliber); }

	switch (sFantomType)
	{
        case "trade":
            // boal 20.01.2004 -->
		    if (rand(1000) < 700)
		    {
			   sCannonType = "cannon";
			}
			else
			{
			   sCannonType = "culverine";
			}
		break;

		case "war":
			if (rand(1000) < 200)
		    {
			   sCannonType = "cannon";
			}
			else
			{
			   sCannonType = "culverine";
			}
		    break;

		case "pirate":
			if (rand(1000) < 400)
		    {
			   sCannonType = "cannon";
			}
			else
			{
			   sCannonType = "culverine";
			}
		    break;
		    // boal 20.01.2004 <--
	}

	if (sti(rShip.Cannon) == CANNON_TYPE_NONECANNON)
	{
		rFantom.Ship.Cannons.Type = CANNON_TYPE_NONECANNON;
		return;
	}
	if (iCaliber == 42)
	{
	    sCannonType = "cannon";
	}
	rFantom.Ship.Cannons.Type = GetCannonByTypeAndCaliber(sCannonType, iCaliber);
}

void Fantom_SetSails(ref rFantom, string sFantomType)
{
	rFantom.Ship.Sails.Gerald = false;
	if (sFantomType == "war") rFantom.Ship.Sails.Gerald = true;
}

void Fantom_SetBalls(ref rFantom, string sFantomType)
{
	float fKBalls = 7 - GetCharacterShipClass(rFantom);

	if (fKBalls <= 0) fKBalls = 0.7; // баркас
	switch (sFantomType)
	{
		case "war":
			fKBalls = fKBalls * 1.5;
		break;
		case "trade":
			fKBalls = fKBalls * 0.7;
		break;
		case "pirate":
			fKBalls = fKBalls * 1.0;
		break;
	}
	if (GetCharacterShipClass(rFantom) == 1) fKBalls = fKBalls * 1.6; //много пушек, большой расход
    // boal 20.01.2004 -->
	Fantom_SetCharacterGoods(rFantom, GOOD_BALLS,    MakeInt(195 * fKBalls + rand(MakeInt(10 * fKBalls))));
	Fantom_SetCharacterGoods(rFantom, GOOD_BOMBS,    MakeInt(180 * fKBalls + rand(MakeInt(20 * fKBalls))));
	Fantom_SetCharacterGoods(rFantom, GOOD_KNIPPELS, MakeInt(90 * fKBalls + rand(MakeInt(10 * fKBalls))));
	Fantom_SetCharacterGoods(rFantom, GOOD_GRAPES,   MakeInt(70 * fKBalls + rand(MakeInt(10 * fKBalls))));
	Fantom_SetCharacterGoods(rFantom, GOOD_POWDER,   MakeInt(350 * fKBalls + rand(MakeInt(30 * fKBalls))));

	Fantom_SetCharacterGoods(rFantom, GOOD_SAILCLOTH, MakeInt(4 * fKBalls + rand(MakeInt(10 * fKBalls))));
	Fantom_SetCharacterGoods(rFantom, GOOD_PLANKS, MakeInt(3 * fKBalls + rand(MakeInt(20 * fKBalls))));

    Fantom_SetCharacterGoods(rFantom, GOOD_FOOD, MakeInt(8 * fKBalls  + rand(MakeInt(26 * fKBalls)))));
    Fantom_SetCharacterGoods(rFantom, GOOD_WEAPON, MakeInt(6 * fKBalls + rand(MakeInt(13 * fKBalls))));
    Fantom_SetCharacterGoods(rFantom, GOOD_RUM, MakeInt(2 * fKBalls  + rand(MakeInt(10 * fKBalls)))));
    Fantom_SetCharacterGoods(rFantom, GOOD_MEDICAMENT, MakeInt(3 * fKBalls + rand(MakeInt(13 * fKBalls))));
    // boal 20.01.2004 <--
}

void Fantom_SetGoods(ref rFantom, string sFantomType)
{
 	int iMultiply = 10;
	int iRandMultiply = 1;
    int iShipClass = 7 - GetCharacterShipClass(rFantom);  // дл€ баркаса = 0
    // boal 20.01.2004 -->
    int iStart = GOOD_PLANKS + 1;
    int iFinish = GOOD_CANNON_12 - iStart - 1; // исключа€ пушки
    int i, iGoods;
	switch (sFantomType)
	{
		case "war":
		    iMultiply = (rand(5)+5) * (rand(iShipClass) + 1);
			iRandMultiply = 1 + rand(iShipClass);
			for (i = 0; i < 3; i++)
			{
			    Fantom_SetCharacterGoods(rFantom, iStart + rand(iFinish), iMultiply * rand(iRandMultiply * 3));
			}
		break;
		case "trade":
			if (CheckAttribute(rFantom, "RealEncounterType") && sti(rFantom.RealEncounterType) == ENCOUNTER_TYPE_ESCORT_LARGE)
			{
                iMultiply = (rand(10)+ 30) * (rand(iShipClass) + 2);
            	iRandMultiply = 2 + rand(iShipClass);
				if (rand(1) == 0)
				{
					Fantom_SetCharacterGoods(rFantom, GOOD_GOLD, 50 + iMultiply * (1 + rand(iRandMultiply * 3)));
				}
				else
				{
		        	Fantom_SetCharacterGoods(rFantom, GOOD_SILVER, 100 + iMultiply * (1 + rand(iRandMultiply * 3)));
		        }
			}
			else
			{
                iGoods = 8;
				if (CheckAttribute(rFantom, "RealEncounterType") && sti(rFantom.RealEncounterType) == ENCOUNTER_TYPE_MERCHANT_LARGE)
                {
                    iGoods = 11;
                }
				iMultiply = (rand(10)+iGoods*3) * (rand(iShipClass) + 4);
            	iRandMultiply = 2 + rand(iShipClass);
				for (i = 0; i < iGoods; i++)
				{
				    Fantom_SetCharacterGoods(rFantom, iStart + rand(iFinish), iMultiply * (1+rand(iRandMultiply * 3)));
				}
			}
		break;
		case "pirate":
			iMultiply = (rand(10)+3) * (rand(iShipClass) + 1);
			iRandMultiply = 2 + rand(iShipClass);
			for (i = 0; i < 4; i++)
			{
			    Fantom_SetCharacterGoods(rFantom, iStart + rand(iFinish), iMultiply * rand(iRandMultiply * 3));
			}
		break;
	}
    // boal 20.01.2004 <--
    // есть еще осталось место, то дать запасных орудий в трюм -->
    iMultiply = GetCannonGoodsIdxByType(sti(rFantom.Ship.Cannons.Type));
    if (iMultiply != -1)
    {
    	iRandMultiply = rand(6) - 2;  // 0..4 пушки
    	if (iRandMultiply > 0)
    	{
    		Fantom_SetCharacterGoods(rFantom, iMultiply, iRandMultiply);	
    	}
    }
    // есть еще осталось место, то дать запасных орудий в трюм <--
	RecalculateCargoLoad(rFantom);
}
// доработал метод, теперь возвращает сколько вз€л
int Fantom_SetCharacterGoods(ref rFantom, int iGoods, int iQuantity)
{
	if (iQuantity == 0) { return 0; }

	string sGood = Goods[iGoods].name;
	int iFreeQuantity = GetGoodQuantityByWeight( iGoods, GetCargoFreeSpace(rFantom) );
	if (iFreeQuantity < 0) { iFreeQuantity = 0; }

	if (iQuantity > iFreeQuantity) { iQuantity = iFreeQuantity; }
    rFantom.Ship.Cargo.Goods.(sGood) = iQuantity;
	RecalculateCargoLoad(rFantom);
	return iQuantity;
}

// опыт матросов
void Fantom_SetRandomCrewExp(ref rFantom, string sFantomType)
{
    if (!CheckAttribute(rFantom, "ship.type")) return; // fix
    
	int iShip = sti(rFantom.ship.type);
	if (iShip == SHIP_NOTUSED) return; // fix
	
	// ship class
	int iSClass = GetCharacterShipClass(rFantom);

	switch (sFantomType)
	{
		case "trade":
			rFantom.Ship.Crew.Exp.Sailors   = 20 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(70);
			rFantom.Ship.Crew.Exp.Cannoners = 20 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(70);
			rFantom.Ship.Crew.Exp.Soldiers  = 20 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(70);
		break;
		case "war":
			rFantom.Ship.Crew.Exp.Sailors   = 30 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(60);
			rFantom.Ship.Crew.Exp.Cannoners = 30 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(60);
			rFantom.Ship.Crew.Exp.Soldiers  = 30 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(60);
		break;
		case "pirate":
		    rFantom.Ship.Crew.Exp.Sailors   = 35 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(55);
			rFantom.Ship.Crew.Exp.Cannoners = 35 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(55);
			rFantom.Ship.Crew.Exp.Soldiers  = 35 - iSClass*2 + MOD_SKILL_ENEMY_RATE + rand(55);
		break;
	}
}

// eddy. подбор типа корабл€ дл€ фантома от ранга и нац. принадлежности
void SetShipToFantom(ref _chr, string _type, bool _setgoods)
{
	int ShipType;
	int Nation = sti(_chr.nation)
	int Rank = sti(pchar.rank);
	switch (_type)
	{
		case "trade":
			if (Rank < 11)
			{
				ShipType = RandFromThreeDight(SHIP_SLOOP, SHIP_SCHOONER, SHIP_LUGGER);
			}
			if (Rank >= 11 && Rank <= 20)
			{
				if (Nation == SPAIN) ShipType = RandFromThreeDight(SHIP_CARAVEL, SHIP_CARAVEL, SHIP_BARQUE);
				else ShipType = RandFromThreeDight(SHIP_FLEUT, SHIP_BARQUE, SHIP_GALEON_L);
			}
			if (Rank > 20)
			{
				if (Nation == SPAIN) ShipType = SHIP_GALEON_H;
				else ShipType = SHIP_PINNACE;
			}
		break;
		case "pirate":
			if (Rank < 4)
			{
				ShipType = SHIP_LUGGER;
			}			
			if (Rank >= 4 && Rank < 11)
			{
				ShipType = RandFromThreeDight(SHIP_SLOOP, SHIP_SCHOONER, SHIP_BRIG);
			}
			if (Rank >= 11 && Rank <= 20)
			{
				ShipType = RandFromThreeDight(SHIP_GALEON_L, SHIP_GALEON_H, SHIP_CORVETTE);
			}
			if (Rank > 20)
			{
				ShipType = SHIP_FRIGATE;
			}
		break;

		/*case "war":
			if (Rank < 11)
			{
				ShipType = RandFromThreeDight(SHIP_SLOOP, SHIP_SCHOONER, SHIP_LUGGER);
			}
			if (Rank >= 11 && Rank <= 20)
			{
				if (Nation == SPAIN) ShipType = RandFromThreeDight(SHIP_CARAVEL, SHIP_BRIG, SHIP_PINNACE);
				else ShipType = RandFromThreeDight(SHIP_BRIG, SHIP_GALEON_L, SHIP_PINNACE);
			}
			if (Rank > 20)
			{
				if (Nation == SPAIN) RandFromFiveDight(SHIP_GALEON_H, SHIP_LINESHIP, SHIP_WARSHIP, SHIP_BATTLESHIP, SHIP_MANOWAR);
				if (Nation == HOLLAND) ShipType = SHIP_GALEON_H;
				if (Nation == ENGLAND || Nation == FRANCE) ShipType = SHIP_PINNACE;
			}
		break;*/
	}
	_chr.Ship.Type = GenerateShip(ShipType, true);
	SetRandomNameToShip(_chr);
    SetBaseShipData(_chr);
    SetCrewQuantityFull(_chr);
    Fantom_SetCannons(_chr, _type);
    Fantom_SetBalls(_chr, _type);
    if (_setgoods)
    {
        Fantom_SetGoods(_chr, _type);
    }
}