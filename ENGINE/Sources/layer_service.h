#ifndef _LAYER_SERVICE_H_
#define _LAYER_SERVICE_H_

#include "layer.h"
#include "c_atom.h"
#include "..\..\common_h\exs.h"

typedef struct
{
	dword Layer_max_index;
	dword Scan_layer_code;
	dword Layers_number;
} LAYER_SERVICE_STATE;

#define INVALID_LAYER_CODE				0xffffffff

class LAYER_SERVICE
{
public:
	LAYER * Layer_Table[_MAX_LAYERS];
	bool	ToClean;
	LAYER_SERVICE_STATE lss;
	
	LAYER_SERVICE();
	bool Create(char * layer_name, bool ordered, bool fail_if_exist);
	bool Verify(char * layer_name);
	void Delete(char * layer_name);
	bool Add(char * layer_name, ENTITY_ID eid, dword priority);
	void Del(char * layer_name, ENTITY_ID eid);
	bool Add(dword index, ENTITY_ID eid, dword priority);
	void Del(dword index, ENTITY_ID eid);
	void Fit(dword pos,char * layer_name,LAYER_STATE ls);
	void Erase(dword dwcode);
	bool DeleteContent(char * layer_name);
	void SetSleep(char * layer_name,dword sleep_time_ms);
	void SetFlags(char * layer_name, dword flags);
	void ClrFlags(char * layer_name, dword flags);
	dword GetFlags(char * layer_name);
	dword GetIndex(char * layer_name);
	LAYER * GetLayer(char * layer_name);
	LAYER * GetLayer(dword index);
	void CheckAutoExceptions();
	void Clean();
	void Release();

};

#endif