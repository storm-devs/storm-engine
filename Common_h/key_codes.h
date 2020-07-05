#ifndef _KEY_CODES_H_
#define _KEY_CODES_H_

#define LOSHORT(l)   ((short) (l)) 
#define HISHORT(l)   ((short) (((DWORD) (l) >> 16) & 0xFFFF)) 

#define DPAD_UP			0
#define DPAD_RIGHT		1
#define DPAD_DOWN		2
#define DPAD_LEFT		4

#define LEFT_STICK		5
#define LEFT_THUMB		6
#define RIGHT_STICK		7
#define RIGHT_THUMB		8

#define LEFT_TRIGGER	9
#define RIGHT_TRIGGER	10

#define BTN_BACK		11
#define BTN_START		12
#define BTN_WHITE		13
#define BTN_BLACK		14
#define BTN_A			15
#define BTN_B			16
#define BTN_X			17
#define BTN_Y			18

#define LEFT_STICK_X	19
#define LEFT_STICK_Y	20
#define RIGHT_STICK_X	21
#define RIGHT_STICK_Y	22

#define KEY_CODES_NUM	23

#define KS_RELEASED		0
#define KS_PRESSED		1
#define KS_HOLD			2

#endif