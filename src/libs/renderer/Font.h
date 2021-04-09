#pragma once

#include "dx9render.h"
#include "matrix.h"
#include "vmodule_api.h"
//#include "iimage.h"

#define USED_CODES 0x2070 // end of https://unicode-table.com/en/blocks/general-punctuation/
#define MAX_SYMBOLS 512
#define SYM_VERTEXS 6

#ifndef IMAGE_VERTEX_DEF
#define IMAGE_VERTEX_DEF
struct IMAGE_VERTEX
{
    CVECTOR pos;
    float rhw;
    uint32_t color;
    float tu, tv;
};
#endif

#define IMAGE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

#ifndef FLOAT_RECT_DEF
#define FLOAT_RECT_DEF

struct FLOAT_RECT
{
    float x1, y1, x2, y2;
};
#endif

#ifndef FONT_SYMBOL_DEF
#define FONT_SYMBOL_DEF
struct FONT_SYMBOL
{
    FLOAT_RECT Pos;
    FLOAT_RECT Tuv;
};
#endif

class FONT
{
    VDX9RENDER *RenderService;
    IDirect3DDevice9 *Device;
    IDirect3DVertexBuffer9 *VBuffer;
    char *techniqueName;
    char *textureName;
    long TextureID;
    uint32_t Color, oldColor;
    float fScale, fOldScale;
    FLOAT_RECT Pos;
    FONT_SYMBOL CharT[USED_CODES];
    long Height;
    long Texture_XSize;
    long Texture_YSize;
    long Symbol_interval;

    bool bShadow, bOldShadow;
    long Shadow_offsetx;
    long Shadow_offsety;

    long Spacebar;

    bool bInverse, bOldInverse;

    float m_fAspectRatioH;
    float m_fAspectRatioV;

  public:
    FONT();
    ~FONT();
    bool Init(const char *font_name, const char *iniName, IDirect3DDevice9 *_device, VDX9RENDER *_render);
    void TempUnload();
    void RepeatInit();
    void Realize(uint32_t DeltaTime);
    void SetColor(uint32_t color);
    void SetScale(float scale);
    void SetShadow(bool s);
    void SetCenter(bool c);
    bool MakeLong(char **pDataPointer, long *result);
    long Printf(long x, long y, char *Text, ...);
    long Print(long x, long y, char *Text);
    long UpdateVertexBuffer(long x, long y, char *data_PTR, int utf8length);
    long GetStringWidth(const char *Text);
    long GetHeight()
    {
        return Height;
    };
    void Inverse(bool v);
    void SetShadowOffset(long ox, long oy);
    void GetShadowOffset(long &ox, long &oy);
    void StoreFontParameters()
    {
        oldColor = Color;
        fOldScale = fScale;
        bOldShadow = bShadow;
        bOldInverse = bInverse;
    };
    void RestoreFontParameters()
    {
        Color = oldColor;
        fScale = fOldScale;
        bShadow = bOldShadow;
        bInverse = bOldInverse;
    };
};
