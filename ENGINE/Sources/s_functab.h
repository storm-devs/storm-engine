#ifndef _S_FUNCTAB_H_
#define _S_FUNCTAB_H_

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "s_vartab.h"
#include "..\..\common_h\s_import_func.h"

#define FUNC_BUFFER_BLOCK_SIZE	1024
#define INVALID_FUNC_CODE		0xffffffff
#define INVALID_FUNC_OFFSET		0xffffffff
#define INTERNAL_SEGMENT_ID		0xffffffff
#define IMPORTED_SEGMENT_ID		0xfffffffe

// when offset value of function is INVALID_FUNC_OFFSET, function segment 
// isnt currently loaded and function call is impossible
// when segment_id is INTERNAL_SEGMENT_ID, this function is internal and offset
// means internal function number

struct LVARINFO
{
	S_TOKEN_TYPE type;
	dword hash;
	char * name;
	dword elements;
	bool bArray;
};

struct FUNCINFO
{
	dword segment_id;
	dword offset;
	dword hash;
	char * name;
	dword arguments;
	dword var_num;
	dword stack_offset;
	LVARINFO * pLocal;
	S_TOKEN_TYPE return_type;
	char * decl_file_name;
	dword  decl_line;
	dword  code;
	double fTimeUsage;
	dword  nNumberOfCalls;
	SIMPORTFUNC pImportedFunc;
	dword ext_args;
};

#define HASHT_SIZE	256

struct HASHLINE
{
	HASHLINE(){nNumElements = 0; pElements = 0;};
	DWORD nNumElements;
	DWORD * pElements;
};

class S_FUNCTAB
{
	dword Buffer_size;
	dword Func_num;
	FUNCINFO * pTable;
	//bool bKeepName;

	HASHLINE HashLine[HASHT_SIZE];

public:
	 S_FUNCTAB();
	~S_FUNCTAB();
	dword GetFuncNum(){return Func_num;};
	dword AddFunc(FUNCINFO& fi);
	bool  GetFunc(FUNCINFO& fi,dword func_code);	// return true if func registred and loaded
	bool  GetFuncX(FUNCINFO& fi,dword func_code);	// return true if func registred
	dword MakeHashValue(const char * string);
	//void  KeepNameMode(bool on){bKeepName = on;};
	void  Release();
	void  InvalidateBySegmentID(dword segment_id);
	dword FindFunc(char * func_name);
	bool  SetFuncOffset(char * func_name, dword offset);
	bool  AddFuncVar(dword func_code, LVARINFO & lvi);
	bool  AddFuncArg(dword func_code, LVARINFO & lvi, bool bExt = false);
	dword FindVar(dword func_code, char * var_name);
	void  AddTime(dword func_code, DWORD time);
	void  SetTimeUsage(dword func_code,double f);
	void  AddCall(dword func_code);
	void InvalidateFunction(dword nFuncHandle);
	void UpdateHashTable(DWORD code, DWORD hash, bool in);
	bool GetVar(LVARINFO & lvi, dword func_code, dword var_code);

};

#endif