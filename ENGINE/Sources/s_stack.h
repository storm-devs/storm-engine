#ifndef _S_STACK_H_
#define _S_STACK_H_

#include "data.h"
#include "..\..\common_h\v_s_stack.h"

#define STACK_BUFFER_BLOCK_SIZE	512
#define STACK_BUFFER_LIMIT		65535

class S_STACK : public VS_STACK
{
	DATA * * pStackData;
	DWORD Buffer_size;
	DWORD Data_num;
	VIRTUAL_COMPILER * pVCompiler;
public:
	 S_STACK();
	~S_STACK();
	void Release();
	DATA * Push(DATA * pdataclass = 0);
	DATA * Pop();
	DATA * Read(DWORD offset,DWORD index);
	DATA * Read();
	DWORD GetDataNum(){return Data_num;};
	void SetVCompiler(VIRTUAL_COMPILER * pvc) {pVCompiler = pvc;}
	void InvalidateFrom(DWORD index);
	
};

#endif