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
#include "bi_utils.h"

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
    int32_t frameTx;
    float frameLeftPos, frameTopPos, frameWidth, frameHeight;
    uint32_t frameColor;

    int32_t skyTx, skyMaskTx;
    float skyLeftPos, skyTopPos, skyWidth, skyHeight;
    uint32_t skyColor;

    int32_t windBarTx, windBarMaskTx;
    float windBarLeftPos, windBarTopPos, windBarWidth, windBarHeight;
    uint32_t windBarColor;

    int32_t windPointerTx;
    float windPointerLeftPos, windPointerTopPos, windPointerWidth, windPointerHeight;
    uint32_t windPointerColor;

    int32_t moraleTx, moraleMaskTx, moraleBarTx;
    float moraleLeftPos, moraleTopPos, moraleWidth, moraleHeight;
    uint32_t moraleColor;

    int32_t dateFont;
    float morale{};
    int32_t food{};
    std::optional<int32_t> rum;
    char month[12][128];

    int32_t coordTx;
    float coordLeftPos, coordTopPos, coordWidth, coordHeight;
    uint32_t coordColor;

    BITextInfo dateText;    // font, scale and position for Date text in UI
    BITextInfo foodText;    // font, scale and position for Food text in UI
    BITextInfo rumText;     // font, scale and position for Rum text in UI
    BITextInfo stCoordText; // same for label/title above coordinates
    BITextInfo coordText;   // same for coordinates text

    int32_t nationFlagTx;
    uint32_t nationFlagCount{8U};
    float nationFlagLeftPos, nationFlagTopPos, nationFlagWidth, nationFlagHeight;
    uint32_t nationFlagColor;

    float resizeRatio;
};
