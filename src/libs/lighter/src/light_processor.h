//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LightProcessor
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "l_geometry.h"
#include "lighter_lights.h"
#include "oct_tree.h"
#include "window.h"

class VDX9RENDER;

class LightProcessor
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LightProcessor();
    virtual ~LightProcessor();
    void SetParams(LGeometry *g, Window *win, LighterLights *lit, OctTree *ot, VDX9RENDER *_rs);
    void UpdateLightsParam();

    // Perform Calculation Step
    void Process();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Calculate shading
    void CalcShadows();
    // Smooth shading
    void SmoothShadows();
    // Smooth lighting
    void BlurLight();
    // Calculate lighting
    void CalcLights(int32_t lit = -1, bool isCos = true, bool isAtt = true, bool isSdw = true);
    // Distribute shading from triangle to vertices
    void ApplyTriangleShadows(Triangle &t);

  private:
    LGeometry *geometry;
    Window *window;
    LighterLights *lights;
    VDX9RENDER *rs;
    OctTree *octtree;

    int32_t shadowTriangle;
    int32_t smoothVertex;
    int32_t blurVertex;
};
