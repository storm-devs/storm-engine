#include "MasterServer.h"

MasterServer	MS;
WSADATA			WSAData;
SOCKET			sock;
SOCKADDR_IN		sin;

char cBuffer[4096];
void _cdecl Trace(char * data_PTR, ...)
{
	/*va_list args;
	va_start(args, data_PTR);
	_vsnprintf(cBuffer, sizeof(cBuffer) - 4, data_PTR, args);
	va_end(args);

	printf(cBuffer);*/
}

int main()
{
   	WSAStartup(MAKEWORD(1, 1), &WSAData);
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sin.sin_family = AF_INET;
	sin.sin_port = htons (29029);//htons (wServerPort);
	sin.sin_addr.s_addr = htonl (INADDR_ANY);

	int iValue;
	dword dwTrue = dword(true);

	iValue = bind(sock, (struct sockaddr *)&sin, sizeof(sin));
	iValue = ioctlsocket(sock, FIONBIO, &dwTrue);

	/*for (long i=1; i<100; i++)
		for (long j=0; j<20; j++)
		{
			NMSend nmSend;
			nmSend.AddByte(rand()%4 + 98);
			NMRecv nmRecv(nmSend.GetDataBuffer(), nmSend.GetDataSize());
			dword dwIP;
			((byte*)&dwIP)[0] = 192;
			((byte*)&dwIP)[1] = 168;
			((byte*)&dwIP)[2] = byte(i);
			((byte*)&dwIP)[3] = byte(j);
			MS.AddServer(dwIP, rand()%65535, nmRecv);
		}*/

	Trace("Master Server is started\n");
	while (true)
	{
	    timeval timeout;
		fd_set	fdset;
		
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset); // network socket
	
		int msec = 3000;
		timeout.tv_sec = msec / 1000;
		timeout.tv_usec = (msec % 1000) * 1000;
		iValue = select(sock, &fdset, NULL, NULL, &timeout);

		// try to read from socket
		while (MS.ReceiveMessage()) {};

		// check clients
		MS.Execute();
	}

	return 0;
}

MasterServer::MasterServer() :
	aClients(_FL_),
	aServers(_FL_)
{
	_timeb	_tb;
	_ftime(&_tb);
	tmStartTime = _tb.time;
}

MasterServer::~MasterServer()
{
	aClients.DelAllWithPointers();
}

void MasterServer::Execute()
{
	dword i;
	dword dwCurrentTime = GetCurrentTime();

	// check servers who long (5 min) dont send any ping)
	for (i=0; i<aServers.Size(); i++) if (dwCurrentTime - aServers[i].dwLastServerTime > 60 * 5)
	{
		Trace("delete server\n");
		aServers.ExtractNoShift(i);
		i--;
	}

	// check clients who dont ask any info for 20 seconds
	for (i=0; i<aClients.Size(); i++) 
		if (dwCurrentTime - aClients[i]->dwLastClientTime > 20)
	{
		Trace("delete client\n");
		delete aClients[i];
		aClients.ExtractNoShift(i);
		i--;
	}
}

bool MasterServer::ReceiveMessage()
{
	char cData[1024];
	SOCKADDR_IN from;
	int iFromLen = sizeof(from);
	int iLen = recvfrom(sock, cData, sizeof(cData), 0, (struct sockaddr *)&from, &iFromLen);
	int iError = WSAGetLastError();

	if (iLen > 0)
	{
		NMRecv nmRecv(cData, iLen);

		dword dwIP = from.sin_addr.S_un.S_addr;
		word wPort = from.sin_port;

		word wCRC16 = nmRecv.GetWord();
		word wNewCRC16 = crc16.GetCRC16(nmRecv.GetDataBuffer() + IS_NM_DEBUG(3, 2), nmRecv.GetDataSize() - IS_NM_DEBUG(3, 2));
		if (wNewCRC16 == wCRC16)
		{
			word wPacketIndex = nmRecv.GetWord();
			ProcessMessage(dwIP, wPort, nmRecv);
		}
		return true;
	}

	return false;
}

void MasterServer::UpdateServer(dword dwIP, word wPort, NMRecv & nmRecv)
{
	for (dword i=0; i<aServers.Size(); i++) if (aServers[i].dwIP == dwIP && aServers[i].wPort == wPort) 
	{
		aServers[i].dwLastServerTime = GetCurrentTime();
		return;
	}
}

void MasterServer::ProcessMessage(dword dwIP, word wPort, NMRecv & nmRecv)
{
	byte byCode = nmRecv.GetByte();
	if (byCode != NC_MASTERSERVER) return;

	byte bySubCode = nmRecv.GetByte();
	switch (bySubCode)
	{
		case NSC_MASTERSERVER_CONNECT:
			OnConnect(dwIP, wPort, nmRecv);
		break;
		case NSC_MASTERSERVER_DISCONNECT:
			OnDisconnect(dwIP, wPort, nmRecv);
		break;
		case NSC_MASTERSERVER_GETSERVERS:
			GetServers(dwIP, wPort, nmRecv);
		break;
		case NSC_MASTERSERVER_UPDATESERVER:
			UpdateServer(dwIP, wPort, nmRecv);
		break;
		case NSC_MASTERSERVER_ADDSERVER:
			AddServer(dwIP, wPort, nmRecv);
		break;
		case NSC_MASTERSERVER_DELSERVER:
			DelServer(dwIP, wPort, nmRecv);
		break;
	}
}

void MasterServer::DelServer(dword dwIP, word wPort, NMRecv & nmRecv)
{
	for (dword i=0; i<aServers.Size(); i++) if (aServers[i].dwIP == dwIP && aServers[i].wPort == wPort) 
	{
		aServers.ExtractNoShift(i);
		in_addr ia;	ia.S_un.S_addr = dwIP;
		Trace("Deleted server: IP: %s:%d\n", inet_ntoa(ia), dword(ntohs(wPort)));
		return;
	}
}

void MasterServer::AddServer(dword dwIP, word wPort, NMRecv & nmRecv)
{
	byte byVersion = nmRecv.GetByte();

	// check if server already exist
	for (dword i=0; i<aServers.Size(); i++) if (aServers[i].dwIP == dwIP && aServers[i].wPort == wPort) return;

	// add new server
	Server & Srv = aServers[aServers.Add()];
	
	Srv.dwIP = dwIP;
	Srv.wPort = wPort;
	Srv.byVersion = byVersion;
	Srv.dwLastServerTime = GetCurrentTime();

	in_addr ia;	ia.S_un.S_addr = dwIP;
	Trace("Added server: IP: %s:%d, Version = %d\n", inet_ntoa(ia), dword(ntohs(wPort)), dword(byVersion));
}

void MasterServer::OnDisconnect(dword dwIP, word wPort, NMRecv & nmRecv)
{
	dword i;

	// search for connected client
	for (i=0; i<aClients.Size(); i++) if (aClients[i]->dwIP == dwIP && aClients[i]->wPort == wPort)
	{
		Trace("delete client\n");
		delete aClients[i];
		aClients.ExtractNoShift(i);
		return;
	}
}

void MasterServer::OnConnect(dword dwIP, word wPort, NMRecv & nmRecv)
{
	dword i;

	// search for already connected client
	for (i=0; i<aClients.Size(); i++) if (aClients[i]->dwIP == dwIP && aClients[i]->wPort == wPort)
	{
		OnConnectCallback(aClients[i]);
		return;
	}
	
	byte byClientVersion = nmRecv.GetByte();

	Client * pClient = new Client();
	
	for (i=0; i<aServers.Size(); i++) if (aServers[i].byVersion == byClientVersion)
	{
		pClient->aServers.Add(aServers[i]);
	}

	pClient->dwIP = dwIP;
	pClient->wPort = wPort;
	pClient->dwLastClientTime = GetCurrentTime();
	aClients.Add(pClient);

	OnConnectCallback(pClient);
}

void MasterServer::OnConnectCallback(Client * pClient)
{
	NMSend nmSend;
	nmSend.AddByte(NC_MASTERSERVER);
	nmSend.AddByte(NSC_MASTERSERVER_CONNECT);
	nmSend.Add24(pClient->aServers.Size());
	SendMessage(pClient->dwIP, pClient->wPort, nmSend);

	in_addr ia;	ia.S_un.S_addr = pClient->dwIP;
	Trace("Client connect IP: %s:%d\n", inet_ntoa(ia), dword(pClient->wPort));
}

void MasterServer::GetServers(dword dwIP, word wPort, NMRecv & nmRecv)
{
	dword	i;
	Client	* pClient = null;

	for (i=0; i<aClients.Size(); i++) if (aClients[i]->dwIP == dwIP && aClients[i]->wPort == wPort)
	{
		pClient = aClients[i];
		break;
	}

	long iLastSendedServerNum = long(nmRecv.Get24());

	long iSendServersNum = 0;
	NMSend nmSend;

	nmSend.AddByte(NC_MASTERSERVER);
	nmSend.AddByte(NSC_MASTERSERVER_GETSERVERS);
	nmSend.Add24(iLastSendedServerNum);
	if (pClient)
	{
		// 5 maximum
		iSendServersNum = Min((long)5, pClient->aServers.Len() - iLastSendedServerNum);
		nmSend.AddByte(byte(iSendServersNum));
		for (long j=iLastSendedServerNum; j<iLastSendedServerNum + iSendServersNum; j++)
		{
			nmSend.AddDword(pClient->aServers[j].dwIP);
			nmSend.AddWord(pClient->aServers[j].wPort);
		}
		pClient->dwLastClientTime = GetCurrentTime();
	}
	else
	{
		nmSend.AddByte(0);
	}

	SendMessage(dwIP, wPort, nmSend);

	/*if (pClient && iSendServersNum == 0)	// delete client if all servers sended
	{
		aClients.Del(pClient);
		delete pClient;
	}*/
}

void MasterServer::SendMessage(dword dwIP, word wPort, NMSend & nmSend)
{
	SOCKADDR_IN to;
	memset(&to, 0, sizeof(SOCKADDR_IN));

	to.sin_addr.S_un.S_addr = dwIP;
	to.sin_port = wPort;
	to.sin_family = AF_INET;

	crc16.MakeCRC16(nmSend.GetDataBuffer(), nmSend.GetDataSize(), 0x0);
	sendto(sock, crc16.GetDataBuffer(), crc16.GetDataSize(), 0, (struct sockaddr *)&to, sizeof(to));
}

dword MasterServer::GetCurrentTime()
{
	_timeb	_tb;
	_ftime(&_tb);

	return _tb.time - tmStartTime;
}

