//============================================================================================
// Spirenkov Maxim, 2004
//============================================================================================
// Capsule
//============================================================================================

#ifndef _Capsule_h_
#define _Capsule_h_

#pragma pack(push, 1)
#include "Sphere.h"
#include "Triangle.h"

//Вертикально расположенная капсула, находящеяся в 0
class Capsule
{
  public:
    Capsule();
    Capsule(float s, float r);

    //Установить радиус сфер и цилиндра
    void SetRadius(float r);
    //Установить растояние между сферами
    void SetSize(float s);
    //Получить параметры выталкивания из треугольника
    bool Extraction(const Triangle &t, CVECTOR &extPoint, CVECTOR &extCVECTOR);

  protected:
    float radius;
    float size05;
    CVECTOR up, down;
};

__forceinline Capsule::Capsule(){};

__forceinline Capsule::Capsule(float s, float r)
{
    SetSize(s);
    SetRadius(r);
};

//Установить радиус сфер и цилиндра
__forceinline void Capsule::SetRadius(float r)
{
    radius = r;
}

//Установить растояние между сферами
__forceinline void Capsule::SetSize(float s)
{
    size05 = s * 0.5f;
    up = CVECTOR(0.0f, size05, 0.0f);
    down = CVECTOR(0.0f, -size05, 0.0f);
}

//Получить параметры выталкивания из треугольника
__forceinline bool Capsule::Extraction(const Triangle &t, CVECTOR &extPoint, CVECTOR &extCVECTOR)
{
    //Определим пересечение с плоскостью
    Plane plane = t.GetPlane();
    if (plane.d > 0.0f)
    {
        //Центр капсулы позади плоскости
        return false;
    }
    float dUp = plane.n.y * size05 - plane.d;
    float dDown = plane.n.y * -size05 - plane.d;
    if (dUp >= radius && dDown >= radius)
    {
        //Капсула не пересекается с плоскостью
        return false;
    }
    //Определим самую нижнию точку пересечения
    CVECTOR pointOnPlane;
    const CVECTOR *nearest;
    float k;
    if (dUp < dDown)
    {
        pointOnPlane = plane.n * -dUp;
        pointOnPlane.y += size05;
        nearest = &up;
        k = 1.0f - dUp;
    }
    else
    {
        pointOnPlane = plane.n * -dDown;
        pointOnPlane.y -= size05;
        nearest = &down;
        k = 1.0f - dDown;
    }
    //Ищим ближайшую точку в треугольнике
    if (t.FindClosestPoint(plane.n, pointOnPlane))
    {
        //Точка пересечения лежит внутри треугольника
        extCVECTOR = plane.n * k;
        extPoint = pointOnPlane - extCVECTOR;
        return true;
    }
    //Смещаем точку на максимальную глубину + 1
    static const float safeDist = 1.0f;
    CVECTOR RayOrigin = pointOnPlane - plane.n * (k + safeDist);
    //Пускаем луч из найденой точки в направлении капсулы и ищем пересечение
    //Для начала решаем задачу для бесконечного вертикального цилиндра в 0
    //Направление на цилиндр
    CVECTOR dir2D = plane.n.Get2D();
    double in2D = ~dir2D;
    if (in2D > 1e-20f)
    {
        //Нормализуем;
        dir2D *= (in2D = 1.0f / sqrt(in2D));
        //Луч наклонный
        float distToOrtoPlane = -(RayOrigin | dir2D);
        float distToEdge2 = radius * radius - (RayOrigin.GetLength2D2() - distToOrtoPlane * distToOrtoPlane);
        if (distToEdge2 <= 0.0f)
        {
            //Капсула не пересекается с треугольником
            return false;
        }
        float distToCylinder = distToOrtoPlane - sqrtf(distToEdge2);
        if (distToCylinder <= 0.0f)
        {
            //Капсула не пересекается с треугольником
            return false;
        }
        //Зная расстояние до цилиндра найдём высоту пересечения
        float y = float(plane.n.y * in2D * distToCylinder);
        //Определимся с попаданием
        if (RayOrigin.y + y > size05)
        {
            //Тестим с верхней сферой
            float dist;
            if (Sphere::Intersection(RayOrigin, plane.n, up, radius, &dist))
            {
                k = k + safeDist - dist;
            }
            else
            {
                return false;
            }
        }
        else if (RayOrigin.y + y < -size05)
        {
            //Тестим с нижней сферой
            float dist;
            if (Sphere::Intersection(RayOrigin, plane.n, down, radius, &dist))
            {
                k = k + safeDist - dist;
            }
            else
            {
                return false;
            }
        }
        else
        {
            //Пересеклись с цилиндром
            k = k + safeDist - sqrtf(distToCylinder * distToCylinder + y * y);
            if (k < 0.0f)
            {
                //Капсула не пересекается с треугольником
                return false;
            }
        }
    }
    else
    {
        //Луч вертикальный
        float dist;
        if (Sphere::Intersection(RayOrigin, plane.n, *nearest, radius, &dist))
        {
            k = k + safeDist - dist;
        }
        else
        {
            return false;
        }
    }
    extCVECTOR = plane.n * k;
    extPoint = pointOnPlane - extCVECTOR;
    return true;
}

#pragma pack(pop)

#endif
