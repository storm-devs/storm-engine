#ifndef _S_VARTAB_H_
#define _S_VARTAB_H_

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "data.h"
#include "..\..\common_h\memop.h"

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define VAR_BUFFER_BLOCK_SIZE	1024
#define INVALID_VAR_CODE		0xffffffff
#define INVALID_SEGMENT_ID		0xffffffff

struct VARINFO
{
	VARINFO()
	{
		pDClass = 0; name = 0; type = UNKNOWN; bArray = false; elements = 0;
		segment_id = INVALID_SEGMENT_ID; hash = 0;
	};
	dword segment_id;
	dword hash;
	DATA * pDClass;
	char * name;
	dword elements;
	S_TOKEN_TYPE type;
	bool bArray;
};

class S_DEBUG;

#define VTHASHT_SIZE	256

struct VTHASHLINE
{
	VTHASHLINE(){nNumElements = 0; pElements = 0;};
	DWORD nNumElements;
	DWORD * pElements;
};


class S_VARTAB
{
	friend S_DEBUG;

	dword Buffer_size;
	dword Var_num;
	VARINFO * pTable;
	//bool bKeepName;
	dword Global_var_num;
	VIRTUAL_COMPILER * pVCompiler;
	VTHASHLINE HashLine[VTHASHT_SIZE];
public:
	 S_VARTAB();
	~S_VARTAB();
	dword GetVarNum(){return Var_num;};
	dword AddVar(VARINFO& vi);
	bool  GetVar(VARINFO& vi,dword var_code);	// return true if var registred and loaded
	bool  GetVarX(VARINFO& vi,dword var_code);	// return true if var registred
	dword MakeHashValue(const char * string);
//	void  KeepNameMode(bool on){bKeepName = on;};
	void  Release();
	void  InvalidateBySegmentID(dword segment_id);
	dword FindVar(const char * var_name);
	void  SetElementsNum(dword var_code, dword elements_num){};
	void  SetType(dword var_code, dword type){};
	void  SetVCompiler(VIRTUAL_COMPILER * pvc) {pVCompiler = pvc;}
	bool  ArraySizeChanged(dword nIndex,dword nNewSize);
	void  UpdateHashTable(DWORD code, DWORD hash, bool in);
};

#endif