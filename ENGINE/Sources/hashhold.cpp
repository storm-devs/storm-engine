#include "hashhold.h"

HASHHOLD::HASHHOLD()
{
	pHashTable = 0;
	pLineSize = 0;
	dwHashTableSize = 0;
}

HASHHOLD::~HASHHOLD()
{
	Release();
}

void HASHHOLD::Release()
{
	DWORD n;
	if(pHashTable)
	{
		for(n=0;n<dwHashTableSize;n++)
		{
			if(pHashTable[n]) delete pHashTable[n];
		}
		delete pHashTable;
		pHashTable = 0;
	}
	if(pLineSize) delete pLineSize; pLineSize = 0;
	dwHashTableSize = 0;
}

bool HASHHOLD::Init(DWORD _dwHashTableSize)
{
	Release();
	dwHashTableSize = _dwHashTableSize;
	pHashTable = (DWORD **)NEW char[dwHashTableSize * sizeof(DWORD*)];
	if(!pHashTable) THROW;
	memset(pHashTable,0,dwHashTableSize * sizeof(DWORD*));
	pLineSize = (DWORD *)NEW char[dwHashTableSize * sizeof(DWORD)];
	if(!pLineSize) THROW;
	memset(pLineSize,0,dwHashTableSize * sizeof(DWORD));
	return true;
}

bool HASHHOLD::Add(DWORD dwHashValue, DWORD dwHoldValue)
{
	DWORD n;
	if(dwHashValue >= dwHashTableSize)
	{
		api->Trace("ERROR: hash value out of range");
		return false;
	}
	n = pLineSize[dwHashValue];
	pLineSize[dwHashValue]++;
	pHashTable[dwHashValue] = (DWORD *)RESIZE(pHashTable[dwHashValue],pLineSize[dwHashValue] * sizeof(DWORD));
	pHashTable[dwHashValue][n] = dwHoldValue;
	return true;
}

DWORD HASHHOLD::GetCodesNum(DWORD dwHashValue,DWORD * & pHashLine)
{
	pHashLine = 0;
	if(dwHashValue >= dwHashTableSize)
	{
		api->Trace("ERROR: request for hash value out of range");
		return 0;
	}
	pHashLine = pHashTable[dwHashValue];
	return pLineSize[dwHashValue];
}

DWORD HASHHOLD::MakeHash(const char * string, bool bCaseIndependent)
{
	DWORD hval = 0;
	DWORD g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if(bCaseIndependent)
		{
			if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		}
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
