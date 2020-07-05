//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Vector4
//============================================================================================

#ifndef _Vector4_h_
#define _Vector4_h_

#include "Vector.h"

#pragma pack(push, 1)


///Класс представления вектора c весовым коэфициентом в 3D пространстве
struct Vector4
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
					///Компонента по X
					float x;
					///Компонента по Y
					float y;
					///Компонента по Z
					float z;
				};
				struct
				{
					///Представление в виде вектора
					Vector v;
				};
			};
			///Весовая компонента
			float w;
		};
		struct
		{
			///Представление в виде массива
			float v4[4];
		};
	};

//-----------------------------------------------------------
//Конструкторы
//-----------------------------------------------------------
public:
	///Пустой конструктор
	Vector4();
	///Заполнить числом
	Vector4(float f);
	///Заполнить числом
	Vector4(double d);
	///Заполнить 3 компоненты, 1
	Vector4(float x, float y, float z);
	///Заполнить все компоненты
	Vector4(float x, float y, float z, float w);
	///Заполнить 3 компоненты, 1
	Vector4(const float f[3]);
	///Заполнить 3 компоненты, 1
	Vector4(const double d[3]);
	///Заполнить 3 компоненты, 1
	Vector4(const Vector & v);
	///Конструктор копирования
	Vector4(const Vector4 & v);

//-----------------------------------------------------------
//Операторы
//-----------------------------------------------------------
public:
	///Найти квадрат длинны вектора
	float operator ~ () const;

	///Получить отрицательный вектор
	Vector4 operator - () const;

	///Присвоить
	Vector4 & operator = (float f);
	///Присвоить
	Vector4 & operator = (double d);
	///Присвоить
	Vector4 & operator = (const Vector & v);
	///Присвоить
	Vector4 & operator = (const Vector4 & v);

//-----------------------------------------------------------
//Преобразование
//-----------------------------------------------------------
public:
	///Нормализовать w компаненту
	void Normalize();

};

//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
mathinline Vector4::Vector4(){}

//Заполнить числом
mathinline Vector4::Vector4(float f) : v(f)
{
	w = f;
}

//Заполнить числом
mathinline Vector4::Vector4(double d)
{
	w = float(d);
}

//Заполнить 3 компоненты, 1
mathinline Vector4::Vector4(float x, float y, float z) : v(x, y, z)
{
	w = 1.0f;	
}

//Заполнить все компоненты
mathinline Vector4::Vector4(float x, float y, float z, float w) : v(x, y, z)
{
	this->w = w;
}

//Заполнить 3 компоненты, 1
mathinline Vector4::Vector4(const float f[3]) : v(f[0], f[1], f[2])
{
	w = 1.0f;
}

//Заполнить 3 компоненты, 1
mathinline Vector4::Vector4(const double d[3]) : v(float(d[0]), float(d[1]), float(d[2]))
{
	w = 1.0f;
}

//Заполнить 3 компоненты, 1
mathinline Vector4::Vector4(const Vector & vc) : v(vc)
{
	w = 1.0f;	
}

//Конструктор копирования
mathinline Vector4::Vector4(const Vector4 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

//===========================================================
//Операторы
//===========================================================

///Найти квадрат длинны вектора
mathinline float Vector4::operator ~ () const
{
	return x*x + y*y + z*z + w*w;
}

///Получить отрицательный вектор
mathinline Vector4 Vector4::operator - () const
{
	Vector4 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	v.w = -v.w;
	return v;
}

///Присвоить
mathinline Vector4 & Vector4::operator = (float f)
{
	v = f;
	w = 1.0f;
	return *this;
}

///Присвоить
mathinline Vector4 & Vector4::operator = (double d)
{
	v = d;
	w = 1.0f;
	return *this;
}

///Присвоить
mathinline Vector4 & Vector4::operator = (const Vector & v)
{
	this->v = v;
	w = 1.0f;
	return *this;
}

///Присвоить
mathinline Vector4 & Vector4::operator = (const Vector4 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}


//===========================================================
//Преобразование
//===========================================================

//Нормализовать w компаненту
mathinline void Vector4::Normalize()
{
	double k = 1.0/w;
	x = float(k*x);
	y = float(k*y);
	z = float(k*z);
	w = 1.0f;
}

#pragma pack(pop)

#endif
