#include "system_api.h"
#include "externs.h"
#include "core.h"

extern CORE Core;

void SYSTEM_API::Push(void * pointer, dword class_code)
{
	//if(entityID_PTR) Control_Stack.Push(idbase.pointer,idbase.class_code,CTP_CONSTRUCTOR);
	if(entityID_PTR) PUSH_CONTROL(pointer,class_code,CTP_CONSTRUCTOR);
}

void SYSTEM_API::Pop(void * pointer)
{
	return;
/*	if(Control_Stack.ScanClassCTPInv(idbase.class_code,CTP_DESTRUCTOR)) _THROW('delete this' not allowed);
	POP_CONTROL(0);
	Memory_Service.Lock(idbase.pointer);	// protect memory from being freed by FreeMem func
	Core.MarkEntityAsDeleted(idbase.pointer);		// assign entity for deleting*/
}

void SYSTEM_API::SetEntityPointer(void * ep)
{
	if(entityID_PTR) entityID_PTR->pointer = ep;
}

ENTITY_ID SYSTEM_API::GetEntityID()
{
	ENTITY_ID id;
	ZeroMemory(&id,sizeof(id));
	if(entityID_PTR == null) return id;
	return *entityID_PTR;
}

char buffer[4096];
void _cdecl SYSTEM_API::Trace(char * data_PTR,...)
{
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(buffer,sizeof(buffer) - 4,data_PTR,args);
	va_end(args);
	trace(buffer);
	strcat(buffer,"\x0d\x0a");
	OutputDebugString(buffer);
}

void SYSTEM_API::SetX()
{
	Exceptions = true;
}

void SYSTEM_API::SetXNF()
{
	ExceptionsNF = true;
}

void * SYSTEM_API::GetCoreApi()
{
	return _CORE_API;
}

void * SYSTEM_API::CreateService(const char * pServiceName)
{
	return _CORE_API->CreateService((char*)pServiceName);
}
