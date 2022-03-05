//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmCameraStdCtrl
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_camera_std_ctrl.h"

#include "controls.h"
#include "core.h"

#include "v_module_api.h"

#define WDM_CAMERASTDCTRL_MAXDLT 400

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmCameraStdCtrl::WdmCameraStdCtrl()
{
    lastFreeMode = GetCurFreeMode();
    mdx = 0.0f;
    mdy = 0.0f;
    mzoom = 0.0f;
    isFree = false;
}

WdmCameraStdCtrl::~WdmCameraStdCtrl()
{
}

void WdmCameraStdCtrl::CtrlProcess(float dltTime)
{
    // Is the camera free?
    isFree = GetCurFreeMode();
    // Orientation
    CONTROL_STATE cs;
    core.Controls->GetControlState("WMapTurnH", cs);
    auto dx = cs.lValue * 4.0f;
    core.Controls->GetControlState("WMapTurnV", cs);
    auto dy = cs.lValue * 4.0f;
    // Calculations
    auto k = (isFree ? 10.0f : 5.0f) * dltTime;
    if (k > 1.0f)
        k = 1.0f;
    if (dx > WDM_CAMERASTDCTRL_MAXDLT)
        dx = WDM_CAMERASTDCTRL_MAXDLT;
    if (dx < -WDM_CAMERASTDCTRL_MAXDLT)
        dx = -WDM_CAMERASTDCTRL_MAXDLT;
    if (dy > WDM_CAMERASTDCTRL_MAXDLT)
        dy = WDM_CAMERASTDCTRL_MAXDLT;
    if (dy < -WDM_CAMERASTDCTRL_MAXDLT)
        dy = -WDM_CAMERASTDCTRL_MAXDLT;
    mdx += (dx - mdx) * k;
    mdy += (dy - mdy) * k;
    if (isFree != lastFreeMode)
    {
        mdx = 0.0f;
        mdy = 0.0f;
        lastFreeMode = isFree;
    }
}

float WdmCameraStdCtrl::MoveLeftRight(float dltTime)
{
    if (isFree)
    {
        CONTROL_STATE cs;
        core.Controls->GetControlState("WMapCameraRotate", cs);
        if (cs.state != CST_ACTIVE)
            return -mdx * 0.2f * dltTime;
    }
    return 0.0f;
}

float WdmCameraStdCtrl::MoveUpDown(float dltTime)
{
    if (isFree)
    {
        CONTROL_STATE cs;
        core.Controls->GetControlState("WMapCameraRotate", cs);
        if (cs.state != CST_ACTIVE)
            return mdy * 0.2f * dltTime;
    }
    return 0.0f;
}

float WdmCameraStdCtrl::RotLeftRight(float dltTime)
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("WMapCameraRotate", cs);
    if (isFree && cs.state != CST_ACTIVE)
        return 0.0f;
    return mdx * 0.06f * dltTime;
}

float WdmCameraStdCtrl::ZoomInOut(float dltTime)
{
    float h;
    if (GetHightHeight(h))
        return 0.0f;
    auto f = 0.0f;
    CONTROL_STATE cs;
    core.Controls->GetControlState("WMapForward", cs);
    if (cs.lValue != 0)
        f += dltTime * cs.fValue;
    core.Controls->GetControlState("WMapBackward", cs);
    if (cs.lValue != 0)
        f -= dltTime * cs.fValue;
    auto k = 12.0f * dltTime;
    if (k > 1.0f)
        k = 1.0f;
    mzoom += (f - mzoom) * k;
    return mzoom * 4.0f;
}

bool WdmCameraStdCtrl::CurrentFreeMode()
{
    return isFree;
}

bool WdmCameraStdCtrl::GetCurFreeMode() const
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("WMapCameraSwitch", cs);
    // if(wdmObjects->isDebug)
    {
        if (cs.state == CST_ACTIVATED)
            return !isFree;
        return isFree;
    }
    return cs.state == CST_ACTIVE;
}

bool WdmCameraStdCtrl::GetHightHeight(float &height)
{
    CONTROL_STATE cs;
    height = 500.0f;

    return isFree;

    //~!~!!!
    core.Controls->GetControlState("WMapCameraSwitch", cs);
    return cs.state == CST_ACTIVE;

    core.Controls->GetControlState("WMapCameraShift", cs);
    return cs.state == CST_ACTIVE;
}
