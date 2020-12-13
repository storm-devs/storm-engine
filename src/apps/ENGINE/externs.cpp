#include "externs.h"

char ENGINE_INI_FILE_NAME[256] = "engine.ini";

FILE_SERVICE File_Service;

CORE Core;
VAPI *api = nullptr;

VFILE_SERVICE *fio = nullptr;

// bool bDebugWindow = false, bAcceleration = false;
bool bActive = true;
bool bSteam = true;

// bool Error_Flag = false;
// bool bNetActive;

// uint32_t Exceptions_Mask;
