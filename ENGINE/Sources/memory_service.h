#ifndef _MEMORY_SERVICE_H_
#define _MEMORY_SERVICE_H_

/* --------------------------------------------------------------------------------------------
   Memory Tracer:
   -------------------------------------------------------------------------------------------- */

#include <windows.h>

#include "..\..\common_h\d_types.h"
#include "..\..\common_h\exs.h"
#include "..\..\common_h\vmemory_service.h"
#include "mem_pool.h"
#include "mem_utils.h"
#include "system_log.h"

#define M_LOCKED 1

typedef struct
{
    DWORD address;
    dword data_size;
    char *link_L;
    char *link_R;
} MEM_BLOCK;

#define SBCNUM 64

class MEMORY_SERVICE : public VMEMORY_SERVICE
{
  protected:
    MEM_BLOCK *Mem_link;
    void *Search_Pointer;
    dword Minimal_Pointer;
    dword Maximal_Pointer;
    dword dwTotalMemory;
    dword dwTotalBlocks;

    MEMPOOL **pMPool;
    DWORD nMPoolClassesNum;

    bool bPoolOn;

  public:
    DWORD SBCounter[SBCNUM];

    MEM_ALLOC_ADDRESS cMemAddress;
    MEM_USAGE_INFO MemStat;
    bool bCollectInfo;

    MEMORY_SERVICE();
    ~MEMORY_SERVICE();

    void *Allocate(long size);
    void *Reallocate(void *block_PTR, long size);
    void Free(void *block_PTR);
    void *PoolAllocate(long size);
    // bool PoolFree(void * block_PTR, DWORD nBlockSize = 0xffffffff);
    long PoolFree(void *block_PTR, DWORD nBlockSize = 0xffffffff);
    DWORD PoolFind(void *block_PTR);

    // bool  ValidateBlock(void * block_PTR);
    long ValidateBlock(void *block_PTR);

    void GlobalFree();
    void SetState(word control_transfer_point, dword module_class);
    dword GetBlockSize(void *block_PTR);

    void *GetFirstPointer();
    void *GetNextPointer();
    // bool GetPointerState(void * block_PTR);
    long GetPointerState(void *block_PTR);

    dword Allocated_memory_user;
    dword Allocated_memory_system;
    dword Blocks;

    void CollectInfo(bool bv)
    {
        bCollectInfo = bv;
    };

    void ProcessMemProfile(char *pFileName);

    char *GetFileName(void *pMemory);
    dword GetFileLineCode(void *pMemory);

    void DumpMemoryState();
};

#endif