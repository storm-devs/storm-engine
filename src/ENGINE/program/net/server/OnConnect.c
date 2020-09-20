
int NetServer_FindFreeClientID()
{
	for (int i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { return i; }
	}

	return DST_INVALID;
}

void NetServer_DropClient(int wNetClientID)
{
	ref rClient = NetServer_GetClient(wNetClientID);
	rClient.Use = false;

	/*if (bServerGameStarted)
	{
		NetServer_UpdateGlobalPlayerStatistics(rClient);
		DeleteClass(&NSClients[sti(rClient.index)]);
	}*/

	NetServer_DelClient(wNetClientID);

	// send to all
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_CLIENT);
	NMAddByte(iSMsg, NSC_CLIENT_DROP);
	NMAddClientID(iSMsg, wNetClientID);
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_OnDisconnect(int wNetClientID, int iMsg)
{
	NetServer_DropClient(wNetClientID);
}

bool NetServer_IsLocalIP(int dwIP)
{
	int dw1 = and(dwIP, 255);
	int dw2 = and(shr(dwIP, 8), 255);
	int dw3 = and(shr(dwIP, 16), 255);
	int dw4 = shr(dwIP, 24);

	if (dw1 == 192 && dw2 == 168) { return true; }
	if (dw1 == 10) { return true; }
	if (dw1 == 172 && dw2 >= 16 && dw2 <= 31) { return true; }
	if (dw1 == 169 && dw2 == 254) { return true; }
	if (dw1 == 127 && dw2 == 0 && dw2 == 0 && dw2 == 1) { return true; }
	// maybe 169.254

	return false;
}

void NetServer_OnConnect(int dwIP, int wPort, int iMsg)
{
	int i, iSMsg;

	int iNetTestDword = NMGet24(iMsg);

	bool bLocal = NetServer_IsLocalIP(dwIP);
	bool bInet = !bLocal;
	bool bError = false;

	if (bLocal && !sti(NetServer.LanServer)) { bError = true; }
	if (bInet && !sti(NetServer.InetServer)) { bError = true; }
	
	if (bError)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	if (iNetTestDword != NET_TEST_DWORD)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	int iClientNetVersion = NMGetByte(iMsg);
	if (iClientNetVersion != NET_SCRIPT_VERSION)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED_DIFFERENT_VERSION);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	string sNickName = NMGetString(iMsg);
	string sNickPassword = NMGetString(iMsg);
	string sServerPassword = NMGetString(iMsg);
	string sShipName = NMGetString(iMsg);
	string sFaceImage = NMGetString(iMsg);
	string sFlagImage = NMGetString(iMsg);
	string sSailImage = NMGetString(iMsg);
	int iSailColor = NMGetDword(iMsg);

	// check for already connect(maybe if big lag)

	// check for connecting to already played game
	if (bServerGameStarted || (iSecondsToStartGame >= 0))
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	// check password
	if (NetServer.ServerPassword != "" && sServerPassword != NetServer.ServerPassword)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_NEED_PASSWORD);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	// check for maxclients
	if (NetServer_GetClientsNum() >= sti(NetServer.MaxClients))
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED_SERVER_FULL);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}

	// find new client id
	int wNetClientID = NetServer_FindFreeClientID();
	if (wNetClientID == DST_INVALID)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CONNECT);
		NMAddByte(iSMsg, NSC_CONNECT_REJECTED);
		NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
		NMDelete(iSMsg);
		return;
	}
	
	// if find client in NSPlayers, check password
	if (CheckAttribute(NSPlayers, "Players." + sNickName))
	{
		if (NSPlayers.Players.(sNickName).NickPassword != sNickPassword)
		{
			iSMsg = NMCreate();
			NMAddByte(iSMsg, NC_CONNECT);
			NMAddByte(iSMsg, NSC_CONNECT_REJECTED_NEED_CLIENT_PASSWORD);
			NetServer_SendMessageDirect(dwIP, wPort, iSMsg);
			NMDelete(iSMsg);
			return;
		}
	}

	// add new client
	NetServer_AddClient(wNetClientID, dwIP, wPort);

	ref rClient = NetServer_GetCLient(wNetClientID);

	rClient.Use = true;
	rClient.dwIP = dwIP;
	rClient.wPort = wPort;

	rClient.NickName = sNickName;
	rClient.NickPassword = sNickPassword;

	int iShipType = 10;
	ref rShip = Net_GetShipByIndex(iShipType);

	float fRadius = 100.0 + frnd() * 200.0;
	float fAng = frnd() * PIm2;

	rClient.LastPingTime = iPingTime;
	rClient.LastPacketIndex = 0;
	rClient.StartReady = false;
	rClient.BuyReady = false;
	rClient.Dead = false;
	rClient.UserData.Face = sFaceImage;
	rClient.UserData.Flag = sFlagImage;
	rClient.UserData.Sail = sSailImage;
	rClient.Ship.Type = -1;
	rClient.Ship.Name = sShipName;
	rClient.Ship.Pos.x = -1400.0 + fRadius * sin(fAng);
	rClient.Ship.Pos.y = 0.0;
	rClient.Ship.Pos.z = -1900.0 + fRadius * cos(fAng);
	rClient.Ship.Ang.y = frnd() * PIm2;
	rClient.Ship.Speed.x = 0.0;
	rClient.Ship.Speed.y = 0.0;
	rClient.Ship.Speed.z = 0.0;
	rClient.Ship.Upgrades.Hull = 1;
	rClient.Ship.Upgrades.Sails = 1;
	rClient.Ship.Upgrades.Cannons = 1;

	rClient.Ship.Sails.Color = iSailColor;

	// setup goods
/*	for (int i=0; i<Net_GetGoodsNum(); i++)
	{
		ref rGood = Net_GetGoodByIndex(i);
		string sGood = rGood.Name;
		rClient.Ship.Goods.(sGood) = (i + 1) * 1350;
	}*/

	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_CONNECT);
	NMAddByte(iSMsg, NSC_CONNECT_ACCEPTED);
	NMAddClientID(iSMsg, wNetClientID);
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	Trace("Client with IP " + inet_ntoa(dwIP) + ":" + ntohs(wPort) + " accepted. wNetClientID = " + wNetClientID);
	NMDelete(iSMsg);

	// send to all
	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_CLIENT);
	NMAddByte(iSMsg, NSC_CLIENT_CONNECTED);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddString(iSMsg, rClient.NickName, 24);	// nick name
	NMAddString(iSMsg, rClient.Ship.Name, 24);	// nick name
	NMAddString(iSMsg, sFaceImage, 12);							// client face
	NMAddString(iSMsg, sFlagImage, 12);							// client flag
	NMAddString(iSMsg, sSailImage, 12);							// client sail emblem
	NMAddDword(iSMsg, iSailColor);								// client sail color
	NMAddByte(iSMsg, sti(rClient.BuyReady));					// Buy Ready
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	// send to new client all other clients
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{
		ref rOldClient = &NSClients[i];
		if (!sti(rOldClient.Use)) { continue; }
		if (sti(rOldClient.ID) == wNetClientID) { continue; }

		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CLIENT);
		NMAddByte(iSMsg, NSC_CLIENT_CONNECTED);
		NMAddClientID(iSMsg, sti(rOldClient.ID));
		NMAddString(iSMsg, rOldClient.NickName, 24);			// nick name
		NMAddString(iSMsg, rOldClient.Ship.Name, 24);			// nick name
		NMAddString(iSMsg, rOldClient.UserData.Face, 12);		// client face
		NMAddString(iSMsg, rOldClient.UserData.Flag, 12);		// client flag
		NMAddString(iSMsg, rOldClient.UserData.Sail, 12);		// client sail emblem
		NMAddDword(iSMsg, sti(rOldClient.Ship.Sails.Color));	// client sail color
		NMAddByte(iSMsg, sti(rOldClient.BuyReady));				// Buy Ready
		NetServer_SendMessage(wNetClientID, iSMsg, true);
		NMDelete(iSMsg);
	}

	//NetServer_DownloadClientFile(wNetClientID, "resource\textures\" + sUserFacesPath + sFaceImage + "tga.tx");
	//NetServer_DownloadClientFile(wNetClientID, "resource\textures\" + sUserSailsPath + sSailImage + "tga.tx");
	//NetServer_DownloadClientFile(wNetClientID, "resource\textures\" + sUserFlagsPath + sFlagImage + "tga.tx");
}

