void NetFileSend_SendData()
{
	int wNetClientID = GetEventData();
	int iFileID1 = GetEventData();
	int iFileID2 = GetEventData();
	int iIndex = GetEventData();
	int pData = GetEventData();
	int iDataSize = GetEventData();

	int iSMsg = NMCreate();

	if (wNetClientID < 0)	// send 2 server
	{
		NMAddClientID(iSMsg, wClientID);
	}

	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_DATA);
	NMAddDword(iSMsg, iFileID2);
	NMAddDword(iSMsg, iFileID1);
	NMAddDword(iSMsg, iIndex);
	NMAddData(iSMsg, pData, iDataSize);

	if (wNetClientID < 0)	{ NetClient_SendMessage(iSMsg, true); }
					else	{ NetServer_SendMessage(wNetClientID, iSMsg, true); }

	NMDelete(iSMsg);
}

void NetFileSend_TransferReady()
{
	int wNetClientID = GetEventData();
	int iFileID1 = GetEventData();
	int iFileID2 = GetEventData();
	int iCRC32 = GetEventData();
	int iFileSize = GetEventData();

	int iSMsg = NMCreate();
	if (wNetClientID < 0)	{ NMAddClientID(iSMsg, wClientID); }

	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_TRANSFER_READY);
	NMAddDword(iSMsg, iFileID2);
	NMAddDword(iSMsg, iFileID1);
	NMAddDword(iSMsg, iCRC32);
	NMAddDword(iSMsg, iFileSize);

	if (wNetClientID < 0)	{ NetClient_SendMessage(iSMsg, true); }
					else	{ NetServer_SendMessage(wNetClientID, iSMsg, true); }

	NMDelete(iSMsg);
}

void NetFileRecv_Done()
{
	int wNetClientID = GetEventData();
	int iEventID = GetEventData();
	int iEventSubID = GetEventData();

	if (wNetClientID < 0)	// client recv
	{
		NetClient_OnFileDone(iEventID, iEventSubID);
	}
}
