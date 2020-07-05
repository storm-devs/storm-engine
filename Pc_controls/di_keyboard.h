#ifndef _DI_KEYBOARD_H_
#define _DI_KEYBOARD_H_

#include <dinput.h>
#include "device_controls.h"

class DI_KEYBOARD : public DEVICE_CONTROLS
{
	IDirectInput8 * pDInput;
	IDirectInputDevice8 * pDIKeyboard;
	char KeyBuffer[256];
public:
	 DI_KEYBOARD(IDirectInput8 * _pdinput, GUID _guid, bool & bRes);
	~DI_KEYBOARD();
	char * GetDeviceName();
	long GetControlsNum();
	const char * GetControlName(long control_n, char * buffer);
	void SetAppState(bool _state);
	void Update();
	bool GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate);
};

#endif