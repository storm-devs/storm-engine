// 
int iCurrentLanServer, iCurrentInternetServer, iCurrentFavoriteServer;
string sCurrentIP = "";
string sCurrentName = ""; 
string sCurrentPort = "";
bool bCurrentProtected = false;
int iMapListSelected = 0;
bool bFavWindowActive = false;
bool bNeedServerPassword = false;
int iTryConnectDeltaTime = 0;

int iGetInetServersFrameTime = 0;
int iNumInternetServerToLoad = 0;
int iCurrentInternetServersLoaded = 0;

void InitInterface(string iniName)
{
	// init some clients data, maybe need place it in another fuction
	NetClient.ClientLastPacketIndex = 0;
	// init some clients data, maybe need place it in another fuction

	DeleteAttribute(&NCFavorites, "");
	Net_LoadFile(false, &NCFavorites, "Favorites.nsv");

	GameInterface.title = "titleNetFindGame";

	SetEventHandler("NCEvent_Client_Accepted", "FG_ClientAccepted", 0);
	SetEventHandler("NCEvent_Client_Rejected", "FG_ClientRejected", 0);
	SetEventHandler("NCEvent_Client_RejectedClientPassword", "FG_Client_RejectedClientPassword", 0);
	SetEventHandler("NCEvent_Client_RejectedServerFull", "FG_Client_RejectedServerFull", 0);
	SetEventHandler("NCEvent_Client_Banned", "FG_ClientBanned", 0);
	SetEventHandler("NCEvent_Client_RejectedServerPassword", "FG_ClientNeedServerPassword", 0);

	SetEventHandler("NCEvent_MasterServer_Connect", "FG_MasterServerOnConnect", 0);
	SetEventHandler("NCEvent_MasterServer_GetServers", "FG_MasterServer_GetServers", 0);

	SetEventHandler("OnRejectedOk", "FG_ClientRejectedOk", 0);
	SetEventHandler("OnNeedPasswordOk", "FG_ClientNeedPasswordOk", 0);
	SetEventHandler("OnNeedPasswordCancel", "FG_ClientNeedPasswordCancel", 0);

	SetEventHandler("frame", "FG_OnFrame", 0);

	SetEventHandler("OnCreateGame", "FG_OnCreateGame", 0);
	SetEventHandler("OnJoinGame", "FG_OnJoinGame", 0);
	SetEventHandler("OnCancel", "FG_OnCancel", 0);

	SetEventHandler("OnConnectOk", "FG_OnConnectOk", 0);
	SetEventHandler("OnConnectCancel", "FG_OnConnectCancel", 0);

	SetEventHandler("OnAddFavOk", "FG_AddFavOnOk", 0);
	SetEventHandler("OnAddFavDelete", "FG_AddFavOnDelete", 0);
	SetEventHandler("OnAddFavCancel", "FG_AddFavOnCancel", 0);

	SetEventHandler("OnRefresh", "FG_OnRefresh", 0);
	SetEventHandler("OnPlayerSettings", "FG_OnPlayerSettings", 0);
	SetEventHandler("OnAddFavorites", "FG_OnAddFavorites", 0);

	SetEventHandler("TableSelectChange", "FG_TableSelectChangeEvent", 0);	
	SetEventHandler("TableActivate", "FG_OnTableDoubleClick", 0);

	SetEventHandler("TabClick_Local", "FG_TabLocal", 0);
	SetEventHandler("TabClick_Inet", "FG_TabInet", 0);
	SetEventHandler("TabClick_Favorites", "FG_TabFavorites", 0);

	SetEventHandler("DecCredit", "FG_CreditLeft", 0);
	SetEventHandler("AddCredit", "FG_CreditRight", 0);

	SetEventHandler("DecMaxShipClass", "FG_ShipClassLeft", 0);
	SetEventHandler("AddMaxShipClass", "FG_ShipClassRight", 0);

	SetEventHandler("DecPing", "FG_PingLeft", 0);
	SetEventHandler("AddPing", "FG_PingRight", 0);

	SetEventHandler("CheckButtonChange", "FG_CheckButtonChange", 0);

	FG_SetTableHeaders("TABLE_LANSERVER");
	FG_SetTableHeaders("TABLE_INETSERVER");
	FG_SetTableHeaders("TABLE_FAVSERVER");

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	Table_Clear("TABLE_LANSERVER", false, true, true);
	iNumLanServers = 0; 
	iNumLanServersList = 0;

	iCurrentLanServer = -1;
	iCurrentInternetServer = -1;
	iCurrentFavoriteServer = -1;

	FindCurrentServers();

	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		FG_TabLocal();		break;
		case NET_FG_INTERNET:	FG_TabInet();		break;
		case NET_FG_FAVORITE:	FG_TabFavorites();	break;
	}

	GameInterface.TABLE_MAPLIST.tr1.td1.str = XI_ConvertString("AllIslands");
	GameInterface.TABLE_MAPLIST.tr1.td1.IslandID = "";

	GameInterface.TABLE_MAPLIST.tr2.td1.str = XI_ConvertString("OpenSea");
	GameInterface.TABLE_MAPLIST.tr2.td1.IslandID = "";
	GameInterface.TABLE_MAPLIST.select = 1;

	for (int i=0; i<Net_GetIslandsNum(); i++)
	{
		ref rIsland = Net_GetIslandByIndex(i);

		string sTest = "tr" + (i + 3);
		GameInterface.TABLE_MAPLIST.(sTest).td1.str = XI_ConvertString(rIsland.ID);
		GameInterface.TABLE_MAPLIST.(sTest).td1.IslandID = rIsland.ID;
	}

	Table_UpdateWindow("TABLE_MAPLIST");

	StringCollection_SetText("STRINGS_PROFILE", 2, "#" + NetClient.NickName);
	
	// 
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 1, iFG_MaxShipClass);
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 2, iFG_Credit);
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 3, iFG_Ping);

	// set state for check buttons
	CheckButton_SetState("CHECKBUTTONS_WO_PASSWORD", 1, bFG_WOPassword);
	CheckButton_SetState("CHECKBUTTONS_FREE_SERVERS", 1, bFG_FreeServers);

	CheckButton_SetState("CHECKBUTTONS_FAV_ACTIVE", 1, bFG_ActiveServers);

	CheckButton_SetState("CHECKBUTTONS_GAMETYPES", 1, bFG_Deathmatch);
	CheckButton_SetState("CHECKBUTTONS_GAMETYPES", 2, bFG_TeamDeathmatch);
	CheckButton_SetState("CHECKBUTTONS_GAMETYPES", 3, bFG_DefendConvoy);
	CheckButton_SetState("CHECKBUTTONS_GAMETYPES", 4, bFG_CaptureTheFort);

	// disable addfav window
	XI_WindowShow("WND_ADDFAVORITES", false);
	XI_WindowDisable("WND_ADDFAVORITES", true);

	// need password window
	XI_WindowShow("WND_NEEDPASSWORD", false);
	XI_WindowDisable("WND_NEEDPASSWORD", true);

	// rejected window
	XI_WindowShow("WND_REJECTED", false);
	XI_WindowDisable("WND_REJECTED", true);

	// connect window
	XI_WindowShow("WND_CONNECT", false);
	XI_WindowDisable("WND_CONNECT", true);

	// tryconnect window
	XI_WindowShow("WND_TRYCONNECT", false);
	XI_WindowDisable("WND_TRYCONNECT", true);

	// account window
	XI_WindowShow("WND_ACCOUNTS", false);
	XI_WindowDisable("WND_ACCOUNTS", true);

	// inet servers loading window
	XI_WindowShow("WND_INETSERVERSLOADING", false);
	XI_WindowDisable("WND_INETSERVERSLOADING", true);

	// inet servers loading window
	XI_WindowShow("WND_YESNO", false);
	XI_WindowDisable("WND_YESNO", true);

	// MAGIC FIX
	DeleteParticles();
}

void FG_SetTableHeaders(string sTable)
{
	GameInterface.(sTable).hr.td1.str = "";
	GameInterface.(sTable).hr.td2.str = XI_ConvertString("capFG_TableServerName");
	GameInterface.(sTable).hr.td3.str = "";
	GameInterface.(sTable).hr.td4.str = XI_ConvertString("capFG_TableServerIP");
	GameInterface.(sTable).hr.td5.str = XI_ConvertString("capFG_TableServerType");
	GameInterface.(sTable).hr.td6.str = XI_ConvertString("capFG_TableServerPlayers");
	GameInterface.(sTable).hr.td7.str = XI_ConvertString("capFG_TableServerPing");
}

void FindCurrentServers()
{
	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		
			if (iNumLanServers == 0) { NetClient_FindLocalServers(); }
		break;
		case NET_FG_INTERNET:	
			if (iNumInternetServers == 0) { FG_FindInternetServers(); }
		break;
		case NET_FG_FAVORITE:	
		break;
	}
}

void FG_ConnectToMasterServer()
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_MASTERSERVER);
	NMAddByte(iSMsg, NSC_MASTERSERVER_CONNECT);
	NMAddByte(iSMsg, NET_SCRIPT_VERSION);
	NetClient_SendMasterServerMessage(sMasterServerAddress, iMasterServerPort, iSMsg);
	NMDelete(iSMsg);
}

void FG_MasterServerTryConnectTimer()
{
	// try connect again if 5 seconds no response
	FG_ConnectToMasterServer();

	PostEvent("FG_MasterServerTryConnectTimer", 1000);
}

void FG_MasterServerOnConnect()
{
	DelEventHandler("FG_MasterServerTryConnectTimer", "FG_MasterServerTryConnectTimer");
	SetEventHandler("frame", "FG_MasterServer_GetServersFrameTimer", 0);

	iGetInetServersFrameTime = 1000;

	int iMsg = GetEventData();
	iNumInternetServerToLoad = NMGet24(iMsg);
	iCurrentInternetServersLoaded = 0;

	StringCollection_SetText("STRINGS_INETSERVERLOADING_STATUS", 3, "capNetFG_MS_LoadingServersList");
	StringCollection_SetText("STRINGS_INETSERVERLOADING_QUANTITY", 2, "#" + "0 / " + iNumInternetServerToLoad);

	// delete current servers list
	DeleteAttribute(&NCInetServers, "");
}

void FG_MasterServer_GetServersFrameTimer()
{
	iGetInetServersFrameTime = iGetInetServersFrameTime + iRealDeltaTime;
	
	if (iGetInetServersFrameTime >= 1000)
	{
		iGetInetServersFrameTime = 0;

		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_MASTERSERVER);
		NMAddByte(iSMsg, NSC_MASTERSERVER_GETSERVERS);
		NMAdd24(iSMsg, iCurrentInternetServersLoaded);
		NetClient_SendMasterServerMessage(sMasterServerAddress, iMasterServerPort, iSMsg);
		NMDelete(iSMsg);
	}
}

void FG_MasterServer_GetServers()
{
	int iMsg = GetEventData();

	int iLastSendedServerNum = NMGet24(iMsg);
	int iNumServersSended = NMGetByte(iMsg);

	if (iNumServersSended && iLastSendedServerNum == iCurrentInternetServersLoaded)
	{
		for (int i=0; i<iNumServersSended; i++)
		{
			int dwIP = NMGetDword(iMsg);
			int wPort = ntohs(NMGetWord(iMsg));

			string sSrvIndex = "l" + iCurrentInternetServersLoaded;
			NCInetServers.list.(sSrvIndex).IP = dwIP;
			NCInetServers.list.(sSrvIndex).Port = wPort;

			iCurrentInternetServersLoaded++;

			int iSMsg = NMCreate();
			NMAddClientID(iSMsg, DST_INVALID);
			NMAddByte(iSMsg, NC_BROADCAST);
			NMAddByte(iSMsg, NSC_BROADCAST_INET);
			NMAddDword(iSMsg, Net_GetTickCount());
			NetClient_SendMessageDirectToIP(dwIP, wPort, iSMsg);
			NMDelete(iSMsg);
		}
	}

	StringCollection_SetText("STRINGS_INETSERVERLOADING_QUANTITY", 2, "#" + iCurrentInternetServersLoaded + " / " + iNumInternetServerToLoad);
	iGetInetServersFrameTime = 1000;

	if (iNumServersSended == 0)
	{
		FG_FindInternetServersClose();
	}
}

void FG_FindInternetServersClose()
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_MASTERSERVER);
	NMAddByte(iSMsg, NSC_MASTERSERVER_DISCONNECT);
	NetClient_SendMasterServerMessage(sMasterServerAddress, iMasterServerPort, iSMsg);
	NMDelete(iSMsg);

	DelEventHandler("frame", "FG_MasterServer_GetServersFrameTimer");

	XI_WindowShow("WND_INETSERVERSLOADING", false);
	XI_WindowDisable("WND_INETSERVERSLOADING", true);

	FG_ShowMainWindow();
}

void FG_MasterServerCancel()
{
	FG_FindInternetServersClose();
}

void FG_FindInternetServers()
{
	XI_WindowDisable("WND_LANSERVER", true);
	XI_WindowDisable("WND_INETSERVER", true);
	XI_WindowDisable("WND_FAVSERVER", true);
	XI_WindowDisable("WND_NETFINDGAME", true);

	XI_WindowShow("WND_INETSERVERSLOADING", true);
	XI_WindowDisable("WND_INETSERVERSLOADING", false);

	SetEventHandler("FG_MasterServerTryConnectTimer", "FG_MasterServerTryConnectTimer", 0);
	SetEventHandler("OnInetServerLoadingCancel", "FG_MasterServerCancel", 0);
	PostEvent("FG_MasterServerTryConnectTimer", 1000);

	FG_ConnectToMasterServer();
}

void SelectServer(ref rServer, int iSelected)
{
	string sValue = rServer.Name;

	sCurrentIP = rServer.IP;
	sCurrentPort = rServer.Port;
	sCurrentName = rServer.Name;

	bCurrentProtected = sti(rServer.Secure);

	string sIslandMap = "OpenSea";
	if (rServer.IslandID != "") { sIslandMap = rServer.IslandID; }

	Picture_SetPicture("SERVER_MAP_PICTURE", "Interfaces\Net\Islands\" + sIslandMap + ".tga");

	SetFormatedText("SERVERINFO_NAME", rServer.Name);

	StringCollection_SetText("STRINGS_SERVERINFO", 1, "#" + XI_ConvertString(sIslandMap));
	StringCollection_SetText("STRINGS_SERVERINFO", 2, "txtCS_" + NetModes[sti(rServer.GameType)].Name);
	StringCollection_SetText("STRINGS_SERVERINFO", 3, "#" + rServer.Credit);
	StringCollection_SetText("STRINGS_SERVERINFO", 4, "#" + rServer.NumClients + " / " + rServer.MaxClients);
	StringCollection_SetText("STRINGS_SERVERINFO", 5, "#" + rServer.MaxClass);

	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		
			GameInterface.TABLE_LANSERVER.select = iSelected + 1;
			Table_UpdateWindow("TABLE_LANSERVER");
		break;
		case NET_FG_INTERNET:	
			GameInterface.TABLE_INETSERVER.select = iSelected + 1;
			Table_UpdateWindow("TABLE_INETSERVER");
		break;
		case NET_FG_FAVORITE:	
			GameInterface.TABLE_FAVSERVER.select = iSelected + 1;
			Table_UpdateWindow("TABLE_FAVSERVER");
		break;
	}
}

void FG_OnTableDoubleClick()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();

	FG_TableSelectChange(sControl, iSelected);

	if (sControl != "TABLE_MAPLIST")
	{
		FG_OnJoinGame();
		if (!bCurrentProtected) { FG_OnConnectOk(); }
	}
}

void FG_TableSelectChange(string sControl, int iSelected)
{
	switch (sControl)
	{
		case "TABLE_LANSERVER":
			if (iCurrentLanServer != iSelected)
			{
				iCurrentLanServer = iSelected;
				SelectServer(&LanServers[ LanServersList[iSelected] ], iSelected);
			}
		break;
		case "TABLE_INETSERVER":
			if (iCurrentInternetServer != iSelected)
			{
				iCurrentInternetServer = iSelected;
				SelectServer(&InternetServers[ InternetServersList[iSelected] ], iSelected);
			}
		break;
		case "TABLE_FAVSERVER":
		break;
		case "TABLE_MAPLIST":
			iMapListSelected = iSelected;
			FG_FilterChange();
		break;
	}
}

void FG_TableSelectChangeEvent()
{
	string sControl = GetEventData();
	int iSelected = GetEventData() - 1;

	FG_TableSelectChange(sControl, iSelected);
}

void FG_SelectEmptyServer()
{
	//sCurrentIP = "";
	//sCurrentPort = "";
	//sCurrentName = "";
	//bCurrentProtected = false;

	Picture_SetPicture("SERVER_MAP_PICTURE", "");

	SetFormatedText("SERVERINFO_NAME", "");

	StringCollection_SetText("STRINGS_SERVERINFO", 1, "#");
	StringCollection_SetText("STRINGS_SERVERINFO", 2, "#");
	StringCollection_SetText("STRINGS_SERVERINFO", 3, "#");
	StringCollection_SetText("STRINGS_SERVERINFO", 4, "#");
	StringCollection_SetText("STRINGS_SERVERINFO", 5, "#");
}

void FG_ClientAccepted()
{
	IDoExit(RC_INTERFACE_NET_BUY);
}

void FG_ClientRejectedWindow(string sReason)
{
	XI_WindowDisable("WND_LANSERVER", true);
	XI_WindowDisable("WND_INETSERVER", true);
	XI_WindowDisable("WND_FAVSERVER", true);
	XI_WindowDisable("WND_NETFINDGAME", true);

	XI_WindowShow("WND_REJECTED", true);
	XI_WindowDisable("WND_REJECTED", false);

	SetCurrentNode("BTN_REJECTED_OK");

	XI_WindowShow("WND_TRYCONNECT", false);
	XI_WindowDisable("WND_TRYCONNECT", true);

	SetFormatedText("TEXT_REJECTED", sReason );
}

void FG_ClientRejectedOk()
{
	XI_WindowShow("WND_REJECTED", false);
	XI_WindowDisable("WND_REJECTED", true);

	FG_ShowMainWindow();
}

void FG_Client_RejectedServerFull() { FG_ClientRejectedWindow(LanguageConvertString(iLangNetClient, "NetFG_ServerIsFull")); }
void FG_ClientRejected() { FG_ClientRejectedWindow(LanguageConvertString(iLangNetClient, "NetFG_RejectedUnknown")); }
void FG_ClientBanned() { FG_ClientRejectedWindow(LanguageConvertString(iLangNetClient, "NetFG_Banned")); }

void FG_Client_RejectedClientPassword()
{
	bNeedServerPassword = false;
	FG_ClientNeedPasswordWindow("NetFG_NeedUserPassword");
}

void FG_ClientNeedServerPassword()
{
	bNeedServerPassword = true;
	FG_ClientNeedPasswordWindow("NetFG_NeedServerPassword");
}

void FG_ClientNeedPasswordWindow(string sNeedPassword)
{
	XI_WindowDisable("WND_LANSERVER", true);
	XI_WindowDisable("WND_INETSERVER", true);
	XI_WindowDisable("WND_FAVSERVER", true);
	XI_WindowDisable("WND_NETFINDGAME", true);

	XI_WindowShow("WND_TRYCONNECT", false);
	XI_WindowDisable("WND_TRYCONNECT", true);

	XI_WindowShow("WND_NEEDPASSWORD", true);
	XI_WindowDisable("WND_NEEDPASSWORD", false);

	SetCurrentNode("BTN_NEEDPASSWORD_CANCEL");

	SetFormatedText("TEXT_NEEDPASSWORD", LanguageConvertString(iLangNetClient, sNeedPassword) );
}

void FG_ClientNeedPasswordOk()
{
	FG_ClientNeedPasswordCancel();
	
	if (bNeedServerPassword)
		{ GameInterface.EDITBOX_CONNECT_SERVER_PASSWORD.str = GameInterface.EDITBOX_NEEDPASSWORD.str; }
	else 
		{ GameInterface.EDITBOX_CONNECT_PASSWORD.str = GameInterface.EDITBOX_NEEDPASSWORD.str; }

	FG_OnConnectOk();
}

void FG_ClientNeedPasswordCancel()
{
	XI_WindowShow("WND_NEEDPASSWORD", false);
	XI_WindowDisable("WND_NEEDPASSWORD", true);

	FG_ShowMainWindow();
}

void FG_WindowShow(int iNewMode, bool bFirstSet)
{
	//if (iCurrentServersList == iNewMode) { return; }

	iCurrentServersList = iNewMode;

	XI_WindowShow("WND_LANSERVER", iCurrentServersList == NET_FG_LAN);
	XI_WindowDisable("WND_LANSERVER", iCurrentServersList != NET_FG_LAN);
	Table_UpdateWindow("TABLE_LANSERVER");

	XI_WindowShow("WND_INETSERVER", iCurrentServersList == NET_FG_INTERNET);
	XI_WindowDisable("WND_INETSERVER", iCurrentServersList != NET_FG_INTERNET);
	Table_UpdateWindow("TABLE_INETSERVER");

	XI_WindowShow("WND_FAVSERVER", iCurrentServersList == NET_FG_FAVORITE);
	XI_WindowDisable("WND_FAVSERVER", iCurrentServersList != NET_FG_FAVORITE);
	Table_UpdateWindow("TABLE_FAVSERVER");
}

void FG_TabLocal()
{
	SetNewGroupPicture("TAB_LOCAL", "TABS", "TabDeSelected");
	SetNewGroupPicture("TAB_INET", "TABS", "TabSelected");
	SetNewGroupPicture("TAB_FAVORITES", "TABS", "TabSelected");

	FG_WindowShow(NET_FG_LAN, false);
	FG_FilterChange();
}

void FG_TabInet()
{
	SetNewGroupPicture("TAB_LOCAL", "TABS", "TabSelected");
	SetNewGroupPicture("TAB_INET", "TABS", "TabDeSelected");
	SetNewGroupPicture("TAB_FAVORITES", "TABS", "TabSelected");

	FG_WindowShow(NET_FG_INTERNET, false);
	FG_FilterChange();

	// check if first time
	if (iNumInternetServers == 0)
	{
		FG_FindInternetServers();
	}
	else
	{
		// check if list not updated - getting info from servers
		if (iNumInternetServersList == 0)
		{
			//iCurrentInternetServer2GetInfo = 0;
			Event("FG_GetInfoFromInternetServer");
		}
	}
}

void FG_GetInfoFromInternetServer()
{
	// send packet to next 5 servers
	//iCurrentInternetServer2GetInfo

	PostEvent("FG_GetInfoFromInternetServer", 500);
}

void FG_TabFavorites()
{
	SetNewGroupPicture("TAB_LOCAL", "TABS", "TabSelected");
	SetNewGroupPicture("TAB_INET", "TABS", "TabSelected");
	SetNewGroupPicture("TAB_FAVORITES", "TABS", "TabDeSelected");

	FG_WindowShow(NET_FG_FAVORITE, false);
	FG_FilterChange();
}

void IDoExit(int exitCode)
{
	DelEventHandler("NCEvent_Client_Accepted", "FG_ClientAccepted");
	DelEventHandler("NCEvent_Client_Rejected", "FG_ClientRejected");
	DelEventHandler("NCEvent_Client_RejectedClientPassword", "FG_Client_RejectedClientPassword");
	DelEventHandler("NCEvent_Client_RejectedServerFull", "FG_Client_RejectedServerFull");
	DelEventHandler("NCEvent_Client_Banned", "FG_ClientBanned");
	DelEventHandler("NCEvent_Client_RejectedServerPassword", "FG_ClientNeedServerPassword");

	DelEventHandler("NCEvent_MasterServer_Connect", "FG_MasterServerOnConnect");
	DelEventHandler("NCEvent_MasterServer_GetServers", "FG_MasterServer_GetServers");

	DelEventHandler("OnRejectedOk", "FG_ClientRejectedOk");
	DelEventHandler("OnNeedPasswordOk", "FG_ClientNeedPasswordOk");
	DelEventHandler("OnNeedPasswordCancel", "FG_ClientNeedPasswordCancel");

	DelEventHandler("OnCreateGame", "FG_OnCreateGame");
	DelEventHandler("OnJoinGame", "FG_OnJoinGame");
	DelEventHandler("OnCancel", "FG_OnCancel");

	DelEventHandler("TableSelectChange", "FG_TableSelectChangeEvent");	
	DelEventHandler("TableActivate", "FG_OnTableDoubleClick");

	DelEventHandler("OnConnectOk", "FG_OnConnectOk");
	DelEventHandler("OnConnectCancel", "FG_OnConnectCancel");

	DelEventHandler("OnAddFavOk", "FG_AddFavOnOk");
	DelEventHandler("OnAddFavDelete", "FG_AddFavOnDelete");
	DelEventHandler("OnAddFavCancel", "FG_AddFavOnCancel");

	DelEventHandler("frame", "FG_OnFrame");

	DelEventHandler("OnRefresh", "FG_OnRefresh");
	DelEventHandler("OnFilter", "FG_OnPlayerSettings");
	DelEventHandler("OnAddFavorites", "FG_OnAddFavorites");

	DelEventHandler("TabClick_Local", "FG_TabLocal");
	DelEventHandler("TabClick_Inet", "FG_TabInet");
	DelEventHandler("TabClick_Favorites", "FG_TabFavorites");

	DelEventHandler("DecCredit", "FG_CreditLeft");
	DelEventHandler("AddCredit", "FG_CreditRight");

	DelEventHandler("DecMaxShipClass", "FG_ShipClassLeft");
	DelEventHandler("AddMaxShipClass", "FG_ShipClassRight");

	DelEventHandler("DecPing", "FG_PingLeft");
	DelEventHandler("AddPing", "FG_PingRight");

	DelEventHandler("CheckButtonChange", "FG_CheckButtonChange");

	interfaceResultCommand = exitCode;

	EndCancelInterface(true);

	Net_SaveFile(false, &NCFavorites, "Favorites.nsv");
	DeleteAttribute(&NCFavorites, "");
}

void FG_CheckButtonChange()
{
	string sNodeName = GetEventData();
	int iNumber = GetEventData();
	int iState = GetEventData();

	switch (sNodeName)
	{
		case "CHECKBUTTONS_WO_PASSWORD":
			bFG_WOPassword = iState;
		break;
		case "CHECKBUTTONS_FREE_SERVERS":
			bFG_FreeServers = iState;
		break;
		case "CHECKBUTTONS_GAMETYPES":
			switch (iNumber)
			{
				case 1: 	bFG_Deathmatch = iState;		break;
				case 2: 	bFG_TeamDeathmatch = iState;	break;
				case 3: 	bFG_DefendConvoy = iState;		break;
				case 4: 	bFG_CaptureTheFort = iState;	break;
			}
		break;
	}

	FG_FilterChange();
}

bool IsFilterOk(ref rServer)
{
	bool fFavActive = iCurrentServersList == NET_FG_FAVORITE;
	if (fFavActive && sti(rServer.Ping) == 9999 && !bFG_ActiveServers) { return true; }

	if (bFG_WOPassword && sti(rServer.Secure)) { return false; }
	if (bFG_FreeServers && (sti(rServer.NumClients) >= sti(rServer.MaxClients))) { return false; }

	// check gametypes
	if (sti(rServer.GameType) == NETGAME_DEATHMATCH && !bFG_Deathmatch) { return false; }
	if (sti(rServer.GameType) == NETGAME_NAVAL && !bFG_TeamDeathmatch) { return false; }
	if (sti(rServer.GameType) == NETGAME_CONVOY && !bFG_DefendConvoy) { return false; }
	if (sti(rServer.GameType) == NETGAME_FORT && !bFG_CaptureTheFort) { return false; }

	// check credit
	if (sti(rServer.Credit) < iFG_Credit) { return false; }
	// check max ship class
	if (sti(rServer.MaxClass) < iFG_MaxShipClass) { return false; }
	// check ping
	if (sti(rServer.Ping) > iFG_Ping) { return false; }

	// check map/island
	if (iMapListSelected != 0) 
	{
		if (rServer.IslandID == "" && iMapListSelected != 1) { return false; }	// check for open sea
		// check for island
		string sRow = "tr" + (iMapListSelected + 1);
		
		if (rServer.IslandID != GameInterface.TABLE_MAPLIST.(sRow).td1.IslandID) { return false; }
	}

	return true;
}

void FG_FilterChange()
{
	int i;

	Table_Clear("TABLE_LANSERVER", false, true, false);
	Table_Clear("TABLE_INETSERVER", false, true, false);
	Table_Clear("TABLE_FAVSERVER", false, true, false);

	FG_SelectEmptyServer();

	iCurrentLanServer = -1;
	iCurrentInternetServer = -1;
	iCurrentFavoriteServer = -1;

	iNumLanServersList = 0;
	iNumInternetServersList = 0;
	iNumFavServersList = 0;

	GameInterface.TABLE_LANSERVER.select = -1;
	GameInterface.TABLE_INETSERVER.select = -1;
	GameInterface.TABLE_FAVSERVER.select = -1;

	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		
			for (i=0; i<iNumLanServers; i++)
			{
				if (AddServerToList("TABLE_LANSERVER", &LanServers[i], iNumLanServersList))
				{
					LanServersList[iNumLanServersList] = i;
					iNumLanServersList++;
				}
			}
			for (i=0; i<iNumLanServersList; i++)
			{
				if (LanServers[ LanServersList[i] ].IP == sCurrentIP && LanServers[ LanServersList[i] ].Port == sCurrentPort)
				{
					SelectServer(&LanServers[ LanServersList[i] ], i);
					break;
				}
			}
			Table_UpdateWindow("TABLE_LANSERVER");
		break;
		case NET_FG_INTERNET:	
			for (i=0; i<iNumInternetServers; i++)
			{
				if (AddServerToList("TABLE_INETSERVER", &InternetServers[i], iNumInternetServersList))
				{
					InternetServersList[iNumInternetServersList] = i;
					iNumInternetServersList++;
				}
			}
			Table_UpdateWindow("TABLE_INETSERVER");
		break;
		case NET_FG_FAVORITE:
			int iNumFavorites = GetAttributesNum(&NCFavorites);
			for (i=0; i<iNumFavorites; i++)
			{
				aref arServer = GetAttributeN(&NCFavorites, i);
				if (AddServerToList("TABLE_FAVSERVER", &arServer, iNumFavServersList))
				{
					FavServersList[iNumFavServersList] = i;
					iNumFavServersList++;
				}
			}
			Table_UpdateWindow("TABLE_FAVSERVER");
		break;
	}
}

int FG_IsShift(int iShift, int iNoShift, int iAlt)
{
	if (XI_IsKeyPressed("shift")) { return iShift; }
	if (XI_IsKeyPressed("alt")) { return iAlt; }
	return iNoShift;
}

void FG_CreditLeft()
{
	iFG_Credit = iFG_Credit - 1000 * FG_IsShift(10, 1, 100);
	if (iFG_Credit < 5000) { iFG_Credit = 5000; }
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 2, iFG_Credit);
	FG_FilterChange();
}

void FG_CreditRight()
{
	iFG_Credit = iFG_Credit + 1000 * FG_IsShift(10, 1, 100);
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 2, iFG_Credit);
	FG_FilterChange();
}

void FG_ShipClassLeft()
{
	iFG_MaxShipClass = iFG_MaxShipClass - 1;
	if (iFG_MaxShipClass < 1) { iFG_MaxShipClass = 1; }
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 1, iFG_MaxShipClass);
	FG_FilterChange();
}

void FG_ShipClassRight()
{
	iFG_MaxShipClass = iFG_MaxShipClass + 1;
	if (iFG_MaxShipClass > 6) { iFG_MaxShipClass = 6; }
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 1, iFG_MaxShipClass);
	FG_FilterChange();
}

void FG_PingLeft()
{
	iFG_Ping = iFG_Ping - 50;
	if (iFG_Ping < 50) { iFG_Ping = 50; }
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 3, iFG_Ping);
	FG_FilterChange();
}

void FG_PingRight()
{
	iFG_Ping = iFG_Ping + 50;
	StringCollection_SetTextValue("STRINGS_FILTERS_VALUE", 3, iFG_Ping);
	FG_FilterChange();
}

void FG_OnRefresh()
{
	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		
			Table_Clear("TABLE_LANSERVER", false, true, true);
			iNumLanServers = 0; 
			iNumLanServersList = 0;
		break;
		case NET_FG_INTERNET:	
			Table_Clear("TABLE_INETSERVER", false, true, true);
			iNumInternetServers = 0; 
			iNumInternetServersList = 0;

			// if we have first ask
		break;
		case NET_FG_FAVORITE:
		break;
	}

	if (iCurrentServersList != NET_FG_FAVORITE)
	{
		iCurrentLanServer = -1;
		iCurrentInternetServer = -1;
		iCurrentFavoriteServer = -1;
	}

	FG_SelectEmptyServer();
	FindCurrentServers();
}

void FG_OnPlayerSettings()
{
	IDoExit(RC_INTERFACE_NET_PLAYERSETTINGS);
}

void FG_OnFrame()
{
	//Button_SetEnable("BTN_JOINGAME", iCurrentLanServer != -1);

	string sNumServers = "#0 / 0";
	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		sNumServers = "#" + iNumLanServersList + " / " + iNumLanServers;			break;
		case NET_FG_INTERNET:	sNumServers = "#" + iNumInternetServersList + " / " + iNumInternetServers;	break;	
		case NET_FG_FAVORITE:	
			int iNumFavorites = GetAttributesNum(&NCFavorites);
			sNumServers = "#" + iNumFavServersList + " / " + iNumFavorites;	break;
		break;
	}

	StringCollection_SetText("STRINGS_INFO", 6, sNumServers);

	if (bFavWindowActive)
	{
		Button_SetEnable("BTN_ADDFAV_OK", GameInterface.EDITBOX_SERVERIP.str != "");
		Button_SetEnable("BTN_ADDFAV_DELETE", CheckAttribute(&NCFavorites, GameInterface.EDITBOX_SERVERIP.str));
	}

	Button_SetEnable("BTN_CONNECT_OK", GameInterface.EDITBOX_CONNECT_IP.str != "");
}

void FG_AddFavOnOk()
{
	string sLink;
	string sAddress = GameInterface.EDITBOX_SERVERIP.str;

	// add record to favorites
	string sIP, sPort;
	FG_ParseIPPort(sAddress, &sIP, &sPort);

	int iLen = strlen(sAddress);
	for (int i=0; i<iLen; i++)
	{
		string sChar = GetSymbol(&sAddress, i);
		if (sChar == ".") { sChar = "_"; }
		sLink = sLink + sChar;
	}

	NCFavorites.(sLink) = "";
	NCFavorites.(sLink).IP = sIP;
	NCFavorites.(sLink).Port = sPort;
	NCFavorites.(sLink).Name = "";
	NCFavorites.(sLink).Secure = false;
	NCFavorites.(sLink).Credit = 0;
	NCFavorites.(sLink).MaxClass = 1;
	NCFavorites.(sLink).GameType = NETGAME_DEATHMATCH;
	NCFavorites.(sLink).WeatherID = "";
	NCFavorites.(sLink).IslandID = "";
	NCFavorites.(sLink).NumClients = 0;
	NCFavorites.(sLink).MaxClients = 0;
	NCFavorites.(sLink).Ping = 9999;

	if (iCurrentServersList == NET_FG_FAVORITE) { FG_FilterChange(); }

	FG_AddFavOnCancel();
}

void FG_AddFavOnDelete()
{
	// delete record from favorites
	string sAddress = GameInterface.EDITBOX_SERVERIP.str;
	DeleteAttribute(NCFavorites, sAddress);

	if (iCurrentServersList == NET_FG_FAVORITE) { FG_FilterChange(); }
	FG_AddFavOnCancel();
}

void FG_ShowMainWindow()
{
	XI_WindowDisable("WND_NETFINDGAME", false);

	switch (iCurrentServersList)
	{
		case NET_FG_LAN:		XI_WindowDisable("WND_LANSERVER", false);		break;
		case NET_FG_INTERNET:	XI_WindowDisable("WND_INETSERVER", false);		break;
		case NET_FG_FAVORITE:	XI_WindowDisable("WND_FAVSERVER", false);		break;
	}
}

void FG_AddFavOnCancel()
{
	XI_WindowShow("WND_ADDFAVORITES", false);
	XI_WindowDisable("WND_ADDFAVORITES", true);

	FG_ShowMainWindow();
}

void FG_OnAddFavorites()
{
	XI_WindowDisable("WND_LANSERVER", true);
	XI_WindowDisable("WND_INETSERVER", true);
	XI_WindowDisable("WND_FAVSERVER", true);

	XI_WindowDisable("WND_NETFINDGAME", true);

	XI_WindowShow("WND_ADDFAVORITES", true);
	XI_WindowDisable("WND_ADDFAVORITES", false);

	string sValue = "";
	switch (iCurrentServersList)
	{
		case NET_FG_LAN:
			if (iCurrentLanServer >= 0)
			{
				sValue = LanServers[ LanServersList[iCurrentLanServer] ].IP + ":" + LanServers[ LanServersList[iCurrentLanServer] ].Port;
			}
			Button_SetEnable("BTN_ADDFAV_DELETE", false);	
		break;
		case NET_FG_INTERNET:
			if (iCurrentInternetServer >= 0)
			{
				sValue = InternetServers[ InternetServersList[iCurrentInternetServer] ].IP + ":" + InternetServers[ InternetServersList[iCurrentInternetServer] ].Port;
			}
			Button_SetEnable("BTN_ADDFAV_DELETE", false);	
		break;
		case NET_FG_FAVORITE:
			//Button_SetEnable("BTN_ADDFAV_DELETE", iCurrentFavoriteServer >= 0);
		break;
	}

	GameInterface.EDITBOX_SERVERIP.str = sValue;

	bFavWindowActive = true;
}

void FG_OnCreateGame()
{
	IDoExit(RC_INTERFACE_NET_CREATEGAME);
}

bool FG_ParseIPPort(string sIPPort, ref rIP, ref rPort)
{
	int iFindPos = findSubStr(&sIPPort, ":", 0);
	if (iFindPos < 0) 
	{
		rIP = sIPPort;
		rPort = NET_DEFAULT_SERVER_PORT;
		return true;
	}

	rIP = strcut(sIPPort, 0, iFindPos - 1);
	rPort = strcut(sIPPort, iFindPos + 1, strlen(&sIPPort) - 1);
	return true;
}

void FG_OnConnectOk()
{
	NetClient.ServerPassword = GameInterface.EDITBOX_CONNECT_SERVER_PASSWORD.str;
	NetClient.NickName = GameInterface.EDITBOX_CONNECT_NICK.str;
	NetClient.Password = GameInterface.EDITBOX_CONNECT_PASSWORD.str;

	FG_ParseIPPort(GameInterface.EDITBOX_CONNECT_IP.str, &sCurrentIP, &sCurrentPort);

	NetClient_TryConnect(sCurrentIP, sti(sCurrentPort));

	XI_WindowShow("WND_CONNECT", false);
	XI_WindowDisable("WND_CONNECT", true);

	// show tryconnect window
	XI_WindowShow("WND_TRYCONNECT", true);
	XI_WindowDisable("WND_TRYCONNECT", false);

	SetCurrentNode("BTN_TRYCONNECT_CANCEL");

	SetFormatedText("TEXT_TRYCONNECT", XI_ConvertString("Net_TryToConnectToServer") +  sCurrentIP + ":" + sCurrentPort);
	SetFormatedText("TEXT_TRYCONNECT_TIMER", XI_ConvertString("Net_TimeToCancelConnection30"));

	SetEventHandler("frame", "FG_OnTryConnectFrame", 0);
	SetEventHandler("OnTryConnectCancel", "FG_OnTryConnectCancel", 0);
	iTryConnectDeltaTime = 0;

	//FG_ShowMainWindow();
}

void FG_OnTryConnectFrame()
{
	iTryConnectDeltaTime = iTryConnectDeltaTime + iRealDeltaTime;

	SetFormatedText("TEXT_TRYCONNECT_TIMER", XI_ConvertString("Net_TimeToCancelConnection") + (30 - (iTryConnectDeltaTime / 1000)) + XI_ConvertString("seconds"));

	if (iTryConnectDeltaTime >= 30000)
	{
		DelEventHandler("frame", "FG_OnTryConnectFrame");
		DelEventHandler("OnTryConnectCancel", "FG_OnTryConnectCancel");

		FG_OnTryConnectCancel();
	}
}

void FG_OnTryConnectCancel()
{
	XI_WindowShow("WND_TRYCONNECT", false);
	XI_WindowDisable("WND_TRYCONNECT", true);

	FG_ShowMainWindow();
}

void FG_OnConnectCancel()
{
	XI_WindowShow("WND_CONNECT", false);
	XI_WindowDisable("WND_CONNECT", true);

	FG_ShowMainWindow();
}

void FG_OnJoinGame()
{
	string sImage;

	XI_WindowDisable("WND_LANSERVER", true);
	XI_WindowDisable("WND_INETSERVER", true);
	XI_WindowDisable("WND_FAVSERVER", true);

	XI_WindowDisable("WND_NETFINDGAME", true);

	XI_WindowShow("WND_CONNECT", true);
	XI_WindowDisable("WND_CONNECT", false);

	SetCurrentNode("BTN_CONNECT_CANCEL");

	GameInterface.EDITBOX_CONNECT_IP.str = "";
	if (sCurrentIP != "")
	{
		if (sCurrentPort == "") sCurrentPort = NET_DEFAULT_SERVER_PORT;
		GameInterface.EDITBOX_CONNECT_IP.str = sCurrentIP + ":" + sCurrentPort;
	}
	GameInterface.EDITBOX_CONNECT_NICK.str = NetClient.NickName;
	GameInterface.EDITBOX_CONNECT_PASSWORD.str = NetClient.Password;

	/*NetClient.NickName = GameInterface.EDITBOX_PLAYERNAME.str;
	NetClient.Password = GameInterface.EDITBOX_PLAYERPASSWORD.str;
	NetClient.ShipName = GameInterface.EDITBOX_PLAYERSHIPNAME.str;*/

	/*if (sti(GameInterface.FACESLIST.ListSize))
	{
		sImage = "pic" + (sti(GameInterface.FACESLIST.current) + 1);
		NetClient.FaceImage = GameInterface.FACESLIST.(sImage).FileName.Name;
	}
	if (sti(GameInterface.SAILSLIST.ListSize))
	{
		sImage = "pic" + (sti(GameInterface.SAILSLIST.current) + 1);
		NetClient.SailImage = GameInterface.SAILSLIST.(sImage).FileName.Name;
	}
	if (sti(GameInterface.FLAGSLIST.ListSize))
	{
		sImage = "pic" + (sti(GameInterface.FLAGSLIST.current) + 1);
		NetClient.FlagImage = GameInterface.FLAGSLIST.(sImage).FileName.Name;
	}*/

	//NetClient_TryConnect(sCurrentIP, sti(sCurrentPort));
	//IDoExit(RC_INTERFACE_NET_EXIT);
}

void FG_OnCancel()
{
	Net_Stop();
	IDoExit(RC_INTERFACE_NET_EXIT);

	ReturnToMainMenu();
}

bool AddServerToList(string sTableName, ref rServer, int iIndex)
{
	if (!IsFilterOk(rServer)) { return false; }

	string sRow = "tr" + (iIndex + 1);

	aref arTable; makearef(arTable, GameInterface.(sTableName).(sRow));

	int iColor = argb(255, 255, 255, 255);

	if (iCurrentServersList == NET_FG_FAVORITE && sti(rServer.Ping) == 9999) { iColor = argb(255, 155, 155, 155); }

	for (int i=2; i<8; i++)
	{
		string sColumn = "td" + i;
		arTable.(sColumn).scale = 0.85;
		arTable.(sColumn).valign = "center";
		arTable.(sColumn).color = iColor;
	}

	arTable.td1.str = "";
	if (sti(rServer.Secure))
	{
		arTable.td1.icon.group = "NET_SERVERPASSWORD";
		arTable.td1.icon.image = "locked";
		arTable.td1.icon.offset = "-2,0";
		arTable.td1.icon.width = 16;
		arTable.td1.icon.height = 16;
		//arTable.td1.icon.cellsize = true;
	}
	arTable.td2.str = rServer.Name;
	arTable.td3.str = "";
	arTable.td4.str = rServer.IP + ":" + rServer.Port;
	arTable.td5.str = XI_ConvertString("txtCS_" + NetModes[sti(rServer.GameType)].Name));
	arTable.td6.str = rServer.NumClients + " / " + rServer.MaxClients;
	arTable.td7.str = rServer.Ping;

	return true; 
}

void AddServer(ref rServer, string sName, string sIP, int wPort, bool bSecure, int iGameType, int iCredit, int iMaxClass, string sWeatherID, string sIslandID, int iNumClients, int iMaxClients, int iPing)
{
	rServer.Name = sName;
	rServer.IP = sIP;
	rServer.Port = wPort;
	rServer.Secure = bSecure;
	rServer.Credit = iCredit;
	rServer.MaxClass = iMaxClass;
	rServer.GameType = iGameType;
	rServer.WeatherID = sWeatherID;
	rServer.IslandID = sIslandID;
	rServer.NumClients = iNumClients;
	rServer.MaxClients = iMaxClients;
	rServer.Ping = iPing;
}

void NetInterface_AddLocalServerToList(string sName, string sIP, int wPort, bool bSecure, int iGameType, int iCredit, int iMaxClass, string sWeatherID, string sIslandID, int iNumClients, int iMaxClients, int iPing)
{
	AddServer(&LanServers[iNumLanServers], sName, sIP, wPort, bSecure, iGameType, iCredit, iMaxClass, sWeatherID, sIslandID, iNumClients, iMaxClients, iPing);
	if (AddServerToList("TABLE_LANSERVER", &LanServers[iNumLanServers], iNumLanServersList))
	{
		LanServersList[iNumLanServersList] = iNumLanServers;
		iNumLanServersList++;
	}
	iNumLanServers++;
	Table_UpdateWindow("TABLE_LANSERVER");
}

void NetInterface_AddInternetServerToList(string sName, string sIP, int wPort, bool bSecure, int iGameType, int iCredit, int iMaxClass, string sWeatherID, string sIslandID, int iNumClients, int iMaxClients, int iPing)
{
	AddServer(&InternetServers[iNumInternetServers], sName, sIP, wPort, bSecure, iGameType, iCredit, iMaxClass, sWeatherID, sIslandID, iNumClients, iMaxClients, iPing);
	if (AddServerToList("TABLE_INETSERVER", &InternetServers[iNumInternetServers], iNumInternetServersList))
	{
		InternetServersList[iNumInternetServersList] = iNumInternetServers;
		iNumInternetServersList++;
	}
	iNumInternetServers++;
	Table_UpdateWindow("TABLE_INETSERVER");
}