#ifndef _POSTEVENTS_H_
#define _POSTEVENTS_H_

#include "..\..\common_h\memop.h"
#include "s_eventmsg.h"

class POSTEVENTS_LIST
{
	S_EVENTMSG ** pTable;
	DWORD		 nClassesNum;
public:
	 POSTEVENTS_LIST(){nClassesNum = 0; pTable = 0; };
	~POSTEVENTS_LIST(){Release();};
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
	void Add(S_EVENTMSG * pClass)
	{
		DWORD n;
		n = nClassesNum;
		nClassesNum++;
		pTable = (S_EVENTMSG**)RESIZE(pTable,nClassesNum*sizeof(S_EVENTMSG*));
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
	S_EVENTMSG * Read(DWORD _n)
	{
		if(_n >= nClassesNum) return 0;
		return pTable[_n];
	};
	DWORD GetClassesNum() {return nClassesNum;}
	void InvalidateAll()
	{
		DWORD n;
		if(pTable)
		{
			for(n=0;n<nClassesNum;n++) pTable[n]->Invalidate();
		}
	};
	void RemoveInvalidated()
	{
		DWORD n;
		if(pTable)
		{
			for(n=0;n<nClassesNum;n++) 
			{
				if(pTable[n]->bInvalide)
				{
					Del(n);
					n = 0;
				}
			}	
		}
	}

};


#endif