#include "CRC.h"

dword CRC16::crc_16_table[16] = {	0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
									0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 };


CRC16::CRC16()
{
	pInternalBuffer = null;
	dwCurrentSize = 0;
	dwReserveSize = 0;
}

CRC16::~CRC16()
{
	DELETE(pInternalBuffer);
	dwCurrentSize = 0;
	dwReserveSize = 0;
}

void CRC16::Reserve(dword dwNewSize)
{
	dwNewSize = ((dwNewSize / 1024) + 1) * 1024;
	if (dwNewSize == dwReserveSize) return;
	pInternalBuffer = (char*)RESIZE(pInternalBuffer, dwNewSize);
	dwReserveSize = dwNewSize;
}

word CRC16::GetCRC16(const char * pBuffer, dword dwSize)
{
	dword crc = 0;
	int r;

	/* while there is more data to process */
	while (dwSize-- > 0) 
	{
		/* compute checksum of lower four bits of *p */
		r = crc_16_table[crc & 0xF];
		crc = (crc >> 4) & 0x0FFF;
		crc = crc ^ r ^ crc_16_table[*pBuffer & 0xF];

		/* now compute checksum of upper four bits of *p */
		r = crc_16_table[crc & 0xF];
		crc = (crc >> 4) & 0x0FFF;
		crc = crc ^ r ^ crc_16_table[(*pBuffer >> 4) & 0xF];

		/* next... */
		pBuffer++;
	}

	return word(crc);
}

void CRC16::MakeCRC16(const char * pBuffer, dword dwSize, word wSendParameters)
{
	Assert(pBuffer && dwSize);

	dwCurrentSize = dwSize + IS_NM_DEBUG(6, 4);

	Reserve(dwCurrentSize);

#ifdef NM_DEBUG
	pInternalBuffer[0] = NM_WORD;
	pInternalBuffer[3] = NM_WORD;
	memcpy(&pInternalBuffer[4], &wSendParameters, sizeof(wSendParameters));
	memcpy(&pInternalBuffer[6], pBuffer, dwSize);

	word crc = GetCRC16(&pInternalBuffer[3], dwCurrentSize - 3);
	memcpy(&pInternalBuffer[1], &crc, sizeof(crc));
#else
	memcpy(&pInternalBuffer[2], &wSendParameters, sizeof(wSendParameters));
	memcpy(&pInternalBuffer[4], pBuffer, dwSize);

	word crc = GetCRC16(&pInternalBuffer[2], dwCurrentSize - 2);
	memcpy(&pInternalBuffer[0], &crc, sizeof(crc));
#endif
}

char * CRC16::GetDataBuffer()
{
	return pInternalBuffer;
}

dword CRC16::GetDataSize()
{
	return dwCurrentSize;
}
