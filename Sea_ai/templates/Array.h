// TEMPLATE ARRAY CLASS
#ifndef TEMPLATE_ARRAY_HPP
#define TEMPLATE_ARRAY_HPP

#include "tcommon.h"

//#pragma pack(push, 8)

#define INVALID_ARRAY_INDEX			0xFFFFFFFF

template< class _Ty, class _A = allocator<_Ty> > class array
{
	_A _Al;
public:
	typedef array<_Ty, _A>	_Myt;

	array(dword _dwAdd = 8) : _Al(_dwAdd)
	{	
	}

	virtual ~array() { }
	virtual void DelAll()	{ _Al.DelAll(); } 
	virtual void Empty()	{ _Al.Empty(); }		// very dangerous function !!!

	void Insert(_Ty _T, dword dwIndex)
	{ 
		_Al.Reserve(Size() + 1);
		if (dwIndex != Size())
			for (dword i=Size(); i > dwIndex; i--)
				GetElement(i) = GetElement(i-1);
		_Al.pE[dwIndex] = _T; 
		_Al.IncSize();
	}

	void Extract(dword dwIndex)
	{ 
		Assert(dwIndex < Size());
		for (dword i=dwIndex; i < Size() - 1; i++)
			GetElement(i) = GetElement(i+1);
		_Al.DecSize();
	}

	void ExtractNoShift(dword dwIndex)
	{ 
		Assert(dwIndex<Size());
		if (Size()>1 && dwIndex != Size()-1) _Al.pE[dwIndex] = _Al.pE[Last()];
		_Al.DecSize();
	}

	dword Add(_Ty _T)
	{	
		_Al.Reserve(Size() + 1);
		_Al.pE[Size()] = _T; 
		_Al.IncSize(); 
		return Size()-1; 
	}

	dword Add()
	{	
		_Al.Reserve(Size() + 1);
		_Al.IncSize(); 
		return Size()-1; 
	}

	dword Find(_Ty _T) const
	{	
		for (dword i=0;i<Size();i++) if (_T == _Al.pE[i]) return i; 
		return INVALID_ARRAY_INDEX; 
	}

	void DelIndex(dword dwIndex)
	{		
		Assert(dwIndex<Size());
		if (dwIndex != Last()) _Al.pE[dwIndex] = _Al.pE[Last()];
		_Al.DecSize();
	}

	void Del(_Ty _T)
	{	
		dword dwIndex = Find(_T); 
		if (dwIndex==INVALID_ARRAY_INDEX) return;
		_Al.pE[dwIndex] = _Al.pE[Last()];
		_Al.DecSize();
	}

	// add new and delete
	void AddDel(_Ty _Tdel, _Ty _Tadd)
	{	
		dword dwIndex = Find(_Tdel);
		if (dwIndex!=INVALID_ARRAY_INDEX) 
			_Al.pE[dwIndex] = _Tadd; 
		else 
			Add(_Tadd); 
	}

	inline dword Size()	const { return _Al.Size(); } 
	inline dword Last()	const { return Size()-1; }
	inline dword First() const { return 0; }

	__forceinline _Ty & GetElement(dword dwIndex)
	{	
		return (_Ty&)_Al.pE[dwIndex]; 
	}

	__forceinline _Ty & operator [] (dword dwIndex)
	{	
		return (_Ty&)_Al.pE[dwIndex]; 
	}

	void Copy(_Myt & _X)
	{
		DelAll();
		for (dword i=0;i<_X.Size();i++) Add(_X[i]);
	}

	_Myt & operator = (_Myt & _X)
	{
		if (this == &_X) return (*this);
		Copy(_X);
		return (*this);
	}

	void Swap(dword i, dword j)
	{
		Assert(i<Size() && j<Size());
		_Ty _T = GetElement(i);
		GetElement(i) = GetElement(j);
		GetElement(j) = _T;
	}

	void Sort(bool bGreatest = true)
	{
		if (Size() < 2) return;
		for (dword i=0;i<Size();i++)
			for (dword j=i+1;j<Size();j++) 
			{
				dword ii = j, jj = i;
				if (!bGreatest) { ii = i; jj = j; }
				if (GetElement(ii) < GetElement(jj)) Swap(ii,jj);
			}
	}
};
	
//#pragma pack(pop)

#endif