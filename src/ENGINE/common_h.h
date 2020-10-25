#ifndef _COMMON_H_H_
#define _COMMON_H_H_

//#include <windows.h>
#include "..\..\common_h\d_types.h"
#include "..\..\common_h\memop.h"

//------------------------------------------------------------------------------------------------
// Predefined codes
//------------------------------------------------------------------------------------------------
#define ERROR_MESSAGE_DELAY 4000

//------------------------------------------------------------------------------------------------
// Control transfer points codes. This code help to identify, where is programm control at current
// time
//------------------------------------------------------------------------------------------------
#define CTP_CORE 0                // execition in core zero ring
#define CTP_MODULE_INTERFACE 1    // execition in module interface function
#define CTP_MODULE_INTAPI 2       // execition in module api class
#define CTP_MODULE_CREATE_CLASS 3 // execition in module api class
#define CTP_API 4                 // control transferred via api to service
#define CTP_CONSTRUCTOR 5         // control transferred to object constructor
#define CTP_DESTRUCTOR 6          // control transferred to object destructor
#define CTP_INIT 7                // control transferred to object Init(...) function
#define CTP_EXECUTE 8             // control transferred to object Execute(...) function
#define CTP_REALIZE 9             // control transferred to object Realize(...) function
#define CTP_MESSAGE_SYSTEM 10     // control transferred to object ProcessMessage(DWORD,DWORD,DWORD) function
#define CTP_MESSAGE 11            // control transferred to object ProcessMessage(void*) function
#define CTP_CREATESTATE 12        // control transferred to object SaveState(void*) function
#define CTP_LOADSTATE 13          // control transferred to object LoadState(void*) function
#define CTP_RUNSTART 14           // control transferred to service RunStart() function
#define CTP_RUNEND 15             // control transferred to service RunEnd() function

#endif
