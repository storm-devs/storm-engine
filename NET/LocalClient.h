#ifndef LOCAL_CLIENT_HPP
#define LOCAL_CLIENT_HPP

#include "Common.h"
#include "CRC.h"

class LocalClient
{
public:
	dword dwTotalBytesSend, dwTotalPacketsSend;

	LocalClient(SOCKET * pSocket, word wClientID, dword dwIP, word wPort);
	~LocalClient();

	void Execute(float fDeltaTime);

	void SetClientID(word wClientID) { this->wClientID = wClientID; };
	word GetClientID() { return wClientID; }

	void AddPacket(NMSend & nmSend, bool bGarantedDelivery);
	void ClearPacketsQueue();
	void GarantedDeliveryCallback(word wGarantedIndex);

private:
	struct packet
	{
		bool	bGarantedDelivery;
		word	wPacketIndex;
		NMSend	nmBuffer;
	};

	CRC16		crc16;
	SOCKET		* pSocket;
	word		wClientID;
	SOCKADDR_IN from;
	NMSend		nmBuffer;
	word		wCurrentDeliveryIndex;
	float		fCurrentDeliveryTime;
	packet		* pGarantedPacket;

	array<packet*>	aPackets;

	void Flush();
	void Send(packet * pPacket);
	word GetCurrentPacketIndex();
};

#endif