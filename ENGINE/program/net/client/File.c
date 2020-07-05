void NetClient_OnFile(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int iFileID1 = NMGetDword(iMsg);
	int iFileID2 = NMGetDword(iMsg);

	switch (iSubCode)
	{
		case NSC_FILE_DATA:				NetClient_OnFileData(iFileID1, iFileID2, iMsg);				break;
		case NSC_FILE_DATA_ACK:			NetClient_OnFileDataACK(iFileID1, iFileID2, iMsg);			break;
		case NSC_FILE_GET_FILE:			NetClient_OnFileGetFile(iFileID1, iFileID2, iMsg);			break;
		case NSC_FILE_TRANSFER_READY:	NetClient_OnFileTransferReady(iFileID1, iFileID2, iMsg);	break;
		case NSC_FILE_RECEIVER_READY:	NetClient_OnFileReceiverReady(iFileID1, iFileID2, iMsg);	break;
		case NSC_FILE_CANCEL:			NetClient_OnFileCancel(iFileID1, iFileID2, iMsg);			break;
	}
}

void NetClient_OnFileData(int iFileID1, int iFileID2, int iMsg)
{
	int iIndex = NMGetDword(iMsg);
	NFRReceiveData(iFileID1, iIndex, iMsg);

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_DATA_ACK);
	NMAddDword(iSMsg, iFileID2);
	NMAddDword(iSMsg, iFileID1);
	NMAddDword(iSMsg, iIndex);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}

void NetClient_OnFileDataACK(int iFileID1, int iFileID2, int iMsg)
{
	int iIndex = NMGetDword(iMsg);
	NFSDataACK(iFileID1, iIndex);
}

void NetClient_OnFileGetFile(int iFileID1, int iFileID2, int iMsg)
{
	string sFileName = NMGetString(iMsg);
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_FILE);
	if (!sti(NetClient.AllowUpload))
	{
		NMAddByte(iSMsg, NSC_FILE_CANCEL);
		NMAddDword(iSMsg, iFileID2);
		NMAddDword(iSMsg, iFileID1);
		NetClient_SendMessage(iSMsg, true);
	}
	else
	{
		// Create Send File 
		iFileID1 = NFSCreateFile(-1, iFileID2, sFileName);
	}
	NMDelete(iSMsg);
}

void NetClient_OnFileTransferReady(int iFileID1, int iFileID2, int iMsg)
{
	int iCRC32 = NMGetDword(iMsg);
	int iFileSize = NMGetDword(iMsg);

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_FILE);

	bool bFileEqual = NFRIsEqualFile(iFileID1, iCRC32, iFileSize);
	if (bFileEqual)
	{
		NMAddByte(iSMsg, NSC_FILE_CANCEL);
		NMAddDword(iSMsg, iFileID2);
		NMAddDword(iSMsg, iFileID1);
		NFRDeleteFile(iFileID1);
	}
	else
	{
		NMAddByte(iSMsg, NSC_FILE_RECEIVER_READY);
		NMAddDword(iSMsg, iFileID2);
		NMAddDword(iSMsg, iFileID1);
		NFRSetSendHandle(iFileID1, iFileID2, iFileSize);
	}

	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}

void NetClient_OnFileReceiverReady(int iFileID1, int iFileID2, int iMsg)
{
	NFSReceiverReady(iFileID1);
}

void NetClient_OnFileCancel(int iFileID1, int iFileID2, int iMsg)
{
	NFSDeleteFile(iFileID1);
}

void NetClient_OnFileDone(int iEventID, int iEventSubID)
{
}
