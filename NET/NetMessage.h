#ifndef NET_MESSAGE_HPP
#define NET_MESSAGE_HPP

#include "..\common_h\templates\tcommon.h"

#ifndef _NOBLOOD_
	#include "..\common_h\templates\string.h"
#endif

//#define NM_DEBUG

#ifdef NM_DEBUG
	#define NM_DEBUG_TST(x) { if (byte(pData[dwCurPos]) != x) throw; dwCurPos += sizeof(byte); }
	#define NM_DEBUG_ADD(x) { byte bByte = x; AddData(&bByte, sizeof(bByte)); }
	#define IS_NM_DEBUG(a, b) (a)
#else
	#define NM_DEBUG_TST(x)
	#define NM_DEBUG_ADD(x)
	#define IS_NM_DEBUG(a, b) (b)
#endif

#define NM_BYTE			120
#define NM_WORD			121
#define NM_DWORD		122
#define NM_FLOAT		123
#define NM_STRING		124
#define NM_24			125

class NMRecv
{
public:
	NMRecv(const char * pData, dword dwSize)
	{
		this->pData = pData;
		this->dwDataSize = dwSize;

		this->dwCurPos = 0;
	}

	~NMRecv()
	{
	}

	byte GetByte()
	{
		NM_DEBUG_TST(NM_BYTE);
		byte bRes = *(byte*)(&pData[dwCurPos]);
		dwCurPos += sizeof(bRes);
		return bRes;
	}

	word GetWord()
	{
		NM_DEBUG_TST(NM_WORD);
		word wRes = *(word*)(&pData[dwCurPos]);
		dwCurPos += sizeof(wRes);
		return wRes;
	}

	dword GetDword_NoDbg()
	{
		dword dwRes = *(dword*)(&pData[dwCurPos]);
		dwCurPos += sizeof(dwRes);
		return dwRes;
	}

	dword GetDword()
	{
		NM_DEBUG_TST(NM_DWORD);
		dword dwRes = *(dword*)(&pData[dwCurPos]);
		dwCurPos += sizeof(dwRes);
		return dwRes;
	}

	dword Get24()
	{
		NM_DEBUG_TST(NM_24);
		dword dwRes = 0;
		memcpy(((char*)&dwRes) + 1, &pData[dwCurPos], 3);
		dwCurPos += 3;
		return dwRes >> 8;
	}

	char * GetData(dword & dwDataSize)
	{
		dwDataSize = GetWord();
		char * pDataBuffer = (char*)&pData[dwCurPos];
		dwCurPos += dwDataSize;
		return pDataBuffer;
	}

	float GetFloat()
	{
		NM_DEBUG_TST(NM_FLOAT);
		float fRes = *(float*)(&pData[dwCurPos]);
		dwCurPos += sizeof(fRes);
		return fRes;
	}

#ifndef _NOBLOOD_
	string GetString()
	{
		NM_DEBUG_TST(NM_STRING);
		word wLen = GetWord();
		string sValue;
		if (wLen > 0)
		{
			sValue.Reserve(wLen + 1);
			sValue.SetDataSize(wLen + 1);
			memcpy(&sValue[0], &pData[dwCurPos], wLen);
			sValue[wLen] = 0;
			dwCurPos += wLen;
			//return string(&pData[dwCurPos - wLen - 1]);
		}
		return sValue;
	}

	string GetString_NoDbg()
	{
		word wLen = GetWord();
		string sValue;
		if (wLen > 0)
		{
			sValue.Reserve(wLen + 1);
			sValue.SetDataSize(wLen + 1);
			memcpy(&sValue[0], &pData[dwCurPos], wLen);
			sValue[wLen] = 0;
			dwCurPos += wLen;
			//return string(&pData[dwCurPos - wLen - 1]);
		}
		return sValue;
	}
#endif

	bool IsEmpty()
	{
		return dwCurPos >= dwDataSize;
	}

	const char * GetDataBuffer() { return pData; }
	dword GetDataSize() { return dwDataSize; }

private:
	dword dwCurPos;
	dword dwDataSize;
	const char * pData;

};

class NMSend
{
public:

	NMSend()
	{
		dwDataSize = 0;
		pData = null;
	}

	~NMSend()
	{
		Clear();
	}

	void Clear()
	{
		DELETE(pData);
		dwDataSize = 0;
	}

	bool IsEmpty()
	{
		return dwDataSize == 0;
	}

	void AddData(void * pBuffer, dword dwSize)
	{
		pData = (char*)RESIZE(pData, ((dwDataSize + dwSize + 32) / 32) * 32);
		memcpy(&pData[dwDataSize], pBuffer, dwSize);
		dwDataSize += dwSize;
	}

	void Add24(dword d) 
	{ 
		d <<= 8;
		NM_DEBUG_ADD(NM_24); 
		AddData(((char*)&d) + 1, 3); 
	}

	void AddByte(byte b) { NM_DEBUG_ADD(NM_BYTE); AddData(&b, sizeof(b)); }
	void AddWord(word w) { NM_DEBUG_ADD(NM_WORD); AddData(&w, sizeof(w)); }
	void AddDword(dword d) { NM_DEBUG_ADD(NM_DWORD); AddData(&d, sizeof(d)); }
	void AddDword_NoDbg(dword d) { AddData(&d, sizeof(d)); }
	void AddFloat(float f) { NM_DEBUG_ADD(NM_FLOAT); AddData(&f, sizeof(f)); }

#ifndef _NOBLOOD_
	void AddString(string & s, long iMaxLen)
	{
		NM_DEBUG_ADD(NM_STRING); 
		word wLen = word((s.Len() > dword(iMaxLen)) ? iMaxLen : s.Len());
		AddWord(wLen);
		if (s.Len())
			AddData((void*)s.GetBuffer(), wLen);
	}
	void AddString_NoDbg(string & s, long iMaxLen)
	{
		word wLen = word((s.Len() > dword(iMaxLen)) ? iMaxLen : s.Len());
		AddWord(wLen);
		if (s.Len())
			AddData((void*)s.GetBuffer(), wLen);
	}
#endif

	char * GetDataBuffer() { return pData; }
	dword GetDataSize() { return dwDataSize; }

private:
	dword dwDataSize;
	char * pData;
};

#endif