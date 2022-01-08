//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Math3D
//============================================================================================

#pragma once

#include <cstdint>

#include <xmmintrin.h> // espkk # remove inline asm # 30/Dec/2017

//#define inline __forceinline

#include "math3d/vector.h"

// Fast conversion of a floating point number to an integer with discarding of the fractional part
inline int32_t fftol(float f)
{
    return _mm_cvttss_si32(_mm_load_ss(&f));

    /*int32_t l;
    static const float cnt[2] = {-0.4999999f, 0.4999999f};
    _asm
    {
      mov        eax, f
      fld        f
      shr        eax, 31
      fadd    uint32_t ptr [cnt + eax*4]
      fistp    l
    }
    return l;*/
}

// Fast conversion of a floating point number to an integer with rounding to the nearest
inline int32_t fftoi(float f)
{
    return _mm_cvtss_si32(_mm_load_ss(&f));

    /*int32_t l;
    _asm
    {
      fld        f
      fistp    l
    }
    return l;*/
}

// Fast floor
inline int32_t ffloor(float f)
{
    return _mm_cvtss_si32(_mm_add_ss(_mm_load_ss(&f), _mm_set_ss(-0.5f)));

    /*int32_t l;
    static const float c = -0.5f;
    _asm
    {
      fld        f
      fadd    c
      fistp    l
    }
    return l;*/
}

// Fast ceil
inline int32_t fceil(float f)
{
    return _mm_cvtss_si32(_mm_add_ss(_mm_load_ss(&f), _mm_set_ss(0.5f)));

    /*int32_t l;
    static const float c = 0.5f;
    _asm
    {
      fld        f
      fadd    c
      fistp    l
    }
    return l;*/
}

// Fast fasb in memory
inline float &ffabs(float &f)
{
    *(uint32_t *)&f &= 0x7fffffff;
    return f;
}

// Square
inline float sqrf(float f)
{
    return f * f;
}

// Random number
inline float Rnd(float max = 1.0f)
{
    return rand() * (max * (1.0f / RAND_MAX));
}

// Random number
inline float RRnd(float min, float max)
{
    return min + rand() * ((max - min) * (1.0f / RAND_MAX));
}

// Limit float
inline float Clampf(float v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

// Limit float
inline float Clampfr(float &v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

// Bring the angle to the range 0..2PI
inline float NormAngle2PI(float angle)
{
    static const auto pi = 3.14159265358979323846f;
    if (angle >= 0.0f && angle <= 2 * pi)
        return angle;
    return (angle / (2.0f * pi) - static_cast<int32_t>(angle / (2.0f * pi))) * 2.0f * pi;
}

// Bring the angle to the range -PI..PI
inline float NormAnglePI(float angle)
{
    static const auto pi = 3.14159265358979323846f;
    if (angle >= -pi && angle <= pi)
        return angle;
    return (angle / (2.0f * pi) - static_cast<int32_t>(angle / (2.0f * pi))) * 2.0f * pi - pi;
}

// Calculate acos with limited range
inline float safeACos(float ang)
{
    auto d = static_cast<double>(ang);
    if (d < -1.0)
        d = -1.0;
    if (d > 1.0)
        d = 1.0;
    d = acos(d);
    return static_cast<float>(d);
}

// Calculate asin with limited range
inline float safeASin(float ang)
{
    auto d = static_cast<double>(ang);
    if (d < -1.0)
        d = -1.0;
    if (d > 1.0)
        d = 1.0;
    d = acos(d);
    return static_cast<float>(d);
}

#define ARRSIZE(ar) (sizeof(ar) / sizeof(ar[0]))

#include "math3d/vector4.h"
