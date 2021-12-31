#pragma once

#include <math.h>

struct CVECTOR
{
  public:
    union {
        struct
        {
            float x, y, z;
        };

        float v[3];
    };

    CVECTOR() = default;

    CVECTOR(float a)
    {
        x = a;
        y = a;
        z = a;
    };

    CVECTOR(double a)
    {
        x = static_cast<float>(a);
        y = static_cast<float>(a);
        z = static_cast<float>(a);
    };

    CVECTOR(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    };

    CVECTOR(double _x, double _y, double _z)
    {
        x = static_cast<float>(_x);
        y = static_cast<float>(_y);
        z = static_cast<float>(_z);
    };

    CVECTOR(float a[3])
    {
        x = a[0];
        y = a[1];
        z = a[2];
    };

    CVECTOR(double a[3])
    {
        x = static_cast<float>(a[0]);
        y = static_cast<float>(a[1]);
        z = static_cast<float>(a[2]);
    };
    // CVECTOR(const DVECTOR &d) : x((float)d.x), y((float)d.y), z((float)d.z){};

    CVECTOR &operator+=(const CVECTOR &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    };

    CVECTOR &operator-=(const CVECTOR &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    };

    CVECTOR &operator*=(const CVECTOR &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    };

    CVECTOR &operator/=(const CVECTOR &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    };

    // square magnitude
    friend float operator~(const CVECTOR &v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    };

    float GetLength() const
    {
        return static_cast<float>(sqrt(static_cast<double>(x) * static_cast<double>(x) +
                                       static_cast<double>(y) * static_cast<double>(y) +
                                       static_cast<double>(z) * static_cast<double>(z)));
    }

    float GetLength2D() const
    {
        return static_cast<float>(
            sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z)));
    }

    // normalize
    friend CVECTOR operator!(const CVECTOR &v)
    {
        double len = v.x * v.x + v.y * v.y + v.z * v.z;
        if (len == 0.0)
            return CVECTOR(0.0f);
        len = 1.0 / sqrt(len);
        return CVECTOR(v.x * len, v.y * len, v.z * len);
    };

    // none
    friend CVECTOR operator+(const CVECTOR &v)
    {
        return v;
    };
    // add
    friend CVECTOR operator+(const CVECTOR &v1, const CVECTOR &v2)
    {
        return CVECTOR(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    };
    // negative
    friend CVECTOR operator-(const CVECTOR &v)
    {
        return CVECTOR(-v.x, -v.y, -v.z);
    };
    // sub
    friend CVECTOR operator-(const CVECTOR &v1, const CVECTOR &v2)
    {
        return CVECTOR(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    };
    // multiply each element by each
    friend CVECTOR operator*(const CVECTOR &v1, const CVECTOR &v2)
    {
        return CVECTOR(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    };
    // multiply each element by float
    friend CVECTOR operator*(const CVECTOR &v1, float f)
    {
        return CVECTOR(v1.x * f, v1.y * f, v1.z * f);
    };
    // divide each element by each
    friend CVECTOR operator/(const CVECTOR &v1, const CVECTOR &v2)
    {
        return CVECTOR(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    };
    // divide each element by float
    friend CVECTOR operator/(const CVECTOR &v1, float f)
    {
        const auto _f = 1.0 / f;
        return CVECTOR(v1.x * _f, v1.y * _f, v1.z * _f);
    };
    // cross product
    friend CVECTOR operator^(const CVECTOR &v1, const CVECTOR &v2)
    {
        return CVECTOR(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    // dot product
    friend float operator|(const CVECTOR &v1, const CVECTOR &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
};

struct DVECTOR
{
  public:
    double x;
    double y;
    double z;

    DVECTOR(){};

    DVECTOR(double a)
    {
        x = a;
        y = a;
        z = a;
    };

    DVECTOR(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    };

    DVECTOR(double a[3])
    {
        x = a[0];
        y = a[1];
        z = a[2];
    };

    DVECTOR(const CVECTOR &d)
    {
        x = d.x;
        y = d.y;
        z = d.z;
    };

    DVECTOR &operator+=(const DVECTOR &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    };

    DVECTOR &operator-=(const DVECTOR &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    };

    DVECTOR &operator*=(const DVECTOR &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    };

    DVECTOR &operator/=(const DVECTOR &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    };

    // square magnitude
    friend double operator~(const DVECTOR &v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    };

    // normalize
    friend DVECTOR operator!(const DVECTOR &v)
    {
        const auto len = 1.0 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return DVECTOR(v.x * len, v.y * len, v.z * len);
    };

    // none
    friend DVECTOR operator+(const DVECTOR &v)
    {
        return v;
    };
    // add
    friend DVECTOR operator+(const DVECTOR &v1, const DVECTOR &v2)
    {
        return DVECTOR(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    };
    // negative
    friend DVECTOR operator-(const DVECTOR &v)
    {
        return DVECTOR(-v.x, -v.y, -v.z);
    };
    // sub
    friend DVECTOR operator-(const DVECTOR &v1, const DVECTOR &v2)
    {
        return DVECTOR(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    };
    // multiply each element by each
    friend DVECTOR operator*(const DVECTOR &v1, const DVECTOR &v2)
    {
        return DVECTOR(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    };
    // multiply each element by double
    friend DVECTOR operator*(const DVECTOR &v1, double f)
    {
        return DVECTOR(v1.x * f, v1.y * f, v1.z * f);
    };
    // divide each element by each
    friend DVECTOR operator/(const DVECTOR &v1, const DVECTOR &v2)
    {
        return DVECTOR(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    };
    // divide each element by double
    friend DVECTOR operator/(const DVECTOR &v1, double f)
    {
        const auto _f = 1.0 / f;
        return DVECTOR(v1.x * _f, v1.y * _f, v1.z * _f);
    };
    // cross product
    friend DVECTOR operator^(const DVECTOR &v1, const DVECTOR &v2)
    {
        return DVECTOR(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    // dot product
    friend double operator|(const DVECTOR &v1, const DVECTOR &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
};
