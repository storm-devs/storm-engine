#pragma once
struct Vectord
{
  public:
    double x;
    double y;
    double z;

    Vectord(){};

    Vectord(double a)
    {
        x = a;
        y = a;
        z = a;
    };

    Vectord(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    };

    Vectord(double a[3])
    {
        x = a[0];
        y = a[1];
        z = a[2];
    };

    Vectord(const Vector &d)
    {
        x = d.x;
        y = d.y;
        z = d.z;
    };

    Vectord &operator+=(const Vectord &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    };

    Vectord &operator-=(const Vectord &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    };

    Vectord &operator*=(const Vectord &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    };

    Vectord &operator/=(const Vectord &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    };

    // square magnitude
    friend double operator~(const Vectord &v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    };

    // normalize
    friend Vectord operator!(const Vectord &v)
    {
        const auto len = 1.0 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return Vectord(v.x * len, v.y * len, v.z * len);
    };

    // none
    friend Vectord operator+(const Vectord &v)
    {
        return v;
    };
    // add
    friend Vectord operator+(const Vectord &v1, const Vectord &v2)
    {
        return Vectord(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    };
    // negative
    friend Vectord operator-(const Vectord &v)
    {
        return Vectord(-v.x, -v.y, -v.z);
    };
    // sub
    friend Vectord operator-(const Vectord &v1, const Vectord &v2)
    {
        return Vectord(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    };
    // multiply each element by each
    friend Vectord operator*(const Vectord &v1, const Vectord &v2)
    {
        return Vectord(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    };
    // multiply each element by double
    friend Vectord operator*(const Vectord &v1, double f)
    {
        return Vectord(v1.x * f, v1.y * f, v1.z * f);
    };
    // divide each element by each
    friend Vectord operator/(const Vectord &v1, const Vectord &v2)
    {
        return Vectord(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    };
    // divide each element by double
    friend Vectord operator/(const Vectord &v1, double f)
    {
        const auto _f = 1.0 / f;
        return Vectord(v1.x * _f, v1.y * _f, v1.z * _f);
    };
    // cross product
    friend Vectord operator^(const Vectord &v1, const Vectord &v2)
    {
        return Vectord(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    // dot product
    friend double operator|(const Vectord &v1, const Vectord &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
};
