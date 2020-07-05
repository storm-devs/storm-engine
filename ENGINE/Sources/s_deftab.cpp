#include "s_DEFTAB.h"
#include <string.h>

#define DTMAKEHASHINDEX(x)	(x&0xff)

S_DEFTAB::S_DEFTAB()
{
	pTable = 0;
	Buffer_size = 0;
	Def_num = 0;
//	bKeepName = false;
	DWORD n;
	for(n=0;n<DTHASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		HashLine[n].pElements = 0;
	}

}

S_DEFTAB::~S_DEFTAB()
{
	Release();
}

void  S_DEFTAB::Release()
{
	dword n;
	if(pTable) 
	{
		for(n=0;n<Def_num;n++)	
		{ 
			if(pTable[n].name) delete pTable[n].name;	
			if(pTable[n].deftype == STRING) 
			{
				if(pTable[n].data4b != 0) delete ((char *)pTable[n].data4b);
			}
		}
		delete pTable; pTable = 0;
	}
	Buffer_size = 0;
	Def_num = 0;
	for(n=0;n<DTHASHT_SIZE;n++)
	{
		HashLine[n].nNumElements = 0;
		if(HashLine[n].pElements)  delete HashLine[n].pElements;
		HashLine[n].pElements = 0;
	}

}

bool S_DEFTAB::GetDef(DEFINFO& di,dword def_code)
{
	if(def_code >= Def_num) return false;
	if(pTable[def_code].segment_id == INVALID_SEGMENT_ID) return false;
	di = pTable[def_code];
	return true;
}

bool S_DEFTAB::GetDefX(DEFINFO& di,dword def_code)
{
	if(def_code >= Def_num) return false;
	di = pTable[def_code];
	return true;
}

dword S_DEFTAB::AddDef(DEFINFO& di)
{
	dword n;
	dword hash;


	if(di.name == 0) return INVALID_DEF_CODE;
	hash = MakeHashValue(di.name);
	
	for(n=0;n<Def_num;n++)
	{
		if(pTable[n].hash == hash)
		if(stricmp(pTable[n].name,di.name)==0)
		{
			// define with such name already registred,
			if(pTable[n].segment_id == INVALID_SEGMENT_ID)
			{
				// but not used
				pTable[n].data4b = di.data4b;
				pTable[n].deftype = di.deftype;
				pTable[n].segment_id = di.segment_id;
				UpdateHashTable(n,hash,true);
				
				return n;
			}
			else
			{
				// and already exist
				// this is 'define redefinition' error 
				return INVALID_DEF_CODE;
			}
		}
	}
	// function not found, add anew one
	// adjust buffer size
	if(Def_num >= Buffer_size)
	{
		Buffer_size += DEF_BUFFER_BLOCK_SIZE;
		pTable = (DEFINFO *)RESIZE(pTable,Buffer_size*sizeof(DEFINFO));
	}
	pTable[n].data4b = di.data4b;
	pTable[n].deftype = di.deftype;
	pTable[n].segment_id = di.segment_id;
	//pTable[Def_num] = di;
	pTable[Def_num].hash = hash;
	pTable[Def_num].name = 0;
	UpdateHashTable(Def_num,hash,true);
	if(true)//bKeepName)
	{
		if(di.name)
		{

			pTable[Def_num].name = NEW char[strlen(di.name) + 1];
			strcpy(pTable[Def_num].name,di.name);
		}
	}
	Def_num++;

	return (Def_num - 1);
}

dword S_DEFTAB::MakeHashValue(const char * string)
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

void S_DEFTAB::InvalidateBySegmentID(dword segment_id)
{
	dword n;
	for(n=0;n<Def_num;n++)
	{
		if(pTable[n].segment_id != segment_id) continue;
		UpdateHashTable(n,pTable[n].hash,false);
		pTable[n].segment_id = INVALID_SEGMENT_ID;
		if(pTable[n].deftype == STRING)
		{
			if(pTable[n].data4b)
			{
				delete ((char *)pTable[n].data4b);
			}
			pTable[n].data4b = 0;
		}
	}
}

dword S_DEFTAB::FindDef(char * def_name)
{
	dword n;
	dword hash;
	if(def_name == 0) return INVALID_DEF_CODE;
	hash = MakeHashValue(def_name);
	
	DWORD hash_index,ni;
	hash_index = DTMAKEHASHINDEX(hash);
	for(n=0;n<HashLine[hash_index].nNumElements;n++)
	{
		ni = HashLine[hash_index].pElements[n];
		if(pTable[ni].hash == hash) //return n;
		if(stricmp(pTable[ni].name,def_name)==0) return ni;
	}
	return INVALID_DEF_CODE;

	/*
	for(n=0;n<Def_num;n++)
	{
		if(pTable[n].hash == hash) return n;
	}
	return INVALID_DEF_CODE;*/
}

void S_DEFTAB::UpdateHashTable(DWORD code, DWORD hash, bool in)
{
	DWORD n;
	DWORD hash_index;
	hash_index = DTMAKEHASHINDEX(hash);

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