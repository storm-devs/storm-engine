#include "Client.h"
#include "LocalClient.h"

NetClient * NetClient::pClient = null;
VDX8RENDER * NetClient::pRS = null;
float NetClient::fDeltaTime = 0.0f;

NetClient::NetClient() : aPackets(_FL_)
{
	pClient = this;

	dwTotalPacketsSend = dwTotalBytesSend =	dwTotalBadPacketsRecv = dwTotalPacketsRecv = dwTotalBytesRecv = 0;
	fPacketsSendPerSecond = fBytesSendPerSecond = fPacketsRecvPerSecond = fBytesRecvPerSecond = 0.0f;
	fLastPSpS = fLastTBSpS = fLastPRpS = fLastTBRpS = 0.0f;
	fStatTime = 0.0f;

	bStarted = false;

	wCurrentDeliveryIndex = 0;
	pGarantedPacket = null;
	fCurrentDeliveryTime = 0.0f;
}

NetClient::~NetClient()
{
	Stop();
}

bool NetClient::Stop()
{
	if (!bStarted) return false;
	bStarted = false;
	Flush();
	WSAAsyncSelect(sock, api->GetAppHWND(), 0, 0);
	closesocket(sock);
	return true;
}

word NetClient::GetCurrentPacketIndex()
{
	wCurrentDeliveryIndex++;
	if (wCurrentDeliveryIndex >= 32767 || wCurrentDeliveryIndex == 0) wCurrentDeliveryIndex = 1;
	return wCurrentDeliveryIndex;
}

void NetClient::SendMessageDirect(NMSend & nmSend)
{
	packet Packet;
	Packet.bGarantedDelivery = false;
	Packet.wPacketIndex = GetCurrentPacketIndex();
	Packet.nmBuffer.AddData(nmSend.GetDataBuffer(), nmSend.GetDataSize());
	Send(&Packet);
}

void NetClient::Send(packet * pPacket)
{
	word wPacketIndex = pPacket->wPacketIndex | ((pPacket->bGarantedDelivery) ? (0x1 << 0xF) : 0x0);
	crc16.MakeCRC16(pPacket->nmBuffer.GetDataBuffer(), pPacket->nmBuffer.GetDataSize(), wPacketIndex);
	int iValue = sendto(sock, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&srv_sin, sizeof(srv_sin));

	dwTotalPacketsSend++;
	dwTotalBytesSend += crc16.GetDataSize();
}

void NetClient::Flush()
{
	if (aPackets.IsEmpty()) return;

	for (long i=0; i<aPackets; i++)
	{
		if (!aPackets[i]->bGarantedDelivery)
		{
			Send(aPackets[i]);
			DELETE(aPackets[i]);
		}
		else
		{
			pGarantedPacket = aPackets[i];
			fCurrentDeliveryTime = 0.0f;
			Send(pGarantedPacket);
			i++;
			break;
		}
	}
	if (i > 0)
		aPackets.DelRange(0, i - 1);
}

void NetClient::AddPacket(NMSend & nmSend, bool bGarantedDelivery)
{
	//api->Trace("cln pct size = %d", nmSend.GetDataSize());
	Assert(nmSend.GetDataSize() <= 128);

	packet * pP = NEW packet;
	pP->bGarantedDelivery = bGarantedDelivery;
	pP->wPacketIndex = GetCurrentPacketIndex();
	pP->nmBuffer.AddData(nmSend.GetDataBuffer(), nmSend.GetDataSize());

	aPackets.Add(pP);

	Execute(0.0f);
}

bool NetClient::Start()
{
	int iValue;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sin.sin_family = AF_INET;
	sin.sin_port = 0;							// any port
	sin.sin_addr.s_addr = htonl(INADDR_ANY);	// any address
	iValue = bind(sock, (struct sockaddr *)&sin, sizeof(sin));
	int iError = WSAGetLastError();
	iValue = WSAAsyncSelect(sock, api->GetAppHWND(), SD_CLIENTMESSAGE, FD_READ);

	int iTrue = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST,(char*)&iTrue, sizeof(iTrue));

	bStarted = true;

	return true;
}

void NetClient::GarantedDeliveryCallback(word wGarantedIndex)
{
	if (pGarantedPacket && wGarantedIndex == pGarantedPacket->wPacketIndex)
	{
		DELETE(pGarantedPacket);
	}
}

void NetClient::Execute(float fDeltaTime)
{
	if (pGarantedPacket)
	{
		fCurrentDeliveryTime += fDeltaTime;
		if (fCurrentDeliveryTime >= 0.1f)
		{
			fCurrentDeliveryTime = 0.0f;
			// try to send garanteed message again
			Send(pGarantedPacket);
			api->Trace("Net: client resend garanteed packet");
		}
		return;
	}
	else
		Flush();

	/*dword dwDeltaTime = api->GetDeltaTime();
	VDATA * pVCTime = (VDATA *)api->GetScriptVariable("iClientTime"); Assert(pVCTime);
	pVCTime->Set(long(pVCTime->GetLong() + long(dwDeltaTime)));

	VDATA * pVCSTime = (VDATA *)api->GetScriptVariable("iClientServerTime"); Assert(pVCSTime);
	pVCSTime->Set(long(pVCSTime->GetLong() + long(dwDeltaTime)));*/
}

void NetClient::Realize(float fDeltaTime)
{
	fStatTime += fDeltaTime;
	if (fStatTime >= 1.0f)
	{
		fLastPSpS = dwTotalPacketsSend - fPacketsSendPerSecond;
		fLastTBSpS = dwTotalBytesSend - fBytesSendPerSecond;
		fLastPRpS = dwTotalPacketsRecv - fPacketsRecvPerSecond;
		fLastTBRpS = dwTotalBytesRecv - fBytesRecvPerSecond;

		fStatTime = 0.0f;
		fPacketsSendPerSecond = float(dwTotalPacketsSend);
		fBytesSendPerSecond = float(dwTotalBytesSend);
		fPacketsRecvPerSecond = float(dwTotalPacketsRecv);
		fBytesRecvPerSecond = float(dwTotalBytesRecv);
	}

	/*string str;
	str.Format("Client: Send: %d, %.1f, %d, %.1f, Recv: %d, %.1f, %d, %.1f", 
		dwTotalPacketsSend, fLastPSpS, dwTotalBytesSend + dwTotalPacketsSend * 26, fLastTBSpS + fLastPSpS * 26,
		dwTotalPacketsRecv, fLastPRpS, dwTotalBytesRecv + dwTotalPacketsRecv * 26, fLastTBRpS + fLastPRpS * 26);

	pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(20), (char*)str.GetBuffer());

	CVECTOR vCamPos, vCamAng;
	float fFov;
	Render().GetCamera(vCamPos, vCamAng, fFov);
	str.Format("Cam: %.2f, %.2f, %.2f", vCamPos.x, vCamPos.y, vCamPos.z);
	pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(40), (char*)str.GetBuffer());
	*/
}

void NetClient::ReceiveMessage(SOCKET s, dword lParam)
{
	char cData[1024];

	SOCKADDR_IN from;
	int iFromLen = sizeof(from);
	int iLen = recvfrom(sock, cData, sizeof(cData), 0, (struct sockaddr *)&from, &iFromLen);

	if (iLen > 0)
	{
		NMRecv nmRecv(cData, iLen);

		word wCRC16 = nmRecv.GetWord();
		word wNewCRC16 = crc16.GetCRC16(nmRecv.GetDataBuffer() + IS_NM_DEBUG(3, 2), nmRecv.GetDataSize() - IS_NM_DEBUG(3, 2));

		dwTotalPacketsRecv++;
		dwTotalBytesRecv += nmRecv.GetDataSize();

		dword dwIP = from.sin_addr.S_un.S_addr;
		word wPort = from.sin_port;

		if (wNewCRC16 == wCRC16)
		{
			//if (rand() % 100 >= 90) return;

			word wPacketIndex = nmRecv.GetWord();
			word wGarantedDeliveryIndex = wPacketIndex & 0x7FFF;
			bool bGarantedDelivery = (wPacketIndex & 0x8000) != 0;

			api->Event("NetClient_OnNetMessage", "llll", long(dwIP), long(wPort), long(&nmRecv), (bGarantedDelivery) ? long(wGarantedDeliveryIndex) : long(0));
		}
		else
		{
			api->Trace("NCError: Bad CRC16 on packet from server");
			dwTotalBadPacketsRecv++;
		}
	}
}

void NetClient::SendBroadcast(NMSend & nmSend)
{
	SOCKADDR_IN target;

	target.sin_family = AF_INET;
	target.sin_port = htons(wServerPort);
	target.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	memset(&target.sin_zero, 0, sizeof(target.sin_zero)); 

	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0);
	sendto(sock, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&target, sizeof(target));
}

void NetClient::SendMessageDirectToIP(dword dwIP, word wPort, NMSend & nmSend)
{
	if (nmSend.IsEmpty()) return;

	SOCKADDR_IN to;
	to.sin_addr.S_un.S_addr = dwIP;
	to.sin_port = htons(wPort);
	to.sin_family = AF_INET;
	ZERO(to.sin_zero);
	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0);
	sendto(sock, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&to, sizeof(to));
	dwTotalBytesSend += crc16.GetDataSize();
	dwTotalPacketsSend++;
}

void NetClient::SendMasterServerMessage(const char * pAddress, word wPort, NMSend & nmSend)
{
	SOCKADDR_IN target;

	hostent * he = gethostbyname(pAddress); if (!he) return;
	
	target.sin_family = AF_INET;
	target.sin_port = htons(wPort);
	target.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&target.sin_zero, 0, sizeof(target.sin_zero)); 

	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0);
	int iResult = sendto(sock, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&target, sizeof(target));
}

void NetClient::UpdateServerAddress()
{
	hostent * he = gethostbyname((const char *)sServerAddr);

	char * p1 = he->h_addr_list[0];
	char * p2 = he->h_addr_list[1];

	char cHostName[256];
	gethostname(cHostName, 256);

	if (!he) return;
	srv_sin.sin_family = AF_INET;
	srv_sin.sin_port = htons (wServerPort);
	srv_sin.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&srv_sin.sin_zero, 0, sizeof(srv_sin.sin_zero)); 
}

void NetClient::ClearPacketsQueue()
{
	DELETE(pGarantedPacket);
	aPackets.DelAll();
}

dword NetClient::AttributeChanged(ATTRIBUTES * pA)
{
	ATTRIBUTES * pAP = AttributesPointer;
	
	if (*pA == "ServerAddr") { sServerAddr = pA->GetThisAttr(); UpdateServerAddress(); return 0; }
	if (*pA == "ServerPort") { wServerPort = (word)pA->GetAttributeAsDword(); UpdateServerAddress(); return 0; }

	if (*pA == "Start")
	{
		Stop();

		sMasterServerAddr = pAP->GetAttribute("MasterServerAddr");
		wMasterServerPort = (word)pAP->GetAttributeAsDword("MasterServerPort", 29020);

		Start();
		return 0;
	}

	if (*pA == "Stop")
	{
		return 0;
	}

	return 0;
}
