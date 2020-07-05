#ifndef _SYSTEM_LOG_H_
#define _SYSTEM_LOG_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include <stdio.h>

void  _cdecl trace(char * data_PTR,...);

#endif