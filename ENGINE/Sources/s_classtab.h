#ifndef _S_CLASSTAB_H_
#define _S_CLASSTAB_H_

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "data.h"

#define CLASS_BUFFER_BLOCK_SIZE	1024
#define INVALID_CLASS_CODE		0xffffffff
#define INVALID_SEGMENT_ID		0xffffffff

#define CCF_SYSTEMTYPEVARIABLE	0x1
#define CCF_VARIABLE			0x2
#define CCF_FUNCTION			0x4
#define CCF_VARARRAY			0x8


struct CLASS_COMPONENT
{
	CLASS_COMPONENT() {name = 0; nFlags = 0; nID = 0xffffffff; nElements = 0;};
	dword nID;
	dword nFlags;
	char * name;
	dword nElements;
};

struct CLASSINFO
{
	char * name;
	dword hash;
	dword segment_id;
	dword nComponentsNum;
	CLASS_COMPONENT * pComponent;
	bool  bClassRegistredOnly;
};

class S_DEBUG;

class S_CLASSTAB
{
	friend S_DEBUG;
	dword Buffer_size;
	dword nClassesNum;
	CLASSINFO * pTable;
	VIRTUAL_COMPILER * pVCompiler;
public:
	 S_CLASSTAB();
	~S_CLASSTAB();
	dword GetClassesNum(){return nClassesNum;};
	dword AddClass(CLASSINFO& ci, bool bRegisterOnly = false);

	bool  GetClass(CLASSINFO& ci,dword class_code);	// return true if class registred and loaded
	bool  GetClassX(CLASSINFO& ci,dword class_code);// return true if class registred
	dword MakeHashValue(const char * string);
	void  Release();
	void  InvalidateBySegmentID(dword segment_id);
	dword FindClass(const char * class_name);
	void  SetVCompiler(VIRTUAL_COMPILER * pvc) {pVCompiler = pvc;}
	void  InitSystemClasses();
	bool  IsClassDataSet(dword class_id);
};

#endif