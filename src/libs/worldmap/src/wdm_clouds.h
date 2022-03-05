//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmClouds
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_render_object.h"

class WdmCloud;

#define WDMCLOUDSMAX 32

class WdmClouds : public WdmRenderObject
{
    static IDirect3DVertexDeclaration9 *vertexDecl_;
    void CreateVertexDeclaration(VDX9RENDER *rs);

    struct Cloud
    {
        struct Cld
        {
            CVECTOR pos;    // Position
            float angle;    // Angle of rotation
            float size;     // The size
            float alpha;    // Transparency
            uint32_t color; // Colour
            uint16_t pict;  // Image index
            uint16_t index; // Pulled-up particle index
            float aspd;     // Swing speed
            CVECTOR dir;    // Current Motion Adjustment Vector
            CVECTOR rdr;    // Additional vector of random gravitation
        };

        Cloud();

        // If the cloud is reborn, returns true
        bool Reset(bool isFirstTime = false);
        // Update cloud state
        void Update(float dltTime);
        // Fill an array of rectangles
        int32_t FillRects(RS_RECT *rects, int32_t cnt, float galpha);
        // Get sphere center and radius
        float GetBound(CVECTOR &_center) const;
        // Run the cloud removal mechanism if there is an intersection
        void Kill(const Cloud &cld);

      private:
        Cld cloud[WDMCLOUDSMAX]; // Positions
        int32_t count;              // amount
        float alpha;             // Overall transparency
        float lifeTime;          // Lifetime
        CVECTOR center;          // Cloud center
        float radius;            // Cloud radius
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmClouds();
    ~WdmClouds() override;

    // Calculations
    void Update(float dltTime) override;
    // Drawing
    void LRender(VDX9RENDER *rs) override;

    int32_t texture, light;
    Cloud clouds[16];
    RS_RECT rects[WDMCLOUDSMAX * 16];
};
