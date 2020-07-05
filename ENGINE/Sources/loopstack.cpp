#include "loopstack.h"

LOOPSTACK::LOOPSTACK()
{
	pBlock = 0;
	Block_Num = 0;
	Block_Size = 0;
}

LOOPSTACK::~LOOPSTACK()
{
	if(pBlock) delete(pBlock);
}

void LOOPSTACK::Push(LOOPBLOCK block)
{
	Block_Num++;
	if(Block_Num >= Block_Size)
	{
		if(Block_Size == 0)
		{

			pBlock = (LOOPBLOCK *)NEW char[sizeof(LOOPBLOCK)];
			pBlock[0] = block;
			Block_Size = 1;
			return;
		}
		pBlock = (LOOPBLOCK *)RESIZE(pBlock,Block_Num * sizeof(LOOPBLOCK));
		Block_Size = Block_Num;
		pBlock[Block_Num - 1] = block;
	}
}

bool LOOPSTACK::Pop(LOOPBLOCK * _pblock)
{
	if(Block_Num == 0) return false;
	if(_pblock) memcpy(_pblock,&pBlock[Block_Num - 1],sizeof(LOOPBLOCK));
	Block_Num--;
	return true;
}

bool LOOPSTACK::Read(LOOPBLOCK * _pblock)
{
	if(Block_Num == 0) return false;
	if(_pblock) memcpy(_pblock,&pBlock[Block_Num - 1],sizeof(LOOPBLOCK));
	return true;
}