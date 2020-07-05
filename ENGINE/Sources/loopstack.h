#ifndef _LOOPSTACK_H_
#define _LOOPSTACK_H_

//#include <windows.h>
#include "..\..\common_h\memop.h"
#include "token.h"

struct LOOPBLOCK
{
	DWORD start;
	DWORD stop;
	S_TOKEN_TYPE type;
};

class LOOPSTACK
{
	LOOPBLOCK * pBlock;
	long Block_Num;
	long Block_Size;
public:
	 LOOPSTACK();
	~LOOPSTACK();
	void Push(LOOPBLOCK block);
	bool Pop(LOOPBLOCK * pblock);
	bool Read(LOOPBLOCK * pblock);


};
#endif