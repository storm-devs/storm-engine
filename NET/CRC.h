#ifndef CRC16_HPP
#define CRC16_HPP

#include "NetMessage.h"

class CRC16
{
private:
	static dword crc_16_table[16];
	char * pInternalBuffer;
	dword dwCurrentSize;
	dword dwReserveSize;

	void Reserve(dword dwNewSize);

public:
	CRC16();
	~CRC16();

	word GetCRC16(const char * pBuffer, dword dwSize);
	void MakeCRC16(const char * pBuffer, dword dwSize, word wSendParameters);

	char * GetDataBuffer();
	dword GetDataSize();
};

class CRC32
{
private:
	static dword crc_32_table[256];
	
public:
	CRC32() {};
	~CRC32() {};

	static dword GetCRC32(const char * pBuffer, dword dwSize);
};

#endif