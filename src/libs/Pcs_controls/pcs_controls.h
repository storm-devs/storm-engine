#pragma once

#include "../Animation/ActionInfo.h"
#include "ControlTree.h"
#include "KeyBuffer.h"
#include "vmodule_api.h"

struct SYSTEM_CONTROL_ELEMENT
{
    uint32_t update_frame;
    CONTROL_STATE state;
};

#define CONTROL_ELEMENTS_NUM 260
#define CE_MOUSE_X_AXIS 256
#define CE_MOUSE_Y_AXIS 257
#define CE_MOUSE_WHEEL_UP 258
#define CE_MOUSE_WHEEL_DOWN 259

class PCS_CONTROLS : public CONTROLS
{
    bool m_bLockAll;

    float fMouseSensivityX;
    float fMouseSensivityY;

    long nSystemControlsNum;
    std::vector<USER_CONTROL> pUserControls;
    long nControlsNum;
    uint32_t nFrameCounter;
    long nLastControlTime;

    long nMouseWheel;

    SYSTEM_CONTROL_ELEMENT ControlsTab[CONTROL_ELEMENTS_NUM];

    long nMouseXPrev, nMouseYPrev;

    ControlKeyBuffer m_KeyBuffer;

    ControlTree m_ControlTree;

  public:
    PCS_CONTROLS();
    ~PCS_CONTROLS();

    void Update(uint32_t DeltaTime);
    long GetSystemControlsNum();
    bool GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC &_control_desc_struct);

    void Release();
    void AddSystemControl(SYSTEM_CONTROL_DESC &_control_desc_struct);

    long CreateControl(const char *control_name);
    long GetControlsNum();
    bool GetControlDesc(long code, USER_CONTROL &_user_desc_struct);
    bool SetControlFlags(long code, uint32_t flags);

    long GetDevicesNum();
    bool GetDeviceDesc(long code, DEVICE_DESC &_device_desc);

    long AddControlTreeNode(long nParent, const char *pcBaseControl, const char *pcOutControl, float fTimeOut);

    void MapControl(long control_code, long system_control_code);
    bool GetControlState(long control_code, CONTROL_STATE &_state_struct);
    bool GetControlState(const char *control_name, CONTROL_STATE &_state_struct);
    bool SetControlState(const char *control_name, CONTROL_STATE &_state_struct);
    bool SetControlState(long control_code, CONTROL_STATE &_state_struct);
    void AppState(bool state);
    long LastControlTime();
    void SetControlTreshold(long control_code, float thval);
    void LockControl(const char *control_name, bool mode);

    void SetMouseSensivityX(float);
    void SetMouseSensivityY(float);

    short GetDebugAsyncKeyState(int vk)
    {
        return (m_bIsOffDebugKeys ? 0 : GetAsyncKeyState(vk));
    }
    short GetDebugKeyState(int vk)
    {
        return (m_bIsOffDebugKeys ? 0 : GetKeyState(vk));
    }
    bool m_bIsOffDebugKeys;

    // Control command from ENGINE
    void EngineMessage(UINT iMsg, WPARAM wParam, LPARAM lParam);

    // Get the keystroke buffer per frame (taking into account the language)
    virtual long GetKeyBufferLength();
    virtual const KeyDescr *GetKeyBuffer();
    virtual void ClearKeyBuffer();
};
