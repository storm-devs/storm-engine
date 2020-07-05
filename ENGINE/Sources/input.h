#ifndef _INPUT_H_
#define _INPUT_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "..\..\common_h\d_types.h"

class INPUT 
{
public:
	 INPUT(){};
	~INPUT(){};
	long GetKeyState(dword key_code);
};

#endif