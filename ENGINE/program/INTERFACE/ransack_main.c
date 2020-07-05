
ref refEnepchararacter;
object oTmpData;

int iGoodIndex = 0;
int iStep = 0;
int iPrevStep = 0;
int iStringsQuantity = 0;
int iCrewAdd = 0;

object GoodsList[GOODS_QUANTITY];

void InitInterface_RS(string iniName,ref enepchar,string captureState)
{
	DeleteAttribute(pchar, "abordage_active");
	DeleteAttribute(enepchar, "ship.upper_model");

	refEnepchararacter = enepchar;

	GameInterface.title = "titleRansack";

	oTmpData.setshipcaptain = -1;
	oTmpData.tooltip = 0;
	oTmpData.Possibility_set_captain = 0;

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetConstantStrings();
	SetShipImagesAndNames();
	SetShipCapacity();
	SetShipSpeedManeuver();
	SetShipUpgrades();

	// main events
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("eGoodGhange","procGoodGhange",0);
	SetEventHandler("TableSelectChange","TableSelectChange",0);
	SetEventHandler("GoodsAct","procGoodsAct",0);
	SetEventHandler("ShipAct","procShipAct",0);
	SetEventHandler("eToolTip","procToolTip",0);
	SetEventHandler("MouseRClickUP","EndTooltip",0);
	SetEventHandler("FTChange","FTChange",0);

	// set captain events
	SetEventHandler("SetCaptainAct","SetCaptainAct",0);
	SetEventHandler("eCrewGhange","procCrewGhange",0);
	SetEventHandler("ChangeSelectScrollImage","ChangeSelectScrollImage",0);

	// get award events
	SetEventHandler("GetAwardOk","EndAwardWindow",0);
	SetEventHandler("ActCaptainSurrender","ActCaptainSurrender",0);
	SetEventHandler("ActCrewSurrender","ActCrewSurrender",0);
	SetEventHandler("ActQuestPassenger","ActQuestPassenger",0);
	SetEventHandler("ActQuestItem","ActQuestItem",0);
	SetEventHandler("eCrewSurrenderGhange","procCrewSurrenderGhange",0);

	SetEventHandler("ConfirmCrewTransfer", "ConfirmCrewTransfer", 0);
	SetEventHandler("CancelCrewTransfer", "CancelCrewTransfer", 0);

	SetEventHandler("ActCrewLeave", "ActCrewLeave", 0);

	oTmpData.captureState = captureState;

	SetClickable("MW_BTN_TRANSFER_OK", false);
	SetClickable("MW_BTN_TRANSFER_CANCEL", false);

	FillGoodsList();

	StartAwardWindow();

	iStep = 0;
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 8, "#"+pchar.ship.name );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 9, "#"+refEnepchararacter.ship.name );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 14, "#"+XI_ConvertString("Take goods")+" "+XI_ConvertString(Goods[sti(GoodsList[iStep].index)].Name)+": 0");
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 15, "#"+XI_ConvertString("Give goods")+" "+XI_ConvertString(Goods[sti(GoodsList[iStep].index)].Name)+": 0" );

	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": 0" );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 3, "#"+XI_ConvertString("Give goods")+": 0" );

	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 1, "#"+XI_ConvertString("Give goods")+": 0" );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": 0" );

	SetFormatedText("MW_HINT_TEXT", XI_ConvertString("CtrlShiftSpeedTransferGoods"));
	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"MW_HINT_TEXT", 5 );

	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"MW_TITLE_GOODS_OUR", 5 );
	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"MW_TITLE_GOODS_OTHER", 5 );
	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"MW_TITLE_GOODS_NAME", 5 );
}

void SetGoodsTakeStrings()
{
	int nGood = sti(GoodsList[iStep].index);
	int nQ = sti(oTmpData.myGoodQnt) - GetCargoGoods(pchar,nGood);

	if( nQ < 0 ) {
		nQ = -nQ;
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 14,
			"#"+XI_ConvertString("Take goods")+" "+XI_ConvertString(Goods[nGood].Name)+": "+nQ );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 15,
			"#"+XI_ConvertString("Give goods")+" "+XI_ConvertString(Goods[nGood].Name)+": 0" );
	} else {
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 14,
			"#"+XI_ConvertString("Take goods")+" "+XI_ConvertString(Goods[nGood].Name)+": 0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 15,
			"#"+XI_ConvertString("Give goods")+" "+XI_ConvertString(Goods[nGood].Name)+": "+nQ );
	}
}

void HideGoods(bool bHide)
{
	if( bHide ) {
		SetNodeUsing("TOP_HIDER",true);
		SetNodeUsing("BOTTOM_HIDER",true);
		SetClickable("MW_BTN_TRANSFER_OK",true);
		SetClickable("MW_BTN_TRANSFER_CANCEL",true);
		SetSelectable("MW_BTN_TAKE_ALL",false);
		SetSelectable("MW_GOODS_SCROLL",false);
		SetSelectable("MW_BTN_SHIP_CHANGE",false);
		SetSelectable("MW_BTN_SHIP_CAPTURE",false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPTRADELIST", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPQUANTITYLIST", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"STOREQUANTITYLIST", 13, true);
	} else {
		SetNodeUsing("TOP_HIDER",false);
		SetNodeUsing("BOTTOM_HIDER",false);
		SetClickable("MW_BTN_TRANSFER_OK",false);
		SetClickable("MW_BTN_TRANSFER_CANCEL",false);
		SetSelectable("MW_BTN_TAKE_ALL",true);
		SetSelectable("MW_GOODS_SCROLL",true);
		SetSelectable("MW_BTN_SHIP_CHANGE",true);
		SetSelectable("MW_BTN_SHIP_CAPTURE",sti(oTmpData.Possibility_set_captain));
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPTRADELIST", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPQUANTITYLIST", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"STOREQUANTITYLIST", 13, false);
	}
}

void procGoodGhange()
{
	int nCommand = GetEventData();
	string sNode = GetEventData();

	int nGood = sti(GoodsList[iStep].index);
	int nQ = 1;
	if( XI_IsKeyPressed("control") ) {nQ = 10;}
	if( XI_IsKeyPressed("alt") ) {nQ=100;}

	if( nCommand == ACTION_LEFTSTEP ) {
		if( XI_IsKeyPressed("shift") ) {nQ = GetCargoGoods(refEnepchararacter,nGood);}
		ActGoodsChange(pchar,refEnepchararacter,nGood,nQ);
	} else {
		if( XI_IsKeyPressed("shift") ) {nQ = GetCargoGoods(pchar,nGood);}
		ActGoodsChange(refEnepchararacter,pchar,nGood,nQ);
	}
}

void ActGoodsChange(ref rIn, ref rOut, int nGoodIdx, int nQnt)
{
	int nOutQ = GetCargoGoods(rOut,nGoodIdx);
	if( nQnt > nOutQ ) {nQnt = nOutQ;}
	int nMaxLimit = GetCharacterFreeSpace(rIn,nGoodIdx);
	if( nQnt > nMaxLimit ) {nQnt = nMaxLimit;}
	if( nQnt<=0 ) {return;}

	RemoveCharacterGoods(rOut,nGoodIdx,nQnt);
	SetCharacterGoods(rIn,nGoodIdx,GetCargoGoods(rIn,nGoodIdx)+nQnt);

	if( sti(oTmpData.myGoodQnt) == GetCargoGoods(pchar,nGoodIdx) &&
		sti(oTmpData.otherGoodQnt) == GetCargoGoods(refEnepchararacter,nGoodIdx) ) 
	{
		HideGoods(false);
	} 
	else 
	{
		HideGoods(true);
	}

	UpdateGoodsList();
	SetShipCapacity();
	SetShipSpeedManeuver();
	SetGoodsTakeStrings();
}

void UpdateGoodsList()
{
	int nGood = sti(GoodsList[iStep].index);
	string sText;

	sText = GetCargoGoods(pchar,nGood);
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"SHIPQUANTITYLIST", 10, iStep, &sText);
	sText = GetCargoGoods(refEnepchararacter,nGood);
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"STOREQUANTITYLIST", 10, iStep, &sText);
}

void TableSelectChange()
{
	string sNode = GetEventData();
	int nLine = GetEventData(); // 1...N
	/*if( sNode=="MW_GOODS_LIST" ) {
		UpdateCounterPos();
		SaveCurrentGoodsData(true);
		return;
	}*/
	//if( sNode=="SC_CREWLIST" ) {
	//	SetCaptainUpdateCounterPos();
	//}
	/*if( sNode=="SHIPCREW_LIST" ) {
		SurrenderCrewUpdateCounterPos();
	}*/
}

void procGoodsAct()
{
	int nCommand = GetEventData();
	string sNode = GetEventData();

	if( sNode == "MW_BTN_TAKE_ALL" ) 
	{
		TakeAllGoods();
		SetGoodsTakeStrings();
		return;
	}

	if( sNode == "MW_BTN_TRANSFER_OK" ) 
	{
		SaveCurrentGoodsData(true);
		HideGoods(false);
		SetGoodsTakeStrings();
		return;
	}
	if( sNode == "MW_BTN_TRANSFER_CANCEL" ) 
	{
		SaveCurrentGoodsData(false);
		UpdateGoodsList();
		SetShipCapacity();
		HideGoods(false);
		SetGoodsTakeStrings();
		return;
	}
}

void TakeAllGoods()
{
	int n,nmax,q;

	// берем товары
	for(n=0; n<GOODS_QUANTITY; n++) {
		nmax = GetSquadronFreeSpace(pchar,n);
		q = GetCargoGoods(refEnepchararacter,n);
		if( q>nmax ) {
			q = nmax;
		}
		RemoveCharacterGoods(refEnepchararacter,n,q);
		AddCharacterGoods(pchar,n,q);
	}

	SetShipCapacity();
	SetShipSpeedManeuver();
	FillGoodsList();
}

void procShipAct()
{
	int nCommand = GetEventData();
	string sNode = GetEventData();

	if( sNode == "MW_BTN_SHIP_CHANGE" ) 
	{
		SwapShips();
		return;
	}
	if( sNode == "MW_BTN_SHIP_CAPTURE" ) 
	{
		ShowCaptainWindow( true );
		SetNewPicture("SC_OURSHIP_IMG", "interfaces\ships\" + ShipsTypes[sti(RealShips[sti(pchar.ship.type)].basetype)].name + ".tga");
		SetNewPicture("SC_ANOTHERSHIP_IMG", "interfaces\ships\" + ShipsTypes[sti(RealShips[sti(refEnepchararacter.ship.type)].basetype)].name + ".tga");
		return;
	}
}

void SwapShips()
{
	SeaAI_SwapShipsAttributes(pchar, refEnepchararacter);
	SeaAI_SwapShipAfterAbordage(pchar, refEnepchararacter);
	
	if( sti(oTmpData.setshipcaptain) == -1 ) 
	{
		ReturnAllCrewToShip();
	} 
	else 
	{
		ReturnMinCrewToShip();
	}

	SetShipImagesAndNames();
	SetShipCapacity();
	SetShipSpeedManeuver();
	SetShipUpgrades();

	FillGoodsList();
}

void ReturnAllCrewToShip()
{
	/*
	int nOther, nMy, nMyMax, n;
	int iShip = sti(pchar.ship.type);
	// "BoardingCrew"
	nOther = sti(refEnepchararacter.ship.crew.BoardingCrew);
	nMy = sti(pchar.ship.crew.BoardingCrew);
	nMyMax = sti(RealShips[iShip].BoardingCrew);
	if( nMy < nMyMax ) 
	{
		n = nMyMax - nMy;
		if( n > nOther ) {n=nOther;}
		pchar.ship.crew.BoardingCrew = nMy + n;
		refEnepchararacter.ship.crew.BoardingCrew = nOther - n;
	}
	// "GunnerCrew"
	nOther = sti(refEnepchararacter.ship.crew.GunnerCrew);
	nMy = sti(pchar.ship.crew.GunnerCrew);
	nMyMax = sti(RealShips[iShip].GunnerCrew);
	if( nMy < nMyMax ) {
		n = nMyMax - nMy;
		if( n > nOther ) {n=nOther;}
		pchar.ship.crew.GunnerCrew = nMy + n;
		refEnepchararacter.ship.crew.GunnerCrew = nOther - n;
	}
	// "CannonerCrew"
	nOther = sti(refEnepchararacter.ship.crew.CannonerCrew);
	nMy = sti(pchar.ship.crew.CannonerCrew);
	nMyMax = sti(RealShips[iShip].CannonerCrew);
	if( nMy < nMyMax ) {
		n = nMyMax - nMy;
		if( n > nOther ) {n=nOther;}
		pchar.ship.crew.CannonerCrew = nMy + n;
		refEnepchararacter.ship.crew.CannonerCrew = nOther - n;
	}

	//"SailorCrew"
	nOther = sti(refEnepchararacter.ship.crew.SailorCrew);
	nMy = sti(pchar.ship.crew.SailorCrew);
	nMyMax = sti(RealShips[iShip].SailorCrew);
	if( nMy < nMyMax ) {
		n = nMyMax - nMy;
		if( n > nOther ) {n=nOther;}
		pchar.ship.crew.SailorCrew = nMy + n;
		refEnepchararacter.ship.crew.SailorCrew = nOther - n;
	}

	*/
	int iMySailorCrew = sti(pchar.ship.crew.SailorCrew);
	int iMyBoardingCrew = sti(pchar.ship.crew.BoardingCrew);
	int iMyGunnerCrew = sti(pchar.ship.crew.GunnerCrew);
	int iMyCannonerCrew = sti(pchar.ship.crew.CannonerCrew);

	int iEnemySailorCrew = sti(refEnepchararacter.ship.crew.SailorCrew);
	int iEnemyBoardingCrew = sti(refEnepchararacter.ship.crew.BoardingCrew);
	int iEnemyGunnerCrew = sti(refEnepchararacter.ship.crew.GunnerCrew);
	int iEnemyCannonerCrew = sti(refEnepchararacter.ship.crew.CannonerCrew);

	pchar.ship.crew.SailorCrew = iEnemySailorCrew;
	pchar.ship.crew.BoardingCrew = iEnemyBoardingCrew;
	pchar.ship.crew.GunnerCrew = iEnemyGunnerCrew;
	pchar.ship.crew.CannonerCrew = iEnemyCannonerCrew;

	refEnepchararacter.ship.crew.SailorCrew = iMySailorCrew;
	refEnepchararacter.ship.crew.BoardingCrew = iMyBoardingCrew;
	refEnepchararacter.ship.crew.GunnerCrew = iMyGunnerCrew;
	refEnepchararacter.ship.crew.CannonerCrew = iMyCannonerCrew;


	pchar.ship.crew.quantity = sti(pchar.ship.crew.SailorCrew) + sti(pchar.ship.crew.BoardingCrew) + sti(pchar.ship.crew.GunnerCrew) + sti(pchar.ship.crew.CannonerCrew);
	refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.SailorCrew) + sti(refEnepchararacter.ship.crew.BoardingCrew) + sti(refEnepchararacter.ship.crew.GunnerCrew) + sti(refEnepchararacter.ship.crew.CannonerCrew);
}

void ReturnMinCrewToShip()
{
	int nOther, nMy, nMyMax, n;

	// "BoardingCrew"
	nOther = sti(refEnepchararacter.ship.crew.BoardingCrew);
	nMy = sti(pchar.ship.crew.BoardingCrew);
	nMyMax = SetCaptain_GetMinCrewQuantity(pchar,"BoardingCrew");
	if( nMy < nMyMax ) {
		n = nMyMax - nMy;
		if( n < nOther ) {n=nOther;}
		pchar.ship.crew.BoardingCrew = nMy + n;
		refEnepchararacter.ship.crew.BoardingCrew = nOther - n;
	}
	// "GunnerCrew"
	nOther = sti(refEnepchararacter.ship.crew.GunnerCrew);
	nMy = sti(pchar.ship.crew.GunnerCrew);
	nMyMax = SetCaptain_GetMinCrewQuantity(pchar,"GunnerCrew");
	if( nMy < nMyMax ) {
		n = nMyMax - nMy;
		if( n < nOther ) {n=nOther;}
		pchar.ship.crew.GunnerCrew = nMy + n;
		refEnepchararacter.ship.crew.GunnerCrew = nOther - n;
	}

	pchar.ship.crew.quantity = sti(pchar.ship.crew.SailorCrew) + sti(pchar.ship.crew.BoardingCrew) + sti(pchar.ship.crew.GunnerCrew) + sti(pchar.ship.crew.CannonerCrew);
	refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.SailorCrew) + sti(refEnepchararacter.ship.crew.BoardingCrew) + sti(refEnepchararacter.ship.crew.GunnerCrew) + sti(refEnepchararacter.ship.crew.CannonerCrew);
}

void SaveCurrentGoodsData(bool bSave)
{
	int nGood = sti(GoodsList[iStep].index);

	if( bSave ) {
		oTmpData.myGoodQnt = GetCargoGoods(pchar,nGood);
		oTmpData.otherGoodQnt = GetCargoGoods(refEnepchararacter,nGood);
	} else {
		SetCharacterGoods(pchar,nGood,sti(oTmpData.myGoodQnt));
		SetCharacterGoods(refEnepchararacter,nGood,sti(oTmpData.otherGoodQnt));
	}
}

void UpdateCounterPos()
{
	int nY = SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPTRADELIST", 12,iStep);

	if( nY < 0 ) {return;} // warning!!!


	int pic_top;
	int pic_bottom;
	
	//310,230,496,247
	pic_top = nY - 1;
	pic_bottom = nY + 18;
	
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,"MW_GOOD_COUNTER", -1, 4, 310,pic_top, 496,pic_bottom);

	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"TOP_HIDER", 0, 238,228,558,nY-2);
	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"BOTTOM_HIDER", 0, 238,nY+20,558,331);
}

void SetShipImagesAndNames()
{
	// ship images
	int iOurShipType = sti(pchar.ship.type);
	iOurShipType = sti(RealShips[iOurShipType].basetype);
	int iOtherShipType = sti(refEnepchararacter.ship.type);
	iOtherShipType = sti(RealShips[iOtherShipType].basetype);
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OUR_SHIP_IMAGE", 6, "SHIPS_"+ShipsTypes[iOurShipType].Name, "ship" );
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OTHER_SHIP_IMAGE", 6, "SHIPS_"+ShipsTypes[iOtherShipType].Name, "ship" );
	// ship names
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 8, "#"+pchar.ship.name );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 9, "#"+refEnepchararacter.ship.name );
}

void SetConstantStrings()
{
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 10, "#"+XI_ConvertString("Capacity")+":" );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 11, "#"+XI_ConvertString("Capacity")+":" );
}

void SetShipCapacity()
{
	string sOurCargo = GetCargoLoad(pchar) + " / " + GetCargoMaxSpace(pchar);
	string sOtherCargo = GetCargoLoad(refEnepchararacter) + " / " + GetCargoMaxSpace(refEnepchararacter);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 12, "#"+sOurCargo );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 13, "#"+sOtherCargo );
}

void SetShipSpeedManeuver()
{
	int iShip;
	float fTmp;
	string sText;

	iShip = sti(pchar.ship.type);
	// our maneuver
	fTmp = stf(RealShips[iShip].turnrate) * FindShipTurnRate(pchar);
	sText  = FloatToString(fTmp,2) + " / " + FloatToString(stf(RealShips[iShip].TurnRate),2);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 4, "#"+sText );
	// our speed
	sText  = FloatToString(FindShipSpeed(pchar),2) + " / " + FloatToString(stf(RealShips[iShip].SpeedRate),2);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 3, "#"+sText );

	iShip = sti(refEnepchararacter.ship.type);
	// enemy maneuver
	fTmp = stf(RealShips[iShip].turnrate) * FindShipTurnRate(refEnepchararacter);
	sText  = FloatToString(fTmp,2) + " / " + FloatToString(stf(RealShips[iShip].TurnRate),2);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 6, "#"+sText );
	// enemy speed
	sText  = FloatToString(FindShipSpeed(refEnepchararacter),2) + " / " + FloatToString(stf(RealShips[iShip].SpeedRate),2);
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"MW_STRINGS", 1, 5, "#"+sText );
}

void SetShipUpgrades()
{
	string sOurHull,sOurSail,sOurCannonGrp,sOurCannonImg;
	string sOtherHull,sOtherSail,sOtherCannonGrp,sOtherCannonImg;

	sOurHull = "HULL_UPGRADE_" + pchar.ship.upgrades.hull;
	sOurSail = "SAILS_UPGRADE_" + pchar.ship.upgrades.sails;
	sOurCannonGrp = "CANNONS_UPGRADE_" + pchar.ship.upgrades.cannons;
	sOurCannonImg = Cannon[sti(pchar.ship.cannons.type)].picture;

	sOtherHull = "HULL_UPGRADE_" + refEnepchararacter.ship.upgrades.hull;
	sOtherSail = "SAILS_UPGRADE_" + refEnepchararacter.ship.upgrades.sails;
	sOtherCannonGrp = "CANNONS_UPGRADE_" + refEnepchararacter.ship.upgrades.cannons;
	sOtherCannonImg = Cannon[sti(refEnepchararacter.ship.cannons.type)].picture;

	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OUR_UPGRADE_HULL", 6, sOurHull, "hull" );
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OUR_UPGRADE_SAIL", 6, sOurSail, "sails" );
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OUR_UPGRADE_CANNON", 6, sOurCannonGrp, sOurCannonImg );

	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OTHER_UPGRADE_HULL", 6, sOtherHull, "hull" );
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OTHER_UPGRADE_SAIL", 6, sOtherSail, "sails" );
	SendMessage(&GameInterface,"lslss",MSG_INTERFACE_MSG_TO_NODE,"MW_OTHER_UPGRADE_CANNON", 6, sOtherCannonGrp, sOtherCannonImg );
}

void ProcessCancelExit()
{
	int n = sti(oTmpData.setshipcaptain);
	if(n<0) {
		ShipDead(sti(refEnepchararacter.index),KILL_BY_ABORDAGE,sti(pchar.index));
	} else {
		ISetShipAttributes( GetCharacter(n) );
		SeaAI_SetOfficer2ShipAfterAbordage( GetCharacter(n), refEnepchararacter );
		RemovePassenger( pchar, GetCharacter(n) );
		SetCompanionIndex( pchar, -1, n );
		int iShip = sti(characters[n].ship.type);
		RealShips[iShip].lock = 1;
		refEnepchararacter.Killer.Status = KILL_BY_ABORDAGE;
		refEnepchararacter.Killer.Index = nMainCharacterIndex;
		LAi_SetCurHP(refEnepchararacter, 0.0);
		PostEvent("evntQuestsCheck",1);
	}

	interfaceResultCommand = RC_INTERFACE_RANSACK_MAIN_EXIT;
	Exit();

	RefreshBattleInterface();
	if( CheckAttribute(&Sailors,"IsOnDeck") && sti(Sailors.IsOnDeck) )
	{
		Sailors.IsOnDeck = true;
	}
}

void ISetShipAttributes(ref chref)
{
	DeleteAttribute(chref,"ship");
	aref arToChar;	
	makearef(arToChar,chref.ship);
	aref arFromChar;	
	makearef(arFromChar,refEnepchararacter.ship);
	CopyAttributes(arToChar,arFromChar);
}

void Exit()
{
	// main events
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("eGoodGhange","procGoodGhange");
	DelEventHandler("TableSelectChange","TableSelectChange");
	DelEventHandler("GoodsAct","procGoodsAct");
	DelEventHandler("ShipAct","procShipAct");
	DelEventHandler("eToolTip","procToolTip");
	DelEventHandler("MouseRClickUP","EndTooltip");
	DelEventHandler("FTChange","FTChange");

	// set captain events
	DelEventHandler("SetCaptainAct","SetCaptainAct");
	DelEventHandler("eCrewGhange","procCrewGhange");
	DelEventHandler("ChangeSelectScrollImage","ChangeSelectScrollImage");

	// get award events
	DelEventHandler("GetAwardOk","EndAwardWindow");
	DelEventHandler("ActCaptainSurrender","ActCaptainSurrender");
	DelEventHandler("ActCrewSurrender","ActCrewSurrender");
	DelEventHandler("ActQuestPassenger","ActQuestPassenger");
	DelEventHandler("ActQuestItem","ActQuestItem");
	DelEventHandler("eCrewSurrenderGhange","procCrewSurrenderGhange");

	DelEventHandler("ConfirmCrewTransfer", "ConfirmCrewTransfer");
	DelEventHandler("CancelCrewTransfer", "CancelCrewTransfer");
	DelEventHandler("ActCrewLeave", "ActCrewLeave");

	EndCancelInterface(true);
	
	if (checkAttribute(pchar, "shipfame"))
	{
		if (pchar.ship.name != pchar.shipfame.shipname)
		{
			if(CheckAttribute(refEnepchararacter, "shipfame.fame"))
			{
				int iMFame = pchar.shipfame.fame;
				int iEFame = refEnepchararacter.shipfame.fame;
				pchar.shipfame.fame = iEFame;
				refEnepchararacter.shipfame.fame = iMFame;
			}
			else
			{
				ShipFameErase(pchar);
			}
		}
	}	
}

void FillGoodsList()
{

	SetFormatedText("SHIPTRADELIST", "");
	SetFormatedText("SHIPQUANTITYLIST", "");
	SetFormatedText("STOREQUANTITYLIST", "");

	int n,i,nOur,nOther;
	string sGood,sText;

	int iColor;

	i = 0;
	for(n=0; n<GOODS_QUANTITY; n++)
	{
		sGood = Goods[n].name;
		nOur = GetCargoGoods(pchar, n);
		nOther = GetCargoGoods(refEnepchararacter, n);
		if( nOur>0 || nOther>0 )
		{
			iColor = COLOR_NORMAL;
			if(CheckAttribute(&goods[n], "type"))
			{
				if(sti(goods[n].type) == TRADE_TYPE_AMMUNITION)
				{
					iColor = argb(255,192,192,192);
				}
				if(sti(goods[n].type) == TRADE_TYPE_CONTRABAND)
				{
					iColor = argb(255,200,125,125);
				}
			}
			if(CheckAttribute(pchar, "ship.cargo.goods." + sGood + ".isquest"))
			{
				iColor = argb(255,255,192,255);
			}
			GoodsList[i].index = n;
			sText = XI_ConvertString(sGood);
			SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPTRADELIST", 0,&sText);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPTRADELIST", 8,-1,iColor);
			sText = nOur;
			SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPQUANTITYLIST", 0,&sText);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPQUANTITYLIST", 8,-1,iColor);
			sText = nOther;
			SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"STOREQUANTITYLIST", 0,&sText);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STOREQUANTITYLIST", 8,-1,iColor);
			i++;
		}
	}

	iGoodIndex = sti(GoodsList[0].index);
	iStringsQuantity = i - 1;
	//GameInterface.MW_GOODS_LIST.select = 1;
	//GameInterface.MW_GOODS_LIST.top = 0;
	SaveCurrentGoodsData(true);

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"MW_GOODS_LIST", 0 );
	UpdateCounterPos();
}

//============================================================
//============================================================
// Set Captain Interfaces
//============================================================
void ShowCaptainWindow(bool bShow)
{
	if( bShow ) {
		XI_WindowShow("SETCAPTAIN_WINDOW", true);
		XI_WindowDisable("SETCAPTAIN_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", true);
		SetCurrentNode("SC_CANCEL");

		StoreCaptainData(true);
		SetDefaultCrewSet();
		SetCaptainFillCrewList();
		SetCaptainFillPassengersList();
		SetCaptainHiderUpdate();
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, string_number, string_text );
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 1, "#"+XI_ConvertString("Give goods")+": 0" );
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": 0" );

	} else {
		XI_WindowShow("SETCAPTAIN_WINDOW", false);
		XI_WindowDisable("SETCAPTAIN_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		SetCurrentNode("MW_BTN_SHIP_CAPTURE");
	}
}

void SetCaptainAct()
{
	int nCommand = GetEventData();
	string sNode = GetEventData();

	if( sNode == "SC_OK" ) {
		string sattr = "pic"+(sti(GameInterface.SC_CAPTAINS_SCROLL.current)+1);
		if( CheckAttribute(&GameInterface,"SC_CAPTAINS_SCROLL."+sattr+".character") ) {
			oTmpData.setshipcaptain = GameInterface.SC_CAPTAINS_SCROLL.(sattr).character;
		} else {
			oTmpData.setshipcaptain = -1;
		}
		ShowCaptainWindow(false);
		return;
	}
	if( sNode == "SC_CANCEL" ) {
		StoreCaptainData(false);
		ShowCaptainWindow(false);
		return;
	}

	if( sNode == "SC_BTN_TRANSFER_OK" ) {
		SetCaptainStoreIntermediaCrew(true);
		SetCaptainHiderUpdate();
		return;
	}
	if( sNode == "SC_BTN_TRANSFER_CANCEL" ) {
		SetCaptainStoreIntermediaCrew(false);
		SetCaptainFillCrewList();
		SetCaptainHiderUpdate();
		return;
	}
}

void SetCaptainHiderUpdate()
{
	int nLine = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 11 );
	string sCrew = "SailorCrew";
	switch( nLine )
	{
	case 0: sCrew = "SailorCrew"; break;
	case 1: sCrew = "BoardingCrew"; break;
	case 2: sCrew = "GunnerCrew"; break;
	case 3: sCrew = "CannonerCrew"; break;
	}

	int nq = sti(pchar.ship.crew.(sCrew)) - sti(oTmpData.tmpcrew.(sCrew));
	if( nq!=0 ) {
		if( nq > 0 ) {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 1, "#"+XI_ConvertString("Give goods")+": 0" );
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": " + nq );
		} else {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 1, "#"+XI_ConvertString("Give goods")+": " + (-nq) );
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": 0" );
		}

		SetSelectable("SC_BTN_TRANSFER_OK",true);
		SetSelectable("SC_BTN_TRANSFER_CANCEL",true);
		SetSelectable("SC_OK",false);
		SetSelectable("SC_CANCEL",false);

		SetNodeUsing("TOP_HIDER",true);
		SetNodeUsing("BOTTOM_HIDER",true);

		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 13, true);
	} else {
		SetSelectable("SC_BTN_TRANSFER_OK",false);
		SetSelectable("SC_BTN_TRANSFER_CANCEL",false);
		SetSelectable("SC_OK",true);
		SetSelectable("SC_CANCEL",true);

		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 1, "#"+XI_ConvertString("Give goods")+": 0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"ST_CREW_STRINGS", 1, 2, "#"+XI_ConvertString("Take goods")+": 0" );

		SetNodeUsing("TOP_HIDER",false);
		SetNodeUsing("BOTTOM_HIDER",false);

		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 13, false);
	}
}

void SetCaptainFillCrewList()
{
	int nLine = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 11 );

	SetFormatedtext("CREWLIST", "");
	SetFormatedtext("OURCREWLIST", "");
	SetFormatedtext("OTHERCREWLIST", "");
	
	int iOurShip = sti(pchar.ship.type);
	int iOtherShip = sti(refEnepchararacter.ship.type);
	string sText;

	sText = sti(pchar.ship.crew.SailorCrew) + " / " + sti(RealShips[iOurShip].SailorCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 0,&sText);

	sText = XI_ConvertString("Sailors");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 0,&sText);
	
	sText = sti(refEnepchararacter.ship.crew.SailorCrew) + " / " + sti(RealShips[iOtherShip].SailorCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 0,&sText);


	sText = sti(pchar.ship.crew.BoardingCrew) + " / " + sti(RealShips[iOurShip].BoardingCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 0,&sText);

	sText = XI_ConvertString("Soldiers");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 0,&sText);
	
	sText = sti(refEnepchararacter.ship.crew.BoardingCrew) + " / " + sti(RealShips[iOtherShip].BoardingCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 0,&sText);

	
	sText = sti(pchar.ship.crew.GunnerCrew) + " / " + sti(RealShips[iOurShip].GunnerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 0,&sText);

	sText = XI_ConvertString("Musketeers");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 0,&sText);
	
	sText = sti(refEnepchararacter.ship.crew.GunnerCrew) + " / " + sti(RealShips[iOtherShip].GunnerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 0,&sText);

	
	sText = sti(pchar.ship.crew.CannonerCrew) + " / " + sti(RealShips[iOurShip].CannonerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 0,&sText);

	sText = XI_ConvertString("Cannoners");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 0,&sText);
	
	sText = sti(refEnepchararacter.ship.crew.CannonerCrew) + " / " + sti(RealShips[iOtherShip].CannonerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 0,&sText);

	SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 1, nLine );
	SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OURCREWLIST", 1, nLine );
	SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"OTHERCREWLIST", 1, nLine );
}

void SetCaptainFillPassengersList()
{
	int i,m;
	string attributeName,PsgAttrName;
	int cn;

	DeleteAttribute(&GameInterface, "SC_CAPTAINS_SCROLL");

	GameInterface.SC_CAPTAINS_SCROLL.current = 0;
	GameInterface.SC_CAPTAINS_SCROLL.BadPicture1 = "interfaces\empty_face.tga";
	GameInterface.SC_CAPTAINS_SCROLL.NotUsed = 9;

	FillFaceList("SC_CAPTAINS_SCROLL.ImagesGroup", pchar, 2); // passengers

	m = 0;
	int start_idx = -1;
	int start_cn = -1;
	int nListSize = GetPassengersQuantity(pchar);
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		PsgAttrName = "id"+(i+1);

		cn = sti(pchar.Fellows.Passengers.(PsgAttrName));

		if(cn!=-1 && !CheckAttribute(&characters[cn], "isfree") && !CheckAttribute(&characters[cn], "isquest") && sti(characters[cn].prisoned) != 1)
		{
			if( start_cn==-1 ) {start_cn=cn;}
			if( cn==sti(oTmpData.setshipcaptain) ) {start_cn=cn; start_idx=m;}
			GameInterface.SC_CAPTAINS_SCROLL.(attributeName).character = cn;
			GameInterface.SC_CAPTAINS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(cn));
			GameInterface.SC_CAPTAINS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("SC_CAPTAINS_SCROLL.ImagesGroup","FACE128_"+Characters[cn].FaceID);
			m++;
		}
	}
	GameInterface.SC_CAPTAINS_SCROLL.ListSize = m;
	if( start_idx!=-1 ) {GameInterface.SC_CAPTAINS_SCROLL.current = start_idx;}

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_REFRESH_SCROLL,"SC_CAPTAINS_SCROLL");
	SetCaptainUpdateCounterPos();
	SelectNewCaptain(start_cn);
}

void SetCaptainUpdateCounterPos()
{
	int nLine = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 11 );
	int nY = SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 12,nLine);
	if( nY < 0 ) {return;} // warning!!!

	// update counter position
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,"SC_CREW_COUNTER", -1, 4, 320,nY, 491,nY+18);

	// update hider position
	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"TOP_HIDER", 0, 239,379,573,nY-1);
	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"BOTTOM_HIDER", 0, 239,nY+18,573,463);
}

void procCrewGhange()
{
	int nCommand = GetEventData();

	string sCrew = "SailorCrew";
	switch( SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CREWLIST", 11 ) + 1 )
	{
	case 1: sCrew = "SailorCrew"; break;
	case 2: sCrew = "BoardingCrew"; break;
	case 3: sCrew = "GunnerCrew"; break;
	case 4: sCrew = "CannonerCrew"; break;
	}
	int n = 1;
	if( XI_IsKeyPressed("control") ) {n = 10;}
	if( XI_IsKeyPressed("alt") ) {n=100;}

	if( nCommand==ACTION_LEFTSTEP ) 
	{
		if( XI_IsKeyPressed("shift") ) 
		{
			n = sti(refEnepchararacter.ship.crew.(sCrew));
		}
		ReplaceCrew(sCrew,refEnepchararacter,pchar,n);
	} 
	else 
	{
		if( XI_IsKeyPressed("shift") ) 
		{
			n = sti(pchar.ship.crew.(sCrew));
		}
		ReplaceCrew(sCrew,pchar,refEnepchararacter,n);
	}

	SetCaptainHiderUpdate();
}

void ReplaceCrew(string sCrew,ref rOut,ref rIn,int nq)
{
	// ограничение по максимальной команде
	int nMax = sti( RealShips[sti(rIn.ship.type)].(sCrew) );
	if( sti(rIn.ship.crew.(sCrew)) + nq > nMax ) {nq = nMax - sti(rIn.ship.crew.(sCrew));}
	if( nq <= 0 ) {return;}

	// ограничение по минимальной команде
	int nMinLeft = SetCaptain_GetMinCrewQuantity(rOut,sCrew);
	int ncur = sti(rOut.ship.crew.(sCrew));
	if( ncur - nq < nMinLeft ) {nq = ncur - nMinLeft;}
	if( nq <= 0 ) {return;}

	rIn.ship.crew.(sCrew) = sti(rIn.ship.crew.(sCrew)) + nq;
	rOut.ship.crew.(sCrew) = sti(rOut.ship.crew.(sCrew)) - nq;
	rIn.ship.crew.quantity = sti(rIn.ship.crew.quantity) + nq;
	rOut.ship.crew.quantity = sti(rOut.ship.crew.quantity) - nq;

	SetCaptainFillCrewList();
}

int SetCaptain_GetMinCrewQuantity(ref rChr, string sCrew)
{
	int iShip = sti(rChr.ship.type);
	switch( sCrew )
	{
	case "SailorCrew":
		return sti(RealShips[iShip].mincrew);
	break;
	case "BoardingCrew":
		if( rChr.id!=pchar.id ) {return 0;}
		else {return (sti(RealShips[iShip].BoardingCrew) / 10);}
	break;
	case "GunnerCrew":
		if( rChr.id!=pchar.id ) {return 0;}
		else {return (sti(RealShips[iShip].GunnerCrew) / 10);}
	break;
	case "CannonerCrew":
		return (sti(RealShips[iShip].CannonerCrew) / 10);
	break;
	}
}

void StoreCaptainData(bool bStore)
{
	if( bStore ) {
		oTmpData.ourcrew.SailorCrew = pchar.ship.crew.SailorCrew;
		oTmpData.crew.SailorCrew = refEnepchararacter.ship.crew.SailorCrew;
		//
		oTmpData.ourcrew.BoardingCrew = pchar.ship.crew.BoardingCrew;
		oTmpData.crew.BoardingCrew = refEnepchararacter.ship.crew.BoardingCrew;
		//
		oTmpData.ourcrew.GunnerCrew = pchar.ship.crew.GunnerCrew;
		oTmpData.crew.GunnerCrew = refEnepchararacter.ship.crew.GunnerCrew;
		//
		oTmpData.ourcrew.CannonerCrew = pchar.ship.crew.CannonerCrew;
		oTmpData.crew.CannonerCrew = refEnepchararacter.ship.crew.CannonerCrew;
		//
		oTmpData.ourcrew.quantity = pchar.ship.crew.quantity;
		oTmpData.crew.quantity = refEnepchararacter.ship.crew.quantity;
	} else {
		pchar.ship.crew.SailorCrew = oTmpData.ourcrew.SailorCrew;
		refEnepchararacter.ship.crew.SailorCrew = oTmpData.crew.SailorCrew;
		//
		pchar.ship.crew.BoardingCrew = oTmpData.ourcrew.BoardingCrew;
		refEnepchararacter.ship.crew.BoardingCrew = oTmpData.crew.BoardingCrew;
		//
		pchar.ship.crew.GunnerCrew = oTmpData.ourcrew.GunnerCrew;
		refEnepchararacter.ship.crew.GunnerCrew = oTmpData.crew.GunnerCrew;
		//
		pchar.ship.crew.CannonerCrew = oTmpData.ourcrew.CannonerCrew;
		refEnepchararacter.ship.crew.CannonerCrew = oTmpData.crew.CannonerCrew;
		//
		pchar.ship.crew.quantity = oTmpData.ourcrew.quantity;
		refEnepchararacter.ship.crew.quantity = oTmpData.crew.quantity;
	}
}

void ChangeSelectScrollImage()
{
	string sNode = GetEventData();
	int nIdx = GetEventData();

	int cn;
	string sattr;

	if( sNode == "SC_CAPTAINS_SCROLL" ) {
		sattr = "pic" + (nIdx+1);
		cn = -1;
		if( CheckAttribute(&GameInterface,"SC_CAPTAINS_SCROLL."+sattr+".character") ) {
			cn = sti(GameInterface.SC_CAPTAINS_SCROLL.(sattr).character);
		}
		SelectNewCaptain(cn);
	}

	if( sNode == "CREWSURRENDER_CREW_LIST" ) {
		sattr = "pic" + (nIdx+1);
		cn = -1;
		if( CheckAttribute(&GameInterface,"CREWSURRENDER_CREW_LIST."+sattr+".character") ) {
			cn = sti(GameInterface.CREWSURRENDER_CREW_LIST.(sattr).character);
		}
		Surrender_SelectCaptain(&characters[cn]);
	}
}

void SelectNewCaptain(int cn)
{
	if( cn<0 ) {
		// image
		SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_CAPTAIN_IMG", 2, 0, "interfaces\empty_face.tga" );
		// name, rank, loyality
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 1, "# " ); // name
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 2, "#"+XI_ConvertString("Level")+": 0" ); // rank
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 3, "#"+XI_ConvertString("Loyality")+": " + XI_ConvertString(GetLoyalityName(99))); // loyality
		// skills
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 4, "#0" ); // leadership
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 5, "#0" ); // sailing
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 6, "#0" ); // tactic
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 6, "#0" ); // fencing
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 7, "#0" ); // gun
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 8, "#0" ); // cannons
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 9, "#0" ); // accuracy
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 10, "#0" ); // grappling
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 11, "#0" ); // defence
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 12, "#0" ); // repair
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 14, "#0" ); // commerce
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 13, "#0" ); // luck
	} else {
		// image
		SendMessage(&GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_CAPTAIN_IMG", 2, 0, "interfaces\portraits\64\face_"+Characters[cn].FaceId+".tga" );
		// name, rank, loyality
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 1, "#"+GetCharacterFullName(characters[cn].id) ); // name
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 2, "#"+XI_ConvertString("level")+": " + characters[cn].rank ); // rank
		if( CheckAttribute(&characters[cn],"loyality") ) {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 3, "#"+XI_ConvertString("Loyality")+": " + XI_ConvertString(GetLoyalityName(sti(characters[cn].loyality)))); // loyality
		} else {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 3, "#"+XI_ConvertString("Loyality")+": " + XI_ConvertString(GetLoyalityName(99))); // loyality
		}
		// skills
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 4, "#"+sti(characters[cn].skill.Leadership) ); // leadership
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 5, "#"+sti(characters[cn].skill.Sailing) ); // sailing
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 6, "#"+characters[cn].skill.Tactic ); // tactic
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 6, "#"+sti(characters[cn].skill.Fencing) ); // fencing
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 7, "#"+sti(characters[cn].skill.Gun) ); // gun
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 8, "#"+sti(characters[cn].skill.Cannons) ); // cannons
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 9, "#"+sti(characters[cn].skill.Accuracy) ); // accuracy
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 10, "#"+sti(characters[cn].skill.Grappling) ); // grappling
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 11, "#"+sti(characters[cn].skill.Defence) ); // defence
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 12, "#"+sti(characters[cn].skill.Repair) ); // repair
		//SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 14, "#"+characters[cn].skill.Commerce ); // commerce
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SC_STRINGS", 1, 13, "#"+sti(characters[cn].skill.Sneak) ); // luck
	}
}

void SetDefaultCrewSet()
{
	int nOtherMin,nOtherCur, nMyMin,nMyCur, n;
	//"SailorCrew"
	nOtherMin = SetCaptain_GetMinCrewQuantity(refEnepchararacter,"SailorCrew");
	nOtherCur = sti(refEnepchararacter.ship.crew.SailorCrew);
	nMyMin = SetCaptain_GetMinCrewQuantity(pchar,"SailorCrew");
	nMyCur = sti(pchar.ship.crew.SailorCrew);
	if( nOtherCur<nOtherMin ) {
		n = nOtherMin - nOtherCur;
		if( n > nMyCur - nMyMin ) {n=nMyCur - nMyMin;}
		if( n > 0 ) {
			refEnepchararacter.ship.crew.SailorCrew = sti(refEnepchararacter.ship.crew.SailorCrew) + n;
			pchar.ship.crew.SailorCrew = sti(pchar.ship.crew.SailorCrew) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
		}
	}
	// "BoardingCrew"
	nOtherMin = SetCaptain_GetMinCrewQuantity(refEnepchararacter,"BoardingCrew");
	nOtherCur = sti(refEnepchararacter.ship.crew.BoardingCrew);
	nMyMin = SetCaptain_GetMinCrewQuantity(pchar,"BoardingCrew");
	nMyCur = sti(pchar.ship.crew.BoardingCrew);
	if( nOtherCur<nOtherMin ) {
		n = nOtherMin - nOtherCur;
		if( n > nMyCur - nMyMin ) {n=nMyCur - nMyMin;}
		if( n > 0 ) {
			refEnepchararacter.ship.crew.BoardingCrew = sti(refEnepchararacter.ship.crew.BoardingCrew) + n;
			pchar.ship.crew.BoardingCrew = sti(pchar.ship.crew.BoardingCrew) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
		}
	}
	// "GunnerCrew"
	nOtherMin = SetCaptain_GetMinCrewQuantity(refEnepchararacter,"GunnerCrew");
	nOtherCur = sti(refEnepchararacter.ship.crew.GunnerCrew);
	nMyMin = SetCaptain_GetMinCrewQuantity(pchar,"GunnerCrew");
	nMyCur = sti(pchar.ship.crew.GunnerCrew);
	if( nOtherCur<nOtherMin ) {
		n = nOtherMin - nOtherCur;
		if( n > nMyCur - nMyMin ) {n=nMyCur - nMyMin;}
		if( n > 0 ) {
			refEnepchararacter.ship.crew.GunnerCrew = sti(refEnepchararacter.ship.crew.GunnerCrew) + n;
			pchar.ship.crew.GunnerCrew = sti(pchar.ship.crew.GunnerCrew) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
		}
	}
	// "CannonerCrew"
	nOtherMin = SetCaptain_GetMinCrewQuantity(refEnepchararacter,"CannonerCrew");
	nOtherCur = sti(refEnepchararacter.ship.crew.CannonerCrew);
	nMyMin = SetCaptain_GetMinCrewQuantity(pchar,"CannonerCrew");
	nMyCur = sti(pchar.ship.crew.CannonerCrew);
	if( nOtherCur<nOtherMin ) {
		n = nOtherMin - nOtherCur;
		if( n > nMyCur - nMyMin ) {n=nMyCur - nMyMin;}
		if( n > 0 ) {
			refEnepchararacter.ship.crew.CannonerCrew = sti(refEnepchararacter.ship.crew.CannonerCrew) + n;
			pchar.ship.crew.CannonerCrew = sti(pchar.ship.crew.CannonerCrew) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
		}
	}

	SetCaptainStoreIntermediaCrew(true);
}

void SetCaptainStoreIntermediaCrew(bool bStore)
{
	int n;
	if( bStore ) {
		oTmpData.tmpcrew.SailorCrew = pchar.ship.crew.SailorCrew;
		oTmpData.tmpcrew.BoardingCrew = pchar.ship.crew.BoardingCrew;
		oTmpData.tmpcrew.GunnerCrew = pchar.ship.crew.GunnerCrew;
		oTmpData.tmpcrew.CannonerCrew = pchar.ship.crew.CannonerCrew;
	} else {
		n = sti(pchar.ship.crew.SailorCrew) - sti(oTmpData.tmpcrew.SailorCrew);
		if( n!=0 ) {
			pchar.ship.crew.SailorCrew = sti(pchar.ship.crew.SailorCrew) - n;
			refEnepchararacter.ship.crew.SailorCrew = sti(refEnepchararacter.ship.crew.SailorCrew) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
		}
		//
		n = sti(pchar.ship.crew.BoardingCrew) - sti(oTmpData.tmpcrew.BoardingCrew);
		if( n!=0 ) {
			pchar.ship.crew.BoardingCrew = sti(pchar.ship.crew.BoardingCrew) - n;
			refEnepchararacter.ship.crew.BoardingCrew = sti(refEnepchararacter.ship.crew.BoardingCrew) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
		}
		//
		n = sti(pchar.ship.crew.GunnerCrew) - sti(oTmpData.tmpcrew.GunnerCrew);
		if( n!=0 ) {
			pchar.ship.crew.GunnerCrew = sti(pchar.ship.crew.GunnerCrew) - n;
			refEnepchararacter.ship.crew.GunnerCrew = sti(refEnepchararacter.ship.crew.GunnerCrew) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
		}
		//
		n = sti(pchar.ship.crew.CannonerCrew) - sti(oTmpData.tmpcrew.CannonerCrew);
		if( n!=0 ) {
			pchar.ship.crew.CannonerCrew = sti(pchar.ship.crew.CannonerCrew) - n;
			refEnepchararacter.ship.crew.CannonerCrew = sti(refEnepchararacter.ship.crew.CannonerCrew) + n;
			pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
			refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;
		}
	}
}

bool CheckSetCaptainPossibility()
{
	int i,cn,nListSize,n;
	string PsgAttrName;

	// проверяем наличие капитана для постановки на корабль
	nListSize = GetPassengersQuantity(pchar);
	for(i=0; i<nListSize; i++)
	{
		PsgAttrName = "id"+(i+1);
		cn = sti(pchar.Fellows.Passengers.(PsgAttrName));
		if(cn!=-1 && !CheckAttribute(&characters[cn], "isfree") && !CheckAttribute(&characters[cn], "isquest") && sti(characters[cn].prisoned) != 1) {
			break; // нашли нужного чела
		}
	}
	if( i < nListSize ) {
		// проверяем наличие команды для постановки на корабль
		// матросы
		n = SetCaptain_GetMinCrewQuantity(pchar,"SailorCrew");
		n = n + SetCaptain_GetMinCrewQuantity(refEnepchararacter,"SailorCrew");
		if( sti(pchar.ship.crew.SailorCrew) >= n ) 
		{
			// абордажная команда
			//n = SetCaptain_GetMinCrewQuantity(pchar,"BoardingCrew");
			//n = n + SetCaptain_GetMinCrewQuantity(refEnepchararacter,"BoardingCrew");
			//if( sti(pchar.ship.crew.BoardingCrew) >= n ) {
				// мушкетеры
				//n = SetCaptain_GetMinCrewQuantity(pchar,"GunnerCrew");
				//n = n + SetCaptain_GetMinCrewQuantity(refEnepchararacter,"GunnerCrew");
				//if( sti(pchar.ship.crew.GunnerCrew) >= n ) {
					// канониры
					n = SetCaptain_GetMinCrewQuantity(pchar,"CannonerCrew");
					n = n + SetCaptain_GetMinCrewQuantity(refEnepchararacter,"CannonerCrew");
					if( sti(pchar.ship.crew.CannonerCrew) >= n ) 
					{
						return true;
					}
				//}
			//}
		}
	}
	return false;
}

//============================================================
//============================================================
// Previouse Interfaces
//============================================================
void StartAwardWindow()
{
	XI_WindowDisable("MAIN_WINDOW", true);

	// деньги
	int iAwardMoney = 0;
	if( CheckAttribute(refEnepchararacter,"money") ) 
	{
		iAwardMoney = sti(refEnepchararacter.money);
	}

	string sPic = "-1";
	int iItemIdx = -1;
	if(CheckAttribute(refEnepchararacter, "booty.item"))
	{
		iItemIdx = sti(refEnepchararacter.booty.item);
	}

	if(iAwardMoney > 0 || iItemIdx != -1)
	{
		XI_WindowShow("GETAWARD_WINDOW", true);
		XI_WindowDisable("GETAWARD_WINDOW", false);
		SetCurrentNode("GETAWARD_OK_BUTTON");
	}
	else
	{
		StartCaptainSurrender();
		return;
	}

	if( iAwardMoney > 0 ) 
	{
		SetFormatedText("GETAWARD_MONEY_TEXT", XI_ConvertString("YouFindGold") + FindRussianMoneyString(iAwardMoney));
		AddMoneyToCharacter(pchar,iAwardMoney);
	} 
	else 
	{
		SetNodeUsing("GETAWARD_MONEY_TEXT",false);
	}

	if( iItemIdx >= 0) 
	{
		int nLangFile = LanguageOpenFile( "ItemsDescribe.txt" );
		SetFormatedText("GETAWARD_ITEM_TEXT", XI_ConvertString("YouFindItem")+LanguageConvertString(nLangFile,Items[iItemIdx].name));
		if(Items[iItemIdx].groupID == BLADE_ITEM_TYPE)
		{
			sPic = "blade" + Items[iItemIdx].picIndex;
		}
		if(Items[iItemIdx].groupID == GUN_ITEM_TYPE)
		{
			sPic = "gun" + Items[iItemIdx].picIndex;
		}
		if(Items[iItemIdx].groupID == SPYGLASS_ITEM_TYPE)
		{
			if(Items[iItemIdx].id == "spyglass5")
			{
				Items[iItemIdx].picIndex = 4;
			}
			sPic = "spyglass" + Items[iItemIdx].picIndex;
		}
		if(sPic == "-1")
		{
			sPic = "itm" + Items[iItemIdx].picIndex;
		}
		SetNewGroupPicture("GETAWARD_ITEM_PICTURE", Items[iItemIdx].picTexture, sPic);
		LanguageCloseFile(nLangFile);
		GiveItem2Character(pchar,Items[iItemIdx].id);
		if( CheckAttribute(&Items[iItemIdx],"uniq") && sti(Items[iItemIdx].uniq)==1 ) 
		{
			Items[iItemIdx].uniq.taked = 1;
		}
	} 
	else 
	{
		SetNodeUsing("GETAWARD_ITEM_TEXT",false);
		SetNodeUsing("GETAWARD_ITEM_PICTURE",false);
	}
}

void StartMainWindow()
{
	/*
	refEnepchararacter.ship.crew.SailorCrew = 0;
	refEnepchararacter.ship.crew.BoardingCrew = 0;
	refEnepchararacter.ship.crew.GunnerCrew = 0;
	refEnepchararacter.ship.crew.CannonerCrew = 0;
	refEnepchararacter.ship.crew.Quantity = 0;*/

	oTmpData.Possibility_set_captain = CheckSetCaptainPossibility();

	XI_WindowDisable("MAIN_WINDOW", false);

	iStep = 0;
	iPrevStep = 0;
	HideGoods(false);
	SetGoodsTakeStrings();

	SetSelectable( "MW_BTN_SHIP_CAPTURE", sti(oTmpData.Possibility_set_captain) );
	SetCurrentNode("MW_GOODS_LIST");
}

void EndAwardWindow()
{
	XI_WindowDisable("GETAWARD_WINDOW", true);
	XI_WindowShow("GETAWARD_WINDOW", false);

	StartCaptainSurrender();
}

void StartCaptainSurrender()
{
	if( oTmpData.captureState=="captain" ) {
		XI_WindowDisable("CAPTAINSURRENDER_WINDOW", false);
		XI_WindowShow("CAPTAINSURRENDER_WINDOW", true);
		SetNewPicture("CAPTAINSURRENDER_PICTURE", "interfaces\portraits\256\face_" + refEnepchararacter.FaceId + ".tga");
		SetCurrentNode("CAPTAINSURRENDER_KILL_BUTTON");
		SetFormatedText("CS_CHARACTER_TEXT", "");

		string sText;

		sText = XI_ConvertString("Name") + ":";
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_TEXT", 0,&sText);
		sText = XI_ConvertString("nation") + ":";
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_TEXT", 0,&sText);
		sText = XI_ConvertString("Ransom") + ":";
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_TEXT", 0,&sText);
		
		sText = GetCharacterFullName(refEnepchararacter.id);
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_INFO", 0,&sText);
		sText = XI_ConvertString(GetNationNameByType(sti(refEnepchararacter.nation)));
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_INFO", 0,&sText);
		sText = FindRussianMoneyString(CalculateRansomPrice(refEnepchararacter));
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CS_CHARACTER_INFO", 0,&sText);

	} else {
		StartCrewSurrender();
	}
}

void ActCaptainSurrender()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();
	if( sNodName=="CAPTAINSURRENDER_CAPTURE_BUTTON" ) 
	{
		int iChar = CreatePrisoner(refEnepchararacter);
		AddPassenger(pchar,&characters[iChar],true);
	}

	XI_WindowDisable("CAPTAINSURRENDER_WINDOW", true);
	XI_WindowShow("CAPTAINSURRENDER_WINDOW", false);
	StartCrewSurrender();
}

void StartCrewSurrender()
{
	bool bJoin = false;
	if( oTmpData.captureState=="captain" || oTmpData.captureState=="crew" ) {
		bJoin = true;
	}
	// включаем интерфейс перехода, если есть кому переходить
	if( bJoin ) 
	{
		iCrewAdd = 0;
		XI_WindowShow("CREWSURRENDER_WINDOW", true);
		XI_WindowDisable("CREWSURRENDER_WINDOW", false);
		SetNewPicture("CREWSURRENDER_OUR_IMG", "interfaces\portraits\256\face_" + pchar.FaceId + ".tga");
		SetNewPicture("CREWSURRENDER_OTHER_IMG", "interfaces\portraits\256\face_" + refEnepchararacter.FaceId + ".tga");
		SetFormatedText("CREWSURRENDER_HINT2_TEXT", XI_ConvertString("SurrendedCrewDestiny"));
		SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_HINT2_TEXT", 5 );
		oTmpData.crewsurrender_curcaptain = pchar.index;
		InitSurrenderCrewQuantity();
		StoreSurrenderCrew();
		FillSurrenderCrewList();
		SetCurrentNode("CREWSURRENDER_OK_BUTTON");
		SetClickable("CREWSURRENDER_TRANSFER_OK", false);
		SetClickable("CREWSURRENDER_TRANSFER_CANCEL", false);
		SetNodeUsing("TOP_HIDER",false);
		SetNodeUsing("BOTTOM_HIDER",false);
		SurrenderCrewUpdateCounterPos();
		SetCurrentNode("CREWSURRENDER_SLAVE_BUTTON");
	} 
	else 
	{
		refEnepchararacter.ship.crew.quantity = 0;
		refEnepchararacter.ship.crew.SailorCrew = 0;
		refEnepchararacter.ship.crew.BoardingCrew = 0;
		refEnepchararacter.ship.crew.GunnerCrew = 0;
		refEnepchararacter.ship.crew.CannonerCrew = 0;
		StartQuestPassengerFound();
	}
}

void InitSurrenderCrewQuantity()
{
	int nq = 0;

	refEnepchararacter.ship.crew.SailorCrew = sti(refEnepchararacter.ship.crew.SailorCrew);
	nq = nq + sti(refEnepchararacter.ship.crew.SailorCrew);

	refEnepchararacter.ship.crew.BoardingCrew = sti(refEnepchararacter.ship.crew.BoardingCrew);
	nq = nq + sti(refEnepchararacter.ship.crew.BoardingCrew);

	refEnepchararacter.ship.crew.GunnerCrew = sti(refEnepchararacter.ship.crew.GunnerCrew);
	nq = nq + sti(refEnepchararacter.ship.crew.GunnerCrew);

	refEnepchararacter.ship.crew.CannonerCrew = sti(refEnepchararacter.ship.crew.CannonerCrew);
	nq = nq + sti(refEnepchararacter.ship.crew.CannonerCrew);

	refEnepchararacter.ship.crew.Quantity = nq;
}

void StoreSurrenderCrew()
{
	int n,cn;
	string sAttr;

	// сохраним нашу команду
	for(n=0; n<COMPANION_MAX; n++) {
		cn = GetCompanionIndex(pchar,n);
		if( cn!=-1 ) {
			sAttr = "mycrew"+n;
			oTmpData.(sAttr).SailorCrew = characters[cn].ship.crew.SailorCrew;
			oTmpData.(sAttr).BoardingCrew = characters[cn].ship.crew.BoardingCrew;
			oTmpData.(sAttr).GunnerCrew = characters[cn].ship.crew.GunnerCrew;
			oTmpData.(sAttr).CannonerCrew = characters[cn].ship.crew.CannonerCrew;
		}
	}
}

void FillSurrenderCrewList()
{
	int iOurShip = sti(pchar.ship.type); 
	int iEnemyShip = sti(refEnepchararacter.ship.type);
	
	string sText;
	sText = sti(pchar.ship.crew.SailorCrew) + " / " + sti(Realships[iOurShip].SailorCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 0,&sText);
	sText = XI_ConvertString("Sailors");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 0,&sText);
	sText = sti(refEnepchararacter.ship.crew.SailorCrew) + " / " + sti(Realships[iEnemyShip].SailorCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 0,&sText);

	sText = sti(pchar.ship.crew.BoardingCrew) + " / " + sti(Realships[iOurShip].BoardingCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 0,&sText);
	sText = XI_ConvertString("Soldiers");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 0,&sText);
	sText = sti(refEnepchararacter.ship.crew.BoardingCrew) + " / " + sti(Realships[iEnemyShip].BoardingCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 0,&sText);

	sText = sti(pchar.ship.crew.GunnerCrew) + " / " + sti(Realships[iOurShip].GunnerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 0,&sText);
	sText = XI_ConvertString("Musketeers");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 0,&sText);
	sText = sti(refEnepchararacter.ship.crew.GunnerCrew) + " / " + sti(Realships[iEnemyShip].GunnerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 0,&sText);

	sText = sti(pchar.ship.crew.CannonerCrew) + " / " + sti(Realships[iOurShip].CannonerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 0,&sText);
	sText = XI_ConvertString("Cannoners");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 0,&sText);
	sText = sti(refEnepchararacter.ship.crew.CannonerCrew) + " / " + sti(Realships[iEnemyShip].CannonerCrew);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 0,&sText);
}

void Surrender_SelectCaptain(ref rchr)
{
	// обновим изображение капитана
	SetNewPicture("CREWSURRENDER_OUR_IMG", "interfaces\portraits\256\face_" + rchr.FaceId + ".tga");

	// обновим список команды для этого капитана
	UpdateSurrenderCrewList(rchr);

	// сохраним индекс капитана для дальнейших операций
	oTmpData.crewsurrender_curcaptain = rchr.index;
}

void procCrewSurrenderGhange()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();

	if( nCommand==ACTION_LEFTSTEP ) {
		AddSurrenderCrewToShip();
	}
	if( nCommand==ACTION_RIGHTSTEP ) {
		RemoveSurrenderCrewFromShip();
	}
}

void AddSurrenderCrewToShip()
{
	int n,nMax;
	string sCrew;
	ref chref;

	//chref = &characters[sti(oTmpData.crewsurrender_curcaptain)];
	int iShip = sti(pchar.ship.type);

	switch(iStep)
	{
		case 0: sCrew = "SailorCrew"; nMax=sti(RealShips[iShip].SailorCrew); break;
		case 1: sCrew = "BoardingCrew"; nMax=sti(RealShips[iShip].BoardingCrew); break;
		case 2: sCrew = "GunnerCrew"; nMax=sti(RealShips[iShip].GunnerCrew); break;
		case 3: sCrew = "CannonerCrew"; nMax=sti(RealShips[iShip].CannonerCrew); break;
	}

	n = 1;
	if( XI_IsKeyPressed("control") ) {n=10;}
	if( XI_IsKeyPressed("alt") ) {n=100;}
	if( XI_IsKeyPressed("shift") ) {n=sti(refEnepchararacter.ship.crew.(sCrew));}

	if( sti(pchar.ship.crew.(sCrew)) + iCrewAdd + n > nMax ) {n = nMax - sti(pchar.ship.crew.(sCrew)) - iCrewAdd}

	if(sti(refEnepchararacter.ship.crew.(sCrew)) - iCrewAdd - n < 0)
	{
		n = sti(refEnepchararacter.ship.crew.(sCrew)) - iCrewAdd;
	}

	//if( n <= 0 ) {return;}

	iCrewAdd = iCrewAdd + n;

	/*pchar.ship.crew.(sCrew) = sti(pchar.ship.crew.(sCrew)) + n;
	refEnepchararacter.ship.crew.(sCrew) = sti(refEnepchararacter.ship.crew.(sCrew)) - n;
	pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) + n;
	refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) - n;
	*/

	UpdateSurrenderCrewList(pchar);
}

void RemoveSurrenderCrewFromShip()
{
	int n,i,nMin;
	string sattr,sCrew;

	switch(iStep)
	{
		case 0: sCrew = "SailorCrew"; break;
		case 1: sCrew = "BoardingCrew"; break;
		case 2: sCrew = "GunnerCrew"; break;
		case 3: sCrew = "CannonerCrew"; break;
	}

	n = 1;
	if( XI_IsKeyPressed("control") ) {n=10;}
	if( XI_IsKeyPressed("alt") ) {n=100;}
	if( XI_IsKeyPressed("shift") ) {n=sti(pchar.ship.crew.(sCrew)) + iCrewAdd}

	//i = sti(GameInterface.CREWSURRENDER_SHIPLIST.current);
	//sattr = "pic" + (i+1);
	sattr = "mycrew" + 0;
	nMin = sti(oTmpData.(sattr).(sCrew));
	if( sti(pchar.ship.crew.(sCrew)) + iCrewAdd - n < nMin ) {n = sti(pchar.ship.crew.(sCrew)) + iCrewAdd - nMin;}
	
	//if( n <= 0 ) {return;}


	iCrewAdd = iCrewAdd - n;
	//pchar.ship.crew.(sCrew) = sti(pchar.ship.crew.(sCrew)) - n;
	//refEnepchararacter.ship.crew.(sCrew) = sti(refEnepchararacter.ship.crew.(sCrew)) + n;
	//pchar.ship.crew.quantity = sti(pchar.ship.crew.quantity) - n;
	//refEnepchararacter.ship.crew.quantity = sti(refEnepchararacter.ship.crew.quantity) + n;

	UpdateSurrenderCrewList(pchar);
}

void UpdateSurrenderCrewList(ref chref)
{
	int iOurShip = sti(pchar.ship.type);
	int iEnemyShip = sti(refEnepchararacter.ship.type);

	string sText1, sText2;
	switch(iStep)
	{
		case 0:
			sText1 = (sti(pchar.ship.crew.SailorCrew) + iCrewAdd) + " / " + sti(Realships[iOurShip].SailorCrew);
			sText2 = (sti(refEnepchararacter.ship.crew.SailorCrew) - iCrewAdd) + " / " + sti(Realships[iEnemyShip].SailorCrew);
		break;

		case 1:
			sText1 = (sti(pchar.ship.crew.BoardingCrew) + iCrewAdd) + " / " + sti(Realships[iOurShip].BoardingCrew);
			sText2 = (sti(refEnepchararacter.ship.crew.BoardingCrew) - iCrewAdd) + " / " + sti(Realships[iEnemyShip].BoardingCrew);
		break;

		case 2:
			sText1 = (sti(pchar.ship.crew.GunnerCrew) + iCrewAdd) + " / " + sti(Realships[iOurShip].GunnerCrew);
			sText2 = (sti(refEnepchararacter.ship.crew.GunnerCrew) - iCrewAdd) + " / " + sti(Realships[iEnemyShip].GunnerCrew);
		break;

		case 3:
			sText1 = (sti(pchar.ship.crew.CannonerCrew) + iCrewAdd) + " / " + sti(Realships[iOurShip].CannonerCrew);
			sText2 = (sti(refEnepchararacter.ship.crew.CannonerCrew) - iCrewAdd) + " / " + sti(Realships[iEnemyShip].CannonerCrew);
		break;
	}

	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 10,iStep,&sText1);
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 10,iStep,&sText2);


	if(iCrewAdd > 0) 
	{
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 2,
			"#"+XI_ConvertString("Take goods")+": "+iCrewAdd );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 3,
			"#"+XI_ConvertString("Give goods")+": 0" );
	} 
	else 
	{
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 2,
			"#"+XI_ConvertString("Take goods")+": 0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_STRINGS", 1, 3,
			"#"+XI_ConvertString("Give goods")+": "+(-iCrewAdd) );
	}

	if(iCrewAdd != 0)
	{
		SetSelectable("CREWSURRENDER_KILL_BUTTON", false);
		SetSelectable("CREWSURRENDER_SLAVE_BUTTON", false);
		SetClickable("CREWSURRENDER_TRANSFER_OK", true);
		SetClickable("CREWSURRENDER_TRANSFER_CANCEL", true);
		SetNodeUsing("TOP_HIDER",true);
		SetNodeUsing("BOTTOM_HIDER",true);

		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 13, true);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 13, true);
	}
	else
	{
		SetSelectable("CREWSURRENDER_KILL_BUTTON", true);
		SetSelectable("CREWSURRENDER_SLAVE_BUTTON", true);
		SetClickable("CREWSURRENDER_TRANSFER_OK", false);
		SetClickable("CREWSURRENDER_TRANSFER_CANCEL", false);
		SetNodeUsing("TOP_HIDER",false);
		SetNodeUsing("BOTTOM_HIDER",false);

		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_QUANTITY", 13, false);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"STORCREW_EQUANTITY", 13, false);
	}
}

void SurrenderCrewUpdateCounterPos()
{
	int nY = SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"SHIPCREW_LIST", 12,iStep);

	if( nY < 0 ) {return;} // warning!!!


	int pic_top;
	int pic_bottom;
	
	//position = 334,286,470,304
	pic_top = nY - 1;
	pic_bottom = nY + 18;
	
	SendMessage(&GameInterface,"lsllllll",MSG_INTERFACE_MSG_TO_NODE,"CREWSURRENDER_COUNTER", -1, 4, 334,pic_top, 470,pic_bottom);

	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"TOP_HIDER", 0, 254,284,553,nY-2);
	SendMessage( &GameInterface,"lslllll",MSG_INTERFACE_MSG_TO_NODE,"BOTTOM_HIDER", 0, 254,nY+19,553,370);
}

void ActCrewSurrender()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();

	// оставшаяся команда берется в плен
	if( sNodName == "CREWSURRENDER_SLAVE_BUTTON" ) 
	{
		int nq = sti(refEnepchararacter.ship.crew.quantity);
		int nmax = GetSquadronFreeSpace(pchar,GOOD_SLAVES);
		if( nq > nmax ) {nq = nmax;}
		AddCharacterGoods(pchar,GOOD_SLAVES,nq);

		refEnepchararacter.ship.crew.SailorCrew = 0;
		refEnepchararacter.ship.crew.BoardingCrew = 0;
		refEnepchararacter.ship.crew.GunnerCrew = 0;
		refEnepchararacter.ship.crew.CannonerCrew = 0;
		refEnepchararacter.ship.crew.Quantity = 0;
	}
	if( sNodName == "CREWSURRENDER_KILL_BUTTON" ) 
	{
		refEnepchararacter.ship.crew.SailorCrew = 0;
		refEnepchararacter.ship.crew.BoardingCrew = 0;
		refEnepchararacter.ship.crew.GunnerCrew = 0;
		refEnepchararacter.ship.crew.CannonerCrew = 0;
		refEnepchararacter.ship.crew.Quantity = 0;
	}

	EndCrewSurrenderWindow();
}

void ActCrewLeave()
{
	EndCrewSurrenderWindow();
}

void EndCrewSurrenderWindow()
{
	XI_WindowDisable("CREWSURRENDER_WINDOW", true);
	XI_WindowShow("CREWSURRENDER_WINDOW", false);

	StartQuestPassengerFound();
}

void StartQuestPassengerFound()
{
	// убрать всю оставшуюся команду

	int questchr = -1;
	if( CheckAttribute(refEnepchararacter,"quest.sink_quest.passenger") ) {
		questchr = GetCharacterIndex( refEnepchararacter.quest.sink_quest.passenger );
	}
	if( questchr >= 0 ) {
		XI_WindowDisable("QUESTPASSENGER_WINDOW", false);
		XI_WindowShow("QUESTPASSENGER_WINDOW", true);
		SetFormatedText("QUESTPASSENGER_TEXT",XI_ConvertString("YouFindQuestPassenger")+GetCharacterFullName(Characters[questchr].id));
		SetNewPicture("QUESTPASSENGER_PICTURE", "interfaces\portraits\256\face_" + Characters[questchr].FaceId + ".tga");
		SetCurrentNode("QUESTPASSENGER_OK_BUTTON");
	} else {
		StartQuestItemFound();
	}
}

void ActQuestPassenger()
{
	XI_WindowDisable("QUESTPASSENGER_WINDOW", true);
	XI_WindowShow("QUESTPASSENGER_WINDOW", false);

	StartQuestItemFound();
}

void StartQuestItemFound()
{
	int nItmIdx = -1;
	if( CheckAttribute(refEnepchararacter,"quest.sink_quest.item") ) {
		nItmIdx = FindItem(refEnepchararacter.quest.sink_quest.item);
	}
	if( nItmIdx >= 0 ) {
		XI_WindowDisable("QUESTITEM_WINDOW", false);
		XI_WindowShow("QUESTITEM_WINDOW", true);
		int nLangFile = LanguageOpenFile( "ItemsDescribe.txt" );
		SetFormatedText("QUESTITEM_TEXT",XI_ConvertString("YouFindQuestItem")+LanguageConvertString(nLangFile,Items[nItmIdx].name));
		LanguageCloseFile(nLangFile);
		SetNewGroupPicture("QUESTITEM_PICTURE", Items[nItmIdx].picTexture, Items[nItmIdx].id);
		SetCurrentNode("QUESTITEM_OK_BUTTON");
	} 
	else 
	{
		StartMainWindow();
	}
}

void ActQuestItem()
{
	XI_WindowDisable("QUESTITEM_WINDOW", true);
	XI_WindowShow("QUESTITEM_WINDOW", false);

	StartMainWindow();
}

//============================================================
//============================================================
// Tooltip Interfaces
//============================================================
void procToolTip()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();

	if( sti(oTmpData.tooltip)!=0 ) {return;} // уже запущено окно тултипа
	oTmpData.tooltip = 1;

	if(XI_IsWindowEnable("MAIN_WINDOW") == true)
	{
		switch( sNodName )
		{
			case "MW_OUR_UPGRADE_HULL": LaunchUpgradeToolTip(pchar,0); break;
			case "MW_OUR_UPGRADE_SAIL": LaunchUpgradeToolTip(pchar,1); break;
			case "MW_OUR_UPGRADE_CANNON": LaunchUpgradeToolTip(pchar,2); break;
			case "MW_OTHER_UPGRADE_HULL": LaunchUpgradeToolTip(refEnepchararacter,0); break;
			case "MW_OTHER_UPGRADE_SAIL": LaunchUpgradeToolTip(refEnepchararacter,1); break;
			case "MW_OTHER_UPGRADE_CANNON": LaunchUpgradeToolTip(refEnepchararacter,2); break;

			case "SHIPTRADELIST": LaunchGoodsToolTip(true); break;
			case "SHIPQUANTITYLIST": LaunchGoodsToolTip(true); break;
			case "STOREQUANTITYLIST": LaunchGoodsToolTip(true); break;
			case "MW_GOOD_COUNTER": LaunchGoodsToolTip(false); break;
			case "TOP_HIDER": LaunchGoodsToolTip(false); break;
			case "BOTTOM_HIDER": LaunchGoodsToolTip(false); break;

			case "MW_OUR_SHIP_IMAGE": LaunchShipToolTip(pchar); break;
			case "MW_OTHER_SHIP_IMAGE": LaunchShipToolTip(refEnepchararacter); break;

			case "SC_CREWLIST": LaunchCrewToolTip(refEnepchararacter,GetTableLine("SC_CREWLIST")); break;
			case "SC_CREW_COUNTER": LaunchCrewToolTip(refEnepchararacter,GetTableLine("SC_CREWLIST")); break;
		}
	}

	if(XI_IsWindowEnable("CREWSURRENDER_WINDOW") == true)
	{
		switch( sNodName )
		{
			case "SHIPCREW_LIST": LaunchCrewToolTip(&characters[sti(oTmpData.crewsurrender_curcaptain)],iStep); break;
			case "SHIPCREW_QUANTITY": LaunchCrewToolTip(&characters[sti(oTmpData.crewsurrender_curcaptain)],iStep); break;
			case "STORCREW_EQUANTITY": LaunchCrewToolTip(&characters[sti(oTmpData.crewsurrender_curcaptain)],iStep); break;
		}
	}
}

int GetTableLine(string sTableControlName)
{
	int nLine = SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,sTableControlName, 1 );
	if( nLine<0 ) {nLine = sti(GameInterface.(sTableControlName).select) - 1;}
	return nLine;
}

void EndTooltip()
{
	if( sti(oTmpData.tooltip)==0 ) {return;} // неn окна тултипа
	oTmpData.tooltip = 0;
	CloseTooltip();
}

void LaunchUpgradeToolTip(ref chref,int nUpgr)
{
	string sHeader,sGroupName,sPicName,sUpgradeDescr;
	string sText2 = "";

	switch( nUpgr )
	{
	case 0:
		sGroupName = "HULL_UPGRADE_" + chref.ship.upgrades.hull;
		sPicName = "hull";
		sHeader = XI_ConvertString("HullUpgrade");
		sUpgradeDescr = XI_ConvertString("UpgradeShipInfoHull"+chref.ship.upgrades.hull);
	break;
	case 1:
		sGroupName = "SAILS_UPGRADE_" + chref.ship.upgrades.sails;
		sPicName = "sails";
		sHeader = XI_ConvertString("SailsUpgrade");
		sUpgradeDescr = XI_ConvertString("UpgradeShipInfoSails"+chref.ship.upgrades.sails);
	break;
	case 2:
		sGroupName = "CANNONS_UPGRADE_" + chref.ship.upgrades.cannons;
		sPicName = Cannon[sti(chref.ship.cannons.type)].picture;
		sHeader = XI_ConvertString("CannonsUpgrade");
		sUpgradeDescr = XI_ConvertString("UpgradeShipInfoCannons"+chref.ship.upgrades.cannons);

		int iOurCannonType = sti(chref.ship.cannons.type);
		sPicName = Cannon[iOurCannonType].picture;
		sGroupName = "CANNONS_UPGRADE_" + chref.ship.upgrades.cannons;
		int iCannonType =  Cannon[sti(chref.ship.cannons.type)].type;
		if(iCannonType == CANNON_NAME_CULVERINE)
		{
			sText2 = XI_ConvertString("cannon type") + ": " + XI_ConvertString("Culverine");
		}
		if(iCannonType == CANNON_NAME_CANNON)
		{
			sText2 = XI_ConvertString("cannon type") + ": " + XI_ConvertString("Cannon");
		}

		sText2 = sText2 + "\n" + XI_ConvertString("Caliber_info");
		sText2 = sText2 + ": " + XI_ConvertString(Cannon[sti(chref.ship.cannons.type)].name);
	break;
	}

	CreateTooltip("#"+sHeader, sUpgradeDescr,argb(255,255,255,255), sText2,argb(255,255,255,255), "",0, "",0, "", sGroupName, sPicName, 64, 64);
}

void LaunchGoodsToolTip(bool bGetRealTableLine)
{
	iGoodIndex = sti(GoodsList[iStep].index);
	string sGoodName = Goods[iGoodIndex].name;

	int lngFileID = LanguageOpenFile("GoodsDescribe.txt");
	string sHeader = XI_ConvertString(sGoodName);

	string sGoodsDescr = GetAssembledString( LanguageConvertString(lngFileID,sGoodName+"_descr"), &Goods[iGoodIndex]);

	string sText3;
	int iColor3 = argb(255,255,255,255);
	if(checkAttribute(pchar, "ship.cargo.goods." + sGoodName + ".isquest"))
	{
		string sTradeQ = pchar.ship.cargo.goods.(sGoodName).isquest.quantity;
		string sColony = pchar.ship.cargo.goods.(sGoodName).isquest.colony;
		sText3 = XI_ConvertString("YouNeedToDelivery") + sTradeQ + XI_ConvertString("QuantityOfGoodsToColony") + XI_ConvertString("Colony"+sColony) + ".";
		iColor3 = argb(255,255,196,255);
	} 

	string sText2 = "";
	if(sGoodName == "Food")
	{
		int ifood = CalculateFood();
		sText2 = XI_ConvertString("FoodSpare") + FindRussianDaysString(ifood) + ".";
	}

	LanguageCloseFile(lngFileID);

	CreateTooltip("#"+sHeader, sGoodsDescr,argb(255,255,255,255), sText2,argb(255,255,255,255), sText3,iColor3, "",0, "", "GOODS", sGoodName, 64, 64);
}

void LaunchShipToolTip(ref chref)
{
	int iRShipType = sti(chref.ship.type);
	int iBShipType = sti(RealShips[iRShipType].basetype);
	string sHeader = XI_ConvertString(ShipsTypes[iBShipType].Name);
	string sGroupName = "SHIPS_"+ShipsTypes[iBShipType].Name;
	string sShipDescr = XI_ConvertString(ShipsTypes[iBShipType].Name+"_descr");
	string sShipParamString = XI_ConvertString("ShipClass") + ": " + ShipsTypes[iBShipType].class + "\n" +
		XI_ConvertString("Hull") + ": " + sti(chref.ship.hp) + " / " + sti(RealShips[iRShipType].hp) + "\n" +
		XI_ConvertString("Sails") + ": " + sti(chref.ship.sp) + " / " + sti(RealShips[iRShipType].sp) + "\n" +
		XI_ConvertString("Cannons") + ": " + GetCannonsNum(chref) + " / " + sti(RealShips[iRShipType].CannonsQuantity) + "\n" +
		XI_ConvertString("Speed") + ": " + FloatToString(stf(RealShips[iRShipType].speedrate),2) + "\n" +
		XI_ConvertString("Maneuver") + ": " + FloatToString(stf(RealShips[iRShipType].turnrate),2) + "\n" +
		XI_ConvertString("Capacity") + ": " + RealShips[iRShipType].Capacity + "\n" +
		XI_ConvertString("Max.Crew") + ": " + RealShips[iRShipType].MaxCrew;

	CreateTooltip("#"+sHeader, sShipDescr,argb(255,255,255,255), sShipParamString,argb(255,255,255,255), "",0, "",0, "", sGroupName,"ship", 64,64);
}

void LaunchCrewToolTip(ref chref,int nCrewIdx)
{
	string sHeader, sText1, sText2;

	switch(nCrewIdx)
	{
	case 0:
		sHeader = XI_ConvertString("sailors");
		sText1 = XI_ConvertString("sailors_descr");
		sText2 = XI_ConvertString("Sailors_max") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].SailorCrew), "acc") + "\n" + XI_ConvertString("Sailors_min") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].SailorCrew)/10, "acc");
		break;
	case 1:
		sHeader = XI_ConvertString("soldiers");
		sText1 = XI_ConvertString("soldiers_descr");
		sText2 = XI_ConvertString("soldiers_max") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].BoardingCrew), "acc") + "\n" + XI_ConvertString("Soldiers_min") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].BoardingCrew)/10, "acc");
		break;
	case 2:
		sHeader = XI_ConvertString("musketeers");
		sText1 = XI_ConvertString("musketeers_descr");
		sText2 = XI_ConvertString("musketeers_max") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].GunnerCrew), "acc") + "\n" + XI_ConvertString("musketeers_min") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].GunnerCrew)/10, "acc");
		break;
	case 3:
		sHeader = XI_ConvertString("cannoners");
		sText1 = XI_ConvertString("cannoners_descr");
		sText2 = XI_ConvertString("cannoners_max") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].CannonerCrew), "acc") + "\n" + XI_ConvertString("Cannoners_min") + FindRussianPeoplesString(sti(RealShips[sti(chref.ship.type)].CannonerCrew)/10, "acc");
		break;
	}

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,255,255), "",0, "",0, "", "","", 64, 64);
}

void FTChange()
{
	string sTemp = GetEventData();
	float newPos = GetEventData();
	float fStep;

	if (stemp == "SHIPTRADELIST" || sTemp == "SHIPQUANTITYLIST" || stemp == "STOREQUANTITYLIST")
	{
		fStep = iStringsQuantity*newPos;
		fStep = fStep + 0.5;
		iStep = sti(fStep);
		if(iPrevStep != iStep)
		{
			iGoodIndex = sti(GoodsList[iStep].index);

			UpdateCounterPos();
			SaveCurrentGoodsData(true);
			SetGoodsTakeStrings();
			iPrevStep = iStep;
			return;
		}
	}

	if (stemp == "SHIPCREW_LIST" || sTemp == "SHIPCREW_QUANTITY" || stemp == "STORCREW_EQUANTITY")
	{
		fStep = 3 * newPos;
		fStep = fStep + 0.5;
		iStep = sti(fStep);
		if(iPrevStep != iStep)
		{
			//iGoodIndex = sti(GoodsList[iStep].index);

			iCrewAdd = 0;
			SurrenderCrewUpdateCounterPos();
			//SaveCurrentGoodsData(true);
			iPrevStep = iStep;
			return;
		}
	}

	if( sTemp=="CREWLIST" || sTemp=="OURCREWLIST" || sTemp=="OTHERCREWLIST" )
	{
		SetCaptainUpdateCounterPos();
	}
}


void ConfirmCrewTransfer()
{
	switch (iStep)
	{
		case 0:
			pchar.ship.crew.SailorCrew = sti(pchar.ship.crew.SailorCrew) + iCrewAdd;
			refEnepchararacter.ship.crew.SailorCrew = sti(refEnepchararacter.ship.crew.SailorCrew) - iCrewAdd;
		break;
		case 1:
			pchar.ship.crew.BoardingCrew = sti(pchar.ship.crew.BoardingCrew) + iCrewAdd;
			refEnepchararacter.ship.crew.BoardingCrew = sti(refEnepchararacter.ship.crew.BoardingCrew) - iCrewAdd;
		break;
		case 2:
			pchar.ship.crew.GunnerCrew = sti(pchar.ship.crew.GunnerCrew) + iCrewAdd;
			refEnepchararacter.ship.crew.GunnerCrew = sti(refEnepchararacter.ship.crew.GunnerCrew) - iCrewAdd;
		break;
		case 3:
			pchar.ship.crew.CannonerCrew = sti(pchar.ship.crew.CannonerCrew) + iCrewAdd;
			refEnepchararacter.ship.crew.CannonerCrew = sti(refEnepchararacter.ship.crew.CannonerCrew) - iCrewAdd;
		break;
	}

	iCrewAdd = 0;

	UpdateSurrenderCrewList(pchar);

	SetCrewForACharacter(pchar);

}

void CancelCrewTransfer()
{
	iCrewAdd = 0;
	UpdateSurrenderCrewList(pchar);
}