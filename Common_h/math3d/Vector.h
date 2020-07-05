//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Vector
//============================================================================================

#ifndef _Vector_h_
#define _Vector_h_

#include <stdlib.h>
#include <math.h>

#pragma pack(push, 1)


///Класс представления вектора в 3D пространстве
class Vector
{
public:
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
		///Представление в виде массива
		float v[3];
	};

//-----------------------------------------------------------
//Конструкторы
//-----------------------------------------------------------
public:
	///Пустой конструктор
	Vector();
	///Заполнить числом
	Vector(float f);
	///Заполнить числом
	Vector(double d);
	///Заполнить все компоненты
	Vector(float x, float y, float z);
	///Заполнить все компоненты
	Vector(const float f[3]);
	///Заполнить все компоненты
	Vector(const double d[3]);
	///Конструктор копирования
	Vector(const Vector & v);
	
//-----------------------------------------------------------
//Операторы
//-----------------------------------------------------------
public:
	///Найти квадрат длинны вектора
	float operator ~ () const;
	///Вернуть нормализованный вектор
	Vector operator ! () const;

	///Получить отрицательный вектор
	Vector operator - () const;

	///Присвоить
	Vector & operator = (float f);
	///Присвоить
	Vector & operator = (double d);
	///Присвоить
	Vector & operator = (const Vector & v);
	///Покомпонентное сложение с присваиванием
	Vector & operator += (float f);
	///Покомпонентное сложение с присваиванием
	Vector & operator += (double d);
	///Покомпонентное сложение с присваиванием
	Vector & operator += (const Vector & v);
	///Покомпонентное вычитание с присваиванием
	Vector & operator -= (float f);
	///Покомпонентное вычитание с присваиванием
	Vector & operator -= (double d);
	///Покомпонентное вычитание с присваиванием
	Vector & operator -= (const Vector & v);
	///Покомпонентное умножение с присваиванием
	Vector & operator *= (float f);
	///Покомпонентное умножение с присваиванием
	Vector & operator *= (double d);
	///Покомпонентное умножение с присваиванием
	Vector & operator *= (const Vector & v);
	///Покомпонентное деление с присваиванием
	Vector & operator /= (float f);
	///Покомпонентное деление с присваиванием
	Vector & operator /= (double d);
	///Покомпонентное деление с присваиванием
	Vector & operator /= (const Vector & v);
	
	///Скалярное перемножение, результат копируется во все компоненты
	Vector & operator |= (const Vector & v);
	///Векторное перемножение
	Vector & operator ^= (const Vector & v);

//-----------------------------------------------------------
//Преобразование
//-----------------------------------------------------------
public:
	///Нормализовать вектор, и вернуть его бывшую длинну
	float Normalize();
	///Спроецировать на плоскость XZ и нормализовать
	float Normalize2D();

	///Ограничить длинну вектора, и вернуть текущую длинну
	float ClampLength(float clampValue);
	
	///Ограничить компоненту X диапазоном
	Vector & ClampX(float min, float max);
	///Ограничить компоненту Y диапазоном
	Vector & ClampY(float min, float max);
	///Ограничить компоненту Z диапазоном
	Vector & ClampZ(float min, float max);
	///Ограничить компоненты диапазоном
	Vector & Clamp(float min, float max);

	///Сохранить в векторе минимальные компаненты
	Vector & Min(const Vector & v);
	///Сохранить в векторе максимальные компаненты
	Vector & Max(const Vector & v);


//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:
	///Установить новые значения
	Vector & Set(float x, float y, float z);
	///Установить новые значения
	Vector & Set(const Vector & v);

	///Получить угол между векторами
	float GetAngle(const Vector & v) const;
	///Получить знаковый угол между векторами в плоскости XZ
	float GetAngle2D(const Vector & v) const;
	///Получить угол поворота вектора вокруг оси Y
	float GetAY(float defAngle = 0.0f) const;
	
	///Получить синус между 2D векторами в плоскости XZ
	float Sin2D(const Vector & v) const;
	///Получить косинус между 2D векторами в плоскости XZ
	float Cos2D(const Vector & v) const;
	///Получить векторное произведение векторов в плоскости XZ
	float Cross2D(const Vector & v) const;

	///Получить вектор в плоскости XZ
	Vector Get2D() const;
	///Получить вектор в плоскости XZ
	Vector Get2D(float y) const;
	///Сформировать вектор в плоскости XZ по углу
	Vector & Make2D(float ay);


	///Получить длинну вектора
	float GetLength() const;
	///Получить длинну вектора в 2D
	float GetLength2D() const;
	///Получить квадрат длинны вектора в 2D
	float GetLength2D2() const;
	
	///Повернуть вектор в плоскости XZ на угол
	Vector & Rotate(float angle);
	///Повернуть вектор в плоскости XZ на угол заданный cos, sin
	Vector & Rotate(float vcos, float vsin);
	///Повернуть вектор по часовой стрелке в плоскости XZ на угол PI/2
	Vector & Rotate_PI2_CW();
	///Повернуть вектор против часовой стрелке в плоскости XZ на угол PI/2
	Vector & Rotate_PI2_CCW();

	///Расчитать линейно интерполированное значение
	Vector & Lerp(const Vector & v1, const Vector & v2, float kBlend);

	///Расчитать отражённый вектор
	Vector & Reflection(const Vector & normal);

	///Заполнить единичным вектором со случайным направлением
	Vector & Rand();
	///Заполнить единичным вектором со случайным направлением в XZ
	Vector & Rand2D();
	///Заполнить случайными значениями в заданном ABB
	Vector & Rand(const Vector & min, const Vector & max);
	///Заполнить случайными значениями в заданной сфере
	Vector & Rand(const Vector & pos, float radius);

	//Переместить текущий вектор к заданной точке на заданный шаг
	bool MoveByStep(const Vector & to, float step);
	//Переместить текущий вектор к заданной точке на заданный шаг в 2D
	bool MoveByStep2D(const Vector & to, float step);

	//Точка находиться в ящике
	bool InBox(const Vector & min, const Vector & max) const;
	//Точка находиться в шаре
	bool InSphere(const Vector & pos, float rad) const;
};


//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
mathinline Vector::Vector(){}

//Заполнить числом
mathinline Vector::Vector(float f)
{
	x = f;
	y = f;
	z = f;
}

//Заполнить числом
mathinline Vector::Vector(double d)
{
	x = float(d);
	y = float(d);
	z = float(d);
}

//Заполнить все компоненты
mathinline Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

//Заполнить все компоненты
mathinline Vector::Vector(const float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

//Заполнить все компоненты
mathinline Vector::Vector(const double d[3])
{
	x = float(d[0]);
	y = float(d[1]);
	z = float(d[2]);
}

//Конструктор копирования
mathinline Vector::Vector(const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}


//===========================================================
//Операторы
//===========================================================

//Найти квадрат длинны вектора
mathinline float Vector::operator ~ () const
{
	return x*x + y*y + z*z;
}

//Вернуть нормализованный вектор
mathinline Vector Vector::operator ! () const
{
	Vector v(*this);
	float length = v.Normalize();
	//assert(fabsf(length) > 1e-20);
	return v;
}

mathinline Vector Vector::operator - () const
{
	Vector v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

//Присвоить
mathinline Vector & Vector::operator = (float f)
{
	x = f;
	y = f;
	z = f;
	return *this;
}

//Присвоить
mathinline Vector & Vector::operator = (double d)
{
	x = float(d);
	y = float(d);
	z = float(d);
	return *this;
}

//Присвоить
mathinline Vector & Vector::operator = (const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

//Покомпонентное сложение с присваиванием
mathinline Vector & Vector::operator += (float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

//Покомпонентное сложение с присваиванием
mathinline Vector & Vector::operator += (double d)
{
	x += float(d);
	y += float(d);
	z += float(d);
	return *this;
}

//Покомпонентное сложение с присваиванием
mathinline Vector & Vector::operator += (const Vector & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//Покомпонентное вычитание с присваиванием
mathinline Vector & Vector::operator -= (float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

//Покомпонентное вычитание с присваиванием
mathinline Vector & Vector::operator -= (double d)
{
	x -= float(d);
	y -= float(d);
	z -= float(d);
	return *this;
}

//Покомпонентное вычитание с присваиванием
mathinline Vector & Vector::operator -= (const Vector & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//Покомпонентное умножение с присваиванием
mathinline Vector & Vector::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

//Покомпонентное умножение с присваиванием
mathinline Vector & Vector::operator *= (double d)
{
	x *= float(d);
	y *= float(d);
	z *= float(d);
	return *this;
}

//Покомпонентное умножение с присваиванием
mathinline Vector & Vector::operator *= (const Vector & v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

//Покомпонентное деление с присваиванием
mathinline Vector & Vector::operator /= (float f)
{
	double d = 1.0/f;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

//Покомпонентное деление с присваиванием
mathinline Vector & Vector::operator /= (double d)
{
	d = 1.0/d;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

//Покомпонентное деление с присваиванием
mathinline Vector & Vector::operator /= (const Vector & v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}


//Скалярное перемножение, результат копируется во все компоненты
mathinline Vector & Vector::operator |= (const Vector & v)
{
	x = y = z = v.x*x + v.y*y + v.z*z;
	return *this;
}

//Векторное перемножение
mathinline Vector & Vector::operator ^= (const Vector & v)
{
	float tx = y * v.z - z * v.y;
	float ty = z * v.x - x * v.z;
	float tz = x * v.y - y * v.x;
	x = tx;
	y = ty;
	z = tz;
	return *this;
}

/*!\relates Vector
Сложить
*/
mathinline Vector operator + (const Vector & v, float f)
{
	Vector tv(v);
	tv += f;
	return tv;
}

/*!\relates Vector
Сложить
*/
mathinline Vector operator + (float f, const Vector & v)
{
	Vector tv(v);
	tv += f;
	return tv;
}

/*!\relates Vector
Сложить
*/
mathinline Vector operator + (const Vector & v, double d)
{
	Vector tv(v);
	tv += d;
	return tv;
}

/*!\relates Vector
Сложить
*/
mathinline Vector operator + (double d, const Vector & v)
{
	Vector tv(v);
	tv += d;
	return tv;
}

/*!\relates Vector
Сложить
*/
mathinline Vector operator + (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv += v2;
	return tv;
}

/*!\relates Vector
Вычесть
*/
mathinline Vector operator - (const Vector & v, float f)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

/*!\relates Vector
Вычесть
*/
mathinline Vector operator - (float f, const Vector & v)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

/*!\relates Vector
Вычесть
*/
mathinline Vector operator - (const Vector & v, double d)
{
	Vector tv(v);
	tv -= d;
	return tv;
}

/*!\relates Vector
Вычесть
*/
mathinline Vector operator - (double d, const Vector & v)
{
	Vector tv(v);
	tv -= d;
	return tv;
}

/*!\relates Vector
Вычесть
*/
mathinline Vector operator - (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv -= v2;
	return tv;
}

/*!\relates Vector
Умножить
*/
mathinline Vector operator * (const Vector & v, float f)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

/*!\relates Vector
Умножить
*/
mathinline Vector operator * (float f, const Vector & v)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

/*!\relates Vector
Умножить
*/
mathinline Vector operator * (const Vector & v, double d)
{
	Vector tv(v);
	tv *= d;
	return tv;
}

/*!\relates Vector
Умножить
*/
mathinline Vector operator * (double d, const Vector & v)
{
	Vector tv(v);
	tv *= d;
	return tv;
}

/*!\relates Vector
Умножить
*/
mathinline Vector operator * (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv *= v2;
	return tv;
}

/*!\relates Vector
Разделить
*/
mathinline Vector operator / (const Vector & v, float f)
{
	Vector tv(v);
	tv /= f;
	return tv;
}

/*!\relates Vector
Разделить
*/
mathinline Vector operator / (float f, const Vector & v)
{
	Vector tv(f);
	tv /= v;
	return tv;
}

/*!\relates Vector
Разделить
*/
mathinline Vector operator / (const Vector & v, double d)
{
	Vector tv(v);
	tv /= d;
	return tv;
}

/*!\relates Vector
Разделить
*/
mathinline Vector operator / (double d, const Vector & v)
{
	Vector tv(d);
	tv /= v;
	return tv;
}

/*!\relates Vector
Разделить
*/
mathinline Vector operator / (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv /= v2;
	return tv;
}

/*!\relates Vector
Скалярное перемножение
*/
mathinline float operator | (const Vector & v1, const Vector & v2)
{	
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

/*!\relates Vector
Векторное перемножение
*/
mathinline Vector operator ^ (const Vector & v1, const Vector & v2)
{
	Vector v;
	v.x = v1.y*v2.z - v1.z*v2.y;
	v.y = v1.z*v2.x - v1.x*v2.z;
	v.z = v1.x*v2.y - v1.y*v2.x;
	return v;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator > (const Vector & v1, const Vector & v2)
{
	if(v1.x > v2.x && v1.y > v2.y && v1.z > v2.z) return true;	
	return false;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator >= (const Vector & v1, const Vector & v2)
{
	if(v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z) return true;	
	return false;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator < (const Vector & v1, const Vector & v2)
{
	if(v1.x < v2.x && v1.y < v2.y && v1.z < v2.z) return true;	
	return false;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator <= (const Vector & v1, const Vector & v2)
{
	if(v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z) return true;	
	return false;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator == (const Vector & v1, const Vector & v2)
{
	if(fabsf(v1.x - v2.x) < 1e-34f && fabsf(v1.y - v2.y) < 1e-34f && fabsf(v1.z - v2.z) < 1e-34f) return true;
	return false;
}

/*!\relates Vector
Сравнить покомпонентно
*/
mathinline bool operator != (const Vector & v1, const Vector & v2)
{	
	return !(v1 == v2);
}

//===========================================================
//Преобразование
//===========================================================

//Нормализовать вектор, и вернуть его бывшую длинну
mathinline float Vector::Normalize()
{
	double len = sqrt(double(x)*double(x) + double(y)*double(y) + double(z)*double(z));
	if(len > 0.0)
	{
		double k = 1.0/len;
		x = float(x*k);
		y = float(y*k);
		z = float(z*k);
	}
	return float(len);
}

//Спроецировать на плоскость XZ и нормализовать
mathinline float Vector::Normalize2D()
{
	y = 0.0f;
	double len = sqrt(double(x)*double(x) + double(z)*double(z));
	if(len > 0.0)
	{
		double k = 1.0/len;
		x = float(x*k);
		z = float(z*k);
	}
	return float(len);
}

//Ограничить длинну вектора, и вернуть текущую длинну
mathinline float Vector::ClampLength(float clampValue)
{
	if(clampValue <= 0.0f)
	{
		x = y = z = 0.0f;
		return 0.0f;
	}
	double len = sqrt(x*x + y*y + z*z);
	if(len > clampValue)
	{
		double k = clampValue/len;
		x = float(x*k);
		y = float(y*k);
		z = float(z*k);
		return clampValue;
	}
	return float(len);
}

//Ограничить компоненту X диапазоном
mathinline Vector & Vector::ClampX(float min, float max)
{
	if(x < min) x = min;
	if(x > max) x = max;
	return *this;
}

//Ограничить компоненту Y диапазоном
mathinline Vector & Vector::ClampY(float min, float max)
{
	if(y < min) y = min;
	if(y > max) y = max;
	return *this;
}

//Ограничить компоненту Z диапазоном
mathinline Vector & Vector::ClampZ(float min, float max)
{
	if(z < min) z = min;
	if(z > max) z = max;
	return *this;
}

//Ограничить компоненты диапазоном
mathinline Vector & Vector::Clamp(float min, float max)
{
	if(x < min) x = min;
	if(x > max) x = max;
	if(y < min) y = min;
	if(y > max) y = max;
	if(z < min) z = min;
	if(z > max) z = max;
	return *this;
}

//Сохранить в векторе минимальные компаненты
mathinline Vector & Vector::Min(const Vector & v)
{
	if(x > v.x) x = v.x;
	if(y > v.y) y = v.y;
	if(z > v.z) z = v.z;
	return *this;
}

//Сохранить в векторе максимальные компаненты
mathinline Vector & Vector::Max(const Vector & v)
{
	if(x < v.x) x = v.x;
	if(y < v.y) y = v.y;
	if(z < v.z) z = v.z;
	return *this;
}


//===========================================================
//Утилитные
//===========================================================

//Установить новые значения
mathinline Vector & Vector::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

//Установить новые значения
mathinline Vector & Vector::Set(const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

//Получить угол между векторами
mathinline float Vector::GetAngle(const Vector & v) const
{
	double len = double(x)*double(x) + double(y)*double(y) + double(z)*double(z);
	len *= double(x)*double(x) + double(y)*double(y) + double(z)*double(z);
	if(len <= 0.0) return 0.0f;
	double cs = (x*v.x + y*v.y + z*v.z)/sqrt(len);
	if(cs > 1.0) cs = 1.0;
	if(cs < -1.0) cs = -1.0;
	return float(acos(cs));
}

///Получить знаковый угол между векторами в плоскости XZ
mathinline float Vector::GetAngle2D(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 0.0f;
	len = (x*v.x + z*v.z)/sqrt(len);
	if(len > 1.0) len = 1.0;
	if(len < -1.0) len = -1.0;
	len = acos(len);
	if(z*v.x - x*v.z < 0) len = -len;
	return float(len);
}

//Получить угол поворота вектора вокруг оси Y
mathinline float Vector::GetAY(float defAngle) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	if(len > 0.00000000001)
	{
		len = z/sqrt(len);
		if(len > 1.0) len = 1.0;
		if(len < -1.0) len = -1.0;
		len = acos(len);
	}else return defAngle;
	if(x < 0) len = -len;
	return float(len);
}

//Получить синус между 2D векторами в плоскости XZ
mathinline float Vector::Sin2D(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 0.0f;
	len = (z*v.x - x*v.z)/sqrt(len);
	return float(len);
}

//Получить косинус между 2D векторами в плоскости XZ
mathinline float Vector::Cos2D(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 1.0f;
	len = (x*v.x + z*v.z)/sqrt(len);
	return float(len);
}

//Получить векторное произведение векторов в плоскости XZ
mathinline float Vector::Cross2D(const Vector & v) const
{
	return z*v.x - x*v.z;
}

//Получить вектор в плоскости XZ
mathinline Vector Vector::Get2D() const
{
	return Vector(x, 0.0f, z);
}

///Получить вектор в плоскости XZ
mathinline Vector Vector::Get2D(float y) const
{
	return Vector(x, y, z);
}

//Сформировать вектор в плоскости XZ по угло
mathinline Vector & Vector::Make2D(float ay)
{
	x = sinf(ay);
	y = 0.0f;
	z = cosf(ay);
	return *this;
}

//Получить длинну вектора
mathinline float Vector::GetLength() const
{
	return float(sqrt(double(x)*double(x) + double(y)*double(y) + double(z)*double(z)));
}

//Получить длинну вектора в 2D
mathinline float Vector::GetLength2D() const
{
	return float(sqrt(double(x)*double(x) + double(z)*double(z)));
}

///Получить квадрат длинны вектора в 2D
mathinline float Vector::GetLength2D2() const
{
	return x*x + z*z;
}

//Повернуть вектор в плоскости XZ на угол
mathinline Vector & Vector::Rotate(float angle)
{
	return Rotate(cosf(angle), sinf(angle));
}

//Повернуть вектор в плоскости XZ на угол заданный cos, sin
mathinline Vector & Vector::Rotate(float vcos, float vsin)
{
	float tx = x*vcos + z*vsin;
	float tz = z*vcos - x*vsin;
	x = tx; z = tz;
	return *this;
}

//Повернуть вектор по часовой стрелке в плоскости XZ на угол PI/2
mathinline Vector & Vector::Rotate_PI2_CW()
{
	float t = x;
	x = z;
	z = -t;
	return *this;
}

//Повернуть вектор против часовой стрелке в плоскости XZ на угол PI/2
mathinline Vector & Vector::Rotate_PI2_CCW()
{
	float t = x;
	x = -z;
	z = t;
	return *this;
}

//Расчитать линейноинтерпалированное значение
mathinline Vector & Vector::Lerp(const Vector & v1, const Vector & v2, float kBlend)
{
	x = v1.x + (v2.x - v1.x)*kBlend;
	y = v1.y + (v2.y - v1.y)*kBlend;
	z = v1.z + (v2.z - v1.z)*kBlend;
	return *this;
}

//Расчитать отражённый вектор
mathinline Vector & Vector::Reflection(const Vector & normal)
{
	float k = -2.0f*(x*normal.x + y*normal.y + z*normal.z);
	x += normal.x*k;
	y += normal.y*k;
	z += normal.z*k;
	return *this;
}

//Заполнить единичным вектором со случайным направлением
mathinline Vector & Vector::Rand()
{
	Make2D(rand()*((2.0f*3.141592654f)/RAND_MAX));
	y = z; z = 0.0f;
	return Rotate(rand()*((2.0f*3.141592654f)/RAND_MAX));
}

//Заполнить единичным вектором со случайным направлением в XZ
mathinline Vector & Vector::Rand2D()
{
	return Make2D(rand()*((2.0f*3.141592654f)/RAND_MAX));
}

///Заполнить случайными значениями в заданном ABB
mathinline Vector & Vector::Rand(const Vector & min, const Vector & max)
{
	x = min.x + rand()*((max.x - min.x)*(1.0f/RAND_MAX));
	y = min.y + rand()*((max.y - min.y)*(1.0f/RAND_MAX));
	z = min.z + rand()*((max.z - min.z)*(1.0f/RAND_MAX));
	return *this;
}

///Заполнить случайными значениями в заданной сфере
mathinline Vector & Vector::Rand(const Vector & pos, float radius)
{
	Rand();
	*this *= rand()*(radius*(1.0f/RAND_MAX));
	return *this;
}

//Переместить текущий вектор к заданной точке на заданный шаг
mathinline bool Vector::MoveByStep(const Vector & to, float step)
{
	if(step <= 0.0f) return false;
	//Дистанция до точки
	float dx = to.x - x;
	float dy = to.y - y;
	float dz = to.z - z;
	float dist = dx*dx + dy*dy + dz*dz;
	if(dist < 1e-20f)
	{
		x = to.x;
		y = to.y;
		z = to.z;
		return false;
	}
	dist = sqrtf(dist);
	//Движение к точке
	if(step >= dist)
	{
		x = to.x;
		y = to.y;
		z = to.z;
		return true;
	}
	step /= dist;
	x += dx*step;
	y += dy*step;
	z += dz*step;
	return true;
}

//Переместить текущий вектор к заданной точке на заданный шаг в 2D
mathinline bool Vector::MoveByStep2D(const Vector & to, float step)
{
	if(step <= 0.0f) return false;
	//Дистанция до точки
	float dx = to.x - x;
	float dz = to.z - z;
	float dist = dx*dx + dz*dz;
	if(dist < 1e-20f)
	{
		x = to.x;
		z = to.z;
		return false;
	}
	dist = sqrtf(dist);
	//Движение к точке
	if(step >= dist)
	{
		x = to.x;
		z = to.z;
		return true;
	}
	step /= dist;
	x += dx*step;
	z += dz*step;
	return true;
}

//Точка находиться в ящике
mathinline bool Vector::InBox(const Vector & min, const Vector & max) const
{
	if(y < min.y || y > max.y) return false;
	if(x < min.x || x > max.x) return false;
	if(z < min.z || z > max.z) return false;
	return true;
}

//Точка находиться в шаре
mathinline bool Vector::InSphere(const Vector & pos, float rad) const
{
	double d = (x - pos.x)*(x - pos.x);
	d += (y - pos.y)*(y - pos.y);
	d += (z - pos.z)*(z - pos.z);
	return d < double(rad)*double(rad);
}

#pragma pack(pop)


#endif
