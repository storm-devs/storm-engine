#ifndef _STRING_CODEC_H_
#define _STRING_CODEC_H_

#include "..\..\common_h\vstring_codec.h"
#define HASH_TABLE_SIZE	512	// must be power of 2

struct HTSUBELEMENT
{
	char * pStr;
	dword dwHashCode;
};

struct HTELEMENT
{
	HTSUBELEMENT * pElements;
	dword nStringsNum;
};

class STRING_CODEC : public VSTRING_CODEC
{
	dword nHTIndex;
	dword nHTEIndex;
	dword nStringsNum;

	HTELEMENT HTable[HASH_TABLE_SIZE];
public:
	STRING_CODEC()
	{ 
		nStringsNum = 0;
		ZeroMemory(HTable, sizeof(HTable));
	};

	~STRING_CODEC() { Release(); };
	
	dword GetNum() { return nStringsNum; };

	void Release()
	{
		dword n, m;
		for (m=0; m<HASH_TABLE_SIZE; m++)
		{
			if (HTable[m].pElements)
			{
				for (n=0; n<HTable[m].nStringsNum; n++) delete HTable[m].pElements[n].pStr;
				delete HTable[m].pElements;
			}
			HTable[m].pElements = null; 
			HTable[m].nStringsNum = 0;
		}
	}
	
	dword Convert(const char * pString, long iLen)
	{
		if (pString == 0) return 0xffffffff;

		char cTemp[1024];
		strncpy(cTemp, pString, iLen);
		cTemp[iLen] = 0;

		bool bNew;
		return Convert(cTemp, bNew);
	}

	dword Convert(const char * pString)
	{
		if (pString == 0) return 0xffffffff;
		bool bNew;
		return Convert(pString, bNew);
	}

	inline dword GetNum(dword dwNum, dword dwAlign = 8)
	{ 
		return (1 + dwNum / dwAlign) * dwAlign;
	}

	dword Convert(const char * pString, bool & bNew)
	{
		dword nHash;
		dword nTableIndex;
		dword nStringCode;
		dword n;
		if (pString == 0) return 0xffffffff;
		nHash = MakeHashValue(pString);
		nTableIndex = nHash & (HASH_TABLE_SIZE - 1);

		HTELEMENT * pE = &HTable[nTableIndex];

		for (n=0; n<pE->nStringsNum; n++)
		{
			if (pE->pElements[n].dwHashCode == nHash && stricmp(pString, pE->pElements[n].pStr) == 0) 
			{
				nStringCode = (nTableIndex<<16)| (n & 0xffff);
				bNew = false;
				return nStringCode;
			}
		}

		n = pE->nStringsNum;
		pE->nStringsNum++;
		pE->pElements = (HTSUBELEMENT *)RESIZE(pE->pElements, GetNum(pE->nStringsNum) * sizeof(HTSUBELEMENT));

		pE->pElements[n].pStr = NEW char[strlen(pString) + 1];
		strcpy(pE->pElements[n].pStr, pString);
		pE->pElements[n].dwHashCode = nHash;

		nStringCode = (nTableIndex << 16) | (n & 0xffff);
		nStringsNum++;
		bNew = true;
		return nStringCode;
	}

	void VariableChanged();

	char * Convert(dword code)
	{
		dword nTableIndex;
		dword n;
		nTableIndex = code>>16;
		if (nTableIndex >= HASH_TABLE_SIZE)
		{
			return "ERROR: invalid SCCT index";
		}
		n = code & 0xffff;
		if (n >= HTable[nTableIndex].nStringsNum) 
		{
			return "INVALID SCC";
		}
		return HTable[nTableIndex].pElements[n].pStr;
	}

	dword MakeHashValue(const char * ps)
	{
		dword hval = 0;
		dword g;
		char v;
		while(*ps != 0)
		{
			v = *ps++;
			if('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
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

	char * Get()
	{
		nHTIndex = 0;
		for(nHTIndex = 0;nHTIndex < HASH_TABLE_SIZE; nHTIndex++)
		{
			if(HTable[nHTIndex].nStringsNum > 0)
			{
				nHTEIndex = 0;
				return HTable[nHTIndex].pElements[nHTEIndex].pStr;
			}
		}
		return 0;

	}

	char * GetNext()
	{
		nHTEIndex++;
		for(;nHTIndex < HASH_TABLE_SIZE; nHTIndex++)
		{
			if(HTable[nHTIndex].nStringsNum == 0) continue;
			if(nHTEIndex >= HTable[nHTIndex].nStringsNum)
			{
				nHTEIndex = 0;
				continue;
			}
			return HTable[nHTIndex].pElements[nHTEIndex].pStr;
		}
		return 0;
	}
	//*/
};

#endif