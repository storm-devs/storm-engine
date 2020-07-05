//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Color
//============================================================================================

#ifndef _Color_h_
#define _Color_h_

#pragma pack(push, 1)


#include "Vector.h"
#include "Vector4.h"


///Класс представления цвета (float)
class Color
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
					///Красный
					float r;
					///Зелёный
					float g;
					///Синий
					float b;
				};
				union
				{
					struct
					{
						///rgb в векторе
						Vector c;
					};
					struct
					{
						///rgb в векторе
						Vector color;
					};
				};
			};
			union
			{
				///Прозрачность
				float a;
				///Прозрачность
				float alpha;
			};
		};
		struct
		{
			///Представление в виде Vector4
			Vector4 v4;
		};
	};


//-----------------------------------------------------------
//Конструкторы
//-----------------------------------------------------------
public:
	///Пустой конструктор
	Color();
	///Заполнить числом rgb
	Color(float rgb, float a = 1.0f);
	///Заполнить все компоненты
	Color(float r, float g, float b, float a = 1.0f);
	///Заполнить все компоненты
	Color(const float f[3], float a = 1.0f);
	///Заполнить все компоненты
	Color(const Vector & v, float a = 1.0f);
	///Заполнить все компоненты
	Color(const Vector4 & v);
	///Распоковать
	Color(unsigned long c);
	///Распоковать
	Color(long c);
	///Конструктор копирования
	Color(const Color & c);

//-----------------------------------------------------------
//Операторы
//-----------------------------------------------------------
public:
	///Получить интенсивность rgb
	float operator ~ () const;
	///Вернуть цвет с ограниченными компанентами 0..1
	Color operator ! () const;

	///Получить отрицательный цвет
	Color operator - () const;

	///Присвоить rgb
	Color & operator = (float f);
	///Распаковать и присвоить
	Color & operator = (unsigned long c);
	///Присвоить rgb
	Color & operator = (const Vector & v);
	///Присвоить
	Color & operator = (const Vector4 & v);
	///Присвоить
	Color & operator = (const Color & c);
	///Покомпонентное сложение с присваиванием rgb
	Color & operator += (float f);
	///Распаковать и сложить
	Color & operator += (unsigned long c);
	///Покомпонентное сложение с присваиванием rgb
	Color & operator += (const Vector & v);
	///Покомпонентное сложение с присваиванием
	Color & operator += (const Vector4 & v);
	///Покомпонентное сложение с присваиванием
	Color & operator += (const Color & c);
	///Покомпонентное вычитание с присваиванием rgb
	Color & operator -= (float f);
	///Распаковать и вычесть
	Color & operator -= (unsigned long c);
	///Покомпонентное вычитание с присваиванием rgb
	Color & operator -= (const Vector & v);
	///Покомпонентное вычитание с присваиванием
	Color & operator -= (const Vector4 & v);
	///Покомпонентное вычитание с присваиванием
	Color & operator -= (const Color & c);
	///Покомпонентное умножение с присваиванием rgb
	Color & operator *= (float f);
	///Распаковать и умножить
	Color & operator *= (unsigned long c);
	///Покомпонентное умножение с присваиванием rgb
	Color & operator *= (const Vector & v);
	///Покомпонентное умножение с присваиванием
	Color & operator *= (const Vector4 & v);
	///Покомпонентное умножение с присваиванием
	Color & operator *= (const Color & c);
	///Покомпонентное деление с присваиванием rgb
	Color & operator /= (float f);
	///Распаковать и разделить
	Color & operator /= (unsigned long c);
	///Покомпонентное деление с присваиванием rgb
	Color & operator /= (const Vector & v);
	///Покомпонентное деление с присваиванием
	Color & operator /= (const Vector4 & v);
	///Покомпонентное деление с присваиванием
	Color & operator /= (const Color & c);
	
	///Скалярное перемножение rgb, результат копируется во все компоненты
	Color & operator |= (const Color & c);

	///Получить упакованный цвет в long
	operator dword () const;

//-----------------------------------------------------------
//Преобразование
//-----------------------------------------------------------
public:
	///Ограничить диапазоном 0..1
	void Clamp();
	///Ограничить диапазоном
	void Clamp(float min, float max);

	///Сохранить минимальные компаненты
	void Min(const Color & c);
	///Сохранить максимальные компаненты
	void Max(const Color & c);

	///Яркость -1..1
	void Brightness(float br);
	///Гамма -1..1
	void Gamma(float gm);
	///Контраст -1..1
	void Contrast(float cn);
	//Изменить сумарно яркость, гамму и контраст
	void Processed(float br, float gm, float cn);

	///Изменить насыщенность
	//...когда понадобиться - сделаю

//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:
	//Получить интенсивность
	float GetIntensity() const;
	//Нормализовать rgb
	Color & Normalize();

	///Получить цветовую дистанцию между цветами
	float GetDistance(const Color & c) const;
	///Получить цветовую дистанцию между цветами в квадрате
	float GetDistanceSqr(const Color & c) const;

	///Расчитать линейно интерполированное значение
	Color & Lerp(const Color & c1, const Color & c2, float kBlend);
	///Расчитать линейно интерполированное значение
	Color & LerpA(const Color & c1, const Color & c2, float kBlend);
	//Умножить цвет на число
	Color & MulColor(float k);
	//Умножить альфу на число
	Color & MulAlpha(float k);

	///Поменять местами r,b
	Color & SwapRB();

	//Получить запакованный цвет в dword
	unsigned long GetDword() const;


	//Преобразование A8R8G8B8 в R5G6B5
	static unsigned short Make565(unsigned long color);
	//Преобразование A8R8G8B8 в X1R5G5B5
	static unsigned short Make555(unsigned long color);
	//Преобразование A8R8G8B8 в A1R5G5B5
	static unsigned short Make1555(unsigned long color);
	//Преобразование A8R8G8B8 в A1R5G5B5
	static unsigned short Make4444(unsigned long color);


};

///Целочисленное представление цвета
class DColor
{
public:

	union
	{
		struct
		{
			///Синий
			unsigned char b;
			///Зелённый
			unsigned char g;
			///Красный
			unsigned char r;
			///Прозрачность
			unsigned char a;
		};
		union
		{
			///Упакованный цвет
			unsigned long c;
			///Упакованный цвет
			unsigned long color;
		};
	};

//-----------------------------------------------------------
//Операторы
//-----------------------------------------------------------
public:
	///Присвоить
	DColor & operator = (unsigned long color);
	///Присвоить
	DColor & operator = (long color);

	//Получить long
	operator dword () const;
};
	

//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
mathinline Color::Color()
{
	r = 0.0f;
	g = 0.0f;
	b = 0.0f;
	a = 1.0f;
}

//Заполнить числом rgb
mathinline Color::Color(float rgb, float a)
{
	r = rgb;
	g = rgb;
	b = rgb;
	this->a = a;
}

//Заполнить все компоненты
mathinline Color::Color(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

//Заполнить все компоненты
mathinline Color::Color(const float f[3], float a)
{
	r = f[0];
	g = f[1];
	b = f[2];
	this->a = a;
}

//Заполнить все компоненты
mathinline Color::Color(const Vector & v, float a)
{
	r = v.x;
	g = v.y;
	b = v.z;
	this->a = a;
}

//Заполнить все компоненты
mathinline Color::Color(const Vector4 & v)
{
	r = v.x;
	g = v.y;
	b = v.z;
	a = v.w;
}

//Распоковать
mathinline Color::Color(unsigned long c)
{
	*this = c;
}

///Распоковать
mathinline Color::Color(long c)
{
	*this = (dword)c;
}

//Конструктор копирования
mathinline Color::Color(const Color & c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}


//===========================================================
//Операторы
//===========================================================

//Получить интенсивность rgb
mathinline float Color::operator ~ () const
{
	return GetIntensity();
}

//Вернуть цвет с ограниченными компанентами 0..1
mathinline Color Color::operator ! () const
{
	Color c(*this);
	c.Clamp();
	return c;
}

//Получить отрицательный цвет
mathinline Color Color::operator - () const
{
	Color c(*this);
	c.r = -c.r;
	c.g = -c.g;
	c.b = -c.b;
	c.a = -c.a;
	return c;
}

//Присвоить rgb
mathinline Color & Color::operator = (float f)
{
	r = f;
	g = f;
	b = f;
	a = 1.0f;
	return *this;
}

//Присвоить rgb
mathinline Color & Color::operator = (const Vector & v)
{
	r = v.x;
	g = v.y;
	b = v.z;
	a = 1.0f;
	return *this;
}

//Присвоить
mathinline Color & Color::operator = (const Vector4 & v)
{
	r = v.x;
	g = v.y;
	b = v.z;
	a = v.w;
	return *this;
}

//Распаковать и присвоить
mathinline Color & Color::operator = (unsigned long c)
{
	r = ((unsigned char)(c >> 16))*(1.0f/255.0f);
	g = ((unsigned char)(c >> 8))*(1.0f/255.0f);
	b = ((unsigned char)(c >> 0))*(1.0f/255.0f);
	a = ((unsigned char)(c >> 24))*(1.0f/255.0f);
	return *this;
}

//Присвоить
mathinline Color & Color::operator = (const Color & c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
	return *this;
}

//Покомпонентное сложение с присваиванием rgb
mathinline Color & Color::operator += (float f)
{
	r += f;
	g += f;
	b += f;
	return *this;
}

//Распаковать и сложить
mathinline Color & Color::operator += (unsigned long c)
{
	Color clr(c);
	*this += clr;
	return *this;
}

//Покомпонентное сложение с присваиванием rgb
mathinline Color & Color::operator += (const Vector & v)
{
	r += v.x;
	g += v.y;
	b += v.z;
	return *this;
}

//Покомпонентное сложение с присваиванием
mathinline Color & Color::operator += (const Vector4 & v)
{
	r += v.x;
	g += v.y;
	b += v.z;
	a += v.w;
	return *this;
}

//Покомпонентное сложение с присваиванием
mathinline Color & Color::operator += (const Color & c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	a += c.a;
	return *this;
}

//Покомпонентное вычитание с присваиванием rgb
mathinline Color & Color::operator -= (float f)
{
	r -= f;
	g -= f;
	b -= f;
	return *this;
}

///Распаковать и вычесть
mathinline Color & Color::operator -= (unsigned long c)
{
	Color clr(c);
	*this = clr;
	return *this;
}

//Покомпонентное вычитание с присваиванием rgb
mathinline Color & Color::operator -= (const Vector & v)
{
	r -= v.x;
	g -= v.y;
	b -= v.z;
	return *this;
}

//Покомпонентное вычитание с присваиванием
mathinline Color & Color::operator -= (const Vector4 & v)
{
	r -= v.x;
	g -= v.y;
	b -= v.z;
	a -= v.w;
	return *this;
}

//Покомпонентное вычитание с присваиванием
mathinline Color & Color::operator -= (const Color & c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	a -= c.a;
	return *this;
}

//Покомпонентное умножение с присваиванием rgb
mathinline Color & Color::operator *= (float f)
{
	r *= f;
	g *= f;
	b *= f;
	return *this;
}

///Распаковать и умножить
mathinline Color & Color::operator *= (unsigned long c)
{
	Color clr(c);
	*this *= clr;
	return *this;
}

//Покомпонентное умножение с присваиванием rgb
mathinline Color & Color::operator *= (const Vector & v)
{
	r *= v.x;
	g *= v.y;
	b *= v.z;
	return *this;
}

//Покомпонентное умножение с присваиванием
mathinline Color & Color::operator *= (const Vector4 & v)
{
	r *= v.x;
	g *= v.y;
	b *= v.z;
	a *= v.w;
	return *this;
}

//Покомпонентное умножение с присваиванием
mathinline Color & Color::operator *= (const Color & c)
{
	r *= c.r;
	g *= c.g;
	b *= c.b;
	a *= c.a;
	return *this;
}

//Покомпонентное деление с присваиванием rgb
mathinline Color & Color::operator /= (float f)
{
	f = 1.0f/f;
	r *= f;
	g *= f;
	b *= f;
	return *this;
}

//Распаковать и разделить
mathinline Color & Color::operator /= (unsigned long c)
{
	Color clr(c);
	*this /= clr;
	return *this;
}

//Покомпонентное деление с присваиванием rgb
mathinline Color & Color::operator /= (const Vector & v)
{
	r /= v.x;
	g /= v.y;
	b /= v.z;
	return *this;
}

//Покомпонентное деление с присваиванием
mathinline Color & Color::operator /= (const Vector4 & v)
{
	r /= v.x;
	g /= v.y;
	b /= v.z;
	a /= v.w;
	return *this;
}

//Покомпонентное деление с присваиванием
mathinline Color & Color::operator /= (const Color & c)
{
	r /= c.r;
	g /= c.g;
	b /= c.b;
	a /= c.a;
	return *this;
}


//Скалярное перемножение rgb, результат копируется во все компоненты
mathinline Color & Color::operator |= (const Color & c)
{
	r = g= b = a = r*c.r + g*c.g + b*c.b;
	return *this;
}

//Получить упакованный цвет в long
mathinline Color::operator dword () const
{
	Color c(*this);
	c.Clamp();
	return long(c.GetDword());
}


/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
mathinline Color operator + (const Color & c, float f)
{
	Color clr(c);
	clr += f;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
mathinline Color operator + (float f, const Color & c)
{
	Color clr(c);
	clr += f;
	return clr;
}

/*!\relates Color
Распаковать и сложить
*/
mathinline Color operator + (const Color & c, unsigned long cl)
{
	Color clr(c);
	clr += cl;
	return clr;
}

/*!\relates Color
Распаковать и сложить
*/
mathinline Color operator + (unsigned long cl, const Color & c)
{
	Color clr(c);
	clr += cl;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
mathinline Color operator + (const Color & c, const Vector & v)
{
	Color clr(c);
	clr += v;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
mathinline Color operator + (const Vector & v, const Color & c)
{
	Color clr(c);
	clr += v;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
mathinline Color operator + (const Color & c, const Vector4 & v)
{
	Color clr(c);
	clr += v;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
mathinline Color operator + (const Vector4 & v, const Color & c)
{
	Color clr(c);
	clr += v;
	return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
mathinline Color operator + (const Color & c1, const Color & c2)
{
	Color clr(c1);
	clr += c2;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
mathinline Color operator - (const Color & c, float f)
{
	Color clr(c);
	clr -= f;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
mathinline Color operator - (float f, const Color & c)
{
	Color clr(f);
	clr -= f;
	return clr;
}

/*!\relates Color
Распаковать и вычесть
*/
mathinline Color operator - (const Color & c, unsigned long cl)
{
	Color clr(c);
	clr -= cl;
	return clr;
}

/*!\relates Color
Распаковать и вычесть
*/
mathinline Color operator - (unsigned long cl, const Color & c)
{
	Color clr(cl);
	clr -= c;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
mathinline Color operator - (const Color & c, const Vector & v)
{
	Color clr(c);
	clr -= v;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
mathinline Color operator - (const Vector & v, const Color & c)
{
	Color clr(v);
	clr -= c;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
mathinline Color operator - (const Color & c, const Vector4 & v)
{
	Color clr(c);
	clr -= v;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
mathinline Color operator - (const Vector4 & v, const Color & c)
{
	Color clr(v);
	clr -= c;
	return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
mathinline Color operator - (const Color & c1, const Color & c2)
{
	Color c(c1);
	c -= c2;
	return c;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
mathinline Color operator * (const Color & c, float f)
{
	Color clr(c);
	clr *= f;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
mathinline Color operator * (float f, const Color & c)
{
	Color clr(c);
	clr *= f;
	return clr;
}

/*!\relates Color
Распаковать и умножить
*/
mathinline Color operator * (const Color & c, unsigned long cl)
{
	Color clr(c);
	clr *= cl;
	return clr;
}

/*!\relates Color
Распаковать и умножить
*/
mathinline Color operator * (unsigned long cl, const Color & c)
{
	Color clr(c);
	clr *= cl;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
mathinline Color operator * (const Color & c, const Vector & v)
{
	Color clr(c);
	clr *= v;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
mathinline Color operator * (const Vector & v, const Color & c)
{
	Color clr(c);
	clr *= v;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
mathinline Color operator * (const Color & c, const Vector4 & v)
{
	Color clr(c);
	clr *= v;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
mathinline Color operator * (const Vector4 & v, const Color & c)
{
	Color clr(c);
	clr *= v;
	return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
mathinline Color operator * (const Color & c1, const Color & c2)
{
	Color clr(c1);
	clr *= c2;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
mathinline Color operator / (const Color & c, float f)
{
	Color clr(c);
	clr /= f;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
mathinline Color operator / (float f, const Color & c)
{
	Color clr(f);
	clr /= c;
	return clr;
}


/*!\relates Color
Распаковать и разделить
*/
mathinline Color operator / (const Color & c, unsigned long cl)
{
	Color clr(c);
	clr /= cl;
	return clr;
}

/*!\relates Color
Распаковать и разделить
*/
mathinline Color operator / (unsigned long cl, const Color & c)
{
	Color clr(cl);
	clr /= c;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
mathinline Color operator / (const Color & c, const Vector & v)
{
	Color clr(c);
	clr /= v;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
mathinline Color operator / (const Vector & v, const Color & c)
{
	Color clr(v);
	clr /= c;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
mathinline Color operator / (const Color & c, const Vector4 & v)
{
	Color clr(c);
	clr /= v;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
mathinline Color operator / (const Vector4 & v, const Color & c)
{
	Color clr(v);
	clr /= c;
	return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
mathinline Color operator / (const Color & c1, const Color & c2)
{
	Color clr(c1);
	clr /= c2;
	return clr;
}

/*!\relates Color
Скалярное перемножение rgb, результат копируется во все компоненты
*/
mathinline Color operator | (const Color & c1, const Color & c2)
{
	Color clr(c1);
	clr |= c2;
	return clr;
}


//===========================================================
//Преобразование
//===========================================================

//Ограничить диапазоном 0..1
mathinline void Color::Clamp()
{
	if(r < 0.0f) r = 0.0f;
	if(r > 1.0f) r = 1.0f;
	if(g < 0.0f) g = 0.0f;
	if(g > 1.0f) g = 1.0f;
	if(b < 0.0f) b = 0.0f;
	if(b > 1.0f) b = 1.0f;
	if(a < 0.0f) a = 0.0f;
	if(a > 1.0f) a = 1.0f;
}

//Ограничить диапазоном
mathinline void Color::Clamp(float min, float max)
{
	if(r < min) r = min;
	if(r > max) r = max;
	if(g < min) g = min;
	if(g > max) g = max;
	if(b < min) b = min;
	if(b > max) b = max;
	if(a < min) a = min;
	if(a > max) a = max;
}

//Сохранить минимальные компаненты
mathinline void Color::Min(const Color & c)
{
	if(r > c.r) r = c.r;
	if(g > c.g) g = c.g;
	if(b > c.b) b = c.b;
	if(a > c.a) a = c.a;
}

//Сохранить максимальные компаненты
mathinline void Color::Max(const Color & c)
{
	if(r < c.r) r = c.r;
	if(g < c.g) g = c.g;
	if(b < c.b) b = c.b;
	if(a < c.a) a = c.a;
}

//Яркость -1..1
mathinline void Color::Brightness(float br)
{
	if(br > 1.0f) br = 1.0f;
	if(br < -1.0f) br = -1.0f;
	*this += br;
	Clamp();
}

//Гамма -1..1
mathinline void Color::Gamma(float gm)
{
	Clamp();
	if(gm > 0.0f)
	{
		if(gm > 1.0f) gm = 1.0f;
		gm = 1.0f + gm*9.0f;
		r = powf(r, gm);
		g = powf(g, gm);
		b = powf(b, gm);
	}else
	if(gm < 0.0f)
	{
		if(gm < -1.0f) gm = -1.0f;
		gm = 1.0f + gm*0.9f;
		r = powf(r, gm*10.0f);
		g = powf(g, gm*10.0f);
		b = powf(b, gm*10.0f);
	}
}

//Контраст -1..1
mathinline void Color::Contrast(float cn)
{
	if(cn > 1.0f) cn = 1.0f;
	if(cn < -1.0f) cn = -1.0f;
	cn *= 5.0f;
	r = (r - 0.5f)*cn + 0.5f;
	g = (g - 0.5f)*cn + 0.5f;
	b = (b - 0.5f)*cn + 0.5f;
	Clamp();

}

//Изменить сумарно яркость, гамму и контраст
mathinline void Color::Processed(float br, float gm, float cn)
{
	Contrast(cn);
	Gamma(gm);
	Brightness(br);
}


//===========================================================
//Утилитные
//===========================================================

//Получить интенсивность
mathinline float Color::GetIntensity() const
{
	return r*0.299f + g*0.587f + b*0.114f;
}

//Нормализовать rgb
mathinline Color & Color::Normalize()
{
	float min = r;
	float max = r;
	if(min > g) min = g;
	if(max < g) max = g;
	if(min > b) min = b;
	if(max < b) max = b;
	if(min > 0.0f) min = 0.0f;
	float dlt = max - min;
	r -= min;
	g -= min;
	b -= min;
	if(dlt > 1.0f)
	{
		dlt = 1.0f/dlt;
		r *= dlt;
		g *= dlt;
		b *= dlt;
	}
	Clamp();
	return *this;
}

//Получить цветовую дистанцию между цветами
mathinline float Color::GetDistance(const Color & c) const
{
	return sqrtf(GetDistanceSqr(c));
}

//Получить цветовую дистанцию между цветами в квадрате
mathinline float Color::GetDistanceSqr(const Color & c) const
{
	float dr = (r - c.r)*0.299f;
	float dg = (g - c.g)*0.587f;
	float db = (b - c.b)*0.114f;
	return dr*dr + dg*dg + db*db;
}

//Расчитать линейно интерполированное значение
mathinline Color & Color::Lerp(const Color & c1, const Color & c2, float kBlend)
{
	r = c1.r + (c2.r - c1.r)*kBlend;
	g = c1.g + (c2.g - c1.g)*kBlend;
	b = c1.b + (c2.b - c1.b)*kBlend;
	return *this;
}

//Расчитать линейно интерполированное значение
mathinline Color & Color::LerpA(const Color & c1, const Color & c2, float kBlend)
{
	r = c1.r + (c2.r - c1.r)*kBlend;
	g = c1.g + (c2.g - c1.g)*kBlend;
	b = c1.b + (c2.b - c1.b)*kBlend;
	a = c1.a + (c2.a - c1.a)*kBlend;
	return *this;
}

//Умножить цвет на число
mathinline Color & Color::MulColor(float k)
{
	r *= k;
	g *= k;
	b *= k;
	return *this;
}

//Умножить альфу на число
mathinline Color & Color::MulAlpha(float k)
{
	a *= k;
	return *this;
}

//Поменять местами r,b
mathinline Color & Color::SwapRB()
{
	float t = r;
	r = b;
	b = t;
	return *this;
}

//Получить запакованный цвет в dword
mathinline unsigned long Color::GetDword() const
{
	long l;
	DColor color;
	float k = 255.0f;
	_asm
	{
		mov		eax, this
		fld		[eax]this.r
		fld		k
		fmul
		fistp	l
		mov		ebx, l
		mov		color.r, bl
		fld		[eax]this.g
		fld		k
		fmul
		fistp	l
		mov		ebx, l
		mov		color.g, bl
		fld		[eax]this.b
		fld		k
		fmul
		fistp	l
		mov		ebx, l
		mov		color.b, bl
		fld		[eax]this.a
		fld		k
		fmul
		fistp	l
		mov		ebx, l
		mov		color.a, bl
	};
	return color.c;
}

//Преобразование A8R8G8B8 в R5G6B5
mathinline unsigned short Color::Make565(unsigned long color)
{
	//   11111000 11111100 11111000
    //           11111 111111 11111
	unsigned long b = (color >> 3) & 0x1f;
	unsigned long g = (color >> 5) & 0x7e0;
	unsigned long r = (color >> 8) & 0xf800;
	return (unsigned short)(r | g | b);
}

//Преобразование A8R8G8B8 в X1R5G5B5
mathinline unsigned short Color::Make555(unsigned long color)
{
	//   11111000 11111000 11111000
	//           011111 11111 11111
	unsigned long b = (color >> 3) & 0x1f;
	unsigned long g = (color >> 6) & 0x3e0;
	unsigned long r = (color >> 9) & 0x7c00;
	return (unsigned short)(r | g | b);
}

//Преобразование A8R8G8B8 в A1R5G5B5
mathinline unsigned short Color::Make1555(unsigned long color)
{
	//   11111000 11111000 11111000
	//          1 11111 11111 11111
	unsigned long b = (color >> 3) & 0x1f;
	unsigned long g = (color >> 6) & 0x3e0;
	unsigned long r = (color >> 9) & 0x7c00;
	unsigned long a = (color >> 16) & 0x8000;
	return (unsigned short)(r | g | b | a);
}

//Преобразование A8R8G8B8 в A1R5G5B5
mathinline unsigned short Color::Make4444(unsigned long color)
{
	//  11110000 11111000 11111000 11111000
	//                  1111 1111 1111 1111
	unsigned long b = (color >> 4) & 0xf;
	unsigned long g = (color >> 8) & 0xf0;
	unsigned long r = (color >> 12) & 0xf00;
	unsigned long a = (color >> 16) & 0xf000;
	return (unsigned short)(r | g | b | a);
}

//===========================================================
//DColor
//===========================================================

//Присвоить
mathinline DColor & DColor::operator = (unsigned long color)
{
	c = color;
	return *this;
}

//Присвоить
mathinline DColor & DColor::operator = (long color)
{
	c = (unsigned long)color;
	return *this;
}

//Получить long
mathinline DColor::operator dword () const
{
	return long(c);
}



#pragma pack(pop)

#endif
