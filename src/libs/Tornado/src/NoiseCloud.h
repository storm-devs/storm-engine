//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    NoiseCloud
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "Pillar.h"
#include "dx9render.h"

class NoiseCloud
{
    struct Rect
    {
        float r, a;   // Radial position
        float angle;  // Swing angle
        float size;   // The size
        float alpha;  // Overall transparency
        float light;  // Illumination
        float kLight; // Illumination value during backlighting
        float tLight; // Time since last skylight
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    NoiseCloud(Pillar &_pillar);
    virtual ~NoiseCloud();

    void Update(float dltTime);
    void Draw(VDX9RENDER *rs);

    void SetGlobalAlpha(float a);

    long texture;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    Pillar &pillar;
    float galpha;
    long lightCnt;
    Rect rect[64];
    Vertex buf[6 * 64];
};
