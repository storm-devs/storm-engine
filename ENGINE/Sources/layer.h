#ifndef _LAYER_H_
#define _LAYER_H_

#include "..\..\common_h\entity_id.h"
#include "common_h.h"
#include "idwalker.h"

#define _MAX_LAYER_NAME		290

typedef struct
{
	bool  System;
	bool  Ordered;
	bool  Deleted;
	dword Flags;
	dword System_flags;
} LAYER_STATE;

class LAYER_NODE
{
public:
	 LAYER_NODE(){};
	~LAYER_NODE(){};

	ENTITY_ID	id;
	LAYER_NODE * link_L;
	LAYER_NODE * link_R;
	dword		priority;
};

class LAYER
{
public:
	 LAYER(char * name, bool ordered,bool system,dword system_flags);
	~LAYER();
	bool Add(ENTITY_ID eid, dword priority);
	bool Add(ENTITY_ID eid);
	bool Del(ENTITY_ID eid);
	ENTITY_ID * GetID();
	ENTITY_ID * GetNextID();
	ENTITY_ID * GetID(dword * priority_PTR);
	ENTITY_ID * GetNextID(dword * priority_PTR);
	void SetFlag(dword flag);
	void ClrFlag(dword flag);
	dword GetPriority(ENTITY_ID eid);

	dword Count;
	LAYER_STATE ls;
	char  Name[_MAX_LAYER_NAME];
	LAYER_NODE * Base_Link;
	LAYER_NODE * Search_Link;

	VIDWALKER * GetWalker();
	void DelWalker(void *);
	IDWALKER * * walkers;
	dword walkers_num;

	ENTITY_ID * WalkerGetID(LPBYTE& sl);
	ENTITY_ID * WalkerGetNextID(LPBYTE& sl);

	
};

#endif