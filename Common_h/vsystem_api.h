#ifndef _VSYSTEM_API_H_
#define _VSYSTEM_API_H_

//#include <windows.h>
#include "entity_id.h"

class VSYSTEM_API
{
public:
	VSYSTEM_API() {};
	virtual ~VSYSTEM_API(){};
	virtual void Push(void * pointer, dword class_code)= 0;
	virtual void Pop(void * pointer)= 0;
	virtual void SetEntityPointer(void *)= 0;
	virtual ENTITY_ID GetEntityID()= 0;
	virtual void _cdecl Trace(char *,...)= 0;
	virtual void SetX()= 0;
	virtual void SetXNF()= 0;
	virtual void * GetCoreApi()= 0;
	virtual void * CreateService(const char * pServiceName)= 0;
};

#endif