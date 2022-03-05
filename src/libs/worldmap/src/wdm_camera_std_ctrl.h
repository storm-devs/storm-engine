//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmCameraStdCtrl
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_camera.h"

class WdmCameraStdCtrl : public WdmCamera
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmCameraStdCtrl();
    ~WdmCameraStdCtrl() override;

    // --------------------------------------------------------------------------------------------
    // Functions that return key reactions
    // --------------------------------------------------------------------------------------------
  public:
    void CtrlProcess(float dltTime) override;
    float MoveLeftRight(float dltTime) override;
    float MoveUpDown(float dltTime) override;
    float RotLeftRight(float dltTime) override;
    float ZoomInOut(float dltTime) override;
    bool CurrentFreeMode() override;
    bool GetHightHeight(float &height) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    bool GetCurFreeMode() const;

  private:
    bool isFree;
    float mdx, mdy, mzoom;
    bool lastFreeMode;
};
