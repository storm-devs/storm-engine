//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Color
//============================================================================================

#ifndef _Color_h_
#define _Color_h_

#pragma pack(push, 1)

#include <stdint.h>

#include "../math3D.h"
#include "Vector.h"
#include "Vector4.h"

///Класс представления цвета (float)
class Color
{
  public:
    union {
        struct
        {
            union {
                struct
                {
                    ///Красный
                    float r;
                    ///Зелёный
                    float g;
                    ///Синий
                    float b;
                };

                union {
                    struct
                    {
                        /// rgb в векторе
                        Vector c;
                    };

                    struct
                    {
                        /// rgb в векторе
                        Vector color;
                    };
                };
            };

            union {
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
    Color(const Vector &v, float a = 1.0f);
    ///Заполнить все компоненты
    Color(const Vector4 &v);
    ///Распоковать
    Color(uint32_t c);
    ///Распоковать
    Color(int32_t c);
    ///Конструктор копирования
    Color(const Color &c);

    //-----------------------------------------------------------
    //Операторы
    //-----------------------------------------------------------
  public:
    ///Получить интенсивность rgb
    float operator~() const;
    ///Вернуть цвет с ограниченными компанентами 0..1
    Color operator!() const;

    ///Получить отрицательный цвет
    Color operator-() const;

    ///Присвоить rgb
    Color &operator=(float f);
    ///Распаковать и присвоить
    Color &operator=(uint32_t c);
    ///Присвоить rgb
    Color &operator=(const Vector &v);
    ///Присвоить
    Color &operator=(const Vector4 &v);
    ///Присвоить
    Color &operator=(const Color &c);
    ///Покомпонентное сложение с присваиванием rgb
    Color &operator+=(float f);
    ///Распаковать и сложить
    Color &operator+=(uint32_t c);
    ///Покомпонентное сложение с присваиванием rgb
    Color &operator+=(const Vector &v);
    ///Покомпонентное сложение с присваиванием
    Color &operator+=(const Vector4 &v);
    ///Покомпонентное сложение с присваиванием
    Color &operator+=(const Color &c);
    ///Покомпонентное вычитание с присваиванием rgb
    Color &operator-=(float f);
    ///Распаковать и вычесть
    Color &operator-=(uint32_t c);
    ///Покомпонентное вычитание с присваиванием rgb
    Color &operator-=(const Vector &v);
    ///Покомпонентное вычитание с присваиванием
    Color &operator-=(const Vector4 &v);
    ///Покомпонентное вычитание с присваиванием
    Color &operator-=(const Color &c);
    ///Покомпонентное умножение с присваиванием rgb
    Color &operator*=(float f);
    ///Распаковать и умножить
    Color &operator*=(uint32_t c);
    ///Покомпонентное умножение с присваиванием rgb
    Color &operator*=(const Vector &v);
    ///Покомпонентное умножение с присваиванием
    Color &operator*=(const Vector4 &v);
    ///Покомпонентное умножение с присваиванием
    Color &operator*=(const Color &c);
    ///Покомпонентное деление с присваиванием rgb
    Color &operator/=(float f);
    ///Распаковать и разделить
    Color &operator/=(uint32_t c);
    ///Покомпонентное деление с присваиванием rgb
    Color &operator/=(const Vector &v);
    ///Покомпонентное деление с присваиванием
    Color &operator/=(const Vector4 &v);
    ///Покомпонентное деление с присваиванием
    Color &operator/=(const Color &c);

    ///Скалярное перемножение rgb, результат копируется во все компоненты
    Color &operator|=(const Color &c);

    ///Получить упакованный цвет в long
    operator uint32_t() const;

    //-----------------------------------------------------------
    //Преобразование
    //-----------------------------------------------------------
  public:
    ///Ограничить диапазоном 0..1
    void Clamp();
    ///Ограничить диапазоном
    void Clamp(float min, float max);

    ///Сохранить минимальные компаненты
    void Min(const Color &c);
    ///Сохранить максимальные компаненты
    void Max(const Color &c);

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
    Color &Normalize();

    ///Получить цветовую дистанцию между цветами
    float GetDistance(const Color &c) const;
    ///Получить цветовую дистанцию между цветами в квадрате
    float GetDistanceSqr(const Color &c) const;

    ///Расчитать линейно интерполированное значение
    Color &Lerp(const Color &c1, const Color &c2, float kBlend);
    ///Расчитать линейно интерполированное значение
    Color &LerpA(const Color &c1, const Color &c2, float kBlend);
    //Умножить цвет на число
    Color &MulColor(float k);
    //Умножить альфу на число
    Color &MulAlpha(float k);

    ///Поменять местами r,b
    Color &SwapRB();

    //Получить запакованный цвет в uint32_t
    uint32_t GetDword() const;

    //Преобразование A8R8G8B8 в R5G6B5
    static unsigned short Make565(uint32_t color);
    //Преобразование A8R8G8B8 в X1R5G5B5
    static unsigned short Make555(uint32_t color);
    //Преобразование A8R8G8B8 в A1R5G5B5
    static unsigned short Make1555(uint32_t color);
    //Преобразование A8R8G8B8 в A1R5G5B5
    static unsigned short Make4444(uint32_t color);
};

///Целочисленное представление цвета
class DColor
{
  public:
    union {
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

        union {
            ///Упакованный цвет
            uint32_t c;
            ///Упакованный цвет
            uint32_t color;
        };
    };

    //-----------------------------------------------------------
    //Операторы
    //-----------------------------------------------------------
  public:
    ///Присвоить
    DColor &operator=(uint32_t color);
    ///Присвоить
    DColor &operator=(long color);

    //Получить long
    operator uint32_t() const;
};

//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
inline Color::Color()
{
    r = 0.0f;
    g = 0.0f;
    b = 0.0f;
    a = 1.0f;
}

//Заполнить числом rgb
inline Color::Color(float rgb, float a)
{
    r = rgb;
    g = rgb;
    b = rgb;
    this->a = a;
}

//Заполнить все компоненты
inline Color::Color(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

//Заполнить все компоненты
inline Color::Color(const float f[3], float a)
{
    r = f[0];
    g = f[1];
    b = f[2];
    this->a = a;
}

//Заполнить все компоненты
inline Color::Color(const Vector &v, float a)
{
    r = v.x;
    g = v.y;
    b = v.z;
    this->a = a;
}

//Заполнить все компоненты
inline Color::Color(const Vector4 &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
}

//Распоковать
inline Color::Color(uint32_t c)
{
    *this = c;
}

///Распоковать
inline Color::Color(int32_t c)
{
    *this = static_cast<uint32_t>(c);
}

//Конструктор копирования
inline Color::Color(const Color &c)
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
inline float Color::operator~() const
{
    return GetIntensity();
}

//Вернуть цвет с ограниченными компанентами 0..1
inline Color Color::operator!() const
{
    auto c(*this);
    c.Clamp();
    return c;
}

//Получить отрицательный цвет
inline Color Color::operator-() const
{
    auto c(*this);
    c.r = -c.r;
    c.g = -c.g;
    c.b = -c.b;
    c.a = -c.a;
    return c;
}

//Присвоить rgb
inline Color &Color::operator=(float f)
{
    r = f;
    g = f;
    b = f;
    a = 1.0f;
    return *this;
}

//Присвоить rgb
inline Color &Color::operator=(const Vector &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = 1.0f;
    return *this;
}

//Присвоить
inline Color &Color::operator=(const Vector4 &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
    return *this;
}

//Распаковать и присвоить
inline Color &Color::operator=(uint32_t c)
{
    r = static_cast<unsigned char>(c >> 16) * (1.0f / 255.0f);
    g = static_cast<unsigned char>(c >> 8) * (1.0f / 255.0f);
    b = static_cast<unsigned char>(c >> 0) * (1.0f / 255.0f);
    a = static_cast<unsigned char>(c >> 24) * (1.0f / 255.0f);
    return *this;
}

//Присвоить
inline Color &Color::operator=(const Color &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
}

//Покомпонентное сложение с присваиванием rgb
inline Color &Color::operator+=(float f)
{
    r += f;
    g += f;
    b += f;
    return *this;
}

//Распаковать и сложить
inline Color &Color::operator+=(uint32_t c)
{
    const Color clr(c);
    *this += clr;
    return *this;
}

//Покомпонентное сложение с присваиванием rgb
inline Color &Color::operator+=(const Vector &v)
{
    r += v.x;
    g += v.y;
    b += v.z;
    return *this;
}

//Покомпонентное сложение с присваиванием
inline Color &Color::operator+=(const Vector4 &v)
{
    r += v.x;
    g += v.y;
    b += v.z;
    a += v.w;
    return *this;
}

//Покомпонентное сложение с присваиванием
inline Color &Color::operator+=(const Color &c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

//Покомпонентное вычитание с присваиванием rgb
inline Color &Color::operator-=(float f)
{
    r -= f;
    g -= f;
    b -= f;
    return *this;
}

///Распаковать и вычесть
inline Color &Color::operator-=(uint32_t c)
{
    const Color clr(c);
    *this = clr;
    return *this;
}

//Покомпонентное вычитание с присваиванием rgb
inline Color &Color::operator-=(const Vector &v)
{
    r -= v.x;
    g -= v.y;
    b -= v.z;
    return *this;
}

//Покомпонентное вычитание с присваиванием
inline Color &Color::operator-=(const Vector4 &v)
{
    r -= v.x;
    g -= v.y;
    b -= v.z;
    a -= v.w;
    return *this;
}

//Покомпонентное вычитание с присваиванием
inline Color &Color::operator-=(const Color &c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}

//Покомпонентное умножение с присваиванием rgb
inline Color &Color::operator*=(float f)
{
    r *= f;
    g *= f;
    b *= f;
    return *this;
}

///Распаковать и умножить
inline Color &Color::operator*=(uint32_t c)
{
    const Color clr(c);
    *this *= clr;
    return *this;
}

//Покомпонентное умножение с присваиванием rgb
inline Color &Color::operator*=(const Vector &v)
{
    r *= v.x;
    g *= v.y;
    b *= v.z;
    return *this;
}

//Покомпонентное умножение с присваиванием
inline Color &Color::operator*=(const Vector4 &v)
{
    r *= v.x;
    g *= v.y;
    b *= v.z;
    a *= v.w;
    return *this;
}

//Покомпонентное умножение с присваиванием
inline Color &Color::operator*=(const Color &c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}

//Покомпонентное деление с присваиванием rgb
inline Color &Color::operator/=(float f)
{
    f = 1.0f / f;
    r *= f;
    g *= f;
    b *= f;
    return *this;
}

//Распаковать и разделить
inline Color &Color::operator/=(uint32_t c)
{
    const Color clr(c);
    *this /= clr;
    return *this;
}

//Покомпонентное деление с присваиванием rgb
inline Color &Color::operator/=(const Vector &v)
{
    r /= v.x;
    g /= v.y;
    b /= v.z;
    return *this;
}

//Покомпонентное деление с присваиванием
inline Color &Color::operator/=(const Vector4 &v)
{
    r /= v.x;
    g /= v.y;
    b /= v.z;
    a /= v.w;
    return *this;
}

//Покомпонентное деление с присваиванием
inline Color &Color::operator/=(const Color &c)
{
    r /= c.r;
    g /= c.g;
    b /= c.b;
    a /= c.a;
    return *this;
}

//Скалярное перемножение rgb, результат копируется во все компоненты
inline Color &Color::operator|=(const Color &c)
{
    r = g = b = a = r * c.r + g * c.g + b * c.b;
    return *this;
}

//Получить упакованный цвет в long
inline Color::operator uint32_t() const
{
    auto c(*this);
    c.Clamp();
    return static_cast<long>(c.GetDword());
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
inline Color operator+(const Color &c, float f)
{
    auto clr(c);
    clr += f;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
inline Color operator+(float f, const Color &c)
{
    auto clr(c);
    clr += f;
    return clr;
}

/*!\relates Color
Распаковать и сложить
*/
inline Color operator+(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr += cl;
    return clr;
}

/*!\relates Color
Распаковать и сложить
*/
inline Color operator+(uint32_t cl, const Color &c)
{
    auto clr(c);
    clr += cl;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
inline Color operator+(const Color &c, const Vector &v)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием rgb
*/
inline Color operator+(const Vector &v, const Color &c)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
inline Color operator+(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
inline Color operator+(const Vector4 &v, const Color &c)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
Покомпонентное сложение с присваиванием
*/
inline Color operator+(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr += c2;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
inline Color operator-(const Color &c, float f)
{
    auto clr(c);
    clr -= f;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
inline Color operator-(float f, const Color &c)
{
    Color clr(f);
    clr -= f;
    return clr;
}

/*!\relates Color
Распаковать и вычесть
*/
inline Color operator-(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr -= cl;
    return clr;
}

/*!\relates Color
Распаковать и вычесть
*/
inline Color operator-(uint32_t cl, const Color &c)
{
    Color clr(cl);
    clr -= c;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
inline Color operator-(const Color &c, const Vector &v)
{
    auto clr(c);
    clr -= v;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием rgb
*/
inline Color operator-(const Vector &v, const Color &c)
{
    Color clr(v);
    clr -= c;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
inline Color operator-(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr -= v;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
inline Color operator-(const Vector4 &v, const Color &c)
{
    Color clr(v);
    clr -= c;
    return clr;
}

/*!\relates Color
Покомпонентное вычитание с присваиванием
*/
inline Color operator-(const Color &c1, const Color &c2)
{
    auto c(c1);
    c -= c2;
    return c;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
inline Color operator*(const Color &c, float f)
{
    auto clr(c);
    clr *= f;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
inline Color operator*(float f, const Color &c)
{
    auto clr(c);
    clr *= f;
    return clr;
}

/*!\relates Color
Распаковать и умножить
*/
inline Color operator*(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr *= cl;
    return clr;
}

/*!\relates Color
Распаковать и умножить
*/
inline Color operator*(uint32_t cl, const Color &c)
{
    auto clr(c);
    clr *= cl;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
inline Color operator*(const Color &c, const Vector &v)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием rgb
*/
inline Color operator*(const Vector &v, const Color &c)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
inline Color operator*(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
inline Color operator*(const Vector4 &v, const Color &c)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
Покомпонентное умножение с присваиванием
*/
inline Color operator*(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr *= c2;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
inline Color operator/(const Color &c, float f)
{
    auto clr(c);
    clr /= f;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
inline Color operator/(float f, const Color &c)
{
    Color clr(f);
    clr /= c;
    return clr;
}

/*!\relates Color
Распаковать и разделить
*/
inline Color operator/(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr /= cl;
    return clr;
}

/*!\relates Color
Распаковать и разделить
*/
inline Color operator/(uint32_t cl, const Color &c)
{
    Color clr(cl);
    clr /= c;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
inline Color operator/(const Color &c, const Vector &v)
{
    auto clr(c);
    clr /= v;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием rgb
*/
inline Color operator/(const Vector &v, const Color &c)
{
    Color clr(v);
    clr /= c;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
inline Color operator/(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr /= v;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
inline Color operator/(const Vector4 &v, const Color &c)
{
    Color clr(v);
    clr /= c;
    return clr;
}

/*!\relates Color
Покомпонентное деление с присваиванием
*/
inline Color operator/(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr /= c2;
    return clr;
}

/*!\relates Color
Скалярное перемножение rgb, результат копируется во все компоненты
*/
inline Color operator|(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr |= c2;
    return clr;
}

//===========================================================
//Преобразование
//===========================================================

//Ограничить диапазоном 0..1
inline void Color::Clamp()
{
    if (r < 0.0f)
        r = 0.0f;
    if (r > 1.0f)
        r = 1.0f;
    if (g < 0.0f)
        g = 0.0f;
    if (g > 1.0f)
        g = 1.0f;
    if (b < 0.0f)
        b = 0.0f;
    if (b > 1.0f)
        b = 1.0f;
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;
}

//Ограничить диапазоном
inline void Color::Clamp(float min, float max)
{
    if (r < min)
        r = min;
    if (r > max)
        r = max;
    if (g < min)
        g = min;
    if (g > max)
        g = max;
    if (b < min)
        b = min;
    if (b > max)
        b = max;
    if (a < min)
        a = min;
    if (a > max)
        a = max;
}

//Сохранить минимальные компаненты
inline void Color::Min(const Color &c)
{
    if (r > c.r)
        r = c.r;
    if (g > c.g)
        g = c.g;
    if (b > c.b)
        b = c.b;
    if (a > c.a)
        a = c.a;
}

//Сохранить максимальные компаненты
inline void Color::Max(const Color &c)
{
    if (r < c.r)
        r = c.r;
    if (g < c.g)
        g = c.g;
    if (b < c.b)
        b = c.b;
    if (a < c.a)
        a = c.a;
}

//Яркость -1..1
inline void Color::Brightness(float br)
{
    if (br > 1.0f)
        br = 1.0f;
    if (br < -1.0f)
        br = -1.0f;
    *this += br;
    Clamp();
}

//Гамма -1..1
inline void Color::Gamma(float gm)
{
    Clamp();
    if (gm > 0.0f)
    {
        if (gm > 1.0f)
            gm = 1.0f;
        gm = 1.0f + gm * 9.0f;
        r = powf(r, gm);
        g = powf(g, gm);
        b = powf(b, gm);
    }
    else if (gm < 0.0f)
    {
        if (gm < -1.0f)
            gm = -1.0f;
        gm = 1.0f + gm * 0.9f;
        r = powf(r, gm * 10.0f);
        g = powf(g, gm * 10.0f);
        b = powf(b, gm * 10.0f);
    }
}

//Контраст -1..1
inline void Color::Contrast(float cn)
{
    if (cn > 1.0f)
        cn = 1.0f;
    if (cn < -1.0f)
        cn = -1.0f;
    cn *= 5.0f;
    r = (r - 0.5f) * cn + 0.5f;
    g = (g - 0.5f) * cn + 0.5f;
    b = (b - 0.5f) * cn + 0.5f;
    Clamp();
}

//Изменить сумарно яркость, гамму и контраст
inline void Color::Processed(float br, float gm, float cn)
{
    Contrast(cn);
    Gamma(gm);
    Brightness(br);
}

//===========================================================
//Утилитные
//===========================================================

//Получить интенсивность
inline float Color::GetIntensity() const
{
    return r * 0.299f + g * 0.587f + b * 0.114f;
}

//Нормализовать rgb
inline Color &Color::Normalize()
{
    auto min = r;
    auto max = r;
    if (min > g)
        min = g;
    if (max < g)
        max = g;
    if (min > b)
        min = b;
    if (max < b)
        max = b;
    if (min > 0.0f)
        min = 0.0f;
    auto dlt = max - min;
    r -= min;
    g -= min;
    b -= min;
    if (dlt > 1.0f)
    {
        dlt = 1.0f / dlt;
        r *= dlt;
        g *= dlt;
        b *= dlt;
    }
    Clamp();
    return *this;
}

//Получить цветовую дистанцию между цветами
inline float Color::GetDistance(const Color &c) const
{
    return sqrtf(GetDistanceSqr(c));
}

//Получить цветовую дистанцию между цветами в квадрате
inline float Color::GetDistanceSqr(const Color &c) const
{
    const auto dr = (r - c.r) * 0.299f;
    const auto dg = (g - c.g) * 0.587f;
    const auto db = (b - c.b) * 0.114f;
    return dr * dr + dg * dg + db * db;
}

//Расчитать линейно интерполированное значение
inline Color &Color::Lerp(const Color &c1, const Color &c2, float kBlend)
{
    r = c1.r + (c2.r - c1.r) * kBlend;
    g = c1.g + (c2.g - c1.g) * kBlend;
    b = c1.b + (c2.b - c1.b) * kBlend;
    return *this;
}

//Расчитать линейно интерполированное значение
inline Color &Color::LerpA(const Color &c1, const Color &c2, float kBlend)
{
    r = c1.r + (c2.r - c1.r) * kBlend;
    g = c1.g + (c2.g - c1.g) * kBlend;
    b = c1.b + (c2.b - c1.b) * kBlend;
    a = c1.a + (c2.a - c1.a) * kBlend;
    return *this;
}

//Умножить цвет на число
inline Color &Color::MulColor(float k)
{
    r *= k;
    g *= k;
    b *= k;
    return *this;
}

//Умножить альфу на число
inline Color &Color::MulAlpha(float k)
{
    a *= k;
    return *this;
}

//Поменять местами r,b
inline Color &Color::SwapRB()
{
    const auto t = r;
    r = b;
    b = t;
    return *this;
}

//Получить запакованный цвет в uint32_t
inline uint32_t Color::GetDword() const
{
    /*	long l;*/
    DColor color;
    const auto k = 255.0f;

    color.r = static_cast<uint8_t>(fftoi(r * k));
    color.g = static_cast<uint8_t>(fftoi(g * k));
    color.b = static_cast<uint8_t>(fftoi(b * k));
    color.a = static_cast<uint8_t>(fftoi(a * k));

    return color.c;

    /*_asm
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
    };*/
}

//Преобразование A8R8G8B8 в R5G6B5
inline unsigned short Color::Make565(uint32_t color)
{
    //   11111000 11111100 11111000
    //           11111 111111 11111
    const auto b = (color >> 3) & 0x1f;
    const auto g = (color >> 5) & 0x7e0;
    const auto r = (color >> 8) & 0xf800;
    return static_cast<unsigned short>(r | g | b);
}

//Преобразование A8R8G8B8 в X1R5G5B5
inline unsigned short Color::Make555(uint32_t color)
{
    //   11111000 11111000 11111000
    //           011111 11111 11111
    const auto b = (color >> 3) & 0x1f;
    const auto g = (color >> 6) & 0x3e0;
    const auto r = (color >> 9) & 0x7c00;
    return static_cast<unsigned short>(r | g | b);
}

//Преобразование A8R8G8B8 в A1R5G5B5
inline unsigned short Color::Make1555(uint32_t color)
{
    //   11111000 11111000 11111000
    //          1 11111 11111 11111
    const auto b = (color >> 3) & 0x1f;
    const auto g = (color >> 6) & 0x3e0;
    const auto r = (color >> 9) & 0x7c00;
    const auto a = (color >> 16) & 0x8000;
    return static_cast<unsigned short>(r | g | b | a);
}

//Преобразование A8R8G8B8 в A1R5G5B5
inline unsigned short Color::Make4444(uint32_t color)
{
    //  11110000 11111000 11111000 11111000
    //                  1111 1111 1111 1111
    const auto b = (color >> 4) & 0xf;
    const auto g = (color >> 8) & 0xf0;
    const auto r = (color >> 12) & 0xf00;
    const auto a = (color >> 16) & 0xf000;
    return static_cast<unsigned short>(r | g | b | a);
}

//===========================================================
// DColor
//===========================================================

//Присвоить
inline DColor &DColor::operator=(uint32_t color)
{
    c = color;
    return *this;
}

//Присвоить
inline DColor &DColor::operator=(long color)
{
    c = static_cast<uint32_t>(color);
    return *this;
}

//Получить long
inline DColor::operator uint32_t() const
{
    return static_cast<long>(c);
}

#pragma pack(pop)

#endif
