#ifndef TEMPLATES_COMMON_HPP
#define TEMPLATES_COMMON_HPP

#include "..\..\common_h\d_types.h"
#include "..\..\common_h\defines.h"

template<class _T1, class _T2> class pair
{
public:
	pair() : first(_T1()), second(_T2()) {}
	
	pair(const _T1& _V1, const _T2& _V2) : first(_V1), second(_V2) {}
	
	template<class U, class V> pair(const pair<U, V> &p) 
		: first(p.first), second(p.second) {}

	_T1 first;
	_T2 second;
};

template<class _Ty> class allocator
{
public:
	typedef allocator<_Ty>	_Mal;

	allocator(const _Mal & _A)
	{
		pE = null; 
		dwAddElements = _A.dwAddElements;
		dwMaxElements = 0;
		dwNumElements = 0;
	}

	allocator(dword _dwAdd)
	{
		pE = null; 
		dwAddElements = _dwAdd;
		dwMaxElements = 0;
		dwNumElements = 0;
	}

	virtual ~allocator() 
	{
		DelAll();
	};

	inline dword Size() const { return dwNumElements; };

	virtual void Empty() { dwNumElements = 0; }		// very dangerous function

	virtual void DelAll()
	{	
		dwMaxElements = 0;
		dwNumElements = 0;
		DELETE_ARRAY(pE);
	}

//protected:

	virtual void Reserve(dword dwNum)
	{	
		dword dwNewSize = dwAddElements * (((dwNum-1) / dwAddElements) + 1);
		if (dwMaxElements == dwNewSize) return;
		_Ty* pNewElements = NEW _Ty[dwNewSize];
		for (dword i=0;i<Size();i++) pNewElements[i] = pE[i];
		DELETE_ARRAY(pE);
		pE = pNewElements;
		dwMaxElements = dwNewSize;
	}

	inline void DecSize() 
	{		
		Assert(dwNumElements>0); 
		dwNumElements--; 
		Reserve(dwNumElements); 
	}

	inline void IncSize() {	dwNumElements++; }

	_Ty		* pE;				// elements array

protected:
	dword	dwNumElements;			// number of elements
	dword	dwAddElements;			// number elements 
	dword	dwMaxElements;			// current maximum elements
};

template<class _Ty> class nonew_allocator : public allocator<_Ty>
{
public:
	nonew_allocator(dword _dwAdd) : allocator<_Ty>(_dwAdd) {};
	
	~nonew_allocator() 
	{
		DelAll();
	};

	virtual void DelAll()
	{	
		dwNumElements = 0;
		FREE(pE); 
	}

	virtual void Reserve(dword dwNum)
	{	
		dword dwNewSize = dwAddElements * ((dwNum / dwAddElements) + 1);
		if (dwMaxElements == dwNewSize) return;
		_Ty* pNewElements = (_Ty*)malloc(sizeof(_Ty) * dwNewSize);
		for (dword i=0;i<Size();i++) pNewElements[i] = pE[i];
		FREE(pE);
		pE = pNewElements;
		dwMaxElements = dwNewSize;
	}
};

#endif