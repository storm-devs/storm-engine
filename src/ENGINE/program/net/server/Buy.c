int iSecondsToStartGame = -1;

void NetServer_OnBuy(int wNetClientID, int iMsg)
{
	int iSMsg;

	ref rClient = NetServer_GetCLient(wNetClientID);

	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_BUY_SHIPCHANGE:
			rClient.Ship.Type = NMGetWord(iMsg);
			iSMsg = NMCreate();
			NMAddByte(iSMsg, NC_PLAYERINFO);
			NMAddByte(iSMsg, NSC_PLAYERINFO_SHIPCHANGE);
			NMAddClientID(iSMsg, wNetClientID);
			NMAddWord(iSMsg, sti(rClient.Ship.Type));
			NetServer_SendMessage(DST_ALL, iSMsg, true);
			NMDelete(iSMsg);
			return;
		break;
		case NSC_BUY_TEAMCHANGE:
			NetServer_OnBuy_TeamChangeRequest(wNetClientID, iMsg);
			return;
		break;
		case NSC_BUY_READY:
			NetServer_OnBuy_Ready(wNetClientID);
			return;
		break;
		case NSC_BUY_FULLINFO:
			NetServer_OnBuy_FullInfo(wNetClientID, iMsg);
			return;
		break;
	}
}	

void NetServer_OnBuy_TeamChangeRequest(int wNetClientID, int iMsg)
{
	ref rClient = NetServer_GetCLient(wNetClientID);
	int iNewTeam = NMGetByte(iMsg);

	if (iSecondsToStartGame >= 0) { return; }

	rClient.Team = iNewTeam;
	rClient.BuyReady = false;

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_PLAYERINFO);
	NMAddByte(iSMsg, NSC_PLAYERINFO_TEAMCHANGE);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddByte(iSMsg, sti(rClient.Team));
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_PLAYERINFO);
	NMAddByte(iSMsg, NSC_PLAYERINFO_BUYREADY);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddByte(iSMsg, sti(rClient.BuyReady));
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_OnBuy_Ready(int wNetClientID)
{
	int i;

	// countdown already started
	if (iSecondsToStartGame >= 0) { return; }

	// if client alone on server - dont start countdown
	if ((!NET_DEBUG) && NetServer_GetNumClients() == 1) { return; }

	ref rClient = NetServer_GetCLient(wNetClientID);
	rClient.BuyReady = true;
	
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_PLAYERINFO);
	NMAddByte(iSMsg, NSC_PLAYERINFO_BUYREADY);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddByte(iSMsg, sti(rClient.BuyReady));
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	for (i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }
		if (!sti(NSClients[i].BuyReady)) { return; }
	}

	// if team game and only one team here - dont start countdown
	if ((!NET_DEBUG) && sti(NetServer.GameType) != NETGAME_DEATHMATCH && NetServer_GetNumTeams() <= 1) { return; }

	if (NET_DEBUG) 
		{ iSecondsToStartGame = 3; }
	else
		{ iSecondsToStartGame = 10; }

	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_PLAYERINFO);
	NMAddByte(iSMsg, NSC_PLAYERINFO_STARTCOUNTDOWN);
	NMAddDword(iSMsg, iSecondsToStartGame);
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	iSecondsToStartGame = iSecondsToStartGame * 1000;

	SetEventHandler("NetServer_StartServerCountDown", "NetServer_StartServerCountDown", 0);
	PostEvent("NetServer_StartServerCountDown", 250);
}

void NetServer_StartServerCountDown()
{
	iSecondsToStartGame = iSecondsToStartGame - 250;

	// check players/teams numbers (if only 1 player or 1 team, or zero players - stop countdown and return to wait state)
	bool bStopCountdown = false;
	if (!NET_DEBUG)
	{
		if (NetServer_GetNumClients() <= 1) { bStopCountdown = true; }
		if (sti(NetServer.GameType) != NETGAME_DEATHMATCH && NetServer_GetNumTeams() <= 1) { bStopCountdown = true; }
	}
	if (bStopCountdown)
	{
		iSecondsToStartGame = -1;

		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_PLAYERINFO);
		NMAddByte(iSMsg, NSC_PLAYERINFO_STOPCOUNTDOWN);
		NetServer_SendMessage(DST_ALL, iSMsg, true);
		NMDelete(iSMsg);

		return;
	}

	if (iSecondsToStartGame <= 0)
	{
		NetServer_StartGame();
		return;
	}

	PostEvent("NetServer_StartServerCountDown", 250);
}

void NetServer_OnBuy_FullInfo(int wNetClientID, int iMsg)
{
	int i;
	int iCredit = 0;

	ref rClient = NetServer_GetCLient(wNetClientID);

	// receive and calculates all 
	int iShipType = NMGetWord(iMsg);
	int iShipUpgradeHull = NMGetByte(iMsg);
	int iShipUpgradeRigging = NMGetByte(iMsg);

	int iCannonsType = NMGetByte(iMsg);
	int iCannonsUpgrade = NMGetByte(iMsg);

	int iSkills[5], iPerks[6], iGoods[6];

	// add skills money
	for (i=0; i<5; i++) 
	{ 
		iSkills[i] = NMGetByte(iMsg); 
		iCredit = iCredit + Net_GetSkillCost(i, iSkills[i]);
	}

	// add perks money
	for (i=0; i<6; i++) 
	{ 
		iPerks[i] = NMGetByte(iMsg); 
		if (iPerks[i]) { iCredit = iCredit + Net_GetPerkCost(i); }
	}

	// add goods money
	for (i=0; i<6; i++) 
	{ 
		iGoods[i] = NMGetWord(iMsg); 
		iCredit = iCredit + Net_GetGoodCost(i, iGoods[i]);
		
		ref rGood = Net_GetGoodByIndex(i);
		string sGood = rGood.Name;
		rClient.Ship.Goods.(sGood) = iGoods[i];
	}

	ref rShip = Net_GetShipByIndex(iShipType);

	float fShipMaxHP = stf(rShip.HP) * (1.0 + stf(NetShipHullUpgrades[iShipUpgradeHull - 1].hp) / 100.0);

	rClient.Ship.Type = iShipType;
	rClient.Ship.HP = fShipMaxHP;
	rClient.Ship.MaxHP = fShipMaxHP;
	rClient.Ship.SP = 100.0;
	rClient.Ship.Upgrades.Hull = iShipUpgradeHull;
	rClient.Ship.Upgrades.Sails = iShipUpgradeRigging;
	rClient.Ship.Cannons.Borts = "";
	rClient.Ship.Cannons.Type = iCannonsType;
	rClient.Ship.Upgrades.Cannons = iCannonsUpgrade;
	rClient.Ship.Cannons.Charge.Type = 0;				// balls // but check first available
	rClient.Ship.Crew.Quantity = sti(rShip.MaxCrew);
	rClient.Skills.Accuracy = iSkills[0];
	rClient.Skills.Cannons = iSkills[1];
	rClient.Skills.Sailing = iSkills[2];
	rClient.Skills.Defence = iSkills[3];
	rClient.Skills.Repair = iSkills[4];
	rClient.Skills.Repair.Active = 0;
	rClient.Skills.Repair.Cooldown = iServerTime - 60 * 1000;		// FIX-ME

	rClient.Ship.SpeedRateModifier = 1.0 + (stf(NetShipHullUpgrades[iShipUpgradeHull - 1].speedrate) + stf(NetShipSailUpgrade[iShipUpgradeRigging - 1].speedrate)) / 100.0;
	rClient.Ship.TurnRateModifier = 1.0 + (stf(NetShipHullUpgrades[iShipUpgradeHull - 1].turnrate) + stf(NetShipSailUpgrade[iShipUpgradeRigging - 1].turnrate)) / 100.0;
	rClient.Ship.SailsDamageModifier = 1.0 + stf(NetShipSailUpgrade[iShipUpgradeRigging - 1].dmgtosails) / 100.0;

	NetServer_CannonRecalculateParameters(rClient);
	NetServer_CalculateFireTime(rClient);
	NetServer_CalculateReloadTime(rClient);

	rClient.StartReady = true;

	// check for all clients ready
	bool bAllClientsReady = true;
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{ 
		if (sti(NSClients[i].Use) && !sti(NSClients[i].StartReady))
		{
			bAllClientsReady = false;
			break;
		}
	}

	// send message to all 
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_PLAYERINFO);
	NMAddByte(iSMsg, NSC_PLAYERINFO_STARTREADY);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddByte(iSMsg, sti(rClient.StartReady));
	NetServer_SendMessage(DST_ALL, iSMsg, true);
	NMDelete(iSMsg);

	if (bAllClientsReady)
	{
		NetServer_StartServerGame();
		return;
	}
}
