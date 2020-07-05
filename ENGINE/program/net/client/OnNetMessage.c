
void NetClient_OnNetMessage()
{
	int dwIP = GetEventData();
	int wPort = GetEventData();
	int iMsg = GetEventData();
	int iPacketIndex = GetEventData();

	if (iPacketIndex)
	{
		// callback garanted message
		int iSMsg = NMCreate();
		NMAddClientID(iSMsg, wClientID);
		NMAddByte(iSMsg, NC_GARANTED);
		NMAddWord(iSMsg, iPacketIndex);
		NetClient_SendMessageDirect(iSMsg);
		NMDelete(iSMsg);

		// skip duplicated packets
		if (iPacketIndex <= sti(NetClient.ClientLastPacketIndex)) { return; }	// check for 32767 << 1

		NetClient.ClientLastPacketIndex = iPacketIndex;
	}

	while (NMNotEmpty(iMsg))
	{
		int iCode = NMGetByte(iMsg);

		switch (iCode)
		{
			case NC_GARANTED:		NetClient_OnGaranted(iMsg);						break;
			case NC_BALL:			NetClient_OnBall(iMsg);							break;
			case NC_SHIP:			NetClient_OnShip(iMsg);							break;
			case NC_FORT:			NetClient_OnFort(iMsg);							break;
			case NC_SAY:			NetClient_OnSay(iMsg);							break;
			case NC_PING:			NetClient_OnPing(iMsg);							break;
			case NC_CONNECT:		NetClient_OnConnect(iMsg);						break;
			case NC_STARTGAME:		NetClient_OnStartGame(iMsg);					break;
			case NC_WEATHER:		NetClient_OnWeather(iMsg); 						break;
			case NC_CLIENT:			NetClient_OnClient(iMsg);						break;
			case NC_CAMERA:			NetClient_OnCamera(iMsg);						break;
			case NC_BROADCAST:		NetClient_OnBroadcast(dwIP, wPort, iMsg);		break;
			case NC_MASTERSERVER:	NetClient_OnMasterServer(dwIP, wPort, iMsg);	break;
			case NC_FILE:			NetClient_OnFile(iMsg);							break;
			case NC_SERVERINFO:		NetClient_OnServerInfo(iMsg);					break;
			case NC_PLAYERINFO:		NetClient_OnPlayerInfo(iMsg);					break;
			case NC_STATISTICS:		NetClient_OnStatistics(iMsg);					break;
			case NC_GAMEINFO:		NetClient_OnGameInfo(iMsg);						break;
		}
	}
}

void NetClient_OnGameInfo(int iMsg)
{
	int iGameType = NMGetByte(iMsg);

	NetClient.ConvoyShipID = NMGetClientID(iMsg);
	NetClient.ConvoyPoint.x = NMGetFloat(iMsg);
	NetClient.ConvoyPoint.z = NMGetFloat(iMsg);

	CreateEntity(&NCLightPillar, "NetLightPillar");
	LayerAddObject("net_execute", NCLightPillar, 10000);
	LayerAddObject("net_realize", NCLightPillar, 10000);

	NCLightPillar.Radius = CONVOY_POINT_RADIUS;
	NCLightPillar.Height = 500.0;
	NCLightPillar.Color1 = argb(128, 255, 255, 255);
	NCLightPillar.Color2 = argb(0, 128, 128, 128);
	NCLightPillar.Pos.x = NetClient.ConvoyPoint.x;
	NCLightPillar.Pos.z = NetClient.ConvoyPoint.z;

	NetBI_CreateShipPointer();
	NetBI_SelectShipPointer(sti(NetClient.ConvoyShipID));
}

void NetClient_OnGaranted(int iMsg)
{
	int iGaranted = NMGetWord(iMsg);
	NetClient_GarantedDeliveryCallback(iGaranted);
}

void NetClient_OnMasterServer(int dwIP, int wPort, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	switch (iSubCode)
	{
		case NSC_MASTERSERVER_CONNECT:
			Event("NCEvent_MasterServer_Connect", "l", iMsg);
		break;
		case NSC_MASTERSERVER_GETSERVERS:
			Event("NCEvent_MasterServer_GetServers", "l", iMsg);
		break;
	}
}

void NetClient_OnBroadcast(int dwIP, int wPort, int iMsg)
{
	int byCallbackCode = NMGetByte(iMsg);
	int iBroacastTime = NMGetDword(iMsg);

	string sServerName = NMGetString(iMsg);
	bool bSecure = NMGetByte(iMsg);
	int iGameType = NMGetByte(iMsg);
	string sIslandID = NMGetString(iMsg);
	string sWeatherID = NMGetString(iMsg);
	int iNumClients = NMGetWord(iMsg);
	int iMaxClients =  NMGetWord(iMsg);
	int iCredit = NMGetDword(iMsg);
	int iMaxShipClass = NMGetByte(iMsg);

	int iPingTime = (Net_GetTickCount() - iBroacastTime) / 2;
	Trace("Found server: " + sServerName + ", num clients:" + iNumClients + ", server IP: " + inet_ntoa(dwIP) + ":" + ntohs(wPort) + ", ping: " + iPingTime);

	switch (byCallbackCode)
	{
		case NSC_BROADCAST_LOCAL:
			NetInterface_AddLocalServerToList(sServerName, inet_ntoa(dwIP), ntohs(wPort), bSecure, iGameType, iCredit, iMaxShipClass, sWeatherID, sIslandID, iNumClients, iMaxClients, iPingTime);
		break;
		case NSC_BROADCAST_INET:
			NetInterface_AddInternetServerToList(sServerName, inet_ntoa(dwIP), ntohs(wPort), bSecure, iGameType, iCredit, iMaxShipClass, sWeatherID, sIslandID, iNumClients, iMaxClients, iPingTime);
		break;
	}
}

void NetClient_OnStatistics(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int iPIndex = NMGetDword(iMsg);
	if (iPIndex == -1)
	{
		Event("NCEvent_PlayerStatistic", "l", -1);
		return;
	}

	string sNickName = NMGetString(iMsg);
	string sFaceImage = NMGetString(iMsg);

	int iRank = NMGetByte(iMsg);
	int iRewardAccuracy = NMGetByte(iMsg);
	int iRewardVitality = NMGetByte(iMsg);
	int iRewardVictorious = NMGetByte(iMsg);

	int iWon = NMGetWord(iMsg);
	int iLost = NMGetWord(iMsg);

	int iWrecks = NMGetWord(iMsg);
	int iSunk = NMGetWord(iMsg);
	
	float fAccuracy = NMGetFloat(iMsg);
	int iTotalDamage = NMGetDword(iMsg);

	Event("NCEvent_PlayerStatistic", "lssllllllllfl", iPIndex, sNickName, sFaceImage, iRank, iRewardAccuracy, iRewardVitality, iRewardVictorious, iWon, iLost, iWrecks, iSunk, fAccuracy, iTotalDamage);
}
