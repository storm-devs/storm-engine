#ifndef _MEM_UTILS_H_
#define _MEM_UTILS_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#define DEBUGCLASSES

#define SMALL_BLOCKS_LIMIT	256
#define MEDIUM_BLOCKS_LIMIT	256
#define MEDIUM_BLOCK_MSIZE  65536

struct MEM_USAGE_INFO_BLOCK
{
	char * pFileName;
	DWORD  nLine;
	DWORD  nCurrentAllocated;
	DWORD  nCurrentBlocks;
	DWORD  nMaxAllocated;
	DWORD  nMaxBlocks;


	DWORD  nTotalAllocated;
	DWORD  nTotalBlocks;
};

struct SBSUNIT
{
	DWORD nCurrent;
	DWORD nMax;
	DWORD nB2TRatio;
};

class MEM_USAGE_INFO
{
	MEM_USAGE_INFO_BLOCK * pMemBL;
	DWORD nMemBLNum;
public:
	DWORD nTotalAllocated;
	DWORD nCurrentAllocated;
	DWORD nMaxSimAllocated;
	DWORD nTotalBlocks;
	DWORD nCurrentBlocks;
	DWORD nMaxSimBlocks;

	DWORD nTestBlocks;

	SBSUNIT nBlockTableSBS[SMALL_BLOCKS_LIMIT];
	SBSUNIT nBlockTableMBS[MEDIUM_BLOCKS_LIMIT];

	DWORD nTimePeriod;
	DWORD nTimeCounter;

	 MEM_USAGE_INFO();
	~MEM_USAGE_INFO();
	void Allocate(char * pFileName, DWORD nLine, DWORD nMemSize);
	void Resize(char * pFileName, DWORD nLine, DWORD nMemSize, 
		char * pOldFileName, DWORD nOldLine, DWORD nOldMemSize);
	void Free(char * pFileName, DWORD nLine, DWORD nMemSize);
	void Report();
	void Register(char * pFileName, DWORD nLine, DWORD nMemSize);
	void Unregister(char * pFileName, DWORD nLine, DWORD nMemSize);

	void ProcessTime(DWORD time);
	void UpdateMemoryProfile(char * pFileName);

};

class MEM_ALLOC_ADDRESS
{
public:
	struct SOURCE_DESC
	{
		SOURCE_DESC(){pFileName = 0; dwLine = 0;};
		char * pFileName;
		DWORD dwLine;
	};
	SOURCE_DESC * pData;
	DWORD dwRecordsNum;
	DWORD dwBufferSize;

	MEM_ALLOC_ADDRESS();
	~MEM_ALLOC_ADDRESS();
	DWORD  GetAddress(const char * pFileName, DWORD dwLine);
	char * GetSource(DWORD dwAddress, DWORD * dwLine);
};

#endif