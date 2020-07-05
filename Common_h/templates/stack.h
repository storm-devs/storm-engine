#ifndef TEMPLATE_STACK_HPP
#define TEMPLATE_STACK_HPP

#include "tcommon.h"

template< class _Ty > class stack
{
	allocator<_Ty>	_Al;
public:
	stack(const char * pFileName, long iFileLine, dword _dwAdd = 16) : _Al(pFileName, iFileLine, _dwAdd) {};
	~stack() {};

	void DelAll() { _Al.DelAll(); }

	// Return number of elements
	__forceinline dword Size() const	{ return _Al.Size(); } 
	// Operator return number of elements
	__forceinline operator long () const { return long(_Al.Size()); }
	// Operator return number of elements
	__forceinline dword operator () (void) const { return _Al.Size(); }

	__forceinline bool	IsEmpty() const	{ return Size() == 0; } 

	__forceinline _Ty & Push()
	{
		if (Size() + 1 >= _Al.GetMaxElements()) _Al.Reserve(Size() + 1);
		_Al.Construct(Size());
		_Al.IncSize(); 

		return Top();
	}

	__forceinline _Ty & Push(const _Ty & _T)
	{
		if (Size() + 1 >= _Al.GetMaxElements()) _Al.Reserve(Size() + 1);
		_Al.Construct(Size());
		_Al.pE[Size()] = _T;
		_Al.IncSize(); 

		return Top();
	}

	__forceinline void Pop()						
	{ 
		//Assert(Size());
		_Al.Destroy(Size() - 1);
		_Al.DecSize(); 

		//return Size();
	}

	__forceinline _Ty & Top(long iLevel = 0)
	{ 
		Assert(Size() - 1 + iLevel < Size());
		return _Al.pE[Size() - 1 + iLevel]; 
	}

	__forceinline const _Ty & Top(long iLevel = 0) const	
	{ 
		Assert(Size() - 1 + iLevel < Size());
		return _Al.pE[Size() - 1 + iLevel]; 
	}

	__forceinline const _Ty & operator [] (long iIndex) const { return Top(iIndex); }
	__forceinline _Ty & operator [] (long iIndex) { return Top(iIndex); }
};

#endif