void NetClient_OnPlayerInfo(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	ref rClient;
	int wNetClientID;

	switch (iSubCode)
	{
		case NSC_PLAYERINFO_SHIPCHANGE:
			wNetClientID = NMGetClientID(iMsg);
			rClient = NetClient_GetClient(wNetClientID);
			rClient.Ship.Type = NMGetWord(iMsg);
			Event("NCEvent_PlayerInfo_ShipType", "ll", wNetClientID, sti(rClient.Ship.Type));
		break;
		case NSC_PLAYERINFO_BUYREADY:
			wNetClientID = NMGetClientID(iMsg);
			rClient = NetClient_GetClient(wNetClientID);
			rClient.BuyReady = NMGetByte(iMsg);
			Event("NCEvent_PlayerInfo_BuyReady", "l", wNetClientID);
		break;
		case NSC_PLAYERINFO_STARTREADY:
			wNetClientID = NMGetClientID(iMsg);
			rClient = NetClient_GetClient(wNetClientID);
			rClient.StartReady = NMGetByte(iMsg);
			Event("NCEvent_PlayerInfo_StartReady", "l", wNetClientID);
		break;
		case NSC_PLAYERINFO_TEAMCHANGE:
			wNetClientID = NMGetClientID(iMsg);
			rClient = NetClient_GetClient(wNetClientID);
			rClient.Team = NMGetByte(iMsg);
			Event("NCEvent_PlayerInfo_Team", "ll", wNetClientID, sti(rClient.Team));
		break;
		case NSC_PLAYERINFO_STARTCOUNTDOWN:
			int iClientStartTimer = NMGetDword(iMsg);
			Event("NCEvent_PlayerInfo_StartCountdown", "l", iClientStartTimer);
		break;
		case NSC_PLAYERINFO_STOPCOUNTDOWN:
			Event("NCEvent_PlayerInfo_StopCountdown");
		break;
	}
}
