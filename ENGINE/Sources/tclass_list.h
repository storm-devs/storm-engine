#ifndef _TCLASS_LIST_
#define _TCLASS_LIST_

#include "..\..\common_h\memop.h"

template <class ClassType>
class TCLASS_LIST
{
	ClassType ** pTable;
	DWORD		 nClassesNum;
public:
	 TCLASS_LIST(){nClassesNum = 0; pTable = 0; };
	~TCLASS_LIST(){Release();};
	void Release()
	{
		DWORD n;
		if(pTable)
		{
			for(n=0;n<nClassesNum;n++) delete pTable[n];
			delete pTable;
			pTable = 0;
		}
		nClassesNum = 0;
	};
	void Add(ClassType * pClass)
	{
		DWORD n;
		n = nClassesNum;
		nClassesNum++;
		pTable = (ClassType**)RESIZE(pTable,nClassesNum*sizeof(ClassType*));
		pTable[n] = pClass;
	};
	void Del(DWORD _n)
	{
		DWORD n;
		if(_n >= nClassesNum) return;
		delete pTable[_n];
		for(n=_n;n<(nClassesNum-1);n++) pTable[n] = pTable[n+1];
		nClassesNum--;
	}
	ClassType * Read(DWORD _n)
	{
		if(_n >= nClassesNum) return 0;
		return pTable[_n];
	};
	DWORD GetClassesNum() {return nClassesNum;}
};

#endif