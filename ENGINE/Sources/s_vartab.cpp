#include "..\..\common_h\memop.h"
#include "s_vartab.h"
//#include <string.h>
#include "system_log.h"

#define VTMAKEHASHINDEX(x)	(x&0xff)

S_VARTAB::S_VARTAB()
{
	pTable = 0;
	Buffer_size = 0;
	Var_num = 0;
//	bKeepName = false;
	Global_var_num = 0;
	DWORD n;
	for(n=0;n<VTHASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		HashLine[n].pElements = 0;
	}
}

S_VARTAB::~S_VARTAB()
{
	Release();
}

void  S_VARTAB::Release()
{
	dword n;
	if(pTable) 
	{
		for(n=0;n<Var_num;n++)	
		{ 
			if(pTable[n].pDClass) delete pTable[n].pDClass;
			if(pTable[n].name) delete pTable[n].name;	
		}
		delete pTable; pTable = 0;
	}
	Buffer_size = 0;
	Var_num = 0;
	for(n=0;n<VTHASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		if(HashLine[n].pElements)  delete HashLine[n].pElements;
		HashLine[n].pElements = 0;
	}
}

bool S_VARTAB::GetVar(VARINFO& vi,dword var_code)
{
	if(var_code >= Var_num) return false;
	if(pTable[var_code].segment_id == INVALID_SEGMENT_ID) return false;
	vi = pTable[var_code];
	return true;
}

bool S_VARTAB::GetVarX(VARINFO& vi,dword var_code)
{
	if(var_code >= Var_num) return false;
	vi = pTable[var_code];
	return true;
}

dword S_VARTAB::AddVar(VARINFO& vi)
{
	dword n;
	dword hash;

	if(vi.name == 0) return INVALID_VAR_CODE;

	//trace("%s : %d",vi.name,(long)vi.bArray);

	hash = MakeHashValue(vi.name);
	
	for(n=0;n<Var_num;n++)
	{
		if(pTable[n].hash == hash)
		if(stricmp(pTable[n].name,vi.name)==0)
		{
			// variable with such name already registred,
			if(pTable[n].segment_id == INVALID_SEGMENT_ID)
			{
				UpdateHashTable(n,hash,true);
				// but segment is unloaded
				pTable[n].segment_id = vi.segment_id;
				
				// if the same type and dimension - just return code
				if(pTable[n].type == vi.type && pTable[n].elements == vi.elements) return n;

				// name the same, but type or dimension different - recreate (keep old name and hash)
				pTable[n].elements = vi.elements;
				pTable[n].type = vi.type;
				if(pTable[n].pDClass) delete pTable[n].pDClass;
				
				pTable[n].pDClass = NEW DATA;
				pTable[n].pDClass->SetVCompiler(pVCompiler);
				pTable[n].pDClass->SetType(vi.type,vi.elements);

				return n;
			}
			else
			{
				// and already exist
				// this is 'double variable name' error 
				// (possible becose hash function error), user must rename variable
				return INVALID_VAR_CODE;
			}
		}
	}
	// function not found, add anew one
	// adjust buffer size
	if(Var_num >= Buffer_size)
	{
		Buffer_size += VAR_BUFFER_BLOCK_SIZE;
		pTable = (VARINFO *)RESIZE(pTable,Buffer_size*sizeof(VARINFO));
	}
	//pTable[Var_num] = vi;
	pTable[Var_num].bArray = vi.bArray;
	pTable[Var_num].segment_id = vi.segment_id;
	pTable[Var_num].elements = vi.elements;
	pTable[Var_num].type = vi.type;
	pTable[Var_num].hash = hash;
	pTable[Var_num].name = 0;
	
	pTable[Var_num].pDClass = NEW DATA;
	pTable[Var_num].pDClass->SetVCompiler(pVCompiler);
	pTable[Var_num].pDClass->SetType(vi.type,vi.elements);
	pTable[Var_num].pDClass->SetGlobalVarTableIndex(Var_num);
	
	UpdateHashTable(Var_num,hash,true);
	
	if(true)//bKeepName)
	{
		if(vi.name)
		{
			
			pTable[Var_num].name = NEW char[strlen(vi.name) + 1];
			strcpy(pTable[Var_num].name,vi.name);
		}
	}
	Var_num++;
	
	return (Var_num - 1);
}

dword S_VARTAB::MakeHashValue(const char * string)
{
	dword hval = 0;
	dword g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		hval = (hval<<4) + (unsigned long int)v;
		g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
	}
	return hval;
}

void S_VARTAB::InvalidateBySegmentID(dword segment_id)
{
	dword n;
	for(n=0;n<Var_num;n++)
	{
		if(pTable[n].segment_id != segment_id) continue;
		pTable[n].segment_id = INVALID_SEGMENT_ID;
		UpdateHashTable(n,pTable[n].hash,false);
	}
}

dword S_VARTAB::FindVar(const char * var_name)
{
	DWORD hash_index,n,hash,ni;
	if(var_name == 0) return INVALID_VAR_CODE;
	hash = MakeHashValue(var_name);
	hash_index = VTMAKEHASHINDEX(hash);
	for(n=0;n<HashLine[hash_index].nNumElements;n++)
	{
		ni = HashLine[hash_index].pElements[n];
		if(pTable[ni].hash == hash) //return n;
		if(stricmp(pTable[ni].name,var_name)==0) return ni;
	}
	return INVALID_VAR_CODE;
	/*

	dword n;
	dword hash;
	if(var_name == 0) return INVALID_VAR_CODE;
	hash = MakeHashValue(var_name);
	for(n=0;n<Var_num;n++)
	{
		if(pTable[n].hash == hash) //return n;
		if(stricmp(pTable[n].name,var_name)==0) return n;
	}
	return INVALID_VAR_CODE;*/
}

bool S_VARTAB::ArraySizeChanged(dword nIndex,dword nNewSize)
{
	if(nIndex >= Var_num) return false;
	if(!pTable[nIndex].pDClass->IsArray()) 
	{
		//trace("ERROR: invalid variable type for 'ChangeArraySize(?)' function");
		return false;
	}
	pTable[nIndex].elements = nNewSize;
	return true;
}

void S_VARTAB::UpdateHashTable(DWORD code, DWORD hash, bool in)
{
	DWORD n;
	DWORD hash_index;
	hash_index = VTMAKEHASHINDEX(hash);

	for(n=0;n<HashLine[hash_index].nNumElements;n++)
	{
		if(HashLine[hash_index].pElements[n] != code) continue;
		if(!in)
		{
			// take element out of list
			HashLine[hash_index].pElements[n] = HashLine[hash_index].pElements[HashLine[hash_index].nNumElements - 1];
			HashLine[hash_index].nNumElements--;
			HashLine[hash_index].pElements = (DWORD *)RESIZE(HashLine[hash_index].pElements,HashLine[hash_index].nNumElements * sizeof(DWORD));
			return;
		}
		else return;	// ok, already in list (? possible)
	}
	// not in list - add
	HashLine[hash_index].nNumElements++;
	HashLine[hash_index].pElements = (DWORD *)RESIZE(HashLine[hash_index].pElements,HashLine[hash_index].nNumElements * sizeof(DWORD));
	HashLine[hash_index].pElements[HashLine[hash_index].nNumElements - 1] = code;
}