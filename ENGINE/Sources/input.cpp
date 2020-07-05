#include "input.h"

long CORE_INPUT::GetKeyState(dword key_code)
{
	return GetAsyncKeyState(key_code);
}