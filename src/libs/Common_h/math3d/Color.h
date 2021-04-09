//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Color
//============================================================================================

#pragma once

#pragma pack(push, 1)

#include <stdint.h>

#include "../math3D.h"
#include "Vector.h"
#include "Vector4.h"

// Color representation class (float)
class Color
{
  public:
    union {
        struct
        {
            union {
                struct
                {
                    // Red
                    float r;
                    // Green
                    float g;
                    // Blue
                    float b;
                };

                union {
                    struct
                    {
                        // rgb in vector
                        Vector c;
                    };

                    struct
                    {
                        // rgb in vector
                        Vector color;
                    };
                };
            };

            union {
                // Transparency
                float a;
                // Transparency
                float alpha;
            };
        };

        struct
        {
            // Vector4 representation
            Vector4 v4;
        };
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Empty constructor
    Color();
    // Fill with rgb number
    Color(float rgb, float a = 1.0f);
    // Fill all components
    Color(float r, float g, float b, float a = 1.0f);
    // Fill all components
    Color(const float f[3], float a = 1.0f);
    // Fill all components
    Color(const Vector &v, float a = 1.0f);
    // Fill all components
    Color(const Vector4 &v);
    // Unpack
    Color(uint32_t c);
    // Unpack
    Color(int32_t c);
    // Copy constructor
    Color(const Color &c);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Get rgb intensity
    float operator~() const;
    // Return color with constrained components 0..1
    Color operator!() const;

    // Get negative color
    Color operator-() const;

    // Assign rgb
    Color &operator=(float f);
    // Unpack and assign
    Color &operator=(uint32_t c);
    // Assign rgb
    Color &operator=(const Vector &v);
    // Assign
    Color &operator=(const Vector4 &v);
    // Assign
    Color &operator=(const Color &c);
    // per component addition with rgb assignment
    Color &operator+=(float f);
    // Unpack and add
    Color &operator+=(uint32_t c);
    // per component addition with rgb assignment
    Color &operator+=(const Vector &v);
    // per component addition with assignment
    Color &operator+=(const Vector4 &v);
    // per component addition with assignment
    Color &operator+=(const Color &c);
    // per component subtraction with rgb assignment
    Color &operator-=(float f);
    // Unpack and subtract
    Color &operator-=(uint32_t c);
    // per component subtraction with rgb assignment
    Color &operator-=(const Vector &v);
    // per component subtraction with assignment
    Color &operator-=(const Vector4 &v);
    // per component subtraction with assignment
    Color &operator-=(const Color &c);
    // per component multiplication with rgb assignment
    Color &operator*=(float f);
    // Unpack and multiply
    Color &operator*=(uint32_t c);
    // per component multiplication with rgb assignment
    Color &operator*=(const Vector &v);
    // per component multiplication with assignment
    Color &operator*=(const Vector4 &v);
    // per component multiplication with assignment
    Color &operator*=(const Color &c);
    // per component division with rgb assignment
    Color &operator/=(float f);
    // Unpack and divide
    Color &operator/=(uint32_t c);
    // per component division with rgb assignment
    Color &operator/=(const Vector &v);
    // per component division with assignment
    Color &operator/=(const Vector4 &v);
    // per component division with assignment
    Color &operator/=(const Color &c);

    // Scalar rgb multiplication, the result is copied to all components
    Color &operator|=(const Color &c);

    // Get packed color as long
    operator uint32_t() const;

    // -----------------------------------------------------------
    // Transformation
    // -----------------------------------------------------------
  public:
    // Limit to range 0..1
    void Clamp();
    // Limit to range
    void Clamp(float min, float max);

    // Save minimal components
    void Min(const Color &c);
    // Save maximum components
    void Max(const Color &c);

    // Brightness -1..1
    void Brightness(float br);
    // Gamma -1..1
    void Gamma(float gm);
    // Contrast -1..1
    void Contrast(float cn);
    // Change brightness, gamma and contrast
    void Processed(float br, float gm, float cn);

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Get intensity
    float GetIntensity() const;
    // Normalize rgb
    Color &Normalize();

    // Get color distance between colors
    float GetDistance(const Color &c) const;
    // Get the color distance between colors squared
    float GetDistanceSqr(const Color &c) const;

    // Calculate linearly interpolated value
    Color &Lerp(const Color &c1, const Color &c2, float kBlend);
    // Calculate linearly interpolated value
    Color &LerpA(const Color &c1, const Color &c2, float kBlend);
    // Multiply color by number
    Color &MulColor(float k);
    // Multiply alpha by number
    Color &MulAlpha(float k);

    // Swap r, b
    Color &SwapRB();

    // Get packed color as uint32_t
    uint32_t GetDword() const;

    // Converting A8R8G8B8 to R5G6B5
    static unsigned short Make565(uint32_t color);
    // Converting A8R8G8B8 to X1R5G5B5
    static unsigned short Make555(uint32_t color);
    // Converting A8R8G8B8 to A1R5G5B5
    static unsigned short Make1555(uint32_t color);
    // Converting A8R8G8B8 to A1R5G5B5
    static unsigned short Make4444(uint32_t color);
};

// Integer color representation
class DColor
{
  public:
    union {
        struct
        {
            // Blue
            unsigned char b;
            // Green
            unsigned char g;
            // Red
            unsigned char r;
            // Transparency
            unsigned char a;
        };

        union {
            // Packed color
            uint32_t c;
            // Packed color
            uint32_t color;
        };
    };

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Assign
    DColor &operator=(uint32_t color);
    // Assign
    DColor &operator=(long color);

    // Get long
    operator uint32_t() const;
};

// ===========================================================
// Constructors
// ===========================================================

// Empty constructor
inline Color::Color()
{
    r = 0.0f;
    g = 0.0f;
    b = 0.0f;
    a = 1.0f;
}

// Fill with rgb number
inline Color::Color(float rgb, float a)
{
    r = rgb;
    g = rgb;
    b = rgb;
    this->a = a;
}

// Fill all components
inline Color::Color(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

// Fill all components
inline Color::Color(const float f[3], float a)
{
    r = f[0];
    g = f[1];
    b = f[2];
    this->a = a;
}

// Fill all components
inline Color::Color(const Vector &v, float a)
{
    r = v.x;
    g = v.y;
    b = v.z;
    this->a = a;
}

// Fill all components
inline Color::Color(const Vector4 &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
}

// Unpack
inline Color::Color(uint32_t c)
{
    *this = c;
}

// Unpack
inline Color::Color(int32_t c)
{
    *this = static_cast<uint32_t>(c);
}

// Copy constructor
inline Color::Color(const Color &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
}

// ===========================================================
// Operators
// ===========================================================

// Get rgb intensity
inline float Color::operator~() const
{
    return GetIntensity();
}

// Return color with constrained components 0..1
inline Color Color::operator!() const
{
    auto c(*this);
    c.Clamp();
    return c;
}

// Get negative color
inline Color Color::operator-() const
{
    auto c(*this);
    c.r = -c.r;
    c.g = -c.g;
    c.b = -c.b;
    c.a = -c.a;
    return c;
}

// Assign rgb
inline Color &Color::operator=(float f)
{
    r = f;
    g = f;
    b = f;
    a = 1.0f;
    return *this;
}

// Assign rgb
inline Color &Color::operator=(const Vector &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = 1.0f;
    return *this;
}

// Assign
inline Color &Color::operator=(const Vector4 &v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
    return *this;
}

// Unpack and assign
inline Color &Color::operator=(uint32_t c)
{
    r = static_cast<unsigned char>(c >> 16) * (1.0f / 255.0f);
    g = static_cast<unsigned char>(c >> 8) * (1.0f / 255.0f);
    b = static_cast<unsigned char>(c >> 0) * (1.0f / 255.0f);
    a = static_cast<unsigned char>(c >> 24) * (1.0f / 255.0f);
    return *this;
}

// Assign
inline Color &Color::operator=(const Color &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
}

// per component addition with rgb assignment
inline Color &Color::operator+=(float f)
{
    r += f;
    g += f;
    b += f;
    return *this;
}

// Unpack and add
inline Color &Color::operator+=(uint32_t c)
{
    const Color clr(c);
    *this += clr;
    return *this;
}

// per component addition with rgb assignment
inline Color &Color::operator+=(const Vector &v)
{
    r += v.x;
    g += v.y;
    b += v.z;
    return *this;
}

// per component addition with assignment
inline Color &Color::operator+=(const Vector4 &v)
{
    r += v.x;
    g += v.y;
    b += v.z;
    a += v.w;
    return *this;
}

// per component addition with assignment
inline Color &Color::operator+=(const Color &c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

// per component subtraction with rgb assignment
inline Color &Color::operator-=(float f)
{
    r -= f;
    g -= f;
    b -= f;
    return *this;
}

// Unpack and subtract
inline Color &Color::operator-=(uint32_t c)
{
    const Color clr(c);
    *this = clr;
    return *this;
}

// per component subtraction with rgb assignment
inline Color &Color::operator-=(const Vector &v)
{
    r -= v.x;
    g -= v.y;
    b -= v.z;
    return *this;
}

// per component subtraction with assignment
inline Color &Color::operator-=(const Vector4 &v)
{
    r -= v.x;
    g -= v.y;
    b -= v.z;
    a -= v.w;
    return *this;
}

// per component subtraction with assignment
inline Color &Color::operator-=(const Color &c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}

// per component multiplication with rgb assignment
inline Color &Color::operator*=(float f)
{
    r *= f;
    g *= f;
    b *= f;
    return *this;
}

// Unpack and multiply
inline Color &Color::operator*=(uint32_t c)
{
    const Color clr(c);
    *this *= clr;
    return *this;
}

// per component multiplication with rgb assignment
inline Color &Color::operator*=(const Vector &v)
{
    r *= v.x;
    g *= v.y;
    b *= v.z;
    return *this;
}

// per component multiplication with assignment
inline Color &Color::operator*=(const Vector4 &v)
{
    r *= v.x;
    g *= v.y;
    b *= v.z;
    a *= v.w;
    return *this;
}

// per component multiplication with assignment
inline Color &Color::operator*=(const Color &c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}

// per component division with rgb assignment
inline Color &Color::operator/=(float f)
{
    f = 1.0f / f;
    r *= f;
    g *= f;
    b *= f;
    return *this;
}

// Unpack and divide
inline Color &Color::operator/=(uint32_t c)
{
    const Color clr(c);
    *this /= clr;
    return *this;
}

// per component division with rgb assignment
inline Color &Color::operator/=(const Vector &v)
{
    r /= v.x;
    g /= v.y;
    b /= v.z;
    return *this;
}

// per component division with assignment
inline Color &Color::operator/=(const Vector4 &v)
{
    r /= v.x;
    g /= v.y;
    b /= v.z;
    a /= v.w;
    return *this;
}

// per component division with assignment
inline Color &Color::operator/=(const Color &c)
{
    r /= c.r;
    g /= c.g;
    b /= c.b;
    a /= c.a;
    return *this;
}

// Scalar rgb multiplication, the result is copied to all components
inline Color &Color::operator|=(const Color &c)
{
    r = g = b = a = r * c.r + g * c.g + b * c.b;
    return *this;
}

// Get packed color as long
inline Color::operator uint32_t() const
{
    auto c(*this);
    c.Clamp();
    return static_cast<long>(c.GetDword());
}

/*!\relates Color
per component addition with rgb assignment
*/
inline Color operator+(const Color &c, float f)
{
    auto clr(c);
    clr += f;
    return clr;
}

/*!\relates Color
per component addition with rgb assignment
*/
inline Color operator+(float f, const Color &c)
{
    auto clr(c);
    clr += f;
    return clr;
}

/*!\relates Color
Unpack and add
*/
inline Color operator+(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr += cl;
    return clr;
}

/*!\relates Color
Unpack and add
*/
inline Color operator+(uint32_t cl, const Color &c)
{
    auto clr(c);
    clr += cl;
    return clr;
}

/*!\relates Color
per component addition with rgb assignment
*/
inline Color operator+(const Color &c, const Vector &v)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
per component addition with rgb assignment
*/
inline Color operator+(const Vector &v, const Color &c)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
per component addition with assignment
*/
inline Color operator+(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
per component addition with assignment
*/
inline Color operator+(const Vector4 &v, const Color &c)
{
    auto clr(c);
    clr += v;
    return clr;
}

/*!\relates Color
per component addition with assignment
*/
inline Color operator+(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr += c2;
    return clr;
}

/*!\relates Color
per component subtraction with rgb assignment
*/
inline Color operator-(const Color &c, float f)
{
    auto clr(c);
    clr -= f;
    return clr;
}

/*!\relates Color
per component subtraction with rgb assignment
*/
inline Color operator-(float f, const Color &c)
{
    Color clr(f);
    clr -= f;
    return clr;
}

/*!\relates Color
Unpack and subtract
*/
inline Color operator-(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr -= cl;
    return clr;
}

/*!\relates Color
Unpack and subtract
*/
inline Color operator-(uint32_t cl, const Color &c)
{
    Color clr(cl);
    clr -= c;
    return clr;
}

/*!\relates Color
per component subtraction with rgb assignment
*/
inline Color operator-(const Color &c, const Vector &v)
{
    auto clr(c);
    clr -= v;
    return clr;
}

/*!\relates Color
per component subtraction with rgb assignment
*/
inline Color operator-(const Vector &v, const Color &c)
{
    Color clr(v);
    clr -= c;
    return clr;
}

/*!\relates Color
per component subtraction with assignment
*/
inline Color operator-(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr -= v;
    return clr;
}

/*!\relates Color
per component subtraction with assignment
*/
inline Color operator-(const Vector4 &v, const Color &c)
{
    Color clr(v);
    clr -= c;
    return clr;
}

/*!\relates Color
per component subtraction with assignment
*/
inline Color operator-(const Color &c1, const Color &c2)
{
    auto c(c1);
    c -= c2;
    return c;
}

/*!\relates Color
per component multiplication with rgb assignment
*/
inline Color operator*(const Color &c, float f)
{
    auto clr(c);
    clr *= f;
    return clr;
}

/*!\relates Color
per component multiplication with rgb assignment
*/
inline Color operator*(float f, const Color &c)
{
    auto clr(c);
    clr *= f;
    return clr;
}

/*!\relates Color
Unpack and multiply
*/
inline Color operator*(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr *= cl;
    return clr;
}

/*!\relates Color
Unpack and multiply
*/
inline Color operator*(uint32_t cl, const Color &c)
{
    auto clr(c);
    clr *= cl;
    return clr;
}

/*!\relates Color
per component multiplication with rgb assignment
*/
inline Color operator*(const Color &c, const Vector &v)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
per component multiplication with rgb assignment
*/
inline Color operator*(const Vector &v, const Color &c)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
per component multiplication with assignment
*/
inline Color operator*(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
per component multiplication with assignment
*/
inline Color operator*(const Vector4 &v, const Color &c)
{
    auto clr(c);
    clr *= v;
    return clr;
}

/*!\relates Color
per component multiplication with assignment
*/
inline Color operator*(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr *= c2;
    return clr;
}

/*!\relates Color
per component division with rgb assignment
*/
inline Color operator/(const Color &c, float f)
{
    auto clr(c);
    clr /= f;
    return clr;
}

/*!\relates Color
per component division with rgb assignment
*/
inline Color operator/(float f, const Color &c)
{
    Color clr(f);
    clr /= c;
    return clr;
}

/*!\relates Color
Unpack and divide
*/
inline Color operator/(const Color &c, uint32_t cl)
{
    auto clr(c);
    clr /= cl;
    return clr;
}

/*!\relates Color
Unpack and divide
*/
inline Color operator/(uint32_t cl, const Color &c)
{
    Color clr(cl);
    clr /= c;
    return clr;
}

/*!\relates Color
per component division with rgb assignment
*/
inline Color operator/(const Color &c, const Vector &v)
{
    auto clr(c);
    clr /= v;
    return clr;
}

/*!\relates Color
per component division with rgb assignment
*/
inline Color operator/(const Vector &v, const Color &c)
{
    Color clr(v);
    clr /= c;
    return clr;
}

/*!\relates Color
per component division with assignment
*/
inline Color operator/(const Color &c, const Vector4 &v)
{
    auto clr(c);
    clr /= v;
    return clr;
}

/*!\relates Color
per component division with assignment
*/
inline Color operator/(const Vector4 &v, const Color &c)
{
    Color clr(v);
    clr /= c;
    return clr;
}

/*!\relates Color
per component division with assignment
*/
inline Color operator/(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr /= c2;
    return clr;
}

/*!\relates Color
Scalar rgb multiplication, the result is copied to all components
*/
inline Color operator|(const Color &c1, const Color &c2)
{
    auto clr(c1);
    clr |= c2;
    return clr;
}

// ===========================================================
// Transformation
// ===========================================================

// Limit to range 0..1
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

// Limit to range
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

// Save minimal components
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

// Save maximum components
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

// Brightness -1..1
inline void Color::Brightness(float br)
{
    if (br > 1.0f)
        br = 1.0f;
    if (br < -1.0f)
        br = -1.0f;
    *this += br;
    Clamp();
}

// Gamma -1..1
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

// Contrast -1..1
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

// Change brightness, gamma and contrast
inline void Color::Processed(float br, float gm, float cn)
{
    Contrast(cn);
    Gamma(gm);
    Brightness(br);
}

// ===========================================================
// Utilities
// ===========================================================

// Get intensity
inline float Color::GetIntensity() const
{
    return r * 0.299f + g * 0.587f + b * 0.114f;
}

// Normalize rgb
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

// Get color distance between colors
inline float Color::GetDistance(const Color &c) const
{
    return sqrtf(GetDistanceSqr(c));
}

// Get the color distance between colors squared
inline float Color::GetDistanceSqr(const Color &c) const
{
    const auto dr = (r - c.r) * 0.299f;
    const auto dg = (g - c.g) * 0.587f;
    const auto db = (b - c.b) * 0.114f;
    return dr * dr + dg * dg + db * db;
}

// Calculate linearly interpolated value
inline Color &Color::Lerp(const Color &c1, const Color &c2, float kBlend)
{
    r = c1.r + (c2.r - c1.r) * kBlend;
    g = c1.g + (c2.g - c1.g) * kBlend;
    b = c1.b + (c2.b - c1.b) * kBlend;
    return *this;
}

// Calculate linearly interpolated value
inline Color &Color::LerpA(const Color &c1, const Color &c2, float kBlend)
{
    r = c1.r + (c2.r - c1.r) * kBlend;
    g = c1.g + (c2.g - c1.g) * kBlend;
    b = c1.b + (c2.b - c1.b) * kBlend;
    a = c1.a + (c2.a - c1.a) * kBlend;
    return *this;
}

// Multiply color by number
inline Color &Color::MulColor(float k)
{
    r *= k;
    g *= k;
    b *= k;
    return *this;
}

// Multiply alpha by number
inline Color &Color::MulAlpha(float k)
{
    a *= k;
    return *this;
}

// Swap r, b
inline Color &Color::SwapRB()
{
    const auto t = r;
    r = b;
    b = t;
    return *this;
}

// Get packed color as uint32_t
inline uint32_t Color::GetDword() const
{
    /*    long l;*/
    DColor color;
    const auto k = 255.0f;

    color.r = static_cast<uint8_t>(fftoi(r * k));
    color.g = static_cast<uint8_t>(fftoi(g * k));
    color.b = static_cast<uint8_t>(fftoi(b * k));
    color.a = static_cast<uint8_t>(fftoi(a * k));

    return color.c;

    /*_asm
    {
      mov        eax, this
      fld        [eax]this.r
      fld        k
      fmul
      fistp    l
      mov        ebx, l
      mov        color.r, bl
      fld        [eax]this.g
      fld        k
      fmul
      fistp    l
      mov        ebx, l
      mov        color.g, bl
      fld        [eax]this.b
      fld        k
      fmul
      fistp    l
      mov        ebx, l
      mov        color.b, bl
      fld        [eax]this.a
      fld        k
      fmul
      fistp    l
      mov        ebx, l
      mov        color.a, bl
    };*/
}

// Converting A8R8G8B8 to R5G6B5
inline unsigned short Color::Make565(uint32_t color)
{
    //   11111000 11111100 11111000
    //           11111 111111 11111
    const auto b = (color >> 3) & 0x1f;
    const auto g = (color >> 5) & 0x7e0;
    const auto r = (color >> 8) & 0xf800;
    return static_cast<unsigned short>(r | g | b);
}

// Converting A8R8G8B8 to X1R5G5B5
inline unsigned short Color::Make555(uint32_t color)
{
    //   11111000 11111000 11111000
    //           011111 11111 11111
    const auto b = (color >> 3) & 0x1f;
    const auto g = (color >> 6) & 0x3e0;
    const auto r = (color >> 9) & 0x7c00;
    return static_cast<unsigned short>(r | g | b);
}

// Converting A8R8G8B8 to A1R5G5B5
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

// Converting A8R8G8B8 to A1R5G5B5
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

// ===========================================================
// DColor
// ===========================================================

// Assign
inline DColor &DColor::operator=(uint32_t color)
{
    c = color;
    return *this;
}

// Assign
inline DColor &DColor::operator=(long color)
{
    c = static_cast<uint32_t>(color);
    return *this;
}

// Get long
inline DColor::operator uint32_t() const
{
    return static_cast<long>(c);
}

#pragma pack(pop)
