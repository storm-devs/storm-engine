#ifndef _S_EVENTMSG_H_
#define _S_EVENTMSG_H_

#include "message_script.h"


class S_EVENTMSG
{
public:
	DWORD nTime;
	DWORD nPeriod;
public:
	MESSAGE_SCRIPT * pMessageClass;
	char * pEventName;
	bool bProcess;
	bool bInvalide;
	S_EVENTMSG(){bInvalide = false; bProcess = false; pMessageClass = 0; nTime = 0; nPeriod = 0;pEventName = 0;};
	S_EVENTMSG(char * _name,MESSAGE_SCRIPT * _pc, DWORD _period) 
	{
		bInvalide = false;
		bProcess = false;
		pMessageClass = _pc; nTime = 0; nPeriod = _period;

		pEventName = NEW char[strlen(_name)+1];
		strcpy(pEventName,_name);
	};
	~S_EVENTMSG()
	{
		if(pMessageClass) delete pMessageClass;
		if(pEventName) delete pEventName;
	};
	bool ProcessTime(DWORD _DeltaTime)
	{
		nTime += _DeltaTime; 
		bProcess = true;
		if(nPeriod == 0) return true;
		if(nTime >= nPeriod) return true;
		return false;
	};
	void Invalidate(){bInvalide = true;};
};

#endif