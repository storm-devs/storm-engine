#ifndef _S_DEFTAB_H_
#define _S_DEFTAB_H_

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "data.h"

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define DEF_BUFFER_BLOCK_SIZE	1024
#define INVALID_DEF_CODE		0xffffffff
#define INVALID_SEGMENT_ID		0xffffffff

struct DEFINFO
{
	char * name;
	dword segment_id;
	dword hash;
	dword data4b;
	dword deftype;
};

#define DTHASHT_SIZE	256

struct DTHASHLINE
{
	DTHASHLINE(){nNumElements = 0; pElements = 0;};
	DWORD nNumElements;
	DWORD * pElements;
};

class S_DEFTAB
{
	dword Buffer_size;
	dword Def_num;
	DEFINFO * pTable;
	//bool bKeepName;
	DTHASHLINE HashLine[DTHASHT_SIZE];
public:
	 S_DEFTAB();
	~S_DEFTAB();
	dword GetDefNum(){return Def_num;};
	dword AddDef(DEFINFO& di);
	bool  GetDef(DEFINFO& di,dword def_code);
	bool  GetDefX(DEFINFO& vi,dword def_code);
	dword MakeHashValue(const char * string);
//	void  KeepNameMode(bool on){bKeepName = on;};
	void  Release();
	void  InvalidateBySegmentID(dword segment_id);
	dword FindDef(char * def_name);
	void UpdateHashTable(DWORD code, DWORD hash, bool in);
};

#endif