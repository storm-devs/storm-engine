#ifndef _DEVICE_CONTROLS_H_
#define _DEVICE_CONTROLS_H_

#include "..\common_h\vmodule_api.h"

class DEVICE_CONTROLS
{
protected:
	bool bAppState;
public:
	DEVICE_CONTROLS(){bAppState = true;};
	virtual ~DEVICE_CONTROLS(){};
	virtual char * GetDeviceName()= 0;
	virtual long GetControlsNum()= 0;
	virtual const char * GetControlName(long control_n, char * buffer)= 0;
	virtual void SetAppState(bool _state) {bAppState = _state;};
	virtual bool GetControlState(long control_code, CONTROL_STATE & _state_struct, CONTROL_STATE_TYPE _oldstate){return false;};
	virtual void Update(){};
};

#endif