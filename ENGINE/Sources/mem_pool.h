#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#ifndef _XBOX
	#include <windows.h>
#else
	#include <xtl.h>
#endif

//#include "..\..\common_h\memop.h"

#define POOLHEADERSIZE	0//1

class BITLEVEL
{
	DWORD * pBits;
	DWORD nManagedBits;
	DWORD nDwordsNum;
	DWORD nTestCounter;
public:
	~BITLEVEL();
	 BITLEVEL(DWORD nManagedBits);
	 bool SetBit(DWORD nIndex, bool & bDwordFull);
	 void ClearBit(DWORD nIndex);
	 bool FindFree(DWORD & nIndex);
};
class MEMORY_SERVICE;

class MEMPOOL 
{
	friend MEMORY_SERVICE;
	DWORD nBlockSize; 
	DWORD nBlocksNum;
	DWORD nSTLevelsNum;
	DWORD nUsedBlocks;
	DWORD nMissed;
	BITLEVEL * * pSTLevel;
	char * pPoolData;
public:
	~MEMPOOL();
	 MEMPOOL(DWORD nBlockSize, DWORD nBlocksNum);
	void * GetMemory();
	bool FreeMemory(void * pMem);
	bool IsInPool(void * pMem);
	DWORD GetBlockSize(){return nBlockSize;};
};

#endif
