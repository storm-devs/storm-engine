//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Triangle
//============================================================================================

#ifndef _Triangle_h_
#define _Triangle_h_

#pragma pack(push, 1)


#include "Vector.h"
#include "Plane.h"


///Класс представления треугольника в 3D пространстве
class Triangle
{
public:
	union
	{
		struct
		{
			///Первая вершина
			Vector p1;
			///Вторая вершина
			Vector p2;
			///Третья вершина
			Vector p3;
		};
		struct
		{
			///Вершины в виде массива
			Vector p[3];
		};
	};


//-----------------------------------------------------------
//Конструкторы
//-----------------------------------------------------------
public:	
	///Пустой конструктор
	Triangle();
	Triangle(const Vector * v);

//-----------------------------------------------------------
//Операторы
//-----------------------------------------------------------
public:
	///Покомпонентное умножение вершин с присваиванием
	Triangle & operator *= (const Vector & v);

//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:	
	//Получить нормаль
	Vector GetNormal() const;
	//Получить среднюю точку
	Vector GetCenter() const;
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
	bool FindClosestPoint(const Vector & trgNormal, Vector & pointOnPlane) const;


	//Преобразовать координаты вершин
	Triangle & Transform(const Matrix & mtx);


	enum CoIntersectionResult
	{
		cir_none = 0,		//Не пересекаются
		cir_deg_cur,			//Текущий треугольник вырожденный
		cir_deg_t,			//Данный треугольник вырожденный
		cir_coplanar,		//Не пересекаються но лежат в одной плоскости
		cir_intersection,	//Лежат в одной плоскости, пересекаються, но не совпадают
		cir_equal,			//Треугольники совпадают
	};

	//Проверка треугольников на пересечение в одной плоскости
	CoIntersectionResult IsCoplanarIntersection(const Triangle & t, float intsEps = 0.0000001f) const;

private:
	static long z_sysClipTriangleEdgePlane(Plane plane, Vector src[8], Vector dst[8], long count);
};


//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
mathinline Triangle::Triangle(){}

///Заполнить все компоненты
mathinline Triangle::Triangle(const Vector * v)
{
	p[0] = v[0];
	p[1] = v[1];
	p[2] = v[2];
}


//===========================================================
//Операторы
//===========================================================

//Покомпонентное умножение вершин с присваиванием
mathinline Triangle & Triangle::operator *= (const Vector & v)
{
	p1 *= v;
	p2 *= v;
	p3 *= v;
	return *this;
}

/*!\relates Triangle
Умножить треугольник на матрицу
*/
mathinline Triangle operator * (const Matrix & mtx, const Triangle & t)
{
	Triangle trg;
	trg.p1 = mtx*t.p1;
	trg.p2 = mtx*t.p2;
	trg.p3 = mtx*t.p3;
	return trg;
}

/*!\relates Triangle
Умножить треугольник на матрицу
*/
mathinline Triangle operator * (const Triangle & t, const Matrix & mtx)
{
	Triangle trg;
	trg.p1 = mtx*t.p1;
	trg.p2 = mtx*t.p2;
	trg.p3 = mtx*t.p3;
	return trg;
}


//===========================================================
//Утилитные
//===========================================================

//Получить нормаль
mathinline Vector Triangle::GetNormal() const
{
    return !((p1 - p2) ^ (p1 - p3));
}

//Получить среднюю точку
mathinline Vector Triangle::GetCenter() const
{
	return (p1 + p2 + p3)*0.33333333333333f;
}

//Получить треугольник
mathinline Plane Triangle::GetPlane() const
{
	return Plane(GetNormal(), p1);	
}

//Плоскость проходящая через грань (p[start % 2], p[(start + 1) % 2])
mathinline Plane Triangle::OrtoPlane(long start) const
{
	Plane plane;
	//Вершины
	const Vector & ps = p[start % 3];
	const Vector & pe = p[NextIndex(start)];
	//Нормаль
	plane.n = !(((p3 - p1) ^ (p1 - p2)) ^ (ps - pe));
	//Дистанция
	plane.Move(ps);
	return plane;
}

//Плоскость проходящая через грань (p[0], p[1])
mathinline Plane Triangle::OrtoPlane01() const
{
	return OrtoPlane(0);
}

//Плоскость проходящая через грань (p[1], p[2])
mathinline Plane Triangle::OrtoPlane12() const
{
	return OrtoPlane(1);
}

//Плоскость проходящая через грань (p[2], p[0])
mathinline Plane Triangle::OrtoPlane20() const
{
	return OrtoPlane(2);
}


//Получить предыдущий индекс вершины треугольника
mathinline long Triangle::PrevIndex(long index)
{
	index--;
	if(index < 0) index = 2;
	if(index > 2) index = 2;
	return index;
}

//Получить следующий индекс вершины треугольника
mathinline long Triangle::NextIndex(long index)
{
	index++;
	if(index < 0) index = 0;
	if(index > 2) index = 0;
	return index;
}

//Найти ближнюю точку в треугольнике к данной, лежащей в плоскости треугольника, true - внутри треугольника
mathinline bool Triangle::FindClosestPoint(const Vector & trgNormal, Vector & pointOnPlane) const
{
	const Vector * cPoint = null;
	for(long i = 0; i < 3; i++)
	{
		//Ребро
		const Vector & ps = p[i];
		const Vector & pe = p[i + 1 < 3 ? i + 1 : 0];
		Vector edge = pe - ps;
		//Если треугольник имеет недопустимый размер ребра, не тестим больше его
		float edgeLen = edge.Normalize();
		if(edgeLen < 1e-37f) return false;
		//Ортоганальная плоскость
		Plane orto(edge ^ trgNormal, ps);
		//Определяем положение точки
		float distToEdge = orto.Dist(pointOnPlane);
		if(distToEdge > 0.0f)
		{
			//Определяем длинну проекции (pointOnPlane - ps) на ребре
			float prjLength = edge | (pointOnPlane - ps);
			if(prjLength < 0.0f)
			{
				cPoint = &ps;
			}else
			if(prjLength > edgeLen)
			{
				cPoint = &pe;
			}else{
				pointOnPlane = ps + (pe - ps)*(prjLength/edgeLen);
				return false;
			}
		}
	}
	if(cPoint)
	{
		pointOnPlane = *cPoint;
		return false;
	}
	return true;
}

//Преобразовать координаты вершин
mathinline Triangle & Triangle::Transform(const Matrix & mtx)
{
	p1 = mtx.MulVertex(p1);
	p2 = mtx.MulVertex(p2);
	p3 = mtx.MulVertex(p3);
	return *this;
}

//Проверка треугольников на пересечение в одной плоскости
mathinline Triangle::CoIntersectionResult Triangle::IsCoplanarIntersection(const Triangle & t, float intsEps) const
{
	//Проверим размеры треугольников
	if(~(p1 - p2) < intsEps*intsEps || ~(p2 - p3) < intsEps*intsEps || ~(p3 - p1) < intsEps*intsEps) return cir_deg_cur;
	if(~(t.p1 - t.p2) < intsEps*intsEps || ~(t.p2 - t.p3) < intsEps*intsEps || ~(t.p3 - t.p1) < intsEps*intsEps) return cir_deg_t;
	//Получим нормали
	Vector n = (p1 - p2) ^ (p1 - p3);
	if(n.Normalize() < 0.0000001f) return cir_deg_cur;
	Vector nt = (t.p1 - t.p2) ^ (t.p1 - t.p3);
	if(nt.Normalize() < 0.0000001f) return cir_deg_t;
	//Проверим копланарность
	float cs = n | nt;
	static const float cosMin = cosf(0.5f*3.141592654f/180.0f);
	if(cs < cosMin) return cir_none;
	//Дистанция плоскостей
	float d = n | p1;
	float dt = n | t.p1;
	if(fabs(d - dt) > intsEps) return cir_none;
	//Проверим на совпадение
	if(~(p1 - t.p1) + ~(p2 - t.p2) + ~(p3 - t.p3) < intsEps*intsEps) return cir_equal;
	if(~(p2 - t.p1) + ~(p3 - t.p2) + ~(p1 - t.p3) < intsEps*intsEps) return cir_equal;
	if(~(p3 - t.p1) + ~(p1 - t.p2) + ~(p2 - t.p3) < intsEps*intsEps) return cir_equal;
	//Проверим на пересечение клипированием
	static Vector poly1[8], poly2[8];
	poly1[0] = t.p1; poly1[1] = t.p2; poly1[2] = t.p3;
	long count = 3;
	count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p1 - p2)), p1), poly1, poly2, count);
	if(!count) return cir_coplanar;
	count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p2 - p3)), p2), poly2, poly1, count);
	if(!count) return cir_coplanar;
	count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p3 - p1)), p3), poly1, poly2, count);
	if(!count) return cir_coplanar;
	for(long s = 0; s < count; s++)
	{
		long e = s + 1 < count ? s + 1 : 0;
		float dist = ~(poly2[e] - poly2[s]);
		if(dist < intsEps*intsEps) return cir_coplanar;
	}
	return cir_intersection;
}

mathinline long Triangle::z_sysClipTriangleEdgePlane(Plane plane, Vector src[8], Vector dst[8], long count)
{
	float ds = plane*src[0], de;
	for(long s = 0, c = 0; s < count; s++, ds = de)
	{
		//Если в области, добавляем вершину
		if(ds <= 0.0f) dst[c++] = src[s];		
		//Индекс следующего
		long e = s + 1 < count ? s + 1 : 0;
		//Дистанции до плоскости
		de = plane*src[e];
		//Если с одной стороны, то продолжаем
		if(ds*de >= 0.0f) continue;
		//Есть пересечение
		dst[c++] = src[s] + (src[e] - src[s])*(ds/(ds - de));
	}
	if(c < 3) c = 0;
	return c;
}

#pragma pack(pop)

#endif
