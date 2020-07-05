int rgbGameMessage = 14610157;

void NetClient_DeleteChatEnvironment()
{
	DeleteClass(&NCSay);

	DelEventHandler("Control Activation", "NetClient_ChatControlActivation");
}

void NetClient_CreateChatEnvironment()
{
	CreateEntity(&NCSay, "NetSay");
	LayerAddObject("net_execute", &NCSay, -1);
	LayerAddObject("net_realize", &NCSay, 65535);

	NCSay.FontScale = 1.0;
	NCSay.StartY = 0.93;
	NCSay.ChatLines = 11;

	SetEventHandler("Control Activation", "NetClient_ChatControlActivation", 0);
}

void NetClient_OnSay(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int wNetClientID = NMGetClientID(iMsg);
	string sMessage = NMGetString(iMsg);

	ref rClient = NetClient_GetClient(wNetClientID);

	NCSay.SenderName = rClient.NickName;
	NCSay.Time = 30.0;

	switch (iSubCode)
	{
		case NSC_SAY_CHAT:
			NCSay.Color = ARGB(0, 251, 251, 251);
			NCSay.ChatMessage = sMessage;
		break;
		case NSC_SAY_PERSON:
			NCSay.Color = ARGB(0, 234, 255, 0);
			NCSay.PersonMessage = sMessage;
		break;
		case NSC_SAY_TEAM:
			NCSay.Color = ARGB(0, 121, 122, 255);
			NCSay.TeamMessage = sMessage;
		break;
	}
}

void NetClient_ChatAddGameMessage(string sMessage, int dwColor, int iMode, int wID1, int wID2)
{
	object oRes;
	string sParseString = LanguageConvertString(iLangNetClient, sMessage);

	oRes.Str = "";

	ref rClient1 = NetClient_GetClient(wID1);
	string sNick1 = rClient1.NickName;
	string sShip1 = rClient1.Ship.Name;

	if (iMode == 1)
	{
		Event(PARSE_STRING, "aslss", &oRes, sParseString, 2, sNick1, sShip1);
	}
	else
	{
		ref rClient2 = NetClient_GetClient(wID2);
		string sNick2 = rClient2.NickName;
		string sShip2 = rClient2.Ship.Name;

		Event(PARSE_STRING, "aslssss", &oRes, sParseString, 4, sNick1, sShip1, sNick2, sShip2);
	}

	NCSay.Color = dwColor;
	NCSay.Time = 30.0;
	NCSay.GameMessage = oRes.Str;
}

void NetClient_ChatControlActivation()
{
	string sControl = GetEventData();

	int iSMsg;
	switch (sControl)
	{
		/*case "Net_RestartGame":
			iSMsg = NMCreate();
			NMAddClientID(iSMsg, wClientID);
			NMAddByte(iSMsg, NC_COMMAND);
			NMAddByte(iSMsg, NSC_COMMAND_RESTARTGAME);
			NetClient_SendMessage(iSMsg, true);
			NMDelete(iSMsg);
		break;
		case "Net_StartGame":
			iSMsg = NMCreate();
			NMAddClientID(iSMsg, wClientID);
			NMAddByte(iSMsg, NC_COMMAND);
			NMAddByte(iSMsg, NSC_COMMAND_STARTGAME);
			NetClient_SendMessage(iSMsg, true);
			NMDelete(iSMsg);
		break;*/
		case "Say":
			NCSay.Mode = "Say";
			LaunchNetChatString();
		break;
		case "Team_Say":
			if (sti(NCServer.GameType) != NETGAME_DEATHMATCH)
			{
				NCSay.Mode = "Team_Say";
				LaunchNetChatString();
			}
		break;
		/*case "Person_Say":
			NCSay.Mode = sControl;
			iSMsg = NMCreate();
			NMAddClientID(iSMsg, wClientID);
			NMAddByte(iSMsg, NC_SAY);
			NMAddByte(iSMsg, NSC_SAY_PERSON);
			NMAddString(iSMsg, "Person Say", 128);
			NMAddByte(iSMsg, wClientID);
			NetClient_SendMessage(iSMsg, false);
			NMDelete(iSMsg);
		break;*/
	}
}
