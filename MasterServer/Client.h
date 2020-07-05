#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Common.h"

struct Server
{
	dword	dwIP;
	word	wPort;
	byte	byVersion;

	dword	dwLastServerTime;
};

class Client
{
public:
	Client();
	~Client();

	dword			dwIP;
	word			wPort;
	array<Server>	aServers;
	dword			dwLastClientTime;
};

#endif