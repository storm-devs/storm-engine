//============================================================================================
//    Spirenkov Maxim, 2005
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindUI
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmWindUI_H_
#define _WdmWindUI_H_

#include "WdmInterfaceObject.h"

class WdmWindUI : public WdmInterfaceObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmWindUI();
    virtual ~WdmWindUI();

    void SetAttributes(ATTRIBUTES *apnt);
    // Rendering
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    long txBack;
    long txSky, txSkyMask;
    long txBar, txBarMask;
    long txWindPointer;
    long txMorale, txMoraleMask, txMoraleBar;
    long dateFont;
    float morale;
    long food;
    long rum;
    char month[12][128];
    long txCoord;
    long txNationFlag;
    float resizeRatio;
};

#endif
