#pragma once

#include "defines.h"
#include "dx9render.h"

#define BI_IMAGE_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct BI_IMAGE_VERTEX
{
    CVECTOR pos;
    float w;
    uint32_t col;
    float tu, tv;
};

enum BIImageType
{
    BIType_square,
    BIType_clocksquare,

    BIType_ForceDword = 0xFFFFFFFF
};

class IBIImage
{
  protected:
    IBIImage()
    {
    }

  public:
    virtual ~IBIImage()
    {
    }

    virtual void SetColor(uint32_t color) = 0;
    virtual void SetPosition(long nLeft, long nTop, long nRight, long nBottom) = 0;
    virtual void Set3DPosition(const CVECTOR &vPos, float fWidth, float fHeight) = 0;
    virtual void SetUV(const FRECT &uv) = 0;

    virtual void CutSide(float fleft, float fright, float ftop, float fbottom) = 0;
    virtual void CutClock(float fBegin, float fEnd, float fFactor) = 0;
};

class IBIString
{
  protected:
    IBIString()
    {
    }

  public:
    virtual ~IBIString()
    {
    }

    virtual void SetColor(uint32_t color) = 0;
    virtual void SetScale(float fScale) = 0;
    virtual void SetFont(const char *pcFontName) = 0;
    virtual void SetAlign(long nHorzAlign, long nVertAlign) = 0;
    virtual void SetPosition(long nLeft, long nTop, long nRight, long nBottom) = 0;
    virtual void SetString(const char *pcStr) = 0;
};

#define ImagePrioritet_MaxValue 100000
#define ImagePrioritet_DefaultValue 10000
