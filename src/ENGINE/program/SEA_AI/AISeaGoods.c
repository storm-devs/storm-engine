object	AISeaGoods;

void DeleteSeaGoodsEnvironment()
{
	DeleteClass(&AISeaGoods);

	DelEventHandler(SHIP_DEAD, "AISeaGoods_ShipDead");
}

void CreateSeaGoodsEnvironment()
{
	CreateEntity(&AISeaGoods, "AISeaGoods");
	
	LayerAddObject(SEA_EXECUTE, &AISeaGoods, -1);
	LayerAddObject(SEA_REALIZE, &AISeaGoods, 65532);

	SetEventHandler(SHIP_DEAD, "AISeaGoods_ShipDead", 0);
	SetEventHandler(SHIP_EAT_SWIM_GOOD, "AISeaGoods_ShipEatGood", 0);

	AISeaGoods.ModelsPath = "SwimGoods";
	AISeaGoods.DeleteGoodAnyway = false;
	AISeaGoods.DistanceMultiply = 0.5;
}

void AISeaGoods_ShipDead()
{
	ref rCharacter = GetCharacter(GetEventData());

	aref aGoods; makearef(aGoods, rCharacter.Ship.Cargo.Goods);
	int iNumGoods;
	iNumGoods = GetAttributesNum(aGoods); // оптимизация
	for (int i=0; i<iNumGoods; i++)
	{
		aref aGood = GetAttributeN(aGoods, i);
		int	iQuantity = sti(GetAttributeValue(aGood));
		ref	rGood = GetGoodByID(GetAttributeName(aGood));

		if (!CheckAttribute(rGood, "Swim")) { continue; }
		
		iQuantity = iQuantity / sti(rGood.Units);
		if (iQuantity <= 1) { continue; }

		if (rand(100) > 90) { continue; }

		int iSwimQuantity = 0; 
		while (iSwimQuantity == 0) { iSwimQuantity = MakeInt(iQuantity / 4 + rand(MakeInt(iQuantity * 2 / 4))); }
		float fTime = stf(rGood.Swim.Time);
		string sModel = rGood.Swim.Model;

		fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

		AISeaGoods_AddGood(rCharacter, GetAttributeName(aGood), sModel, fTime * 60.0, iSwimQuantity);
	}
}

void AISeaGoods_AddGood(ref rCharacter, string sGood, string sModel, float fTime, int iQuantity)
{
	if (!bSeaActive) return;

	//Trace("Add good : " + sGood + ", rCharacter.id = " + rCharacter.id + ", iQuantity = " + iQuantity);

	// calculate random position
		float fAng = stf(rCharacter.Ship.Ang.y) + PI + (frnd() - 0.5) * PI;
		float fDist = stf(rCharacter.Ship.BoxSize.z) / 2.0 + frnd() * 10.0;
		//float x = (frnd() - 0.5) * 20.0;
		//float z = (frnd() - 0.5) * 40.0;
		//RotateAroundY(&x, &z, cos(fAng), sin(fAng));

	AISeaGoods.Good = sGood;
	AISeaGoods.Pos.x = stf(rCharacter.Ship.Pos.x) + fDist * sin(fAng);
	AISeaGoods.Pos.z = stf(rCharacter.Ship.Pos.z) + fDist * cos(fAng);
	AISeaGoods.CharIndex = rCharacter.Index;
	AISeaGoods.Time = fTime;
	AISeaGoods.Quantity = iQuantity;
	AISeaGoods.Model = sModel;
	AISeaGoods.Add = "";
}

bool AISeaGoods_ShipEatGood()
{
	object oRes;

	int iCharacterIndex = GetEventData();
	int iGoodCharacterIndex = GetEventData();
	string sGood = GetEventData();
	int iQuantity = GetEventData();

	ref rCharacter = GetCharacter(iCharacterIndex);
	
	if (sGood == "boat")   //homo 22/06/07 если подобрали шлюпку и ГГ
	{

		if(iCharacterIndex == sti(pchar.index))
		{
			pchar.GenQuest.Survive_In_SeaOfficerIdx = iGoodCharacterIndex;
            DoQuestCheckDelay("Survive_In_SeaOfficer", 1.0);
            return true;
		}
		else
		{
			DeleteAttribute(Pchar, "GenQuest.Survive_In_SeaOfficerIdx");
			return true;
		}
	}
	if (sGood == "enemy_boat")   //homo 22/06/07 если подобрали шлюпку
	{
        if(iCharacterIndex == sti(pchar.index))
		{
			pchar.GenQuest.Survive_In_SeaPrisonerIdx = iGoodCharacterIndex;
            DoQuestCheckDelay("Survive_In_SeaPrisoner", 1.0);
            return true;
		}
		else
		{
			return true;
		}

    }

	if (iGoodCharacterIndex == iCharacterIndex) 
	{
		return false;
	}
	if (LAi_IsDead(rCharacter)) 
	{
		return false;
	}

	//trace("character " + rCharacter.id + " eat good");

	ref rShip = GetRealShip(sti(rCharacter.Ship.Type));
	int iGood = FindGood(sGood);

	ref rGood = &Goods[iGood];

	//int iLoad = GetCargoLoad(rCharacter);
	//int iCapacity = sti(rShip.Capacity);
	int iGoodWeight = sti(rGood.Weight);
	int iGoodUnits = sti(rGood.Units);

	int iMaxGoodAllow = GetCharacterFreeSpace(rCharacter, iGood);
	//int iMaxGoodAllow = (iCapacity - iLoad) / iGoodWeight;

	/*if(iCapacity < iGoodWeight)
	{
		return false;
	}
	*/

	if(iMaxGoodAllow == 0)
	{
		return false;
	}

	string sGoodName = LanguageConvertString(iSeaSectionLang, "seg_" + sGood);

	if (iMaxGoodAllow == 0)
	{
		string sShipCantGotGood = LanguageConvertString(iSeaSectionLang, "Ship_cant_got_good");
		Event(PARSE_STRING, "asls", &oRes, sShipCantGotGood, 1, sGoodName);
		Log_SetStringToLog(oRes.Str);
		return false;
	}

	if (iQuantity > iMaxGoodAllow) 
	{
		iQuantity = iMaxGoodAllow;
	}

	if (iCharacterIndex == nMainCharacterIndex)
	{
        PlaySound("INTERFACE\_Gotcha.wav"); // boal
		string sGoodQuantity = iQuantity * iGoodWeight;
		string sShipGotGood = LanguageConvertString(iSeaSectionLang, "Ship_got_good");
		Event(PARSE_STRING, "aslss", &oRes, sShipGotGood, 2, sGoodQuantity, sGoodName);
		Log_SetStringToLog(oRes.Str);
	}

	iQuantity = iQuantity * iGoodUnits;

	AddCharacterGoods(rCharacter, iGood, iQuantity);
	
	return true;
}