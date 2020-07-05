#ifndef _DI_GAMEPAD_H_
#define _DI_GAMEPAD_H_

#include <dinput.h>
#include "device_controls.h"

class DI_GAMEPAD : public DEVICE_CONTROLS
{
	IDirectInput8 * pDInput;
	IDirectInputDevice8 * pDIGamepad;
	DIOBJECTDATAFORMAT * pDIODF;
	DWORD nObjectsNum;
	BYTE * pDataBuffer;
	DWORD nDataBufferSize;
	DWORD nDataSize;
	DWORD fakeN;
public:
	 DI_GAMEPAD(IDirectInput8 * _pdinput, GUID _guid, bool & bRes);
	~DI_GAMEPAD();
	char * GetDeviceName();
	long GetControlsNum();
	const char *  GetControlName(long control_n, char * buffer);
	void SetAppState(bool _state);
	void AddObject(LPCDIDEVICEOBJECTINSTANCE lpddoi);
	bool GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate);
	void Update();
};

#endif