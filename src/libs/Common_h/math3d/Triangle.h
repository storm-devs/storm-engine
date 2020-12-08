//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Triangle
//============================================================================================

#ifndef _Triangle_h_
#define _Triangle_h_

#pragma pack(push, 1)

#include "CVector.h"
#include "Matrix.h"
#include "Plane.h"

///Класс представления треугольника в 3D пространстве
class Triangle
{
  public:
    union {
        struct
        {
            ///Первая вершина
            CVECTOR p1;
            ///Вторая вершина
            CVECTOR p2;
            ///Третья вершина
            CVECTOR p3;
        };
        struct
        {
            ///Вершины в виде массива
            CVECTOR p[3];
        };
    };

    //-----------------------------------------------------------
    //Конструкторы
    //-----------------------------------------------------------
  public:
    ///Пустой конструктор
    Triangle();
    Triangle(const CVECTOR *v);

    Triangle(const Triangle &rhs) : p1(rhs.p1), p2(rhs.p2), p3(rhs.p3)
    {
    }

    //-----------------------------------------------------------
    //Операторы
    //-----------------------------------------------------------
  public:
    ///Покомпонентное умножение вершин с присваиванием
    Triangle &operator*=(const CVECTOR &v);

    //-----------------------------------------------------------
    //Утилитные
    //-----------------------------------------------------------
  public:
    //Получить нормаль
    CVECTOR GetNormal() const;
    //Получить среднюю точку
    CVECTOR GetCenter() const;
    //Получить треугольник
    Plane GetPlane() const;
    //Плоскость проходящая через грань (p[start], p[NextIndex(start)])
    Plane OrtoPlane(long start) const;
    //Плоскость проходящая через грань (p[0], p[1])
    Plane OrtoPlane01() const;
    //Плоскость проходящая через грань (p[1], p[2])
    Plane OrtoPlane12() const;
    //Плоскость проходящая через грань (p[2], p[0])
    Plane OrtoPlane20() const;
    //Получить предыдущий индекс вершины треугольника
    static long PrevIndex(long index);
    //Получить следующий индекс вершины треугольника
    static long NextIndex(long index);

    //Найти ближнюю точку в треугольнике к данной, лежащей в плоскости треугольника, true - внутри треугольника
    bool FindClosestPoint(const CVECTOR &trgNormal, CVECTOR &pointOnPlane) const;

    //Преобразовать координаты вершин
    Triangle &Transform(const CMatrix &mtx);

    enum CoIntersectionResult
    {
        cir_none = 0, //Не пересекаются
        cir_deg_cur,  //Текущий треугольник вырожденный
        cir_deg_t,    //Данный треугольник вырожденный
        cir_coplanar, //Не пересекаються но лежат в одной плоскости
        cir_intersection, //Лежат в одной плоскости, пересекаються, но не совпадают
        cir_equal, //Треугольники совпадают
    };

    //Проверка треугольников на пересечение в одной плоскости
    CoIntersectionResult IsCoplanarIntersection(const Triangle &t, float intsEps = 0.0000001f) const;

  private:
    static long z_sysClipTriangleEdgePlane(Plane plane, CVECTOR src[8], CVECTOR dst[8], long count);
};

//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
__forceinline Triangle::Triangle()
{
}

///Заполнить все компоненты
__forceinline Triangle::Triangle(const CVECTOR *v)
{
    p[0] = v[0];
    p[1] = v[1];
    p[2] = v[2];
}

//===========================================================
//Операторы
//===========================================================

//Покомпонентное умножение вершин с присваиванием
__forceinline Triangle &Triangle::operator*=(const CVECTOR &v)
{
    p1 *= v;
    p2 *= v;
    p3 *= v;
    return *this;
}

/*!\relates Triangle
Умножить треугольник на матрицу
*/
__forceinline Triangle operator*(const CMatrix &mtx, const Triangle &t)
{
    Triangle trg;
    trg.p1 = mtx * t.p1;
    trg.p2 = mtx * t.p2;
    trg.p3 = mtx * t.p3;
    return trg;
}

/*!\relates Triangle
Умножить треугольник на матрицу
*/
__forceinline Triangle operator*(const Triangle &t, const CMatrix &mtx)
{
    Triangle trg;
    trg.p1 = mtx * t.p1;
    trg.p2 = mtx * t.p2;
    trg.p3 = mtx * t.p3;
    return trg;
}

//===========================================================
//Утилитные
//===========================================================

//Получить нормаль
__forceinline CVECTOR Triangle::GetNormal() const
{
    return !((p1 - p2) ^ (p1 - p3));
}

//Получить среднюю точку
__forceinline CVECTOR Triangle::GetCenter() const
{
    return (p1 + p2 + p3) * 0.33333333333333f;
}

//Получить треугольник
__forceinline Plane Triangle::GetPlane() const
{
    return Plane(GetNormal(), p1);
}

//Плоскость проходящая через грань (p[start % 2], p[(start + 1) % 2])
__forceinline Plane Triangle::OrtoPlane(long start) const
{
    Plane plane;
    //Вершины
    const CVECTOR &ps = p[start % 3];
    const CVECTOR &pe = p[NextIndex(start)];
    //Нормаль
    plane.n = !(((p3 - p1) ^ (p1 - p2)) ^ (ps - pe));
    //Дистанция
    plane.Move(ps);
    return plane;
}

//Плоскость проходящая через грань (p[0], p[1])
__forceinline Plane Triangle::OrtoPlane01() const
{
    return OrtoPlane(0);
}

//Плоскость проходящая через грань (p[1], p[2])
__forceinline Plane Triangle::OrtoPlane12() const
{
    return OrtoPlane(1);
}

//Плоскость проходящая через грань (p[2], p[0])
__forceinline Plane Triangle::OrtoPlane20() const
{
    return OrtoPlane(2);
}

//Получить предыдущий индекс вершины треугольника
__forceinline long Triangle::PrevIndex(long index)
{
    index--;
    if (index < 0)
        index = 2;
    if (index > 2)
        index = 2;
    return index;
}

//Получить следующий индекс вершины треугольника
__forceinline long Triangle::NextIndex(long index)
{
    index++;
    if (index < 0)
        index = 0;
    if (index > 2)
        index = 0;
    return index;
}

//Найти ближнюю точку в треугольнике к данной, лежащей в плоскости треугольника, true - внутри треугольника
__forceinline bool Triangle::FindClosestPoint(const CVECTOR &trgNormal, CVECTOR &pointOnPlane) const
{
    const CVECTOR *cPoint = nullptr;
    for (long i = 0; i < 3; i++)
    {
        //Ребро
        const CVECTOR &ps = p[i];
        const CVECTOR &pe = p[i + 1 < 3 ? i + 1 : 0];
        CVECTOR edge = pe - ps;
        //Если треугольник имеет недопустимый размер ребра, не тестим больше его
        float edgeLen = edge.Normalize();
        if (edgeLen < 1e-37f)
            return false;
        //Ортоганальная плоскость
        Plane orto(edge ^ trgNormal, ps);
        //Определяем положение точки
        float distToEdge = orto.Dist(pointOnPlane);
        if (distToEdge > 0.0f)
        {
            //Определяем длинну проекции (pointOnPlane - ps) на ребре
            float prjLength = edge | (pointOnPlane - ps);
            if (prjLength < 0.0f)
            {
                cPoint = &ps;
            }
            else if (prjLength > edgeLen)
            {
                cPoint = &pe;
            }
            else
            {
                pointOnPlane = ps + (pe - ps) * (prjLength / edgeLen);
                return false;
            }
        }
    }
    if (cPoint)
    {
        pointOnPlane = *cPoint;
        return false;
    }
    return true;
}

//Преобразовать координаты вершин
__forceinline Triangle &Triangle::Transform(const CMatrix &mtx)
{
    p1 = mtx * p1;
    p2 = mtx * p2;
    p3 = mtx * p3;
    return *this;
}

//Проверка треугольников на пересечение в одной плоскости
__forceinline Triangle::CoIntersectionResult Triangle::IsCoplanarIntersection(const Triangle &t, float intsEps) const
{
    //Проверим размеры треугольников
    if (~(p1 - p2) < intsEps * intsEps || ~(p2 - p3) < intsEps * intsEps || ~(p3 - p1) < intsEps * intsEps)
        return cir_deg_cur;
    if (~(t.p1 - t.p2) < intsEps * intsEps || ~(t.p2 - t.p3) < intsEps * intsEps || ~(t.p3 - t.p1) < intsEps * intsEps)
        return cir_deg_t;
    //Получим нормали
    CVECTOR n = (p1 - p2) ^ (p1 - p3);
    if (n.Normalize() < 0.0000001f)
        return cir_deg_cur;
    CVECTOR nt = (t.p1 - t.p2) ^ (t.p1 - t.p3);
    if (nt.Normalize() < 0.0000001f)
        return cir_deg_t;
    //Проверим копланарность
    float cs = n | nt;
    static const float cosMin = cosf(0.5f * 3.141592654f / 180.0f);
    if (cs < cosMin)
        return cir_none;
    //Дистанция плоскостей
    float d = n | p1;
    float dt = n | t.p1;
    if (fabs(d - dt) > intsEps)
        return cir_none;
    //Проверим на совпадение
    if (~(p1 - t.p1) + ~(p2 - t.p2) + ~(p3 - t.p3) < intsEps * intsEps)
        return cir_equal;
    if (~(p2 - t.p1) + ~(p3 - t.p2) + ~(p1 - t.p3) < intsEps * intsEps)
        return cir_equal;
    if (~(p3 - t.p1) + ~(p1 - t.p2) + ~(p2 - t.p3) < intsEps * intsEps)
        return cir_equal;
    //Проверим на пересечение клипированием
    static CVECTOR poly1[8], poly2[8];
    poly1[0] = t.p1;
    poly1[1] = t.p2;
    poly1[2] = t.p3;
    long count = 3;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p1 - p2)), p1), poly1, poly2, count);
    if (!count)
        return cir_coplanar;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p2 - p3)), p2), poly2, poly1, count);
    if (!count)
        return cir_coplanar;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p3 - p1)), p3), poly1, poly2, count);
    if (!count)
        return cir_coplanar;
    for (long s = 0; s < count; s++)
    {
        long e = s + 1 < count ? s + 1 : 0;
        float dist = ~(poly2[e] - poly2[s]);
        if (dist < intsEps * intsEps)
            return cir_coplanar;
    }
    return cir_intersection;
}

__forceinline long Triangle::z_sysClipTriangleEdgePlane(Plane plane, CVECTOR src[8], CVECTOR dst[8], long count)
{
    float ds = plane * src[0], de;
    long c, s;
    for (s = 0, c = 0; s < count; s++, ds = de)
    {
        //Если в области, добавляем вершину
        if (ds <= 0.0f)
            dst[c++] = src[s];
        //Индекс следующего
        long e = s + 1 < count ? s + 1 : 0;
        //Дистанции до плоскости
        de = plane * src[e];
        //Если с одной стороны, то продолжаем
        if (ds * de >= 0.0f)
            continue;
        //Есть пересечение
        dst[c++] = src[s] + (src[e] - src[s]) * (ds / (ds - de));
    }
    if (c < 3)
        c = 0;
    return c;
}

#pragma pack(pop)

#endif
