int iClientLastPingCode;
int iClientLastPingTime;
int iClientLastPingCodeTime;

void NetClient_PingInit()
{
	iClientLastPingCode = 0;
	iClientLastPingTime = 0;
	iClientLastPingCodeTime = 0;
}

void NetClient_OnPing(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int iPingCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_PING: 
			int iSMsg = NMCreate();
			NMAddClientID(iSMsg, wClientID);
			NMAddByte(iSMsg, NC_PING);
			NMAddByte(iSMsg, NSC_PONG);
			NMAddByte(iSMsg, iPingCode);
			NetClient_SendMessage(iSMsg, false);
			NMDelete(iSMsg);
		break;
		case NSC_PONG: 
			NetClient_Pong(iMsg, iPingCode);
		break;
	}
}

void NetClient_Ping()
{
	PostEvent("NetClient_Ping", 1000);

	iClientLastPingCode = iClientLastPingCode + 1;
	if (iClientLastPingCode > 255) { iClientLastPingCode = 0; }
	iClientLastPingCodeTime = Net_GetTickCount();

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_PING);
	NMAddByte(iSMsg, NSC_PING);
	NMAddByte(iSMsg, iClientLastPingCode);
	NetClient_SendMessage(iSMsg, false);
	NMDelete(iSMsg);
}

void NetClient_Pong(int iMsg, int iPingCode)
{
	iClientServerTime = NMGetDword(iMsg);

	if (iPingCode == iClientLastPingCode)
	{
		// calculate 
		iClientLastPingTime = (Net_GetTickCount() - iClientLastPingCodeTime) / 2;
		if (iClientServerTime > 0)
		{
			iClientServerTime = iClientServerTime - iClientLastPingTime;
		}

		//Trace("Ping = " + iClientLastPingTime + ", delta = " + (iClientTime - iClientServerTime));
	}
	else
	{
		Trace("iPingCode != iClientLastPingCode");
	}
}