#include "LocalClient.h"

LocalClient::LocalClient(SOCKET * pSocket, word wClientID, dword dwIP, word wPort) : aPackets(_FL_)
{
	this->pSocket = pSocket;
	this->wClientID = wClientID;
	this->from.sin_addr.S_un.S_addr = dwIP;
	this->from.sin_port = wPort;
	this->from.sin_family = AF_INET;
	ZERO(this->from.sin_zero);
	pSocket = null;

	dwTotalBytesSend = 0;
	dwTotalPacketsSend = 0;

	fCurrentDeliveryTime = 0.0f;
	wCurrentDeliveryIndex = 0;
	pGarantedPacket = null;
}

LocalClient::~LocalClient()
{
}

word LocalClient::GetCurrentPacketIndex()
{
	wCurrentDeliveryIndex++;
	if (wCurrentDeliveryIndex >= 32767 || wCurrentDeliveryIndex == 0) wCurrentDeliveryIndex = 1;
	return wCurrentDeliveryIndex;
}

void LocalClient::Send(packet * pPacket)
{
	word wPacketIndex = pPacket->wPacketIndex | ((pPacket->bGarantedDelivery) ? (0x1 << 0xF) : 0x0);
	crc16.MakeCRC16(pPacket->nmBuffer.GetDataBuffer(), pPacket->nmBuffer.GetDataSize(), wPacketIndex);
	sendto(*pSocket, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&from, sizeof(from));
	
	dwTotalPacketsSend++;
	dwTotalBytesSend += crc16.GetDataSize();
}

void LocalClient::Flush()
{
	Assert(pSocket);
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

void LocalClient::Execute(float fDeltaTime)
{
	if (pGarantedPacket)
	{
		fCurrentDeliveryTime += fDeltaTime;
		if (fCurrentDeliveryTime >= 0.1f)
		{
			fCurrentDeliveryTime = 0.0f;
			// try to send garanteed message again
			Send(pGarantedPacket);
			//api->Trace("Net: server resend garanteed packet");
		}
		return;
	}
	else
		Flush();
}

void LocalClient::ClearPacketsQueue()
{
	DELETE(pGarantedPacket);
	aPackets.DelAll();
}

void LocalClient::GarantedDeliveryCallback(word wGarantedIndex)
{
	if (pGarantedPacket && wGarantedIndex == pGarantedPacket->wPacketIndex)
	{
		DELETE(pGarantedPacket);
	}
}

void LocalClient::AddPacket(NMSend & nmSend, bool bGarantedDelivery)
{
	//api->Trace("srv pct size = %d", nmSend.GetDataSize());
	Assert(nmSend.GetDataSize() <= 128);

	packet * pP = NEW packet;
	pP->bGarantedDelivery = bGarantedDelivery;
	pP->wPacketIndex = GetCurrentPacketIndex();
	pP->nmBuffer.AddData(nmSend.GetDataBuffer(), nmSend.GetDataSize());

	aPackets.Add(pP);

	Execute(0.0f);
}
