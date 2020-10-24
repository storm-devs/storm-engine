//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Sphere
//============================================================================================

#ifndef _Sphere_h_
#define _Sphere_h_

#pragma pack(push, 1)

#include "../CVector.h"
#include "../CVector4.h"

///Класс представления шара в 3D пространстве
class Sphere
{
  public:
    union {
        struct
        {
            union {
                struct
                {
                    ///Позиция по X
                    float x;
                    ///Позиция по Y
                    float y;
                    ///Позиция по Z
                    float z;
                };
                union {
                    struct
                    {
                        ///Позиция
                        CVECTOR p;
                    };
                    struct
                    {
                        ///Позиция
                        CVECTOR pos;
                    };
                };
            };
            union {
                ///Радиус
                float r;
                ///Радиус
                float radius;
            };
        };
        struct
        {
            ///Представление в виде CVECTOR4
            CVECTOR4 v4;
        };
    };

    //-----------------------------------------------------------
    //Утилитные
    //-----------------------------------------------------------
  public:
    Sphere()
    {
    }

    //Точка в сфере
    bool Intersection(const CVECTOR &p);
    //Проверить пересечение отрезка и сферы
    bool Intersection(const CVECTOR &src, const CVECTOR &dst);
    //Проверить пересечение луча и сферы
    bool Intersection(const CVECTOR &orig, const CVECTOR &normdir, float *res);
    //Проверить пересечение сферы и сферы
    bool Intersection(const Sphere &sph);

    //Установить сферу в точку с 0 радиусом
    void Reset(const CVECTOR &p);
    //Включить в описывающую сферу точку
    void AddPoint(const CVECTOR &p);

    //Проверить пересечение луча и сферы
    static bool Intersection(const CVECTOR &orig, const CVECTOR &normdir, const CVECTOR &pos, float r, float *res);
};

//===========================================================
//Утилитные
//===========================================================

//Точка в сфере
__forceinline bool Sphere::Intersection(const CVECTOR &p)
{
    return ~(pos - p) <= radius * radius;
}

//Проверить пересечение отрезка и сферы
__forceinline bool Sphere::Intersection(const CVECTOR &src, const CVECTOR &dst)
{
    CVECTOR dir = dst - src;
    float len = dir.Normalize();
    if (len > 1e-10f)
    {
        float dist;
        if (!Intersection(src, dir, pos, r, &dist))
            return false;
        if (dist >= 0.0f)
        {
            return (dist <= len);
        }
        dir = -dir;
        if (!Intersection(dst, dir, pos, r, &dist))
            return false;
        if (dist >= 0.0f)
        {
            return (dist <= len);
        }
        return false;
    }
    return ~(pos - src) <= radius * radius;
}

//Проверить пересечение луча и сферы
__forceinline bool Sphere::Intersection(const CVECTOR &orig, const CVECTOR &normdir, float *res)
{
    return Intersection(orig, normdir, pos, r, res);
}

//Проверить пересечение сферы и сферы
__forceinline bool Sphere::Intersection(const Sphere &sph)
{
    return (~(p - sph.p) <= (r + sph.r) * (r + sph.r));
}

//Установить сферу в точку с 0 радиусом
__forceinline void Sphere::Reset(const CVECTOR &p)
{
    pos = p;
    r = 0.0f;
}

//Включить в описывающую сферу точку
__forceinline void Sphere::AddPoint(const CVECTOR &p)
{
    //Вектор из точки к центру
    float dx = pos.x - p.x;
    float dy = pos.y - p.y;
    float dz = pos.z - p.z;
    float len = dx * dx + dy * dy + dz * dz;
    if (len <= r * r)
        return;
    len = sqrtf(len);
    //Новый радиус
    r = (len + r) * 0.5f;
    //Новая позиция
    len = r / len;
    pos.x = p.x + dx * len;
    pos.y = p.y + dy * len;
    pos.z = p.z + dz * len;
}

//Проверить пересечение луча и сферы
__forceinline bool Sphere::Intersection(const CVECTOR &orig, const CVECTOR &normdir, const CVECTOR &pos, float r,
                                        float *res)
{
    CVECTOR toCenter = pos - orig;
    float distToOrtoPlane = normdir | toCenter;
    float distFromOrtoPlaneToSphere2 = r * r - (~toCenter - distToOrtoPlane * distToOrtoPlane);
    if (distFromOrtoPlaneToSphere2 < 0.0f)
        return false;
    if (res)
    {
        *res = distToOrtoPlane - sqrtf(distFromOrtoPlaneToSphere2);
    }
    return true;
}

#pragma pack(pop)

#endif
