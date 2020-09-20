void NetClient_OnServerInfo(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_SERVERINFO_COMMON:				NetClient_OnServerInfo_Common(iMsg);			break;
		case NSC_SERVERINFO_PLAYER:				NetClient_OnServerInfo_Player(iMsg);			break;
		case NSC_SERVERINFO_SHIP:				NetClient_OnServerInfo_Ship(iMsg);				break;
		case NSC_SERVERINFO_CANNONS:			NetClient_OnServerInfo_Cannons(iMsg);			break;
		case NSC_SERVERINFO_SKILLS:				NetClient_OnServerInfo_Skills(iMsg);			break;
		case NSC_SERVERINFO_PERKS:				NetClient_OnServerInfo_Perks(iMsg);				break;
		case NSC_SERVERINFO_GOODS:				NetClient_OnServerInfo_Goods(iMsg);				break;
		case NSC_SERVERINFO_TOPLIST_PLAYER:		NetClient_OnServerInfo_TopListPlayer(iMsg);		break;
	}

	Event("NetClient_OnServerInfo_Event", "l", iSubCode);
}

void NetClient_OnServerInfo_Common(int iMsg)
{
	NCServer.NumClients = NMGetByte(iMsg);
	NCServer.MaxClients = NMGetByte(iMsg);
	NCServer.GameType = NMGetByte(iMsg);
	NCServer.NumTeams = NMGetByte(iMsg);
	NCServer.MaxShipClass = NMGetByte(iMsg);
	NCServer.Credit = NMGetDword(iMsg);
	NCServer.GameSpeed = NMGetByte(iMsg);
	NCServer.WeatherID = NMGetString(iMsg);
	NCServer.IslandID = NMGetString(iMsg);
}

void NetClient_OnServerInfo_Player(int iMsg)
{
	int wSrvClientID = NMGetClientID(iMsg);
	ref rClient = NetClient_GetClient(wSrvClientID);
	rClient.NickName = NMGetString(iMsg);
	rClient.Ship.Name = NMGetString(iMsg);
	rClient.UserData.Face = NMGetString(iMsg);
	rClient.UserData.Flag = NMGetString(iMsg);
	rClient.UserData.Sail = NMGetString(iMsg);
	rClient.Ship.Type = NMGetDword(iMsg);
	rClient.Team = NMGetByte(iMsg);
}

void NetClient_OnServerInfo_Ship(int iMsg)
{
	int i = NMGetWord(iMsg);
	
	ref rShip = Net_GetShipByIndex(i);

	rShip.Cost = NMGetDword(iMsg);
	rShip.Upgrades.Hull1 = NMGetDword(iMsg);
	rShip.Upgrades.Hull2 = NMGetDword(iMsg);
	rShip.Upgrades.Hull3 = NMGetDword(iMsg);
	rShip.Upgrades.Rigging1 = NMGetDword(iMsg);
	rShip.Upgrades.Rigging2 = NMGetDword(iMsg);
	rShip.Upgrades.Rigging3 = NMGetDword(iMsg);

	rShip.MaxCrew = NMGetWord(iMsg);
	rShip.MinCrew = NMGetWord(iMsg);
	rShip.SpeedRate = NMGetFloat(iMsg);
	rShip.TurnRate = NMGetFloat(iMsg);
	rShip.HP = NMGetFloat(iMsg);
	rShip.SP = NMGetFloat(iMsg);
}

void NetClient_OnServerInfo_Cannons(int iMsg)
{
	int iNumCannons = NMGetWord(iMsg);

	for (int i=0; i<iNumCannons; i++)
	{
		ref rCannon = Net_GetCannonByIndex(i);
		rCannon.Cost = NMGetWord(iMsg);
		rCannon.Upgrade1 = NMGetWord(iMsg);
		rCannon.Upgrade2 = NMGetWord(iMsg);
		rCannon.Upgrade3 = NMGetWord(iMsg);
	}
}

void NetClient_OnServerInfo_Skills(int iMsg)
{
	int iNumSkills = NMGetWord(iMsg);

	for (int i=0; i<iNumSkills; i++)
	{
		ref rSkill = Net_GetSkillByIndex(i);
		rSkill.Cost = NMGetDword(iMsg);
	}
}

void NetClient_OnServerInfo_Perks(int iMsg)
{
	int iNumPerks = NMGetWord(iMsg);

	for (int i=0; i<iNumPerks; i++)
	{
		ref rPerk = Net_GetPerkByIndex(i);
		rPerk.Cost = NMGetDword(iMsg);
	}
}

void NetClient_OnServerInfo_Goods(int iMsg)
{
	int iNumGoods = NMGetWord(iMsg);

	for (int i=0; i<iNumGoods; i++)
	{
		ref rGood = Net_GetGoodByIndex(i);
		rGood.Unit = NMGetWord(iMsg);
		rGood.Cost = NMGetDword(iMsg);
	}
}

void NetClient_OnServerInfo_TopListPlayer(int iMsg)
{
	int iNumPlayers = NMGetWord(iMsg);
	
	if (!iNumPlayers)
	{
		Event("NetClient_OnServerInfo_TopListEnd_Event");
		return;
	}

	Event("NetClient_OnServerInfo_TopListPlayer_Event");
}