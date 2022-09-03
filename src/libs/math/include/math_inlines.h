#pragma once

#include <cstdint>
#include <cmath>

#include <xmmintrin.h>

#include "c_vector.h"

#define SQR(a) ((a) * (a))
#define FRAND(x) ((x) * (float)rand() / (float)RAND_MAX)
#define SIGN(x) ((x) < 0 ? -1.0f : 1.0f)
#define SIGNZ(x) ((x) == 0.0f ? 0.0f : ((x) < 0.0f ? -1.0f : 1.0f))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX3(x, y, z) (((x) > (y)) ? ((x) > (z) ? (x) : (z)) : ((y) > (z) ? (y) : (z)))
#define MIN3(x, y, z) (((x) < (y)) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)))
#define CLAMP(x) (((x) > 1.0f) ? 1.0f : (((x) < 0.0f) ? 0.0f : (x)))
#define METERS2KNOTS(x) ((x)*3600.0f / 1853.0f)
#define KNOTS2METERS(x) ((x)*1853.0f / 3600.0f)

template <class T> T Clamp(T t)
{
    if (t < static_cast<T>(0))
        return static_cast<T>(0);
    if (t > static_cast<T>(1))
        return static_cast<T>(1);
    return t;
};

template <class T> T Clamp(T t, T Min, T Max)
{
    if (t < Min)
        return Min;
    if (t > Max)
        return Max;
    return t;
};

template <class T> T Bring2Range(T Min1, T Max1, T Min2, T Max2, T Value)
{
    if (Value < Min2)
        Value = Min2;
    if (Value > Max2)
        Value = Max2;
    auto Delta = static_cast<float>(Value - Min2) / static_cast<float>(Max2 - Min2);
    return Min1 + Delta * (Max1 - Min1);
};

template <class T> T Bring2RangeNoCheck(T Min1, T Max1, T Min2, T Max2, T Value)
{
    auto Delta = static_cast<float>(Value - Min2) / static_cast<float>(Max2 - Min2);
    return Min1 + Delta * (Max1 - Min1);
};

template <class T> T Min(T t1, T t2)
{
    return ((t1 < t2) ? t1 : t2);
};

template <class T> T Max(T t1, T t2)
{
    return ((t1 > t2) ? t1 : t2);
};

template <class T> T Sqr(T t1)
{
    return (t1 * t1);
};

constexpr float PI = 3.14159265358979323846f;
constexpr float PIm2 = (PI * 2.0f);
constexpr float PId2 = (PI / 2.0f);
constexpr float PId4 = (PI / 4.0f);

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

inline int ftoi(float f)
{
    return _mm_cvt_ss2si(_mm_load_ss(&f));
}