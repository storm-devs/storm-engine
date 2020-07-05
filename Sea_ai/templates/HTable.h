// TEMPLATE HASH TABLE CLASS
#ifndef TEMPLATE_HASH_TABLE_HPP
#define TEMPLATE_HASH_TABLE_HPP

#include "tcommon.h"
#include "array.h"

template< class _Ty > class htable
{
private:
	struct htable_subelement
	{
		char	* pStr;
		dword	dwHashFullValue;
		_Ty		_T;
	};

	struct htable_element
	{
		array<htable_subelement>	aSubElements;

		htable_element() : aSubElements(2) {}
	};

	htable_element	* pElements;
	dword			dwMask;
	dword			dwSize;

public:
	htable(dword _dwSize = 128)
	{
		dwSize = _dwSize;
		dwMask = dwSize - 1;
		pElements = NEW htable_element[dwSize];
		dword dwSE = sizeof(htable_element);
	}

	virtual ~htable() 
	{ 
		for (dword i=0; i<dwSize; i++)
		{
			htable_element * pE = &pElements[i];
			for (dword j=0; j<pE->aSubElements.Size(); j++)
			{
				DELETE(pE->aSubElements[j].pStr);
			}
			pE->aSubElements.DelAll();
		}
		DELETE_ARRAY(pElements);
	}

	dword Add(dword dwHash, char * pStr, _Ty _T)
	{
		htable_element * pE = &pElements[dwHash & dwMask];
		
		htable_subelement htse;
		COPY_STRING(htse.pStr, pStr);
		htse._T = _T;
		htse.dwHashFullValue = dwHash;

		pE->aSubElements.Add(htse);
		return pE->aSubElements.Size();
	}

	dword Add(char * pStr, _Ty _T)
	{
		dword dwHash = hash_string(pStr);
		
		return Add(dwHash, pStr, _T);
	}

	bool Find(dword dwHashFullValue, char * pStr, _Ty & _T)
	{
		htable_element & pE = pElements[dwHashFullValue & dwMask];
		for (dword i=0; i<pE.aSubElements.Size(); i++) 
		{
			htable_subelement & pSE = pE.aSubElements[i];
			if (dwHashFullValue != pSE.dwHashFullValue) continue;
			if (stricmp(pSE.pStr , pStr) == 0) 
			{
				_T = pSE._T;
				return true;
			}
		}
		return false;
	}

	bool Find(char * pStr, _Ty & _T)
	{
		dword dwHash = hash_string(pStr);
		return Find(dwHash, pStr, _T);
	}
};

#endif
