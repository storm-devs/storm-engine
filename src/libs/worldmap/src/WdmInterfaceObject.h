//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmInterfaceObject
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmRenderObject.h"

class WdmInterfaceObject : public WdmRenderObject
{
  protected:
    struct Vertex
    {
        float x, y, z, rhw;
        uint32_t color;
        float tu, tv;
        float tu1, tv1;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmInterfaceObject();
    ~WdmInterfaceObject() override;

    // --------------------------------------------------------------------------------------------
    // Interface for Inherited Classes
    // --------------------------------------------------------------------------------------------
  protected:
    // Fill 6 vertices with rectangle coordinates
    static void FillRectCoord(Vertex *vrt, float x, float y, float w, float h, float ang = 0.0f);
    // Fill 6 vertices with rectangle texture coordinates
    static void FillRectUV(Vertex *vrt, float tu, float tv, float tw, float th, float ang = 0.0f);
    // Fill 6 vertices with rectangle texture coordinates
    static void FillRectUV1(Vertex *vrt, float tu, float tv, float tw, float th, float ang = 0.0f);
    // Fill 6 vertices with color
    static void FillRectColor(Vertex *vrt, uint32_t color = 0xffffffff);
    // Draw buffer of rectangles
    static void DrawRects(Vertex *vrt, long numRects, const char *techName = nullptr);

    // Fill 18 vertices of the scaling rectangle
    static void FillSRectCoord(Vertex *vrt, float x, float y, float w, float h, float ws);
    // Fill 18 vertices with the texture coordinates of the scaling rectangle
    static void FillSRectUV(Vertex *vrt, float tu, float tv, float tw, float th, float ts);
    // Fill 18 vertices with a scaled rectangle color
    static void FillSRectColor(Vertex *vrt, uint32_t color = 0xffffffff);

    // Display text in a given strip horizontally and at a given height
    static void Print(long font, long color, float xleft, float xright, float y, const char *format, ...);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  protected:
    static char stringBuffer[1024];

  private:
};
