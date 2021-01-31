//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Math3D
//============================================================================================

#ifndef _Math3D_h_
#define _Math3D_h_

#include <xmmintrin.h> // espkk # remove inline asm # 30/Dec/2017

//#define inline __forceinline

#include "Math3D/Vector.h"

///Быстрое приведение числа с плавающей точкой к целому с отбрасыванием дробной части
inline long fftol(float f)
{
    return _mm_cvttss_si32(_mm_load_ss(&f));

    /*long l;
    static const float cnt[2] = {-0.4999999f, 0.4999999f};
    _asm
    {
      mov		eax, f
      fld		f
      shr		eax, 31
      fadd	uint32_t ptr [cnt + eax*4]
      fistp	l
    }
    return l;*/
}

///Быстрое приведение числа с плавающей точкой к целому с округлением к ближайшему
inline long fftoi(float f)
{
    return _mm_cvtss_si32(_mm_load_ss(&f));

    /*long l;
    _asm
    {
      fld		f
      fistp	l
    }
    return l;*/
}

/// Fast floor
inline long ffloor(float f)
{
    return _mm_cvtss_si32(_mm_add_ss(_mm_load_ss(&f), _mm_set_ss(-0.5f)));

    /*long l;
    static const float c = -0.5f;
    _asm
    {
      fld		f
      fadd	c
      fistp	l
    }
    return l;*/
}

/// Fast ceil
inline long fceil(float f)
{
    return _mm_cvtss_si32(_mm_add_ss(_mm_load_ss(&f), _mm_set_ss(0.5f)));

    /*long l;
    static const float c = 0.5f;
    _asm
    {
      fld		f
      fadd	c
      fistp	l
    }
    return l;*/
}

/// Fast fasb in memory
inline float &ffabs(float &f)
{
    *(unsigned long *)&f &= 0x7fffffff;
    return f;
}

//Возвести в квадрат
inline float sqrf(float f)
{
    return f * f;
}

//Случайное число
inline float Rnd(float max = 1.0f)
{
    return rand() * (max * (1.0f / RAND_MAX));
}

//Случайное число
inline float RRnd(float min, float max)
{
    return min + rand() * ((max - min) * (1.0f / RAND_MAX));
}

//Ограничить float
inline float Clampf(float v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

//Ограничить float
inline float Clampfr(float &v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

//Привести угол к диапазону 0..2PI
inline float NormAngle2PI(float angle)
{
    static const auto pi = 3.14159265358979323846f;
    if (angle >= 0.0f && angle <= 2 * pi)
        return angle;
    return (angle / (2.0f * pi) - static_cast<long>(angle / (2.0f * pi))) * 2.0f * pi;
}

//Привести угол к диапазону -PI..PI
inline float NormAnglePI(float angle)
{
    static const auto pi = 3.14159265358979323846f;
    if (angle >= -pi && angle <= pi)
        return angle;
    return (angle / (2.0f * pi) - static_cast<long>(angle / (2.0f * pi))) * 2.0f * pi - pi;
}

//Посчитать acos с ограничением диапазона
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

//Посчитать asin с ограничением диапазона
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

#include "Math3D/Vector4.h"

#endif
