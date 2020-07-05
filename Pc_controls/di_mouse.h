#ifndef _DI_MOUSE_H_
#define _DI_MOUSE_H_

#include <dinput.h>
#include "device_controls.h"

#define MOUSE_OBJECTS_MAXNUM	16

struct MOUSE_OBJECT
{
	DWORD offset;
	const char * name;
};

class DI_MOUSE : public DEVICE_CONTROLS
{
	IDirectInput8 * pDInput;
	IDirectInputDevice8 * pDIMouse;
	MOUSE_OBJECT Object[MOUSE_OBJECTS_MAXNUM];
	long nObjects;
	DIMOUSESTATE dims;
public:
	 DI_MOUSE(IDirectInput8 * _pdinput, GUID _guid, bool & bRes);
	~DI_MOUSE();
	char * GetDeviceName();
	long GetControlsNum();
	const char * GetControlName(long control_n, char * buffer);
	void SetAppState(bool _state);
	bool GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate);
	void AddObject(DWORD _offset, const char * name);
	void Update();
};

#endif