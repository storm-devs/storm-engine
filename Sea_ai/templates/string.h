// string class
#ifndef STRING_HPP
#define STRING_HPP

#include <stdio.h>
#include "..\..\common_h\defines.h"

#define STRING_DEFAULT_ADD		16

class string 
{
private:
	char	*pBuffer;
	dword	dwSize;

	dword	dwCurSize, dwAddSize;

	void Reserve(dword dwReserveSize)
	{
		dword dwNewSize = dwAddSize * (1 + dwReserveSize / dwAddSize);
		if (dwCurSize == dwNewSize) return;
		dwCurSize = dwNewSize;	
		pBuffer = (char*)RESIZE(pBuffer,dwCurSize); 
	}

	void Init(dword _dwAdd)
	{
		pBuffer = null;
		dwSize = 0;
		dwCurSize = 0;
		dwAddSize = (_dwAdd==0) ? STRING_DEFAULT_ADD : _dwAdd;
	}

	inline void	SetZero()	
	{ 
		if (pBuffer) pBuffer[Len()] = 0; 
	};

public:

	// functions area

	inline bool		IsEmpty()  	const { return (dwSize == 0); }
	inline char*	GetBuffer()	{ return pBuffer; }
	inline dword	Len()  		const { return dwSize; }
	inline dword	Size()  	const { return Len(); }

	// operators area

	// string compare
	inline bool operator == (string & s1) { return (GetBuffer() && s1.GetBuffer()) ? stricmp(GetBuffer(),s1)==0 : false; }
	inline bool operator == (char *pString) { return GetBuffer() ? stricmp(GetBuffer(),pString)==0 : false; }

	// type conversion, to char*
	inline operator char* () { return pBuffer; }

	string & operator = (const char * pText) 
	{ 
		if (!pText || !pText[0]) { dwSize = 0; SetZero(); return *this;}
		dwSize = strlen(pText);

		Reserve(dwSize);
		memcpy(pBuffer,pText,dwSize); SetZero();

		return (*this);
	}

	string & operator = (string & sText) 
	{ 
		dwSize = sText.Len();
		if (sText.IsEmpty()) return *this;

		Reserve(dwSize);
		memcpy(pBuffer,sText,dwSize); SetZero();

		return *this;
	}

	string & operator += (string & sText) 
	{
		if (sText.IsEmpty()) return (*this);
		dword dwNewSize = Len() + sText.Len();
		Reserve(dwNewSize);
		memcpy(&pBuffer[dwSize],sText,sText.Len()); 
		dwSize = dwNewSize;
		SetZero();
		return *this;
	}

	string & operator += (float fValue) 
	{
		char temp[16];
		_gcvt(fValue,7,temp);
		*this += temp;
		return *this;
	}

	string & operator += (long lValue) 
	{
		char cBuffer[16];
		itoa(lValue,cBuffer,10);
		*this += cBuffer;
		return *this;
	}

	string & operator += (const char * pText) 
	{
		if (!pText || !pText[0]) return *this;
		dword dwSLen = strlen(pText);
		dword dwNewSize = Len() + dwSLen;
		Reserve(dwNewSize);
		memcpy(&pBuffer[dwSize],pText,dwSLen); 
		dwSize = dwNewSize;
		SetZero();
		return *this;
	}

	friend string operator + (string & s1, float fValue) 
	{
		string sRes = s1; sRes += fValue; return sRes;
	}

	friend string operator + (string & s1, long lValue) 
	{
		string sRes = s1; sRes += (long)lValue; return sRes;
	}

	friend string operator + (string s1, string s2) 
	{		
		string sRes = s1; sRes += s2; return sRes;
	}

	void _cdecl Format(char *cFormat, ... ) 
	{
		char	str[1024];
		va_list args;
		va_start(args, cFormat);
		vsprintf(str, cFormat, args);
		va_end(args);

		*this = str;
	}

	void CheckPath()
	{
		dword dwReal = 0;
		dword dwCurr = 0;
		while (dwCurr < Len())
		{
			pBuffer[dwReal] = pBuffer[dwCurr];
			if (pBuffer[dwCurr] == '\\') 
				while (pBuffer[dwCurr + 1] == '\\' && dwCurr < Len() - 1) dwCurr++;
			dwReal++;
			dwCurr++;
		}
		dwSize = dwReal;
		SetZero();
	}

	void TrimLeft()
	{
		if (!pBuffer || !dwSize) return;

		char *pTemp = pBuffer;
		while ((*pTemp == ' ' || *pTemp == '\t') && (*pTemp != '\0')) pTemp++;

		// do nothing if string is already trimed
		if (pTemp == pBuffer) return;

		if (dword(pTemp - pBuffer) == Len()) dwSize = 0;
		else
		{
			dwSize = Len() - (pTemp - pBuffer);
			memcpy(pBuffer,pTemp,dwSize);
			SetZero();
		}
	}

	void TrimRight()
	{
		if (!pBuffer || !dwSize) return;

		char *pTemp = pBuffer + Len() - 1;
		while ( (*pTemp == ' ' || *pTemp == '\t') && (pTemp >= pBuffer) ) pTemp--;

		// do nothing if string is already trimed
		if (pTemp == pBuffer + Len() - 1) return;

		if (pTemp < pBuffer) dwSize = 0;
		else 
		{ 
			dwSize = dword(pTemp - pBuffer) + 1; 
			SetZero(); 
		}
	}

	void Trim()
	{
		TrimRight();
		TrimLeft();
	}

	bool GetToken(char *pDest, dword & dwShift, char *pSkip)
	{
		if (!pSkip || dwShift >= Len()-1) return false;
		for (dword i=dwShift;i<Len();i++) if (GetBuffer()[i] == pSkip[0])
		{
			strncpy(pDest,&GetBuffer()[dwShift],i - dwShift);
			pDest[i - dwShift] = 0;
			dwShift = i + 1;
			return true;
		}
		return true;
	}

	// constructors area
	string(dword _dwAdd = STRING_DEFAULT_ADD)
	{
		Init(_dwAdd);
	}

	// char * constructor
	string(const char *pText, dword _dwAdd = STRING_DEFAULT_ADD) 
	{
		Init(_dwAdd);
		if (!pText || !pText[0]) return;
		dwSize = strlen(pText);

		Reserve(dwSize);
		memcpy(pBuffer,pText,dwSize); SetZero();
	}

	// string constructor
	string(string & sText, dword _dwAdd = STRING_DEFAULT_ADD) 
	{
		Init(_dwAdd);
		if (sText.IsEmpty()) return;
		(*this) = sText;
	}

	~string()
	{
		DelAll();
	}

	void DelAll()
	{
		DELETE(pBuffer);
		dwSize = 0;
		dwCurSize = 0;
	}
};

#endif