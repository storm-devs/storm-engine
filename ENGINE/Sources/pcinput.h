#ifndef _PCINPUT_H_
#define _PCINPUT_H_

#include "..\..\common_h\input.h"
#include "..\..\common_h\key_codes.h"

struct KEY_STATE
{
	dword state;
	dword value;
};

class PCINPUT : public INPUT
{
	KEY_STATE KeyState[KEY_CODES_NUM];
	POINT Mouse_Pos;
	POINT Mouse_Delta;
public:
	 PCINPUT(){};
	~PCINPUT(){};
	float GetKeyState(dword key_code, dword * value);
	void ProcessKeyState();
	void SetKeyState(dword key_code, dword * value);
};

#endif
