int iSkillAccuracy = 0, iSkillCannons = 0, iSkillSailing = 0, iSkillDefence = 0, iSkillRepair = 0;

int BuyClients[NET_TOTALCLIENTS];

int iCurrentCredit = 0, iMaxCredit = 0;
int iSkillsQuantity[5];
int iNetGoodsQuantity[6];
int iPerks[6];
int iCurrentShipType = -1;
int iCurrentShipUpgradeHull = -1;
int iCurrentShipUpgradeRigging = -1;
int iCurrentCannonType = -1;
int iCurrentCannonUpgrade = -1;
int iSelectedTeam = -1;
int iNumClients = 0;

int iClientStartTimer = 10000;
bool bStartCountDown = false;
bool bSelectNewTeamWindow = false;

float fServerInfoPiecesLoaded = 0.0;

bool bServerInfoReady = false;

void InitInterface(string iniName)
{
	int i;

	bServerInfoReady = false;

	GameInterface.title = "titleNetBuy";

	Buy_InitTable("TABLE_PLAYERS");
	Buy_InitTable("TABLE_STARTGAME_WAIT_PLAYERS");

	GameInterface.TABLE_SELECTTEAM.hr.td1.str = "";
	GameInterface.TABLE_SELECTTEAM.hr.td2.str = XI_ConvertString("capNetBuy_TableSelectTeamQuantity");

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	SetEventHandler("CheckButtonChange", "Buy_CheckButtonChange", 0);

	SetEventHandler("OnTmpStart", "OnTmpStart", 0);
	SetEventHandler("OnReset", "OnReset", 0);
	SetEventHandler("OnDisconnect", "OnDisconnect", 0);

	SetEventHandler("ShipLeft", "Buy_ShipLeft", 0);
	SetEventHandler("ShipRight", "Buy_ShipRight", 0);

	SetEventHandler("AddUpgradeHull", "Buy_AddUpgradeHull", 0);			SetEventHandler("DecUpgradeHull", "Buy_DecUpgradeHull", 0);
	SetEventHandler("AddUpgradeRigging", "Buy_AddUpgradeRigging", 0);	SetEventHandler("DecUpgradeRigging", "Buy_DecUpgradeRigging", 0);

	SetEventHandler("CannonLeft", "Buy_CannonLeft", 0);
	SetEventHandler("CannonRight", "Buy_CannonRight", 0);

	SetEventHandler("AddAccuracy", "Buy_AddAccuracy", 0);	SetEventHandler("DecAccuracy", "Buy_DecAccuracy", 0);
	SetEventHandler("AddCannons", "Buy_AddCannons", 0);		SetEventHandler("DecCannons", "Buy_DecCannons", 0);
	SetEventHandler("AddSailing", "Buy_AddSailing", 0);		SetEventHandler("DecSailing", "Buy_DecSailing", 0);
	SetEventHandler("AddDefence", "Buy_AddDefence", 0);		SetEventHandler("DecDefence", "Buy_DecDefence", 0);
	SetEventHandler("AddRepair", "Buy_AddRepair", 0);		SetEventHandler("DecRepair", "Buy_DecRepair", 0);

	SetEventHandler("AddBalls", "Buy_AddBalls", 0);			SetEventHandler("DecBalls", "Buy_DecBalls", 0);
	SetEventHandler("AddGrapes", "Buy_AddGrapes", 0);		SetEventHandler("DecGrapes", "Buy_DecGrapes", 0);
	SetEventHandler("AddKnippels", "Buy_AddKnippels", 0);	SetEventHandler("DecKnippels", "Buy_DecKnippels", 0);
	SetEventHandler("AddBombs", "Buy_AddBombs", 0);			SetEventHandler("DecBombs", "Buy_DecBombs", 0);
	SetEventHandler("AddPlanks", "Buy_AddPlanks", 0);		SetEventHandler("DecPlanks", "Buy_DecPlanks", 0);
	SetEventHandler("AddSailcloth", "Buy_AddSailcloth", 0);	SetEventHandler("DecSailcloth", "Buy_DecSailcloth", 0);

	SetEventHandler("OnTableClick", "Buy_OnTableClick", 0);
	SetEventHandler("NCEvent_PlayerInfo_ShipType", "Buy_PlayerInfo_ShipType", 0);
	SetEventHandler("NCEvent_PlayerInfo_BuyReady", "Buy_PlayerInfo_BuyReady", 0);
	SetEventHandler("NCEvent_PlayerInfo_Team", "Buy_PlayerInfo_Team", 0);
	SetEventHandler("NCEvent_Client_Connect", "Buy_NewClientConnect", 0);

	SetEventHandler("NCEvent_StartGame", "Buy_StartGame", 0);
	SetEventHandler("NCEvent_StartGame_GetBuyInfo", "Buy_GetBuyInfo", 0);
	SetEventHandler("NCEvent_PlayerInfo_StartCountdown", "Buy_StartCountdown", 0);
	SetEventHandler("NCEvent_PlayerInfo_StopCountdown",  "Buy_StopCountdown", 0);
	SetEventHandler("NCEvent_Disconnect", "Buy_Disconnect", 0);

	SetEventHandler("NCEvent_ClientDrop", "Buy_ClientDrop", 0);

	SetEventHandler("frame", "NetBuy_OnFrame", 0);

	SetEventHandler("OnClickTeam", "NetBuy_OnClickTeam", 0);

	Button_SetEnable("BTN_START", false);
	Button_SetEnable("BTN_RESET", false);

	XI_WindowDisable("WND_SELECT_TEAM", true);
	XI_WindowShow("WND_SELECT_TEAM", false);

	Buy_GetFullServerInfo();

	// temporary disable
	for (i=0; i<Net_GetPerksNum(); i++)
	{
		CheckButton_SetDisable("CHECKBUTTONS_PERKS", i + 1, true);
	}
}

void NetBuy_OnClickTeam()
{
	int iTmp = GetEventData();
	string sControl = GetEventData();

	int iNewTeam = -1;
	switch (sControl)
	{
		case "RECTANGLE_TEAM0":	iNewTeam = 0; break;
		case "RECTANGLE_TEAM1": iNewTeam = 1; break;
		case "RECTANGLE_TEAM2": iNewTeam = 2; break;
		case "RECTANGLE_TEAM3": iNewTeam = 3; break;
	}

	if (iNewTeam < sti(NCServer.NumTeams))
	{
		iSelectedTeam = iNewTeam;
		NetBuy_SelectTeamUpdate();
	}
}

void NetBuy_SelectTeamUpdate()
{
	int i;
	// fill current team colors
	for (i=0; i<4; i++)
	{
		string sTeam = "team" + i;

		int iFrameColor = argb(255, 128, 128, 128);

		SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, "PICTURES_SELECTTEAM", 4, i, sTeam);
		SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "PICTURES_SELECTTEAM", 3, i, argb(255, 128, 128, 128));
		if (i > sti(NCServer.NumTeams) - 1)
		{
			SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "PICTURES_SELECTTEAM", 3, i, argb(255, 30, 30, 30));
			//SendMessage(&GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, "PICTURES_SELECTTEAM", 3, argb(0,0,0,0));	
			iFrameColor = argb(255, 28, 28, 28);
		}

		if (iSelectedTeam == i) { iFrameColor = argb(255, 255, 255, 255); }

		string sTeamFrame = "FRAME_TEAM" + i;
		SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sTeamFrame, 0, iFrameColor);
	}
}

void NetBuy_SelectTeamOk()
{
	// send change team code to server
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_BUY);
	NMAddByte(iSMsg, NSC_BUY_TEAMCHANGE);
	NMAddByte(iSMsg, iSelectedTeam);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
	
	NetBuy_SelectTeamCancel();
}

void NetBuy_SelectTeamCancel()
{
	DelEventHandler("OnSelectTeamOk", "NetBuy_SelectTeamOk");
	DelEventHandler("OnSelectTeamCancel", "NetBuy_SelectTeamCancel");

	XI_WindowDisable("WND_SELECT_TEAM", true);
	XI_WindowShow("WND_SELECT_TEAM", false);

	XI_WindowDisable("WND_NETBUY", false);

	bSelectNewTeamWindow = false;
}

void NetBuy_OpenSelectTeam()
{
	if (bStartCountDown) { return; }

	ref rOurClient = NetClient_GetThisClient();
	iSelectedTeam = sti(rOurClient.Team);

	SetEventHandler("OnSelectTeamOk", "NetBuy_SelectTeamOk", 0);
	SetEventHandler("OnSelectTeamCancel", "NetBuy_SelectTeamCancel", 0);

	XI_WindowDisable("WND_NETBUY", true);

	XI_WindowDisable("WND_SELECT_TEAM", false);
	XI_WindowShow("WND_SELECT_TEAM", true);

	bSelectNewTeamWindow = true;

	SetFormatedText("TEXT_SELECTTEAM", XI_ConvertString("capNetBuy_SelectFreeTeam"));

	NetBuy_SelectTeamUpdate();

	NetBuy_OnFrame();	// update some parameters
}

void NetBuy_OnFrame()
{
	int i;
	int iTeamQuantity[4];

	if (bStartCountDown)
	{
		iClientStartTimer = iClientStartTimer - iRealDeltaTime;
	}
	int iCurTime = iClientStartTimer / 1000;
	if (iCurTime < 0) { iCurTime = 0; }

	string sMinutes, sSeconds;

	int iMinutes = iCurTime / 60;
	int iSeconds = iCurTime - iMinutes * 60;
	if (iMinutes < 10) { sMinutes = "0" + iMinutes; } else { sMinutes = iMinutes; }
	if (iSeconds < 10) { sSeconds = "0" + iSeconds; } else { sSeconds = iSeconds; }
	string sValue = "#" + XI_ConvertString("btnNetBuy_Start") + " " + sMinutes + ":" + sSeconds;
	
	Button_SetText("BTN_START", sValue);

	// set selectteam button
	ref rOurClient = NetClient_GetThisClient();
	Button_SetEnable("BTN_SELECTTEAM_OK", !(iSelectedTeam == sti(rOurClient.Team)));

	// update team count
	for (i=0; i<4; i++) { iTeamQuantity[i] = 0;	}
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{
		if ( sti(NCClients[i].Use) )
		{
			int iTeam = sti(NCClients[i].Team);
			if (iTeam >= 0)	{ iTeamQuantity[iTeam] = iTeamQuantity[iTeam] + 1; }
		}
	}

	for (i=0; i<4; i++) 
	{ 
		if (i >= sti(NCServer.NumTeams)) 
			{ StringCollection_SetText("STRINGS_SELECTTEAM_QUANTITY", i + 1, "#"); }
		else 
			{ StringCollection_SetTextValue("STRINGS_SELECTTEAM_QUANTITY", i + 1, iTeamQuantity[i]); }
	}
}

void Buy_InitTable(string sTable)
{
	GameInterface.(sTable).hr.td1.str = XI_ConvertString("capNetBuy_TableFace");
	GameInterface.(sTable).hr.td1.scale = 0.85;
	GameInterface.(sTable).hr.td2.str = XI_ConvertString("capNetBuy_TableShip");
	GameInterface.(sTable).hr.td2.scale = 0.85;
	GameInterface.(sTable).hr.td3.str = XI_ConvertString("capNetBuy_TableNames");
	GameInterface.(sTable).hr.td3.scale = 0.85;
	GameInterface.(sTable).hr.td4.str = "";
	GameInterface.(sTable).hr.td5.str = XI_ConvertString("capNetBuy_TableTeams");
	GameInterface.(sTable).hr.td5.scale = 0.85;
	GameInterface.(sTable).hr.td6.str = XI_ConvertString("capNetBuy_TableReady");
	GameInterface.(sTable).hr.td6.scale = 0.85;
	GameInterface.(sTable).hr.td7.str = XI_ConvertString("capNetBuy_TableKick");
	GameInterface.(sTable).hr.td7.scale = 0.85;
}

void Buy_ClientDrop()
{
	Buy_FillPlayersTable("TABLE_PLAYERS");
	Buy_UpdatePlayersQuantity();
}

void Buy_NewClientConnect()
{
	Buy_FillPlayersTable("TABLE_PLAYERS");
	Buy_UpdatePlayersQuantity();
}

void Buy_PlayerInfo_BuyReady()
{
	Buy_FillPlayersTable("TABLE_PLAYERS");
}

void Buy_PlayerInfo_StartReady()
{
	Buy_FillPlayersTable("TABLE_STARTGAME_WAIT_PLAYERS");
}

void Buy_PlayerInfo_Team()
{
	Buy_FillPlayersTable("TABLE_PLAYERS");
}

void Buy_PlayerInfo_ShipType()
{
	int wNetClientID = GetEventData();
	int iShipType = GetEventData();

	Buy_FillPlayersTable("TABLE_PLAYERS");
}

void Buy_GetFullServerInfo()
{
	XI_WindowDisable("WND_NETBUY", true);

	XI_WindowShow("WND_STARTGAME_WAIT", false);
	XI_WindowDisable("WND_STARTGAME_WAIT", true);

	XI_WindowShow("WND_NETBUY_WAITINFO", true);
	XI_WindowDisable("WND_NETBUY_WAITINFO", false);

	SetCurrentNode("BTN_WAITINFO_DISCONNECT");

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_GET);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);

	fServerInfoPiecesLoaded = 0;
	SetEventHandler("NetClient_OnServerInfo_Event", "Buy_OnServerInfo", 0);
}

void Buy_OnServerInfo()
{
	int i;
	int iCode = GetEventData();
	
	fServerInfoPiecesLoaded = fServerInfoPiecesLoaded + 1.0;
	float fMaxPieces = 5.0 + stf(NCServer.NumClients) + stf(Net_GetShipsNum());

	int left = 180;
	int right = MakeInt(left + (640 - left) * (fServerInfoPiecesLoaded / fMaxPieces));

	SendMessage(&GameInterface, "lslllll", MSG_INTERFACE_MSG_TO_NODE, "RECTANGLE_WAITINFO_PROGRESS", 0, left, 273, right, 299);

	if (fServerInfoPiecesLoaded >= fMaxPieces)
	{
		bServerInfoReady = true;

		iMaxCredit = sti(NCServer.Credit);

		XI_WindowDisable("WND_NETBUY", false);

		XI_WindowShow("WND_NETBUY_WAITINFO", false);
		XI_WindowDisable("WND_NETBUY_WAITINFO", true);

		// update skills costs 
		for (i=0; i<5; i++)	
		{ 
			iSkillsQuantity[i] = 1;
			StringCollection_SetTextValue("STRINGS_SKILLS_QUANTITY", i + 1, iSkillsQuantity[i]); 
			StringCollection_SetTextValue("STRINGS_SKILLS_MONEY", i + 1, Net_GetSkillCost(i, 1)); 
		}

		// update perks costs 
		for (i=0; i<6; i++)	
		{ 
			iPerks[i] = 0; 
			StringCollection_SetTextValue("STRINGS_PERKS_MONEY", i + 1, Net_GetPerkCost(i)); 
		}

		// update goods costs 
		for (i=0; i<6; i++)	
		{ 
			iNetGoodsQuantity[i] = 0;
			StringCollection_SetTextValue("STRINGS_AMMO_MONEY", i + 1, Net_GetGoodCost(i, sti(NetGoods[i].Units))); 
			StringCollection_SetTextValue("STRINGS_AMMO_QUANTITY", i + 1, iNetGoodsQuantity[i]); 
		}

		Buy_SetShip(1);

		string sIslandMap = "OpenSea";
		if (NCServer.IslandID != "") { sIslandMap = NCServer.IslandID; }
		Picture_SetPicture("SERVER_MAP_PICTURE", "Interfaces\Net\Islands\" + sIslandMap + ".tga");
		SetFormatedText("CAPTION_MAP_NAME", XI_ConvertString(sIslandMap));

		Buy_FillPlayersTable("TABLE_PLAYERS");
		Buy_UpdatePlayersQuantity();

		StringCollection_SetText("STRINGS_MAP_INFO_VALUE", 1, "txtCS_" + NetModes[sti(NCServer.GameType)].Name);
		StringCollection_SetText("STRINGS_MAP_INFO_VALUE", 4, "#" + NCServer.MaxShipClass);

		Buy_AddGood(1, 2);
	}
}

void OnReset()
{
}

void Buy_SetShipUpgradeHull(int iDirection)
{
	if (iDirection == 0) { return; }
	iDirection = Clampi(iDirection, -1, 1);

	int iOldPrice = Net_GetShipUpgradeHullCost(iCurrentShipType, iCurrentShipUpgradeHull);
	int iNumUpgrades = 3;
	int iNextUpgrade = iCurrentShipUpgradeHull;
	for (int i=0; i<iNumUpgrades; i++)
	{
		iNextUpgrade = iNextUpgrade + iDirection;
		if (iNextUpgrade >= iNumUpgrades) { return; }//iNextUpgrade = 0;
		if (iNextUpgrade < 0) { return; }//iNextUpgrade = iNumUpgrades - 1;

		int iNewPrice = Net_GetShipUpgradeHullCost(iCurrentShipType, iNextUpgrade);

		if (iCurrentCredit - iOldPrice + iNewPrice < iMaxCredit)
		{
			iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;
			iCurrentShipUpgradeHull = iNextUpgrade;

			string sCurrentUpgrade = "HULL_UPGRADE_" + (iCurrentShipUpgradeHull + 1);
			Picture_SetGroupPicture("PICTURE_SHIP_UPGRADE_HULL", sCurrentUpgrade, "hull");
			StringCollection_SetTextValue("STRINGS_SHIP_UPGRADE_MONEY", 1, iNewPrice);
			Buy_UpdateCredit();
			break;
		}
	}
}
void Buy_DecUpgradeHull() { Buy_SetShipUpgradeHull(-1); }
void Buy_AddUpgradeHull() { Buy_SetShipUpgradeHull(1); }

void Buy_SetShipUpgradeRigging(int iDirection)
{
	if (iDirection == 0) { return; }
	iDirection = Clampi(iDirection, -1, 1);

	int iOldPrice = Net_GetShipUpgradeRiggingCost(iCurrentShipType, iCurrentShipUpgradeRigging);
	int iNumUpgrades = 3;
	int iNextUpgrade = iCurrentShipUpgradeRigging;
	for (int i=0; i<iNumUpgrades; i++)
	{
		iNextUpgrade = iNextUpgrade + iDirection;
		if (iNextUpgrade >= iNumUpgrades) { return; }//iNextUpgrade = 0;
		if (iNextUpgrade < 0) { return; }//iNextUpgrade = iNumUpgrades - 1;

		int iNewPrice = Net_GetShipUpgradeRiggingCost(iCurrentShipType, iNextUpgrade);

		if (iCurrentCredit - iOldPrice + iNewPrice < iMaxCredit)
		{
			iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;
			iCurrentShipUpgradeRigging = iNextUpgrade;

			string sCurrentUpgrade = "SAILS_UPGRADE_" + (iCurrentShipUpgradeRigging + 1);
			Picture_SetGroupPicture("PICTURE_SHIP_UPGRADE_RIGGING", sCurrentUpgrade, "sails");
			StringCollection_SetTextValue("STRINGS_SHIP_UPGRADE_MONEY", 2, iNewPrice);
			Buy_UpdateCredit();
			break;
		}
	}
}
void Buy_DecUpgradeRigging() { Buy_SetShipUpgradeRigging(-1); }
void Buy_AddUpgradeRigging() { Buy_SetShipUpgradeRigging(1); }

void Buy_SetShip(int iDirection)
{
	if (iDirection == 0) { return; }
	iDirection = Clampi(iDirection, -1, 1);

	int iOldCannonsNum = Net_GetShipCannonsNum(iCurrentShipType);
	int iOldPrice = Net_GetShipCost(iCurrentShipType)

	int iMaxShipClass = sti(NCServer.MaxShipClass);

	// find next ship
	int iNumShips = Net_GetShipsNum();
	int iNextShipType = iCurrentShipType;
	for (int i=0; i<iNumShips; i++)
	{
		iNextShipType = iNextShipType + iDirection;
		if (iNextShipType >= iNumShips) { return; }//iNextShipType = 0;
		if (iNextShipType < 0) { return; }//iNextShipType = iNumShips - 1;
		
		ref rShip = Net_GetShipByIndex(iNextShipType);
		if (sti(rShip.TradeOff)) { continue; }
		if (sti(rShip.Class) < iMaxShipClass) { continue; }
		int iNewPrice = Net_GetShipCost(iNextShipType);

		if (iCurrentCredit - iOldPrice + iNewPrice < iMaxCredit)
		{
			if (iCurrentShipType >= 0)
			{
				// sell old cannons and cannons upgrade
				if (iCurrentCannonType >= 0)
				{
					iCurrentCredit = iCurrentCredit - iOldCannonsNum * Net_GetCannonCost(iCurrentCannonType);
					iCurrentCannonType = -1;

					// sell old cannon upgrade
					if (iCurrentCannonUpgrade >= 0)
					{
						iCurrentCredit = iCurrentCredit - iOldCannonsNum * Net_GetCannonUpgradeCost(iCurrentCannonType, iCurrentCannonUpgrade);
						iCurrentCannonUpgrade = -1;
					}
				}

				// sell old hull upgrades
				if (iCurrentShipUpgradeHull >= 0)
				{
					iCurrentCredit = iCurrentCredit - Net_GetShipUpgradeHullCost(iCurrentShipType, iCurrentShipUpgradeHull);
					iCurrentShipUpgradeHull = -1;
				}

				// sell old rigging upgrades
				if (iCurrentShipUpgradeRigging >= 0)
				{
					iCurrentCredit = iCurrentCredit - Net_GetShipUpgradeRiggingCost(iCurrentShipType, iCurrentShipUpgradeRigging);
					iCurrentShipUpgradeRigging = -1;
				}
		}

			// buy new ship
			iCurrentShipType = iNextShipType;
			iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;

			string sGroup = "SHIPS_" + rShip.Name;
			Picture_SetGroupPicture("PICTURE_SHIP", sGroup, "ship");
			
			// buy new cannons
			iCurrentCannonType = -1; Buy_SetCannon(1);
			// set price of cannons
			StringCollection_SetTextValue("STRINGS_SHIPCANNONS_MONEY", 1, iNewPrice);

			iCurrentShipUpgradeHull = -1;
			iCurrentShipUpgradeRigging = -1;
			Buy_SetShipUpgradeHull(1);
			Buy_SetShipUpgradeRigging(1);

			StringCollection_SetText("STRINGS_SHIPCANNONS_TYPE", 1, rShip.Name);
			Buy_UpdateCredit();
			Buy_Server_UpdateShipType();
			break;
		}
	}
}
void Buy_ShipLeft() { Buy_SetShip(-1); }
void Buy_ShipRight() { Buy_SetShip(1); }

void Buy_SetCannonUpgrade(int iNewCannonUpgrade, bool bSetRadioButton)
{
	int iNumShipCannons = Net_GetShipCannonsNum(iCurrentShipType);
	
	int iOldPrice = iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, iCurrentCannonUpgrade);
	int iNewPrice = iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, iNewCannonUpgrade);

	if (iCurrentCredit - iOldPrice + iNewPrice < iMaxCredit)
	{
		if (iCurrentCannonUpgrade >= 0) { iCurrentCredit = iCurrentCredit - iOldPrice; }

		iCurrentCannonUpgrade = iNewCannonUpgrade;
		iCurrentCredit = iCurrentCredit + iNewPrice;

		if (bSetRadioButton)
		{
			CheckButton_SetState("RADIO_CANNONS_UPGRADELIST", iCurrentCannonUpgrade + 1, true);
		}

		Buy_UpdateCredit();
	}
}

void Buy_SetCannon(int iDirection)
{
	if (iDirection == 0) { return; }
	iDirection = Clampi(iDirection, -1, 1);	

	int iNumShipCannons = Net_GetShipCannonsNum(iCurrentShipType);

	int iOldPrice = iNumShipCannons * Net_GetCannonCost(iCurrentCannonType);

	// find next cannon
	int iNumCannons = Net_GetCannonsNum();
	int iNextType = iCurrentCannonType;
	for (int i=0; i<iNumCannons; i++)
	{
		iNextType = iNextType + iDirection;
		if (iNextType >= iNumCannons) { return; }//iNextType = 0;
		if (iNextType < 0) { return; }//iNextType = iNumCannons - 1;

		ref rCannon = Net_GetCannonByIndex(iNextType);
		if (sti(rCannon.TradeOff)) { continue; }

		int iNewPrice = iNumShipCannons * Net_GetCannonCost(iNextType);

		if (iCurrentCredit - iOldPrice + iNewPrice < iMaxCredit)
		{
			// sell old cannon upgrade
			if (iCurrentCannonUpgrade >= 0)
			{
				iCurrentCredit = iCurrentCredit - iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, iCurrentCannonUpgrade);
				iCurrentCannonUpgrade = -1;
			}

			iCurrentCannonType = iNextType;
			iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;
			Picture_SetGroupPicture("PICTURE_CANNON", "CANNONS_UPGRADE_1", rCannon.picture);
			
			// set price of ship
			StringCollection_SetTextValue("STRINGS_SHIPCANNONS_MONEY", 2, iNewPrice);

			string sCannonTypeText = XI_ConvertString(rCannon.Name);
			string sCannonCaliberText = XI_ConvertString("caliber" + rCannon.caliber);
			StringCollection_SetText("STRINGS_SHIPCANNONS_TYPE", 2, "#" + sCannonTypeText + " " + sCannonCaliberText);

			StringCollection_SetText("STRINGS_CANNON_UPGRADE_MONEY", 1, "#" + iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, 0));
			StringCollection_SetText("STRINGS_CANNON_UPGRADE_MONEY", 2, "#" + iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, 1));
			StringCollection_SetText("STRINGS_CANNON_UPGRADE_MONEY", 3, "#" + iNumShipCannons * Net_GetCannonUpgradeCost(iCurrentCannonType, 2));
			
			// set new cannon upgrade = Copper
			Buy_SetCannonUpgrade(0, true);
			Picture_SetGroupPicture("PICTURE_CANNON_UPGRADE_COPPER", "CANNONS_UPGRADE_1", rCannon.picture);
			Picture_SetGroupPicture("PICTURE_CANNON_UPGRADE_BRONZE", "CANNONS_UPGRADE_2", rCannon.picture);
			Picture_SetGroupPicture("PICTURE_CANNON_UPGRADE_IRON", "CANNONS_UPGRADE_3", rCannon.picture);

			Buy_UpdateCredit();
			break;
		}
	}
}
void Buy_CannonLeft()	{ Buy_SetCannon(-1); }
void Buy_CannonRight()	{ Buy_SetCannon(1); }

void Buy_SetPerk(int iPerkIndex, bool bState)
{
	if (iPerks[iPerkIndex] == bState) return;

	int iPerkCost = Net_GetPerkCost(iPerkIndex);

	if (iPerks[iPerkIndex])
	{
		// sell old perk
		iCurrentCredit = iCurrentCredit - iPerkCost;
	}

	if (bState)
	{
		iCurrentCredit = iCurrentCredit + iPerkCost;
	}

	iPerks[iPerkIndex] = bState;
	Buy_UpdateCredit();
}

void Buy_SortPlayers()
{
	int i, j;
	int iThisClientID = NetClient_GetThisClientID();
	int iMainTeam;

	iNumClients = 0;

	// create list
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{
		if (!sti(NCClients[i].Use)) { continue; }
		if (iThisClientID == sti(NCClients[i].ID))
		{
			iMainTeam = sti(NCClients[i].Team);
		}
		BuyClients[iNumClients] = sti(NCClients[i].ID);
		iNumClients++;
	}

	// sort BuyClients
	for (i=0; i<iNumClients; i++) 
	{
		for (j=i + 1; j<iNumClients; j++) 
		{
			bool bSwap = false;

			ref rClient1 = NetClient_GetClient(BuyClients[i]);
			ref rClient2 = NetClient_GetClient(BuyClients[j]);

			if (sti(rClient2.ID) == iThisClientID) { bSwap = true; }

			int iTeam1 = sti(rClient1.Team);
			int iTeam2 = sti(rClient2.Team);

			if (iTeam1 != iMainTeam)
			{
				if (iTeam2 == iMainTeam || iTeam2 > iTeam1) { bSwap = true; }
			}

			if (bSwap)
			{
				int iTmp = BuyClients[i];
				BuyClients[i] = BuyClients[j];
				BuyClients[j] = iTmp;
			}
		}
	}
}

void Buy_FillPlayersTable(string sTable)
{
	Table_Clear(sTable, false, true, false);

	if (!bServerInfoReady) { return; }

	// sort players: our player first and his team first too
	Buy_SortPlayers();

	for (int i=0; i<iNumClients; i++)
	{
		ref rClient = NetClient_GetClient(BuyClients[i]);

		string sRow = "tr" + (i + 1);

		bool bHimself = (rClient.ID == wClientID);

		string sShipGroup = "SHIPS_empty";

		int iShipType = sti(rClient.Ship.Type);
		if (iShipType >= 0)
		{
			ref rShip = Net_GetShipByIndex(iShipType);
			sShipGroup = "SHIPS_" + rShip.Name;
		}

		int dwColor = 0; if (bHimself) { dwColor = ARGB(120, 248, 255, 214); }
		GameInterface.(sTable).(sRow).speccolor = dwColor;

		GameInterface.(sTable).(sRow).ClientID = rClient.ID;

		// face
		GameInterface.(sTable).(sRow).td1.icon.texturepointer = sti(rClient.UserData.Face.Texture);
		GameInterface.(sTable).(sRow).td1.icon.cellsize = true;

		// ship
		GameInterface.(sTable).(sRow).td2.icon.group = sShipGroup;
		GameInterface.(sTable).(sRow).td2.icon.image = "ship";
		GameInterface.(sTable).(sRow).td2.icon.cellsize = true;

		GameInterface.(sTable).(sRow).td3.str = rClient.NickName + "\n" + rClient.Ship.Name;

		if (sti(NCServer.GameType) == NETGAME_DEATHMATCH)
		{
			GameInterface.(sTable).(sRow).td5.icon.group = "NET_TEAMICONS";
			GameInterface.(sTable).(sRow).td5.icon.image = "no_team";
		}
		else
		{
			GameInterface.(sTable).(sRow).td5.icon.group = "NET_TEAMICONS";
			GameInterface.(sTable).(sRow).td5.icon.image = "team" + rClient.Team;
		}

		GameInterface.(sTable).(sRow).td5.icon.cellsize = true;

		GameInterface.(sTable).(sRow).td6.icon.group = "NET_BUYCHECKICONS";
		string sReady = "check_empty";
		if (bHimself) 
		{ 
			sReady = "check_no"; 
			if (sti(rClient.BuyReady)) { sReady = "check_yes"; }
		}
		else 
		{ 
			if (sti(rClient.BuyReady)) { sReady = "check_yes_no_choice"; } 
		}
		GameInterface.(sTable).(sRow).td6.icon.image = sReady;
		GameInterface.(sTable).(sRow).td6.icon.cellsize = true;

		GameInterface.(sTable).(sRow).td7.icon.group = "NET_BUYCHECKICONS";
		string sKickIcon = "check_no";
		if (sti(rClient.Kick)) { sKickIcon = "check_yes"; }
		if (bHimself) { sKickIcon = "check_empty"; }
		GameInterface.(sTable).(sRow).td7.icon.image = sKickIcon;
		GameInterface.(sTable).(sRow).td7.icon.cellsize = true;
	}

	Table_UpdateWindow(sTable);
}

void Buy_TryChangeTeam()
{
	int iGameType = sti(NCServer.GameType);
	if (iGameType == NETGAME_DEATHMATCH) { return; }


}

void Buy_OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

	if (sControl != "TABLE_PLAYERS") { return; } 
	//if (iRow == 0) { return; } 

	string sRow = "tr" + (iRow + 1);
	int wNetClientID = sti(GameInterface.TABLE_PLAYERS.(sRow).ClientID);
	bool bClickHimself = (wNetClientID == wClientID);

	ref rClient = NetClient_GetClient(wNetClientID);

	int iSMsg;
	switch (iColumn)
	{
		case 5:		// team click
			if (!bClickHimself) { return; }
			// try to change team and resort table
			if (sti(NCServer.NumTeams) > 1) NetBuy_OpenSelectTeam();
		break;
		case 6:		// ready click
			if (!bClickHimself) { return; }
			Buy_Server_UpdateReady();
		break;
		case 7:		// kick click
			if (bClickHimself) { return; }
			rClient.Kick = !sti(rClient.Kick);

			iSMsg = NMCreate();
			NMAddClientID(iSMsg, wClientID);
			NMAddByte(iSMsg, NC_COMMAND);
			NMAddByte(iSMsg, NSC_COMMAND_VOTE_KICK);
			NMAddClientID(iSMsg, wNetClientID);
			NetClient_SendMessage(iSMsg, true);
			NMDelete(iSMsg);
		break;
	}

	Buy_FillPlayersTable("TABLE_PLAYERS");
}

void Buy_CheckButtonChange()
{
	string sControl = GetEventData();
	int iNumber = GetEventData();
	int iState = GetEventData();

	switch (sControl)
	{
		case "RADIO_CANNONS_UPGRADELIST":
			Buy_SetCannonUpgrade(iNumber - 1, false);
		break;
		case "CHECKBUTTONS_PERKS":
			Buy_SetPerk(iNumber - 1, iState);
		break;
	}
}

void IDoExit(int exitCode)
{
	DelEventHandler("CheckButtonChange", "Buy_CheckButtonChange");

	DelEventHandler("OnTmpStart", "OnTmpStart");
	DelEventHandler("OnReset", "OnReset");
	DelEventHandler("OnDisconnect", "OnDisconnect");

	DelEventHandler("ShipLeft", "Buy_ShipLeft");		DelEventHandler("ShipRight", "Buy_ShipRight");

	DelEventHandler("AddUpgradeHull", "Buy_AddUpgradeHull");		DelEventHandler("DecUpgradeHull", "Buy_DecUpgradeHull");
	DelEventHandler("AddUpgradeRigging", "Buy_AddUpgradeRigging");	DelEventHandler("DecUpgradeRigging", "Buy_DecUpgradeRigging");

	DelEventHandler("CannonLeft", "Buy_CannonLeft");	DelEventHandler("CannonRight", "Buy_CannonRight");

	DelEventHandler("AddAccuracy", "Buy_AddAccuracy");	DelEventHandler("DecAccuracy", "Buy_DecAccuracy");
	DelEventHandler("AddCannons", "Buy_AddCannons");	DelEventHandler("DecCannons", "Buy_DecCannons");
	DelEventHandler("AddSailing", "Buy_AddSailing");	DelEventHandler("DecSailing", "Buy_DecSailing");
	DelEventHandler("AddDefence", "Buy_AddDefence");	DelEventHandler("DecDefence", "Buy_DecDefence");
	DelEventHandler("AddRepair", "Buy_AddRepair");		DelEventHandler("DecRepair", "Buy_DecRepair");

	DelEventHandler("AddBalls", "Buy_AddBalls");			DelEventHandler("DecBalls", "Buy_DecBalls");
	DelEventHandler("AddGrapes", "Buy_AddGrapes");			DelEventHandler("DecGrapes", "Buy_DecGrapes");
	DelEventHandler("AddKnippels", "Buy_AddKnippels");		DelEventHandler("DecKnippels", "Buy_DecKnippels");
	DelEventHandler("AddBombs", "Buy_AddBombs");			DelEventHandler("DecBombs", "Buy_DecBombs");
	DelEventHandler("AddPlanks", "Buy_AddPlanks");			DelEventHandler("DecPlanks", "Buy_DecPlanks");
	DelEventHandler("AddSailcloth", "Buy_AddSailcloth");	DelEventHandler("DecSailcloth", "Buy_DecSailcloth");

	DelEventHandler("OnTableClick", "Buy_OnTableClick");
	DelEventHandler("NCEvent_PlayerInfo_ShipType", "Buy_PlayerInfo_ShipType");
	DelEventHandler("NCEvent_PlayerInfo_BuyReady", "Buy_PlayerInfo_BuyReady");
	DelEventHandler("NCEvent_PlayerInfo_Team", "Buy_PlayerInfo_Team");
	DelEventHandler("NCEvent_Client_Connect", "Buy_NewClientConnect");

	DelEventHandler("NCEvent_StartGame", "Buy_StartGame");
	DelEventHandler("NCEvent_StartGame_GetBuyInfo", "Buy_GetBuyInfo");
	DelEventHandler("NCEvent_PlayerInfo_StartCountdown", "Buy_StartCountdown");
	DelEventHandler("NCEvent_PlayerInfo_StopCountdown",  "Buy_StopCountdown");
	DelEventHandler("NCEvent_Disconnect", "Buy_Disconnect");

	DelEventHandler("NCEvent_ClientDrop", "Buy_ClientDrop");

	DelEventHandler("frame", "NetBuy_OnFrame");

	DelEventHandler("OnClickTeam", "NetBuy_OnClickTeam");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void Buy_Disconnect()
{
	//IDoExit(RC_INTERFACE_NET_FINDGAME);
}

void Buy_StopCountdown()
{
	iClientStartTimer = 10000;
	bStartCountDown = false;
}

void Buy_StartCountdown()
{
	bStartCountDown = true;
	iClientStartTimer = GetEventData() * 1000;

	// drop team select interface
	if (bSelectNewTeamWindow) { NetBuy_SelectTeamCancel(); }
}

void Buy_GetBuyInfo()
{
	int i;

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_BUY);
	NMAddByte(iSMsg, NSC_BUY_FULLINFO);
	NMAddWord(iSMsg, iCurrentShipType);
	NMAddByte(iSMsg, iCurrentShipUpgradeHull + 1);
	NMAddByte(iSMsg, iCurrentShipUpgradeRigging + 1);
	NMAddByte(iSMsg, iCurrentCannonType);
	NMAddByte(iSMsg, iCurrentCannonUpgrade + 1);
	for (i=0; i<5; i++) { NMAddByte(iSMsg, iSkillsQuantity[i]); }
	for (i=0; i<6; i++) { NMAddByte(iSMsg, iPerks[i]); }
	for (i=0; i<6; i++) { NMAddWord(iSMsg, iNetGoodsQuantity[i]); }
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);

	XI_WindowDisable("WND_NETBUY", true);

	XI_WindowShow("WND_STARTGAME_WAIT", true);
	XI_WindowDisable("WND_STARTGAME_WAIT", false);
}

void Buy_StartGame()
{
	IDoExit(RC_INTERFACE_DO_NOTHING);
}

void OnDisconnect()
{
	NetClient_ClearPacketsQueue();

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_DISCONNECT);
	NetClient_SendMessage(iSMsg, false);
	NMDelete(iSMsg);

	Net_DeleteClient();
	IDoExit(RC_INTERFACE_NET_FINDGAME);
}

void OnTmpStart()
{
	Net_StartServer();
}

void Buy_UpdatePlayersQuantity()
{
	//int iNumClients = 0;
	//for (int i=0; i<NET_MAXCLIENTS; i++) { if (sti(NCClients[i].Use)) { iNumClients++; } }

	string sValue = "#" + iNumClients + " / " + NCServer.MaxClients;
	StringCollection_SetText("STRINGS_MAP_INFO_VALUE", 3, sValue);
}

void Buy_UpdateCredit()
{
	string sValue = "#" + iCurrentCredit + " / " + iMaxCredit;
	StringCollection_SetText("STRINGS_MAP_INFO_VALUE", 2, sValue);

	int i, bState;
	bool bDisable;

	// enable/disable cannons upgrade
	for (i=0; i<3; i++)
	{
		bDisable = false;
		bState = CheckButton_GetState("RADIO_CANNONS_UPGRADELIST", i + 1);
		if (!bState && (iCurrentCredit + Net_GetCannonUpgradeCost(iCurrentCannonType, i) > iMaxCredit)) { bDisable = true; }
		CheckButton_SetDisable("RADIO_CANNONS_UPGRADELIST", i + 1, bDisable);
	}

	// enable/disable perks
	/*for (i=0; i<Net_GetPerksNum(); i++)
	{
		bDisable = false;
		bState = CheckButton_GetState("CHECKBUTTONS_PERKS", i + 1);
		if (!bState && (iCurrentCredit + Net_GetPerkCost(i) > iMaxCredit)) { bDisable = true; }
		CheckButton_SetDisable("CHECKBUTTONS_PERKS", i + 1, bDisable);
	}*/
}

// ========================================
//				SKILLS SECTION
// ========================================

int Clampi(int iValue, int iMin, int iMax)
{
	if (iValue < iMin) { iValue = iMin; }
	if (iValue > iMax) { iValue = iMax; }
	return iValue;
}

void Buy_AddSkills(int iSkillIndex, int iSkillAdd)
{
	if (XI_IsKeyPressed("shift")) { iSkillAdd = iSkillAdd * 10; }

	int i = iSkillIndex - 1;
	ref rSkill = Net_GetSkillByIndex(i);
	int iMoney = sti(rSkill.Cost);

	int iOldSkill = iSkillsQuantity[i];
	int iNewSkill = Clampi(iOldSkill + iSkillAdd, 1, 10);

	int iOldPrice = (iOldSkill - 1) * iMoney;
	int iNewPrice = (iNewSkill - 1) * iMoney;

	if (iCurrentCredit - iOldPrice + iNewPrice > iMaxCredit) { return; }

	iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;
	iSkillsQuantity[i] = iNewSkill;

	StringCollection_SetTextValue("STRINGS_SKILLS_QUANTITY", iSkillIndex, iSkillsQuantity[iSkillIndex - 1]);

	Buy_UpdateCredit();
}

void Buy_AddAccuracy() { Buy_AddSkills(1, 1); }
void Buy_DecAccuracy() { Buy_AddSkills(1, -1); }

void Buy_AddCannons() { Buy_AddSkills(2, 1); }
void Buy_DecCannons() { Buy_AddSkills(2, -1); }

void Buy_AddSailing() { Buy_AddSkills(3, 1); }
void Buy_DecSailing() { Buy_AddSkills(3, -1); }

void Buy_AddDefence() { Buy_AddSkills(4, 1); }
void Buy_DecDefence() { Buy_AddSkills(4, -1); }

void Buy_AddRepair() { Buy_AddSkills(5, 1); }
void Buy_DecRepair() { Buy_AddSkills(5, -1); }

// ========================================
//				GOODS SECTION
// ========================================

void Buy_AddGood(int iGoodIndex, int iGoodAddQuantity)
{
	if (XI_IsKeyPressed("shift")) { iGoodAddQuantity = iGoodAddQuantity * 5; }

	int i = iGoodIndex - 1;
	ref rGood = Net_GetGoodByIndex(i);
	int iMoney = sti(rGood.Cost);
	int iUnits = sti(rGood.Units);

	int iOldQuantity = iNetGoodsQuantity[i];
	int iNewQuantity = iOldQuantity + iGoodAddQuantity * iUnits;
	if (iNewQuantity < 0) { iNewQuantity = 0; }
	if (iNewQuantity > 9999) { iNewQuantity = 9999; }

	int iOldPrice = (iOldQuantity * iMoney) / iUnits;
	int iNewPrice = (iNewQuantity * iMoney) / iUnits;

	if (iCurrentCredit - iOldPrice + iNewPrice > iMaxCredit) { return; }

	iCurrentCredit = iCurrentCredit - iOldPrice + iNewPrice;
	iNetGoodsQuantity[i] = iNewQuantity;

	StringCollection_SetTextValue("STRINGS_AMMO_QUANTITY", iGoodIndex, iNewQuantity);

	Buy_UpdateCredit();
}

void Buy_AddBalls() { Buy_AddGood(1, 1); }
void Buy_DecBalls() { Buy_AddGood(1, -1); }

void Buy_AddGrapes() { Buy_AddGood(2, 1); }
void Buy_DecGrapes() { Buy_AddGood(2, -1); }

void Buy_AddKnippels() { Buy_AddGood(3, 1); }
void Buy_DecKnippels() { Buy_AddGood(3, -1); }

void Buy_AddBombs() { Buy_AddGood(4, 1); }
void Buy_DecBombs() { Buy_AddGood(4, -1); }

void Buy_AddPlanks() { Buy_AddGood(5, 1); }
void Buy_DecPlanks() { Buy_AddGood(5, -1); }

void Buy_AddSailcloth() { Buy_AddGood(6, 1); }
void Buy_DecSailcloth() { Buy_AddGood(6, -1); }

void Buy_Server_UpdateShipType()
{
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_BUY);
	NMAddByte(iSMsg, NSC_BUY_SHIPCHANGE);
	NMAddWord(iSMsg, iCurrentShipType);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}

void Buy_Server_UpdateReady()
{
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_BUY);
	NMAddByte(iSMsg, NSC_BUY_READY);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}