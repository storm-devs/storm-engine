//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Plane
//============================================================================================

#ifndef _Plane_h_
#define _Plane_h_

#pragma pack(push, 1)

#include "CVector.h"

///Класс представления плоскости в 3D пространстве
class Plane
{
  public:
    union {
        struct
        {
            ///Нормаль
            CVECTOR n;
        };
        struct
        {
            ///Нормаль
            CVECTOR normal;
        };
        struct
        {
            ///Нормаль
            CVECTOR N;
        };
    };
    union {
        ///Дистанция от центра
        float d;
        ///Дистанция от центра
        float dist;
        ///Дистанция от центра
        float D;
    };

    //-----------------------------------------------------------
    //Конструкторы
    //-----------------------------------------------------------
  public:
    ///Пустой конструктор
    Plane();
    ///Задать направление
    Plane(float Nx, float Ny, float Nz);
    ///Задать направление
    Plane(const CVECTOR &normal);
    ///Создать плоскость
    Plane(const CVECTOR &normal, const CVECTOR &point);
    ///Конструктор копирования
    Plane(const Plane &plane);

    //-----------------------------------------------------------
    //Преобразование
    //-----------------------------------------------------------
  public:
    ///Нормализовать
    Plane &Normalize();
    ///Переместить плоскость в заданную точку
    Plane &Move(const CVECTOR &point);

    //-----------------------------------------------------------
    //Утилитные
    //-----------------------------------------------------------
  public:
    //Найти дистанцию до плоскости (*)
    float Dist(const CVECTOR &point) const;
    //Проверить на пересечение отрезка и плоскости
    bool Intersection(const CVECTOR &src, const CVECTOR &dst) const;
    //Найти точку пересечения отрезка и плоскости
    bool Intersection(const CVECTOR &src, const CVECTOR &dst, CVECTOR &res) const;
    //Проверить на пересечение линии и плоскости
    bool IntersectionLine(const CVECTOR &src, const CVECTOR &dst, float &k) const;
};

//===========================================================
//Конструкторы
//===========================================================

///Пустой конструктор
__forceinline Plane::Plane()
{
}

///Задать направление
__forceinline Plane::Plane(float Nx, float Ny, float Nz)
{
    N.x = Nx;
    N.y = Ny;
    N.z = Nz;
    D = 0.0f;
}

///Задать направление
__forceinline Plane::Plane(const CVECTOR &normal)
{
    N = normal;
}

///Создать плоскость
__forceinline Plane::Plane(const CVECTOR &normal, const CVECTOR &point)
{
    N = normal;
    D = normal | point;
}

///Конструктор копирования
__forceinline Plane::Plane(const Plane &plane)
{
    N = plane.N;
    D = plane.D;
}

//===========================================================
//Операторы
//===========================================================

/*!\relates Plane
Дистанция от точки до плоскости
*/
__forceinline float operator*(const CVECTOR &point, const Plane &plane)
{
    return (plane.N | point) - plane.D;
}

/*!\relates Plane
Дистанция от точки до плоскости
*/
__forceinline float operator*(const Plane &plane, const CVECTOR &point)
{
    return (plane.N | point) - plane.D;
}

//===========================================================
//Преобразование
//===========================================================

///Нормализовать
__forceinline Plane &Plane::Normalize()
{
    float d = normal.Normalize();
    if (d != 0.0f)
        dist /= d;
    else
        dist = 0.0f;
    return *this;
}

///Переместить плоскость в заданную точку
__forceinline Plane &Plane::Move(const CVECTOR &point)
{
    D = (N | point);
    return *this;
}

//===========================================================
//Утилитные
//===========================================================

//Найти дистанцию до плоскости (*)
__forceinline float Plane::Dist(const CVECTOR &point) const
{
    return *this * point;
}

//Проверить на пересечение отрезка и плоскости
__forceinline bool Plane::Intersection(const CVECTOR &src, const CVECTOR &dst) const
{
    float dsrc = *this * src;
    float ddst = *this * dst;
    return (dsrc * ddst <= 0.0f);
}

//Найти точку пересечения отрезка и плоскости
__forceinline bool Plane::Intersection(const CVECTOR &src, const CVECTOR &dst, CVECTOR &res) const
{
    float dsrc = *this * src;
    float ddst = *this * dst;
    if (dsrc * ddst > 0.0f)
        return false;
    ddst = dsrc - ddst;
    res = src;
    if (ddst != 0.0f)
        res += (dst - src) * dsrc / ddst;
    return true;
}

//Проверить на пересечение линии и плоскости
__forceinline bool Plane::IntersectionLine(const CVECTOR &src, const CVECTOR &dst, float &k) const
{
    float dsrc = *this * src;
    float ddst = *this * dst;
    ddst = dsrc - ddst;
    if (fabsf(ddst) <= 1e-30f)
        return false;
    k = dsrc / ddst;
    return true;
}

#pragma pack(pop)

#endif
