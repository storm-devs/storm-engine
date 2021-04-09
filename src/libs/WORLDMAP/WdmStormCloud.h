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
    virtual ~WdmStormCloud();

    // Calculations
    virtual void Update(float dltTime);

    // Rendering
    virtual void PRender(VDX9RENDER *rs);
    virtual void LRender(VDX9RENDER *rs);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    virtual void BuildCloud(long n);
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
