
void NetClient_OnConnect(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	
	switch (iSubCode)
	{
		case NSC_CONNECT_ACCEPTED:
			NetClient_OnConnectAccepted(iMsg);
		break;
		case NSC_CONNECT_REJECTED:
			NetClient_OnConnectRejected(iMsg);
		break;
		case NSC_CONNECT_REJECTED_DIFFERENT_VERSION:
			NetClient_OnConnectRejected(iMsg);
		break;
		case NSC_CONNECT_REJECTED_SERVER_FULL:
			NetClient_OnConnectRejectedServerFull(iMsg);
		break;
		case NSC_CONNECT_REJECTED_NEED_CLIENT_PASSWORD:
			NetClient_OnConnectRejectedClientPassword(iMsg);
		break;
		case NSC_CONNECT_BANNED:
			NetClient_OnConnectBanned(iMsg);
		break;
		case NSC_CONNECT_NEED_PASSWORD:
			NetClient_OnConnectNeedPassword(iMsg);
		break;
		case NSC_CONNECT_RECONNECT:
			NetClient_OnConnectReconnect();
		break;
		case NSC_CONNECT_DISCONNECT:
			Event("NCEvent_Disconnect");
		break;
	}
}

void NetClient_OnConnectReconnect()
{
	Net_DeleteClient();
	//NetClient_TryConnect();
}

void NetClient_OnConnectAccepted(int iMsg)
{
	wClientID = NMGetClientID(iMsg);

	// create chat environment
	NetClient_CreateChatEnvironment();

	// start ping
	NetClient_Ping();

	Event("NCEvent_Client_Accepted");
	//LaunchNetBuyScreen();

	SetEventHandler("NCEvent_Disconnect", "NetClient_OnDisconnect", 0);
}

void NetClient_OnConnectRejected(int iMsg)
{
	Event("NCEvent_Client_Rejected", "s", "");	// FIX-ME
}

void NetClient_OnConnectRejectedServerFull(int iMsg)
{
	Event("NCEvent_Client_RejectedServerFull");
}

void NetClient_OnConnectRejectedClientPassword(int iMsg)
{
	Event("NCEvent_Client_RejectedClientPassword");
}

void NetClient_OnConnectBanned(int iMsg)
{
	string sReason = NMGetString(iMsg);
	Event("NCEvent_Client_Banned", "s", sReason);
}

void NetClient_OnConnectNeedPassword(int iMsg)
{
	Event("NCEvent_Client_RejectedServerPassword");
}

