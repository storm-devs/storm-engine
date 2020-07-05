#ifndef _PC_CONTROLS_H_
#define _PC_CONTROLS_H_

#include <dinput.h>
#include "..\common_h\vmodule_api.h"
#include "device_controls.h"

struct SYSTEM_CONTROL_ELEMENT
{
	SYSTEM_CONTROL_DESC ControlDesc;
	CONTROL_STATE_TYPE State;
	float Value;
};

#define GET_DEVICE_CODE(x) ((x & 0xffff0000)>>16)
#define GET_CONTROL_CODE(x) (x & 0xffff)

class PC_CONTROLS : public CONTROLS 
{
	IDirectInput8 * pDInput;
	VAPI * api;
	DEVICE_CONTROLS * * pDevices;
	long nDevicesNum;
	SYSTEM_CONTROL_ELEMENT * pSystemControlsTable;
	long nSystemControlsNum;
	USER_CONTROL * pUserControls;
	long nControlsNum;
	DWORD nFrameCounter;

public:
	 PC_CONTROLS();
	~PC_CONTROLS();

	IDirectInput8 * GetDevice(){return pDInput;};

	void Update(DWORD DeltaTime);
	long GetSystemControlsNum();
	bool GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC & _control_desc_struct);

	void Release();
	void AddSystemControl(SYSTEM_CONTROL_DESC & _control_desc_struct);

	long CreateControl(char * control_name);
	long GetControlsNum();
	bool GetControlDesc(long code, USER_CONTROL & _user_desc_struct);
	bool SetControlFlags(long code, DWORD _flags);


	void AddDevice(DEVICE_CONTROLS * _pDeviceClass);
	long GetDevicesNum();
	bool GetDeviceDesc(long code, DEVICE_DESC & _device_desc);

	void MapControl(long control_code, long system_control_code);
	bool GetControlState(long control_code, CONTROL_STATE & _state_struct);
	bool GetControlState(char * control_name, CONTROL_STATE & _state_struct);
	bool SetControlState(char * control_name, CONTROL_STATE & _state_struct);
	bool SetControlState(long control_code, CONTROL_STATE & _state_struct);
	void AppState(bool state);
	
};

//API_SERVICE_START("PC Controls")
//	DECLARE_MAIN_SERVICE(PC_CONTROLS)	
//API_SERVICE_END(PC_CONTROLS)

#endif
