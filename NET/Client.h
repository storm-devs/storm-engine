#ifndef K2CLIENT_HPP
#define K2CLIENT_HPP

#include "common.h"
#include "CRC.h"

class NetClient : public ENTITY
{
public:
	NetClient();
	~NetClient();

	void Execute(float fDeltaTime);
	void Realize(float fDeltaTime);
	void ReceiveMessage(SOCKET s, dword lParam);
	dword AttributeChanged(ATTRIBUTES * pA);

public:
	static NetClient * pClient;
	static VDX8RENDER * pRS;
	static float fDeltaTime;	// current delta time

	// add packet to queue
	void AddPacket(NMSend & nmSend, bool bGarantedDelivery = false);
	// send broadcast message
	void SendBroadcast(NMSend & nmSend);
	// send message to master server
	void SendMasterServerMessage(const char * pAddress, word wPort, NMSend & nmSend);
	// send message directly/immediatelly to server
	void SendMessageDirect(NMSend & nmSend);
	// send message directly/immediatelly to another IP:port
	void SendMessageDirectToIP(dword dwIP, word wPort, NMSend & nmSend);
	// garanted message delivery callback
	void GarantedDeliveryCallback(word wGarantedIndex);
	// clear packets queue and delete current garanteed packet
	void ClearPacketsQueue();

private:
	struct packet
	{
		bool	bGarantedDelivery;
		word	wPacketIndex;
		NMSend	nmBuffer;
	};

	CRC16 crc16;
	string sServerAddr, sMasterServerAddr;
	word wServerPort;
	word wMasterServerPort;
	bool bStarted;

	SOCKET sock;
	SOCKADDR_IN sin, srv_sin;

	array<packet*>	aPackets;
	packet			* pGarantedPacket;
	float			fCurrentDeliveryTime;
	word			wCurrentDeliveryIndex;

	dword dwTotalPacketsSend, dwTotalBytesSend, dwTotalBadPacketsRecv, dwTotalPacketsRecv, dwTotalBytesRecv;
	float fPacketsSendPerSecond, fBytesSendPerSecond, fPacketsRecvPerSecond, fBytesRecvPerSecond;
	float fLastPSpS, fLastTBSpS, fLastPRpS, fLastTBRpS;
	float fStatTime;

	bool Start();
	bool Stop();
	void Flush();
	void UpdateServerAddress();
	word GetCurrentPacketIndex();
	void Send(packet * pPacket);
};

#endif