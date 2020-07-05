#ifndef _PCS_CONTROLS_H_
#define _PCS_CONTROLS_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\templates\array.h"
#include "KeyBuffer.h"
#include "ControlTree.h"

struct SYSTEM_CONTROL_ELEMENT
{
	DWORD update_frame;
	CONTROL_STATE state;
};

#define CONTROL_ELEMENTS_NUM	260
#define CE_MOUSE_X_AXIS			256
#define CE_MOUSE_Y_AXIS			257
#define CE_MOUSE_WHEEL_UP		258
#define CE_MOUSE_WHEEL_DOWN		259

class PCS_CONTROLS : public CONTROLS 
{
	bool m_bLockAll;

	float fMouseSensivityX;
	float fMouseSensivityY;
	VAPI * api;
	
	long nSystemControlsNum;
	USER_CONTROL * pUserControls;
	long nControlsNum;
	DWORD nFrameCounter;
	long nLastControlTime;

	long nMouseWheel;

	SYSTEM_CONTROL_ELEMENT ControlsTab[CONTROL_ELEMENTS_NUM];

	long nMouseXPrev, nMouseYPrev;

	ControlKeyBuffer m_KeyBuffer;

	ControlTree m_ControlTree;

public:
	 PCS_CONTROLS();
	~PCS_CONTROLS();

	void Update(DWORD DeltaTime);
	long GetSystemControlsNum();
	bool GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC & _control_desc_struct);

	void Release();
	void AddSystemControl(SYSTEM_CONTROL_DESC & _control_desc_struct);

	long CreateControl(char * control_name);
	long GetControlsNum();
	bool GetControlDesc(long code, USER_CONTROL & _user_desc_struct);
	bool SetControlFlags(long code, DWORD _flags);

	long GetDevicesNum();
	bool GetDeviceDesc(long code, DEVICE_DESC & _device_desc);

	long AddControlTreeNode(long nParent,const char* pcBaseControl,const char* pcOutControl,float fTimeOut);

	void MapControl(long control_code, long system_control_code);
	bool GetControlState(long control_code, CONTROL_STATE & _state_struct);
	bool GetControlState(char * control_name, CONTROL_STATE & _state_struct);
	bool SetControlState(char * control_name, CONTROL_STATE & _state_struct);
	bool SetControlState(long control_code, CONTROL_STATE & _state_struct);
	void AppState(bool state);
	long LastControlTime();
	void LockControl(char * control_name, bool mode);

	void SetMouseSensivityX(float);
	void SetMouseSensivityY(float);

	short GetDebugAsyncKeyState(int vk){return (m_bIsOffDebugKeys?0:GetAsyncKeyState(vk));}
	short GetDebugKeyState(int vk){return (m_bIsOffDebugKeys?0:GetKeyState(vk));}
	bool m_bIsOffDebugKeys;

	// Команда управления от ENGINE
	void EngineMessage(UINT iMsg,WPARAM wParam,LPARAM lParam);

	// Получить буфер нажатык клавиш за кадр (с учетом языка)
	virtual long GetKeyBufferLength();
	virtual const KeyDescr* GetKeyBuffer();
	virtual void ClearKeyBuffer();
};

#endif
