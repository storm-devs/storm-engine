#ifndef _VS_STACK_H_
#define _VS_STACK_H_

#include "vdata.h"

class DATA;

class VS_STACK
{
public:
	VS_STACK(){};
	virtual ~VS_STACK(){};
	virtual DATA * Push(DATA * pdataclass = 0)= 0;
	virtual DATA * Pop()= 0;
	virtual DATA * Read(DWORD offset,DWORD index)= 0;
	virtual DATA * Read()= 0;
	virtual DWORD GetDataNum()= 0;
	virtual void InvalidateFrom(DWORD index)= 0;
};

#endif