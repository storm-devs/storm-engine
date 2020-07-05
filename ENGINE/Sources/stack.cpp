#include "stack.h"

VSTACK::VSTACK()
{
	pBlock = 0;
	Block_Num = 0;
	Block_Size = 0;
}

VSTACK::~VSTACK()
{
	//if(pBlock) delete(pBlock); pBlock = 0;
	Release();
}

void VSTACK::Push(DATA * block)
{
	Block_Num++;
	if(Block_Num >= Block_Size)
	{
		if(Block_Size == 0)
		{

			pBlock = (DATA * *)NEW char[sizeof(DATA *)];
			pBlock[0] = block;
			Block_Size = 1;
			return;
		}
		pBlock = (DATA * *)RESIZE(pBlock,Block_Num * sizeof(DATA *));
		Block_Size = Block_Num;
	}
	pBlock[Block_Num - 1] = block;
}

bool VSTACK::Pop(DATA * * _pblock)
{
	if(Block_Num == 0) 
	{
		if(_pblock) *_pblock = 0; 
		return false;
	}
	if(_pblock) memcpy(_pblock,&pBlock[Block_Num - 1],sizeof(DATA *));
	Block_Num--;
	return true;
}

bool VSTACK::Read(DATA * * _pblock)
{
	if(Block_Num == 0) return false;
	if(_pblock) memcpy(_pblock,&pBlock[Block_Num - 1],sizeof(DATA *));
	return true;
}

void VSTACK::Release()
{
	if(pBlock) delete(pBlock); pBlock = 0;
}