#pragma once

#include "ControlTree.h"
#include "KeyBuffer.h"

#include <memory>

namespace storm
{
struct InputEvent;
class Input;
} // namespace storm

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
    long nMouseDx, nMouseDy;

    SYSTEM_CONTROL_ELEMENT ControlsTab[CONTROL_ELEMENTS_NUM];

    ControlKeyBuffer m_KeyBuffer;

    ControlTree m_ControlTree;

    std::shared_ptr<storm::Input> input_;
    int inputHandlerID_ = 0;

  public:
    PCS_CONTROLS();
    ~PCS_CONTROLS() override;

    void Update(uint32_t DeltaTime) override;
    long GetSystemControlsNum() override;
    bool GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC &_control_desc_struct) override;

    void Release();
    void AddSystemControl(SYSTEM_CONTROL_DESC &_control_desc_struct);

    long CreateControl(const char *control_name) override;
    long GetControlsNum() override;
    bool GetControlDesc(long code, USER_CONTROL &_user_desc_struct) override;
    bool SetControlFlags(long code, uint32_t flags) override;

    long GetDevicesNum() override;
    bool GetDeviceDesc(long code, DEVICE_DESC &_device_desc) override;

    long AddControlTreeNode(long nParent, const char *pcBaseControl, const char *pcOutControl, float fTimeOut) override;

    void MapControl(long control_code, long system_control_code) override;
    bool GetControlState(long control_code, CONTROL_STATE &_state_struct) override;
    bool GetControlState(const char *control_name, CONTROL_STATE &_state_struct) override;
    bool SetControlState(const char *control_name, CONTROL_STATE &_state_struct) override;
    bool SetControlState(long control_code, CONTROL_STATE &_state_struct) override;
    void AppState(bool state) override;
    long LastControlTime() override;
    void SetControlTreshold(long control_code, float thval) override;
    void LockControl(const char *control_name, bool mode) override;

    void SetMouseSensivityX(float) override;
    void SetMouseSensivityY(float) override;

    short GetDebugAsyncKeyState(int vk) override;
    short GetDebugKeyState(int vk) override;
    bool m_bIsOffDebugKeys;

    void HandleEvent(const storm::InputEvent &evt);

    // Get the keystroke buffer per frame (taking into account the language)
    long GetKeyBufferLength() override;
    const KeyDescr *GetKeyBuffer() override;
    void ClearKeyBuffer() override;
};
