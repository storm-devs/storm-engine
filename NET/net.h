#ifndef K2NET_HPP
#define K2NET_HPP

#include "Common.h"

class Net : public ENTITY
{
public:
	Net();
	~Net();

	bool Init();

	void Execute(dword dwDeltaTime);
	void Realize(dword dwDeltaTime);

	dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES * pA);

private:
	WSADATA WSAData;

	bool WSAStartup(dword dwVerHi, dword dwVerLo);
	bool WSACleanup();
};

#endif