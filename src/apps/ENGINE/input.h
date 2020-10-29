#ifndef _INPUT_H_
#define _INPUT_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "d_types.h"

class CORE_INPUT
{
  public:
    CORE_INPUT(){};
    ~CORE_INPUT(){};
    long GetKeyState(dword key_code);
};

#endif