#ifndef _SYSTEM_API_H_
#define _SYSTEM_API_H_

#include "..\..\common_h\vsystem_api.h"

class SYSTEM_API : public VSYSTEM_API
{
public:
	SYSTEM_API() {entityID_PTR = null; Exceptions = false;ExceptionsNF = false;};
	virtual ~SYSTEM_API(){};
	void Push(void * pointer, dword class_code);
	void Pop(void * pointer);
	void SetEntityPointer(void *);
	void _cdecl Trace(char *,...);
	void SetX();
	void SetXNF();
	void * GetCoreApi();
	void * CreateService(const char * pServiceName);

	ENTITY_ID GetEntityID();
	ENTITY_ID * entityID_PTR;
	bool Exceptions;
	bool ExceptionsNF;
};


#endif