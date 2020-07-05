#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include "Common.h"

class MasterServer
{
public:
	MasterServer();
	~MasterServer();

	void Execute();
	bool ReceiveMessage();

	void AddServer(dword dwIP, word wPort, NMRecv & nmRecv);
private:
	CRC16			crc16;
	time_t			tmStartTime;

	array<Client*>	aClients;
	array<Server>	aServers;

	dword GetCurrentTime();

	void ProcessMessage(dword dwIP, word wPort, NMRecv & nmRecv);
	void SendMessage(dword dwIP, word wPort, NMSend & nmSend);

	void DelServer(dword dwIP, word wPort, NMRecv & nmRecv);

	void OnConnect(dword dwIP, word wPort, NMRecv & nmRecv);
	void OnDisconnect(dword dwIP, word wPort, NMRecv & nmRecv);
	void OnConnectCallback(Client * pClient);
	void GetServers(dword dwIP, word wPort, NMRecv & nmRecv);

	void UpdateServer(dword dwIP, word wPort, NMRecv & nmRecv);
};

#endif