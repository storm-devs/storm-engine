#ifndef _INPUT_H_
#define _INPUT_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "..\..\common_h\d_types.h"

class CORE_INPUT 
{
public:
	 CORE_INPUT(){};
	virtual ~CORE_INPUT(){};
	virtual float GetKeyState(dword key_code, dword * value = 0)= 0;
	virtual void ProcessKeyState(){};
	virtual void SetKeyState(dword key_code, dword * value){};
};

#endif