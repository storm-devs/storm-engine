#ifndef _S_COMPRESS_H_
#define _S_COMPRESS_H_

#ifndef DWORD
#define DWORD unsigned long
#endif

#include "..\..\common_h\memop.h"
//#include "sstring.h"
#include <malloc.h>
#include "dstring_codec.h"

struct PACKHEADER
{
	PACKHEADER(){dwSign[0] = 'A'; dwSign[0] = 'K'; dwSign[0] = 'S'; dwSign[0] = 'P';};
	char  dwSign[4];
	DWORD dwHeaderSize;
	DWORD dwDataSize;
	DWORD dwBitsGhangesNum;
};

struct BITSCHANGE
{
	DWORD dwBits;
	DWORD dwChangeOffset;
};

class COMPRESS
{
	PACKHEADER Header;
	DSTRING_CODEC SCodec;
	BITSCHANGE * pBTCompressionTable;
	DWORD dwBTCompressionTableSize;
public:
	 COMPRESS();
	~COMPRESS();
	void Release();
	void SetBitsChange(DWORD dwChangeOffset, DWORD dwBits);
	bool Pack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nPackedSize);
	bool APack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nPackedSize);
	bool Unpack(const char * pSource, DWORD nSourceSize, char * & pDestination, DWORD & nPackedSize);
	bool AppendCode(char * & pDestination, DWORD & nPackedSize, DWORD nCode);
	bool AAppendCode(char * & pDestination, DWORD & nPackedSize, DWORD nCode, DWORD & dwBitsOnCode, DWORD & dwBitsCoded);
	bool TranslateCode(DWORD code, char * & pDataBuffer, DWORD & nBufferSize, DWORD & nDataSize);
	bool ReadCode(const char * pSource, DWORD nSourceSize, DWORD & nOffset, DWORD & nResult);
	bool AppendData(char * & pDestination, DWORD & nUnpackedSize, char * pData, DWORD nSize);
};

#endif