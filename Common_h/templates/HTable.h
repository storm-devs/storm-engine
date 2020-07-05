// TEMPLATE HASH TABLE CLASS
#ifndef TEMPLATE_HASH_TABLE_HPP
#define TEMPLATE_HASH_TABLE_HPP

#include "tcommon.h"
#include "array.h"

template< class _Ty > class htable
{
private:
	struct subelem
	{
		char	* pStr;
		dword	dwHashFullValue;
		_Ty		_T;
	};

	struct elem
	{
		array<subelem>	aSubElements;

		elem(const char * pFileName, long iFileLine) : aSubElements(pFileName, iFileLine, 2) {}
	};

	_Ty				_TBadFind;
	elem			* pElements;
	dword			dwMask;
	dword			dwSize;
	const char		* pFileName;
	long			iFileLine;

public:
	htable(const char * pFileName, long iFileLine, dword _dwSize = 64)
	{
		this->dwSize = _dwSize;
		this->dwMask = dwSize - 1;
		this->pElements = null;
		this->pFileName = pFileName;
		this->iFileLine = iFileLine;

		Reset();
	}

	~htable() 
	{ 
		DelAll();
	}

	// DelAll and delete all pointers (can be compile if (_Ty) is pointer)
	void DelAllWithPointers()
	{
		if (pElements)
		{
			for (dword i=0; i<dwSize; i++)
			{
				elem * pE = &pElements[i];
				for (dword j=0; j<pE->aSubElements.Size(); j++)
				{
					DELETE(pE->aSubElements[j]._T);
					DELETE(pE->aSubElements[j].pStr);
				}
				pE->aSubElements.DelAll();

				pElements[i].~elem();
			}
			//DELETE_ARRAY(pElements);
			DELETE(pElements);
		}

		DELETE(_TBadFind);
	}

	void Reset()
	{
		DelAll();
		
		//pElements = NEW elem[dwSize]; 
		pElements = (elem*)resize(pElements, sizeof(elem) * dwSize, (char*)pFileName, iFileLine); 
		
		if (pElements) for (dword i=0; i<dwSize; i++)
		{
			new('a', &pElements[i]) elem(pFileName, iFileLine);
			pElements[i].aSubElements.SetFileLine(pFileName, iFileLine);
		}
	}

	void DelAll()
	{
		if (!pElements) return;

		for (dword i=0; i<dwSize; i++)
		{
			elem * pE = &pElements[i];
			for (dword j=0; j<pE->aSubElements.Size(); j++)
			{
				DELETE(pE->aSubElements[j].pStr);
			}
			pE->aSubElements.DelAll();

			pElements[i].~elem();
		}
		//DELETE_ARRAY(pElements);
		DELETE(pElements);
	}

	void SetBadFind(_Ty _T)
	{
		_TBadFind = _T;
	}

	dword Add(const char * pStr, const _Ty & _T)
	{
		Assert(pStr);
		dword dwHash = string::HashNoCase(pStr);
		elem * pE = &pElements[dwHash & dwMask];
		
		subelem htse;
		COPY_STRING(htse.pStr, pStr);
		htse._T = _T;
		htse.dwHashFullValue = string::HashNoCase(pStr);

		pE->aSubElements.Add(htse);
		return pE->aSubElements.Size();
	}

	bool Find(dword dwHashFullValue, const char * pStr, _Ty & _T) const
	{
		Assert(pStr && pElements);
		const elem & pE = pElements[dwHashFullValue & dwMask];
		for (dword i=0; i<pE.aSubElements.Size(); i++) 
		{
			const subelem & pSE = pE.aSubElements[i];
			if (dwHashFullValue != pSE.dwHashFullValue) continue;
			if (stricmp(pSE.pStr , pStr) == 0) 
			{
				_T = pSE._T;
				return true;
			}
		}
		return false;
	}

	bool Find(const char * pStr, _Ty & _T) const
	{
		Assert(pStr && pElements);
		dword dwHash = string::HashNoCase(pStr);
		return Find(dwHash, pStr, _T);
	}

	__forceinline const _Ty & Get(const char * pStr) const
	{
		Assert(pStr && pElements);
		dword dwHashFullValue = string::HashNoCase(pStr);
		elem & pE = pElements[dwHashFullValue & dwMask];
		for (dword i=0; i<pE.aSubElements.Size(); i++) 
		{
			subelem & pSE = pE.aSubElements[i];
			if (dwHashFullValue != pSE.dwHashFullValue) continue;
			if (stricmp(pSE.pStr , pStr) == 0) return pSE._T;
		}
		return _TBadFind;
	}

	_Ty & operator [] (const char * pStr) { return (_Ty&)Get(pStr); }
	const _Ty & operator [] (const char * pStr) const { return Get(pStr); }

	bool Del(const char * pStr)
	{
		Assert(pStr && pElements);
		dword dwHashFullValue = string::HashNoCase(pStr);

		elem & pE = pElements[dwHashFullValue & dwMask];
		for (dword i=0; i<pE.aSubElements.Size(); i++) 
		{
			subelem & pSE = pE.aSubElements[i];
			if (dwHashFullValue != pSE.dwHashFullValue) continue;
			if (stricmp(pSE.pStr , pStr) == 0) 
			{
				DELETE(pSE.pStr);
				pE.aSubElements.ExtractNoShift(i);
				return true;
			}
		}
		return false;
	}

public:
	class iterator
	{
	public:
		iterator(htable<_Ty> & _hTable) :
			hTable(_hTable)	{}

		inline void Begin() { iIndex = 0; iSubIndex = -1; Next(); }
		inline bool IsDone() { return iIndex >= (long)hTable.dwSize; }
		inline void Next() 
		{
			while (!IsDone())
			{
				iSubIndex++;
				if (iSubIndex >= hTable.pElements[iIndex].aSubElements)
				{
					iIndex++;
					iSubIndex = -1;
					continue;
				}
				break;
			}
		}

		inline const char * GetName()
		{
			if (IsDone()) return null;
			return hTable.pElements[iIndex].aSubElements[iSubIndex].pStr;
		}

		inline _Ty & Get()
		{
			if (IsDone()) return hTable._TBadFind;
			return hTable.pElements[iIndex].aSubElements[iSubIndex]._T;
		}

	private:
		int iIndex, iSubIndex;
		htable<_Ty> & hTable;
	};

};

#endif
