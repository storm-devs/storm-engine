//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmInterfaceObject
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include <stdio.h>

#include "WdmInterfaceObject.h"
#include "WdmObjects.h"

//============================================================================================

char WdmInterfaceObject::stringBuffer[1024];

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmInterfaceObject::WdmInterfaceObject()
{
}

WdmInterfaceObject::~WdmInterfaceObject()
{
}

// Fill 6 vertices with rectangle coordinates
void WdmInterfaceObject::FillRectCoord(Vertex *vrt, float x, float y, float w, float h, float ang)
{
    if (!vrt)
        return;
    if (ang == 0.0f)
    {
        // No turning
        vrt[0].x = x;
        vrt[0].y = y;
        vrt[0].z = 0.0f;
        vrt[0].rhw = 0.5f;
        vrt[1].x = x + w;
        vrt[1].y = y;
        vrt[1].z = 0.0f;
        vrt[1].rhw = 0.5f;
        vrt[2].x = x;
        vrt[2].y = y + h;
        vrt[2].z = 0.0f;
        vrt[2].rhw = 0.5f;
        vrt[3].x = x + w;
        vrt[3].y = y;
        vrt[3].z = 0.0f;
        vrt[3].rhw = 0.5f;
        vrt[4].x = x + w;
        vrt[4].y = y + h;
        vrt[4].z = 0.0f;
        vrt[4].rhw = 0.5f;
        vrt[5].x = x;
        vrt[5].y = y + h;
        vrt[5].z = 0.0f;
        vrt[5].rhw = 0.5f;
    }
    else
    {
        // With a turn
        const auto sn = -0.5f * sinf(ang);
        const auto cs = -0.5f * cosf(ang);
        const auto dwx = cs * w;
        const auto dwy = sn * w;
        const auto dhx = -sn * h;
        const auto dhy = cs * h;
        x += 0.5f * w;
        y += 0.5f * h;
        vrt[0].x = x - dwx + dhx;
        vrt[0].y = y - dwy + dhy;
        vrt[0].z = 0.0f;
        vrt[0].rhw = 0.5f;
        vrt[1].x = x + dwx + dhx;
        vrt[1].y = y + dwy + dhy;
        vrt[1].z = 0.0f;
        vrt[1].rhw = 0.5f;
        vrt[2].x = x - dwx - dhx;
        vrt[2].y = y - dwy - dhy;
        vrt[2].z = 0.0f;
        vrt[2].rhw = 0.5f;
        vrt[3].x = x + dwx + dhx;
        vrt[3].y = y + dwy + dhy;
        vrt[3].z = 0.0f;
        vrt[3].rhw = 0.5f;
        vrt[4].x = x + dwx - dhx;
        vrt[4].y = y + dwy - dhy;
        vrt[4].z = 0.0f;
        vrt[4].rhw = 0.5f;
        vrt[5].x = x - dwx - dhx;
        vrt[5].y = y - dwy - dhy;
        vrt[5].z = 0.0f;
        vrt[5].rhw = 0.5f;
    }
}

// Fill 6 vertices with rectangle texture coordinates
void WdmInterfaceObject::FillRectUV(Vertex *vrt, float tu, float tv, float tw, float th, float ang)
{
    if (!vrt)
        return;
    if (ang == 0.0f)
    {
        vrt[0].tu = tu;
        vrt[0].tv = tv;
        vrt[1].tu = tu + tw;
        vrt[1].tv = tv;
        vrt[2].tu = tu;
        vrt[2].tv = tv + th;
        vrt[3].tu = tu + tw;
        vrt[3].tv = tv;
        vrt[4].tu = tu + tw;
        vrt[4].tv = tv + th;
        vrt[5].tu = tu;
        vrt[5].tv = tv + th;
    }
    else
    {
        // With a turn
        const auto sn = 0.5f * sinf(ang);
        const auto cs = 0.5f * cosf(ang);
        const auto dwu = cs * tw;
        const auto dwv = sn * tw;
        const auto dhu = -sn * th;
        const auto dhv = cs * th;
        tu += 0.5f * tw;
        tv += 0.5f * th;
        vrt[0].tu = tu - dwu + dhu;
        vrt[0].tv = tv - dwv + dhv;
        vrt[1].tu = tu + dwu + dhu;
        vrt[1].tv = tv + dwv + dhv;
        vrt[2].tu = tu - dwu - dhu;
        vrt[2].tv = tv - dwv - dhv;
        vrt[3].tu = tu + dwu + dhu;
        vrt[3].tv = tv + dwv + dhv;
        vrt[4].tu = tu + dwu - dhu;
        vrt[4].tv = tv + dwv - dhv;
        vrt[5].tu = tu - dwu - dhu;
        vrt[5].tv = tv - dwv - dhv;
    }
}

// Fill 6 vertices with rectangle texture coordinates
void WdmInterfaceObject::FillRectUV1(Vertex *vrt, float tu, float tv, float tw, float th, float ang)
{
    if (!vrt)
        return;
    if (ang == 0.0f)
    {
        vrt[0].tu1 = tu;
        vrt[0].tv1 = tv;
        vrt[1].tu1 = tu + tw;
        vrt[1].tv1 = tv;
        vrt[2].tu1 = tu;
        vrt[2].tv1 = tv + th;
        vrt[3].tu1 = tu + tw;
        vrt[3].tv1 = tv;
        vrt[4].tu1 = tu + tw;
        vrt[4].tv1 = tv + th;
        vrt[5].tu1 = tu;
        vrt[5].tv1 = tv + th;
    }
    else
    {
        // With a turn
        const auto sn = 0.5f * sinf(ang);
        const auto cs = 0.5f * cosf(ang);
        const auto dwu = cs * tw;
        const auto dwv = sn * tw;
        const auto dhu = -sn * th;
        const auto dhv = cs * th;
        tu += 0.5f * tw;
        tv += 0.5f * th;
        vrt[0].tu1 = tu - dwu + dhu;
        vrt[0].tv1 = tv - dwv + dhv;
        vrt[1].tu1 = tu + dwu + dhu;
        vrt[1].tv1 = tv + dwv + dhv;
        vrt[2].tu1 = tu - dwu - dhu;
        vrt[2].tv1 = tv - dwv - dhv;
        vrt[3].tu1 = tu + dwu + dhu;
        vrt[3].tv1 = tv + dwv + dhv;
        vrt[4].tu1 = tu + dwu - dhu;
        vrt[4].tv1 = tv + dwv - dhv;
        vrt[5].tu1 = tu - dwu - dhu;
        vrt[5].tv1 = tv - dwv - dhv;
    }
}

// Fill 6 vertices with color
void WdmInterfaceObject::FillRectColor(Vertex *vrt, uint32_t color)
{
    if (!vrt)
        return;
    vrt[0].color = color;
    vrt[1].color = color;
    vrt[2].color = color;
    vrt[3].color = color;
    vrt[4].color = color;
    vrt[5].color = color;
}

// Draw buffer of rectangles
void WdmInterfaceObject::DrawRects(Vertex *vrt, long numRects, const char *techName)
{
    wdmObjects->rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2, numRects * 2, vrt,
                                    sizeof(Vertex), techName);
}

// Fill 18 vertices of the scaling rectangle
void WdmInterfaceObject::FillSRectCoord(Vertex *vrt, float x, float y, float w, float h, float ws)
{
    FillRectCoord(vrt, x, y, ws, h);
    FillRectCoord(vrt + 6, x + ws, y, w - 2.0f * ws, h);
    FillRectCoord(vrt + 12, x + w - ws, y, ws, h);
}

// Fill 18 vertices with the texture coordinates of the scaling rectangle
void WdmInterfaceObject::FillSRectUV(Vertex *vrt, float tu, float tv, float tw, float th, float ts)
{
    FillRectUV(vrt, tu, tv, ts, th);
    FillRectUV(vrt + 6, tu + ts, tv, tw - 2.0f * ts, th);
    FillRectUV(vrt + 12, tu + tw - ts, tv, ts, th);
}

// Fill 18 vertices with a scaled rectangle color
void WdmInterfaceObject::FillSRectColor(Vertex *vrt, uint32_t color)
{
    FillRectColor(vrt, color);
    FillRectColor(vrt + 6, color);
    FillRectColor(vrt + 12, color);
}

// Display text in a given strip horizontally and at a given height
void WdmInterfaceObject::Print(long font, long color, float xleft, float xright, float y, const char *format, ...)
{
    _vsnprintf_s(stringBuffer, sizeof(stringBuffer), format, ((char *)&format + sizeof(char *)));
    const auto strw = wdmObjects->rs->StringWidth(stringBuffer, font);
    const auto x = (xright + xleft - strw) * 0.5f;
    wdmObjects->rs->Print(font, color, static_cast<long>(x), static_cast<long>(y), stringBuffer);
}
