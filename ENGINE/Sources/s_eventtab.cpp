#include "s_eventtab.h"
#include <string.h>
#include "system_log.h"
#define HASHT_INDEX(x)	(BYTE)(x>>24)
#define HASHT_CODE(x)	(x&0xffffff)
#define HASH2INDEX(x)	(BYTE)(x&0x2f)

S_EVENTTAB::S_EVENTTAB()
{
	DWORD n;
	for(n=0;n<HASHTABLE_SIZE;n++)
	{
		pTable[n] = 0;
		Buffer_size[n] = 0;
		Event_num[n] = 0;
	}

}

S_EVENTTAB::~S_EVENTTAB()
{
	Release();
}

void  S_EVENTTAB::Clear()
{
	dword n,m,i;
	for(i=0;i<HASHTABLE_SIZE;i++)
	{
		if(pTable[i]) 
		{
			for(n=0;n<Event_num[i];n++)	
			{ 
				for(m=0;m<pTable[i][n].elements;m++) 
				{
					if(!pTable[i][n].pFuncInfo[m].bStatic) pTable[i][n].pFuncInfo[m].status = FSTATUS_DELETED;
				}

				//if(pTable[n].pFuncInfo) delete pTable[n].pFuncInfo;
				//if(pTable[n].name) delete pTable[n].name;	
			}
			//delete pTable; pTable = 0;
		}
		//Buffer_size = 0;
		//Event_num = 0;
	}
	ProcessFrame();
}

void  S_EVENTTAB::Release()
{
	dword n,i;
	for(i=0;i<HASHTABLE_SIZE;i++)
	{
		if(pTable[i]) 
		{
			for(n=0;n<Event_num[i];n++)	
			{ 
				if(pTable[i][n].pFuncInfo) delete pTable[i][n].pFuncInfo;
				if(pTable[i][n].name) delete pTable[i][n].name;	
			}
			delete pTable[i]; pTable[i] = 0;
		}
		Buffer_size[i] = 0;
		Event_num[i] = 0;
	}
}

bool S_EVENTTAB::GetEvent(EVENTINFO& ei, dword event_code)
{
	DWORD tc;
	BYTE  ti;
	ti = HASHT_INDEX(event_code);
	tc = HASHT_CODE(event_code);
	if(tc >= Event_num[ti]) return false;
	ei = pTable[ti][tc];
	return true;
}

dword S_EVENTTAB::AddEventHandler(char * event_name, dword func_code, dword func_segment_id,long flag, bool bStatic)
{
	dword n,i;
	dword hash;
	BYTE ti;

	hash = MakeHashValue(event_name);

	ti = HASH2INDEX(hash);

	for(n=0;n<Event_num[ti];n++)
	{
		if(pTable[ti][n].hash == hash)
		{
			if(stricmp(event_name,pTable[ti][n].name) != 0) continue;
			// event already in list
			for(i=0;i<pTable[ti][n].elements;i++)
			{
				// event handler function already set
				if(pTable[ti][n].pFuncInfo[i].func_code == func_code) 
				{
					/*if(pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED)
					{
						trace("pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED : %s",pTable[ti][n].name);
					}*/
					//return n;
					pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;
					
					return (((ti<<24)&0xff000000)|(n&0xffffff));
				}
			}
			// add function
			i = pTable[ti][n].elements;
			pTable[ti][n].elements++;
			pTable[ti][n].pFuncInfo = (EVENT_FUNC_INFO*)RESIZE(pTable[ti][n].pFuncInfo,pTable[ti][n].elements*sizeof(EVENT_FUNC_INFO));
			
			pTable[ti][n].pFuncInfo[i].func_code = func_code;
			pTable[ti][n].pFuncInfo[i].segment_id = func_segment_id;
			if(flag) pTable[ti][n].pFuncInfo[i].status = FSTATUS_NEW;
			else pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;
			pTable[ti][n].pFuncInfo[i].bStatic = bStatic;
			//return n;
			return (((ti<<24)&0xff000000)|(n&0xffffff));
		}
	}

	// add new event
	if(Event_num[ti] >= Buffer_size[ti])
	{
		Buffer_size[ti] += BUFFER_BLOCK_SIZE;
		pTable[ti] = (EVENTINFO *)RESIZE(pTable[ti],Buffer_size[ti]*sizeof(EVENTINFO));
	}
	
	pTable[ti][Event_num[ti]].elements = 1;
	pTable[ti][Event_num[ti]].hash = hash;
	pTable[ti][Event_num[ti]].name = 0;

	pTable[ti][Event_num[ti]].pFuncInfo = (EVENT_FUNC_INFO*)NEW char[sizeof(EVENT_FUNC_INFO)];
	pTable[ti][Event_num[ti]].pFuncInfo[0].func_code = func_code;
	pTable[ti][Event_num[ti]].pFuncInfo[0].segment_id = func_segment_id;
	if(flag) pTable[ti][Event_num[ti]].pFuncInfo[0].status = FSTATUS_NEW;
	else pTable[ti][Event_num[ti]].pFuncInfo[0].status = FSTATUS_NORMAL;
	pTable[ti][Event_num[ti]].pFuncInfo[0].bStatic = bStatic;
	
	if(true)//bKeepName)
	{
		if(event_name)
		{

			pTable[ti][Event_num[ti]].name = NEW char[strlen(event_name) + 1];
			strcpy(pTable[ti][Event_num[ti]].name,event_name);
		}
	}
	Event_num[ti]++;
	//return (Event_num[ti] - 1);
	return (((ti<<24)&0xff000000)|((Event_num[ti] - 1)&0xffffff));
}

dword S_EVENTTAB::MakeHashValue(const char * string)
{
	dword hval = 0;
	dword g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		hval = (hval<<4) + (unsigned long int)v;
		g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
	}
	return hval;
}

bool S_EVENTTAB::DelEventHandler(char * event_name, dword func_code)
{
	dword hash;
	dword n;
	BYTE ti;
	if(event_name == 0) return false;
	hash = MakeHashValue(event_name);
	
	ti = HASH2INDEX(hash);

	for(n=0;n<Event_num[ti];n++)
	{
		if(pTable[ti][n].hash == hash) 
		if(stricmp(pTable[ti][n].name,event_name)==0)
		{
			return DelEventHandler(ti,n,func_code);
			//return;
		}
	}
	return false;
}

void S_EVENTTAB::SetStatus(char * event_name, dword func_code, dword status)
{
	dword hash;
	dword n,i;
	BYTE ti;

	if(event_name == 0) return;

	hash = MakeHashValue(event_name);
	ti = HASH2INDEX(hash);

	for(n=0;n<Event_num[ti];n++)
	{
		if(pTable[ti][n].hash == hash) 
		if(stricmp(pTable[ti][n].name,event_name)==0)
		{
			for(i = 0;i<pTable[ti][n].elements;i++)
			{
				if(pTable[ti][n].pFuncInfo[i].func_code == func_code)
				{
					pTable[ti][n].pFuncInfo[i].status = status;
					return;
				}
			}

		}
	}
}

bool S_EVENTTAB::DelEventHandler(BYTE ti, dword event_code, dword func_code, bool bDelStatic)
{
	dword n;

	if(!bDelStatic)
	{
		if(pTable[ti][event_code].pFuncInfo[func_code].bStatic) 
		{
			return false;
		}
	}

	for(n = func_code;n<(pTable[ti][event_code].elements-1);n++)
	{
		pTable[ti][event_code].pFuncInfo[n] = pTable[ti][event_code].pFuncInfo[n+1];
	}
	pTable[ti][event_code].elements--;
	pTable[ti][event_code].pFuncInfo = (EVENT_FUNC_INFO*)RESIZE(pTable[ti][event_code].pFuncInfo,pTable[ti][event_code].elements*sizeof(EVENT_FUNC_INFO));
	return true;
}

void S_EVENTTAB::InvalidateBySegmentID(dword segment_id)
{
	dword n,i,ti;
	for(ti=0;ti<HASHTABLE_SIZE;ti++)
	{
		for(n=0;n<Event_num[ti];n++)
		{
			for(i=0;i<pTable[ti][n].elements;i++)
			{
				if(pTable[ti][n].pFuncInfo[i].segment_id == segment_id)
				{
					if(DelEventHandler((BYTE)ti,n,i,true)) i = 0;
				}
			}
		}
	}
}

dword S_EVENTTAB::FindEvent(char * event_name)
{
	dword n;
	dword hash;
	BYTE ti;
	if(event_name == 0) return INVALID_EVENT_CODE;
	hash = MakeHashValue(event_name);
	ti = HASH2INDEX(hash);
	for(n=0;n<Event_num[ti];n++)
	{
		if(pTable[ti][n].hash == hash) 
		if(stricmp(pTable[ti][n].name,event_name) == 0)	return (((ti<<24)&0xff000000)|(n&0xffffff));
	}
	return INVALID_EVENT_CODE;
}

void S_EVENTTAB::ProcessFrame()
{
	dword n,i,ti;
	for(ti=0;ti<HASHTABLE_SIZE;ti++)
	for(n=0;n<Event_num[ti];n++)
	{
		// delete old handlers
		for(i=0;i<pTable[ti][n].elements;i++)
		{
			if(pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED)
			{
				DelEventHandler((BYTE)ti,n,i);
				i = 0;
			} else pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;
		}
	}
}