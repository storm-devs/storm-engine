#ifndef TEMPLATE_MAP_HPP
#define TEMPLATE_MAP_HPP

#include "tcommon.h"

#define INVALID_MAP_INDEX		0xFFFFFFFF

template< class _K, class _V > class map 
{
	allocator< pair< _K,_V > >	_Al;
public:
	map(const char * pFileName, long iFileLine, dword _dwAdd = 8) : _Al(pFileName, iFileLine, _dwAdd)
	{
	}

	~map() 
	{
	}

	dword Find(const _K & Key) const
	{
		for (dword i=0; i<Size(); i++) if (Key == _Al.pE[i].first) return i;
		return INVALID_MAP_INDEX;
	}

	dword Add(const _K & Key)
	{
		_Al.Reserve(Size() + 1);
		_Al.pE[Size()].first = Key;
		//_Al.pE[Size()].second = 0;
		_Al.IncSize(); 
		return Size()-1;
	}

	dword Add(const _K & Key, const _V & Value)
	{
		_Al.Reserve(Size() + 1);
		_Al.pE[Size()].first = Key;
		_Al.pE[Size()].second = Value;
		_Al.IncSize(); 
		return Size()-1;
	}

	_V & operator [] (const _K & Key)
	{
		dword dwIdx = Find(Key);
		if (dwIdx == INVALID_MAP_INDEX)	dwIdx = Add(Key);
		return _Al.pE[dwIdx].second;
	}

	inline dword Size()	const { return _Al.Size(); } 
};

#endif