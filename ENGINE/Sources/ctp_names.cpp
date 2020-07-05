#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif


char * CTP_Name[] = {
	"CTP_CORE",
	"CTP_MODULE_INTERFACE",
	"CTP_MODULE_INTAPI",
	"CTP_MODULE_CREATE_CLASS",
	"CTP_API",
	"object constructor",
	"object destructor",
	"Init() function",
	"Execute() function",
	"Realize() function",
	"ProcessMessage(DWORD,DWORD,DWORD) function",
	"ProcessMessage(MESSAGE) function",
	"CreateState(void*) function",
	"LoadState(void*) function",
	"RunStart()",
	"RunEnd()",
};
