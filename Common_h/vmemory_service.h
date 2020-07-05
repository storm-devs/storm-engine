#ifndef _VMEMORY_SERVICE_H_
#define _VMEMORY_SERVICE_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "d_types.h"

class VMEMORY_SERVICE
{
public:
	VMEMORY_SERVICE(){};
	virtual ~VMEMORY_SERVICE(){};
	
	virtual void * Allocate(long size)=0;
	virtual void * Reallocate(void * block_PTR,long size)=0;
	virtual void  Free(void * block_PTR)=0;
	virtual void  GlobalFree()=0;
//	virtual void  SetState(word entry_point, dword module_code)=0;
//	virtual void  SetFlags(dword _flags,void * block_PTR)=0;
//	virtual dword GetFlags(void * block_PTR)=0;
};

#endif