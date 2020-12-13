#pragma once

#include "Core.h"
#include "file_service.h"
#include "vapi.h"

extern CORE Core;
extern VAPI *api;

// extern uint32_t Exceptions_Mask;
extern FILE_SERVICE File_Service;

extern char ENGINE_INI_FILE_NAME[256];
// extern bool bDebugWindow, bAcceleration;
// extern bool bNetActive;
// extern bool Error_Flag;
extern bool bActive;
extern bool bSteam;