//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Math3D
//============================================================================================

#ifndef _Math3D_h_
#define _Math3D_h_

#include "d_types.h"

#include "CVector.h"
#include "CVector4.h"
#include "Math3D\Box.h"
#include "Math3D\Capsule.h"
#include "Math3D\Color.h"
#include "Math3D\Line.h"
#include "Math3D\Plane.h"
#include "Math3D\Quaternion.h"
#include "Math3D\Sphere.h"
#include "Math3D\Triangle.h"
#include "Matrix.h"
#include <cstdlib>

///Быстрое приведение числа с плавающей точкой к целому с отбрасыванием дробной части
__forceinline long fftol(float f)
{
    long l;
    static const float cnt[2] = {-0.4999999f, 0.4999999f};
    _asm
    {
		mov		eax, f
		fld		f
		shr		eax, 31
		fadd	dword ptr [cnt + eax*4]
		fistp	l
    }
    return l;
}

///Быстрое приведение числа с плавающей точкой к целому с округлением к ближайшему
__forceinline long fftoi(float f)
{
    long l;
    _asm
    {
		fld		f
		fistp	l
    }
    return l;
}

/// Fast floor
__forceinline long ffloor(float f)
{
    long l;
    static const float c = -0.5f;
    _asm
    {
		fld		f
		fadd	c
		fistp	l
    }
    return l;
}

/// Fast ceil
__forceinline long fceil(float f)
{
    long l;
    static const float c = 0.5f;
    _asm
    {
		fld		f
		fadd	c
		fistp	l
    }
    return l;
}

/// Fast fasb in memory
__forceinline float &ffabs(float &f)
{
    *(unsigned long *)&f &= 0x7fffffff;
    return f;
}

//Возвести в квадрат
__forceinline float sqrf(float f)
{
    return f * f;
}

//Случайное число
__forceinline float Rnd(float max = 1.0f)
{
    return rand() * (max * (1.0f / RAND_MAX));
}

//Случайное число
__forceinline float RRnd(float min, float max)
{
    return min + rand() * ((max - min) * (1.0f / RAND_MAX));
}

//Ограничить float
__forceinline float Clampf(float v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

//Ограничить float
__forceinline float Clampfr(float &v, float min = 0.0f, float max = 1.0f)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

//Привести угол к диапазону 0..2PI
__forceinline float NormAngle2PI(float angle)
{
    static const float pi = 3.14159265358979323846f;
    if (angle >= 0.0f && angle <= 2 * pi)
        return angle;
    return (angle / (2.0f * pi) - long(angle / (2.0f * pi))) * 2.0f * pi;
}

//Привести угол к диапазону -PI..PI
__forceinline float NormAnglePI(float angle)
{
    static const float pi = 3.14159265358979323846f;
    if (angle >= -pi && angle <= pi)
        return angle;
    return (angle / (2.0f * pi) - long(angle / (2.0f * pi))) * 2.0f * pi - pi;
}

//Посчитать acos с ограничением диапазона
__forceinline float safeACos(float ang)
{
    double d = (double)ang;
    if (d < -1.0)
        d = -1.0;
    if (d > 1.0)
        d = 1.0;
    d = acos(d);
    return (float)d;
}

//Посчитать asin с ограничением диапазона
__forceinline float safeASin(float ang)
{
    double d = (double)ang;
    if (d < -1.0)
        d = -1.0;
    if (d > 1.0)
        d = 1.0;
    d = acos(d);
    return (float)d;
}

#define ARRSIZE(ar) (sizeof(ar) / sizeof(ar[0]))

#endif
