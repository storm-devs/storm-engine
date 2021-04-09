#pragma once

#include "utf8.h"
#include <cstdint>

#define INVALID_CONTROL_CODE 0xffffffff
#define UNASSIGNED_CONTROL 0xffffffff

// control flags
#define USE_AXIS_AS_BUTTON 0x1
#define USE_AXIS_AS_INVERSEBUTTON 0x2
#define INVERSE_CONTROL 0x4

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
    long system_code;
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
    long lValue;
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
    virtual long GetSystemControlsNum()
    {
        return 0;
    };
    virtual bool GetSystemControlDesc(long code, SYSTEM_CONTROL_DESC &_control_desc_struct)
    {
        return false;
    };

    virtual void ResetControlsMap(){};

    virtual long CreateControl(const char *control_name)
    {
        return 0;
    };
    virtual long GetControlsNum()
    {
        return 0;
    };
    virtual bool GetControlDesc(long code, USER_CONTROL &_user_desc_struct)
    {
        return false;
    };
    virtual bool SetControlFlags(long code, uint32_t flags)
    {
        return true;
    }

    virtual long GetDevicesNum()
    {
        return 0;
    };
    virtual bool GetDeviceDesc(long code, DEVICE_DESC &_device_desc)
    {
        return false;
    };
    virtual long GetDeviceControlsNum(long device_code)
    {
        return 0;
    };
    virtual char *GetDeviceControlName(long device_code, long code)
    {
        return nullptr;
    };

    virtual long AddControlTreeNode(long nParent, const char *pcBaseControl, const char *pcOutControl, float fTimeOut)
    {
        return -1;
    }

    virtual void MapControl(long control_code, long system_control_code){};

    virtual void AppState(bool state){};

    /*    virtual CONTROL_STATE_TYPE GetControlState(char * control_name, float * pvalue = 0){return CST_INACTIVE;};
      virtual CONTROL_STATE_TYPE GetControlState(long control_code, float * pvalue = 0){return CST_INACTIVE;};
      virtual float GetControlValue(char * control_name, CONTROL_STATE_TYPE * pstate = 0){return 0;};
      virtual float GetControlValue(long control_code, CONTROL_STATE_TYPE * pstate = 0){return 0;};
    */
    virtual void SetControlEffect(FFB_EFFECT effect, long time = 0){};

    virtual bool GetControlState(long control_code, CONTROL_STATE &_state_struct)
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

    virtual bool SetControlState(long control_code, CONTROL_STATE &_state_struct)
    {
        memset(&_state_struct, 0, sizeof(_state_struct));
        return false;
    };
    virtual long LastControlTime()
    {
        return 0;
    };

    virtual void SetControlTreshold(long control_code, float thval){};

    virtual void LockControl(const char *control_name, bool mode){};

    virtual void SetMouseSensivityX(float){};

    virtual void SetMouseSensivityY(float){};
    virtual short GetDebugAsyncKeyState(int vk)
    {
        return GetAsyncKeyState(vk);
    }
    virtual short GetDebugKeyState(int vk)
    {
        return GetKeyState(vk);
    }

    // control command from ENGINE
    virtual void EngineMessage(UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
    }

    // Get the keystroke buffer per frame (taking into account the language)
    virtual long GetKeyBufferLength()
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
