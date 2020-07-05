#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "system_log.h"
#include "gdi_display.h"
#include "file_service.h"
#include "memory_service.h"
#include "control_stack.h"
#include "system_api.h"
#include "..\..\common_h\vapi.h"
#include "..\..\common_h\memop.h"

extern dword Exceptions_Mask;
extern GDI_DISPLAY gdi_display;
extern FILE_SERVICE File_Service;
extern MEMORY_SERVICE Memory_Service;
extern char Last_Error[512];
extern bool Error_Flag;
extern char * CTP_Name[];
extern CONTROL_STACK Control_Stack;
extern SYSTEM_API System_Api;
extern VSYSTEM_API * _VSYSTEM_API;
extern VAPI * _CORE_API;
extern CONTROL_BLOCK Control_Block;

#define SET_ERROR(x) {Error_Flag = true; if(x) strcpy(Last_Error,x); }
#define GET_ERROR Last_Error
#define CLR_ERROR {Error_Flag = false; Last_Error[0] = 0;}
#define TST_ERROR Error_Flag
//#define SET_CODE_STATE(a,b) {Code_State.ctp = (word)a; Code_State.class_code = b; Memory_Service.SetState((word)a,b,0); }
//#define CODE_CORE {Code_State.ctp = (word)CTP_CORE; Code_State.class_code = 0xffffffff; Memory_Service.SetState((word)CTP_CORE,0xffffffff,0);}
#define CTP_NAME(x) CTP_Name[x]

#define PUSH_CONTROL(ptr,code,c) {/*Control_Stack.Push(ptr,code,c);*/ Control_Block.pointer = ptr; Control_Block.class_code = code; Control_Block.ctp = c; Memory_Service.SetState((word)c,code); Control_Stack.Push(ptr,code,c);}
#define POP_CONTROL(control_block_ptr) {Control_Stack.Pop(control_block_ptr); Control_Stack.Read(&Control_Block); Memory_Service.SetState((word)Control_Block.ctp,Control_Block.class_code);}


#endif