//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lights
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "LTypes.h"
#include "storm_assert.h"

class LighterLights
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LighterLights();
    virtual ~LighterLights();

    void AddAmbient(const CVECTOR &color);
    void AddWeaterLights(const CVECTOR &color, const CVECTOR &dir);
    void AddPointLight(const CVECTOR &color, const CVECTOR &pos, float att0, float att1, float att2, float range,
                       const char *group);
    void PostInit();
    void UpdateLights(long lit);

    long Num() const;
    Light &operator[](long i);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void SetDefLightParam(long i);

  private:
    std::vector<Light> light;
    long numLights;
    long maxLights;
};

inline long LighterLights::Num() const
{
    return numLights;
}

inline Light &LighterLights::operator[](long i)
{
    Assert(i >= 0 && i < numLights);
    return light[i];
}
