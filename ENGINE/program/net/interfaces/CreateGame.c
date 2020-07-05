int iGameType;
int iMaxNumPlayers, iGameSpeed, iTimeLimit, iStartPosition, iCredit, iMaxShipClass;
int iGameTime = 0; 
int iWeatherType = 0;
int iNumTeams = 2;

void InitInterface(string iniName)
{
	GameInterface.title = "titleNetCreateGame";

	GameInterface.TABLE_GAMETYPE.tr1.td1.str = XI_ConvertString("txtCS_" + NetModes[NETGAME_DEATHMATCH].Name);
	GameInterface.TABLE_GAMETYPE.tr2.td1.str = XI_ConvertString("txtCS_" + NetModes[NETGAME_NAVAL].Name);
	GameInterface.TABLE_GAMETYPE.tr3.td1.str = XI_ConvertString("txtCS_" + NetModes[NETGAME_CONVOY].Name);
	GameInterface.TABLE_GAMETYPE.tr4.td1.str = XI_ConvertString("txtCS_" + NetModes[NETGAME_FORT].Name);

	GameInterface.TABLE_GAMETIME.tr1.td1.str = XI_ConvertString("txtCS_Morning");
	GameInterface.TABLE_GAMETIME.tr2.td1.str = XI_ConvertString("txtCS_Day");
	GameInterface.TABLE_GAMETIME.tr3.td1.str = XI_ConvertString("txtCS_Evening");
	GameInterface.TABLE_GAMETIME.tr4.td1.str = XI_ConvertString("txtCS_Night");

	GameInterface.TABLE_WEATHER.tr1.td1.str = XI_ConvertString("txtCS_Calm");
	GameInterface.TABLE_WEATHER.tr2.td1.str = XI_ConvertString("txtCS_Windy");
	GameInterface.TABLE_WEATHER.tr3.td1.str = XI_ConvertString("txtCS_Heavy");
	GameInterface.TABLE_WEATHER.tr4.td1.str = XI_ConvertString("txtCS_Storm");

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	// select checkbox LAN game
	CheckButton_SetState("CHECKBUTTON_LAN", 1, true);

	iMaxNumPlayers = NET_MAXCLIENTS;		
	iGameSpeed = 1;
	iTimeLimit = 0;
	iStartPosition = 0;
	iCredit = 300000;
	iMaxShipClass = 1;
	
	CS_SetNumTeams();
	CS_SetNumPlayers();
	CS_SetGameSpeed();
	CS_SetTimeLimit();
	CS_SetStartPosition();
	CS_SetCredit();
	CS_SetMaxShipClass();

	SetEventHandler("OnCreate", "OnCreate", 0);

	SetEventHandler("OnCancel", "OnCancel", 0);

	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);	

	SetEventHandler("AddPlayer", "CS_AddPlayer", 0);
	SetEventHandler("DecPlayer", "CS_DecPlayer", 0);

	SetEventHandler("AddGameSpeed", "CS_AddGameSpeed", 0);
	SetEventHandler("DecGameSpeed", "CS_DecGameSpeed", 0);

	SetEventHandler("AddTimeLimit", "CS_AddTimeLimit", 0);
	SetEventHandler("DecTimeLimit", "CS_DecTimeLimit", 0);

	SetEventHandler("AddStartPosition", "CS_AddStartPosition", 0);
	SetEventHandler("DecStartPosition", "CS_DecStartPosition", 0);

	SetEventHandler("AddCredit", "CS_AddCredit", 0);
	SetEventHandler("DecCredit", "CS_DecCredit", 0);

	SetEventHandler("AddTeams", "CS_AddTeams", 0);
	SetEventHandler("DecTeams", "CS_DecTeams", 0);

	SetEventHandler("AddMaxShipClass", "CS_AddMaxShipClass", 0);
	SetEventHandler("DecMaxShipClass", "CS_DecMaxShipClass", 0);

	SetEventHandler("CheckButtonChange", "CS_CheckButtonChange", 0);

	FillIslandsList();

	// disable OK window
	XI_WindowShow("WND_OK", false);
	XI_WindowDisable("WND_OK", true);

	// disable RECREATE window
	XI_WindowShow("WND_RECREATE", false);
	XI_WindowDisable("WND_RECREATE", true);

	CheckButton_SetDisable("CHECKBUTTON_DEDICATED", 1, true);
	//CheckButton_SetDisable("CHECKBUTTON_INET", 1, true);
}

int CS_IsShift(int iShift, int iNoShift, int iAlt)
{
	if (XI_IsKeyPressed("shift")) { return iShift; }
	if (XI_IsKeyPressed("alt")) { return iAlt; }
	return iNoShift;
}

void CS_SetNumTeams()
{
	if (iNumTeams > 4) { iNumTeams = 4; }
	if (iNumTeams < 2) { iNumTeams = 2; }

	switch (iGameType)
	{
		case NETGAME_DEATHMATCH:	StringCollection_SetText("STRINGS_VALUE", 1, "#");				break;
		case NETGAME_NAVAL:			StringCollection_SetTextValue("STRINGS_VALUE", 1, iNumTeams);	break;
		case NETGAME_CONVOY:		StringCollection_SetText("STRINGS_VALUE", 1, "#2");				break;
		case NETGAME_FORT:			StringCollection_SetText("STRINGS_VALUE", 1, "#2");				break;
	}
}
void CS_AddTeams() { iNumTeams++; CS_SetNumTeams() }
void CS_DecTeams() { iNumTeams--; CS_SetNumTeams() }

void CS_SetNumPlayers()
{
	if (iMaxNumPlayers > NET_MAXCLIENTS) { iMaxNumPlayers = NET_MAXCLIENTS; }
	if (iMaxNumPlayers < 2) { iMaxNumPlayers = 2; }

	StringCollection_SetTextValue("STRINGS_VALUE", 2, iMaxNumPlayers);
	//Gameinterface.strings.MaxPlayers = iMaxNumPlayers;
}
void CS_AddPlayer() { iMaxNumPlayers++; CS_SetNumPlayers(); }
void CS_DecPlayer() { iMaxNumPlayers--; CS_SetNumPlayers(); }

void CS_SetGameSpeed()
{
	if (iGameSpeed > 4) { iGameSpeed = 4; }
	if (iGameSpeed < 1) { iGameSpeed = 1; }

	StringCollection_SetText("STRINGS_VALUE", 3, "#" + iGameSpeed + "x");
	//Gameinterface.strings.GameSpeed = iGameSpeed + "x";
}
void CS_AddGameSpeed() { iGameSpeed++; CS_SetGameSpeed(); }
void CS_DecGameSpeed() { iGameSpeed--; CS_SetGameSpeed(); }

void CS_SetTimeLimit()
{
	if (iTimeLimit <= 0) 
	{ 
		iTimeLimit = 0; 
		StringCollection_SetText("STRINGS_VALUE", 4, "#");
	}
	else
	{
		StringCollection_SetTextValue("STRINGS_VALUE", 4, iTimeLimit);
	}
	//Gameinterface.strings.TimeLimit = iTimeLimit;
}
void CS_AddTimeLimit() { iTimeLimit = iTimeLimit + 5; CS_SetTimeLimit(); }
void CS_DecTimeLimit() { iTimeLimit = iTimeLimit - 5; CS_SetTimeLimit(); }

void CS_SetStartPosition()
{
	if (iStartPosition < 0) { iStartPosition = 0; }
	if (iStartPosition > 2) { iStartPosition = 2; }
	StringCollection_SetText("STRINGS_VALUE", 5, "#" + XI_ConvertString("txtNetCS_StartPosition" + iStartPosition));
	//Gameinterface.strings.StartPosition = XI_ConvertString("txtNetCS_StartPosition" + iStartPosition);
}
void CS_AddStartPosition() { iStartPosition++; CS_SetStartPosition(); }
void CS_DecStartPosition() { iStartPosition--; CS_SetStartPosition(); }

void CS_SetCredit()
{
	if (iCredit < 5000) { iCredit = 5000; }
	StringCollection_SetTextValue("STRINGS_VALUE", 6, iCredit);
	//Gameinterface.strings.Credit = iCredit;
}
void CS_AddCredit() { iCredit = iCredit + 1000 * CS_IsShift(10, 1, 100); CS_SetCredit(); }
void CS_DecCredit() { iCredit = iCredit - 1000 * CS_IsShift(10, 1, 100); CS_SetCredit(); }

void CS_SetMaxShipClass()
{
	if (iMaxShipClass < 1) { iMaxShipClass = 1; }
	if (iMaxShipClass > 7) { iMaxShipClass = 7; }
	StringCollection_SetTextValue("STRINGS_VALUE", 7, iMaxShipClass);
	//Gameinterface.strings.MaxShipClass = iMaxShipClass;
}
void CS_AddMaxShipClass() { iMaxShipClass++; CS_SetMaxShipClass(); }
void CS_DecMaxShipClass() { iMaxShipClass--; CS_SetMaxShipClass(); }

void FillIslandsList()
{
	int iForts = 1;

	GameInterface.TABLE_MAPLIST.tr1.td1.str = XI_ConvertString("OpenSea");
	GameInterface.TABLE_MAPLIST.tr1.td1.IslandID = "";
	GameInterface.TABLE_MAPLIST.tr1.td1.my_texture = "Interfaces\Net\Islands\OpenSea.tga";
	GameInterface.TABLE_MAPLIST.select = 1;

	GameInterface.TABLE_MAPLIST_FORT.select = 1;

	for (int i=0; i<Net_GetIslandsNum(); i++)
	{
		ref rIsland = Net_GetIslandByIndex(i);

		string sTexture = "Interfaces\Net\Islands\" + rIsland.ID + ".tga";

		string sTest = "tr" + (i + 2);
		GameInterface.TABLE_MAPLIST.(sTest).td1.str = XI_ConvertString(rIsland.ID);
		GameInterface.TABLE_MAPLIST.(sTest).td1.IslandID = rIsland.ID;
		GameInterface.TABLE_MAPLIST.(sTest).td1.my_texture = sTexture;

		if (Net_IsIslandHaveForts(rIsland))
		{
			sTest = "tr" + iForts;
			GameInterface.TABLE_MAPLIST_FORT.(sTest).td1.str = XI_ConvertString(rIsland.ID);
			GameInterface.TABLE_MAPLIST_FORT.(sTest).td1.IslandID = rIsland.ID;
			GameInterface.TABLE_MAPLIST_FORT.(sTest).td1.my_texture = sTexture;
			iForts++;
		}
	}

	Table_UpdateWindow("TABLE_MAPLIST");
	Table_UpdateWindow("TABLE_MAPLIST_FORT");

	CS_SelectNewGameType();
}

void CS_SelectNewIsland()
{
	string sControl = "TABLE_MAPLIST";
	if (iGameType == NETGAME_FORT) { sControl = "TABLE_MAPLIST_FORT"; }
	string sRow = "tr" + sti(GameInterface.(sControl).select);
	string sTexture = GameInterface.(sControl).(sRow).td1.my_texture;

	Picture_SetPicture("SERVER_MAP_PICTURE", sTexture);
}

void CS_SelectNewGameType()
{
	iGameType = sti(GameInterface.TABLE_GAMETYPE.select) - 1;

	SetNodeUsing("TABLE_MAPLIST", false);
	SetNodeUsing("SCROLL_MAPLIST", false);
	SetNodeUsing("TABLE_MAPLIST_FORT", false);
	SetNodeUsing("SCROLL_MAPLIST_FORT", false);

	if (iGameType == NETGAME_FORT)
	{
		SetNodeUsing("TABLE_MAPLIST_FORT", true);
		SetNodeUsing("SCROLL_MAPLIST_FORT", true);
	}
	else
	{
		SetNodeUsing("TABLE_MAPLIST", true);
		SetNodeUsing("SCROLL_MAPLIST", true);
	}

	CS_SelectNewIsland();
	CS_SetNumTeams();
}

void CS_TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	
	if (sControl == "TABLE_GAMETYPE") { CS_SelectNewGameType(); return; }
	if (sControl == "TABLE_MAPLIST" || sControl == "TABLE_MAPLIST_FORT") { CS_SelectNewIsland(); return; }

	if (sControl == "TABLE_GAMETIME") { iGameTime = iSelected - 1; return; }
	if (sControl == "TABLE_WEATHER") { iWeatherType = iSelected - 1; return; }
}

void IDoExit(int exitCode)
{
	DelEventHandler("OnCreate", "OnCreate");
	DelEventHandler("OnCancel", "OnCancel");

	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);	

	DelEventHandler("AddPlayer", "CS_AddPlayer");
	DelEventHandler("DecPlayer", "CS_DecPlayer");

	DelEventHandler("AddGameSpeed", "CS_AddGameSpeed");
	DelEventHandler("DecGameSpeed", "CS_DecGameSpeed");

	DelEventHandler("AddTimeLimit", "CS_AddTimeLimit");
	DelEventHandler("DecTimeLimit", "CS_DecTimeLimit");

	DelEventHandler("AddStartPosition", "CS_AddStartPosition");
	DelEventHandler("DecStartPosition", "CS_DecStartPosition");

	DelEventHandler("AddCredit", "CS_AddCredit");
	DelEventHandler("DecCredit", "CS_DecCredit");

	DelEventHandler("AddTeams", "CS_AddTeams");
	DelEventHandler("DecTeams", "CS_DecTeams");

	DelEventHandler("AddMaxShipClass", "CS_AddMaxShipClass");
	DelEventHandler("DecMaxShipClass", "CS_DecMaxShipClass");

	DelEventHandler("CheckButtonChange", "CS_CheckButtonChange");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void CS_CheckButtonChange()
{
	string sNode = GetEventData();
	int iButtonNumber = GetEventData();
	int bChecked = GetEventData();
	
	if (sNode == "CHECKBUTTON_LAN" && !bChecked)
	{
		//CheckButton_SetState("CHECKBUTTON_LAN", 1, true);
	}
}

string CS_GetCurrentIsland()
{
	string sControl = "TABLE_MAPLIST";
	if (iGameType == NETGAME_FORT) { sControl = "TABLE_MAPLIST_FORT"; }
	string sRow = "tr" + sti(GameInterface.(sControl).select);
	return GameInterface.(sControl).(sRow).td1.IslandID;
}

string CS_GetCurrentWeather()
{
	switch (iGameTime)
	{
		case 0: 
			switch (iWeatherType)
			{
				case 0:	return "Six Hours"; break;
				case 1:	return "Eight Hour"; break;
				case 2:	return "Ten Hours"; break;
				case 3:	return "Overcast2"; break;
			}
		break;
		case 1: 
			switch (iWeatherType)
			{
				case 0:	return "Eleven Hour"; break;
				case 1:	return "Fourhteen Hour"; break;
				case 2:	return "Sixteen Nour"; break;
				case 3:	return "Stormy1"; break;
			}
		break;
		case 2: 
			switch (iWeatherType)
			{
				case 0:	return "Nineteen Hour"; break;
				case 1:	return "TwentyTwo Hour"; break;
				case 2:	return "TwentyThree Hour"; break;
				case 3:	return "Overcast1"; break;
			}
		break;
		case 3: 
			switch (iWeatherType)
			{
				case 0:	return "Tweny Four Hour"; break;
				case 1:	return "One Hour"; break;
				case 2:	return "Three Hour"; break;
				case 3:	return "Overcast5"; break;
			}
		break;
	}
}

void CS_CreateServer()
{
	Object oSrvParams;

	int iCurrentNumTeams = 2;

	switch (iGameType)
	{
		case NETGAME_DEATHMATCH:	iCurrentNumTeams = 1;			break;
		case NETGAME_NAVAL:			iCurrentNumTeams = iNumTeams;	break;
		case NETGAME_CONVOY:		break;
		case NETGAME_FORT:			break;
	}

	oSrvParams.ServerName = GameInterface.EDITBOX_GAMENAME.str;
	oSrvParams.ServerPort = NET_DEFAULT_SERVER_PORT;
	oSrvParams.ServerPassword = GameInterface.EDITBOX_GAMEPASSWORD.str;
	oSrvParams.Dedicated = sti(GameInterface.CHECKBUTTON_DEDICATED.state1);
	oSrvParams.LanServer = sti(GameInterface.CHECKBUTTON_LAN.state1);
	oSrvParams.InetServer = sti(GameInterface.CHECKBUTTON_INET.state1);
	oSrvParams.UpdateMasterServer = true;
	oSrvParams.MaxFps = 100;
	oSrvParams.GameType = iGameType;
	oSrvParams.NumTeams = iCurrentNumTeams;
	oSrvParams.MaxPlayers = iMaxNumPlayers;
	oSrvParams.GameSpeed = iGameSpeed;
	oSrvParams.TimeLimit = iTimeLimit;
	oSrvParams.StartPosition = iStartPosition;
	oSrvParams.Credit = iCredit;
	oSrvParams.MaxShipClass = iMaxShipClass;
	oSrvParams.IslandID = CS_GetCurrentIsland();
	oSrvParams.WeatherID = CS_GetCurrentWeather();
	oSrvParams.AllowUpload = true;
	oSrvParams.AllowDownload = true;

	Net_CreateServer(&oSrvParams);

	IDoExit(RC_INTERFACE_NET_FINDGAME);
}

void CS_OnMustOk() { XI_WindowDisable("WND_NETCREATEGAME", false); }

void CS_RestartOk() { XI_WindowDisable("WND_NETCREATEGAME", false); CS_CreateServer(); }
void CS_RestartCancel() { XI_WindowDisable("WND_NETCREATEGAME", false); }

void CS_OnOk()
{
	XI_WindowDisable("WND_NETCREATEGAME", false);

	XI_WindowShow("WND_OK", false);
	XI_WindowDisable("WND_OK", true);

	XI_WindowShow("WND_RECREATE", false);
	XI_WindowDisable("WND_RECREATE", true);
}

void CS_EventCreateServer()
{
	DelEventHandler("CS_EventCreateServer", "CS_EventCreateServer");
	CS_OnOk();
	CS_CreateServer();
}

void CS_RecreateOk()
{
	Net_DeleteServer();
	SetEventHandler("CS_EventCreateServer", "CS_EventCreateServer", 0);
	PostEvent("CS_EventCreateServer", 1);
}

void CS_RecreateCancel()
{
	CS_OnOk();
}

void OnCreate()
{
	bool bError = false;

	// if server name not exist
	if (GameInterface.EDITBOX_GAMENAME.str == "")
	{
		bError = true;

		SetFormatedText("TEXT_OK", XI_ConvertString("Net_YouNeedtoEnterServerName"));

		XI_WindowDisable("WND_NETCREATEGAME", true);

		XI_WindowShow("WND_OK", true);
		XI_WindowDisable("WND_OK", false);

		SetEventHandler("OnOk", "CS_OnOk", 0);

		return;
	}

	// if LanServer == false && InetServer == false 
	if (sti(GameInterface.CHECKBUTTON_LAN.state1) == false && sti(GameInterface.CHECKBUTTON_INET.state1) == false)
	{
		SetFormatedText("TEXT_OK", XI_ConvertString("Net_YouNeedtoChoseServerMode"));

		XI_WindowDisable("WND_NETCREATEGAME", true);

		XI_WindowShow("WND_OK", true);
		XI_WindowDisable("WND_OK", false);

		SetEventHandler("OnOk", "CS_OnOk", 0);

		return;
	}

	if (NetServer_IsStarted())
	{
		SetFormatedText("TEXT_RECREATE", XI_ConvertString("Net_ServerAlreadyRun"));

		XI_WindowDisable("WND_NETCREATEGAME", true);

		XI_WindowShow("WND_RECREATE", true);
		XI_WindowDisable("WND_RECREATE", false);

		SetEventHandler("OnRecreateOk", "CS_RecreateOk", 0);
		SetEventHandler("OnRecreateCancel", "CS_RecreateCancel", 0);

		return;
	}

	CS_CreateServer();
}

void OnCancel()
{
	IDoExit(RC_INTERFACE_NET_FINDGAME);
}