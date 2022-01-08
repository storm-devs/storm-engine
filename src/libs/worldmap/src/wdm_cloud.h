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

#include "matrix.h"
#include "wdm_render_object.h"

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

    int32_t FillRain(RS_RECT *rainRect, int32_t rcnt);

    CVECTOR pos;       // Cloud position
    CVECTOR dir;       // Direction of cloud movement
    float globalAlpha; // Current transparency
    float constAlpha;  // Current transparency

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  protected:
    virtual void BuildCloud(int32_t n);
    RS_RECT rect[8];
    int32_t texture;
    int32_t light;
    int32_t lightning;
    int32_t curMove;
    int32_t numRects;
    MoveInfo move[8];

    // Lightning parameters
    int32_t curLightning;
    float lightningWaitTime;
    float lightningTime;
    float flashTime;
    int32_t lastColor;
    int32_t lightningColor;

    // Rain
    Rain rain[48];
    float rainBurnTime;

  private:
    void Render(VDX9RENDER *rs);
    void FindPartPos(CVECTOR &v);
    float Rnd();
};
