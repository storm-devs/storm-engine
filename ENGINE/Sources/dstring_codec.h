#ifndef _DSTRING_CODEC_H_
#define _DSTRING_CODEC_H_

#include <memory.h>
#include "..\..\common_h\memop.h"

#define DHASH_TABLE_SIZE	255
#define DHASH_SINGLESYM		DHASH_TABLE_SIZE

class DSTRING_CODEC
{
	struct HTSUBELEMENT
	{
		HTSUBELEMENT() {pDString = 0; nSize = 0;};
		char * pDString;
		DWORD  nSize;
	};

	struct HTDELEMENT
	{
		HTDELEMENT() {ppDat = 0; nStringsNum = 0;};
		HTSUBELEMENT * ppDat;
		DWORD   nStringsNum;
	};

	DWORD   nStringsNum;
	DWORD	nHTIndex;
	DWORD	nHTEIndex;
	char	pSymbol[2];

public:
	HTDELEMENT HTable[DHASH_TABLE_SIZE];
	DSTRING_CODEC(){nStringsNum = 0;pSymbol[0] = 0; pSymbol[1] = 0;};
	~DSTRING_CODEC(){Release();};
	
	
	DWORD GetNum() {return nStringsNum;};

	void Release()
	{
		DWORD n,m;
		for(m=0;m<DHASH_TABLE_SIZE;m++)
		{
			if(HTable[m].ppDat)
			{
				for(n=0;n<HTable[m].nStringsNum;n++) delete HTable[m].ppDat[n].pDString;
				delete HTable[m].ppDat;
			}
			HTable[m].ppDat = 0; HTable[m].nStringsNum = 0;
		}
	}
	
	DWORD Convert(const char * pString, DWORD nDataSize, bool & bNew)
	{
		DWORD nHash;
		DWORD nTableIndex;
		DWORD nStringCode;
		DWORD n;
		if(pString == 0) return 0xffffffff;

		if(nDataSize == 1)
		{
			//nStringCode = (DHASH_SINGLESYM<<16)| (pString[0] & 0xffff);
			nStringCode = ((((unsigned char)pString[0])<<8) & 0xffffff00)|(DHASH_SINGLESYM);
			bNew = true;
			if(nStringCode >= 0xffffff)
				{
					_asm int 3;
				}
			return nStringCode;
		}

		nHash = MakeHashValue(pString,nDataSize);
		nTableIndex = nHash%DHASH_TABLE_SIZE;

		
		for(n=0;n<HTable[nTableIndex].nStringsNum;n++)
		{
			if(nDataSize != HTable[nTableIndex].ppDat[n].nSize) continue;
			
			if(memcmp(pString,HTable[nTableIndex].ppDat[n].pDString,nDataSize)==0) 
			{
				//nStringCode = (nTableIndex<<16)| (n & 0xffff);
				nStringCode = ((n<<8) & 0xffffff00)| (nTableIndex & 0xff);
				bNew = false;
				if(nStringCode >= 0xffffff)
				{
					_asm int 3;
				}
				return nStringCode;
			}
		}
		
		n = HTable[nTableIndex].nStringsNum;
		HTable[nTableIndex].nStringsNum++;
		HTable[nTableIndex].ppDat = (HTSUBELEMENT*)RESIZE(HTable[nTableIndex].ppDat,HTable[nTableIndex].nStringsNum * sizeof(HTSUBELEMENT));

		HTable[nTableIndex].ppDat[n].pDString = NEW char[nDataSize];
		HTable[nTableIndex].ppDat[n].nSize = nDataSize;
		memcpy(HTable[nTableIndex].ppDat[n].pDString,pString,nDataSize);
		//nStringCode = (nTableIndex<<16)| (n & 0xffff);
		nStringCode = ((n<<8) & 0xffffff00)| (nTableIndex & 0xff);
		nStringsNum++;
		bNew = true;
		return nStringCode;
	}
	char * Convert(DWORD code, DWORD & nSize)
	{
		DWORD nTableIndex;
		DWORD n;
		//nTableIndex = code>>16;
		nTableIndex = code & 0xff;
		if(nTableIndex == DHASH_SINGLESYM)
		{
			nSize = 1;
			//pSymbol[0] = (char)(code & 0xffff);
			pSymbol[0] = (char)(code >> 8);
			return pSymbol;
		}
		if(nTableIndex >= DHASH_TABLE_SIZE)
		{
			return 0;
		}
		//n = code & 0xffff;
		n = code >> 8;
		if(n >= HTable[nTableIndex].nStringsNum) 
		{
			return 0;
		}
		nSize = HTable[nTableIndex].ppDat[n].nSize; 
		return HTable[nTableIndex].ppDat[n].pDString;
	}
	DWORD MakeHashValue(const char * ps, DWORD nSize)
	{
		DWORD hval = 0;
		DWORD g;
		DWORD n = 0;
		char v;
		//while(*ps != 0)
		for(n=0;n<nSize;n++)
		{
			v = ps[n];
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
	/*char * Get()
	{
		nHTIndex = 0;
		for(nHTIndex = 0;nHTIndex < DHASH_TABLE_SIZE; nHTIndex++)
		{
			if(HTable[nHTIndex].nStringsNum > 0)
			{
				nHTEIndex = 0;
				return HTable[nHTIndex].ppDat[nHTEIndex];
			}
		}
		return 0;

	}
	char * GetNext()
	{
		nHTEIndex++;
		for(;nHTIndex < DHASH_TABLE_SIZE; nHTIndex++)
		{
			if(HTable[nHTIndex].nStringsNum == 0) continue;
			if(nHTEIndex >= HTable[nHTIndex].nStringsNum)
			{
				nHTEIndex = 0;
				continue;
			}
			return HTable[nHTIndex].ppDat[nHTEIndex];
		}
		return 0;
	}*/
};

#endif