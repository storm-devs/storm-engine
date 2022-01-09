#pragma once

#include "c_vector.h"
#include "quest_file_reader/quest_file_reader.h"
#include "dx9render.h"
#include "shared/interface/messages.h"
#include "string_service.h"
#include "vx_service.h"
#include <entity_manager.h>

#define ALPHA(x) ((x) >> 24)
#define RED(x) (((x) >> 16) & 0xFF)
#define GREEN(x) (((x) >> 8) & 0xFF)
#define BLUE(x) ((x)&0xFF)
//#define ARGB(a,r,g,b)    (((a)<<24)|((r)<<16)|((g)<<8)|b)

struct COMMANDDESCR
{
    const char *sName;
    int code;
};

extern COMMANDDESCR pCommandsList[COMMAND_QUANTITY];
extern int FindCommand(const char *comName);
extern int FindCommand(int comID);

struct SAVE_DATA_HANDLE
{
    int32_t StringDataSize;
    int32_t SurfaceDataSize;
};

struct XYPOINT
{
    int32_t x, y;

    XYPOINT()
    {
        x = y = 0;
    }

    XYPOINT(int32_t ix, int32_t iy)
    {
        x = ix;
        y = iy;
    }

    void operator=(const XYPOINT &lp)
    {
        this->x = lp.x;
        this->y = lp.y;
    }
};

struct XYRECT
{
    int32_t left, top, right, bottom;

    XYRECT()
    {
        left = top = right = bottom = 0;
    }

    XYRECT(int32_t ileft, int32_t itop, int32_t iright, int32_t ibottom)
    {
        left = ileft;
        top = itop;
        right = iright;
        bottom = ibottom;
    }

    void operator=(const XYRECT &ir)
    {
        this->left = ir.left;
        this->top = ir.top;
        this->right = ir.right;
        this->bottom = ir.bottom;
    }
};

struct FXYPOINT
{
    float x, y;

    FXYPOINT()
    {
        x = y = 0.f;
    }

    FXYPOINT(const FXYPOINT &fp)
    {
        x = fp.x;
        y = fp.y;
    }

    FXYPOINT(float fx, float fy)
    {
        this->x = fx;
        this->y = fy;
    }

    FXYPOINT &operator=(const FXYPOINT &fp)
    {
        this->x = fp.x;
        this->y = fp.y;
        return (*this);
    }

    FXYPOINT operator+(FXYPOINT &fp)
    {
        FXYPOINT np;
        np.x = this->x + fp.x;
        np.y = this->y + fp.y;
        return np;
    }

    FXYPOINT operator-(FXYPOINT &fp)
    {
        FXYPOINT np;
        np.x = this->x - fp.x;
        np.y = this->y - fp.y;
        return np;
    }
};

struct FXYRECT
{
    float left, top, right, bottom;

    FXYRECT()
    {
        left = top = right = bottom = 0.f;
    }

    FXYRECT(float fleft, float ftop, float fright, float fbottom)
    {
        left = fleft;
        top = ftop;
        right = fright;
        bottom = fbottom;
    }

    void operator=(const FXYRECT &fr)
    {
        this->left = fr.left;
        this->top = fr.top;
        this->right = fr.right;
        this->bottom = fr.bottom;
    }

    void operator+=(const FXYRECT &fr)
    {
        this->left += fr.left;
        this->top += fr.top;
        this->right += fr.right;
        this->bottom += fr.bottom;
    }

    void operator-=(const FXYRECT &fr)
    {
        this->left -= fr.left;
        this->top -= fr.top;
        this->right -= fr.right;
        this->bottom -= fr.bottom;
    }

    FXYRECT operator+(const FXYRECT &fr)
    {
        FXYRECT tmp;
        tmp.left = this->left + fr.left;
        tmp.top = this->top + fr.top;
        tmp.right = this->right + fr.right;
        tmp.bottom = this->bottom + fr.bottom;
        return tmp;
    }

    FXYRECT operator-(const FXYRECT &fr)
    {
        FXYRECT tmp;
        tmp.left = this->left - fr.left;
        tmp.top = this->top - fr.top;
        tmp.right = this->right - fr.right;
        tmp.bottom = this->bottom - fr.bottom;
        return tmp;
    }

    FXYRECT operator+(const FXYPOINT &fp)
    {
        FXYRECT tmp;
        tmp.left = this->left + fp.x;
        tmp.top = this->top + fp.y;
        tmp.right = this->right + fp.x;
        tmp.bottom = this->bottom + fp.y;
        return tmp;
    }

    FXYRECT operator-(const FXYPOINT &fp)
    {
        FXYRECT tmp;
        tmp.left = this->left - fp.x;
        tmp.top = this->top - fp.y;
        tmp.right = this->right - fp.x;
        tmp.bottom = this->bottom - fp.y;
        return tmp;
    }
};

// vertex formates
#define XI_NOTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0)

struct XI_NOTEX_VERTEX
{
    CVECTOR pos;
    uint32_t color;
};

#define XI_ONLYONETEX_FVF (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct XI_ONLYONETEX_VERTEX
{
    CVECTOR pos;
    float tu, tv;
};

#define XI_ONETEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct XI_ONETEX_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;
};

#define XI_TWOTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2)

struct XI_TWOTEX_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu1, tv1;
    float tu2, tv2;
};

#define XI_THREETEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXTUREFORMAT2)

struct XI_THREETEX_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu1, tv1;
    float tu2, tv2;
    float tu3, tv3;
};

inline void PICTURE_TEXTURE_RELEASE(VXSERVICE *ps, const char *gn, int32_t &tex)
{
    if (tex != -1 && ps != nullptr)
    {
        ps->ReleaseTextureID(gn);
        tex = -1;
    }
}

inline void VIDEOTEXTURE_RELEASE(VDX9RENDER *rs, CVideoTexture *tex)
{
    if (rs != nullptr && tex != nullptr)
    {
        rs->ReleaseVideoTexture(tex);
        tex = nullptr;
    }
}

extern entid_t g_idInterface;

extern uint32_t ColorInterpolate(uint32_t sCol, uint32_t dCol, float m);
extern void DublicateString(char *&pDstStr, const char *pSrcStr);
