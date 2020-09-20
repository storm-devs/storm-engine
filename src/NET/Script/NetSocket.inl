// NetClient

dword _cdecl NetClient_ClearPacketsQueue(VS_STACK * pS)
{
	NetClient::pClient->ClearPacketsQueue();
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_GarantedDeliveryCallback(VS_STACK * pS)
{
	word wGarantedIndex = word(((VDATA*)pS->Pop())->GetLong());
	NetClient::pClient->GarantedDeliveryCallback(wGarantedIndex);
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_SendMessageDirect(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	NetClient::pClient->SendMessageDirect(*pSend);
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_SendMessageDirectToIP(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wPort = word(((VDATA*)pS->Pop())->GetLong());
	dword dwIP = dword(((VDATA*)pS->Pop())->GetLong());
	NetClient::pClient->SendMessageDirectToIP(dwIP, wPort, *pSend);
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_SendBroadcastMessage(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	NetClient::pClient->SendBroadcast(*pSend);
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_SendMessage(VS_STACK * pS)
{
	bool bGarantedDelivery = ((VDATA*)pS->Pop())->GetLong() != 0;
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	NetClient::pClient->AddPacket(*pSend, bGarantedDelivery);
	return IFUNCRESULT_OK;
}

dword _cdecl NetClient_SendMasterServerMessage(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wPort = word(((VDATA*)pS->Pop())->GetLong());
	char * pAddress = (((VDATA*)pS->Pop())->GetString());
	NetClient::pClient->SendMasterServerMessage(pAddress, wPort, *pSend);
	return IFUNCRESULT_OK;
}

// NetServer
dword _cdecl NetServer_SendMasterServerMessage(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wPort = word(((VDATA*)pS->Pop())->GetLong());
	char * pAddress = (((VDATA*)pS->Pop())->GetString());
	NetServer::pServer->SendMasterServerMessage(pAddress, wPort, *pSend);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_SendMessage(VS_STACK * pS)
{
	bool bGarantedDelivery = ((VDATA*)pS->Pop())->GetLong() != 0;
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wDestination = word(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->AddPacket(wDestination, *pSend, bGarantedDelivery);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_SendMessageDirect(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wPort = word(((VDATA*)pS->Pop())->GetLong());
	dword dwIP = dword(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->AddPacketDirect(dwIP, wPort, *pSend);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_SendMessageAllExcept(VS_STACK * pS)
{
	bool bGarantedDelivery = ((VDATA*)pS->Pop())->GetLong() != 0;
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	word wDestination = word(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->AddPacketAllExcept(wDestination, *pSend, bGarantedDelivery);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_GarantedDeliveryCallback(VS_STACK * pS)
{
	word wGarantedIndex = word(((VDATA*)pS->Pop())->GetLong());
	word wClient = word(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->GarantedDeliveryCallback(wClient, wGarantedIndex);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_ClearPacketsQueue(VS_STACK * pS)
{
	NetServer::pServer->ClearPacketsQueue();
	return IFUNCRESULT_OK;
}
