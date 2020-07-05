// ===================================
// Net native function
// ===================================
native void Net_SaveFile(bool bServer, ref rObject, string sFileName);
native void Net_LoadFile(bool bServer, ref rObject, string sFileName);
native int Net_GetTickCount();
native void Net_Sort2i(ref rInMassive, ref rOutMassive, int iDirection, int iMaxOutNum, string sAttribute1, string sAttribute2);

// ===================================
// Net INI native function
// ===================================
native int NetIni_GetLong(string sIniFile, string sSection, string sValue);
native string NetIni_GetString(string sIniFile, string sSection, string sValue);

// ===================================
// NetServer/NetClient native function
// ===================================
native void NetServer_AddClient(int wClientID, int dwIP, int wPort);
native void NetServer_DelClient(int wClientID);
native void NetServer_SendMessage(int wClientID, int iMsg, bool bGarantedDelivery);
native void NetServer_SendMessageAllExcept(int wClientID, int iMsg, bool bGarantedDelivery);
native void NetServer_SendMessageDirect(int dwIP, int wPort, int iMsg);
native void NetServer_SendMasterServerMessage(string sAddress, int wPort, int iMsg);
native void NetServer_GarantedDeliveryCallback(int wClientID, int byGarantedDeliveryIndex);
native void NetServer_ClearPacketsQueue();

native void NetClient_SendMessage(int iMsg, bool bGarantedDelivery);					// put packet to queue for current game server
native void NetClient_SendMessageDirect(int iMsg);										// immediately send to current game server
native void NetClient_SendMessageDirectToIP(int dwIP, int wPort, int iMsg);				// immediately send to IP:port
native void NetClient_SendBroadcastMessage(int iMsg);									// send broadcast message
native void NetClient_SendMasterServerMessage(string sAddress, int wPort, int iMsg);	// send to current master server
native void NetClient_GarantedDeliveryCallback(int byGarantedDeliveryIndex);
native void NetClient_ClearPacketsQueue();

// ===================================
// NetMessages native function
// ===================================
native void NMAddByte(int iSMsg, int bByte);
native void NMAddWord(int iSMsg, int wWord);
native void NMAddDword(int iSMsg, int dDword);
native void NMAdd24(int iSMsg, int dDword);
native void NMAddFloat(int iSMsg, float fFloat);
native void NMAddString(int iSMsg, string sString, int iMaxLen);
native void NMAddData(int iSMsg, int pData, int iSize);

native int NMCreate();
native void NMDelete(int iMsg);

native int NMNotEmpty(int iMsg);

native int NMGetByte(int iMsg);
native int NMGetWord(int iMsg);
native int NMGetDword(int iMsg);
native int NMGet24(int iMsg);
native float NMGetFloat(int iMsg);
native string NMGetString(int iMsg);

native string inet_ntoa(int dwIP);
native string inet_ntoax(int dwIP);		// return IP string with dots as 'x': 127.0.0.1 - return 127x0x0x1
native int ntohs(int wPort);

native int NFFindFiles(ref rObject, string sDirectory, string sMask, bool bRecursive);

// ===================================
// NetFileSend/NetFileRecv native function
// ===================================
native int NFExist(string sFileName);

native int NFRCreateFile(int wClientID, string sFileName, int iEventID, int iEventSubID);
native void NFRReceiveData(int iFileID, int iIndex, int iMsg);
native void NFRDeleteFile(int iFileID);
native int NFRIsEqualFile(int iFileID, int iCRC32, int iSize);
native void NFRSetSendHandle(int iFileID1, int iFileID2);

native int NFSCreateFile(int wClientID, int iRecvHandler, string sFileName);
native void NFSDeleteFile(int iFileID);
native void NFSReceiverReady(int iFileID);
native void NFSDataACK(int iFileID, int iIndex);

// ===================================
// Load/Delete Texture
// ===================================
native int NFGetTexture(string sGroupID, string sFilename);
native void NFDelTexture(string sGroupID, string sFilename);
native void NFDelGroupTextures(string sGroupID);

#libriary "NET_SCRIPT_LIBRIARY"
