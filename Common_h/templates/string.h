// string class
#ifndef STRING_HPP
#define STRING_HPP

#include "tcommon.h"

#define STRING_DEFAULT_ADD		8

class string 
{
private:
	char	* pBuffer;
	dword	dwSize;

	dword	dwCurSize, dwAddSize;

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
	void Reserve(dword dwReserveSize)
	{
		dword dwNewSize = dwAddSize * (1 + dwReserveSize / dwAddSize);
		if (dwCurSize >= dwNewSize) return;
		dwCurSize = dwNewSize;	
		pBuffer = (char*)RESIZE(pBuffer, dwCurSize); 
	}

	inline bool			IsEmpty()  	const { return (dwSize == 0); }
	inline const char *	GetBuffer()	const { return pBuffer ? pBuffer : ""; }
	inline const char *	c_str()		const { return pBuffer ? pBuffer : ""; }
	inline dword		Len()  		const { return dwSize; }
	inline dword		Size()  	const { return Len(); }

	// return pointer for direct operations with string buffer
	inline char *		GetDataBuffer()	{ Assert(pBuffer); return pBuffer; };
	// return string buffer size for direct operations with string buffer
	inline dword		GetDataSize()	{ return dwCurSize; };
	// set new string size
	inline void			SetDataSize(dword dwNewSize)	{ Assert(dwNewSize < dwCurSize); dwSize = dwNewSize; };

	// Operator return string length
	inline dword operator () (void) const { return Size(); }

	// operators area

	// string compare
	inline bool operator == (const char * pString) const { return (pString) ? stricmp(GetBuffer(), pString)==0 : false; }
	inline bool operator == (const string & s1) const { return *this == s1.GetBuffer(); }

	inline bool operator != (const char * pString) const { return !(*this == pString); }
	inline bool operator != (const string & s1) const { return !(*this == s1); }

	inline bool operator > (const char * pString) const { return (pString) ? stricmp(GetBuffer(), pString) > 0 : false; }
	inline bool operator > (const string & s1) const { return (*this > s1.GetBuffer()); }

	inline bool operator < (const char * pString) const { return !(*this > pString); }
	inline bool operator < (const string & s1) const { return !(*this > s1.GetBuffer()); }

	inline bool operator >= (const char * pString) const { return (pString) ? stricmp(GetBuffer(), pString) >= 0 : false; }
	inline bool operator >= (const string & s1) const { return (*this >= s1.GetBuffer()); }

	inline bool operator <= (const char * pString) const { return !(*this >= pString); }
	inline bool operator <= (const string & s1) const { return !(*this >= s1.GetBuffer()); }

	inline bool CompareLen(const string & s, dword dwLen)
	{
		if (Len() < dwLen || s.Len() < dwLen) return false;
		return strnicmp(GetBuffer(), s.GetBuffer(), dwLen) == 0;
	}

	// type conversion, to (const char *)
	inline operator const char * () const { return GetBuffer(); }

	string & operator = (const char * pText) 
	{ 
		if (!pText || !pText[0]) { Empty(); return *this;}
		dwSize = (dword)strlen(pText);

		Reserve(dwSize);
		memcpy(pBuffer, pText, dwSize); SetZero();

		return (*this);
	}

	string & operator = (const string & sText) 
	{ 
		dwSize = sText.Len();
		if (sText.IsEmpty()) { Empty(); return *this; }

		Reserve(dwSize);
		memcpy(pBuffer,sText,dwSize); SetZero();

		return *this;
	}

	string & operator += (const string & sText) 
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
		char temp[20];
		_gcvt(fValue,7,temp);
		*this += temp;
		return *this;
	}

	string & operator += (long lValue) 
	{
		char cBuffer[20];
		itoa(lValue,cBuffer,10);
		*this += cBuffer;
		return *this;
	}

	string & operator += (dword dwValue) 
	{
		char cBuffer[20];
		_ultoa(dwValue, cBuffer, 10);
		*this += cBuffer;
		return *this;
	}

	string & operator += (char cSym) 
	{
		Reserve(dwSize + 1);
		pBuffer[dwSize] = cSym;
		dwSize++;
		SetZero();
		return *this;
	}

	string & operator += (const char * pText) 
	{
		if (!pText || !pText[0]) return *this;
		dword dwSLen = (dword)strlen(pText);
		dword dwNewSize = Len() + dwSLen;
		Reserve(dwNewSize);
		memcpy(&pBuffer[dwSize],pText,dwSLen); 
		dwSize = dwNewSize;
		SetZero();
		return *this;
	}

	friend string operator + (const string & s1, dword dwValue) 
	{
		string sRes = s1; sRes += dwValue; return sRes;
	}

	friend string operator + (const string & s1, float fValue) 
	{
		string sRes = s1; sRes += fValue; return sRes;
	}

	friend string operator + (const string & s1, long lValue) 
	{
		string sRes = s1; sRes += (long)lValue; return sRes;
	}

	friend string operator + (const string s1, const string & s2) 
	{		
		string sRes = s1; sRes += s2; return sRes;
	}

	friend string operator - (const string s1, const string & s2) 
	{		
		string sRes = s1; 
		if (EqualPostfix(sRes.c_str(), s2.c_str()))
		{
			sRes.dwSize -= s2.Len();
			sRes.SetZero();
		}
		return sRes;
	}

	string & _cdecl Format(char *cFormat, ... ) 
	{
		char	str[1024];
		va_list args;
		va_start(args, cFormat);
		vsprintf(str, cFormat, args);
		va_end(args);

		*this = str;

		return *this;
	}

	string & Insert(dword dwIndex, const char * pInsString)
	{
		if (!pInsString) return *this;
		string sTmp = pInsString;
		return Insert(dwIndex, sTmp);
	}

	string & Delete(dword dwIndex, dword dwLen)
	{
		if (dwLen == 0 || dwIndex >= Len()) return *this;
		if (dwLen > Len() - dwIndex)
		{
			dwSize = dwIndex;
			SetZero();
			return *this;
		}
		memmove(&pBuffer[dwIndex], &pBuffer[dwIndex + dwLen], Len() - dwIndex - dwLen);
		dwSize = Len() - dwLen;
		SetZero();
		return *this;
	}

	string & Insert(dword dwIndex, const string & sInsString)
	{
		if (sInsString.IsEmpty()) return *this;
		if (dwIndex > Len()) dwIndex = Len();
		Reserve(Len() + sInsString.Len());
		dword dwShiftSize = Len() - dwIndex;
		if(dwShiftSize) memmove(&pBuffer[dwIndex + sInsString.Len()], &pBuffer[dwIndex], dwShiftSize);
		memcpy(&pBuffer[dwIndex], sInsString.GetBuffer(), sInsString.Len());
		dwSize += sInsString.Len();
		SetZero();
		return *this;
	}

	string & CheckPath()
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
		return *this;
	}

	string & TrimLeft()
	{
		if (!pBuffer || !dwSize) return *this;;

		char *pTemp = pBuffer;
		while ((*pTemp == ' ' || *pTemp == '\t') && (*pTemp != '\0')) pTemp++;

		// do nothing if string is already trimed
		if (pTemp == pBuffer) return *this;

		if (dword(pTemp - pBuffer) == Len()) dwSize = 0;
		else
		{
			dwSize = Len() - (pTemp - pBuffer);
			memcpy(pBuffer, pTemp, dwSize);
			SetZero();
		}
		return *this;
	}

	string & TrimRight()
	{
		if (!pBuffer || !dwSize) return *this;

		char *pTemp = pBuffer + Len() - 1;
		while ( (*pTemp == ' ' || *pTemp == '\t') && (pTemp >= pBuffer) ) pTemp--;

		// do nothing if string is already trimed
		if (pTemp == pBuffer + Len() - 1) return *this;

		if (pTemp < pBuffer) dwSize = 0;
		else 
		{ 
			dwSize = dword(pTemp - pBuffer) + 1; 
			SetZero(); 
		}
		return *this;
	}

	string & Trim()
	{
		TrimRight();
		TrimLeft();

		return *this;
	}

	// Convert a string to lowercase.
	string & Lower()	{ if (pBuffer) _strlwr(pBuffer); return *this; }

	// Convert a string to uppercase.
	string & Upper()	{ if (pBuffer) _strupr(pBuffer);	return *this; }

	bool GetToken(char * pDest, dword & dwShift, char * pSkip)
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
	string(const char * pText, dword _dwAdd = STRING_DEFAULT_ADD) 
	{
		Init(_dwAdd);
		if (!pText || !pText[0]) return;
		dwSize = (dword)strlen(pText);

		Reserve(dwSize);
		memcpy(pBuffer,pText,dwSize); SetZero();
	}

	// string constructor
	string(const string & sText, dword _dwAdd = STRING_DEFAULT_ADD) 
	{
		Init(_dwAdd);
		if (sText.IsEmpty()) return;
		(*this) = sText;
	}

	~string()
	{
		DelAll();
	}

	void Empty()
	{
		dwSize = 0;
		SetZero();
	}

	void DelAll()
	{
		DELETE(pBuffer);
		dwSize = 0;
		dwCurSize = 0;
	}

	static inline unsigned long Hash(const char * str)
	{
		unsigned long hval = 0;
		if (!str) return 0;

		while(*str != '\0')
		{
			hval = (hval<<4) + (unsigned long) *str++;
			unsigned long g = hval & ((unsigned long) 0xf << (32 - 4));
			if(g != 0)
			{
				hval ^= g >> (32 - 8);
				hval ^= g;
			}
		}
		return hval;
	}

	static inline unsigned long HashNoCase(const char * str)
	{
		unsigned long hval = 0;
		while(*str != '\0')
		{
			char c = *str++;
			if(c >= 'A' && c <= 'Z') c += 'a' - 'A';
			hval = (hval<<4) + (unsigned long)c;
			unsigned long g = hval & ((unsigned long) 0xf << (32 - 4));
			if(g != 0)
			{
				hval ^= g >> (32 - 8);
				hval ^= g;
			}
		}
		return hval;
	}

	inline char Last()
	{
		return (pBuffer && Len()) ? pBuffer[Len() - 1] : 0;
	}

	template<class _T> inline char & operator [] (_T Index)
	{
		Assert(Index < (_T)Len());
		return pBuffer[Index];
	}

	// find substring
	long FindSubStr(const char * pStr) const
	{
		if (!pBuffer || !pStr || !pStr[0]) return -1;
		char * pFindSubStr = strstr(pBuffer, pStr);
		return (pFindSubStr) ? dword(pFindSubStr - pBuffer) : -1;
	}
	// if last symbol == cSymbol, delete it
	inline string & DeleteLastSymbol(char cSymbol = '\\')
	{
		if (!Len() || pBuffer[Len() - 1] != cSymbol) return *this;
		dwSize--;
		SetZero();
		return *this;
	}
	// add extension to path if is't exist
	inline string & AddExtention(const char * ext)
	{
		if(!ext || !ext[0]) return *this;
		dword esize = (dword)strlen(ext);
		if(Len() < esize)
		{
			*this += ext;
			return *this;
		}
		const char * pStr = GetBuffer() + Len() - 1;
		const char * pExt = ext + esize - 1;
		for(dword c = 0; c < esize; c++)
		{
			char cs = *pStr--;
			char ce = *pExt--;
			if(cs >= 'A' && cs <= 'Z') cs += 'a' - 'A';
			if(ce >= 'A' && ce <= 'Z') ce += 'a' - 'A';
			if(cs != ce) break;
		}
		if(c == esize) return *this;
		*this += ext;
		return *this;		
	}
	// 
	inline long LastChar(char c) const 
	{
		const char * pStr = GetBuffer();
		for(long i = Len() - 1; i >= 0; i--) if(pBuffer[i] == c) break;
		return i;
	}
	// return relative path = this - relativePath, if can't return this
	inline string & GetRelativePath(const string & relativePath)
	{
		if (Len() < relativePath.Len()) return *this;

		for (dword i=0; i<relativePath.Len(); i++)
		{
			char cs = GetBuffer()[i];
			char ce = relativePath[i];
			if(cs >= 'A' && cs <= 'Z') cs += 'a' - 'A';
			if(ce >= 'A' && ce <= 'Z') ce += 'a' - 'A';
			if (cs != ce) return *this;
		}

		memcpy(pBuffer, &pBuffer[relativePath.Len()], Len() - relativePath.Len());
		dwSize = Len() - relativePath.Len();
		SetZero();
		return *this;
	}
	//
	inline string & GetFilePath(const string & fullPath)
	{
		Empty();
		long i = fullPath.LastChar('\\');
		if(i >= 0)
		{
			Reserve(i + 1);
			for(long j = 0; j <= i; j++) *this += fullPath.pBuffer[j];
		}
		return *this;
	}
	//
	inline string & GetFileName(const string & fullPath)
	{
		Empty();
		long i = fullPath.LastChar('\\');
		if(i >= 0)
		{
			long size = fullPath.dwSize;
			Reserve(size - i);
			for(i++; i < size; i++) *this += fullPath.pBuffer[i];
		}else{
			*this = fullPath;
		}
		return *this;
	}
	//
	inline string & GetFileTitle(const string & fullPath)
	{
		GetFileName(fullPath);
		long i = LastChar('.');
		if(i >= 0)
		{
			Delete(i, dwSize - i + 1);
		}
		return *this;
	}
	//
	inline string & GetFileExt(const string & fullPath)
	{
		Empty();
		long i = fullPath.LastChar('.');
		if(i >= 0)
		{
			long size = fullPath.dwSize;
			Reserve(size - i);
			for(i++; i < size; i++) *this += fullPath.pBuffer[i];
		}
		return *this;
	}
	// return true if Filename - directory(last symbol must be '\')
	inline bool IsDir() const 
	{
		return (Len() && pBuffer[Len() - 1] == '\\');
	}

	// return true if Filename equal with sMask
	inline bool IsFileMask(const string & sMask) const
	{
		if (!Len() || !sMask.Len()) return false;
		if (sMask == "*.*" || sMask == "*") return true;
		
		return tstFileMask(GetBuffer(), sMask.GetBuffer());
	}

	//return true if str ended by postfix string
	inline static bool EqualPostfix(const char * str, const char * postfix)
	{
		if(!str || !postfix) return false;
		if(!str[0] || !postfix[0]) return false;
		long slen = (long)strlen(str);
		long plen = (long)strlen(postfix);
		if(slen < plen) return false;
		if(stricmp(str + slen - plen, postfix) == 0) return true;
		return false;
	}

private:
	// inner recursive mask test, FIX-ME for beauty
	inline bool tstFileMask(const char * pF, const char * pM) const
	{
		long iF = 0, iM = 0;
		while (pF[iF] && pM[iM])
		{
			// return true if current mask symbol is '*' and next is '\0'
			if (pM[iM] == '*' && pM[iM + 1] == '\0') return true;

			// test for '*'
			if (pM[iM] == '*')
			{
				while (true)
				{
					bool bSuccess = tstFileMask(&pF[iF], &pM[iM + 1]);
					if (bSuccess) return true;
					if (pF[iF] == '\0') return false;
					iF++;
				}
			}

			// test for '?'
			if (pM[iM] == '?') { iF++; iM++; continue; }

			// test for any symbol except '*' & '?'
			if (pF[iF] != pM[iM]) return false;

			iF++; iM++;
		}

		if (!pF[iF] && !pM[iM]) return true;

		if (pF[iF] == '\0' && pM[iM])
		{
			for (dword i=iM; i<strlen(pM); i++)
				if (pM[i] != '*') return false;
			return true;
		}

		return false;
	}

};

#endif