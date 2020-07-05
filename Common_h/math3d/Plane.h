//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Plane
//============================================================================================

#ifndef _Plane_h_
#define _Plane_h_

#pragma pack(push, 1)

#include "Vector.h"

///Класс представления плоскости в 3D пространстве
class Plane
{
public:
	union
	{
		struct
		{
			///Нормаль
			Vector n;
		};
		struct
		{
			///Нормаль
			Vector normal;
		};
		struct
		{
			///Нормаль
			Vector N;
		};
	};
	union
	{
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
	Plane(const Vector & normal);
	///Создать плоскость
	Plane(const Vector & normal, const Vector & point);
	///Конструктор копирования
	Plane(const Plane & plane);

//-----------------------------------------------------------
//Преобразование
//-----------------------------------------------------------
public:
	///Нормализовать
	Plane & Normalize();
	///Переместить плоскость в заданную точку
	Plane & Move(const Vector & point);


//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:
	//Найти дистанцию до плоскости (*)
	float Dist(const Vector & point) const;
	//Проверить на пересечение отрезка и плоскости
	bool Intersection(const Vector & src, const Vector & dst) const;
	//Найти точку пересечения отрезка и плоскости
	bool Intersection(const Vector & src, const Vector & dst, Vector & res) const;
	//Проверить на пересечение линии и плоскости
	bool IntersectionLine(const Vector & src, const Vector & dst, float & k) const;

};


//===========================================================
//Конструкторы
//===========================================================

///Пустой конструктор
mathinline Plane::Plane()
{
}

///Задать направление
mathinline Plane::Plane(float Nx, float Ny, float Nz)
{
	N.x = Nx; N.y = Ny; N.z = Nz; D = 0.0f;
}

///Задать направление
mathinline Plane::Plane(const Vector & normal)
{
	N = normal;
}

///Создать плоскость
mathinline Plane::Plane(const Vector & normal, const Vector & point)
{
	N = normal; D = normal | point;
}

///Конструктор копирования
mathinline Plane::Plane(const Plane & plane)
{
	N = plane.N; D = plane.D;
}


//===========================================================
//Операторы
//===========================================================

/*!\relates Plane
Дистанция от точки до плоскости
*/
mathinline float operator * (const Vector & point, const Plane & plane)
{
	return (plane.N | point) - plane.D;
}

/*!\relates Plane
Дистанция от точки до плоскости
*/
mathinline float operator * (const Plane & plane, const Vector & point)
{
	return (plane.N | point) - plane.D;
}


//===========================================================
//Преобразование
//===========================================================

///Нормализовать
mathinline Plane & Plane::Normalize()
{
	float d = normal.Normalize();
	if(d != 0.0f) dist /= d; else dist = 0.0f;
	return *this;
}

///Переместить плоскость в заданную точку
mathinline Plane & Plane::Move(const Vector & point)
{
	D = (N | point);
	return *this;
}

//===========================================================
//Утилитные
//===========================================================

//Найти дистанцию до плоскости (*)
mathinline float Plane::Dist(const Vector & point) const
{
	return *this*point;
}

//Проверить на пересечение отрезка и плоскости
mathinline bool Plane::Intersection(const Vector & src, const Vector & dst) const
{
	float dsrc = *this*src;
	float ddst = *this*dst;
	return (dsrc*ddst <= 0.0f);
}

//Найти точку пересечения отрезка и плоскости
mathinline bool Plane::Intersection(const Vector & src, const Vector & dst, Vector & res) const
{
	float dsrc = *this*src;
	float ddst = *this*dst;
	if(dsrc*ddst > 0.0f) return false;
	ddst = dsrc - ddst;
	res = src;
	if(ddst != 0.0f) res += (dst - src)*dsrc/ddst;	
	return true;
}

//Проверить на пересечение линии и плоскости
mathinline bool Plane::IntersectionLine(const Vector & src, const Vector & dst, float & k) const
{
	float dsrc = *this*src;
	float ddst = *this*dst;
	ddst = dsrc - ddst;
	if(fabsf(ddst) <= 1e-30f) return false;
	k = dsrc/ddst;
	return true;
}

#pragma pack(pop)

#endif
