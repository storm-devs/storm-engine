#ifndef _S_IMPORT_FUNC_H_
#define _S_IMPORT_FUNC_H_

#include "v_s_stack.h"

#define IFUNCRESULT_OK		0
#define IFUNCRESULT_FAILED	0xffffffff

#define INVALID_FUNCHANDLE	0xffffffff

typedef DWORD (__cdecl * SIMPORTFUNC)(VS_STACK *);

struct IFUNCINFO
{
	IFUNCINFO()
	{
		pFuncName = 0;
		pReturnValueName = 0;
		pFuncAddress = 0;
		pDeclFileName = 0;
		nDeclLine = 0;
	};
	char * pFuncName;
	char * pReturnValueName;
	DWORD  nArguments;
	SIMPORTFUNC pFuncAddress;
	char * pDeclFileName;
	DWORD  nDeclLine;
};

#endif