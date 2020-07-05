#include "di_mouse.h"
#include "..\common_h\vmodule_api.h"

DI_MOUSE * pMouseDevice = 0;

BOOL CALLBACK DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	if(pMouseDevice == 0) return false;
	pMouseDevice->AddObject(lpddoi->dwOfs,lpddoi->tszName);
	_CORE_API->Trace("    Device control: %s",lpddoi->tszName);
	return true;
}

void DI_MOUSE::AddObject(DWORD _offset, const char * name)
{
	if(nObjects >= MOUSE_OBJECTS_MAXNUM) return;
	Object[nObjects].offset = _offset;
	Object[nObjects].name = name;
	nObjects++;
}

DI_MOUSE::DI_MOUSE(IDirectInput8 * _pdinput, GUID _guid, bool & bRes)
{
	pMouseDevice = this;
	HRESULT hRes;
	pDInput = _pdinput;
	DWORD Ref;
	bRes = true;
	pDIMouse = 0;
	nObjects = 0;

	hRes = pDInput->CreateDevice(_guid,&pDIMouse,NULL);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIMouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIMouse->SetCooperativeLevel(_CORE_API->GetAppHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIMouse->EnumObjects(DIEnumDeviceObjectsCallback,&Ref,DIDFT_ALL);
	if(FAILED(hRes)) {bRes = false; return;}

	DIPROPDWORD dd;
	dd.diph.dwSize = sizeof(DIPROPDWORD);
	dd.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dd.diph.dwHow = DIPH_DEVICE;
	dd.diph.dwObj = 0;
	dd.dwData = DIPROPAXISMODE_REL;
	hRes = pDIMouse->SetProperty(DIPROP_AXISMODE,&dd.diph);
	if(FAILED(hRes)) {bRes = false; return;}

	RECT r;
	GetWindowRect(_CORE_API->GetAppHWND(),&r);
	ClipCursor(&r);
}

DI_MOUSE::~DI_MOUSE()
{
	if(pDIMouse) 
	{
		pDIMouse->Unacquire();
		pDIMouse->Release();
	}
}

char * DI_MOUSE::GetDeviceName()
{
	return "Mouse";
}

long DI_MOUSE::GetControlsNum()
{
	return nObjects;
}

const char * DI_MOUSE::GetControlName(long control_n, char * buffer)
{
	control_n = control_n & 0xffff;
	if(control_n < 0 || control_n >= nObjects) return "Inavalid object code";
	return Object[control_n].name;
}

void DI_MOUSE::SetAppState(bool _state) 
{
	bAppState = _state;
	if(bAppState) 
	{
		pDIMouse->Acquire(); 
		RECT r;
		GetWindowRect(_CORE_API->GetAppHWND(),&r);
		ClipCursor(&r);
	}
	else 
	{
		pDIMouse->Unacquire();
		ClipCursor(0);
	}
}

void DI_MOUSE::Update()
{
	HRESULT hRes;
	hRes = pDIMouse->GetDeviceState(sizeof(dims),&dims);
	if(hRes == DIERR_INPUTLOST) 
	{
		pDIMouse->Acquire();
		hRes = pDIMouse->GetDeviceState(sizeof(dims),&dims);
	}
	// pDIMouse ? ...
}

bool DI_MOUSE::GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate)
{
//	HRESULT hRes;
	long nControlN;
	
	_state_struct.fValue = 0.0f;
	_state_struct.lValue = 0;
	_state_struct.state = CST_INACTIVE;

	if(!bAppState) return false;

	nControlN = control_code & 0xffff;
	if(nControlN < 0  || nControlN >= MOUSE_OBJECTS_MAXNUM) return false;

/*	hRes = pDIMouse->GetDeviceState(sizeof(dims),&dims);
	if(hRes == DIERR_INPUTLOST) 
	{
		pDIMouse->Acquire();
		hRes = pDIMouse->GetDeviceState(sizeof(dims),&dims);
	}
	if(hRes != DI_OK) return false;
//*/
//	_CORE_API->Trace("DI_MOUSE OK! : object code: %d",nControlN);

	switch(Object[nControlN].offset)
	{
		case DIMOFS_BUTTON0:
			if(dims.rgbButtons[0])
			{
				_state_struct.state = CST_ACTIVE;
				_state_struct.lValue = 1;
				_state_struct.fValue = 1.0f;
			}
			else 
			{
				_state_struct.state = CST_INACTIVE;
				_state_struct.lValue = 0;
				_state_struct.fValue = 0.0f;
			}
		break;
		case DIMOFS_BUTTON1:
			if(dims.rgbButtons[1])
			{
				_state_struct.state = CST_ACTIVE;
				_state_struct.lValue = 1;
				_state_struct.fValue = 1.0f;
			}
			else 
			{
				_state_struct.state = CST_INACTIVE;
				_state_struct.lValue = 0;
				_state_struct.fValue = 0.0f;
			}
		break;
		case DIMOFS_BUTTON2:
			if(dims.rgbButtons[2])
			{
				_state_struct.state = CST_ACTIVE;
				_state_struct.lValue = 1;
				_state_struct.fValue = 1.0f;
			}
			else 
			{
				_state_struct.state = CST_INACTIVE;
				_state_struct.lValue = 0;
				_state_struct.fValue = 0.0f;
			}
		break;
		case DIMOFS_BUTTON3:
			if(dims.rgbButtons[3])
			{
				_state_struct.state = CST_ACTIVE;
				_state_struct.lValue = 1;
				_state_struct.fValue = 1.0f;
			}
			else 
			{
				_state_struct.state = CST_INACTIVE;
				_state_struct.lValue = 0;
				_state_struct.fValue = 0.0f;
			}
		break;
		case DIMOFS_X:
			_state_struct.lValue = dims.lX;
			_state_struct.fValue = (float)dims.lX;
		break;
		case DIMOFS_Y:
			_state_struct.lValue = dims.lY;
			_state_struct.fValue = (float)dims.lY;
		break;
		case DIMOFS_Z:
			_state_struct.lValue = dims.lZ;
			_state_struct.fValue = (float)dims.lZ;
		break;
	}
	return true;
}