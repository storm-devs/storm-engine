//============================================================================================
//    Spirenkov Maxim, 2005
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindUI
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_interface_object.h"

#include <optional>

class WdmWindUI : public WdmInterfaceObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmWindUI();
    ~WdmWindUI() override;

    void SetAttributes(ATTRIBUTES *apnt);
    // Rendering
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    int32_t txBack;
    int32_t txSky, txSkyMask;
    int32_t txBar, txBarMask;
    int32_t txWindPointer;
    int32_t txMorale, txMoraleMask, txMoraleBar;
    int32_t dateFont;
    float morale{};
    int32_t food{};
    std::optional<int32_t> rum;
    char month[12][128];
    int32_t txCoord;

    int32_t nationFlagTx;
    uint32_t nationFlagCount{8U};
    float nationFlagWidth{48.0f};
    float nationFlagHeight{48.0f};

    float resizeRatio;
};
