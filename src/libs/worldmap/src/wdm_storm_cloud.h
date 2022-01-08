//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmStormCloud
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_cloud.h"

class WdmStormCloud : public WdmCloud
{
    struct RainVertex
    {
        float x, y, z;
        float tu, tv;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmStormCloud();
    ~WdmStormCloud() override;

    // Calculations
    void Update(float dltTime) override;

    // Rendering
    void PRender(VDX9RENDER *rs) override;
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void BuildCloud(int32_t n) override;
    void FillRects();

    // Lightning parameters
    int32_t curLightning;
    float lightningWaitTime;
    float lightningTime;
    float flashTime;
    int32_t lastColor;
    int32_t lightningColor;

    // Rain
    int32_t rainTexture;
    float curU, curV;
    CVECTOR rainpos[2048];        // Particle position
    static RainVertex rain[4096]; // The current array to draw
};
