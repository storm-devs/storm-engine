#include "s_compress.h"

#define BYTEBITS	8

COMPRESS::COMPRESS()
{
	pBTCompressionTable = 0;
	dwBTCompressionTableSize = 0;
}

COMPRESS::~COMPRESS()
{
	Release();
}

void COMPRESS::Release()
{
	if(pBTCompressionTable) delete pBTCompressionTable;
	pBTCompressionTable = 0;
	dwBTCompressionTableSize = 0;
}

void COMPRESS::SetBitsChange(DWORD dwChangeOffset, DWORD dwBits)
{
	DWORD n;
	n = dwBTCompressionTableSize;
	dwBTCompressionTableSize++;

	if(!pBTCompressionTable) pBTCompressionTable = (BITSCHANGE *)NEW char[dwBTCompressionTableSize*sizeof(BITSCHANGE)];
	else pBTCompressionTable = (BITSCHANGE *)RESIZE(pBTCompressionTable,dwBTCompressionTableSize*sizeof(BITSCHANGE));

	pBTCompressionTable[n].dwBits = dwBits;
	pBTCompressionTable[n].dwChangeOffset = dwChangeOffset;
}

bool COMPRESS::Pack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nPackedSize)
{
	//return APack(pSource,nSourceSize,pDestination,nPackedSize);

	DWORD n;
	DWORD nCode;
	char * pDataBuffer;
	DWORD nBufferSize;
	DWORD nDataSize;
	bool bNew;

	SCodec.Release();

	if(pSource == 0) return false;

	DWORD nOldCode;
	nBufferSize = 1024;
	pDataBuffer = (char *)malloc(nBufferSize);
	memset(pDataBuffer,0,nBufferSize);

	pDataBuffer[0] = pSource[0];
	nDataSize = 1;
	nOldCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
	for(n=1;n<nSourceSize;n++)
	{
		pDataBuffer[nDataSize] = pSource[n];
		nDataSize++;
		if(nDataSize >= nBufferSize)
		{
			nBufferSize = nBufferSize * 2;
			pDataBuffer = (char *)realloc(pDataBuffer,nBufferSize);
		}
		nCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
		if(!bNew) { continue; }
		nDataSize--;
		nOldCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
		AppendCode(pDestination,nPackedSize,nOldCode);
		pDataBuffer[0] = pSource[n];
		pDataBuffer[1] = 0;
		nDataSize = 1;
	}

	nCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
	AppendCode(pDestination,nPackedSize,nCode);
	
	if(pDataBuffer) free(pDataBuffer); pDataBuffer = 0;

	return true;
}

bool COMPRESS::APack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nPackedSize)
{
	DWORD n;
	DWORD nCode;
	char * pDataBuffer;
	DWORD nBufferSize;
	DWORD nDataSize;
	bool bNew;
	DWORD nOldCode;
	char * pCodedData;

	DWORD dwBitsOnCode;
	DWORD dwBitsCoded;

	// release words table
	Release();
	SCodec.Release();

	if(pSource == 0) return false;

	pCodedData = 0;

	// set buffer for accumulating word
	nBufferSize = 1024;
	pDataBuffer = (char *)malloc(nBufferSize);
	memset(pDataBuffer,0,nBufferSize);

	// initial value bits on code is 8
	dwBitsOnCode = 8;
	// no coded data yet
	dwBitsCoded = 0;

	SetBitsChange(0,8);
	
	pDataBuffer[0] = pSource[0];
	nDataSize = 1;

	nOldCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
	for(n=1;n<nSourceSize;n++)
	{
		pDataBuffer[nDataSize] = pSource[n];
		nDataSize++;
		if(nDataSize >= nBufferSize)
		{
			nBufferSize = nBufferSize * 2;
			pDataBuffer = (char *)realloc(pDataBuffer,nBufferSize);
		}
		nCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
		if(!bNew) { continue; }
		nDataSize--;
		nOldCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
		AAppendCode(pCodedData,nPackedSize,nOldCode,dwBitsOnCode,dwBitsCoded);
		pDataBuffer[0] = pSource[n];
		pDataBuffer[1] = 0;
		nDataSize = 1;
	}

	nCode = SCodec.Convert(pDataBuffer,nDataSize,bNew);
	AAppendCode(pCodedData,nPackedSize,nCode,dwBitsOnCode,dwBitsCoded);
	
	if(pDataBuffer) free(pDataBuffer); pDataBuffer = 0;

	pDestination = pCodedData;
	return true;
}

bool COMPRESS::AppendCode(char * & pDestination, DWORD & nPackedSize, DWORD nCode)
{
	DWORD nBSize;
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
}

bool COMPRESS::AAppendCode(char * & pDest, DWORD & dwDestinationBufferSize, DWORD dwCode, DWORD & dwBitsOnCode, DWORD & dwBitsCoded)
{
	DWORD n;
	DWORD dwByteOffset;
	DWORD dwSrcMask;
	unsigned char DestMask;
	bool bBitsChanged;

	// verify - there must be enough bits for code
	DWORD dwMaxCodeValue = (1<<dwBitsOnCode) - 1;

	bBitsChanged = false;
	while(dwCode >= dwMaxCodeValue)
	{
		dwBitsOnCode++;
		dwMaxCodeValue = (1<<dwBitsOnCode) - 1;
		//
		// ... set changes to bits code table
		// 
		if(dwBitsOnCode >= 32) return false;	// iloop break
		bBitsChanged = true;
	}
	if(bBitsChanged) SetBitsChange(dwBitsCoded,dwBitsOnCode);

	dwByteOffset = dwBitsCoded/BYTEBITS;
	
	DestMask = 0x80 >> (dwBitsCoded%BYTEBITS);
	dwSrcMask = 0x1 << dwBitsOnCode;

	
	if(pDest == 0) 
	{
		dwDestinationBufferSize = 1;
		pDest = (char *)malloc(dwDestinationBufferSize);
	}
	
	for(n=0;n<dwBitsOnCode;n++)
	{
		if(dwCode & dwSrcMask) pDest[dwByteOffset] |= DestMask;// set bit
		else pDest[dwByteOffset] &= (~DestMask);// clear bit

		DestMask = DestMask>>1;
		dwSrcMask = dwSrcMask>>1;

		if(DestMask == 0)
		{
			dwDestinationBufferSize++;
			if(pDest == 0) pDest = (char *)malloc(dwDestinationBufferSize);
			else pDest = (char *)realloc(pDest,dwDestinationBufferSize);


			dwByteOffset++;
			DestMask = 0x80;// bin 10000000
		}

	}

	dwBitsCoded += dwBitsOnCode;

	return true;
/*	DWORD nBSize;
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

bool COMPRESS::AppendData(char * & pDestination, DWORD & nUnpackedSize, char * pData, DWORD nSize)
{
	if(pDestination == 0) pDestination = (char *)malloc(nSize);
	else pDestination = (char *)realloc(pDestination,nUnpackedSize + nSize);
	if(pDestination == 0) return false;
	memcpy(pDestination + nUnpackedSize,pData,nSize);
	nUnpackedSize += nSize;
	return true;
}


bool COMPRESS::ReadCode(const char * pSource, DWORD nSourceSize, DWORD & nOffset, DWORD & nResult)
{
	char * pB;
	pB = (char *)&nResult;
	nResult = 0;
	pB[0] = *((char *)(pSource + nOffset)); nOffset++;
	pB[1] = *((char *)(pSource + nOffset)); nOffset++;
	pB[2] = *((char *)(pSource + nOffset)); nOffset++;
	return true;
}

bool COMPRESS::TranslateCode(DWORD code, char * & pDataBuffer, DWORD & nBufferSize, DWORD & nDataSize)
{
	DWORD  nSize;
	char * pData;
	pData = SCodec.Convert(code,nSize);
	if(pData == 0) return false;
	if(pDataBuffer == 0)
	{
		nBufferSize = nSize;
		nDataSize = nSize;
		pDataBuffer = (char *)malloc(nSize);
	} else
	{
		if(nSize > nBufferSize)
		{
			pDataBuffer = (char *)realloc(pDataBuffer,nSize);
			nBufferSize = nSize;
			nDataSize = nSize;
		}
		else
		{
			nDataSize = nSize;
		}
	}
	memcpy(pDataBuffer,pData,nSize);
	return true;
}

bool COMPRESS::Unpack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nUnpackedSize)
{
	DWORD nOffset;
	DWORD n;
	DWORD nOldCode;
	DWORD nNewCode;
	char  Symbol;
	bool  bNew;

	char * pDataBuffer;
	DWORD nBufferSize;
	DWORD nDataSize;

	SCodec.Release();
	
	nOffset = 0;
	
	nBufferSize = 1024;
	pDataBuffer = (char *)malloc(nBufferSize);
	memset(pDataBuffer,0,nBufferSize);

	n = 0;
	nOffset = 0;
	ReadCode(pSource,nSourceSize,nOffset,nOldCode);
	TranslateCode(nOldCode,pDataBuffer,nBufferSize,nDataSize);
	AppendData(pDestination,nUnpackedSize,pDataBuffer,nDataSize);
	Symbol = pDataBuffer[0];

	while(nOffset < nSourceSize)
	{
		ReadCode(pSource,nSourceSize,nOffset,nNewCode);
		if(!TranslateCode(nNewCode,pDataBuffer,nBufferSize,nDataSize))
		{
			TranslateCode(nOldCode,pDataBuffer,nBufferSize,nDataSize);
			if(nBufferSize < (nDataSize + 1))
			{
				nBufferSize = nBufferSize * 2;
				pDataBuffer = (char *)realloc(pDataBuffer,nBufferSize);
			}
			pDataBuffer[nDataSize] = Symbol;
			nDataSize++;
		}
		// App Data
		AppendData(pDestination,nUnpackedSize,pDataBuffer,nDataSize);

		Symbol = pDataBuffer[0];
		TranslateCode(nOldCode,pDataBuffer,nBufferSize,nDataSize);
		pDataBuffer[nDataSize] = Symbol;
		nDataSize++;
		SCodec.Convert(pDataBuffer,nDataSize,bNew);
		nOldCode = nNewCode;
	}
	
	return true;
}