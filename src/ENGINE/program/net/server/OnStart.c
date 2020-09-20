bool bServerGameStarted = false;
int iServerEnvSMsg;

void NetServer_EndGame(bool bReconnect)
{
	//if (!bServerGameStarted) { return; }
	bServerGameStarted = false;

	NetServer_ClearPacketsQueue();

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_CONNECT);
	if (bReconnect)
		{ NMAddByte(iSMsg, NSC_CONNECT_RECONNECT); }
	else
		{ NMAddByte(iSMsg, NSC_CONNECT_DISCONNECT); }

	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	if (sti(NetServer.GameType) == NETGAME_DEATHMATCH)
	{
		if (sti(NetServer.wWinnerClientID) != DST_INVALID)
		{
			for (i=0; i<NET_MAXCLIENTS; i++)
			{
				if (!sti(NSClients[i].Use)) { continue; }

				NSClients[i].Stat.Won = sti(NSClients[i].ID) == sti(NetServer.wWinnerClientID);
				NSClients[i].Stat.Lost = sti(NSClients[i].ID) != sti(NetServer.wWinnerClientID);
			}
		}
	}
	else
	{
		if (sti(NetServer.iWinnerTeam) >= 0)
		{
			for (i=0; i<NET_MAXCLIENTS; i++)
			{
				if (!sti(NSClients[i].Use)) { continue; }

				NSClients[i].Stat.Won = sti(NSClients[i].Team) == sti(NetServer.iWinnerTeam);
				NSClients[i].Stat.Lost = sti(NSClients[i].Team) != sti(NetServer.iWinnerTeam);
			}
		}
	}

	// disconnect clients
	for (int i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }
		
		NetServer_UpdateGlobalPlayerStatistics(&NSClients[i]);
		DeleteClass(&NSClients[i]);
	}

	NetServer_DeleteWeatherEnvironment();
	NetServer_DeleteIslandEnvironment();
	NetServer_DeleteBallsEnvironment();
	NetServer_DeleteTouchEnvironment();
	NetServer_DeleteRiggingEnvironment();
	NetServer_DeleteShipEnvironment();
	NetServer_DeleteFortEnvironment();
	NetServer_DeleteTimeEnvironment();

	NMDelete(iServerEnvSMsg);

	DelEventHandler("frame", "NetServer_OnCheckGameOverFrame");
	DelEventHandler("NetServer_StartServerCountDown", "NetServer_StartServerCountDown");
}

void NetServer_OnCheckGameOverFrame()
{
	int i;

	// check the game over - only if game started
	if (!sti(NetServer.GameOver))
	{
		NetServer.wWinnerClientID = DST_INVALID;		// for deathmatch
		NetServer.iWinnerTeam = -1;						// for all other modes

		bool bCurrentGameOver = false;
		switch (sti(NetServer.GameType))
		{
			case NETGAME_DEATHMATCH:	
				bCurrentGameOver = NetServer_CheckGameOver_DeathMatch();
			break;
			case NETGAME_NAVAL:			
				bCurrentGameOver = NetServer_CheckGameOver_Naval();
			break;
			case NETGAME_CONVOY:		
				bCurrentGameOver = NetServer_CheckGameOver_Convoy();
			break;
			case NETGAME_FORT:			
				bCurrentGameOver = NetServer_CheckGameOver_Fort();
			break;
		}

		if (!NET_DEBUG && bCurrentGameOver)
		{
			// game is over
			NetServer.GameOver = true;
			int iSMsg = NMCreate();
			NMAddByte(iSMsg, NC_STARTGAME);
			NMAddByte(iSMsg, NSC_STARTGAME_GAMEOVER);
			NMAddByte(iSMsg, sti(NetServer.GameType));
			NMAddClientID(iSMsg, sti(NetServer.wWinnerClientID));
			NMAddByte(iSMsg, sti(NetServer.iWinnerTeam));
			NetServer_SendMessage(DST_ALL, iSMsg, true);
			NMDelete(iSMsg);
		}
	}
}

void NetServer_RestartGame()
{
	Net_DeleteServer();

	//Net_CreateServer();
}

void NetServer_StartGame()
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_STARTGAME);
	NMAddByte(iSMsg, NSC_STARTGAME_GETBUYINFO);
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_StartServerGame()
{
	int		iSMsg;
	int		i;
	int		iTeams[4];	// 
	object	oTeams[4];	// team positions

	for (i=0; i<4; i++) { iTeams[i] = 0; }

	if (bServerGameStarted) { return; }
	//DelEventHandler("NetServer_StartServerGame", "NetServer_StartServerGame");

	bServerGameStarted = true;

		int iServerEnvSMsg = NMCreate();
		NMAddByte(iServerEnvSMsg, NC_STARTGAME);
		NMAddByte(iServerEnvSMsg, NSC_STARTGAME_ENVIRONMENT);

	// create all environments
		NetServer_CreateWeatherEnvironment(iServerEnvSMsg);
		NetServer_CreateIslandEnvironment(iServerEnvSMsg);
		NetServer_CreateBallsEnvironment(iServerEnvSMsg);
		NetServer_CreateTouchEnvironment(iServerEnvSMsg);
		NetServer_CreateRiggingEnvironment(iServerEnvSMsg);
		NetServer_CreateShipEnvironment(iServerEnvSMsg);
		NetServer_CreateFortEnvironment(iServerEnvSMsg);
		NetServer_CreateTimeEnvironment(iServerEnvSMsg);

	// send message to all client's for start game and load environment
		NetServer_SendMessage(DST_ALL, iServerEnvSMsg, true);
		NMDelete(iServerEnvSMsg);

	// setup group positions
	if (NetServer.IslandID == "")
	{
		switch (sti(NetServer.GameType))
		{
			case NETGAME_DEATHMATCH:
				oTeams[0].Pos.x = 0.0;
				oTeams[0].Pos.z = 0.0;
			break;
			case NETGAME_NAVAL:
				oTeams[0].Pos.x = -500.0;	oTeams[1].Pos.x = 500.0;	oTeams[2].Pos.x = 500.0;	oTeams[3].Pos.x = -500.0;
				oTeams[0].Pos.z = -500.0;	oTeams[1].Pos.z = -500.0;	oTeams[2].Pos.z = 500.0;	oTeams[3].Pos.z = 500.0;
			break;
			case NETGAME_CONVOY:
				oTeams[0].Pos.x = 1300.0;	oTeams[1].Pos.x = 400.0;
				oTeams[0].Pos.z = 1300.0;	oTeams[1].Pos.z = 400.0;
			break;
		}
	}
	else
	{
		int iRand;
		string s1, s2, s3, s4;
		ref rIsland = Net_GetIsland(NetServer.IslandID);

		string sStartPosition = "l" + (2 + sti(NetServer.StartPosition));

		aref arConvoy, arTeam, arDeathmatch;
		makearef(arConvoy, rIsland.net_convoy);
		makearef(arTeam, rIsland.net_team);
		makearef(arDeathmatch, rIsland.net_deathmatch);

		int iNumConvoySpawns = GetAttributesNum(arConvoy) / 3;
		int iNumTeamSpawns = GetAttributesNum(arTeam) / 4;
		int iNumDeathmatchSpawns = GetAttributesNum(arDeathmatch);

		switch (sti(NetServer.GameType))
		{
			case NETGAME_DEATHMATCH:
				sStartPosition = "l" + (1 + rand(iNumDeathmatchSpawns - 1));
				oTeams[0].Pos.x = stf(arDeathmatch.(sStartPosition).x);
				oTeams[0].Pos.z = stf(arDeathmatch.(sStartPosition).z);
			break;
			case NETGAME_NAVAL:
				iRand = 4 * rand(iNumTeamSpawns - 1);
				s1 = "l" + (iRand + 1);
				s2 = "l" + (iRand + 2);
				s3 = "l" + (iRand + 3);
				s4 = "l" + (iRand + 4);

				oTeams[0].Pos.x = stf(arTeam.(s1).x);	oTeams[0].Pos.z = stf(arTeam.(s1).z);
				oTeams[1].Pos.x = stf(arTeam.(s2).x);	oTeams[1].Pos.z = stf(arTeam.(s2).z);
				oTeams[2].Pos.x = stf(arTeam.(s3).x);	oTeams[2].Pos.z = stf(arTeam.(s3).z);
				oTeams[3].Pos.x = stf(arTeam.(s4).x);	oTeams[3].Pos.z = stf(arTeam.(s4).z);
			break;
			case NETGAME_FORT:
				oTeams[0].Pos.x = stf(rIsland.net_fort.l1.x);
				oTeams[0].Pos.z = stf(rIsland.net_fort.l1.z);

				oTeams[1].Pos.x = stf(rIsland.net_fort.(sStartPosition).x);
				oTeams[1].Pos.z = stf(rIsland.net_fort.(sStartPosition).z);
			break;
			case NETGAME_CONVOY:
				iRand = iNumConvoySpawns * rand(iNumTeamSpawns - 1);
				s1 = "l" + (iRand + 1);
				s2 = "l" + (iRand + 2);
				s3 = "l" + (iRand + 3);

				NetServer.ConvoyPoint.x = stf(arConvoy.(s2).x);
				NetServer.ConvoyPoint.z = stf(arConvoy.(s2).z);

				oTeams[0].Pos.x = stf(arConvoy.(s1).x);
				oTeams[0].Pos.z = stf(arConvoy.(s1).z);

				oTeams[1].Pos.x = stf(arConvoy.(s3).x);
				oTeams[1].Pos.z = stf(arConvoy.(s3).z);
			break;
		}
	}

	// load players
	
	// send message to all client's for load clients

	// client
	// 1. Ship Type
	//		a. Ship Name
	//		b. Ship Upgrade
	// 2. Cannons Type
	// 3. Crew quantity
	// 4. Skills
	// 5. Perks

	for (i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }

		NSClients[i].Server = true;

		int iTeam = sti(NSClients[i].Team);

		if ((iTeam == 0) && (sti(NetServer.GameType) == NETGAME_CONVOY) && (iTeams[0] == 0))
		{
			NetServer.ConvoyShipID = NSClients[i].ID;
		}

		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_CLIENT);
		NMAddByte(iSMsg, NSC_CLIENT_LOAD);
		NMAddClientID(iSMsg, i);									// clientID
		NMAddString(iSMsg, NSClients[i].NickName, 24);				// Nick Name
		NMAddWord(iSMsg, sti(NSClients[i].Ship.Type));				// Ship Type
		NMAddFloat(iSMsg, stf(NSClients[i].Ship.HP));				// Ship HP
		NMAddByte(iSMsg, sti(NSClients[i].Ship.Upgrades.Hull));		// Ship Upgrade: Hull
		NMAddByte(iSMsg, sti(NSClients[i].Ship.Upgrades.Sails));	// Ship Upgrade: Masts
		NMAddByte(iSMsg, sti(NSClients[i].Ship.Cannons.Type));		// Cannons Type
		NMAddByte(iSMsg, sti(NSClients[i].Ship.Upgrades.Cannons));	// Cannons Upgrade
		NMAddWord(iSMsg, sti(NSClients[i].Ship.Crew.Quantity));		// Crew quantity

		// sails parameters

		// skills
		NMAddByte(iSMsg, sti(NSClients[i].Skills.Accuracy));	// Skills: Accuracy
		NMAddByte(iSMsg, sti(NSClients[i].Skills.Cannons));		// Skills: Cannons
		NMAddByte(iSMsg, sti(NSClients[i].Skills.Sailing));		// Skills: Sailing
		NMAddByte(iSMsg, sti(NSClients[i].Skills.Defence));		// Skills: Defence
		NMAddByte(iSMsg, sti(NSClients[i].Skills.Repair));		// Skills: Repair
	
		//NMAddBytes(iSMsg, 0);									// Perks: 

		float fRadius = 60.0;
		if (sti(NetServer.GameType) == NETGAME_DEATHMATCH) { fRadius = 350 * (1.0 + stf(NetServer.StartPosition)); }

		if (sti(NetServer.GameType) == NETGAME_DEATHMATCH)
			{ NSClients[i].FriendlyTeam = -1; }
		else
			{ NSClients[i].FriendlyTeam = iTeam; }

		// select ship position
		float x = stf(oTeams[iTeam].Pos.x);
		float z = stf(oTeams[iTeam].Pos.z);
		float dx = makefloat(iTeams[iTeam] - (makeint(iTeams[iTeam] / 4) * 4)) * fRadius;
		float dz = makefloat(iTeams[iTeam] / 4) * fRadius;

		NSClients[i].Ship.Pos.x = x + dx;
		NSClients[i].Ship.Pos.z = z + dz;

		NSClients[i].LastCollisionShip = -500;
		NSClients[i].LastCollisionIsland = -500;

		NSClients[i].Ship.TurnLeft = 0.0;
		NSClients[i].Ship.TurnRight = 0.0;
		NSClients[i].Ship.TurnState = 0.0;
		NSClients[i].Ship.SailState = 0.0;

		// load ship for server
		ref rShip = Net_GetShipByIndex(sti(NSClients[i].Ship.Type));
		NSClients[i].Model = rShip.Name + "1";

		// when ship create, he trying to check - can he be placed there, or not, if not he must found new point to spawn
		CreateEntity(&NSClients[i], "NetShip");
		SendMessage(&NSClients[i], "la", MSG_SHIP_CREATE, rShip);

		LayerAddObject("net_execute", &NSClients[i], 31);
		LayerAddObject("ship_cannon_trace", &NSClients[i], 31);

		PostEvent("NetServer_ShipUpdateParameters", 1, "l", i);

		iTeams[iTeam] = iTeams[iTeam] + 1;
		
		// 
		NMAddFloat(iSMsg, stf(NSClients[i].Ship.Pos.x));		// pos.x
		NMAddFloat(iSMsg, stf(NSClients[i].Ship.Pos.y));		// pos.y
		NMAddFloat(iSMsg, stf(NSClients[i].Ship.Pos.z));		// pos.z
		NMAddFloat(iSMsg, stf(NSClients[i].Ship.Ang.y));		// ang.y

		// add goods
		for (int j=0; j<iNetGoodsNum; j++)
		{
			ref rGood = Net_GetGoodByIndex(j);
			string sGoodName = rGood.Name;
			NMAddWord(iSMsg, sti(NSClients[i].Ship.Goods.(sGoodName)));
		}

		// send message
		NetServer_SendMessage(DST_ALL, iSMsg, true);
		NMDelete(iSMsg);
	}

	// send some game info
	if (sti(NetServer.GameType) == NETGAME_CONVOY)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_GAMEINFO);
		NMAddByte(iSMsg, sti(NetServer.GameType));
		NMAddClientID(iSMsg, sti(NetServer.ConvoyShipID));
		NMAddFloat(iSMsg, stf(NetServer.ConvoyPoint.x));
		NMAddFloat(iSMsg, stf(NetServer.ConvoyPoint.z));
		NetServer_SendMessage(DST_ALL, iSMsg, true);
		NMDelete(iSMsg);
	}

	SetEventHandler("frame", "NetServer_OnCheckGameOverFrame", 1);
}

void NetServer_DownloadClientFile(int wClientID, string sFileName)
{
	if (!sti(NetServer.AllowDownload)) { return; }
	//if (NFExist(sFileName)) { return; }

	int iFileID1 = NFRCreateFile(wClientID, sFileName, -1, -1);

	Trace("NetServer try download client file: " + sFileName);

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_GET_FILE);
	NMAddDword(iSMsg, 0);
	NMAddDword(iSMsg, iFileID1);
	NMAddString(iSMsg, sFileName, 128);
	NetServer_SendMessage(wClientID, iSMsg, true);
	NMDelete(iSMsg);
}