#ifndef K2SERVER_HPP
#define K2SERVER_HPP

#include "Common.h"
#include "CRC.h"

class NetServer : public ENTITY
{
public:
	NetServer();
	~NetServer();

	void Execute(float fDeltaTime);
	void Realize(float fDeltaTime);
	void ReceiveMessage(SOCKET s, dword lParam);
	dword AttributeChanged(ATTRIBUTES * pA);

public:
	static NetServer * pServer;
	static VDX8RENDER * pRS;
	static float fDeltaTime;	// current delta time
	static float fGravity;
	
	bool AddClient(word wClientID, dword dwIP, word wPort);
	bool DelClient(word wClientID);

	// return current game time
	dword GetTime();
	// add packet to queue
	void AddPacket(word wDestination, NMSend & nmSend, bool bGarantedDelivery = false);
	void AddPacketAllExcept(word wDestination, NMSend & nmSend, bool bGarantedDelivery = false);
	void AddPacketDirect(dword dwIP, word wPort, NMSend & nmSend);
	void SendMasterServerMessage(const char * pAddress, word wPort, NMSend & nmSend);

	void ClearPacketsQueue();

	void GarantedDeliveryCallback(word wClient, word wGarantedIndex);

private:
	CRC16 crc16;
	dword dwServerTime;
	word wServerPort;
	dword dwMaxFPS;

	SOCKET s;
	SOCKADDR_IN sin;
	bool bStarted;
	bool bDedicated;
	
	dword dwTotalBytesSend, dwTotalBytesRecv;
	dword dwTotalPacketsSend, dwTotalPacketsRecv; 
	float fPacketsSendPerSecond, fBytesSendPerSecond, fLastPSpS, fLastTBSpS, fStatTime;

	// server side clients
	array<LocalClient*> aClients;

	bool Start();
	bool Stop();

	// request for connect from client
	LocalClient * FindClient(word wClientID);
	bool IsDedicated() { return bDedicated; }
};

#endif