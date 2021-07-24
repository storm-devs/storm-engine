#pragma once

#include <cstring>
#include <malloc.h>
#include <stdint.h>

#define DHASH_TABLE_SIZE 255
#define DHASH_SINGLESYM DHASH_TABLE_SIZE

class DSTRING_CODEC
{
    struct HTSUBELEMENT
    {
        HTSUBELEMENT()
        {
            pDString = nullptr;
            nSize = 0;
        };
        char *pDString;
        uint32_t nSize;
    };

    struct HTDELEMENT
    {
        HTDELEMENT()
        {
            ppDat = nullptr;
            nStringsNum = 0;
        };
        HTSUBELEMENT *ppDat;
        uint32_t nStringsNum;
    };

    uint32_t nStringsNum;
    uint32_t nHTIndex;
    uint32_t nHTEIndex;
    char pSymbol[2];

  public:
    HTDELEMENT HTable[DHASH_TABLE_SIZE];

    DSTRING_CODEC() : nHTIndex(0), nHTEIndex(0)
    {
        nStringsNum = 0;
        pSymbol[0] = 0;
        pSymbol[1] = 0;
    };

    ~DSTRING_CODEC()
    {
        Release();
    };

    uint32_t GetNum()
    {
        return nStringsNum;
    };

    void Release()
    {
        for (uint32_t m = 0; m < DHASH_TABLE_SIZE; m++)
        {
            if (HTable[m].ppDat)
            {
                for (uint32_t n = 0; n < HTable[m].nStringsNum; n++)
                    delete HTable[m].ppDat[n].pDString;
                free(HTable[m].ppDat);
            }
            HTable[m].ppDat = nullptr;
            HTable[m].nStringsNum = 0;
        }
    }

    uint32_t Convert(const char *pString, uint32_t nDataSize, bool &bNew)
    {
        uint32_t nStringCode;
        uint32_t n;
        if (pString == nullptr)
            return 0xffffffff;

        if (nDataSize == 1)
        {
            // nStringCode = (DHASH_SINGLESYM<<16)| (pString[0] & 0xffff);
            nStringCode = ((((unsigned char)pString[0]) << 8) & 0xffffff00) | (DHASH_SINGLESYM);
            bNew = true;
            if (nStringCode >= 0xffffff)
            {
                __debugbreak();
            }
            return nStringCode;
        }

        uint32_t nHash = MakeHashValue(pString, nDataSize);
        uint32_t nTableIndex = nHash % DHASH_TABLE_SIZE;

        for (n = 0; n < HTable[nTableIndex].nStringsNum; n++)
        {
            if (nDataSize != HTable[nTableIndex].ppDat[n].nSize)
                continue;

            if (memcmp(pString, HTable[nTableIndex].ppDat[n].pDString, nDataSize) == 0)
            {
                // nStringCode = (nTableIndex<<16)| (n & 0xffff);
                nStringCode = ((n << 8) & 0xffffff00) | (nTableIndex & 0xff);
                bNew = false;
                if (nStringCode >= 0xffffff)
                {
                    __debugbreak();
                }
                return nStringCode;
            }
        }

        n = HTable[nTableIndex].nStringsNum;
        HTable[nTableIndex].nStringsNum++;
        HTable[nTableIndex].ppDat =
            (HTSUBELEMENT *)realloc(HTable[nTableIndex].ppDat, HTable[nTableIndex].nStringsNum * sizeof(HTSUBELEMENT));

        HTable[nTableIndex].ppDat[n].pDString = new char[nDataSize];
        HTable[nTableIndex].ppDat[n].nSize = nDataSize;
        memcpy(HTable[nTableIndex].ppDat[n].pDString, pString, nDataSize);
        // nStringCode = (nTableIndex<<16)| (n & 0xffff);
        nStringCode = ((n << 8) & 0xffffff00) | (nTableIndex & 0xff);
        nStringsNum++;
        bNew = true;
        return nStringCode;
    }

    char *Convert(uint32_t code, uint32_t &nSize)
    {
        // nTableIndex = code>>16;
        uint32_t nTableIndex = code & 0xff;
        if (nTableIndex == DHASH_SINGLESYM)
        {
            nSize = 1;
            // pSymbol[0] = (char)(code & 0xffff);
            pSymbol[0] = (char)(code >> 8);
            return pSymbol;
        }
        if (nTableIndex >= DHASH_TABLE_SIZE)
        {
            return nullptr;
        }
        // n = code & 0xffff;
        uint32_t n = code >> 8;
        if (n >= HTable[nTableIndex].nStringsNum)
        {
            return nullptr;
        }
        nSize = HTable[nTableIndex].ppDat[n].nSize;
        return HTable[nTableIndex].ppDat[n].pDString;
    }

    uint32_t MakeHashValue(const char *ps, uint32_t nSize)
    {
        uint32_t hval = 0;
        uint32_t n = 0;
        // while(*ps != 0)
        for (n = 0; n < nSize; n++)
        {
            char v = ps[n];
            hval = (hval << 4) + (unsigned long int)v;
            uint32_t g = hval & ((unsigned long int)0xf << (32 - 4));
            if (g != 0)
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
