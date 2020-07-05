//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Sphere
//============================================================================================

#ifndef _Sphere_h_
#define _Sphere_h_

#pragma pack(push, 1)


#include "Vector.h"
#include "Vector4.h"


///Класс представления шара в 3D пространстве
class Sphere
{
public:
	union
	{
		struct
		{
			union
			{
				struct
				{
					///Позиция по X
					float x;
					///Позиция по Y
					float y;
					///Позиция по Z
					float z;
				};
				union
				{
					struct
					{
						///Позиция
						Vector p;
					};
					struct
					{
						///Позиция
						Vector pos;
					};
				};
			};
			union
			{
				///Радиус
				float r;
				///Радиус
				float radius;
			};
		};
		struct
		{
			///Представление в виде Vector4
			Vector4 v4;
		};
	};


//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:
	//Точка в сфере
	bool Intersection(const Vector & p);
	//Проверить пересечение отрезка и сферы
	bool Intersection(const Vector & src, const Vector & dst);
	//Проверить пересечение луча и сферы
	bool Intersection(const Vector & orig, const Vector & normdir, float * res);
	//Проверить пересечение сферы и сферы
	bool Intersection(const Sphere & sph);
	
	//Установить сферу в точку с 0 радиусом
	void Reset(const Vector & p);
	//Включить в описывающую сферу точку
	void AddPoint(const Vector & p);


	//Проверить пересечение луча и сферы
	static bool Intersection(const Vector & orig, const Vector & normdir, const Vector & pos, float r, float * res);

};



//===========================================================
//Утилитные
//===========================================================

//Точка в сфере
mathinline bool Sphere::Intersection(const Vector & p)
{
	return ~(pos - p) <= radius*radius;
}

//Проверить пересечение отрезка и сферы
mathinline bool Sphere::Intersection(const Vector & src, const Vector & dst)
{
	Vector dir = dst - src;
	float len = dir.Normalize();
	if(len > 1e-10f)
	{
		float dist;
		if(!Intersection(src, dir, pos, r, &dist)) return false;
		if(dist >= 0.0f)
		{
			return (dist <= len);
		}
		dir = -dir;
		if(!Intersection(dst, dir, pos, r, &dist)) return false;
		if(dist >= 0.0f)
		{
			return (dist <= len);
		}
		return false;
	}
	return ~(pos - src) <= radius*radius;
}

//Проверить пересечение луча и сферы
mathinline bool Sphere::Intersection(const Vector & orig, const Vector & normdir, float * res)
{
	return Intersection(orig, normdir, pos, r, res);
}

//Проверить пересечение сферы и сферы
mathinline bool Sphere::Intersection(const Sphere & sph)
{
	return (~(p - sph.p) <= (r + sph.r)*(r + sph.r));
}

//Установить сферу в точку с 0 радиусом
mathinline void Sphere::Reset(const Vector & p)
{
	pos = p;
	r = 0.0f;
}

//Включить в описывающую сферу точку
mathinline void Sphere::AddPoint(const Vector & p)
{
	//Вектор из точки к центру
	float dx = pos.x - p.x;
	float dy = pos.y - p.y;
	float dz = pos.z - p.z;
	float len = dx*dx + dy*dy + dz*dz;
	if(len <= r*r) return;
	len = sqrtf(len);
	//Новый радиус
	r = (len + r)*0.5f;
	//Новая позиция
	len = r/len;
	pos.x = p.x + dx*len;
	pos.y = p.y + dy*len;
	pos.z = p.z + dz*len;
}

//Проверить пересечение луча и сферы
mathinline bool Sphere::Intersection(const Vector & orig, const Vector & normdir, const Vector & pos, float r, float * res)
{
	Vector toCenter = pos - orig;
	float distToOrtoPlane = normdir | toCenter;
	float distFromOrtoPlaneToSphere2 = r*r - (~toCenter - distToOrtoPlane*distToOrtoPlane);
	if(distFromOrtoPlaneToSphere2 < 0.0f) return false;
	if(res)
	{
		*res = distToOrtoPlane - sqrtf(distFromOrtoPlaneToSphere2);
	}
	return true;	
}

#pragma pack(pop)

#endif
