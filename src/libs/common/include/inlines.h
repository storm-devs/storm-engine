#pragma once

#include "dx9render.h"

inline uint32_t hash_string(const char *str)
{
    uint32_t hval = 0;
    while (*str != '\0')
    {
        hval = (hval << 4) + static_cast<uint32_t>(*str++);
        const auto g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

inline void RotateAroundY(float &x, float &z, float cos, float sin)
{
    const auto xx = x * cos + z * sin;
    const auto zz = z * cos - x * sin;
    x = xx;
    z = zz;
}

inline float NormalizeAngle(float fAngle)
{
    int32_t times;
    if (fAngle >= 2 * PI)
    {
        times = static_cast<int32_t>(fAngle / PIm2);
        return fAngle - times * PIm2;
    }
    if (fAngle < 0)
    {
        times = static_cast<int32_t>(-fAngle / PIm2);
        return fAngle + PIm2 * (times + 1);
    }
    return fAngle;
}

inline void GetLineABC(const CVECTOR &v1, const CVECTOR &v2, float &A, float &B, float &C)
{
    A = (v2.z - v1.z);
    B = (v1.x - v2.x);
    C = -v1.x * v2.z + v1.z * v2.x;
}

inline bool IntersectLines2D(const CVECTOR &v1, const CVECTOR &v2, const CVECTOR &v3, const CVECTOR &v4, CVECTOR &vRes)
{
    float a1, b1, c1, a2, b2, c2;
    GetLineABC(v1, v2, a1, b1, c1);
    GetLineABC(v3, v4, a2, b2, c2);

    if (a1 == 0.0f && a2 == 0.0f)
        return false;
    if (b1 == 0.0f && b2 == 0.0f)
        return false;
    if (a2 != 0.0f && b2 != 0.0f && fabsf(a1 / a2 - b1 / b2) < 1e-5)
        return false;
    vRes.z = (a2 * c1 - a1 * c2) / (b2 * a1 - a2 * b1);
    vRes.x = (-b1 * vRes.z - c1) / (a1 + 1e-5f);

    const auto half1 = 1.0f / 4.0f * (SQR(v2.x - v1.x) + SQR(v2.z - v1.z));
    const auto d1 = SQR(vRes.x - (v1.x + v2.x) / 2.0f) + SQR(vRes.z - (v1.z + v2.z) / 2.0f);

    if (d1 > half1)
        return false;

    const auto half2 = 1.0f / 4.0f * (SQR(v4.x - v3.x) + SQR(v4.z - v3.z));
    const auto d2 = SQR(vRes.x - (v3.x + v4.x) / 2.0f) + SQR(vRes.z - (v3.z + v4.z) / 2.0f);

    if (d2 > half2)
        return false;

    return true;
}

inline void FillRectVertex(RS_RECT &pV, float x, float y, float z, float fSize, float fAngle = 0.0f,
                           uint32_t dwColor = 0xFFFFFF, uint32_t dwSubTexture = 0)
{
    pV.vPos.x = x;
    pV.vPos.y = y;
    pV.vPos.z = z;
    pV.dwColor = dwColor;
    pV.dwSubTexture = dwSubTexture;
    pV.fAngle = fAngle;
    pV.fSize = fSize;
}

inline void FillSpriteVertex(RS_SPRITE &pV, float x, float y, float z = 1.0f, uint32_t dwColor = 0xFFFFFF,
                             float tu = 0.0f, float tv = 0.0f)
{
    pV.vPos.x = x;
    pV.vPos.y = y;
    pV.vPos.z = z;
    pV.dwColor = dwColor;
    pV.rhw = 0.5f;
    pV.tu = tu;
    pV.tv = tv;
}

inline uint32_t Number2Shift(uint32_t dwNumber)
{
    for (uint32_t i = 0; i < 31; i++)
        if (static_cast<uint32_t>(1 << i) == dwNumber)
            return i;
    return 0;
}
