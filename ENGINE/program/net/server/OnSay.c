void NetServer_OnSay(int wNetClientID, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	string sSayString = NMGetString(iMsg);

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SAY);
	NMAddByte(iSMsg, iSubCode);
	NMAddClientID(iSMsg, wNetClientID);
	NMAddString(iSMsg, sSayString, 256);

	switch (iSubCode)
	{
		case NSC_SAY_CHAT:
			NetServer_SendMessage(DST_ALL, iSMsg, false);
			break;
		case NSC_SAY_PERSON:
			int wToClientID = NMGetClientID(iMsg);
			NetServer_SendMessage(wToClientID, iSMsg, false);
			break;
		case NSC_SAY_TEAM:
			ref rClient = NetServer_GetClient(wNetClientID);
			int iTeam = sti(rClient.Team);
			// find team clients and send say message
			for (int i=0; i<NET_MAXCLIENTS; i++)
			{
				if (!sti(NSClients[i].Use) || sti(NSClients[i].Team) != iTeam) { continue; }

				NetServer_SendMessage(sti(NSClients[i].id), iSMsg, false);
			}
			break;
	}

	NMDelete(iSMsg);
}