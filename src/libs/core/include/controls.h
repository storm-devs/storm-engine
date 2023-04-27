#pragma once

#include <shared/controls.h>

#include "utf8.h"
#include <cstdint>

#define INVALID_CONTROL_CODE 0xffffffff
#define UNASSIGNED_CONTROL 0xffffffff

struct DEVICE_DESC
{
    const char *name;
};

struct SYSTEM_CONTROL_DESC
{
    enum CONTROL_TYPE
    {
        CT_BUTTON,
        CT_AXIS,
        CT_FORCE_FEEDBACK,
        FORCE_DWORD = 0xffffffff
    };

    CONTROL_TYPE ControlType;
    const char *pControlName;
};

enum CONTROL_STATE_TYPE
{
    CST_INACTIVE,
    CST_ACTIVE,
    CST_INACTIVATED,
    CST_ACTIVATED,
    FORCE_DWORD = 0xffffffff
};

enum USER_CONTROL_TYPE
{
    UCT_Standart,
    UCT_ControlTree,
    UCT_FORCE_DWORD = 0xffffffff
};

struct USER_CONTROL
{
    char *name;
    int32_t system_code;
    uint32_t flags;
    CONTROL_STATE_TYPE state;
    uint32_t nframe;
    float fTreshold;
    bool bLocked;
    bool bSystemLocked;
    USER_CONTROL_TYPE control_type;
};

enum FFB_EFFECT
{
    FFB_EFFECT0,
    FFB_EFFECT1,
};

struct CONTROL_STATE
{
    CONTROL_STATE_TYPE state;
    float fValue;
    int32_t lValue;
};

struct KeyDescr
{
    utf8::u8_char ucVKey;
    bool bSystem;
};

class CONTROLS
{
  public:
    CONTROLS(){};

    virtual ~CONTROLS(){};

    virtual void Update(uint32_t DeltaTime){};

    virtual int32_t GetSystemControlsNum()
    {
        return 0;
    };

    virtual bool GetSystemControlDesc(int32_t code, SYSTEM_CONTROL_DESC &_control_desc_struct)
    {
        return false;
    };

    virtual void ResetControlsMap(){};

    virtual int32_t CreateControl(const char *control_name)
    {
        return 0;
    };

    virtual int32_t GetControlsNum()
    {
        return 0;
    };

    virtual bool GetControlDesc(int32_t code, USER_CONTROL &_user_desc_struct)
    {
        return false;
    };

    virtual bool SetControlFlags(int32_t code, uint32_t flags)
    {
        return true;
    }

    virtual int32_t GetDevicesNum()
    {
        return 0;
    };

    virtual bool GetDeviceDesc(int32_t code, DEVICE_DESC &_device_desc)
    {
        return false;
    };

    virtual int32_t GetDeviceControlsNum(int32_t device_code)
    {
        return 0;
    };

    virtual char *GetDeviceControlName(int32_t device_code, int32_t code)
    {
        return nullptr;
    };

    virtual int32_t AddControlTreeNode(int32_t nParent, const char *pcBaseControl, const char *pcOutControl, float fTimeOut)
    {
        return -1;
    }

    virtual void MapControl(int32_t control_code, int32_t system_control_code){};

    virtual void AppState(bool state){};

    /*    virtual CONTROL_STATE_TYPE GetControlState(char * control_name, float * pvalue = 0){return CST_INACTIVE;};
      virtual CONTROL_STATE_TYPE GetControlState(int32_t control_code, float * pvalue = 0){return CST_INACTIVE;};
      virtual float GetControlValue(char * control_name, CONTROL_STATE_TYPE * pstate = 0){return 0;};
      virtual float GetControlValue(int32_t control_code, CONTROL_STATE_TYPE * pstate = 0){return 0;};
    */
    virtual void SetControlEffect(FFB_EFFECT effect, int32_t time = 0){};

    virtual bool GetControlState(int32_t control_code, CONTROL_STATE &_state_struct)
    {
        memset(&_state_struct, 0, sizeof(_state_struct));
        return false;
    };

    virtual bool GetControlState(const char *control_name, CONTROL_STATE &_state_struct)
    {
        memset(&_state_struct, 0, sizeof(_state_struct));
        return false;
    };

    virtual bool SetControlState(const char *control_name, CONTROL_STATE &_state_struct)
    {
        memset(&_state_struct, 0, sizeof(_state_struct));
        return false;
    };

    virtual bool SetControlState(int32_t control_code, CONTROL_STATE &_state_struct)
    {
        memset(&_state_struct, 0, sizeof(_state_struct));
        return false;
    };

    virtual int32_t LastControlTime()
    {
        return 0;
    };

    virtual void SetControlTreshold(int32_t control_code, float thval){};

    virtual void LockControl(const char *control_name, bool mode){};

    virtual void SetMouseSensivityX(float){};

    virtual void SetMouseSensivityY(float){};

    virtual short GetAsyncKeyState(int vk)
    {
        return 0;
    }

    virtual short GetKeyState(int vk)
    {
        return 0;
    }

    virtual short GetDebugAsyncKeyState(int vk)
    {
        return 0;
    }

    virtual short GetDebugKeyState(int vk)
    {
        return 0;
    }

    virtual bool IsKeyPressed(int vk)
    {
        return 0;
    }

    // Get the keystroke buffer per frame (taking into account the language)
    virtual int32_t GetKeyBufferLength()
    {
        return 0;
    }

    virtual const KeyDescr *GetKeyBuffer()
    {
        return nullptr;
    }

    virtual void ClearKeyBuffer()
    {
    }
};
