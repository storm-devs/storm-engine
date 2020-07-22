#include "memory_service.h"

#include "..\..\common_h\vapi.h"
extern VAPI * api;

#define DEBUGCLASSES

#define CONST_CHECKSUM		0xabcdef00
#define DBGMEMLINK			0x1b0cdf0

extern dword Exceptions_Mask;
extern CODESOURCE CodeSource;

#define SBPOOL
//#define NOMEMHEADER

MEMORY_SERVICE::MEMORY_SERVICE()
{
	for(DWORD n=0;n<SBCNUM;n++) SBCounter[n] = 0;
	Allocated_memory_user	= 0;
	Allocated_memory_system	= 0;
	Blocks = 0;
	Mem_link = null;
	Search_Pointer = null;
	Minimal_Pointer = null;
	Maximal_Pointer = null;
	dwTotalMemory = 0;
	dwTotalBlocks = 0;
	bCollectInfo = true; // make it true becose mem allocation starts in constructors area
						 // if we dont want to gather this info, this flag will dropped later
						 // rest of information doesnt hart the system
						 // But.. here must be TRUE for proper work 

	nMPoolClassesNum = 0;
	pMPool = 0;
	bPoolOn = false;
}

MEMORY_SERVICE::~MEMORY_SERVICE()
{
	GlobalFree();
}

// current execute state, help to identify who allocate memory
void MEMORY_SERVICE::SetState(word control_transfer_point, dword class_code)
{
}

void * MEMORY_SERVICE::PoolAllocate(long size)
{
	void * data_PTR;
	if(pMPool)
	{
		if((DWORD)size < nMPoolClassesNum) 
		{
			if(pMPool[size])
			{
				data_PTR = pMPool[size]->GetMemory();
				if(data_PTR) 
				{
					if(size < SBCNUM)SBCounter[size]++;
					return data_PTR;
				}
			}
		}
	}
	return 0;
}

void * MEMORY_SERVICE::Allocate(long size)
{
	char * data_PTR;

#ifdef SBPOOL
	if(bPoolOn)
	{
		data_PTR = (char *)PoolAllocate(size);
		if(data_PTR) return data_PTR;
	}
#endif

#ifdef 	NOMEMHEADER
	if(size == 0) return null;
	return malloc(size);
#endif
	MEM_BLOCK * mb_PTR;

	if(size == 0) return null;
	data_PTR = (char *)malloc(sizeof(MEM_BLOCK) + size);
	
	if(data_PTR == null) 
	{
		if(Exceptions_Mask & _X_NO_MEM) SE_THROW_MSG(_X_NO_MEM);
		return null;
	}	
	
	if(size < SBCNUM) SBCounter[size]++;
	
	if(bCollectInfo) MemStat.Allocate(CodeSource.pFileName,CodeSource.line,size);

	MemStat.nTestBlocks++;

	mb_PTR = (MEM_BLOCK *)data_PTR;
	mb_PTR->data_size	= size;

	mb_PTR->address = cMemAddress.GetAddress(CodeSource.pFileName,CodeSource.line);

	mb_PTR->link_L = (char *)Mem_link;
	mb_PTR->link_R = null;
	
	if(Mem_link) Mem_link->link_R = (char *)mb_PTR;
	Mem_link = mb_PTR;

	Allocated_memory_user	+= size;
	Allocated_memory_system	+= sizeof(MEM_BLOCK);
	Blocks++;

	dwTotalMemory = Allocated_memory_system + Allocated_memory_user;
	dwTotalBlocks = Blocks;

	dword result;
	result = (dword)Mem_link + sizeof(MEM_BLOCK);
	if(Minimal_Pointer > result) Minimal_Pointer = result;
	if(Minimal_Pointer == 0) Minimal_Pointer = result;
	if(Maximal_Pointer < result) Maximal_Pointer = result;
	
	if((dword)result == DBGMEMLINK)// - 0xff && (dword)DBGMEMLINK <= 0x11bb4b30 + 0xff)// + sizeof(MEM_BLOCK))
	{
		result = result;
	}

	return 	(void *)((char *)Mem_link + sizeof(MEM_BLOCK));
}

DWORD MEMORY_SERVICE::PoolFind(void * block_PTR)
{
	DWORD n;
	for(n=0;n<nMPoolClassesNum;n++)
	{
		if(pMPool[n] == 0) continue;
		if(pMPool[n]->IsInPool(block_PTR)) return pMPool[n]->GetBlockSize();		
	}
	return 0xffffffff;
}

//bool MEMORY_SERVICE::PoolFree(void * block_PTR, DWORD nBlockSize)
long MEMORY_SERVICE::PoolFree(void * block_PTR, DWORD nBlockSize)
{
	DWORD n;
	if(nBlockSize != 0xffffffff)
	{
		if(nBlockSize >= nMPoolClassesNum) return false;
		if(pMPool[nBlockSize] == 0) return false;
		if(pMPool[nBlockSize]->FreeMemory((void *)block_PTR))
		{
			if(nBlockSize < SBCNUM) 
			{
				if(SBCounter[nBlockSize] > 0) SBCounter[nBlockSize]--;
			}
			return true;
		}
		
		return false;
	}
	for(n=0;n<nMPoolClassesNum;n++)
	{
		if(pMPool[n] == 0) continue;
		if(pMPool[n]->IsInPool(block_PTR))
		{
			pMPool[n]->FreeMemory(block_PTR);

			nBlockSize = pMPool[n]->GetBlockSize();
			if(nBlockSize < SBCNUM) 
			{
				if(SBCounter[nBlockSize] > 0) SBCounter[nBlockSize]--;
			}
			return true;
		}
	}
	return false;
}

void MEMORY_SERVICE::Free(void * block_PTR)
{
	MEM_BLOCK * mb_PTR;
	MEM_BLOCK * link_L;
	MEM_BLOCK * link_R;

#ifdef SBPOOL
	if(bPoolOn)
	if(PoolFree(block_PTR)) return;
#endif

#ifdef 	NOMEMHEADER
	if(block_PTR)
	{
			free(block_PTR);
	}
	return;
#endif

	// decrement reference to memory block
	
	if(block_PTR == 0) 
	{
		return;
	}

	if((dword)block_PTR < Minimal_Pointer || (dword)block_PTR > Maximal_Pointer || Blocks <= 0) throw "MEMORY_SERVICE::Free : Invalid pointer";

	mb_PTR = (MEM_BLOCK *)((char *)block_PTR - sizeof(MEM_BLOCK));

	if(bCollectInfo) 
	{
		DWORD dwLine;
		char * pFileName;

		pFileName = cMemAddress.GetSource(mb_PTR->address,&dwLine);

		MemStat.Free(pFileName,dwLine,mb_PTR->data_size);
	}
	
	link_L = (MEM_BLOCK *)mb_PTR->link_L;
	link_R = (MEM_BLOCK *)mb_PTR->link_R;

	if(link_L) link_L->link_R = (char *)link_R;
	if(link_R) link_R->link_L = (char *)link_L;
	
	if(Mem_link == mb_PTR) Mem_link = link_L;

	if(mb_PTR->data_size < SBCNUM) 
	if(SBCounter[mb_PTR->data_size])
	{
		SBCounter[mb_PTR->data_size]--;
	}

	Allocated_memory_user	-= mb_PTR->data_size;
	Allocated_memory_system	-= sizeof(MEM_BLOCK);
	Blocks--;
	free(mb_PTR);	
	MemStat.nTestBlocks--;

	dwTotalMemory = Allocated_memory_system + Allocated_memory_user;
	dwTotalBlocks = Blocks;
}

void * MEMORY_SERVICE::Reallocate(void * block_PTR,long size)
{
	long data_size;
	long size_diff;
	MEM_BLOCK * old_mb_PTR;
	MEM_BLOCK * mb_PTR;
	MEM_BLOCK * link_L;
	MEM_BLOCK * link_R;
	char * data_PTR;
	char * old_data_PTR;

	if(block_PTR == null) return Allocate(size);

	// return null if arguments are invalid
	if(size <= 0)
	{
		if(block_PTR) Free(block_PTR);
		return null;
	}

#ifdef SBPOOL
	
	if(bPoolOn)
	{
		DWORD nBlockSize;
		nBlockSize = PoolFind(block_PTR);
		if(nBlockSize != 0xffffffff)
		{
			// if trying to change to the same size - just return same pointer
			if(nBlockSize == (DWORD)size) return block_PTR;
			void * pTemp;
			pTemp = PoolAllocate(size);
			if(pTemp == 0) pTemp = Allocate(size);
			if(pTemp == 0) SE_THROW("no mem");
			if((DWORD)size > nBlockSize) memcpy(pTemp,block_PTR,nBlockSize);
			else memcpy(pTemp,block_PTR,size);
			PoolFree(block_PTR,nBlockSize);
			return pTemp;
		}
	}
#endif

#ifdef 	NOMEMHEADER
	return realloc(block_PTR,size);
#endif

	data_PTR = (char *)((dword)block_PTR - sizeof(MEM_BLOCK));
	mb_PTR = (MEM_BLOCK *)data_PTR;
	data_size = mb_PTR->data_size;

	if (data_size == size) return block_PTR;

	if(bCollectInfo) 
	{
		DWORD dwLine;
		char * pFileName;
		pFileName = cMemAddress.GetSource(mb_PTR->address,&dwLine);
		MemStat.Resize(CodeSource.pFileName,CodeSource.line,size,
		pFileName,dwLine,mb_PTR->data_size);
	}
	
	// store information about old block
	old_data_PTR = data_PTR;
	old_mb_PTR = mb_PTR;
	link_L = (MEM_BLOCK *)mb_PTR->link_L;
	link_R = (MEM_BLOCK *)mb_PTR->link_R;
	size_diff = size - data_size;

	if(mb_PTR->data_size < SBCNUM) 
	if(SBCounter[mb_PTR->data_size])
	{
		SBCounter[mb_PTR->data_size]--;
	}
	if(size < SBCNUM) SBCounter[size]++;

	// realloc 
	data_PTR = (char *)realloc(data_PTR,size + sizeof(MEM_BLOCK));

	// pointer to reallocated block
	mb_PTR = (MEM_BLOCK *)data_PTR;

	if(data_PTR == null) 
	{
		// if failed
		// remove references from pointers list and update system info
		if(link_L) link_L->link_R = (char *)link_R;
		if(link_R) link_R->link_L = (char *)link_L;
		if(Mem_link == old_mb_PTR) Mem_link = link_L;

		Allocated_memory_user	-= data_size;
		Allocated_memory_system	-= sizeof(MEM_BLOCK);
		Blocks--;

		dwTotalMemory = Allocated_memory_system + Allocated_memory_user;
		dwTotalBlocks = Blocks;

		trace("cant resize  old : %d    new : %d   header : %d",data_size,size,sizeof(MEM_BLOCK));
		if(Exceptions_Mask & _X_NO_MEM) SE_THROW_MSG(_X_NO_MEM);

		return null;
	}

	if(old_data_PTR != data_PTR)
	{
		// if block change location in memory
		// update data in pointers list
		if(link_L) link_L->link_R = (char *)data_PTR;
		if(link_R) link_R->link_L = (char *)data_PTR;
		if(Mem_link == old_mb_PTR) Mem_link = mb_PTR;
	}

	// update size information
	mb_PTR->data_size = size;
	Allocated_memory_user += size_diff;

	dword result;
	result = (dword)data_PTR + sizeof(MEM_BLOCK);
	if(Minimal_Pointer > result) Minimal_Pointer = result;
	if(Minimal_Pointer == 0) Minimal_Pointer = result;

	if(Maximal_Pointer < result) Maximal_Pointer = result;

	if((dword)result == DBGMEMLINK)// - 0xff && (dword)data_PTR <= DBGMEMLINK + 0xff)// + sizeof(MEM_BLOCK))
	{
		result = result;
	}

	dwTotalMemory = Allocated_memory_system + Allocated_memory_user;
	dwTotalBlocks = Blocks;

	return (void *)((char *)data_PTR + sizeof(MEM_BLOCK));
	return null;
}


//bool MEMORY_SERVICE::ValidateBlock(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/)
long MEMORY_SERVICE::ValidateBlock(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/)
{
#ifdef	NOMEMHEADER
	return 1;
#endif

#ifdef SBPOOL
	if(PoolFind(block_PTR)) return 1;
#endif

	//return true;
	MEM_BLOCK * mb_PTR;
	if((dword)block_PTR < Minimal_Pointer || (dword)block_PTR > Maximal_Pointer || Blocks <= 0) return false;
	mb_PTR = (MEM_BLOCK *)((char *)block_PTR - sizeof(MEM_BLOCK));

	return 1;
}

// Free all allocated blocks : destructor or emergency situation
void MEMORY_SERVICE::GlobalFree() 
{
	DWORD n;
	for(n=0;n<SBCNUM;n++) SBCounter[n] = 0;

#ifdef SBPOOL
	bPoolOn = false;
	if(pMPool)
	{
		for(n=0;n<nMPoolClassesNum;n++)
		{
			if(pMPool[n]) delete pMPool[n];
		}
		delete pMPool; pMPool = 0;
		nMPoolClassesNum = 0;
	}
#endif
	bCollectInfo = false;
#ifdef	NOMEMHEADER
	return;
#endif
	DWORD dwLine;
	char * pFileName;
	BYTE * data = (BYTE *)Mem_link + sizeof(MEM_BLOCK);
	long dataSize = 0;
	
//	trace("GlobalFree()");
	
	while(Mem_link) 
	{
		try 
		{
			pFileName = cMemAddress.GetSource(Mem_link->address,&dwLine);
			trace("Leak: '%s' line %d, size %d",pFileName,dwLine,Mem_link->data_size);
		}
		catch(...)
		{
			dataSize = Mem_link->data_size;
			if(dataSize > 64) dataSize = 64;
			try
			{		
				pFileName = cMemAddress.GetSource(Mem_link->address,&dwLine);
				trace("Leak: '%s' line %d, size %d",pFileName,dwLine,Mem_link->data_size);

				char buf[512];
				buf[0] = 0;
				for(int i = 0, j = 0; i < dataSize; i++)
				{
					if(data[i]) sprintf(buf + strlen(buf), "%.2x('%c') ", data[i], data[i]);
						else sprintf(buf + strlen(buf), "%.2x(   ) ", data[i]);
					if(j >= 7)
					{
						trace("       %s", buf);
						j = 0;
						buf[0] = 0;
					}else j++;
				}
				if(buf[0]) trace("       %s", buf);
			}catch(...)
			{
				trace("Leak: 'unknown' , size %d, invalide data pointer: 0x%x",Mem_link->data_size, data);
			}
			
		}
		Free((char *)Mem_link + sizeof(MEM_BLOCK));
		if(Blocks <= 0 && Mem_link != null) throw "MBLFREE_ERROR";	// ... ; fatal, close programm
	}
	Minimal_Pointer = null;
	Maximal_Pointer = null;
}

dword MEMORY_SERVICE::GetBlockSize(void * block_PTR)
{
	return 0;
#ifdef	NOMEMHEADER
	return 0;
#endif

	MEM_BLOCK * mb_PTR;
	if((dword)block_PTR < Minimal_Pointer || (dword)block_PTR > Maximal_Pointer || Blocks <= 0) throw "MEMORY_SERVICE::GetBlockSize : Invalid pointer";

	mb_PTR = (MEM_BLOCK *)((char *)block_PTR - sizeof(MEM_BLOCK));
	return mb_PTR->data_size;

}

void * MEMORY_SERVICE::GetFirstPointer()
{
#ifdef	NOMEMHEADER
	return 0;
#endif

	Search_Pointer = (void *)((char *)Mem_link + sizeof(MEM_BLOCK));
	return Search_Pointer;
}

void * MEMORY_SERVICE::GetNextPointer()
{
#ifdef	NOMEMHEADER
	return 0;
#endif

	MEM_BLOCK * mb_PTR;
	if(!ValidateBlock(Search_Pointer/*,0*/)) return null;
	mb_PTR = (MEM_BLOCK *)((char *)Search_Pointer - sizeof(MEM_BLOCK));
	if(mb_PTR->link_L == null) Search_Pointer = null;
	else Search_Pointer = (void *)((char *)mb_PTR->link_L +  sizeof(MEM_BLOCK));
	return Search_Pointer;

}

//bool MEMORY_SERVICE::GetPointerState(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/)
long MEMORY_SERVICE::GetPointerState(void * block_PTR/*,MEM_EXE_STATE * lpBlockState*/)
{
	return 0;
}

void MEMORY_SERVICE::ProcessMemProfile(char * pFileName)
{
#ifdef SBPOOL
	HANDLE fh;
	DWORD nProfileValue;
	DWORD dwR,n;
	MEMPOOL * * pTempMPool;

	nMPoolClassesNum = 0;
	pMPool = 0;
	bPoolOn = false;

	if(!pFileName) return;
	fh = api->fio->_CreateFile(pFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	if(fh == INVALID_HANDLE_VALUE) return;

	api->fio->_ReadFile(fh,&nMPoolClassesNum,sizeof(DWORD),&dwR);
	if(dwR != sizeof(DWORD)) return;
	
	if(nMPoolClassesNum >= 0xff)
	{
		api->fio->_CloseHandle(fh);
		trace("ERROR: invalid memory profile data '%s'",pFileName);
		return;
	}
	
	pTempMPool = (MEMPOOL * *)new char[nMPoolClassesNum * sizeof(MEMPOOL *)];
	
	for(n=0;n<nMPoolClassesNum;n++)
	{
		api->fio->_ReadFile(fh,&nProfileValue,sizeof(DWORD),&dwR);
		if(dwR != sizeof(DWORD))
		{
			trace("Error reading memory profile");
			if(pTempMPool) delete pTempMPool;
			return;
		}
		if(n == 0)
		{
			pTempMPool[n] = 0;
			continue;
		}
		if(nProfileValue == 0) 
		{
			pTempMPool[n] = 0;
			continue;
		}
		pTempMPool[n] = new MEMPOOL(n,nProfileValue);
	}
	api->fio->_CloseHandle(fh);

	pMPool = pTempMPool;
	bPoolOn = true;

#endif
}

char * MEMORY_SERVICE::GetFileName(void * pMemory)
{
#ifdef NOMEMHEADER
		return "unknown";
#else
	MEM_BLOCK * mb_PTR;
#ifdef SBPOOL
	if(PoolFind(pMemory)) return "unknown";
#endif
	mb_PTR = (MEM_BLOCK *)((char *)pMemory - sizeof(MEM_BLOCK));

	return cMemAddress.GetSource(mb_PTR->address,0);
#endif
}

dword MEMORY_SERVICE::GetFileLineCode(void * pMemory)
{
#ifdef NOMEMHEADER
		return 0;
#else
	MEM_BLOCK * mb_PTR;
#ifdef SBPOOL
		if(PoolFind(pMemory)) return 0;
#endif
	mb_PTR = (MEM_BLOCK *)((char *)pMemory - sizeof(MEM_BLOCK));

	DWORD dwLine;
	if(cMemAddress.GetSource(mb_PTR->address,&dwLine))
	{
		return dwLine;
	}
	return 0;
#endif
}

void MEMORY_SERVICE::DumpMemoryState()
{
	DWORD n;
	trace("[ ---- MEMORY STATE ----]");
#ifdef SBPOOL
	DWORD dwTotalPoolMem = 0;
	DWORD dwTotalPoolUsedMem = 0;
	DWORD dwTotalPoolBlocks = 0;
	DWORD dwTotalPoolUsedBlocks = 0;
	DWORD dwTotalMissed = 0;

	if(bPoolOn)
	{
		trace("Mem Pools: %d",nMPoolClassesNum);
		for(n=0;n<nMPoolClassesNum;n++)
		{
			if(!pMPool[n]) continue;
			if(pMPool[n]->nBlocksNum == 0) continue;
			trace("Pool : [%d] bytes",pMPool[n]->nBlockSize);
			trace("     : blocks  [%d of %d]   memory  [%.3fkb of %.3fkb]",pMPool[n]->nUsedBlocks,pMPool[n]->nBlocksNum,
				pMPool[n]->nUsedBlocks*(pMPool[n]->nBlockSize + POOLHEADERSIZE)/1024.f,
				pMPool[n]->nBlocksNum * (pMPool[n]->nBlockSize + POOLHEADERSIZE)/1024.f);
			trace("     : usage %.2f%%    missed %d",pMPool[n]->nUsedBlocks*100.f/pMPool[n]->nBlocksNum,
				pMPool[n]->nMissed);

			dwTotalPoolMem += pMPool[n]->nBlocksNum * (pMPool[n]->nBlockSize + POOLHEADERSIZE);
			dwTotalPoolUsedMem += pMPool[n]->nUsedBlocks*(pMPool[n]->nBlockSize + POOLHEADERSIZE);
			dwTotalPoolBlocks += pMPool[n]->nBlocksNum;
			dwTotalPoolUsedBlocks += pMPool[n]->nUsedBlocks;
			dwTotalMissed += pMPool[n]->nMissed;
			
		}
		trace("Total:");
		trace("     : blocks  [%d of %d]   memory  [%.3fkb of %.3fkb]",
			dwTotalPoolUsedBlocks,dwTotalPoolBlocks,
			dwTotalPoolUsedMem/1024.f,dwTotalPoolMem/1024.f);
	
		if(dwTotalPoolMem) trace("     : usage %.2f%%   missed %d",dwTotalPoolUsedMem*100.f/dwTotalPoolMem,dwTotalMissed);
	}
#endif

	DWORD dwMemoryTotal;
	DWORD dwBlocksTotal;

	MEM_BLOCK * pM;
	pM = Mem_link;

	dwMemoryTotal = 0;
	dwBlocksTotal = 0;
	while(pM)
	{
		dwBlocksTotal++;
		dwMemoryTotal += (pM->data_size + sizeof(MEM_BLOCK));
		pM = (MEM_BLOCK *)pM->link_R;
		
	}
	trace("Allocated memory:");
	trace("  Pool          : %.3fkb   in %d blocks", dwTotalPoolMem/1024.f, dwTotalPoolBlocks);
	trace("  Common        : %.3fkb   in %d blocks", dwMemoryTotal/1024.f, dwBlocksTotal);
	trace("  Common + Pool : %.3fkb   ", (dwTotalPoolMem + dwMemoryTotal)/1024.f);

	DWORD dwBlocks;
	DWORD dwMem;
	DWORD dwTLine;
	char * pTFileName;

	for(n=0;n<cMemAddress.dwRecordsNum;n++)
	{
		dwBlocks = 0;
		dwMem = 0;
		pM = Mem_link;
		while(pM)
		{
			if(n == pM->address)
			{
				dwMemoryTotal += pM->data_size;
				dwMem += pM->data_size;
				dwBlocks++;
			}
			pM = (MEM_BLOCK *)pM->link_L;
		}

		if(dwBlocks)
		{
			trace("MEMUSER [%s, %d] : \n memory  %.3f kb in %d blocks",cMemAddress.pData[n].pFileName,cMemAddress.pData[n].dwLine,
				dwMem/1024.f,dwBlocks);
		}
	}
	trace("Blocks header Used: %.3f kb",dwBlocksTotal*sizeof(MEM_BLOCK)/1024.f);
	trace("TotalMemory Used: %.3f kb",(dwMemoryTotal + dwBlocksTotal*sizeof(MEM_BLOCK))/1024.f);

	trace("[---- mem state end ----]");//*/
//#endif	
}
