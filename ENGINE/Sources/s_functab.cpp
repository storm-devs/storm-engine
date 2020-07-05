#include "s_functab.h"
#include <string.h>

/*DWORD __cdecl testfunc(VS_STACK * ps)
{
	return 7;
}*/

#define MAKEHASHINDEX(x)	(x&0xff)

S_FUNCTAB::S_FUNCTAB()
{
	pTable = 0;
	Buffer_size = 0;
	Func_num = 0;
	//bKeepName = false;
	DWORD n;
	for(n=0;n<HASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		HashLine[n].pElements = 0;
	}
}

S_FUNCTAB::~S_FUNCTAB()
{
	Release();
}

void  S_FUNCTAB::Release()
{
	dword n,i;
	if(pTable) 
	{
		for(n=0;n<Func_num;n++)	
		{ 
			if(pTable[n].name) delete pTable[n].name;	
			if(pTable[n].pLocal)
			{
				for(i=0;i<pTable[n].var_num;i++)
				{
					if(pTable[n].pLocal[i].name) delete pTable[n].pLocal[i].name;
				}
				delete pTable[n].pLocal;
			}
			if(pTable[n].decl_file_name) delete pTable[n].decl_file_name;
			pTable[n].decl_file_name = 0;
		}
		delete pTable; pTable = 0;
	}
	Buffer_size = 0;
	Func_num = 0;
	for(n=0;n<HASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		if(HashLine[n].pElements)  delete HashLine[n].pElements;
		HashLine[n].pElements = 0;
	}
}

bool S_FUNCTAB::GetFunc(FUNCINFO& fi,dword func_code)
{
	if(func_code >= Func_num) return false;

	if(pTable[func_code].segment_id == IMPORTED_SEGMENT_ID)
	{
		if(pTable[func_code].pImportedFunc == 0) 
		{
			return false;
		}
		fi = pTable[func_code];
		return true;
	}
	if(pTable[func_code].offset == INVALID_FUNC_OFFSET)
	{
		return false;
	}
	fi = pTable[func_code];
	return true;
}

bool S_FUNCTAB::GetFuncX(FUNCINFO& fi,dword func_code)
{
	if(func_code >= Func_num) return false;
	fi = pTable[func_code];
	return true;
}

dword S_FUNCTAB::AddFunc(FUNCINFO& fi)
{
	dword n;
	dword hash;
	DWORD hash_index;

	if(fi.name == 0) return INVALID_FUNC_CODE;
	hash = MakeHashValue(fi.name);
	hash_index = MAKEHASHINDEX(hash);
	
	for(n=0;n<Func_num;n++)
	{
		if(pTable[n].hash != hash) continue;
		if(stricmp(pTable[n].name,fi.name)!=0) continue;
		
		// function with such name already registred,
		if(pTable[n].offset == INVALID_FUNC_OFFSET)
		{
			// but offset isnt set (function segment unloaded)
			// - set function segment and offset info
			
			pTable[n].fTimeUsage = 0;
			pTable[n].nNumberOfCalls = 0;
			pTable[n].offset = fi.offset;
			pTable[n].segment_id = fi.segment_id;
			pTable[n].decl_line = fi.decl_line;
			pTable[n].pImportedFunc = fi.pImportedFunc;
			
			if(pTable[n].decl_file_name) delete pTable[n].decl_file_name;

			pTable[n].decl_file_name = NEW char[strlen(fi.decl_file_name) + 1];
			strcpy(pTable[n].decl_file_name,fi.decl_file_name);
			pTable[n].code = n;
			UpdateHashTable(n,hash,true);
			return n;
		}
		else
		{
			// and already exist
			// this is 'double function name' error 
			// (possible becose hash function error), user must rename function
			return INVALID_FUNC_CODE;
		}
		
	}
	// function not found, add anew one
	// adjust buffer size
	if(Func_num >= Buffer_size)
	{
		Buffer_size += FUNC_BUFFER_BLOCK_SIZE;
		pTable = (FUNCINFO *)RESIZE(pTable,Buffer_size*sizeof(FUNCINFO));
	}
	pTable[Func_num] = fi;
	pTable[Func_num].hash = hash;
	pTable[Func_num].name = 0;
	pTable[Func_num].arguments = fi.arguments;
	pTable[Func_num].ext_args = 0;
	pTable[Func_num].var_num = 0;
	pTable[Func_num].pLocal = 0;
	pTable[Func_num].decl_line = fi.decl_line;
	pTable[Func_num].fTimeUsage = 0;
	pTable[Func_num].nNumberOfCalls = 0;

	pTable[Func_num].decl_file_name = NEW char[strlen(fi.decl_file_name) + 1];
	strcpy(pTable[Func_num].decl_file_name,fi.decl_file_name);
	pTable[Func_num].code = Func_num;
	pTable[Func_num].pImportedFunc = fi.pImportedFunc;

	UpdateHashTable(Func_num,hash,true);


/*	DWORD a;
	pTable[Func_num].pImportedFunc = testfunc;
	a = pTable[Func_num].pImportedFunc(0);
*/
	if(true)//bKeepName)
	{
		if(fi.name)
		{

			pTable[Func_num].name = NEW char[strlen(fi.name) + 1];
			strcpy(pTable[Func_num].name,fi.name);
		}
	}
	Func_num++;

	return (Func_num - 1);
}

dword S_FUNCTAB::MakeHashValue(const char * string)
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

void S_FUNCTAB::InvalidateBySegmentID(dword segment_id)
{
	dword n,i;
	for(n=0;n<Func_num;n++)
	{
		if(pTable[n].segment_id != segment_id) continue;
		UpdateHashTable(n,pTable[n].hash,false);
		pTable[n].offset = INVALID_FUNC_OFFSET;
		if(pTable[n].pLocal)
		{
			for(i=0;i<pTable[n].var_num;i++)
			{
				if(pTable[n].pLocal[i].name) delete pTable[n].pLocal[i].name;
			}
			delete pTable[n].pLocal;
		}
		pTable[n].pLocal = 0;
		pTable[n].var_num = 0;
		pTable[n].arguments = 0;
		if(pTable[n].decl_file_name) delete pTable[n].decl_file_name;
		pTable[n].decl_file_name = 0;
		
	}
}

void S_FUNCTAB::InvalidateFunction(dword nFuncHandle)
{
	dword n,i;
	
	if(nFuncHandle < Func_num)
	{
		n = nFuncHandle;
		UpdateHashTable(n,pTable[n].hash,false);
		pTable[n].offset = INVALID_FUNC_OFFSET;
		if(pTable[n].pLocal)
		{
			for(i=0;i<pTable[n].var_num;i++)
			{
				if(pTable[n].pLocal[i].name) delete pTable[n].pLocal[i].name;
			}
			delete pTable[n].pLocal;
		}
		pTable[n].pLocal = 0;
		//pTable[n].var_num = 0;
		//pTable[n].arguments = 0;
		if(pTable[n].decl_file_name) delete pTable[n].decl_file_name;
		pTable[n].decl_file_name = 0;
		pTable[n].pImportedFunc = 0;
	}
}

dword S_FUNCTAB::FindFunc(char * func_name)
{
	dword n;
	dword hash;
	if(func_name == 0) return INVALID_FUNC_CODE;
	hash = MakeHashValue(func_name);

	DWORD hash_index,nNum,ni;
	hash_index = MAKEHASHINDEX(hash);
	nNum = HashLine[hash_index].nNumElements;
	for(n=0;n<nNum;n++)
	{
		ni = HashLine[hash_index].pElements[n];
		if(pTable[ni].hash == hash) 
		if(stricmp(pTable[ni].name,func_name)== 0) return ni;
	}

/*	for(n=0;n<Func_num;n++)
	{
		if(pTable[n].hash == hash) 
		if(stricmp(pTable[n].name,func_name)== 0) return n;
	}
*/
	return INVALID_FUNC_CODE;
}

bool S_FUNCTAB::SetFuncOffset(char * func_name, dword offset)
{
	DWORD n;
	n = FindFunc(func_name);
	if(n == INVALID_FUNC_CODE) return false;
	pTable[n].offset = offset;
	return true;
/*
	dword n;
	dword hash;
	hash = MakeHashValue(func_name);
	for(n=0;n<Func_num;n++)
	{
		if(pTable[n].hash == hash) 
		{
			pTable[n].offset = offset;
			return true;
		}
	}
	return false;
*/
}

bool S_FUNCTAB::AddFuncVar(dword func_code, LVARINFO & lvi)
{
	dword vindex;
	dword hash;
	dword n;
	if(func_code >= Func_num) return false;
	if(lvi.name == 0) return false;
	
	hash = MakeHashValue(lvi.name);
	for(n=0;n<pTable[func_code].var_num;n++)
	{
		if(hash == pTable[func_code].pLocal[n].hash) //return false;
		if(stricmp(pTable[func_code].pLocal[n].name,lvi.name)==0) return false;
	}

	vindex = pTable[func_code].var_num;
	pTable[func_code].var_num++;
	pTable[func_code].pLocal = (LVARINFO *)RESIZE(pTable[func_code].pLocal,pTable[func_code].var_num*sizeof(LVARINFO));
	pTable[func_code].pLocal[vindex].name = 0;
	if(true)//bKeepName)
	{

		pTable[func_code].pLocal[vindex].name = NEW char[strlen(lvi.name)+1];
		strcpy(pTable[func_code].pLocal[vindex].name,lvi.name);
	}
	pTable[func_code].pLocal[vindex].bArray = lvi.bArray;
	pTable[func_code].pLocal[vindex].hash = hash;
	pTable[func_code].pLocal[vindex].type = lvi.type;
	pTable[func_code].pLocal[vindex].elements = lvi.elements;
	return true;
}

bool S_FUNCTAB::AddFuncArg(dword func_code, LVARINFO & lvi, bool bExt)
{
	if(func_code >= Func_num) return false;
	if(bExt)
	{
		pTable[func_code].ext_args++;
		return true;
	}
	pTable[func_code].arguments++;
	return AddFuncVar(func_code,lvi);
}

dword S_FUNCTAB::FindVar(dword func_code, char * var_name)
{
	dword hash;
	dword n;
	if(var_name == 0) return INVALID_VAR_CODE;
	if(func_code >= Func_num) return INVALID_VAR_CODE;
	hash = MakeHashValue(var_name);
	for(n=0;n<pTable[func_code].var_num;n++)
	{
		if(hash == pTable[func_code].pLocal[n].hash) //return n;
		if(stricmp(var_name,pTable[func_code].pLocal[n].name)==0) return n;
	}
	return INVALID_VAR_CODE;
}

bool S_FUNCTAB::GetVar(LVARINFO & lvi, dword func_code, dword var_code)
{
	if(func_code >= Func_num) return false;
	if(var_code >= pTable[func_code].var_num) return false;
	lvi = pTable[func_code].pLocal[var_code];
	return true;
}

void S_FUNCTAB::AddTime(dword func_code, DWORD time)
{
	if(func_code >= Func_num) return;
	pTable[func_code].fTimeUsage += time;
}

void S_FUNCTAB::SetTimeUsage(dword func_code, double f)
{
	if(func_code >= Func_num) return;
	pTable[func_code].fTimeUsage = f;
}

void  S_FUNCTAB::AddCall(dword func_code)
{
	if(func_code >= Func_num) return;
	pTable[func_code].nNumberOfCalls++;
}

void S_FUNCTAB::UpdateHashTable(DWORD code, DWORD hash, bool in)
{
	DWORD n;
	DWORD hash_index;
	hash_index = MAKEHASHINDEX(hash);

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