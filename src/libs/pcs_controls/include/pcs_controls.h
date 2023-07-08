#pragma once

#include "control_tree.h"
#include "key_buffer.h"

#include <shared/controls.h>

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

class PCS_CONTROLS : public CONTROLS
{
    bool m_bLockAll;
    bool updateCursor_ = true;

    float fMouseSensivityX;
    float fMouseSensivityY;

    int32_t nSystemControlsNum;
    std::vector<USER_CONTROL> pUserControls;
    int32_t nControlsNum;
    uint32_t nFrameCounter;
    int32_t nLastControlTime;

    int32_t nMouseWheel;
    int32_t nMouseDx, nMouseDy;

    SYSTEM_CONTROL_ELEMENT ControlsTab[CONTROL_ELEMENTS_NUM];

    ControlKeyBuffer m_KeyBuffer;

    ControlTree m_ControlTree;

    std::shared_ptr<storm::Input> input_;
    int inputHandlerID_ = 0;

  public:
    PCS_CONTROLS();
    ~PCS_CONTROLS() override;

    void Update(uint32_t DeltaTime) override;
    int32_t GetSystemControlsNum() override;
    bool GetSystemControlDesc(int32_t code, SYSTEM_CONTROL_DESC &_control_desc_struct) override;

    void Release();
    void AddSystemControl(SYSTEM_CONTROL_DESC &_control_desc_struct);

    int32_t CreateControl(const char *control_name) override;
    int32_t GetControlsNum() override;
    bool GetControlDesc(int32_t code, USER_CONTROL &_user_desc_struct) override;
    bool SetControlFlags(int32_t code, uint32_t flags) override;

    int32_t GetDevicesNum() override;
    bool GetDeviceDesc(int32_t code, DEVICE_DESC &_device_desc) override;

    int32_t AddControlTreeNode(int32_t nParent, const char *pcBaseControl, const char *pcOutControl, float fTimeOut) override;

    void MapControl(int32_t control_code, int32_t system_control_code) override;
    bool GetControlState(int32_t control_code, CONTROL_STATE &_state_struct) override;
    bool GetControlState(const char *control_name, CONTROL_STATE &_state_struct) override;
    bool SetControlState(const char *control_name, CONTROL_STATE &_state_struct) override;
    bool SetControlState(int32_t control_code, CONTROL_STATE &_state_struct) override;
    void AppState(bool state) override;
    int32_t LastControlTime() override;
    void SetControlTreshold(int32_t control_code, float thval) override;
    void LockControl(const char *control_name, bool mode) override;

    void SetMouseSensivityX(float) override;
    void SetMouseSensivityY(float) override;

    short GetAsyncKeyState(int vk) override;
    short GetKeyState(int vk) override;
    short GetDebugAsyncKeyState(int vk) override;
    short GetDebugKeyState(int vk) override;
    bool m_bIsOffDebugKeys;

    bool IsKeyPressed(int vk) override;

    void HandleEvent(const storm::InputEvent &evt);

    // Get the keystroke buffer per frame (taking into account the language)
    int32_t GetKeyBufferLength() override;
    const KeyDescr *GetKeyBuffer() override;
    void ClearKeyBuffer() override;
};
