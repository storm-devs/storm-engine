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

#include "WdmCloud.h"

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
    void BuildCloud(long n) override;
    void FillRects();

    // Lightning parameters
    long curLightning;
    float lightningWaitTime;
    float lightningTime;
    float flashTime;
    long lastColor;
    long lightningColor;

    // Rain
    long rainTexture;
    float curU, curV;
    CVECTOR rainpos[2048];        // Particle position
    static RainVertex rain[4096]; // The current array to draw
};
