#ifndef _MEMORY_SERVICE_H_
#define _MEMORY_SERVICE_H_

/* --------------------------------------------------------------------------------------------
   Memory Tracer:
   -------------------------------------------------------------------------------------------- */

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "..\..\common_h\vmemory_service.h"
#include "..\..\common_h\d_types.h"
#include "..\..\common_h\exs.h"
#include "system_log.h"
#include "mem_utils.h"
#include "mem_pool.h"


#define		M_LOCKED	1

//#pragma pack(push,1)

/*typedef struct 
{
	dword  ctp;
	dword  class_code;
} MEM_EXE_STATE;
*/
typedef struct 
{
//	dword reference;
//	dword check_sum;
//	MEM_EXE_STATE state;
	//DWORD a;
	//char b;
	DWORD address;
	dword data_size;
	char * link_L;
	char * link_R;
	//CODESOURCE cs;
	//word flags;	
	//word le;
	//byte sb1;
	//byte sb2;
} MEM_BLOCK;

#define SBCNUM	64

//#pragma pack(pop)


class MEMORY_SERVICE : public VMEMORY_SERVICE
{
protected:
//	MEM_EXE_STATE Exe_state;
	MEM_BLOCK * Mem_link;
	void * Search_Pointer;
	dword Minimal_Pointer;
	dword Maximal_Pointer;
	dword dwTotalMemory;
	dword dwTotalBlocks;
	
	MEMPOOL * * pMPool;
	DWORD nMPoolClassesNum;

	bool bPoolOn;

public:

	DWORD SBCounter[SBCNUM];

//#ifdef DEBUGCLASSES
	MEM_ALLOC_ADDRESS cMemAddress;
//#endif
	MEM_USAGE_INFO MemStat;
	bool bCollectInfo;

	MEMORY_SERVICE();
	~MEMORY_SERVICE();
	
	void * Allocate(long size);
	void * Reallocate(void * block_PTR,long size);
	void  Free(void * block_PTR);
	void * PoolAllocate(long size);
	bool PoolFree(void * block_PTR, DWORD nBlockSize = 0xffffffff);
	DWORD PoolFind(void * block_PTR);

	//void  IsolateMemBlock(MEM_BLOCK mem_block, MEM_BLOCK * mb_PTR);
	bool  ValidateBlock(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/);

	void  GlobalFree();
	void  SetState(word control_transfer_point, dword module_class);
	//void  SetFlags(dword _flags,void * block_PTR);
	//void  ClrFlags(dword _flags,void * block_PTR);
	//dword GetFlags(void * block_PTR);
	dword GetBlockSize(void * block_PTR);

	//dword RefInc(void * block_PTR);
	//dword RefDec(void * block_PTR);
	//void  SetRef(void * block_PTR,dword ref);
	//void Lock(void * block_PTR);
	//void Unlock(void * block_PTR);

	void * GetFirstPointer();
	void * GetNextPointer();
	bool GetPointerState(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/);
	
	dword Allocated_memory_user;
	dword Allocated_memory_system;
	dword Blocks;

	void CollectInfo(bool bv){ bCollectInfo = bv;};

	void ProcessMemProfile(char * pFileName);

	char * GetFileName(void * pMemory);
	dword GetFileLineCode(void * pMemory);

	void DumpMemoryState();
		
};

//#pragma pack(pop)

#endif