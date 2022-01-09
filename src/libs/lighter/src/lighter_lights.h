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

#include "l_types.h"
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
    void UpdateLights(int32_t lit);

    int32_t Num() const;
    Light &operator[](int32_t i);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void SetDefLightParam(int32_t i);

  private:
    std::vector<Light> light;
    int32_t numLights;
    int32_t maxLights;
};

inline int32_t LighterLights::Num() const
{
    return numLights;
}

inline Light &LighterLights::operator[](int32_t i)
{
    Assert(i >= 0 && i < numLights);
    return light[i];
}
