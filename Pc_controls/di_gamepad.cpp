#include "di_gamepad.h"
#include "..\common_h\vmodule_api.h"

DI_GAMEPAD * pGamepadDevice = 0;

BOOL CALLBACK DIEnumGamepadObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	if(pGamepadDevice == 0) return false;
/*	_CORE_API->Trace("    Device control: %s, offset: %d",lpddoi->tszName,lpddoi->dwOfs);
	if(lpddoi->dwType & DIDFT_BUTTON)_CORE_API->Trace("      Button");
	if(lpddoi->dwType & DIDFT_AXIS)_CORE_API->Trace("      Axis");
	if(lpddoi->dwType & DIDFT_COLLECTION)_CORE_API->Trace("      Collection");
	if(lpddoi->dwType & DIDFT_FFACTUATOR )_CORE_API->Trace("      ForceFeedBack");
	if(lpddoi->dwType & DIDFT_VENDORDEFINED )_CORE_API->Trace("      Vendor Defined");
	if(lpddoi->dwType & DIDFT_POV )_CORE_API->Trace("      Point of view controller");
*/	 
	pGamepadDevice->AddObject(lpddoi);
	
	return true;
}

void DI_GAMEPAD::AddObject(LPCDIDEVICEOBJECTINSTANCE lpddoi)
{
	DWORD n;
	if(lpddoi->dwType & DIDFT_BUTTON)
	{
		_CORE_API->Trace("    [%d] Button: %s, offset: %d",nObjectsNum,lpddoi->tszName,lpddoi->dwOfs);
		n = nObjectsNum;
		nObjectsNum++;
		pDIODF = (DIOBJECTDATAFORMAT *)RESIZE(pDIODF,nObjectsNum * sizeof(DIOBJECTDATAFORMAT));
		pDIODF[n].pguid = 0;
		pDIODF[n].dwOfs = nDataSize;//lpddoi->dwOfs;
		pDIODF[n].dwType = lpddoi->dwType;
		pDIODF[n].dwFlags = lpddoi->dwFlags;

		nDataSize += sizeof(BYTE);
	} else
	
	if(lpddoi->dwType & DIDFT_AXIS)
	{
		_CORE_API->Trace("    [%d] Axis: %s, offset: %d",nObjectsNum,lpddoi->tszName,lpddoi->dwOfs);
		n = nObjectsNum;
		nObjectsNum++;
		pDIODF = (DIOBJECTDATAFORMAT *)RESIZE(pDIODF,nObjectsNum * sizeof(DIOBJECTDATAFORMAT));
		pDIODF[n].pguid = 0;
		pDIODF[n].dwOfs = nDataSize;//lpddoi->dwOfs;
		pDIODF[n].dwType = lpddoi->dwType;
		pDIODF[n].dwFlags = lpddoi->dwFlags;

		nDataSize += sizeof(DWORD);


		DIPROPRANGE dd;
		HRESULT hRes;
		dd.diph.dwSize = sizeof(DIPROPRANGE);
		dd.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dd.diph.dwHow = DIPH_BYID;
		dd.diph.dwObj = lpddoi->dwType;
		//dd.lMin = 0;
		//dd.lMax = 65000;
		hRes = pDIGamepad->GetProperty(DIPROP_RANGE,&dd.diph);
		if(hRes == DI_OK) _CORE_API->Trace("    Range: %d . . %d",dd.lMin,dd.lMax);

		DIPROPDWORD dw;
		dw.diph.dwSize = sizeof(DIPROPDWORD);
		dw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dw.diph.dwHow = DIPH_BYID;
		dw.diph.dwObj = lpddoi->dwType;
		if(n == 0 || n == 1)dw.dwData = 5000;
		else dw.dwData = 2500;	// 10000 = 100% dead zone
		hRes = pDIGamepad->SetProperty(DIPROP_DEADZONE,&dw.diph);
		

	} 
	else
	if(lpddoi->dwType & DIDFT_POV)
	{
		_CORE_API->Trace("    [%d] ???????: %s, offset: %d",nObjectsNum,lpddoi->tszName,lpddoi->dwOfs);
		n = nObjectsNum;
		fakeN = n;
		nObjectsNum++;
		pDIODF = (DIOBJECTDATAFORMAT *)RESIZE(pDIODF,nObjectsNum * sizeof(DIOBJECTDATAFORMAT));
		pDIODF[n].pguid = 0;
		pDIODF[n].dwOfs = nDataSize;//lpddoi->dwOfs;
		pDIODF[n].dwType = lpddoi->dwType;
		pDIODF[n].dwFlags = lpddoi->dwFlags;

		nDataSize += 8 * sizeof(BYTE);	// @#$@%!!$!$$%^&&&?????
	}

	if(nDataSize >= nDataBufferSize)
	{
		nDataBufferSize = (nDataSize/sizeof(DWORD) + 1)*sizeof(DWORD);
		pDataBuffer = (BYTE *)RESIZE(pDataBuffer,nDataBufferSize);
	}
}

DI_GAMEPAD::DI_GAMEPAD(IDirectInput8 * _pdinput, GUID _guid, bool & bRes)
{
	fakeN = 0xffffffff;
	pGamepadDevice = this;
	HRESULT hRes;
	pDInput = _pdinput;
	DWORD Ref;
	bRes = true;
	pDIGamepad = 0;
	pDIODF = 0;
	nObjectsNum = 0;
	pDataBuffer = 0;
	nDataBufferSize = 0;
	nDataSize = 0;

	hRes = pDInput->CreateDevice(_guid,&pDIGamepad,NULL);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIGamepad->EnumObjects(DIEnumGamepadObjectsCallback,&Ref,DIDFT_ALL);
	if(FAILED(hRes)) {bRes = false; return;}

	DIDATAFORMAT didf;
	didf.dwSize = sizeof(didf);
	didf.dwNumObjs = nObjectsNum;
	didf.rgodf = pDIODF;
	didf.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
	didf.dwFlags = DIDF_ABSAXIS;//DIDF_RELAXIS;
	didf.dwDataSize = nDataBufferSize;

	//hRes = pDIGamepad->SetDataFormat(&c_dfDIJoystick2);
	hRes = pDIGamepad->SetDataFormat(&didf);
	if(FAILED(hRes)) {bRes = false; return;}

	hRes = pDIGamepad->SetCooperativeLevel(_CORE_API->GetAppHWND(), DISCL_EXCLUSIVE|DISCL_FOREGROUND);
	if(FAILED(hRes)) {bRes = false; return;}

	
/*
	//DIPROPDWORD dd;
	DIPROPRANGE dd;
	dd.diph.dwSize = sizeof(DIPROPDWORD);
	dd.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dd.diph.dwHow = DIPH_DEVICE;
	dd.diph.dwObj = 0;
	dd.lMin = 0;// .dwData = DIPROPAXISMODE_REL;
	dd.lMax = 32000;
	//hRes = pDIGamepad->SetProperty(DIPROP_AXISMODE,&dd.diph);
	hRes = pDIGamepad->SetProperty(DIPROP_RANGE,&dd.diph);
	if(FAILED(hRes)) {bRes = false; return;}
*/
}

DI_GAMEPAD::~DI_GAMEPAD()
{
	if(pDIGamepad) 
	{
		pDIGamepad->Unacquire();
		pDIGamepad->Release();
	}
	if(pDIODF) delete pDIODF;
	if(pDataBuffer) delete pDataBuffer;
}

char * DI_GAMEPAD::GetDeviceName()
{
	return "Gamepad";
}

long DI_GAMEPAD::GetControlsNum()
{
	return nObjectsNum;
}

const char *  DI_GAMEPAD::GetControlName(long control_n, char * buffer)
{
	return "?";
}

void DI_GAMEPAD::SetAppState(bool _state) 
{
	bAppState = _state;
	if(bAppState) pDIGamepad->Acquire(); else pDIGamepad->Unacquire();
}

void DI_GAMEPAD::Update()
{
	HRESULT hRes;
	hRes = pDIGamepad->GetDeviceState(nDataBufferSize,pDataBuffer);
	if(hRes == DIERR_INPUTLOST) 
	{
		pDIGamepad->Acquire();
		hRes = pDIGamepad->GetDeviceState(nDataBufferSize,pDataBuffer);
	}
	if(hRes != DI_OK) return;

/*	DWORD n;
	BYTE state;
	long lstate;
	_CORE_API->Trace("----------------- GAMEPAD_STATE ----------------");	
	for(n=0;n<nObjectsNum;n++)
	{
		if(pDIODF[n].dwType & DIDFT_BUTTON)
		{
			state = pDataBuffer[pDIODF[n].dwOfs];
			_CORE_API->Trace("btn[%d]: %d",n,state);
		}
		if(pDIODF[n].dwType & DIDFT_AXIS)
		{
			lstate = *((long *)(pDataBuffer + pDIODF[n].dwOfs));
			_CORE_API->Trace("axs[%d]: %d",n,lstate);
		}
	}*/
}

bool DI_GAMEPAD::GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate)
{
	DWORD n;
	
	_state_struct.fValue = 0.0f;
	_state_struct.lValue = 0;
	_state_struct.state = CST_INACTIVE;

	if(!bAppState) return false;

	n = control_code & 0xffff;

	// fake
	if(n >= 256)
	{
		if(fakeN == 0xffffffff) return false;
		
		if(pDIODF[fakeN].dwType & DIDFT_POV)
		{
			long lstate = *((long *)(pDataBuffer + pDIODF[fakeN].dwOfs));
			//lstate = (lstate - 32768.0f)* 0.00025f;
			switch(n - 256)
			{
				// control 1
				case 0:
					if(lstate == 0 || lstate == 4500 || lstate == 31500)
					{
						_state_struct.lValue = 1;
						_state_struct.fValue = 1.0f;
						if(_oldstate == CST_INACTIVE || _oldstate == CST_INACTIVATED) _state_struct.state = CST_ACTIVATED;
						else _state_struct.state = CST_ACTIVE;
					}
					else 
					{
						_state_struct.lValue = 0;
						_state_struct.fValue = 0.0f;
						if(_oldstate == CST_ACTIVE || _oldstate == CST_ACTIVATED) _state_struct.state = CST_INACTIVATED;
						else _state_struct.state = CST_INACTIVE;
					}
				break;
				// control 2
				case 1:
					if(lstate == 4500 || lstate == 9000 || lstate == 13500)
					{
						_state_struct.lValue = 1;
						_state_struct.fValue = 1.0f;
						if(_oldstate == CST_INACTIVE || _oldstate == CST_INACTIVATED) _state_struct.state = CST_ACTIVATED;
						else _state_struct.state = CST_ACTIVE;
					}
					else 
					{
						_state_struct.lValue = 0;
						_state_struct.fValue = 0.0f;
						if(_oldstate == CST_ACTIVE || _oldstate == CST_ACTIVATED) _state_struct.state = CST_INACTIVATED;
						else _state_struct.state = CST_INACTIVE;
					}

				break;
				// control 3
				case 2:
					if(lstate == 13500 || lstate == 18000 || lstate == 22500)
					{
						_state_struct.lValue = 1;
						_state_struct.fValue = 1.0f;
						if(_oldstate == CST_INACTIVE || _oldstate == CST_INACTIVATED) _state_struct.state = CST_ACTIVATED;
						else _state_struct.state = CST_ACTIVE;
					}
					else 
					{
						_state_struct.lValue = 0;
						_state_struct.fValue = 0.0f;
						if(_oldstate == CST_ACTIVE || _oldstate == CST_ACTIVATED) _state_struct.state = CST_INACTIVATED;
						else _state_struct.state = CST_INACTIVE;
					}
				break;
				// control 4
				case 3:
					if(lstate == 22500 || lstate == 27000 || lstate == 31500)
					{
						_state_struct.lValue = 1;
						_state_struct.fValue = 1.0f;
						if(_oldstate == CST_INACTIVE || _oldstate == CST_INACTIVATED) _state_struct.state = CST_ACTIVATED;
						else _state_struct.state = CST_ACTIVE;
					}
					else 
					{
						_state_struct.lValue = 0;
						_state_struct.fValue = 0.0f;
						if(_oldstate == CST_ACTIVE || _oldstate == CST_ACTIVATED) _state_struct.state = CST_INACTIVATED;
						else _state_struct.state = CST_INACTIVE;
					}
				break;
			}
			//_state_struct.lValue = lstate;
			//_state_struct.fValue = (float)lstate;
		}
		return true;
	}

	if(n >= nObjectsNum) return false;

	if(pDIODF[n].dwType & DIDFT_BUTTON)
	{
		BYTE state = pDataBuffer[pDIODF[n].dwOfs];
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
	} 
	else
	if(pDIODF[n].dwType & DIDFT_AXIS)
	{
		long lstate = *((long *)(pDataBuffer + pDIODF[n].dwOfs));
		lstate = (lstate - 32768.0f)* 0.00025f;
		_state_struct.lValue = lstate;
		_state_struct.fValue = (float)lstate;
		if(lstate != 0)
		{
			_state_struct.state = CST_ACTIVE;
		} 
		else
		{
			_state_struct.state = CST_INACTIVE;
		}
	}
	/*else
	if(pDIODF[n].dwType & DIDFT_POV)
	{
		long lstate = *((long *)(pDataBuffer + pDIODF[n].dwOfs));
		//lstate = (lstate - 32768.0f)* 0.00025f;
		_state_struct.lValue = lstate;
		_state_struct.fValue = (float)lstate;
		char s[8];
		char ss[256];
		DWORD mask;
		mask = 1;
		//memcpy(s,pDataBuffer + pDIODF[n].dwOfs,8);
		wsprintf(ss,"HS: %d  - ",lstate);

		for(long m=0;m<16;m++)
		{
			if(lstate & mask) strcat(ss,"1 ");
			else strcat(ss,"0 ");
			mask = mask<<1;
			//api->Trace("HS: %d %d %d %d %d %d %d %d", s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]);
			if(m == 3 || m == 7 || m == 11) strcat(ss,"  ");
			
		}
		api->Trace(ss);
		
	}*/

	

	return true;
}