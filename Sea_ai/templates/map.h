#ifndef TEMPLATE_MAP_HPP
#define TEMPLATE_MAP_HPP

#include "tcommon.h"

#define INVALID_MAP_INDEX		0xFFFFFFFF

template<class _K, class _V, class _A = allocator< pair<_K,_V> > > class map 
{
	_A	_Al;
public:
	map(dword _dwAdd = 8) : _Al(_dwAdd)
	{
	}

	virtual ~map() 
	{
	}

	dword Find(_K Key) 
	{
		for (dword i=0;i<Size();i++) if (Key == _Al.pElements[i].first) return i;
		return INVALID_MAP_INDEX;
	}

	dword Add(_K Key)
	{
		_Al.Reserve(Size() + 1);
		_Al.pElements[Size()].first = Key;
		_Al.pElements[Size()].second = 0;
		_Al.IncSize(); 
		return Size()-1;
	}

	dword Add(_K Key, _V Value)
	{
		_Al.Reserve(Size() + 1);
		_Al.pElements[Size()].first = Key;
		_Al.pElements[Size()].second = Value;
		_Al.IncSize(); 
		return Size()-1;
	}

	_V & operator [] (_K Key)
	{
		dword dwIdx = Find(Key);
		if (dwIdx == INVALID_MAP_INDEX)	dwIdx = Add(Key);
		return _Al.pElements[dwIdx].second;
	}

	inline dword Size()	const { return _Al.Size(); } 
};

#endif