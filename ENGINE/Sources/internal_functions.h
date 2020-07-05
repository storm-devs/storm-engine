#ifndef _INTERNAL_FUNCTIONS_H_
#define _INTERNAL_FUNCTIONS_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif


#include "token.h"
#include "data.h"
#include "stack.h"

class INTERNAL_FUNCTIONS
{
	VSTACK * pStack;
	TOKEN *  pToken;
public:
	 INTERNAL_FUNCTIONS();
	~INTERNAL_FUNCTIONS();

	void SetStackAndToken(VSTACK * pStack, TOKEN *  pToken);
	DATA * CallIntFunction(char * func_name);
	DATA * CallIntFunction(DWORD func_code);
	DWORD  GetIntFunctionCode(char * func_name);
};

#endif