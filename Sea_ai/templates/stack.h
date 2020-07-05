#ifndef TEMPLATE_STACK_HPP
#define TEMPLATE_STACK_HPP

#include "tcommon.h"

template<class _Ty, class _A = allocator<_Ty> > class stack
{
	_A	_Al;
public:
	stack(dword _dwAdd = 16) : _Al(_dwAdd)
	{
	}

	virtual ~stack() 
	{
	}

	inline _Ty & Top()
	{
		return (_Ty&)_Al.pElements[Size()-1];
	}

	void Push(_Ty Item)
	{
		_Al.Reserve(Size() + 1);
		(_Ty&)_Al.pElements[Size()] = Item;
		_Al.IncSize(); 
	}

	inline void Pop()	{ if (Size()) _Al.DecSize(); };
	
	inline dword Size()	{ return _Al.Size(); } 
};

#endif