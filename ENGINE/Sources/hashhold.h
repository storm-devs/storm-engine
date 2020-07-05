#ifndef _HASHHOLD_H_
#define _HASHHOLD_H_

#include "..\..\COMMON_H\vmodule_api.h"

class HASHHOLD
{
	DWORD * * pHashTable;
	DWORD * pLineSize;
	DWORD dwHashTableSize;
public:
	 HASHHOLD();
	~HASHHOLD();

	void  Release();
	bool  Init(DWORD dwHashTableSize);
	bool  Add(DWORD dwHashValue, DWORD dwHoldValue);
	DWORD GetCodesNum(DWORD dwHashValue,DWORD * & pHashLine);
	DWORD MakeHash(const char * string, bool bCaseIndependent = false);
	
};

#endif