#pragma once

#include "dx9render.h"
#include "matrix.h"
#include "vma.hpp"
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
    int32_t TextureID;
    uint32_t Color, oldColor;
    float fScale, fOldScale;
    FLOAT_RECT Pos{};
    FONT_SYMBOL CharT[USED_CODES]{};
    int32_t Height;
    int32_t Texture_XSize;
    int32_t Texture_YSize;
    int32_t Symbol_interval;

    bool bShadow, bOldShadow;
    int32_t Shadow_offsetx;
    int32_t Shadow_offsety;

    int32_t Spacebar;

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
    bool MakeLong(char **pDataPointer, int32_t *result);
    int32_t Printf(int32_t x, int32_t y, char *Text, ...);
    int32_t Print(int32_t x, int32_t y, char *Text);
    int32_t UpdateVertexBuffer(int32_t x, int32_t y, char *data_PTR, int utf8length);
    int32_t GetStringWidth(const char *Text);

    int32_t GetHeight()
    {
        return Height;
    };
    void Inverse(bool v);
    void SetShadowOffset(int32_t ox, int32_t oy);
    void GetShadowOffset(int32_t &ox, int32_t &oy);

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
