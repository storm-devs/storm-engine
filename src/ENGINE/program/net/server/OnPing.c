void NetServer_OnPing(int wNetClientID, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int iPingCode = NMGetByte(iMsg);
	
	switch (iSubCode)
	{
		case NSC_PING: 
			int iSMsg = NMCreate();
			NMAddByte(iSMsg, NC_PING);
			NMAddByte(iSMsg, NSC_PONG);
			NMAddByte(iSMsg, iPingCode);
			NMAddDword(iSMsg, iServerTime);
			NetServer_SendMessage(wNetClientID, iSMsg, false);
			NMDelete(iSMsg);

			ref rClient = NetServer_GetClient(wNetClientID);
			rClient.LastPingTime = iPingTime;
			//Trace("Server: got ping code = " + iPingCode + ", client = " + wNetClientID);
		break;
		case NSC_PONG:	
			Trace("Server: got pong");
		break;
	}
}
