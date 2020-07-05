#include "pcinput.h"
#include "..\..\common_h\vapi.h"
extern VAPI * _CORE_API;
extern bool bActive;
float PCINPUT::GetKeyState(dword key_code, dword * value)
{
	if(!bActive) return 0;
//	POINT point;
	if(value)
	{
		if(key_code < KEY_CODES_NUM) *value = KeyState[key_code].state;
	}
	switch(key_code)
	{
		case DPAD_UP:
			if(GetAsyncKeyState(VK_UP) < 0) return 1;
		return 0;
		case DPAD_RIGHT:
			if(GetAsyncKeyState(VK_RIGHT) < 0) return 1;
		return 0;
		case DPAD_DOWN:
			if(GetAsyncKeyState(VK_DOWN) < 0) return 1;
		return 0;
		case DPAD_LEFT:
			if(GetAsyncKeyState(VK_LEFT) < 0) return 1;
		return 0;

		case LEFT_STICK:
		return 0;

		case LEFT_THUMB:
			
		return 0;

		case RIGHT_STICK:
			
		return 0;

		case RIGHT_THUMB:
			
		return 0;

		case LEFT_TRIGGER:
			if(GetAsyncKeyState('A') < 0) return 255;
		return 0;

		case RIGHT_TRIGGER:
			if(GetAsyncKeyState('D') < 0) return 255;
		return 0;
		

		case BTN_BACK:
			if(GetAsyncKeyState(VK_F6) < 0) return 255;
		return 0;

		case BTN_START:
			if(GetAsyncKeyState(VK_RETURN) < 0) return 255;
		return 0;

		case BTN_WHITE:
			if(GetAsyncKeyState(VK_SPACE) < 0) return 255;
		return 0;
		
		
		case BTN_BLACK:
			if(GetAsyncKeyState(VK_F1) < 0) return 255;
			//if(GetAsyncKeyState('2') < 0) return 255;
		return 0;

		case BTN_A:
			if(GetAsyncKeyState('3') < 0) return 255;
		return 0;
		
		case BTN_B:
			if(GetAsyncKeyState('4') < 0) return 255;
		return 0;
		
		case BTN_X:
			if(GetAsyncKeyState('5') < 0) return 255;
		return 0;

		case BTN_Y:
			if(GetAsyncKeyState(VK_F2) < 0) return 255;
		return 0;

		case LEFT_STICK_X:
			//if(GetAsyncKeyState('J') < 0) return (long)-1.0f;
			//if(GetAsyncKeyState('L') < 0) return (long)1.0f;
			//GetCursorPos(&point);
			if(value) *value = Mouse_Pos.x;
		return (float)Mouse_Delta.x;
		
		case LEFT_STICK_Y:
			//if(GetAsyncKeyState('I') < 0) return (long)1.0f;
			//if(GetAsyncKeyState('K') < 0) return (long)-1.0f;
			//GetCursorPos(&point);
			if(value) *value = Mouse_Pos.y;
		return (float)Mouse_Delta.y;

		case RIGHT_STICK_X:
			if(GetAsyncKeyState('F') < 0) return -1.0f;
			if(GetAsyncKeyState('H') < 0) return 1.0f;
		return 0;
		
		case RIGHT_STICK_Y:
			if(GetAsyncKeyState('T') < 0) return (long)1.0f;
			if(GetAsyncKeyState('G') < 0) return (long)-1.0f;
		return 0;
		
	}
	return 0;
}

void PCINPUT::ProcessKeyState()
{
	DWORD n;
	float state;
	POINT point;

	GetCursorPos(&point);
	Mouse_Delta.x = point.x - Mouse_Pos.x;
	Mouse_Delta.y = point.y - Mouse_Pos.y;
	Mouse_Pos = point;

	for(n=0;n<KEY_CODES_NUM;n++)
	{
		state = GetKeyState(n,0);
		switch(KeyState[n].state)
		{
			case KS_RELEASED:
				if(state == 0) break;
				else
				{
					KeyState[n].state = KS_PRESSED;
					_CORE_API->Event("key_event","lf",n,state);
				}
			break;
			case KS_PRESSED:
				if(state == 0) 
				{
					KeyState[n].state = KS_RELEASED;
					_CORE_API->Event("key_event","lf",n,state);
				}
				else
					KeyState[n].state = KS_HOLD;
			break;
			case KS_HOLD:
				if(state == 0) 
				{
					KeyState[n].state = KS_RELEASED;
					_CORE_API->Event("key_event","lf",n,state);
				}
				else break;
			break;
		}
	}
}

void PCINPUT::SetKeyState(dword key_code, dword * value)
{
	switch(key_code)
	{

		case LEFT_STICK_X:
			SetCursorPos(*value,Mouse_Pos.y);
			
		break;
		
		case LEFT_STICK_Y:
			SetCursorPos(Mouse_Pos.x,*value);
			
		break;
	}
}