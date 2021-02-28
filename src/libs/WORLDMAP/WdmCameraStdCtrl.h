//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	WdmCameraStdCtrl
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmCameraStdCtrl_H_
#define _WdmCameraStdCtrl_H_

#include "WdmCamera.h"

class WdmCameraStdCtrl : public WdmCamera
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmCameraStdCtrl();
    virtual ~WdmCameraStdCtrl();

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

#endif
