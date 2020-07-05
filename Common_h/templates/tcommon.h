#ifndef TEMPLATES_COMMON_HPP
#define TEMPLATES_COMMON_HPP

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef _NOBLOOD_
	#include "..\..\common_h\vmodule_api.h"
	#include "..\..\common_h\memop.h"
	#include "..\..\common_h\d_types.h"
	#include "..\..\common_h\defines.h"
	#include "..\..\common_h\Exs.h"
#else
	#include <memory.h>

	typedef unsigned long dword;
	typedef unsigned short word;
	typedef unsigned char byte;

	#ifdef RGB
		#undef RGB
	#endif
	#ifdef DELETE
		#undef DELETE
	#endif
	#ifdef SQR
		#undef SQR
	#endif

	inline void __XxX_Assert__(bool expression) { if (!expression) _asm int 3 }

	#define FRAND(x)		( (x) * (float)rand()/(float)RAND_MAX )

	#define DELETE(x)		{ free(x); x = null; }
	#define RESIZE(x, y)	realloc((x), (y))

	#define resize(x, y, z, w)	realloc((x), (y))

	#define null 0

	#define Assert(exp)		{ __XxX_Assert__((exp) != 0); }
#endif

inline void operator delete(void * p, char c,  void * pAddress) {}
inline void * operator new(size_t Size, char c,  void * pAddress) { return pAddress; }

template<class _T1, class _T2> class pair
{
public:
	pair() : first(_T1()), second(_T2()) {}
	
	pair(const _T1 & _V1, const _T2 & _V2) : first(_V1), second(_V2) {}
	
	template<class U, class V> pair(const pair<U, V> &p) 
		: first(p.first), second(p.second) {}

	_T1 first;
	_T2 second;
};

template<class _Ty> class allocator
{
public:
	typedef allocator<_Ty>	_Mal;

	// copy constructor
	allocator(const _Mal & _A)			// FIX-ME ????
	{
		pE = null; 
		dwAddElements = _A.dwAddElements;
		dwMaxElements = 0;
		dwNumElements = 0;
		pFileName = _A.pFileName;
		iFileLine = _A.iFileLine;
		if (_A.dwNumElements)
		{
			Reserve(_A.dwNumElements);
			for (dword i=0; i<Size(); i++)
				pE[i] = _A.pE[i];
		}
	}
	// constructor
	allocator(const char * pFileName, long iFileLine, dword _dwAdd)
	{
		pE = null; 
		Assert(_dwAdd != 0);
		dwAddElements = _dwAdd;
		dwMaxElements = 0;
		dwNumElements = 0;

		this->pFileName = pFileName;
		this->iFileLine = iFileLine;
	}

	// default destructor
	~allocator() 
	{
		DelAll();
	};

	// return number of elements
	__forceinline dword Size() const { return dwNumElements; };
	// very dangerous function
	__forceinline void Empty() { dwNumElements = 0; }		
	// construct new element
	__forceinline void Construct(dword dwIndex)
	{
		new('a', &pE[dwIndex]) _Ty;
	}
	// desctruct element
	__forceinline void Destroy(dword dwIndex)
	{
		((_Ty*)&pE[dwIndex])->~_Ty();
	}
	// delete all elementd
	__forceinline void DelAll()
	{	
		for (dword i=0; i<dwNumElements; i++) Destroy(i);

		dwMaxElements = 0;
		dwNumElements = 0;
		void * pTE = pE; DELETE(pTE);
		pE = null;
	}

//protected:
	// set new add_chunk size
	__forceinline void SetAddElements(dword dwAdd)
	{
		dwAddElements = dwAdd;
	}
	// reserve elements 
	__forceinline void Reserve(dword dwNum)
	{	
		dword dwNewSize = dwAddElements * (((dwNum-1) / dwAddElements) + 1);
		if (dwMaxElements == dwNewSize) return;
		pE = (_Ty*)resize(pE, sizeof(_Ty) * dwNewSize, (char*)pFileName, (dword)iFileLine);
		//if (dwNewSize) Assert(pE);
		dwMaxElements = dwNewSize;
	}
	// decrease elements number
	__forceinline void DecSize() 
	{		
		//Assert(dwNumElements > 0); 
		dwNumElements--; 
		Reserve(dwNumElements); 
	}
	// increase elements number
	__forceinline void IncSize() {	dwNumElements++; }

	// Setup filename and line for memory allocation 
	__forceinline void SetFileLine(const char * pFileName, long iFileLine) 
	{ 
		this->pFileName = pFileName;
		this->iFileLine = iFileLine; 
	}
	__forceinline dword GetMaxElements() { return dwMaxElements; }

	_Ty		* pE;				// elements array

protected:
	dword	dwNumElements;		// number of elements
	dword	dwAddElements;		// number elements 
	dword	dwMaxElements;		// current maximum elements

	const char * pFileName;
	long iFileLine;
};

#endif