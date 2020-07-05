#include "input.h"

long INPUT::GetKeyState(dword key_code)
{
	return GetAsyncKeyState(key_code);
}