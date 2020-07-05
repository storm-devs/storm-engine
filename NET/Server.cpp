#include "Server.h"
#include "LocalClient.h"

NetServer * NetServer::pServer = null;
VDX8RENDER * NetServer::pRS = null;
float NetServer::fDeltaTime = 0.0f;
float NetServer::fGravity = 9.81f;

NetServer::NetServer() : aClients(_FL_)
{
	pServer = this;
	dwServerTime = 0;
	dwMaxFPS = 100;

	dwTotalBytesSend = 0;
	dwTotalBytesRecv = 0;
	dwTotalPacketsSend = 0;
	dwTotalPacketsRecv = 0;

	fPacketsSendPerSecond = 0.0f;
	fBytesSendPerSecond = 0.0f;
	fLastPSpS = 0.0f;
	fLastTBSpS = 0.0f;
	fStatTime = 0.0f;

	bStarted = false;
	bDedicated = false;
}

NetServer::~NetServer()
{
	Stop();
}

bool NetServer::Stop()
{
	if (!bStarted) return true;

	WSAAsyncSelect(s, api->GetAppHWND(), 0, 0);
	closesocket(s);

	bStarted = false;

	aClients.DelAllWithPointers();

	return true;
}

dword NetServer::GetTime()
{
	return dwServerTime;
}

LocalClient * NetServer::FindClient(word wClientID)
{
	for (long i=0; i<aClients; i++) if (aClients[i]->GetClientID() == wClientID) return aClients[i];
	return null;
}

bool NetServer::AddClient(word wClientID, dword dwIP, word wPort)
{
	aClients.Add(NEW LocalClient(&s, wClientID, dwIP, wPort));
	api->Trace("Srv: AddClient: %d", wClientID);
	return true;
}

bool NetServer::DelClient(word wClientID)
{
	for (long i=0; i<aClients; i++)
		if (aClients[i]->GetClientID() == wClientID)
		{
			DELETE(aClients[i]);
			aClients.DelIndex(i);
			return true;
		}
	return false;
}

void NetServer::AddPacketDirect(dword dwIP, word wPort, NMSend & nmSend)
{
	if (nmSend.IsEmpty()) return;

	SOCKADDR_IN to;
	to.sin_addr.S_un.S_addr = dwIP;
	to.sin_port = wPort;
	to.sin_family = AF_INET;
	ZERO(to.sin_zero);
	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0);
	sendto(s, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&to, sizeof(to));
	dwTotalBytesSend += crc16.GetDataSize();
	dwTotalPacketsSend++;
}

void NetServer::GarantedDeliveryCallback(word wClientID, word wGarantedIndex)
{
	for (long i=0; i<aClients; i++) if (aClients[i]->GetClientID() == wClientID)
		aClients[i]->GarantedDeliveryCallback(wGarantedIndex);
}

void NetServer::AddPacketAllExcept(word wDestination, NMSend & nmSend, bool bGarantedDelivery)
{
	if (nmSend.IsEmpty()) return;
	if (wDestination == DST_ALL) return;

	for (long i=0; i<aClients; i++) if (aClients[i]->GetClientID() != wDestination)
		aClients[i]->AddPacket(nmSend, bGarantedDelivery);
}

void NetServer::ClearPacketsQueue()
{
	for (long i=0; i<aClients; i++) aClients[i]->ClearPacketsQueue();
}

void NetServer::AddPacket(word wDestination, NMSend & nmSend, bool bGarantedDelivery)
{
	if (nmSend.IsEmpty()) return;

	if (wDestination == DST_ALL)
	{
		for (long i=0; i<aClients; i++)
			aClients[i]->AddPacket(nmSend, bGarantedDelivery);
	} 
	else 
	{
		if (LocalClient * pLC = FindClient(wDestination))
			pLC->AddPacket(nmSend, bGarantedDelivery);
	}
}

void NetServer::SendMasterServerMessage(const char * pAddress, word wPort, NMSend & nmSend)
{
	SOCKADDR_IN target;
	hostent * he = gethostbyname(pAddress); if (!he) return;

	target.sin_family = AF_INET;
	target.sin_port = htons(wPort);
	target.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&target.sin_zero, 0, sizeof(target.sin_zero)); 

	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0);
	sendto(s, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&target, sizeof(target));
}

bool NetServer::Start()
{
	int iValue;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sin.sin_family = AF_INET;
	sin.sin_port = htons (wServerPort);
	sin.sin_addr.s_addr = htonl (INADDR_ANY);

	iValue = bind(s, (struct sockaddr *)&sin, sizeof(sin));

	iValue = WSAAsyncSelect(s, api->GetAppHWND(), SD_SERVERMESSAGE, FD_READ);

	bStarted = true;

	return true;
}

void NetServer::Execute(float fDeltaTime)
{
	// execute server-side clients
	for (long i=0; i<aClients; i++)
		aClients[i]->Execute(fDeltaTime);

	if (IsDedicated()) 
	{
		dword dwDeltaTime = api->GetRDeltaTime();
		dword dwFPSDeltaTime = 1000 / dwMaxFPS;
	
		if (dwFPSDeltaTime > dwDeltaTime)
			Sleep(dwFPSDeltaTime - dwDeltaTime);
	}

	//Sleep(30);
}

void NetServer::Realize(float fDeltaTime)
{
	for (long i=0; i<aClients; i++)
	{
		dwTotalPacketsSend += aClients[i]->dwTotalPacketsSend;
		dwTotalBytesSend += aClients[i]->dwTotalBytesSend;

		fPacketsSendPerSecond += aClients[i]->dwTotalPacketsSend;
		fBytesSendPerSecond += aClients[i]->dwTotalBytesSend;

		aClients[i]->dwTotalPacketsSend = 0;
		aClients[i]->dwTotalBytesSend = 0;
	}
	
	fStatTime += fDeltaTime;
	if (fStatTime >= 1.0f)
	{
		fLastPSpS = fPacketsSendPerSecond;
		fLastTBSpS = fBytesSendPerSecond;
		fStatTime = 0.0f;
		fPacketsSendPerSecond = 0.0f;
		fBytesSendPerSecond = 0.0f;
	}

	/*string str;
	str.Format("Server: cl: %d, Send: %d, %.1f, %d, %.1f, Recv: %d, %d", aClients.Size(),
		dwTotalPacketsSend, fLastPSpS, dwTotalBytesSend + dwTotalPacketsSend * 26, fLastTBSpS + fLastPSpS * 26,
		dwTotalPacketsRecv, dwTotalBytesRecv + dwTotalPacketsRecv * 26);

	pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(0), (char*)str.GetBuffer());*/
}

void NetServer::ReceiveMessage(SOCKET s, dword lParam)
{
	char cData[1024];
	
	SOCKADDR_IN from;
	int iFromLen = sizeof(from);
	int iLen = recvfrom(s, cData, sizeof(cData), 0, (struct sockaddr *)&from, &iFromLen);

	if (iLen > 0)
	{
		dwTotalBytesRecv += dword(iLen);
		dwTotalPacketsRecv++;

		NMRecv nmRecv(cData, iLen);

		dword dwIP = from.sin_addr.S_un.S_addr;
		word wPort = from.sin_port;

		word wCRC16 = nmRecv.GetWord();
		word wNewCRC16 = crc16.GetCRC16(nmRecv.GetDataBuffer() + IS_NM_DEBUG(3, 2), nmRecv.GetDataSize() - IS_NM_DEBUG(3, 2));
		if (wNewCRC16 == wCRC16)
		{
			//if (rand() % 100 >= 50) return;

			word wPacketIndex = nmRecv.GetWord();
			word wGarantedDeliveryIndex = wPacketIndex & 0x7FFF;
			bool bGarantedDelivery = (wPacketIndex & 0x8000) != 0;

			api->Event("NetServer_OnNetMessage", "llll", long(dwIP), long(wPort), long(&nmRecv), (bGarantedDelivery) ? long(wGarantedDeliveryIndex) : long(0));
		}
		else
		{
			in_addr in;
			in.S_un.S_addr = from.sin_addr.S_un.S_addr;
			api->Trace("NSError: Bad CRC16 on packet from IP = %s, port = %d", inet_ntoa(in), wPort);
		}
	}
}

dword NetServer::AttributeChanged(ATTRIBUTES * pA)
{
	ATTRIBUTES * pAP = AttributesPointer;

	if (*pA == "StartServer")
	{
		Stop();
		wServerPort = (word)pAP->GetAttributeAsDword("ServerPort", 29020);
		bDedicated = pAP->GetAttributeAsDword("Dedicated") != 0;
		dwMaxFPS = pAP->GetAttributeAsDword("MaxFps", 100);
		Start();
	}

	if (*pA == "StopServer")
	{
		Stop();
	}

	return 0;
}
