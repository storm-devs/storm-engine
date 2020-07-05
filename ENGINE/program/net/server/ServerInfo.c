object TopList;

void NetServer_OnServerInfo(int wNetClientID, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	switch (iSubCode)
	{
		case NSC_SERVERINFO_GET:
			NetServer_GetFullServerInfo(wNetClientID);
		break;
		case NSC_SERVERINFO_TOPLIST_PLAYER:
			NetServer_OnServerInfo_SendTopListInfo(wNetClientID, iMsg);
		break;
	}
}

void NetServer_GetFullServerInfo(int wNetClientID)
{
	NetServer_OnServerInfo_SendCommonServerInfo(wNetClientID);
	NetServer_OnServerInfo_SendClientsInfo(wNetClientID);
	NetServer_OnServerInfo_SendShipsInfo(wNetClientID);
	NetServer_OnServerInfo_SendCannonsInfo(wNetClientID);
	NetServer_OnServerInfo_SendSkillsInfo(wNetClientID);
	NetServer_OnServerInfo_SendPerksInfo(wNetClientID);
	NetServer_OnServerInfo_SendGoodsInfo(wNetClientID);
}

// NSC_SERVERINFO_COMMON
void NetServer_OnServerInfo_SendCommonServerInfo(int wNetClientID)
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_COMMON);
	NMAddByte(iSMsg, NetServer_GetClientsNum());
	NMAddByte(iSMsg, sti(NetServer.MaxClients));
	NMAddByte(iSMsg, sti(NetServer.GameType));
	NMAddByte(iSMsg, sti(NetServer.NumTeams));
	NMAddByte(iSMsg, sti(NetServer.MaxShipClass));
	NMAddDword(iSMsg, sti(NetServer.Credit));
	NMAddByte(iSMsg, sti(NetServer.GameSpeed));
	NMAddString(iSMsg, NetServer.WeatherID, 32);
	NMAddString(iSMsg, NetServer.IslandID, 32);
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

// NSC_SERVERINFO_PLAYER 
void NetServer_OnServerInfo_SendClientsInfo(int wNetClientID)
{
	for (int i=0; i<NetServer_GetClientsNum(); i++)
	{
		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_SERVERINFO);
		NMAddByte(iSMsg, NSC_SERVERINFO_PLAYER);
		NMAddClientID(iSMsg, sti(NSClients[i].id));
		NMAddString(iSMsg, NSClients[i].NickName, 24);
		NMAddString(iSMsg, NSClients[i].Ship.Name, 24);
		NMAddString(iSMsg, NSClients[i].UserData.Face, 20);
		NMAddString(iSMsg, NSClients[i].UserData.Flag, 20);
		NMAddString(iSMsg, NSClients[i].UserData.Sail, 20);
		NMAddDword(iSMsg, sti(NSClients[i].Ship.Type));
		NMAddByte(iSMsg, sti(NSClients[i].Team));
		NetServer_SendMessage(wNetClientID, iSMsg, true);
		NMDelete(iSMsg);
	}
}

// NSC_SERVERINFO_SHIPS
void NetServer_OnServerInfo_SendShipsInfo(int wNetClientID)
{
	for (int i=0; i<Net_GetShipsNum(); i++)
	{
		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_SERVERINFO);
		NMAddByte(iSMsg, NSC_SERVERINFO_SHIP);
		NMAddWord(iSMsg, i);

		ref rShip = Net_GetShipByIndex(i);
		NMAddDword(iSMsg, sti(rShip.Cost));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Hull1));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Hull2));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Hull3));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Rigging1));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Rigging2));
		NMAddDword(iSMsg, sti(rShip.Upgrades.Rigging3));

		NMAddWord(iSMsg, sti(rShip.MaxCrew));
		NMAddWord(iSMsg, sti(rShip.MinCrew));
		NMAddFloat(iSMsg, stf(rShip.SpeedRate));
		NMAddFloat(iSMsg, stf(rShip.TurnRate));
		NMAddFloat(iSMsg, stf(rShip.HP));
		NMAddFloat(iSMsg, stf(rShip.SP));

		NetServer_SendMessage(wNetClientID, iSMsg, true);
		NMDelete(iSMsg);
	}
}

// NSC_SERVERINFO_CANNONS
void NetServer_OnServerInfo_SendCannonsInfo(int wNetClientID)
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_CANNONS);
	NMAddWord(iSMsg, Net_GetCannonsNum());
	for (int i=0; i<Net_GetCannonsNum(); i++)
	{
		ref rCannon = Net_GetCannonByIndex(i);
		NMAddWord(iSMsg, sti(rCannon.Cost));
		NMAddWord(iSMsg, sti(rCannon.Upgrade1));
		NMAddWord(iSMsg, sti(rCannon.Upgrade2));
		NMAddWord(iSMsg, sti(rCannon.Upgrade3));
	}
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

// NSC_SERVERINFO_SKILLS
void NetServer_OnServerInfo_SendSkillsInfo(int wNetClientID)
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_SKILLS);
	NMAddWord(iSMsg, Net_GetSkillsNum());
	for (int i=0; i<Net_GetSkillsNum(); i++)
	{
		ref rSkill = Net_GetSkillByIndex(i);
		NMAddDword(iSMsg, sti(rSkill.Cost));
	}
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

// NSC_SERVERINFO_PERKS
void NetServer_OnServerInfo_SendPerksInfo(int wNetClientID)
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_PERKS);
	NMAddWord(iSMsg, Net_GetPerksNum());
	for (int i=0; i<Net_GetPerksNum(); i++)
	{
		NMAddDword(iSMsg, Net_GetPerkCost(i));
	}
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

// NSC_SERVERINFO_GOODS
void NetServer_OnServerInfo_SendGoodsInfo(int wNetClientID)
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_GOODS);
	NMAddWord(iSMsg, Net_GetGoodsNum());
	for (int i=0; i<Net_GetGoodsNum(); i++)
	{
		ref rGood = Net_GetGoodByIndex(i);
		NMAddWord(iSMsg, sti(rGood.Units));
		NMAddDword(iSMsg, sti(rGood.Cost));
	}
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_OnServerInfo_SendTopListInfo(int wNetClientID, int iMsg)
{
	int iSMsg;
	int iGetIndex = NMGetWord(iMsg);

	aref arPlayers, arPlayer;
	makearef(arPlayers, TopList.Players);
	int iNumPlayers = GetAttributesNum(arPlayers);

	if (!iNumPlayers || iGetIndex >= iNumPlayers)
	{
		iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_SERVERINFO);
		NMAddByte(iSMsg, NSC_SERVERINFO_TOPLIST_PLAYER);
		NMAddWord(iSMsg, 0);	// players num in table
		NetServer_SendMessage(wNetClientID, iSMsg, true);
		NMDelete(iSMsg);
		return;
	}

	arPlayer = GetAttributeN(arPlayers, iGetIndex);
	string sNickName = GetAttributeName(arPlayer);

	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SERVERINFO);
	NMAddByte(iSMsg, NSC_SERVERINFO_TOPLIST_PLAYER);
	NMAddWord(iSMsg, iNumPlayers);
	NMAddWord(iSMsg, iGetIndex);
	NMAddString(iSMsg, sNickName, 24);
	NMAddString(iSMsg, arPlayer.sFaceImage, 20);
	NMAddByte(iSMsg, sti(arPlayer.Rank));
	NMAddDword(iSMsg, sti(arPlayer.Won));
	NMAddDword(iSMsg, sti(arPlayer.Lost));
	NMAddDword(iSMsg, sti(arPlayer.Shipwrecks));
	NMAddDword(iSMsg, sti(arPlayer.ShipsSunk));
	NMAddByte(iSMsg, MakeInt(stf(arPlayer.Accuracy) * 100.0));
	NMAddDword(iSMsg, sti(arPlayer.TotalDamage));
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}
