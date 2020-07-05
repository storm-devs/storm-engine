#ifndef __V_UDP_TRANSPORT_H__
#define __V_UDP_TRANSPORT_H__

#include <windows.h> 

#include "..\ENGINE\SOURCES\vmodule_api.h"

#define NETWORK_MESSAGE		0xF3F3F3F3

class V_UDP_TRANSPORT  : public SERVICE
{
public:
	virtual ~V_UDP_TRANSPORT(){};
	virtual bool Send(char *s) {return false;}; 
};

#endif