//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmCloud
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "WdmRenderObject.h"

class WdmCloud : public WdmRenderObject
{
    static IDirect3DVertexDeclaration9 *vertexDecl_;
    void CreateVertexDeclaration(VDX9RENDER *rs);

    struct MoveInfo
    {
        CVECTOR pos;
        CVECTOR v;
        CVECTOR cent;
        float rotSpd;
        float kSpd;
        float dx, dz;
        float dTime;
    };

    struct Rain
    {
        uint32_t isLive;
        CVECTOR pos;
        float alpha;
        float vy;
        float size;
        float angle;
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t c;
        float u, v;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmCloud();
    ~WdmCloud() override;

    // Calculations
    void Update(float dltTime) override;
    // Rendering
    void PRender(VDX9RENDER *rs) override;
    void LRender(VDX9RENDER *rs) override;

    virtual void Move(float dltTime);

    long FillRain(RS_RECT *rainRect, long rcnt);

    CVECTOR pos;       // Cloud position
    CVECTOR dir;       // Direction of cloud movement
    float globalAlpha; // Current transparency
    float constAlpha;  // Current transparency

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  protected:
    virtual void BuildCloud(long n);
    RS_RECT rect[8];
    long texture;
    long light;
    long lightning;
    long curMove;
    long numRects;
    MoveInfo move[8];

    // Lightning parameters
    long curLightning;
    float lightningWaitTime;
    float lightningTime;
    float flashTime;
    long lastColor;
    long lightningColor;

    // Rain
    Rain rain[48];
    float rainBurnTime;

  private:
    void Render(VDX9RENDER *rs);
    void FindPartPos(CVECTOR &v);
    float Rnd();
};
