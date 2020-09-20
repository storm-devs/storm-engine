/// BOAL Верфь
#define sailCostPercent		10
#define hullCostPercent		20

int nCurScrollNum;
ref xi_refCharacter;
ref refNPCShipyard;
int shipIndex;

int nCurScrollOfficerNum;

string CurTable, CurRow;
int iSelected; // курсор в таблице

string sMessageMode, sFrom_sea, sShipId;

float shipCostRate;
bool  bShipyardOnTop, bEmptySlot;

void InitInterface(string iniName)
{
	GameInterface.title = "titleShipyard";

	xi_refCharacter = pchar;
	
    refNPCShipyard  = characterFromID("SentJons_shipyarder");
    SetShipyardStore(refNPCShipyard);
    
    shipCostRate = stf(refNPCShipyard.ShipCostRate);
    
    int iTest = FindColony(refNPCShipyard.City); // город магазина
    ref rColony;
    sFrom_sea = "";
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		sFrom_sea = rColony.from_sea; // ветка верфи, в сухопутных верфи нет, значит везде правильная
	}
	
	FillShipsScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("frame","ProcessFrame",1);
    SetEventHandler("confirmShipChangeName","confirmShipChangeName",0);
	SetEventHandler("CheckForRename","CheckForRename",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ExitMsgMenu", "ExitMsgMenu", 0);
	SetEventHandler("ShowOtherClick", "ShowOtherClick", 0);
	SetEventHandler("ExitCannonsMenu", "ExitCannonsMenu", 0);
    SetEventHandler("ExitOfficerMenu","ExitOfficerMenu",0);
	SetEventHandler("acceptaddofficer","AcceptAddOfficer",0);
    //////////////////
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",156,40,366,275); // tak from SHIP_BIG_PICTURE
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 161,246,361,1, 4);
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",20,46,134,161);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,187,147,1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,166,147,1, 4);

    SetNewGroupPicture("Money_PIC", "ICONS_CHAR", "Money");
    
    FillShipyardTable();
    
	SetCurrentNode("SHIPS_SCROLL");
	bShipyardOnTop = false;
	OnShipScrollChange();
	bEmptySlot = false;
	sMessageMode = "";
	SetButtionsAccess();

}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("frame","ProcessFrame");
    DelEventHandler("confirmShipChangeName","confirmShipChangeName");
	DelEventHandler("CheckForRename","CheckForRename");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ExitMsgMenu", "ExitMsgMenu");
	DelEventHandler("ShowOtherClick", "ShowOtherClick");
	DelEventHandler("ExitCannonsMenu", "ExitCannonsMenu");
    DelEventHandler("ExitOfficerMenu","ExitOfficerMenu");
	DelEventHandler("acceptaddofficer","AcceptAddOfficer");
	
	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
        DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	
    switch(nodName)
	{
        case "BUTTON_SELL":
			if (comName=="click" || comName=="activate")
			{
			    ShowMessageInfo();
			}
		break;
		
		case "BUTTON_BUY":
			if (comName=="click" || comName=="activate")
			{
			    ShowMessageInfo();
			}
		break;
		
		case "MSG_OK":
			if (comName=="click" || comName=="activate")
			{
			    MessageOk();
			}
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
void FillShipsScroll()
{
    DeleteAttribute(&GameInterface, "SHIPS_SCROLL");
	nCurScrollNum = -1;
	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}

	string attributeName;
	string shipName;
	int iShipType, cn;
	GameInterface.SHIPS_SCROLL.ImagesGroup.t0 = "BLANK_SHIP2";

	FillShipList("SHIPS_SCROLL.ImagesGroup", xi_refCharacter);
	GameInterface.SHIPS_SCROLL.BadTex1 = 0;
	GameInterface.SHIPS_SCROLL.BadPic1 = "Not Used2";

	int m = 0;
	for(int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(xi_refCharacter, i);
		if(cn != -1)
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				attributeName = "pic" + (m+1);
				iShipType = sti(RealShips[iShipType].basetype);
				shipName = ShipsTypes[iShipType].Name;

				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#"+"Класс "+ShipsTypes[iShipType].Class;
				GameInterface.SHIPS_SCROLL.(attributeName).str4 = shipName;
				GameInterface.SHIPS_SCROLL.(attributeName).str3 = "#" + MakeMoneyShow(GetSellPrice(&characters[cn]), MONEY_SIGN,MONEY_DELIVER);
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "ship";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("SHIPS_SCROLL.ImagesGroup","SHIPS_"+shipName);
				m++;
			}
			else
			{
				attributeName = "pic" + (m+1);
				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#";
				GameInterface.SHIPS_SCROLL.(attributeName).str2 = "NoneBoat";
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "Not Used2";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = "BLANK_SHIP2";
				m++;
			}
		}
		else
		{
		    attributeName = "pic" + (m+1);
			GameInterface.SHIPS_SCROLL.(attributeName).character = -1;
			GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#";
			GameInterface.SHIPS_SCROLL.(attributeName).str2 = "";
			GameInterface.SHIPS_SCROLL.(attributeName).img1 = "Not Used2";
			GameInterface.SHIPS_SCROLL.(attributeName).tex1 = "BLANK_SHIP2";
			m++;
		}
	}

	GameInterface.SHIPS_SCROLL.ListSize = m;
	//GameInterface.SHIPS_SCROLL.NotUsed = 5 - m + 1;
	GameInterface.SHIPS_SCROLL.NotUsed = 0;
}

void ProcessFrame()
{
	string attributeName;
	int iCharacter;

	string sNode = GetCurrentNode();
	
    if (sNode == "PASSENGERSLIST" && sti(GameInterface.PASSENGERSLIST.current)!= nCurScrollOfficerNum)
	{
		nCurScrollOfficerNum = sti(GameInterface.PASSENGERSLIST.current);
		SetOfficersSkills();
	}
	if(sNode == "SHIPS_SCROLL")
	{
		if (sti(GameInterface.SHIPS_SCROLL.current)!= nCurScrollNum || CurTable != "")
		{
            CurTable = "";
			NullSelectTable("TABLE_SHIPYARD"); // убрать скрол
			bShipyardOnTop = false;
			
			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);

			attributeName = "pic" + (nCurScrollNum+1);

			iCharacter = sti(GameInterface.SHIPS_SCROLL.(attributeName).character);

			if(iCharacter > 0)
			{
				string sChrId = characters[iCharacter].id;

				xi_refCharacter = characterFromID(sChrId);
				bEmptySlot = false;
				shipIndex= nCurScrollNum+ 1;
				if (shipIndex< 0)
				{
					shipIndex= 0;
				}
				if (sti(xi_refCharacter.ship.type) == SHIP_NOTUSED)
				{
					shipIndex= -1;
				}
				// boal оптимизация скилов -->
			    DelBakSkillAttr(xi_refCharacter);
			    ClearCharacterExpRate(xi_refCharacter);
			    RefreshCharacterSkillExpRate(xi_refCharacter);

			    SetEnergyToCharacter(xi_refCharacter);
			    // boal оптимизация скилов <--
				OnShipScrollChange();
			}
			else
			{
				shipIndex= -1;
				bEmptySlot = true;
			}
			SetButtionsAccess();
		}
	}
}

void FillShipParam(ref _chr)
{
    int iShip = sti(_chr.ship.type);
	if (iShip != SHIP_NOTUSED)
	{
		ref refBaseShip = GetRealShip(iShip);
		string sShip = refBaseShip.BaseName;
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\ships\" + sShip + ".tga");

		GameInterface.edit_box.str = _chr.ship.name;
		SetFormatedText("SHIP_RANK", refBaseShip.Class);
		SetShipOTHERTable("TABLE_OTHER", _chr);
		SetFormatedText("FRAME_INFO_CAPTION", XI_ConvertString(sShip));
		SetFormatedText("INFO_TEXT", GetConvertStr(sShip, "ShipsDescribe.txt"));
	}
	else
	{
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\blank_ship2.tga");
		GameInterface.edit_box.str = XI_Convertstring("NoneBoat");
		SetFormatedText("FRAME_INFO_CAPTION","");
		SetFormatedText("INFO_TEXT","");
	}
	Table_UpdateWindow("TABLE_OTHER");
}

void OnShipScrollChange()
{
	SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\128\face_" + xi_refCharacter.FaceId + ".tga");
	SetFormatedText("HERO_NAME", GetFullName(xi_refCharacter));
	SetFormatedText("HERO_RANK",xi_refCharacter.Rank);
	// нулим все формы, тк корабля может не быть
	SetFormatedText("SHIP_RANK","");
	Table_Clear("TABLE_OTHER", false, true, false);
	//Ship info window
	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(pchar.Money), MONEY_SIGN,MONEY_DELIVER));
    FillShipParam(xi_refCharacter);
}

void confirmShipChangeName()
{
	/*
	if (shipIndex== -1) return;
	xi_refcharacter.ship.name = GameInterface.edit_box.str;
	GameInterface.edit_box.str = xi_refcharacter.ship.name;
	SetCurrentNode("SHIP_INFO_TEXT");   */
}

void CheckForRename()
{
	/*if (GetShipRemovable(xi_refCharacter) == true && shipIndex!= -1)
	{
		SetCurrentNode("EDIT_BOX");
	}    */
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	sPicture = "-1";
	string sAttributeName;
	int nChooseNum = -1;
	int iShip;
	ref refBaseShip;

	bool  bShowHint = true;
	
	ref rChr;
	if (bShipyardOnTop)
	{
	    rChr = refNPCShipyard;
	}
	else
	{
		rChr = xi_refCharacter;
	}
	switch (sCurrentNode)
	{
		case "SHIP_BIG_PICTURE":
			if (shipIndex != -1 || bShipyardOnTop)
			{
			    iShip = sti(rChr.ship.type);
			    refBaseShip = GetRealShip(iShip);
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat2", "ShipsDescribe.txt");
			}
		break;

		case "SHIPS_SCROLL":
			if (shipIndex != -1)
			{
			    iShip = sti(xi_refCharacter.ship.type);
			    refBaseShip = GetRealShip(iShip);
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
				//sText2 = GetRPGText("shipChoose_hint");
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat2", "ShipsDescribe.txt");
			}
		break;

		case "MAIN_CHARACTER_PICTURE":
			// отдельная форма
			bShowHint = false;
			ShowRPGHint();
		break;

		case "TABLE_OTHER":
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID, "ShipsDescribe.txt");
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(rChr.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
		    {
		    	ref Cannon = GetCannonByType(sti(rChr.Ship.Cannons.Type));
		    	sText2 = "Тип: " + XI_ConvertString(GetCannonType(sti(rChr.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Калибр: " + XI_ConvertString("caliber" + GetCannonCaliber(sti(rChr.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Дальность: "  + sti(Cannon.FireRange);
		    	sText2 = sText2 + NewStr() + "Урон: x"  + FloatToString(stf(Cannon.DamageMultiply), 1);
		    	sText2 = sText2 + NewStr() + "Перезарядка: "  + sti(GetCannonReloadTime(Cannon)) + " сек.";
		    	sText2 = sText2 + NewStr() + "Вес: "  + sti(Cannon.Weight) + " ц.";

		    	sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(rChr.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(rChr.Ship.Cannons.Type));

				sText3 = "Двойной клик или Enter по этому полю вызывает просмотр экипировки орудий по бортам.";
		    }
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "Crew" && sti(rChr.ship.type) != SHIP_NOTUSED)
			{
				sText2 = "Команды может быть больше максимальной, но это вызывает перегруз и условия жизни на корабле становятся ужасными, что влечет ежедневное падение морали. В среднем можно взять на борт до 25% больше матросов.";
				sText2 = sText2 + NewStr() + "Максимальное количество экипажа с учетом перегруза: " + GetMaxCrewQuantity(rChr);
			}
		break;
		case "TABLE_SHIPYARD" :
		    sHeader = XI_Convertstring("Shipyard");
			sText1  = GetConvertStr("Shipyard_hint", "ShipsDescribe.txt");
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	}
}

void HideInfoWindow()
{
	CloseTooltip();
	ExitRPGHint();
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurTable = sControl;
    CurRow   =  "tr" + (iSelected);
    NullSelectTable("TABLE_OTHER");
    // не тереть скрол на верфи
    
    // заполнялка
    if (CurTable == "TABLE_SHIPYARD")
	{
        FillShipyardShip(refNPCShipyard, GameInterface.(CurTable).(CurRow).sShipId);
		FillShipParam(refNPCShipyard);
		bShipyardOnTop = true;
		SetButtionsAccess();
	}
}

void ExitRPGHint()
{
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("RPG_WINDOW", false);
		XI_WindowDisable("RPG_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		sMessageMode = "";
	}
}

void ShowRPGHint()
{
	SetSPECIALMiniTable("RPG_TABLE_SMALLSKILL", xi_refCharacter);
    SetOTHERMiniTable("RPG_TABLE_SMALLOTHER", xi_refCharacter);
    SetFormatedText("RPG_OFFICER_NAME", GetFullName(xi_refCharacter));

	XI_WindowShow("RPG_WINDOW", true);
	XI_WindowDisable("RPG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "RPG_Hint";
}


void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
	    GameInterface.(sControl).select = 0;
	    Table_UpdateWindow(sControl);
	}
}

void ExitMsgMenu()
{
	XI_WindowShow("MSG_WINDOW", false);
	XI_WindowDisable("MSG_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
	sMessageMode = "";
}

void ShowMsgMenu()
{
	XI_WindowShow("MSG_WINDOW", true);
	XI_WindowDisable("MSG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("MSG_CANCEL");
}

void ShowOtherClick()
{
	if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType")
	{
		ShowCannonsMenu();
	}
}

void ShowCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", true);
	XI_WindowDisable("CANNONS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("CANNONS_CANCEL");

    DeleteAttribute(&GameInterface, "CANNONS_TABLE.BackUp")
    CannonsMenuRefresh();
}
void CannonsMenuRefresh()
{
	ref rChr;
	if (bShipyardOnTop)
	{
	    rChr = refNPCShipyard;
	}
	else
	{
		rChr = xi_refCharacter;
	}
	int idx = GetCannonGoodsIdxByType(sti(rChr.Ship.Cannons.Type));
	if (idx != -1)
	{
	    SetNewGroupPicture("CANNONS_PIC", "GOODS", Goods[idx].Name);
		SetFormatedText("CANNONS_TEXT", XI_ConvertString(Goods[idx].Name));

		SetFormatedText("CANNONS_QTY_F", its(GetBortCannonsQty(rChr, "cannonf")));
		SetFormatedText("CANNONS_QTY_B", its(GetBortCannonsQty(rChr, "cannonb")));
		SetFormatedText("CANNONS_QTY_R", its(GetBortCannonsQty(rChr, "cannonr")));
		SetFormatedText("CANNONS_QTY_L", its(GetBortCannonsQty(rChr, "cannonl")));
		/// всего GetCannonsNum(xi_refCharacter)
	}
	else
	{
		SetNewGroupPicture("CANNONS_PIC", "GOODS", "");
		SetFormatedText("CANNONS_TEXT", "");
		SetFormatedText("CANNONS_QTY_F", "0");
		SetFormatedText("CANNONS_QTY_B", "0");
		SetFormatedText("CANNONS_QTY_R", "0");
		SetFormatedText("CANNONS_QTY_L", "0");
	}
}
void ExitCannonsMenu()
{
	HideCannonsMenu();
}

void HideCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", false);
	XI_WindowDisable("CANNONS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

////////////// ценообразование

int GetShipPriceByType(int st)
{
	if(st==SHIP_NOTUSED) return 0;
	ref shref = GetRealShip(st);

	return makeint(shipCostRate * sti(shref.Price));
}

int GetBuyPrice(int iType)
{
	// boal учет скилов торговли 22.01.2004 -->
	float nCommerce   = GetSummonSkillFromNameToOld(GetMainCharacter(), SKILL_COMMERCE);

    if(CheckOfficersPerk(pchar,"AdvancedCommerce"))	{ nCommerce += 4; }
	else
	{
		if(CheckOfficersPerk(pchar,"BasicCommerce"))	{ nCommerce += 2; }
	}
		
    return makeint(GetShipPriceByType(iType) + GetShipPriceByType(iType)/(nCommerce*10));
    // boal 22.01.2004 <--
}

int GetSellPrice(ref _chr)
{
	int st = GetCharacterShipType(_chr);
	int price = GetShipPriceByType(st);
	price = makeint(price - 1.5*GetSailRepairCost(st, GetSailDamagePercent(_chr)));
	price = makeint(price - 1.5*GetHullRepairCost(st, GetHullDamagePercent(_chr)));

	float nCommerce   = GetSummonSkillFromNameToOld(GetMainCharacter(), SKILL_COMMERCE) + 0.001;
	
	if(CheckOfficersPerk(pchar,"AdvancedCommerce"))	{ nCommerce += 4; }
	else
	{
		if(CheckOfficersPerk(pchar,"BasicCommerce"))	{ nCommerce += 2; }
	}
	
	price = price - price / (nCommerce*10.5);
	
	ref rRealShip = GetRealShip(st);
	
	if (sti(rRealShip.Stolen) == true) //проверка на ворованный
	{
    	price = makeint(price/3);
    }
    if (price < 0 && sti(_chr.Ship.Type) != SHIP_NOTUSED)
	{
	   price = 0;
	}

	return price;
}

int GetSailDamagePercent(ref _chr)
{
	return 100 - MakeInt(GetSailPercent(_chr));
}

int GetHullDamagePercent(ref _chr)
{
	return 100 - MakeInt(GetHullPercent(_chr));
}

int GetSailRepairCost(int shipType, int repairPercent)
{
	int shipPrice = GetShipPriceByType(shipType);
	if(shipPrice<=0) return 0;
	int scost = (shipPrice*sailCostPercent+99)/100;
	return (scost*repairPercent+99)/100;
}

int GetHullRepairCost(int shipType,int repairPercent)
{
	int shipPrice = GetShipPriceByType(shipType);
	if(shipPrice<=0) return 0;
	int scost = (shipPrice*hullCostPercent+99)/100;
	return (scost*repairPercent+99)/100;
}

void FillShipyardTable()
{
    Table_Clear("TABLE_SHIPYARD", false, true, false);
    
	GameInterface.TABLE_SHIPYARD.hr.td1.str = "Корабль";
	GameInterface.TABLE_SHIPYARD.hr.td1.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td2.str = "Класс";
	GameInterface.TABLE_SHIPYARD.hr.td2.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td3.str = "Пушки";
	GameInterface.TABLE_SHIPYARD.hr.td3.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td4.str = "Трюм";
	GameInterface.TABLE_SHIPYARD.hr.td4.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td5.str = "Цена";
	GameInterface.TABLE_SHIPYARD.hr.td5.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.select = 0;
	GameInterface.TABLE_SHIPYARD.top = 0;
	
	aref   arDest, arImt;
	string sAttr;
	int    iNum, i, iShip;
	ref    refBaseShip;
	string sShip;
	string row;
	
	makearef(arDest, refNPCShipyard.shipyard);
	iNum = GetAttributesNum(arDest);
	for (i = 0; i < iNum; i++)
	{
        row = "tr" + (i+1);
        
		arImt = GetAttributeN(arDest, i);
		//Log_Info(GetAttributeName(arImt));
		sAttr = GetAttributeName(arImt);
    	FillShipyardShip(refNPCShipyard, sAttr); // скинуть в стандарт корабль из ШипХХ
    	
    	iShip = sti(refNPCShipyard.Ship.Type);
    	refBaseShip = GetRealShip(iShip);
		sShip = refBaseShip.BaseName;
		//
		GameInterface.TABLE_SHIPYARD.(row).sShipId = sAttr;
        GameInterface.TABLE_SHIPYARD.(row).td1.icon.texture = "interfaces\ships\" + sShip + ".tga";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.uv = "0,0,1,1";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.width = 46;
    	GameInterface.TABLE_SHIPYARD.(row).td1.icon.height = 46;
    	GameInterface.TABLE_SHIPYARD.(row).td1.icon.offset = "0, 1";
    	GameInterface.TABLE_SHIPYARD.(row).td1.textoffset = "53,0";
		GameInterface.TABLE_SHIPYARD.(row).td1.str = XI_Convertstring(sShip) + "\n\n"+refNPCShipyard.ship.name;
		GameInterface.TABLE_SHIPYARD.(row).td1.align = "left";
		GameInterface.TABLE_SHIPYARD.(row).td1.scale = 0.82;
		GameInterface.TABLE_SHIPYARD.(row).td2.str = refBaseShip.Class;
		GameInterface.TABLE_SHIPYARD.(row).td3.str = sti(refBaseShip.CannonsQuantity);
		GameInterface.TABLE_SHIPYARD.(row).td4.str = GetCargoMaxSpace(refNPCShipyard);
		GameInterface.TABLE_SHIPYARD.(row).td5.str = GetBuyPrice(iShip);
		GameInterface.TABLE_SHIPYARD.(row).td5.color = argb(255,255,228,80);
    }
	    
	Table_UpdateWindow("TABLE_SHIPYARD");
}

void SetButtionsAccess()
{
    SetSelectable("BUTTON_REPAIR", false);
    SetSelectable("BUTTON_SELL", true);
    SetSelectable("BUTTON_BUY", true);
    
    if (bShipyardOnTop)
    {
        SetSelectable("BUTTON_REPAIR", false);
    	SetSelectable("BUTTON_SELL", false);
    	if (shipIndex == -1)// проверка на цену
    	{
    	    if (GetBuyPrice(sti(refNPCShipyard.Ship.Type)) > sti(pchar.Money) )
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
   			if (bEmptySlot && sti(pchar.ship.type) == SHIP_NOTUSED)
   			{ // нельзя купить корабль компаньону, если у ГГ нету
   			    SetSelectable("BUTTON_BUY", false);
   			}
    	}
    	else
    	{
    	    if (GetBuyPrice(sti(refNPCShipyard.Ship.Type)) > (GetSellPrice(xi_refCharacter) + sti(pchar.Money)) )
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
    	    if (!GetRemovable(xi_refCharacter) || !GetShipRemovable(xi_refCharacter))
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
    	}
    }
    else
    {
        SetSelectable("BUTTON_BUY", false);

        if (GetHullPercent(xi_refCharacter) < 100 || GetSailPercent(xi_refCharacter) < 100)
        {
            SetSelectable("BUTTON_REPAIR", true);
        }
        if (shipIndex == -1)
        {
            SetSelectable("BUTTON_SELL", false);
            SetSelectable("BUTTON_REPAIR", false);
        }
        else
        {
            if (!GetRemovable(xi_refCharacter) || !GetShipRemovable(xi_refCharacter))
    	    {
    	        SetSelectable("BUTTON_SELL", false);
    	    }
    	    if (xi_refCharacter.id == pchar.id && GetCompanionQuantity(pchar) > 1)
    	    { // нельзя продать корабль ГГ, если есть еще компаньоны
    	        SetSelectable("BUTTON_SELL", false);
    	    }
		}
    }
}

//////////////// назначение капитана  //////////////////////////////////////////////////////////////////////
void ShipChangeCaptan()
{
	FillPassengerScroll();
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
    SetCurrentNode("PASSENGERSLIST");
	ProcessFrame();
	SetOfficersSkills();

	XI_WindowShow("OFFICERS_WINDOW", true);
	XI_WindowDisable("OFFICERS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
}

void ExitOfficerMenu()
{
	XI_WindowShow("OFFICERS_WINDOW", false);
	XI_WindowDisable("OFFICERS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
}

void AcceptAddOfficer()
{
	string  attributeName2 = "pic"+(nCurScrollOfficerNum+1);
    ref     sld;

    if (checkAttribute(GameInterface, "PASSENGERSLIST."+attributeName2 + ".character"))
    {
		int iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);
        // назначение нового кэпа, возможно, если там уже не наш, те или враг или снят
        sld = GetCharacter(iChar);
		DeleteAttribute(sld, "ship");
		sld.ship.Type = SHIP_NOTUSED;

		// снимем пассажира -->
		CheckForReleaseOfficer(iChar);
		RemovePassenger(pchar, sld);
		// снимем пассажира <--
		SetCompanionIndex(pchar, -1, iChar);

		xi_refCharacter = sld;
		DoBuyShip();
	}
	ExitOfficerMenu();
}

void SetOfficersSkills()
{
	string sCharacter = "pic"+(sti(GameInterface.PASSENGERSLIST.current)+1);
	if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter + ".character"))
		{
			sCharacter = GameInterface.PASSENGERSLIST.(sCharacter).character;
			ref otherchr = &characters[sti(sCharacter)];
	        SetSPECIALMiniTable("TABLE_SMALLSKILL", otherchr);
	        SetOTHERMiniTable("TABLE_SMALLOTHER", otherchr);
	        SetFormatedText("OFFICER_NAME", GetFullName(otherchr));
	        SetSelectable("ACCEPT_ADD_OFFICER", true);
	        return;
        }
	}
    Table_Clear("TABLE_SMALLSKILL", false, true, true);
    Table_Clear("TABLE_SMALLOTHER", false, true, true);
    SetFormatedText("OFFICER_NAME", "");
    SetSelectable("ACCEPT_ADD_OFFICER", false);
}

void FillPassengerScroll()
{
	int i, howWork;
	string faceName;
	string attributeName;
	int _curCharIdx;
	ref _refCurChar;
	bool  ok;

	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollOfficerNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;

	int nListSize = GetPassengersQuantity(pchar);
	int nListSizeFree = nListSize;

	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ListSize = nListSizeFree + 2;

	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";

	FillFaceList("PASSENGERSLIST.ImagesGroup", pchar, 2); // passengers

	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";
	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		_curCharIdx = GetPassenger(pchar,i);

		if (_curCharIdx!=-1)
		{
			ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
			if (!ok && GetRemovable(&characters[_curCharIdx]))
			{
				GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
				GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup", "FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
			}
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m + 2; // не знаю зачем, но для совместимости с "было"
}
// рефреш скилов офа и ГГ, если офа сняли для компаньона
void DelBakSkill()
{
	DelBakSkillAttr(xi_refCharacter);
    ClearCharacterExpRate(xi_refCharacter);
    RefreshCharacterSkillExpRate(xi_refCharacter);
    SetEnergyToCharacter(xi_refCharacter);

    DelBakSkillAttr(pchar);
    ClearCharacterExpRate(pchar);
    RefreshCharacterSkillExpRate(pchar);
    SetEnergyToCharacter(pchar);
}

void ShowMessageInfo()
{
	bool   bBuy;
	int    iMoney;
	string add = "";
	
	if (bShipyardOnTop) // на стороне верфи (купить)
    {
        bBuy = true;
		if (shipIndex == -1)// проверка на цену
    	{
    	    iMoney = GetBuyPrice(sti(refNPCShipyard.Ship.Type));
    	}
    	else
    	{
            iMoney = GetBuyPrice(sti(refNPCShipyard.Ship.Type)) - GetSellPrice(xi_refCharacter);
    	}
    }
    else  // продать
    {
        bBuy = false;
        iMoney = GetSellPrice(xi_refCharacter);
    }
	SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("Ship"));
	
	if (bBuy)
	{
		if (iMoney < 0) add = "-";
		SetFormatedText("MSG_WINDOW_TEXT", "Купить корабль за " + add + FindRussianMoneyString(abs(iMoney)) + "?");
		SetSelectable("MSG_OK", true);
		sMessageMode = "ShipBuy";
	}
	else
	{
	    SetFormatedText("MSG_WINDOW_TEXT", "Продать корабль за " + add + FindRussianMoneyString(abs(iMoney)) + "?");
		SetSelectable("MSG_OK", true);
		sMessageMode = "ShipSell";
	}
	ShowMsgMenu();
}

void MessageOk()
{
	switch (sMessageMode)
	{
	    case "ShipSell" :
	        DoSellShip(true);
	        ExitMsgMenu();
	    break;
	    
	    case "ShipBuy" :
	        DoBuyShipMain();
	    break;
	}
}

void DoSellShip(bool _refresh)
{
	ref chref = xi_refCharacter;
	if (shipIndex != -1 && sti(chref.ship.type) != SHIP_NOTUSED)
	{
    	int sellPrice = GetSellPrice(chref);

    	AddMoneyToCharacter(pchar, sellPrice);
        AddCharacterExpToSkill(pchar, "Commerce", sellPrice / 1200);
        Statistic_AddValue(pchar, "SellShip", 1);

    	DeleteAttribute(chref,"ship.sails");
    	DeleteAttribute(chref,"ship.blots");
        DeleteAttribute(chref,"ship.masts");

        // в списке на покупку. -->
        AddShip2Shipyard(chref);
        // в списке на покупку. <--
    	chref.ship.type = SHIP_NOTUSED;
    	// снять кэпа -->
    	if (_refresh)
		{
	    	if (sti(chref.index) != GetMainCharacterIndex())
	    	{
	            RemoveCharacterCompanion(pchar, chref);
				AddPassenger(pchar, chref, false);
				DelBakSkill();
	    	}
	    	else
	    	{
          		pchar.location.from_sea = ""; // нет корабля в порту
	    	}
    		RefreshShipLists();
    	}
	}
}

void RefreshShipLists()
{
    xi_refCharacter = pchar;
	FillShipsScroll();
   	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
    SetCurrentNode("SHIPS_SCROLL");
	ProcessFrame();
	
	FillShipyardTable();
	bShipyardOnTop = false;
}

void AddShip2Shipyard(ref _chr)
{
    aref    arTo, arFrom;
    
	aref   arDest, arImt;
	string sAttr;
	int    iNum, i, iShip;
	ref    refBaseShip;
	string sShip;
	string row;

    iShip = sti(_chr.ship.type);
    RealShips[iShip].StoreShip = true;  // кораль на верфи, трется отдельным методом
    RealShips[iShip].Stolen = 0; // уже не ворованный
	makearef(arDest, refNPCShipyard.shipyard);
	iNum = GetAttributesNum(arDest);
	if (iNum == 0)
	{
	    sAttr = "ship1";
	}
	else
	{
	    arImt = GetAttributeN(arDest, iNum - 1);
	    sAttr = GetAttributeName(arImt) + "2";
	}
	makearef(arTo,   refNPCShipyard.ship);
	makearef(arFrom, _chr.Ship);
	CopyAttributes(arTo, arFrom);
	// нулим экипаж и трюм
	SetCrewQuantity(refNPCShipyard, 0);
	DeleteAttribute(refNPCShipyard, "Ship.Cargo");  //пустой трюм
    RecalculateCargoLoad(refNPCShipyard);
    
	FillShipyardShipBack(refNPCShipyard, sAttr);
	// бакапим атрибуты груза и матросов
	DeleteAttribute(refNPCShipyard, "BakCargo");
 	DeleteAttribute(refNPCShipyard, "BakCrew");
 	refNPCShipyard.BakCargo = "";
	makearef(arTo,   refNPCShipyard.BakCargo);
	makearef(arFrom, _chr.Ship.Cargo);
	CopyAttributes(arTo, arFrom);
	
	refNPCShipyard.BakCrew = "";
	makearef(arTo,   refNPCShipyard.BakCrew);
	makearef(arFrom, _chr.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void DoBuyShip()
{
    aref    arTo, arFrom;
	int     iBuyMoney = GetBuyPrice(sti(refNPCShipyard.Ship.Type));
	
 	DeleteAttribute(refNPCShipyard, "BakCargo");
 	DeleteAttribute(refNPCShipyard, "BakCrew");
	DoSellShip(false);
	AddMoneyToCharacter(pchar, -iBuyMoney);

    AddCharacterExpToSkill(pchar, "Commerce", iBuyMoney / 450);
    Statistic_AddValue(pchar, "BuyShip", 1);

    FillShipyardShip(refNPCShipyard, sShipId); // то, что покупаем

    makearef(arTo,   xi_refCharacter.ship);
	makearef(arFrom, refNPCShipyard.Ship);
	CopyAttributes(arTo, arFrom);

	DeleteAttribute(refNPCShipyard, "shipyard." + sShipId);
	
    int iShip = sti(xi_refCharacter.ship.type);
    DeleteAttribute(&RealShips[iShip], "StoreShip"); // можно тереть
    
	if (CheckAttribute(refNPCShipyard, "BakCargo")) // есть бакап корабля
	{
        DeleteAttribute(xi_refCharacter, "Ship.Cargo");
		DeleteAttribute(xi_refCharacter, "Ship.Crew");

		xi_refCharacter.Ship.Cargo = "";
		makearef(arTo,   xi_refCharacter.Ship.Cargo);
		makearef(arFrom, refNPCShipyard.BakCargo);
		CopyAttributes(arTo, arFrom);

        xi_refCharacter.Ship.Crew = "";
		makearef(arTo,   xi_refCharacter.Ship.Crew);
		makearef(arFrom, refNPCShipyard.BakCrew);
		CopyAttributes(arTo, arFrom);

		AddCharacterCrew(xi_refCharacter, 0); // обрезать перегруз
	    RecalculateCargoLoad(xi_refCharacter);
	}
	if (sti(xi_refCharacter.index) == GetMainCharacterIndex())
	{
  		pchar.location.from_sea = sFrom_sea; // корабль в порту
	}
	DelBakSkill();
	RefreshShipLists();
}

void DoBuyShipMain()
{
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".sShipId"))
	{
	    sShipId = GameInterface.(CurTable).(CurRow).sShipId;

		if (!bEmptySlot) // был коарбль или ГГ без корабля
		{
		    DoBuyShip();
		    ExitMsgMenu();
		}
		else
		{
		    ExitMsgMenu();
		    ShipChangeCaptan();
		}
	}
}