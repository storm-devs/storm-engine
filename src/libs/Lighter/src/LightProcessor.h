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

#include "LGeometry.h"
#include "LighterLights.h"
#include "OctTree.h"
#include "Window.h"

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
    void CalcLights(long lit = -1, bool isCos = true, bool isAtt = true, bool isSdw = true);
    // Distribute shading from triangle to vertices
    void ApplyTriangleShadows(Triangle &t);

  private:
    LGeometry *geometry;
    Window *window;
    LighterLights *lights;
    VDX9RENDER *rs;
    OctTree *octtree;

    long shadowTriangle;
    long smoothVertex;
    long blurVertex;
};
