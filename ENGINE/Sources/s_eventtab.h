#ifndef _S_EVENTTAB_H_
#define _S_EVENTTAB_H_

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "data.h"


#define BUFFER_BLOCK_SIZE		4
#define INVALID_EVENT_CODE		0xffffffff
#define INVALID_SEGMENT_ID		0xffffffff


#define FSTATUS_NEW		0
#define FSTATUS_NORMAL	1
#define FSTATUS_DELETED	2

struct EVENT_FUNC_INFO
{
	dword segment_id;
	dword func_code;
	dword status;
	bool  bStatic;
};

struct EVENTINFO
{
	dword hash;
	EVENT_FUNC_INFO * pFuncInfo;
	char * name;
	dword elements;
};

#define HASHTABLE_SIZE			64

class S_EVENTTAB
{
	dword Buffer_size[HASHTABLE_SIZE];
	dword Event_num[HASHTABLE_SIZE];
	EVENTINFO * pTable[HASHTABLE_SIZE];
	//bool bKeepName;
public:
	 S_EVENTTAB();
	~S_EVENTTAB();
	//dword GetEventNum(){return Event_num;};
	void  SetStatus(char * event_name, dword func_code, dword status);
	dword AddEventHandler(char * event_name, dword func_code, dword func_segment_id,long flag, bool bStatic = false);
	bool  DelEventHandler(char * event_name, dword func_code);
	bool  DelEventHandler(BYTE ti, dword event_code, dword func_code, bool bDelStatic = false);
	bool  GetEvent(EVENTINFO& ei, dword event_code);	// return true if var registred and loaded
	dword MakeHashValue(const char * string);
//	void  KeepNameMode(bool on){bKeepName = on;};
	void  Release();
	void  Clear();
	void  InvalidateBySegmentID(dword segment_id);
	dword FindEvent(char * event_name);
	void ProcessFrame();
};

#endif