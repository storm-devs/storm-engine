//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Quaternion
//============================================================================================

#pragma once

#include "Matrix.h"
#include "Vector.h"

//============================================================================================

struct D3DXQUATERNION;
#pragma pack(push, 1)

//============================================================================================

// Quaternion representation class
class Quaternion
{
  public:
    union {
        struct
        {
            // X vector component
            float x;
            // Y vector component
            float y;
            // Z Vector component
            float z;
            // Scalar component
            float w;
        };

        // Array representation
        float q[4];
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Set identity quaternion
    Quaternion();
    // With rotation angles
    Quaternion(float ax, float ay, float az);
    // With rotation angles
    Quaternion(const Vector &ang);
    // From components
    Quaternion(float qx, float qy, float qz, float qw);
    // From the matrix
    Quaternion(const Matrix &mtx);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Negation
    Quaternion &operator-();
    // Equate
    Quaternion &operator=(const Quaternion &q);
    // Addition
    Quaternion &operator+=(const Quaternion &q);
    // Addition
    Quaternion &operator+=(float f);
    // Subtract
    Quaternion &operator-=(const Quaternion &q);
    // Subtract
    Quaternion &operator-=(float f);
    // Multiply
    Quaternion &operator*=(float f);
    // Multiply
    Quaternion &operator*=(const Quaternion &q);
    // Divide
    Quaternion &operator/=(float f);

    // -----------------------------------------------------------
    // Filling
    // -----------------------------------------------------------
  public:
    // Set identity quaternion
    void SetIdentity();
    // Set from angles of rotation
    void Set(float ax, float ay, float az);
    // Set from angles of rotation
    void Set(const Vector &ang);
    // Construct from components
    void Set(float qx, float qy, float qz, float qw);
    // Construct from matrix
    void Set(const Matrix &mtx);

    // -----------------------------------------------------------
    // Transformation
    // -----------------------------------------------------------
  public:
    // Normalize
    Quaternion &Normalize();
    // Get coplanar quaternion
    Quaternion &Conjugate();
    // Invert quaternion
    Quaternion &Inverse();

    // Spherical linear interpolation
    Quaternion &SLerp(const Quaternion &q1, const Quaternion &q2, float kBlend);

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Get a matrix from a quaternion
    void GetMatrix(Matrix &mtx) const;
    // Get the length of the quaternion
    float GetLength() const;
    // Get the square of the length of a quaternion
    float GetLengthSqr() const;
};

// ===========================================================
// Constructors
// ===========================================================

// Set identity quaternion
inline Quaternion::Quaternion()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

// From roration angles
inline Quaternion::Quaternion(float ax, float ay, float az)
{
    Set(ax, ay, az);
}

// From rotation angles
inline Quaternion::Quaternion(const Vector &ang)
{
    Set(ang);
}

// From components
inline Quaternion::Quaternion(float qx, float qy, float qz, float qw)
{
    Set(qx, qy, qz, qw);
}

// From a matrix
inline Quaternion::Quaternion(const Matrix &mtx)
{
    Set(mtx);
}

// ===========================================================
// Operators
// ===========================================================

// Negation
inline Quaternion &Quaternion::operator-()
{
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    return *this;
}

// Equate
inline Quaternion &Quaternion::operator=(const Quaternion &q)
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

// Addition
inline Quaternion &Quaternion::operator+=(const Quaternion &q)
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

// Addition
inline Quaternion &Quaternion::operator+=(float f)
{
    x += f;
    y += f;
    z += f;
    w += f;
    return *this;
}

// Subtract
inline Quaternion &Quaternion::operator-=(const Quaternion &q)
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

// Subtract
inline Quaternion &Quaternion::operator-=(float f)
{
    x -= f;
    y -= f;
    z -= f;
    w -= f;
    return *this;
}

// Multiply
inline Quaternion &Quaternion::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

// Multiply
inline Quaternion &Quaternion::operator*=(const Quaternion &q)
{
    const auto a = (w + x) * (q.w + q.x);
    const auto b = (z - y) * (q.y - q.z);
    const auto c = (x - w) * (q.y - q.z);
    const auto d = (y + z) * (q.x - q.w);
    const auto e = (x + z) * (q.x + q.y);
    const auto f = (x - z) * (q.x - q.y);
    const auto g = (w + y) * (q.w - q.z);
    const auto h = (w - y) * (q.w + q.z);
    x = a - (e + f + g + h) * 0.5f;
    y = -c + (e - f + g - h) * 0.5f;
    z = -d + (e - f - g + h) * 0.5f;
    w = b + (-e - f + g + h) * 0.5f;
    return *this;
}

// Divide
inline Quaternion &Quaternion::operator/=(float f)
{
    const auto k = 1.0 / f;
    x = static_cast<float>(x * k);
    y = static_cast<float>(y * k);
    z = static_cast<float>(z * k);
    w = static_cast<float>(w * k);
    return *this;
}

/*!\relates Quaternion
Addition
*/
inline Quaternion operator+(float f, const Quaternion &q)
{
    auto tq(q);
    tq += f;
    return tq;
}

/*!\relates Quaternion
Addition
*/
inline Quaternion operator+(const Quaternion &q, float f)
{
    auto tq(q);
    tq += f;
    return tq;
}

/*!\relates Quaternion
Addition
*/
inline Quaternion operator+(const Quaternion &q1, const Quaternion &q2)
{
    auto tq(q1);
    tq += q2;
    return tq;
}

/*!\relates Quaternion
Subtract
*/
inline Quaternion operator-(float f, const Quaternion &q)
{
    auto tq(q);
    tq -= f;
    return tq;
}

/*!\relates Quaternion
Subtract
*/
inline Quaternion operator-(const Quaternion &q, float f)
{
    auto tq(q);
    tq -= f;
    return tq;
}

/*!\relates Quaternion
Subtract
*/
inline Quaternion operator-(const Quaternion &q1, const Quaternion &q2)
{
    auto tq(q1);
    tq -= q2;
    return tq;
}

/*!\relates Quaternion
Multiply
*/
inline Quaternion operator*(float f, const Quaternion &q)
{
    auto tq(q);
    tq *= f;
    return tq;
}

/*!\relates Quaternion
Multiply
*/
inline Quaternion operator*(const Quaternion &q, float f)
{
    auto tq(q);
    tq *= f;
    return tq;
}

/*!\relates Quaternion
Multiply
*/
inline Quaternion operator*(const Quaternion &q1, const Quaternion &q2)
{
    auto tq(q1);
    tq *= q2;
    return tq;
}

/*!\relates Quaternion
Divide
*/
inline Quaternion operator/(const Quaternion &q, float f)
{
    auto tq(q);
    tq /= f;
    return tq;
}

// ===========================================================
// Filling
// ===========================================================

// Set identity quaternion
inline void Quaternion::SetIdentity()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

// Construct from angles of rotation
inline void Quaternion::Set(float ax, float ay, float az)
{
    Set(Matrix(ax, ay, az));
}

// Construct from angles of rotation
inline void Quaternion::Set(const Vector &ang)
{
    Set(ang.x, ang.y, ang.z);
}

// Construct from component
inline void Quaternion::Set(float qx, float qy, float qz, float qw)
{
    x = qx;
    y = qy;
    z = qz;
    w = qw;
}

// Construct from matrix
inline void Quaternion::Set(const Matrix &mtx)
{
    const auto tr = mtx.m[0][0] + mtx.m[1][1] + mtx.m[2][2];
    if (tr > 0.0f)
    {
        double s = sqrt(tr + 1.0f);
        w = static_cast<float>(s * 0.5);
        s = 0.5 / s;
        x = static_cast<float>((mtx.m[1][2] - mtx.m[2][1]) * s);
        y = static_cast<float>((mtx.m[2][0] - mtx.m[0][2]) * s);
        z = static_cast<float>((mtx.m[0][1] - mtx.m[1][0]) * s);
    }
    else
    {
        long i, j, k;
        if (mtx.m[1][1] > mtx.m[0][0])
        {
            if (mtx.m[2][2] > mtx.m[1][1])
            {
                i = 2;
                j = 0;
                k = 1;
            }
            else
            {
                i = 1;
                j = 2;
                k = 0;
            }
        }
        else if (mtx.m[2][2] > mtx.m[0][0])
        {
            i = 2;
            j = 0;
            k = 1;
        }
        else
        {
            i = 0;
            j = 1;
            k = 2;
        }
        auto s = sqrt((mtx.m[i][i] - (mtx.m[j][j] + mtx.m[k][k])) + 1.0);
        q[i] = static_cast<float>(s * 0.5);
        if (s != 0.0)
            s = 0.5 / s;
        q[j] = static_cast<float>((mtx.m[i][j] + mtx.m[j][i]) * s);
        q[k] = static_cast<float>((mtx.m[i][k] + mtx.m[k][i]) * s);
        q[3] = static_cast<float>((mtx.m[j][k] - mtx.m[k][j]) * s);
    }
}

// ===========================================================
// Transformation
// ===========================================================

// Normalize
inline Quaternion &Quaternion::Normalize()
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) +
               static_cast<double>(z) * static_cast<double>(z) + static_cast<double>(w) * static_cast<double>(w);
    if (len <= 0.0)
        return *this;
    len = 1.0 / sqrt(len);
    x = static_cast<float>(x * len);
    y = static_cast<float>(y * len);
    z = static_cast<float>(z * len);
    w = static_cast<float>(w * len);
    return *this;
}

// Get coplanar quaternion
inline Quaternion &Quaternion::Conjugate()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

// Invert quaternion
inline Quaternion &Quaternion::Inverse()
{
    Conjugate();
    Normalize();
    return *this;
}

// Spherical linear interpolation
inline Quaternion &Quaternion::SLerp(const Quaternion &q1, const Quaternion &q2, float kBlend)
{
    // The angle between the quaternions
    double cosomega = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    double k = 1.0f;
    // Nearest direction
    if (cosomega < 0.0f)
    {
        cosomega = -cosomega;
        k = -1.0f;
    }
    if (cosomega > 1.0)
        cosomega = 1.0;
    // Interpolation coefficients
    double k0, k1;
    if (1.0 - cosomega > 0.1)
    {
        // slerp
        const auto omega = acos(cosomega);
        const auto sinomega = 1.0 / sin(omega);
        k0 = sin(omega * (1.0 - kBlend)) * sinomega;
        k1 = sin(omega * kBlend) * sinomega;
    }
    else
    {
        // lerp
        k0 = 1.0 - kBlend;
        k1 = kBlend;
    }
    k0 *= k;
    // Interpolate
    x = static_cast<float>(q1.x * k0 + q2.x * k1);
    y = static_cast<float>(q1.y * k0 + q2.y * k1);
    z = static_cast<float>(q1.z * k0 + q2.z * k1);
    w = static_cast<float>(q1.w * k0 + q2.w * k1);
    return *this;
}

// ===========================================================
// Utilities
// ===========================================================

// Get a matrix from a quaternion
inline void Quaternion::GetMatrix(Matrix &mtx) const
{
    const auto xx = x * x * 2.0f, xy = x * y * 2.0f, xz = x * z * 2.0f;
    const auto yy = y * y * 2.0f, yz = y * z * 2.0f;
    const auto zz = z * z * 2.0f;
    const auto wx = w * x * 2.0f, wy = w * y * 2.0f, wz = w * z * 2.0f;
    mtx.m[0][0] = 1.0f - (yy + zz);
    mtx.m[0][1] = xy + wz;
    mtx.m[0][2] = xz - wy;
    mtx.m[0][3] = 0.0f;
    mtx.m[1][0] = xy - wz;
    mtx.m[1][1] = 1.0f - (xx + zz);
    mtx.m[1][2] = yz + wx;
    mtx.m[1][3] = 0.0f;
    mtx.m[2][0] = xz + wy;
    mtx.m[2][1] = yz - wx;
    mtx.m[2][2] = 1.0f - (xx + yy);
    mtx.m[2][3] = 0.0f;
    mtx.m[3][0] = 0.0f;
    mtx.m[3][1] = 0.0f;
    mtx.m[3][2] = 0.0f;
    mtx.m[3][3] = 1.0f;
}

// Get the length of the quaternion
inline float Quaternion::GetLength() const
{
    return sqrtf(x * x + y * y + z * z + w * w);
}

// Get the square of the length of a quaternion
inline float Quaternion::GetLengthSqr() const
{
    return x * x + y * y + z * z + w * w;
}

#pragma pack(pop)
