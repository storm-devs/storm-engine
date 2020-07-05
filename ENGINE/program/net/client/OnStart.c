object oGameOver;

void NetClient_OnEndGame()
{
	for (int i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NCClients[i].Use)) { continue; }
		DeleteClass(&NCClients[i]);
	}

	NetClient_DeleteWeatherEnvironment();
	NetClient_DeleteIslandEnvironment();
	NetClient_DeleteBallsEnvironment();
	NetClient_DeleteRiggingEnvironment();
	NetClient_DeleteCamerasEnvironment();
	NetClient_DeleteShipEnvironment();
	NetClient_DeleteShipLightsEnvironment();
	NetClient_DeleteFortEnvironment();
	NetClient_DeleteTimeEnvironment();
	NetClient_DeleteBattleInterface();
	NetClient_DeleteInterfaceEnvironment();
	NetClient_DeleteShipFoamEnvironment();
	NetClient_DeleteChatEnvironment();

	DelEventHandler("Control Activation", "NetClient_ControlActivation");
	DelEventHandler("NCEvent_Disconnect", "NetClient_OnDisconnect");
	DelEventHandler("NetClient_OnDisconnectStartFindGame", "NetClient_OnDisconnectStartFindGame");
}

void NetClient_OnDisconnectStartFindGame()
{
	DelEventHandler("NetClient_OnDisconnectStartFindGame", "NetClient_OnDisconnectStartFindGame");

	PrepareNetEnvironment();
	LaunchNetFindGame();
}

void NetClient_OnDisconnect()
{
	Net_DeleteClient();

	if (sti(InterfaceStates.Launched))
	{
		interfaceResultCommand = RC_INTERFACE_DO_NOTHING;
		EndCancelInterface(true);
	}

	SetEventHandler("NetClient_OnDisconnectStartFindGame", "NetClient_OnDisconnectStartFindGame", 0);
	PostEvent("NetClient_OnDisconnectStartFindGame", 1);
}

void NetClient_DeleteShipFoamEnvironment()
{
	DeleteClass(&Seafoam);
}

void NetClient_CreateShipFoamEnvironment(int iMsg)
{
	Seafoam.Server = false;
	CreateEntity(&Seafoam, "Seafoam");				
	
	LayerAddObject("net_execute", &Seafoam, -1);
	LayerAddObject("net_realize", &Seafoam, -1);
	
	if (NetClient_WhrIsStorm()) { Seafoam.storm = "true"; }
}

void NetClient_OnStartGame(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_STARTGAME_GETBUYINFO:
			Event("NCEvent_StartGame_GetBuyInfo");
		break;
		case NSC_STARTGAME_ENVIRONMENT:
			Event("NCEvent_StartGame");

			NetClient_CreateWeatherEnvironment(iMsg);
			NetClient_CreateIslandEnvironment(iMsg);
			NetClient_CreateBallsEnvironment(iMsg);
			NetClient_CreateRiggingEnvironment(iMsg);
			NetClient_CreateCamerasEnvironment(iMsg);
			NetClient_CreateShipEnvironment(iMsg);
			NetClient_CreateShipLightsEnvironment(iMsg);
			NetClient_CreateFortEnvironment(iMsg);
			NetClient_CreateTimeEnvironment(iMsg);
			NetClient_CreateShipFoamEnvironment(iMsg);

			// particles init last because crashed if deleted first when ALT-F4 pressed
			InitParticles(); MoveParticlesToLayers("net_execute", "net_realize");

			SetEventHandler("Control Activation", "NetClient_ControlActivation", 0);

			PauseAllSounds();
			ResetSoundScheme();
			SetMusic("music_net");
		break;
		case NSC_STARTGAME_GAMEOVER:
			NetClient_OnGameOver(iMsg);
		break;
	}
}

void NetClient_OnGameOver(int iMsg)
{
	int iGameMode = NMGetByte(iMsg);
	int iWinnerID = NMGetClientID(iMsg);
	int iWinnerTeam = NMGetByte(iMsg);

	string sTeam = "";

	switch (iGameMode)
	{
		case NETGAME_DEATHMATCH:
			ref rClient = NetClient_GetClient(iWinnerID);
			string sNick = rClient.NickName;
			Event(PARSE_STRING, "asls", &oGameOver, LanguageConvertString(iLangNetClient, "NetEG_DeathMatch"), 1, sNick);
		break;
		case NETGAME_NAVAL:
			sTeam = XI_ConvertString("nteam_" + NetTeamName[iWinnerTeam]);
			Event(PARSE_STRING, "asls", &oGameOver, LanguageConvertString(iLangNetClient, "NetEG_Team"), 1, sTeam);
		break;
		case NETGAME_CONVOY:
			sTeam = XI_ConvertString("nteam_" + NetTeamName[iWinnerTeam]);
			Event(PARSE_STRING, "asls", &oGameOver, LanguageConvertString(iLangNetClient, "NetEG_Team"), 1, sTeam);
		break;
		case NETGAME_FORT:
			sTeam = XI_ConvertString("nteam_" + NetTeamName[iWinnerTeam]);
			Event(PARSE_STRING, "asls", &oGameOver, LanguageConvertString(iLangNetClient, "NetEG_Team"), 1, sTeam);
		break;
	}

	LaunchNetEndGameScreen();
}


void NetClient_ControlActivation()
{
	string sControl = GetEventData();
	
	switch (sControl)
	{
		case "Net_Menu":
			LaunchNetMenu();
		break;
	}
}