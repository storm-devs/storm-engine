////    boal 30.07.06 найм матросов
int	nCurScrollNum = 0;
ref refCharacter;
ref refTown;

int  BuyOrSell = 0; // 1-buy -1 sell
int  iPriceSailor;
void InitInterface(string iniName)
{
 	StartAboveForm(true);

	refCharacter = pchar;

	// город, где √√
	refTown = GetColonyByIndex(FindColony(loadedLocation.fastreload));
    FillShipsScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	CreateString(true,"ShipName","",FONT_NORMAL,COLOR_MONEY, 405,165,SCRIPT_ALIGN_CENTER,1.0);

	SetDescription();

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);

	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("frame","ProcessFrame",1);
	
	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("TransactionCancel", "TransactionCancel", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);
	
	SetNewGroupPicture("CREW_PICTURE", "SHIP_STATE_ICONS", "Crew");
	SetNewGroupPicture("CREW_PICTURE2", "SHIP_STATE_ICONS", "Crew");
	SetBackupQty();
	SetCurrentNode("SHIPS_SCROLL");
	GameInterface.qty_edit.str = 0;
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);
    GetBackupQty(); // если неожиданно вышли
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("frame","ProcessFrame");
	
	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("TransactionCancel", "TransactionCancel");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");
	
	interfaceResultCommand = exitCode;
	EndCancelInterface(true);

}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "QTY_OK_BUTTON":
			if(comName=="leftstep")
			{
	            ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            REMOVE_ALL_BUTTON();
			}
		break;

		case "QTY_CANCEL_BUTTON":
			if(comName=="leftstep")
			{
	            ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            REMOVE_ALL_BUTTON();
			}
		break;	
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void FillShipsScroll()
{
	nCurScrollNum = -1;
	FillScrollImageWithCompanionShips("SHIPS_SCROLL", 5);

	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}
}

void SetVariable()
{
	string sText, sTextSecond;
	int iColor;
	int nShipType = sti(refCharacter.ship.type);
	
	if (nShipType == SHIP_NOTUSED)
	{
        GameInterface.strings.shipname = "";
		return;
	}
	
	ref refBaseShip = GetRealShip(nShipType);
	
    SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\256\face_" + its(refCharacter.FaceId) + ".tga");
    
	SetFormatedText("CAPACITY", GetFullName(refCharacter));

	sText = XI_ConvertString("OurMoney") + NewStr() + FindRussianMoneyString(sti(pchar.money));
	SetFormatedText("OUR_GOLD", sText);

	if (CheckAttribute(refCharacter, "ship.name"))
	{
		GameInterface.strings.shipname = refCharacter.ship.name;
	}
	else
	{
	    GameInterface.strings.shipname = "";
	}
	SetCrewExpTable(refCharacter, "TABLE_CREW", "BAR_Sailors", "BAR_Cannoners", "BAR_Soldiers");
	
	SetFormatedText("CREW_QTY", ""+GetCrewQuantity(refCharacter));
	if (GetCrewQuantity(refCharacter) > GetOptCrewQuantity(refCharacter) || GetCrewQuantity(refCharacter) < GetMinCrewQuantity(refCharacter))
	{
		iColor = argb(255,255,64,64);
	}
	else
	{
		iColor = argb(255,255,255,255);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREW_QTY", 8,-1,iColor);
	SetNewGroupPicture("CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(stf(refCharacter.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refCharacter.Ship.crew.morale))));
	
	// на одном корабле
	SetFoodShipInfo(refCharacter, "FOOD_SHIP");
	
	SetFormatedText("INFO_SHIP", XI_ConvertString(refBaseShip.BaseName) + ", класс " + refBaseShip.Class +", команда: мин. "+GetMinCrewQuantity(refCharacter) + ", макс. " + GetOptCrewQuantity(refCharacter));
	SetFormatedText("MONEY_SHIP", "—одержание корабл€: " + NewStr() + FindRussianMoneyString(GetSalaryForShip(refCharacter)));
	////  заполн€лка города
	SetCrewExpTable(refTown, "TABLE_CREW2", "BAR_Sailors2", "BAR_Cannoners2", "BAR_Soldiers2");
	
	SetFormatedText("CREW_QTY2", ""+GetCrewQuantity(refTown));
	SetNewGroupPicture("CREW_MORALE_PIC2", "MORALE_SMALL", GetMoraleGroupPicture(stf(refTown.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT2", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refTown.Ship.crew.morale))));
	iPriceSailor = GetCrewPriceForTavern(refTown.id);
	SetFormatedText("TAVERN_PRICE", "—тоимость найма одного матроса " + FindRussianMoneyString(iPriceSailor));
}

void ProcessFrame()
{
	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		if(sti(GameInterface.SHIPS_SCROLL.current)!=nCurScrollNum)
		{
			GetBackupQty(); // если неожиданно вышли
			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);
			SetDescription();
			GameInterface.qty_edit.str = 0;
			SetFormatedText("QTY_TypeOperation", "");
		    SetFormatedText("QTY_Result", "");
		}
	}
}

void SetDescription()
{
 	string sChrId;
 	 
	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		string attributeName = attributeName = "pic" + (nCurScrollNum+1);
		if(CheckAttribute(&GameInterface, "SHIPS_SCROLL." + attributeName))
		{
			int iCharacter = GameInterface.SHIPS_SCROLL.(attributeName).companionIndex;
			sChrId = characters[iCharacter].id;
			refCharacter = characterFromID(sChrId);
			SetBackupQty();
			if (GetRemovable(refCharacter))
			{
			    SetSelectable("QTY_OK_BUTTON", true);
			}
			else
			{
			    SetSelectable("QTY_OK_BUTTON", false);
			}
		}
	}
	SetVariable();
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
	switch (sCurrentNode)
	{
		case "TABLE_CREW":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break; 
		case "TABLE_CREW2":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
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
}
// бакап значений, до применени€
void SetBackupQty()
{
	aref    arTo, arFrom;
	NullCharacter.TavernBak.Hero = "";
	NullCharacter.TavernBak.Tavern = "";
	
	makearef(arTo,   NullCharacter.TavernBak.Hero);
	makearef(arFrom, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);
	
	makearef(arTo,   NullCharacter.TavernBak.Tavern);
	makearef(arFrom, refTown.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void GetBackupQty()
{
	aref    arTo, arFrom;
	
	makearef(arFrom,   NullCharacter.TavernBak.Hero);
	makearef(arTo, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);
	
	makearef(arFrom,   NullCharacter.TavernBak.Tavern);
	makearef(arTo, refTown.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void TransactionCancel()
{
	if (sti(GameInterface.qty_edit.str) == 0)
	{   // выход
		ProcessCancelExit();	
	}
	else
	{
		CancelQty();	
    }
}

void CancelQty()
{
	GetBackupQty();	
	SetVariable();
	GameInterface.qty_edit.str = 0;
	SetFormatedText("QTY_TypeOperation", "");
    SetFormatedText("QTY_Result", "");
}
void TransactionOK()
{
	int nTradeQuantity, moneyback;
	confirmChangeQTY_EDIT();
	nTradeQuantity = sti(GameInterface.qty_edit.str);
	if (BuyOrSell == 0)
	{
	    CancelQty();
		return;
	}

    if (!GetRemovable(refCharacter)) return;
    
 	if (BuyOrSell == 1) // BUY  нан€ть
	{
		moneyback = makeint(iPriceSailor*stf(GameInterface.qty_edit.str));
		pchar.money = sti(pchar.money)  - moneyback;
	}
 	else
	{ // SELL
		//moneyback = makeint(iPriceSailor*stf(GameInterface.qty_edit.str));
  		//pchar.money = sti(pchar.money)  + moneyback;
	}
	SetBackupQty(); // применим и согласимс€
	CancelQty();
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
    SetCurrentNode("QTY_OK_BUTTON");
}

void ChangeQTY_EDIT()
{
	float fQty;
	GameInterface.qty_edit.str = sti(GameInterface.qty_edit.str); // приведение к целому
	GetBackupQty();	 // обновим как было до правок
	if (sti(GameInterface.qty_edit.str) == 0)
	{
	    SetFormatedText("QTY_TypeOperation", "");
	    SetFormatedText("QTY_Result", "");
	    BuyOrSell = 0;
	}
	else
	{
		if (sti(GameInterface.qty_edit.str) < 0 || BuyOrSell == -1)
		{  // уволить
			if (BuyOrSell != -1)
			{
		    	GameInterface.qty_edit.str = -sti(GameInterface.qty_edit.str);
		    }
            BuyOrSell = -1;
		    // проверка на колво доступное -->
		    if (sti(GameInterface.qty_edit.str) > GetCrewQuantity(refCharacter))
		    {
		        GameInterface.qty_edit.str = GetCrewQuantity(refCharacter);
		    }
		    // проверка на колво доступное <--
		    SetFormatedText("QTY_TypeOperation", "”волить");
		    SetFormatedText("QTY_Result", "");
		}
		else
		{  // нан€ть
			BuyOrSell = 1;
         	// проверка на колво доступное -->
		    if (sti(GameInterface.qty_edit.str) > GetCrewQuantity(refTown))
		    {
		        GameInterface.qty_edit.str = GetCrewQuantity(refTown);
		    }
		    if (sti(GameInterface.qty_edit.str) > (GetMaxCrewQuantity(refCharacter) -  GetCrewQuantity(refCharacter)))
		    {
		        GameInterface.qty_edit.str = (GetMaxCrewQuantity(refCharacter) -  GetCrewQuantity(refCharacter));
		    }
		    
		    if (makeint(iPriceSailor*stf(GameInterface.qty_edit.str)) > sti(pchar.money))
		    {
		        GameInterface.qty_edit.str = makeint(sti(pchar.money) / iPriceSailor);
		    }
		    // проверка на колво доступное <--

			SetFormatedText("QTY_TypeOperation", "Ќан€ть");
			SetFormatedText("QTY_Result", "—тоимость найма " + makeint(iPriceSailor*stf(GameInterface.qty_edit.str)));
		}
		// если получили ноль
		if (sti(GameInterface.qty_edit.str) == 0)
		{
		    SetFormatedText("QTY_TypeOperation", "");
		    SetFormatedText("QTY_Result", "");
		    BuyOrSell = 0;
		}
	}
	if (sti(GameInterface.qty_edit.str) > 0)
	{ // применение кол-ва
		
		if (BuyOrSell == 1)
		{   // найм мен€ет опыт и мораль корабл€
			fQty = stf(GetCrewQuantity(refCharacter) + sti(GameInterface.qty_edit.str));
			refCharacter.Ship.Crew.Exp.Sailors   = (stf(refCharacter.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refCharacter) + 
			                                        stf(refTown.Ship.Crew.Exp.Sailors)*sti(GameInterface.qty_edit.str)) / fQty; 
			refCharacter.Ship.Crew.Exp.Cannoners   = (stf(refCharacter.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refCharacter) + 
			                                        stf(refTown.Ship.Crew.Exp.Cannoners)*sti(GameInterface.qty_edit.str)) / fQty;
			refCharacter.Ship.Crew.Exp.Soldiers   = (stf(refCharacter.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refCharacter) + 
			                                        stf(refTown.Ship.Crew.Exp.Soldiers)*sti(GameInterface.qty_edit.str)) / fQty;
			refCharacter.Ship.Crew.Morale   = (stf(refCharacter.Ship.Crew.Morale)*GetCrewQuantity(refCharacter) + 
			                                        stf(refTown.Ship.Crew.Morale)*sti(GameInterface.qty_edit.str)) / fQty;																													                                        
		}
		else
		{ // увольнение мен€ет таверну
			fQty = stf(GetCrewQuantity(refTown) + sti(GameInterface.qty_edit.str));
			refTown.Ship.Crew.Exp.Sailors   = (stf(refTown.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refTown) + 
			                                        stf(refCharacter.Ship.Crew.Exp.Sailors)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Exp.Cannoners   = (stf(refTown.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refTown) + 
			                                        stf(refCharacter.Ship.Crew.Exp.Cannoners)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Exp.Soldiers   = (stf(refTown.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refTown) + 
			                                        stf(refCharacter.Ship.Crew.Exp.Soldiers)*sti(GameInterface.qty_edit.str)) / fQty;
			refTown.Ship.Crew.Morale   = (stf(refTown.Ship.Crew.Morale)*GetCrewQuantity(refTown) + 
			                                        stf(refCharacter.Ship.Crew.Morale)*sti(GameInterface.qty_edit.str)) / fQty;
		}
		SetCrewQuantity(refCharacter, GetCrewQuantity(refCharacter) + BuyOrSell*sti(GameInterface.qty_edit.str));
		refTown.Ship.Crew.Quantity = sti(refTown.Ship.Crew.Quantity) - BuyOrSell*sti(GameInterface.qty_edit.str));	
	}
    SetVariable(); // обновим экран
}

void REMOVE_ALL_BUTTON()  // продать все (уволить)
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	GameInterface.qty_edit.str = -GetCrewQuantity(refCharacter);
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // купить все
{
	GetBackupQty();	// вернем все как было
	if (!GetRemovable(refCharacter)) return;
	GameInterface.qty_edit.str = GetCrewQuantity(refTown);
	BuyOrSell = 0;
	ChangeQTY_EDIT();
}

void REMOVE_BUTTON()  // продать
{
	if (!GetRemovable(refCharacter)) return;
	if (BuyOrSell == 0)
    {
        GameInterface.qty_edit.str = -1;
    }
    else
    {
		if (BuyOrSell == -1)
		{
			GameInterface.qty_edit.str = -(sti(GameInterface.qty_edit.str) + 1);
		}
		else
		{
			GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) - 1);
		}
		BuyOrSell = 0;
	}
	ChangeQTY_EDIT();
}

void ADD_BUTTON()  // купить
{
	if (!GetRemovable(refCharacter)) return;
	if (BuyOrSell == 0)
    {
        GameInterface.qty_edit.str = 1;
    }
    else
    {
  		if (BuyOrSell == 1)
		{
			GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + 1);
		}
		else
		{
			GameInterface.qty_edit.str = -(sti(GameInterface.qty_edit.str) - 1);
		}
		BuyOrSell = 0;
	}
	ChangeQTY_EDIT();
}