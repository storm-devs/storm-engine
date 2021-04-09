#pragma once

#include "dstring_codec.h"
#include <cstdint>
#include <vector>

struct PACKHEADER
{
    PACKHEADER()
    {
        dwSign[0] = 'A';
        dwSign[0] = 'K';
        dwSign[0] = 'S';
        dwSign[0] = 'P';
    };
    char dwSign[4];
    uint32_t dwHeaderSize;
    uint32_t dwDataSize;
    uint32_t dwBitsGhangesNum;
};

struct BITSCHANGE
{
    uint32_t dwBits;
    uint32_t dwChangeOffset;
};

class COMPRESS
{
    PACKHEADER Header;
    DSTRING_CODEC SCodec;
    std::vector<BITSCHANGE> pBTCompressionTable;
    uint32_t dwBTCompressionTableSize;

  public:
    COMPRESS();
    ~COMPRESS();
    void Release();
    void SetBitsChange(uint32_t dwChangeOffset, uint32_t dwBits);
    bool Pack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nPackedSize);
    bool APack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nPackedSize);
    bool Unpack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nPackedSize);
    bool AppendCode(char *&pDestination, uint32_t &nPackedSize, uint32_t nCode);
    bool AAppendCode(char *&pDestination, uint32_t &nPackedSize, uint32_t nCode, uint32_t &dwBitsOnCode,
                     uint32_t &dwBitsCoded);
    bool TranslateCode(uint32_t code, char *&pDataBuffer, uint32_t &nBufferSize, uint32_t &nDataSize);
    bool ReadCode(const char *pSource, uint32_t nSourceSize, uint32_t &nOffset, uint32_t &nResult);
    bool AppendData(char *&pDestination, uint32_t &nUnpackedSize, char *pData, uint32_t nSize);
};
