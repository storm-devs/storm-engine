#include "pc_controls.h"
#include "di_mouse.h"
#include "di_gamepad.h"
#include "di_keyboard.h"

INTERFACE_FUNCTION
CREATE_SERVICE(PC_CONTROLS)	

static PC_CONTROLS * pClassInstance = 0;
BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

PC_CONTROLS::PC_CONTROLS()
{
	if(pClassInstance == 0) pClassInstance = this;

	nFrameCounter = 0;
	api = _CORE_API;
	pSystemControlsTable = 0;
	nSystemControlsNum = 0;

	pUserControls = 0;
	nControlsNum = 0;

	pDevices = 0;
	nDevicesNum = 0;
	
	HRESULT hRes;
	DWORD nTR;
	
	api->Trace("Installed devices:");
	hRes = DirectInput8Create(api->GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&pDInput , NULL); 
	if(!FAILED(hRes)) 
	{
		pDInput->EnumDevices(0,DIEnumDevicesCallback, &nTR, DIEDFL_ATTACHEDONLY);
	}
}

PC_CONTROLS::~PC_CONTROLS()
{
	if(pDInput) pDInput->Release();
	Release();
}

BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	DWORD device_type;
	DEVICE_CONTROLS * pD;
	bool bRes;

	if(pClassInstance == 0) return false;
	_CORE_API->Trace("%s",lpddi->tszInstanceName);
	device_type = lpddi->dwDevType & 0xff;
	switch(device_type)
	{
		case DI8DEVTYPE_GAMEPAD:
			_CORE_API->Trace("  Gamepad");
			pD = NEW DI_GAMEPAD(pClassInstance->GetDevice(),lpddi->guidInstance,bRes);
			if(bRes) 
			{
				pClassInstance->AddDevice(pD);
				pD->SetAppState(true);
			}
			else delete pD;
		break;
		case DI8DEVTYPE_JOYSTICK:
			_CORE_API->Trace("  Joystick");
		break;
		case DI8DEVTYPE_KEYBOARD:
			_CORE_API->Trace("  Keyboard or keyboard like device");
			pD = NEW DI_KEYBOARD(pClassInstance->GetDevice(),lpddi->guidInstance,bRes);
			if(bRes) 
			{
				pClassInstance->AddDevice(pD);
				pD->SetAppState(true);
			}
			else delete pD;
		break;
		case DI8DEVTYPE_MOUSE:
			_CORE_API->Trace("  Mouse or mouse like device");
			pD = NEW DI_MOUSE(pClassInstance->GetDevice(),lpddi->guidInstance,bRes);
			if(bRes) 
			{
				pClassInstance->AddDevice(pD);
				pD->SetAppState(true);
			}
			else delete pD;
		break;
		default:
			_CORE_API->Trace("  Unknown device type");
		break;
	}
	return true;
}

void PC_CONTROLS::AppState(bool state)
{
	DWORD n;
	if(pDevices)
	{
		for(n=0;n<nDevicesNum;n++) pDevices[n]->SetAppState(state);
	}
}


void PC_CONTROLS::Release()
{
	long n;
	if(pSystemControlsTable)
	{
		for(n=0;n<nSystemControlsNum;n++)
		{
			if(pSystemControlsTable[n].ControlDesc.pControlName)
				delete pSystemControlsTable[n].ControlDesc.pControlName;
		}
		delete pSystemControlsTable;
	}
	if(pUserControls)
	{
		for(n=0;n<nControlsNum;n++)
		{
			if(pUserControls[n].name) delete pUserControls[n].name;
		}
		delete pUserControls;
	}
	
	pUserControls = 0;
	nControlsNum = 0;
	 
	pSystemControlsTable = 0;
	nSystemControlsNum = 0;

	if(pDevices)
	{
		for(n=0;n<nDevicesNum;n++) delete pDevices[n];
		delete pDevices;
	}
	pDevices = 0;
	nDevicesNum = 0;
}

long PC_CONTROLS::GetSystemControlsNum()
{
	return nSystemControlsNum;
}

bool PC_CONTROLS::GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC & _control_desc_struct)
{
	if(code >= nSystemControlsNum) return false;
	_control_desc_struct = pSystemControlsTable[code].ControlDesc;
	return true;
}

void PC_CONTROLS::AddSystemControl(SYSTEM_CONTROL_DESC & _control_desc_struct)
{
	long n;
	n = nSystemControlsNum;
	nSystemControlsNum++;
	pSystemControlsTable = (SYSTEM_CONTROL_ELEMENT *)RESIZE(pSystemControlsTable,nSystemControlsNum*sizeof(SYSTEM_CONTROL_ELEMENT));
	pSystemControlsTable[n].ControlDesc.ControlType = _control_desc_struct.ControlType;
	pSystemControlsTable[n].ControlDesc.pControlName = 0;
	if(_control_desc_struct.pControlName)
	{
		pSystemControlsTable[n].ControlDesc.pControlName = NEW char[strlen(_control_desc_struct.pControlName) + 1];
		strcpy(pSystemControlsTable[n].ControlDesc.pControlName,_control_desc_struct.pControlName);
	}
}

long PC_CONTROLS::CreateControl(char * control_name)
{
	long n;
	if(control_name == 0) return INVALID_CONTROL_CODE;
	for(n=0;n<nControlsNum;n++)
	{
		if(stricmp(control_name,pUserControls[n].name)==0) return n;
	}
	n = nControlsNum;
	nControlsNum++;
	pUserControls = (USER_CONTROL *)RESIZE(pUserControls,nControlsNum * sizeof(USER_CONTROL));
	pUserControls[n].name = NEW char[strlen(control_name) + 1];
	strcpy(pUserControls[n].name,control_name);
	pUserControls[n].system_code = UNASSIGNED_CONTROL;
	pUserControls[n].flags = 0;
	pUserControls[n].state = CST_INACTIVE;
	pUserControls[n].nframe = 0;
	
	return n;
}

long PC_CONTROLS::GetControlsNum()
{
	return nControlsNum;
}

bool PC_CONTROLS::GetControlDesc(long code, USER_CONTROL & _user_desc_struct)
{
	if(code < 0 || code >= nControlsNum) return false;
	
	_user_desc_struct.name = pUserControls[code].name;
	_user_desc_struct.system_code = pUserControls[code].system_code;

	return true;
}

void PC_CONTROLS::AddDevice(DEVICE_CONTROLS * _pDeviceClass)
{
	long n;
	if(_pDeviceClass == 0) return;	// trace->...
	n = nDevicesNum;
	nDevicesNum++;
	pDevices = (DEVICE_CONTROLS **)RESIZE(pDevices,nDevicesNum * sizeof(DEVICE_CONTROLS *));
	pDevices[n] = _pDeviceClass;
}

long PC_CONTROLS::GetDevicesNum()
{
	return nDevicesNum;
}

bool PC_CONTROLS::GetDeviceDesc(long code, DEVICE_DESC & _device_desc)
{
	if(code < 0 || code >= nDevicesNum) return false;
	_device_desc.name = pDevices[code]->GetDeviceName();
	return true;
}


void PC_CONTROLS::MapControl(long control_code, long system_control_code)
{
	if(control_code < 0 || control_code >= nControlsNum) return;
	pUserControls[control_code].system_code = system_control_code;
}

bool PC_CONTROLS::GetControlState(char * control_name, CONTROL_STATE & _state_struct)
{
	long n;
	_state_struct.state = CST_INACTIVE;
	_state_struct.lValue = 0;
	_state_struct.lValue = 0.0f;

	if(control_name == 0) return false;
	for(n=0;n<nControlsNum;n++)
	{
		if(stricmp(control_name,pUserControls[n].name)==0) return GetControlState(n,_state_struct);
	}
	return false;
}

bool PC_CONTROLS::GetControlState(long control_code, CONTROL_STATE & _state_struct)
{
	DWORD system_code;
	DWORD device_code;
	DWORD object_code;

	_state_struct.state = CST_INACTIVE;
	_state_struct.lValue = 0;
	_state_struct.lValue = 0.0f;

	if(control_code < 0 || control_code >= nControlsNum) return false;
	system_code = pUserControls[control_code].system_code;
	device_code = (pUserControls[control_code].system_code & 0xffff0000)>>16;
	object_code = pUserControls[control_code].system_code & 0xffff;
	if(device_code >= nDevicesNum) return false;
	
	pDevices[device_code]->GetControlState(object_code,_state_struct,pUserControls[control_code].state);

	if(pUserControls[control_code].flags & USE_AXIS_AS_INVERSEBUTTON)
	{
		if(_state_struct.lValue < 0)
		{
			if(pUserControls[control_code].state == CST_INACTIVE || 
				pUserControls[control_code].state == CST_INACTIVATED)
			_state_struct.state = CST_ACTIVATED;
				else 
			_state_struct.state = CST_ACTIVE;

			_state_struct.lValue = 1;
			_state_struct.fValue = 1.0f;
		}
		else
		{
			if(pUserControls[control_code].state == CST_ACTIVE || 
				pUserControls[control_code].state == CST_INACTIVE)
			_state_struct.state = CST_INACTIVATED;
				else 
			_state_struct.state = CST_INACTIVE;
			
			_state_struct.lValue = 0;
			_state_struct.fValue = 0.0f;
		}
	}

	if(pUserControls[control_code].flags & INVERSE_CONTROL)
	{
		_state_struct.lValue = -_state_struct.lValue;
		_state_struct.fValue = -_state_struct.fValue;
		switch(_state_struct.state)
		{
			case CST_ACTIVE: _state_struct.state = CST_INACTIVE; break;
			case CST_INACTIVE: _state_struct.state = CST_ACTIVE; break;
			case CST_ACTIVATED: _state_struct.state = CST_INACTIVATED; break;
			case CST_INACTIVATED: _state_struct.state = CST_ACTIVATED; break;
		}
	}


	if(pUserControls[control_code].flags & USE_AXIS_AS_BUTTON)
	{
		if(_state_struct.lValue > 0)
		{
			if(pUserControls[control_code].state == CST_INACTIVE || 
				pUserControls[control_code].state == CST_INACTIVATED)
			_state_struct.state = CST_ACTIVATED;
				else 
			_state_struct.state = CST_ACTIVE;
			
			_state_struct.lValue = 1;
			_state_struct.fValue = 1.0f;
		}
		else
		{
			if(pUserControls[control_code].state == CST_ACTIVE || 
				pUserControls[control_code].state == CST_INACTIVE)
			_state_struct.state = CST_INACTIVATED;
				else 
			_state_struct.state = CST_INACTIVE;
			_state_struct.lValue = 0;
			_state_struct.fValue = 0.0f;
		}
	}
	
	if(pUserControls[control_code].nframe != nFrameCounter)
	{
		pUserControls[control_code].nframe = nFrameCounter;
		pUserControls[control_code].state = _state_struct.state;
	}
	else _state_struct.state = pUserControls[control_code].state;

	return true;
}

void PC_CONTROLS::Update(DWORD DeltaTime)
{
	DWORD n;
	for(n=0;n<nDevicesNum;n++) pDevices[n]->Update();
	nFrameCounter++;
}

bool PC_CONTROLS::SetControlFlags(long code, DWORD _flags)
{
	if(code < 0 || code >= nControlsNum) return false;
	pUserControls[code].flags = _flags;
	return true;
}

bool PC_CONTROLS::SetControlState(char * control_name, CONTROL_STATE & _state_struct)
{
	DWORD n;
	if(control_name == 0) return false;
	for(n=0;n<nControlsNum;n++)
	{
		if(stricmp(control_name,pUserControls[n].name)==0) return SetControlState(n,_state_struct);
	}
	return false;
}

bool PC_CONTROLS::SetControlState(long control_code, CONTROL_STATE & _state_struct)
{
	if(control_code < 0 || control_code >= nControlsNum) return false;
	pUserControls[control_code].state = _state_struct.state;
	return true;
}