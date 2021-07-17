#include "s_compress.h"

#define BYTEBITS 8

COMPRESS::COMPRESS()
{
    dwBTCompressionTableSize = 0;
}

COMPRESS::~COMPRESS()
{
    Release();
}

void COMPRESS::Release()
{
    dwBTCompressionTableSize = 0;
}

void COMPRESS::SetBitsChange(uint32_t dwChangeOffset, uint32_t dwBits)
{
    const auto n = dwBTCompressionTableSize;
    dwBTCompressionTableSize++;

    pBTCompressionTable.resize(dwBTCompressionTableSize);

    pBTCompressionTable[n].dwBits = dwBits;
    pBTCompressionTable[n].dwChangeOffset = dwChangeOffset;
}

bool COMPRESS::Pack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nPackedSize)
{
    // return APack(pSource,nSourceSize,pDestination,nPackedSize);

    uint32_t nCode;
    bool bNew;

    SCodec.Release();

    if (pSource == nullptr)
        return false;

    uint32_t nBufferSize = 1024;
    auto *pDataBuffer = static_cast<char *>(malloc(nBufferSize));
    memset(pDataBuffer, 0, nBufferSize);

    pDataBuffer[0] = pSource[0];
    uint32_t nDataSize = 1;
    auto nOldCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
    for (uint32_t n = 1; n < nSourceSize; n++)
    {
        pDataBuffer[nDataSize] = pSource[n];
        nDataSize++;
        if (nDataSize >= nBufferSize)
        {
            nBufferSize = nBufferSize * 2;
            pDataBuffer = static_cast<char *>(realloc(pDataBuffer, nBufferSize));
        }
        nCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
        if (!bNew)
        {
            continue;
        }
        nDataSize--;
        nOldCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
        AppendCode(pDestination, nPackedSize, nOldCode);
        pDataBuffer[0] = pSource[n];
        pDataBuffer[1] = 0;
        nDataSize = 1;
    }

    nCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
    AppendCode(pDestination, nPackedSize, nCode);

    if (pDataBuffer)
        free(pDataBuffer);
    pDataBuffer = nullptr;

    return true;
}

bool COMPRESS::APack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nPackedSize)
{
    uint32_t nCode;
    bool bNew;

    // release words table
    Release();
    SCodec.Release();

    if (pSource == nullptr)
        return false;

    char *pCodedData = nullptr;

    // set buffer for accumulating word
    uint32_t nBufferSize = 1024;
    auto *pDataBuffer = static_cast<char *>(malloc(nBufferSize));
    memset(pDataBuffer, 0, nBufferSize);

    // initial value bits on code is 8
    uint32_t dwBitsOnCode = 8;
    // no coded data yet
    uint32_t dwBitsCoded = 0;

    SetBitsChange(0, 8);

    pDataBuffer[0] = pSource[0];
    uint32_t nDataSize = 1;

    auto nOldCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
    for (uint32_t n = 1; n < nSourceSize; n++)
    {
        pDataBuffer[nDataSize] = pSource[n];
        nDataSize++;
        if (nDataSize >= nBufferSize)
        {
            nBufferSize = nBufferSize * 2;
            pDataBuffer = static_cast<char *>(realloc(pDataBuffer, nBufferSize));
        }
        nCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
        if (!bNew)
        {
            continue;
        }
        nDataSize--;
        nOldCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
        AAppendCode(pCodedData, nPackedSize, nOldCode, dwBitsOnCode, dwBitsCoded);
        pDataBuffer[0] = pSource[n];
        pDataBuffer[1] = 0;
        nDataSize = 1;
    }

    nCode = SCodec.Convert(pDataBuffer, nDataSize, bNew);
    AAppendCode(pCodedData, nPackedSize, nCode, dwBitsOnCode, dwBitsCoded);

    if (pDataBuffer)
        free(pDataBuffer);
    pDataBuffer = nullptr;

    pDestination = pCodedData;
    return true;
}

bool COMPRESS::AppendCode(char *&pDestination, uint32_t &nPackedSize, uint32_t nCode)
{
    const uint32_t nBSize = 3;
    if (pDestination == nullptr)
        pDestination = static_cast<char *>(malloc(nBSize));
    else
        pDestination = static_cast<char *>(realloc(pDestination, nPackedSize + nBSize));
    if (pDestination == nullptr)
        return false;
    auto *const pB = (char *)&nCode;
    *static_cast<char *>(pDestination + nPackedSize) = pB[0];
    nPackedSize++;
    *static_cast<char *>(pDestination + nPackedSize) = pB[1];
    nPackedSize++;
    *static_cast<char *>(pDestination + nPackedSize) = pB[2];
    nPackedSize++;
    return true;
}

bool COMPRESS::AAppendCode(char *&pDest, uint32_t &dwDestinationBufferSize, uint32_t dwCode, uint32_t &dwBitsOnCode,
                           uint32_t &dwBitsCoded)
{
    // verify - there must be enough bits for code
    uint32_t dwMaxCodeValue = (1 << dwBitsOnCode) - 1;

    auto bBitsChanged = false;
    while (dwCode >= dwMaxCodeValue)
    {
        dwBitsOnCode++;
        dwMaxCodeValue = (1 << dwBitsOnCode) - 1;
        //
        // ... set changes to bits code table
        //
        if (dwBitsOnCode >= 32)
            return false; // iloop break
        bBitsChanged = true;
    }
    if (bBitsChanged)
        SetBitsChange(dwBitsCoded, dwBitsOnCode);

    auto dwByteOffset = dwBitsCoded / BYTEBITS;

    unsigned char DestMask = 0x80 >> (dwBitsCoded % BYTEBITS);
    uint32_t dwSrcMask = 0x1 << dwBitsOnCode;

    if (pDest == nullptr)
    {
        dwDestinationBufferSize = 1;
        pDest = static_cast<char *>(malloc(dwDestinationBufferSize));
    }

    for (uint32_t n = 0; n < dwBitsOnCode; n++)
    {
        if (dwCode & dwSrcMask)
            pDest[dwByteOffset] |= DestMask; // set bit
        else
            pDest[dwByteOffset] &= (~DestMask); // clear bit

        DestMask = DestMask >> 1;
        dwSrcMask = dwSrcMask >> 1;

        if (DestMask == 0)
        {
            dwDestinationBufferSize++;
            if (pDest == nullptr)
                pDest = static_cast<char *>(malloc(dwDestinationBufferSize));
            else
                pDest = static_cast<char *>(realloc(pDest, dwDestinationBufferSize));

            dwByteOffset++;
            DestMask = 0x80; // bin 10000000
        }
    }

    dwBitsCoded += dwBitsOnCode;

    return true;
    /*    DWORD nBSize;
      char * pB;
      nBSize = 3;
      if(pDestination == 0) pDestination = (char *)malloc(nBSize);
      else pDestination = (char *)realloc(pDestination,nPackedSize + nBSize);
      if(pDestination == 0) return false;
      pB = (char *)&nCode;
      *((char *)(pDestination + nPackedSize)) = pB[0]; nPackedSize++;
      *((char *)(pDestination + nPackedSize)) = pB[1]; nPackedSize++;
      *((char *)(pDestination + nPackedSize)) = pB[2]; nPackedSize++;
      return true;
    */
}

bool COMPRESS::AppendData(char *&pDestination, uint32_t &nUnpackedSize, char *pData, uint32_t nSize)
{
    if (pDestination == nullptr)
        pDestination = static_cast<char *>(malloc(nSize));
    else
        pDestination = static_cast<char *>(realloc(pDestination, nUnpackedSize + nSize));
    if (pDestination == nullptr)
        return false;
    memcpy(pDestination + nUnpackedSize, pData, nSize);
    nUnpackedSize += nSize;
    return true;
}

bool COMPRESS::ReadCode(const char *pSource, uint32_t nSourceSize, uint32_t &nOffset, uint32_t &nResult)
{
    auto *const pB = (char *)&nResult;
    nResult = 0;
    pB[0] = *((char *)(pSource + nOffset));
    nOffset++;
    pB[1] = *((char *)(pSource + nOffset));
    nOffset++;
    pB[2] = *((char *)(pSource + nOffset));
    nOffset++;
    return true;
}

bool COMPRESS::TranslateCode(uint32_t code, char *&pDataBuffer, uint32_t &nBufferSize, uint32_t &nDataSize)
{
    uint32_t nSize;
    char *pData = SCodec.Convert(code, nSize);
    if (pData == nullptr)
        return false;
    if (pDataBuffer == nullptr)
    {
        nBufferSize = nSize;
        nDataSize = nSize;
        pDataBuffer = static_cast<char *>(malloc(nSize));
    }
    else
    {
        if (nSize > nBufferSize)
        {
            pDataBuffer = static_cast<char *>(realloc(pDataBuffer, nSize));
            nBufferSize = nSize;
            nDataSize = nSize;
        }
        else
        {
            nDataSize = nSize;
        }
    }
    memcpy(pDataBuffer, pData, nSize);
    return true;
}

bool COMPRESS::Unpack(const char *pSource, uint32_t nSourceSize, char *&pDestination, uint32_t &nUnpackedSize)
{
    uint32_t nOldCode;
    uint32_t nNewCode;
    bool bNew;

    uint32_t nDataSize;

    SCodec.Release();

    uint32_t nOffset = 0;

    uint32_t nBufferSize = 1024;
    char *pDataBuffer = static_cast<char *>(malloc(nBufferSize));
    memset(pDataBuffer, 0, nBufferSize);

    uint32_t n = 0;
    nOffset = 0;
    ReadCode(pSource, nSourceSize, nOffset, nOldCode);
    TranslateCode(nOldCode, pDataBuffer, nBufferSize, nDataSize);
    AppendData(pDestination, nUnpackedSize, pDataBuffer, nDataSize);
    char Symbol = pDataBuffer[0];

    while (nOffset < nSourceSize)
    {
        ReadCode(pSource, nSourceSize, nOffset, nNewCode);
        if (!TranslateCode(nNewCode, pDataBuffer, nBufferSize, nDataSize))
        {
            TranslateCode(nOldCode, pDataBuffer, nBufferSize, nDataSize);
            if (nBufferSize < (nDataSize + 1))
            {
                nBufferSize = nBufferSize * 2;
                pDataBuffer = static_cast<char *>(realloc(pDataBuffer, nBufferSize));
            }
            pDataBuffer[nDataSize] = Symbol;
            nDataSize++;
        }
        // App Data
        AppendData(pDestination, nUnpackedSize, pDataBuffer, nDataSize);

        Symbol = pDataBuffer[0];
        TranslateCode(nOldCode, pDataBuffer, nBufferSize, nDataSize);
        pDataBuffer[nDataSize] = Symbol;
        nDataSize++;
        SCodec.Convert(pDataBuffer, nDataSize, bNew);
        nOldCode = nNewCode;
    }

    return true;
}
