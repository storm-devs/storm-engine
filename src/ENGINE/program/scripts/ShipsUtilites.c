
#define REAL_SHIPS_QUANTITY 900

object	RealShips[REAL_SHIPS_QUANTITY];

ref GetRealShip(int iType) 
{ 
	if(iType >= REAL_SHIPS_QUANTITY)
	{
		trace ("--- Wrong Ship Index. iType is " + iType);
		Log_TestInfo("--- Wrong Ship Index. iType is " + iType);
		return &ShipsTypes[SHIP_TYPES_QUANTITY + 1]; // для отлова
	}
	return &RealShips[iType]; 
}

// isLock - рудимент, можно выкидывать (26.05.06 boal)
// новая жизнь isLock (04.07.06) теперь это признак ворованности (те цены на верфи)
// если 1 - ворованный, цена копеешная, 0 - честно купленный - можно перепродать.
int GenerateShip(int iBaseType, bool isLock)
{
	int iShip = CreateBaseShip(iBaseType);

	if (iShip == -1)
	{
		return SHIP_NOTUSED;
	}
	
	ref rRealShip = GetRealShip(iShip);
	ref rBaseShip = GetShipByType(sti(rRealShip.BaseType));
    // boal 26/05/06 изменим
    rRealShip.ship.upgrades.hull  = 1 + rand(2);  //признак корабля теперь
	rRealShip.ship.upgrades.sails = 1 + rand(2);  // только визуальная разница
	
	if (!CheckAttribute(rRealShip, "isFort"))
	{
	    int iCaliber = sti(rRealShip.MaxCaliber);
	    if (sti(rRealShip.Class) != 7)
	    {  // чтоб не было баркаса с 16ф
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
					iCaliber = 3;
				break;
				case 42:
					iCaliber = 4;
				break;
		    }
		    iCaliber = iCaliber + rand(2) - 1;
		    if (iCaliber < 0) iCaliber = 0;
		    if (rRealShip.BaseName == "Manowar")
		    {
		    	if (iCaliber > 4) iCaliber = 4;
		    }
		    else
		    {
		    	if (iCaliber > 3) iCaliber = 3;
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
		    rRealShip.MaxCaliber = iCaliber;
	    }
	    rRealShip.SpeedRate	       = stf(rRealShip.SpeedRate) + frandSmall(stf(rRealShip.SpeedRate) / 5.0) - stf(rRealShip.SpeedRate) / 10.0;
	    rRealShip.TurnRate         = stf(rRealShip.TurnRate) + frandSmall(stf(rRealShip.TurnRate) / 5.0) - stf(rRealShip.TurnRate) / 10.0;
	    //rRealShip.Price            = sti(rRealShip.Price) + rand(makeint(sti(rRealShip.Price)/2)) - makeint(sti(rRealShip.Price)/4);
	    rRealShip.HP               = sti(rRealShip.HP) + rand(makeint(sti(rRealShip.HP)/5)) - makeint(sti(rRealShip.HP)/10);
	    rRealShip.WindAgainstSpeed = stf(rRealShip.WindAgainstSpeed) + frandSmall(stf(rRealShip.WindAgainstSpeed)/5.0) - stf(rRealShip.WindAgainstSpeed)/10.0;
	}
    rRealShip.Capacity        = sti(rRealShip.Capacity) + rand(makeint(sti(rRealShip.Capacity)/4)) - makeint(sti(rRealShip.Capacity)/8);
    rRealShip.OptCrew         = makeint(sti(rRealShip.OptCrew) + rand(makeint(sti(rRealShip.OptCrew)/3)) - makeint(sti(rRealShip.OptCrew)/6));
    rRealShip.MaxCrew         = makeint(sti(rRealShip.OptCrew) * 1.25 + 0.5);  // 25% перегруза
    rRealShip.MinCrew         = makeint(sti(rRealShip.MinCrew) + rand(makeint(sti(rRealShip.MinCrew)/3)) - makeint(sti(rRealShip.MinCrew)/6));

	rRealShip.Weight		  = sti(rRealShip.Weight) + rand(sti(rRealShip.Weight)/20) - rand(sti(rRealShip.Weight)/20);
	
	// to_do del -->
	rRealShip.BoardingCrew    = 0;
	rRealShip.GunnerCrew      = 0;
	rRealShip.CannonerCrew    = 0;
	rRealShip.SailorCrew      = sti(rRealShip.OptCrew);
    // to_do del <--
    
	int iDiffWeight			= sti(rRealShip.Weight) - sti(rBaseShip.Weight);
	int iDiffCapacity		= sti(rRealShip.Capacity) - sti(rBaseShip.Capacity);
	int iDiffMaxCrew		= sti(rRealShip.MaxCrew) - sti(rBaseShip.MaxCrew);
	int iDiffMinCrew		= sti(rRealShip.MinCrew) - sti(rBaseShip.MinCrew);
	float fDiffSpeedRate	= stf(rRealShip.SpeedRate) - stf(rBaseShip.SpeedRate);
	int iDiffTurnRate		= sti(rRealShip.TurnRate) - sti(rBaseShip.TurnRate);
	int iDiffHP	    		= sti(rRealShip.HP) - sti(rBaseShip.HP);

	rRealShip.Price	= (iDiffWeight + iDiffCapacity + iDiffMaxCrew*2 + iDiffMinCrew + fDiffSpeedRate*2 + iDiffTurnRate*2 + iDiffHP)*5 + sti(rRealShip.Price);

	if (sti(rRealShip.Price) <= 0) rRealShip.Price = 100;
	
	rRealShip.Stolen = isLock;  // ворованность

	return iShip;
}

int CreateBaseShip(int iBaseType)
{
	int iShip = FindFirstEmptyShip();

	if(iShip == -1)
	{
        trace("Can not find empty ship slot");
		return SHIP_NOTUSED;
	}

	ref rBaseShip = GetShipByType(iBaseType);
	ref rRealShip = GetRealShip(iShip);
	DeleteAttribute(rRealShip, "");
	CopyAttributes(rRealShip, rBaseShip);
    rRealShip.index = iShip;
    rRealShip.BaseType = iBaseType;
    if (!CheckAttribute(rRealShip, "ship.upgrades.hull")) // если задано в базе, то квестовый тип, не перебивать
    {
    	rRealShip.ship.upgrades.hull = 1;  //признак корабля теперь
    }
	rRealShip.ship.upgrades.sails = 1;
	
    rRealShip.BaseName = rRealShip.name; // запоминалка для нужд, тк далее идет "странное"
	if (rRealShip.name != "Fort" && rRealShip.name != "Boat") // не знаю зачем :(
	{
		rRealShip.name = rRealShip.name + "1"; // без этого вылет, но почему!!!!!!!!!!!!!!
		// выяснил - папка с моделью имеет на конце 1, вот и ответ
	}
    rRealShip.Stolen = 0;  // ворованность - честный
    
	if (iArcadeSails == 0) // момент инерции ниже для тактики
	{
	    rRealShip.InertiaAccelerationY = stf(rRealShip.InertiaAccelerationY) / 2.0;
	}
    
	return iShip;
}
// Генерация корабля для верфи
int GenerateStoreShip(int iBaseType)
{
	int iShip = GenerateShip(iBaseType, 0); // честный 

	if (iShip == -1)
	{
		return SHIP_NOTUSED;
	}
	
	ref rRealShip = GetRealShip(iShip);
    rRealShip.StoreShip = true;  // кораль на верфи, трется отдельным методом
    
	return iShip;	
}
// переработка метода 26.05.06 boal
// идея: трем все корабли, где нет НПС-владельца, все галки пофиг
void EmptyAllFantomShips()
{
	int  i;
	ref  chr;
	// оптимальный код - два прохода, а не вложенный цикл. В начале всем сносим флаг удаляемости, потом проставляем, где есть НПС
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
        DeleteAttribute(&RealShips[i], "lock");
	}
	// лодку ставим, тк он имеет индекс НПС за пределами MAX_CHARACTERS
	RealShips[sti(Characters[BOAT_CHARACTER].ship.type)].lock = true;
	// теперь НПС-кэпы
	for (i = 0; i < MAX_CHARACTERS; i++)
	{
        chr = &Characters[i];
        if (CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        {
            RealShips[sti(chr.ship.type)].lock = true;
        }
        // очистка для верфи
		if (CheckAttribute(chr, "shipyard"))
		{
		    FreeShipFromShipyard(chr);
		}
	}
	for (i = FANTOM_CHARACTERS; i < TOTAL_CHARACTERS; i++) // фантомы в море
	{
        chr = &Characters[i];
        if (CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        {
            RealShips[sti(chr.ship.type)].lock = true;
        }
	}
	// теперь сборка мусора
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
        if (!CheckAttribute(&RealShips[i], "lock") && !CheckAttribute(&RealShips[i], "StoreShip"))  // на верфи корабли не трем
        {
			DeleteAttribute(&RealShips[i], "");
		}
	}
}

int GetRealShipsCount()
{ // метод для отладки
	int result = 0;

	for (int i= 0; i< REAL_SHIPS_QUANTITY; i++)
	{
		if (checkAttribute(&RealShips[i], "name"))
		{
			result++;
		}
	}

	return result;
}

// поиск пустого слота
int FindFirstEmptyShip()
{
	for(int i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
		if(!checkAttribute(&RealShips[i], "name"))
		{
			return i;
		}
	}

	return -1;
}


/*
float SpeedByHullUpgrade(aref refCharacter)
{
	int iHull = sti(refCharacter.ship.upgrades.hull) - 1;

	float fResult = 1.0 - (0.05 * iHull);

	return fResult;
}


float SpeedBySailsUpgrade(aref refCharacter)
{
	int iSails = sti(refCharacter.ship.upgrades.sails) - 1;
	iSails = iSails * 5;
	float fResult = 1.0 + (0.01 * iSails);

	return fResult;
}
*/

float FindShipSpeed(aref refCharacter)
{
	if(!CheckAttribute(refCharacter, "Ship.type"))
	{
		trace("Character " + refCharacter.id + " have no ship!");
		return 0.0;
	}
	int nShipType = sti(refCharacter.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " + refCharacter.id + " have invalid ship!");
		return 1.0;
	}
	ref rShip = GetRealShip(nShipType);
	
	float	fMaxSpeedZ = stf(rShip.SpeedRate);  // базовая скорость
	float fShipHp = stf(refCharacter.ship.hp);
	float fShipMaxHp = stf(rShip.HP);
	float fSpeedFromHp = fShipHp/fShipMaxHp;
	fSpeedFromHp = 0.8 + fSpeedFromHp*0.3;

	if(fSpeedFromHp > 1.0) // некий запас перед снижением скорости
	{
		fSpeedFromHp = 1.0;
	}

	float fSailsDamage = stf(refCharacter.ship.sp);

	float	fTRFromSailDamage = Bring2Range(0.1, 1.0, 0.1, 100.0, fSailsDamage); //0.3

	float fTRFromShipState = fSpeedFromHp * fTRFromSailDamage;
	
	float	fLoad = Clampf(stf(refCharacter.Ship.Cargo.Load) / stf(rShip.Capacity));
	float	fTRFromWeight = Clampf(1.03 - stf(rShip.SpeedDependWeight) * fLoad);
	float   fTRFromSkill = SpeedBySkill(refCharacter);
	// от команды
	float fCrewMax = stf(rShip.MaxCrew);
	float fCrewOpt = stf(rShip.OptCrew);
	float fCrewCur = stf(refCharacter.Ship.Crew.Quantity);
	if (fCrewCur > fCrewMax)
	{
		fCrewCur = fCrewMax;
	}
	float  fTRFromPeople;
	fTRFromPeople = 0.85 + ((GetCrewExp(refCharacter, "Sailors") * fCrewCur) / (fCrewOpt * GetCrewExpRate())) * 0.15;
	if (fTRFromPeople > 1) fTRFromPeople = 1;
	 
	fMaxSpeedZ = fMaxSpeedZ * fTRFromWeight * fTRFromSkill * fTRFromShipState * fTRFromPeople;

	return fMaxSpeedZ;
}

float SpeedBySkill(aref refCharacter)
{
	float fSkill = GetSummonSkillFromNameToOld(refCharacter, SKILL_SAILING);

	float fTRFromSkill = 0.7 + (0.03 *  fSkill);

	//float fSpeedByHullUpgrade = SpeedByHullUpgrade(refCharacter);
	//float fSpeedBySailsUpgrade = SpeedBySailsUpgrade(refCharacter);

	//fTRFromSKill = fTRFromSKill * fSpeedByHullUpgrade * fSpeedBySailsUpgrade;
    float fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "ShipSpeedUp"), 1.0, 1.15);   //slib
    fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SailingProfessional"), fSpeedPerk, 1.20);
    
	return fTRFromSKill*fSpeedPerk;
}

float FindShipTurnRate(aref refCharacter)
{
	if(!CheckAttribute(refCharacter, "Ship.type"))
	{
		trace("Character " + refCharacter.id + " have no ship!");
		return 0.0;
	}
	int nShipType = sti(refCharacter.ship.type);
	if(nShipType == SHIP_NOTUSED)
	{
		trace("Character " + refCharacter.id + " have invalid ship!");
		return 1.0;
	}
	ref rShip = GetRealShip(nShipType);

	float fShipHp = stf(refCharacter.ship.hp);
	float fShipMaxHp = stf(rShip.HP);
	float fSpeedFromHp = fShipHp/fShipMaxHp;
	fSpeedFromHp = 0.7 + fSpeedFromHp * 0.3;

	float fLoad = Clampf(stf(refCharacter.Ship.Cargo.Load) / stf(rShip.Capacity));
	float fTRFromWeight;
	if (iArcadeSails == 1)
	{
		fTRFromWeight = Clampf(1.03 - stf(rShip.TurnDependWeight) * fLoad);
	}
	else
	{
		fTRFromWeight = Clampf(1.03 - 2.0*stf(rShip.TurnDependWeight) * fLoad); // влияние веса больше
	}
	// не реализовано, всегда 1 float fTRFromSpeed = 1.0 - stf(rShip.TurnDependSpeed);
	// от команды
	//float fCrewMin = stf(rShip.MinCrew);
	float fCrewMax = stf(rShip.MaxCrew);
	float fCrewOpt = stf(rShip.OptCrew);
	float fCrewCur = stf(refCharacter.Ship.Crew.Quantity);
	if (fCrewCur > fCrewMax)
	{
		fCrewCur = fCrewMax;
	}
	float  fTRFromPeople;
	if (iArcadeSails == 1)
	{
		fTRFromPeople = 0.3 + ((GetCrewExp(refCharacter, "Sailors") * fCrewCur) / (fCrewOpt * GetCrewExpRate())) * 0.7;
	}
	else
	{
		fTRFromPeople = 0.05 + ((GetCrewExp(refCharacter, "Sailors") * fCrewCur) / (fCrewOpt * GetCrewExpRate())) * 0.95;
	}
	if (fTRFromPeople > 1) fTRFromPeople = 1;
	
	float fTRFromSKill = TurnBySkill(refCharacter);

	float fTRFromSailDamage = Bring2Range(0.05, 1.0, 0.1, 100.0, stf(refCharacter.ship.sp));

	float fTurn = fTRFromWeight * fTRFromSkill * fTRFromPeople * fTRFromSailDamage * fSpeedFromHp;

	//Log_info(refCharacter.id + "  " + fTurn);
	return fTurn;	
}

float TurnBySkill(aref refCharacter)
{
	float fSkill = GetSummonSkillFromNameToOld(refCharacter, SKILL_SAILING);
	float fTRFromSKill;

    if (iArcadeSails == 1)
	{
		fTRFromSKill = 0.5 + (0.05 * fSkill);
	}
	else
	{
		fTRFromSKill = 0.3 + (0.07 * fSkill);
	}
	
    float fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "ShipTurnRateUp"), 1.0, 1.15);   //slib
    fSpeedPerk = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "SailingProfessional"), fSpeedPerk, 1.20);
    float fFastTurn180 = AIShip_isPerksUse(CheckOfficersPerk(refCharacter, "Turn180"), 1.0, 8.0);
    
	return fTRFromSKill*fSpeedPerk*fFastTurn180;
}

int GetCannonsNum(aref chr)
{
	int nShipType = sti(chr.ship.type);
    ref refBaseShip = GetRealShip(nShipType);

	int iCannons = GetBortIntactCannonsNum(chr, "cannonf", sti(refBaseShip.fcannon)) + GetBortIntactCannonsNum(chr, "cannonb", sti(refBaseShip.bcannon)) + GetBortIntactCannonsNum(chr, "cannonl", sti(refBaseShip.lcannon)) + GetBortIntactCannonsNum(chr, "cannonr", sti(refBaseShip.rcannon));

	return iCannons;
}

// boal
int GetBortCannonsQty(aref chr, string  sType)
{
	int nShipType = sti(chr.ship.type);
    ref refBaseShip = GetRealShip(nShipType);

	int iCannons = GetBortIntactCannonsNum(chr, sType, sti(refBaseShip.(sType)));

	return iCannons;
}

int GetBortCannonsMaxQty(aref chr, string  sType)
{
	int nShipType = sti(chr.ship.type);
    ref refBaseShip = GetRealShip(nShipType);

	int iCannons = sti(refBaseShip.(sType));

	return iCannons;
}

///////////////////////  ВЕРФИ //////////////////////
void SetShipyardStore(ref NPChar)
{
    int    iTest_ship, i;
	string attrName;
	
	if (CheckAttribute(NPChar, "shipyard")) return; // еще есть корабли с того раза
	
	SaveCurrentNpcQuestDateParam(npchar, "shipyardDate"); // дата заполнения верфи
    
    if (bBettaTestMode)
    {
        for (i = 1; i <=SHIP_TYPES_QUANTITY; i++)
        {
            attrName = "ship" + i;
            FillShipParamShipyard(NPChar, GenerateStoreShip(i-1), attrName);
        }
        
        return;
    }
    
    FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_TARTANE), "ship1");
    
	iTest_ship = rand(2);
	if (iTest_ship == 1)
	{
		FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_WAR_TARTANE), "ship2");
	}
	if (iTest_ship == 2)
	{
		FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_LUGGER), "ship2");
	}

	iTest_ship = rand(2);
	if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_LUGGER), "ship3");
	if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_SLOOP), "ship3");

	iTest_ship = rand(3);
	if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_SLOOP), "ship4");
	if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_LUGGER), "ship4");

    if (sti(PChar.rank) > 1)
    {
		iTest_ship = rand(4);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_SCHOONER), "ship5");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BARQUE), "ship5");
	
		iTest_ship = rand(4);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_SCHOONER), "ship6");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BARQUE), "ship6");
	}
	if (sti(PChar.rank) > 3)
    {
		iTest_ship = rand(6);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CARAVEL), "ship8");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CARAVEL), "ship8");
	
		iTest_ship = rand(6);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_FLEUT), "ship9");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CARAVEL), "ship9");
	
		iTest_ship = rand(6);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_FLEUT), "ship10");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CARAVEL), "ship10");
	}
	if (sti(PChar.rank) > 5)
    {
		iTest_ship = rand(8);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BRIG), "ship11");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_FLEUT), "ship11");
	
		iTest_ship = rand(8);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BRIG), "ship12");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_GALEON_L), "ship12");
	
		iTest_ship = rand(8);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BRIG), "ship13");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_GALEON_L), "ship13");
    }
    if (sti(PChar.rank) > 8)
    {
		iTest_ship = rand(50);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_GALEON_L), "ship14");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CORVETTE), "ship14");
	
		iTest_ship = rand(70);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_GALEON_H), "ship15");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_PINNACE), "ship15");
    }
    
    if (sti(PChar.rank) > 12)
    {
		iTest_ship = rand(50);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_FRIGATE), "ship141");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_CORVETTE), "ship141");
	
		iTest_ship = rand(70);
		if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_FRIGATE), "ship151");
		if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_PINNACE), "ship151");
		if (iTest_ship == 3) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_GALEON_H), "ship151");
    }
    
    if (sti(PChar.rank) > 18)
    {
       iTest_ship = rand(120);
	   if (iTest_ship == 1)   FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_LINESHIP), "ship16");
       if (iTest_ship == 2)   FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_WARSHIP), "ship16");
       if (iTest_ship == 3)   FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BATTLESHIP), "ship16");

	   iTest_ship = rand(120);
	   if (iTest_ship == 1) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_LINESHIP), "ship17");
	   if (iTest_ship == 2) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_WARSHIP), "ship17");
       if (iTest_ship == 3) FillShipParamShipyard(NPChar, GenerateStoreShip(SHIP_BATTLESHIP), "ship17");
    }
}

void FreeShipFromShipyard(ref NPChar)
{
	if (GetNpcQuestPastDayParam(NPChar, "shipyardDate") > (3 + rand(6)))
	{
        aref   arDest, arImt;
		string sAttr;
		int    iNum, i, iShip;
		makearef(arDest, NPChar.shipyard);
		iNum = GetAttributesNum(arDest);
		for (i = 0; i < iNum; i++)
		{
	    	arImt = GetAttributeN(arDest, i);
	    	iShip = sti(arImt.Type); //GetAttributeValue(arImt));
	    	DeleteAttribute(&RealShips[iShip], "StoreShip"); // можно тереть
	    }
        DeleteAttribute(NPChar, "shipyard");
	}
}

void FillShipParamShipyard(ref NPChar, int _iType, string _sShipNum)
{
    aref    arTo, arFrom;
    
	DeleteAttribute(NPChar, "ship");
    NPChar.Ship.Type = _iType;
    
	SetRandomNameToShip(NPChar);

    SetBaseShipData(NPChar);
    SetCrewQuantity(NPChar, 0);
    if (sti(NPChar.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
    {
    	NPChar.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS12; // to_do переделать на продажу без орудий вообще
    }
    DeleteAttribute(NPChar, "Ship.Cargo");  //пустой трюм
    SetGoodsInitNull(NPChar);
    RecalculateCargoLoad(NPChar);
    
    FillShipyardShipBack(NPChar, _sShipNum);
}

void FillShipyardShip(ref NPChar, string _sShipNum)
{
    aref    arTo, arFrom;

	DeleteAttribute(NPChar, "ship");

    makearef(arFrom,   NPChar.shipyard.(_sShipNum));
    NPChar.Ship = "";
	makearef(arTo, NPChar.Ship);
	CopyAttributes(arTo, arFrom);
}

void FillShipyardShipBack(ref NPChar, string _sShipNum)
{
    aref    arTo, arFrom;

  	DeleteAttribute(NPChar, "shipyard." + _sShipNum);
    NPChar.shipyard.(_sShipNum) = "";
    makearef(arTo,   NPChar.shipyard.(_sShipNum));
	makearef(arFrom, NPChar.Ship);
	CopyAttributes(arTo, arFrom);
}

// палуба от типа корабля
string GetShip_deck(ref chr, bool map2sea)
{
	string ret, add;
	int    iType = sti(chr.Ship.Type);
	if (map2sea)
	{
	    ret = "Ship_deck_";
	}
	else
	{
	    ret = "Deck_Near_Ship_";
	}
	add = "Low"; // если нет типа
	if (iType != SHIP_NOTUSED)
	{
        ref rRealShip = GetRealShip(iType);
        
		add = rRealShip.DeckType;
	}
	
	return ret + add;
}

void MakeCloneShipDeck(ref chr, bool map2sea)
{
	ref rOrg, rClone;
	int iOrg, iClone;
    string locId, toLocId;
    
    locId = GetShip_deck(chr, map2sea);
    if (map2sea)
	{
	    toLocId = "Ship_deck";
	}
	else
	{
	    toLocId = "Deck_Near_Ship";
	}
	
	iOrg = FindLocation(locId);
	iClone = FindLocation(toLocId);

	makeref(rOrg, Locations[iOrg]);
	makeref(rClone, Locations[iClone]);

	DeleteAttribute(rClone, "");
	CopyAttributes(rClone, rOrg);
	rClone.id = toLocId;
	rClone.index = iClone;
}

void MakeCloneFortBoarding(string fromLocId)
{
	ref rOrg, rClone;
	int iOrg, iClone;
    string toLocId;
    
    toLocId = "BOARDING_FORT";

	iOrg = FindLocation(fromLocId);
	iClone = FindLocation(toLocId);

	makeref(rOrg, Locations[iOrg]);
	makeref(rClone, Locations[iClone]);

	DeleteAttribute(rClone, "");
	CopyAttributes(rClone, rOrg);
	rClone.id = toLocId;
	rClone.index = iClone;
	
    rClone.type = "fort_attack";
	LAi_LocationFantomsGen(rClone, false);
	DeleteAttribute(rClone, "reload");
	//это подкручивание другого файла локаторов. там нет goto, soldiers и пр. есть rld и дополн.сундуки
	if (rClone.models.always.locators == "fortV_locators") 
	{
		rClone.models.always.locators = "fortV_lAttack";
		rClone.models.always.fortV = "fortV_attack";	
	}
	else
	{
		rClone.models.always.locators = "fortVRight_lAttack";
		rClone.models.always.fortV = "fortVRight_attack";
	}
	rClone.boarding.locatorNum = 15;
	rClone.boarding = "false";
	rClone.boarding.nextdeck = "";
	rClone.image = "loading\Fort_512.tga";
}