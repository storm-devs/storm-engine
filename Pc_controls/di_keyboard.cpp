#include "DI_KEYBOARD.h"
#include "..\common_h\vmodule_api.h"

DI_KEYBOARD * pKeyboardDevice = 0;
DWORD nControlN = 0;
BOOL CALLBACK DIEnumKeyboardObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	if(pKeyboardDevice == 0) return false;
	//_CORE_API->Trace("    Device control[%d]: %s",nControlN,lpddoi->tszName);
	nControlN++;
	return true;
}

DI_KEYBOARD::DI_KEYBOARD(IDirectInput8 * _pdinput, GUID _guid, bool & bRes)
{
	pKeyboardDevice = this;
	HRESULT hRes;
	pDInput = _pdinput;
	DWORD Ref;
	bRes = true;
	pDIKeyboard = 0;
	hRes = pDInput->CreateDevice(_guid,&pDIKeyboard,NULL);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hRes)) {bRes = false; return;}

	//hRes = pDIKeyboard->SetCooperativeLevel(_CORE_API->GetAppHWND(), DISCL_EXCLUSIVE|DISCL_FOREGROUND);
	hRes = pDIKeyboard->SetCooperativeLevel(_CORE_API->GetAppHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIKeyboard->EnumObjects(DIEnumKeyboardObjectsCallback,&Ref,DIDFT_BUTTON);
	if(FAILED(hRes)) {bRes = false; return;}
}

DI_KEYBOARD::~DI_KEYBOARD()
{
	if(pDIKeyboard) 
	{
		pDIKeyboard->Unacquire();
		pDIKeyboard->Release();
	}
}

char * DI_KEYBOARD::GetDeviceName()
{
	return "Keyboard";
}

long DI_KEYBOARD::GetControlsNum()
{
	return 256;
}

const char *  DI_KEYBOARD::GetControlName(long control_n, char * buffer)
{
	return "?";
}

void DI_KEYBOARD::SetAppState(bool _state) 
{
	bAppState = _state;
	if(bAppState) pDIKeyboard->Acquire(); else pDIKeyboard->Unacquire();
}

void DI_KEYBOARD::Update()
{
	HRESULT hRes;
	hRes = pDIKeyboard->GetDeviceState(sizeof(KeyBuffer),(LPVOID)&KeyBuffer); 
	if(hRes == DIERR_INPUTLOST) 
	{
		pDIKeyboard->Acquire();
		hRes = pDIKeyboard->GetDeviceState(sizeof(KeyBuffer),(LPVOID)&KeyBuffer); 
	}
}

bool DI_KEYBOARD::GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate)
{
	DWORD nControlN;
	
	_state_struct.fValue = 0.0f;
	_state_struct.lValue = 0;
	_state_struct.state = CST_INACTIVE;

	if(!bAppState) return false;

	nControlN = control_code & 0xffff;
	if(nControlN >= 256) return false;


	BYTE state = KeyBuffer[nControlN] & 0x80;
	if(state != 0)
	{
		_state_struct.lValue = 1;
		_state_struct.fValue = 1.0f;
		if(_oldstate == CST_INACTIVATED || _oldstate == CST_INACTIVE)
		_state_struct.state = CST_ACTIVATED;
		else _state_struct.state = CST_ACTIVE;
	} 
	else
	{
		_state_struct.lValue = 0;
		_state_struct.fValue = 0.0f;
		_state_struct.state = CST_INACTIVE;

		if(_oldstate == CST_ACTIVATED || _oldstate == CST_ACTIVE)
		_state_struct.state = CST_INACTIVATED;
		else _state_struct.state = CST_INACTIVE;
	}

	return true;
}