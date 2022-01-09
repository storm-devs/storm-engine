#pragma once

#include "defines.h"
#include "dx9render.h"
#include "storm_assert.h"
#include "v_data.h"

#define INI_FILE_NAME "resource\\ini\\interfaces\\battle_interface.ini"

#define FULLRECT(r)                                                                                                    \
    {                                                                                                                  \
        r.left = 0.f;                                                                                                  \
        r.top = 0.f;                                                                                                   \
        r.right = 1.f;                                                                                                 \
        r.bottom = 1.f;                                                                                                \
    }
#define ZERROPOINT(p)                                                                                                  \
    {                                                                                                                  \
        p.x = 0.f;                                                                                                     \
        p.y = 0.f;                                                                                                     \
    }
#define FILLPOINT(p, _x, _y)                                                                                           \
    {                                                                                                                  \
        p.x = _x;                                                                                                      \
        p.y = _y;                                                                                                      \
    }

// icons control defines
#define BI_MSG_COMMAND_ACTIVATE 1
#define BI_MSG_COMMAND_RIGHT 2
#define BI_MSG_COMMAND_LEFT 3
#define BI_MSG_COMMAND_UP 4
#define BI_MSG_COMMAND_DOWN 5
#define BI_MSG_COMMAND_DEACTIVATE 6

#define BI_COMMANDS_ACTIVATE_LAND "LICommandsActivate"
#define BI_COMMANDS_ACTIVATE_SEA "BICommandsActivate"
#define BI_COMMANDS_CONFIRM "BICommandsConfirm"
#define BI_COMMANDS_RIGHTSTEP "BICommandsRight"
#define BI_COMMANDS_LEFTSTEP "BICommandsLeft"
#define BI_COMMANDS_UPSTEP "BICommandsUp"
#define BI_COMMANDS_DOWNSTEP "BICommandsDown"
#define BI_COMMANDS_CANCEL "BICommandsCancel"

#define BI_ONETEX_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct BI_ONETEXTURE_VERTEX
{
    CVECTOR pos;
    float w;
    float tu, tv;
};

struct BIFPOINT
{
    float x, y;

    BIFPOINT()
    {
    }

    BIFPOINT(const BIFPOINT &bp)
    {
        x = bp.x;
        y = bp.y;
    }

    const BIFPOINT &operator+=(const BIFPOINT &bp)
    {
        x += bp.x;
        y += bp.y;
        return *this;
    }

    const BIFPOINT &operator-=(const BIFPOINT &bp)
    {
        x -= bp.x;
        y -= bp.y;
        return *this;
    }

    friend BIFPOINT operator+(const BIFPOINT &p1, const BIFPOINT &p2)
    {
        BIFPOINT bp(p1);
        bp += p2;
        return bp;
    }

    friend BIFPOINT operator+(const FPOINT &p1, const BIFPOINT &p2)
    {
        BIFPOINT bp(p2);
        bp.x += p1.x;
        bp.y += p1.y;
        return bp;
    }

    friend BIFPOINT operator+(const BIFPOINT &p1, const FPOINT &p2)
    {
        BIFPOINT bp(p1);
        bp.x += p2.x;
        bp.y += p2.y;
        return bp;
    }

    // FPOINT operator FPOINT() {return *(FPOINT*)this;}
};

#define BI_COLOR_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct BI_COLOR_VERTEX
{
    CVECTOR pos;
    float w;
    uint32_t col;
    float tu, tv;
};

#define BI_NOTEX_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_TEX0)

struct BI_NOTEXTURE_VERTEX
{
    CVECTOR pos;
    float w;
};

#define BI_COLORONLY_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX0)

struct BI_COLORONLY_VERTEX
{
    CVECTOR pos;
    float w;
    uint32_t col;
};

#define BI_3D_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct BI_3D_VERTEX
{
    CVECTOR pos;
    float tu, tv;
};

// inline function

inline float GetBetwinFloat(float beg, float end, float ratio)
{
    return beg + ratio * (end - beg);
}

inline FRECT &GetTexRectFromPosition(FRECT &fr, int posNum, int hq, int vq)
{
    if (posNum < 0)
        posNum = 0;
    int py = posNum / hq;
    int px = posNum - py * hq;
    fr.left = (float)px / hq;
    fr.top = (float)py / vq;
    fr.right = fr.left + 1.f / hq;
    fr.bottom = fr.top + 1.f / vq;
    return fr;
}

inline FRECT &GetTexRectFromPosition(FRECT &fr, int posNum, int hq, int vq, bool bSelected)
{
    int py = posNum / hq;
    int px = posNum - py * hq;
    fr.left = .5f * (float)px / (float)hq;
    fr.top = (float)py / (float)vq;
    fr.right = fr.left + .5f / hq;
    fr.bottom = fr.top + 1.f / vq;
    if (bSelected)
    {
        fr.left += .5f;
        fr.right += .5f;
    }
    return fr;
}

inline void SetRectanglePos(BI_ONETEXTURE_VERTEX *pv, FRECT &tr)
{
    Assert(pv != NULL);
    pv[0].pos.x = pv[1].pos.x = tr.left;
    pv[0].pos.y = pv[2].pos.y = tr.top;
    pv[2].pos.x = pv[3].pos.x = tr.right;
    pv[1].pos.y = pv[3].pos.y = tr.bottom;
}

inline void SetRectangleTexture(BI_ONETEXTURE_VERTEX *pv, FRECT &tr)
{
    Assert(pv != NULL);
    pv[0].tu = pv[1].tu = tr.left;
    pv[0].tv = pv[2].tv = tr.top;
    pv[2].tu = pv[3].tu = tr.right;
    pv[1].tv = pv[3].tv = tr.bottom;
}

inline void SetRectanglePos(BI_COLOR_VERTEX *pv, FRECT &tr)
{
    Assert(pv != NULL);
    pv[0].pos.x = pv[1].pos.x = tr.left;
    pv[0].pos.y = pv[2].pos.y = tr.top;
    pv[2].pos.x = pv[3].pos.x = tr.right;
    pv[1].pos.y = pv[3].pos.y = tr.bottom;
}

inline void SetRectanglePos(BI_COLOR_VERTEX *pv, float fl, float ft, float fr, float fb)
{
    Assert(pv != NULL);
    pv[0].pos.x = pv[1].pos.x = fl;
    pv[0].pos.y = pv[2].pos.y = ft;
    pv[2].pos.x = pv[3].pos.x = fr;
    pv[1].pos.y = pv[3].pos.y = fb;
}

inline void SetRectangleTexture(BI_COLOR_VERTEX *pv, FRECT &tr)
{
    Assert(pv != NULL);
    pv[0].tu = pv[1].tu = tr.left;
    pv[0].tv = pv[2].tv = tr.top;
    pv[2].tu = pv[3].tu = tr.right;
    pv[1].tv = pv[3].tv = tr.bottom;
}

inline void SetRectangleTexture(BI_COLOR_VERTEX *pv, float fl, float ft, float fr, float fb)
{
    Assert(pv != NULL);
    pv[0].tu = pv[1].tu = fl;
    pv[0].tv = pv[2].tv = ft;
    pv[2].tu = pv[3].tu = fr;
    pv[1].tv = pv[3].tv = fb;
}

inline bool GetVDATABool(VDATA *pvd, bool defVal)
{
    int32_t tmp = defVal;
    if (pvd != nullptr)
        pvd->Get(tmp);
    return (tmp != 0);
}

inline int32_t GetVDATALong(VDATA *pvd, int32_t defVal)
{
    int32_t tmp = defVal;
    if (pvd != nullptr)
        pvd->Get(tmp);
    return tmp;
}
