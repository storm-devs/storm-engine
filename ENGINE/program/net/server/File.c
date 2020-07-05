
void NetServer_OnFile(int wNetClientID, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	int iFileID1 = NMGetDword(iMsg);
	int iFileID2 = NMGetDword(iMsg);
	switch (iSubCode)
	{
		case NSC_FILE_DATA:				NetServer_OnFileData(wNetClientID, iFileID1, iFileID2, iMsg);			break;
		case NSC_FILE_DATA_ACK:			NetServer_OnFileDataACK(wNetClientID, iFileID1, iFileID2, iMsg);		break;
		case NSC_FILE_GET_FILE:			NetServer_OnFileGetFile(wNetClientID, iFileID1, iFileID2, iMsg);		break;
		case NSC_FILE_TRANSFER_READY:	NetServer_OnFileTransferReady(wNetClientID, iFileID1, iFileID2, iMsg);	break;
		case NSC_FILE_RECEIVER_READY:	NetServer_OnFileReceiverReady(wNetClientID, iFileID1, iFileID2, iMsg);	break;
		case NSC_FILE_CANCEL:			NetServer_OnFileCancel(wNetClientID, iFileID1, iFileID2, iMsg);		break;
	}
}

void NetServer_OnFileData(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	int iIndex = NMGetDword(iMsg);
	
	NFRReceiveData(iFileID1, iIndex, iMsg);

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_DATA_ACK);
	NMAddDword(iSMsg, iFileID2);
	NMAddDword(iSMsg, iFileID1);
	NMAddDword(iSMsg, iIndex);
	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_OnFileDataACK(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	int iIndex = NMGetDword(iMsg);
	
	NFSDataACK(iFileID1, iIndex);
}

void NetServer_OnFileGetFile(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	string sFileName = NMGetString(iMsg);

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_FILE);
	if (!sti(NetServer.AllowUpload))
	{
		NMAddByte(iSMsg, NSC_FILE_CANCEL);
		NMAddDword(iSMsg, iFileID2);
		NMAddDword(iSMsg, iFileID1);
		NetServer_SendMessage(wNetClientID, iSMsg, true);
	}
	else
	{
		// Create Send File 
		iFileID1 = NFSCreateFile(wNetClientID, iFileID2, sFileName);
	}
	NMDelete(iSMsg);
}

void NetServer_OnFileTransferReady(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	int iCRC32 = NMGetDword(iMsg);
	int iFileSize = NMGetDword(iMsg);

	int iSMsg = NMCreate();
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

	NetServer_SendMessage(wNetClientID, iSMsg, true);
	NMDelete(iSMsg);
}

void NetServer_OnFileReceiverReady(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	NFSReceiverReady(iFileID1);
}

void NetServer_OnFileCancel(int wNetClientID, int iFileID1, int iFileID2, int iMsg)
{
	NFSDeleteFile(iFileID1);
}