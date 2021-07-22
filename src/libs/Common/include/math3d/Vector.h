//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Vector
//============================================================================================

#pragma once

#include <math.h>
#include <stdlib.h>

#pragma pack(push, 1)

// Class for representing a vector in 3D space
class Vector
{
  public:
    union {
        struct
        {
            // The X component
            float x;
            // The Y component
            float y;
            // The Z component
            float z;
        };

        // Array representation
        float v[3];
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Empty constructor
    Vector();
    // Fill with number
    Vector(float f);
    // Fill with number
    Vector(double d);
    // Fill all components
    Vector(float x, float y, float z);
    // Fill all components
    Vector(const float f[3]);
    // Fill all components
    Vector(const double d[3]);
    // Copy constructor
    Vector(const Vector &v);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Find the square of the length of a vector
    float operator~() const;
    // Return normalized vector
    Vector operator!() const;

    // Get negative vector
    Vector operator-() const;

    // Assign
    Vector &operator=(float f);
    // Assign
    Vector &operator=(double d);
    // Assign
    Vector &operator=(const Vector &v);
    // per Component addition with assignment
    Vector &operator+=(float f);
    // per Component addition with assignment
    Vector &operator+=(double d);
    // per Component addition with assignment
    Vector &operator+=(const Vector &v);
    // per Component Subtraction with Assignment
    Vector &operator-=(float f);
    // per Component Subtraction with Assignment
    Vector &operator-=(double d);
    // per Component Subtraction with Assignment
    Vector &operator-=(const Vector &v);
    // per Component multiplication with assignment
    Vector &operator*=(float f);
    // per Component multiplication with assignment
    Vector &operator*=(double d);
    // per Component multiplication with assignment
    Vector &operator*=(const Vector &v);
    // per Component division with assignment
    Vector &operator/=(float f);
    // per Component division with assignment
    Vector &operator/=(double d);
    // per Component division with assignment
    Vector &operator/=(const Vector &v);

    // Scalar multiplication, the result is copied to all components
    Vector &operator|=(const Vector &v);
    // Vector multiplication
    Vector &operator^=(const Vector &v);

    // -----------------------------------------------------------
    // Transformation
    // -----------------------------------------------------------
  public:
    // Normalize the vector, and return it to its former length
    float Normalize();
    // Project to XZ Plane and Normalize
    float Normalize2D();

    // Limit the length of the vector, and return the current length
    float ClampLength(float clampValue);

    // Limit X component to range
    Vector &ClampX(float min, float max);
    // Constrain Y component to range
    Vector &ClampY(float min, float max);
    // Constrain Z component to range
    Vector &ClampZ(float min, float max);
    // Limit components to a range
    Vector &Clamp(float min, float max);

    // Keep vector minimal components
    Vector &Min(const Vector &v);
    // Keep vector maximum components
    Vector &Max(const Vector &v);

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Set new values
    Vector &Set(float x, float y, float z);
    // Set new values
    Vector &Set(const Vector &v);

    // Get the angle between vectors
    float GetAngle(const Vector &v) const;
    // Get the sign angle between vectors in the XZ plane
    float GetAngle2D(const Vector &v) const;
    // Get the angle of rotation of a vector around the Y axis
    float GetAY(float defAngle = 0.0f) const;

    // Get sine between 2D vectors in XZ plane
    float Sin2D(const Vector &v) const;
    // Get cosine between 2D vectors in XZ plane
    float Cos2D(const Vector &v) const;
    // Get the cross product of vectors in the XZ plane
    float Cross2D(const Vector &v) const;

    // Get a vector in the XZ plane
    Vector Get2D() const;
    // Get a vector in the XZ plane
    Vector Get2D(float y) const;
    // Generate vector in XZ plane by angle
    Vector &Make2D(float ay);

    // Get vector length
    float GetLength() const;
    // Get vector length in 2D
    float GetLength2D() const;
    // Get the squared length of a vector in 2D
    float GetLength2D2() const;

    // Rotate a vector in the XZ plane by an angle
    Vector &Rotate(float angle);
    // Rotate the vector in the XZ plane by an angle specified by cos, sin
    Vector &Rotate(float vcos, float vsin);
    // Rotate the vector clockwise in the XZ plane by an angle PI/2
    Vector &Rotate_PI2_CW();
    // Rotate the vector counterclockwise in the XZ plane by an angle PI/2
    Vector &Rotate_PI2_CCW();

    // Calculate linearly interpolated value
    Vector &Lerp(const Vector &v1, const Vector &v2, float kBlend);

    // Calculate the reflected vector
    Vector &Reflection(const Vector &normal);

    // Fill with a unit vector with a random direction
    Vector &Rand();
    // Fill with unit vector with random direction in XZ
    Vector &Rand2D();
    // Fill with random values in a given ABB
    Vector &Rand(const Vector &min, const Vector &max);
    // Fill with random values in a given sphere
    Vector &Rand(const Vector &pos, float radius);

    // Move the current vector to the specified point by the specified step
    bool MoveByStep(const Vector &to, float step);
    // Move the current vector to a given point by a given step in 2D
    bool MoveByStep2D(const Vector &to, float step);

    // The point is in the box
    bool InBox(const Vector &min, const Vector &max) const;
    // The point is in the sphere
    bool InSphere(const Vector &pos, float rad) const;
};

// ===========================================================
// Constructors
// ===========================================================

// Empty constructor
inline Vector::Vector() = default;

// Fill with number
inline Vector::Vector(float f)
{
    x = f;
    y = f;
    z = f;
}

// Fill with number
inline Vector::Vector(double d)
{
    x = static_cast<float>(d);
    y = static_cast<float>(d);
    z = static_cast<float>(d);
}

// Fill all components
inline Vector::Vector(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

// Fill all components
inline Vector::Vector(const float f[3])
{
    x = f[0];
    y = f[1];
    z = f[2];
}

// Fill all components
inline Vector::Vector(const double d[3])
{
    x = static_cast<float>(d[0]);
    y = static_cast<float>(d[1]);
    z = static_cast<float>(d[2]);
}

// Copy constructor
inline Vector::Vector(const Vector &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

// ===========================================================
// Operators
// ===========================================================

// Find the square of the length of a vector
inline float Vector::operator~() const
{
    return x * x + y * y + z * z;
}

// Return normalized vector
inline Vector Vector::operator!() const
{
    auto v(*this);
    auto length = v.Normalize();
    // assert(fabsf(length) > 1e-20);
    return v;
}

inline Vector Vector::operator-() const
{
    auto v(*this);
    v.x = -v.x;
    v.y = -v.y;
    v.z = -v.z;
    return v;
}

// Assign
inline Vector &Vector::operator=(float f)
{
    x = f;
    y = f;
    z = f;
    return *this;
}

// Assign
inline Vector &Vector::operator=(double d)
{
    x = static_cast<float>(d);
    y = static_cast<float>(d);
    z = static_cast<float>(d);
    return *this;
}

// Assign
inline Vector &Vector::operator=(const Vector &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

// per Component addition with assignment
inline Vector &Vector::operator+=(float f)
{
    x += f;
    y += f;
    z += f;
    return *this;
}

// per Component addition with assignment
inline Vector &Vector::operator+=(double d)
{
    x += static_cast<float>(d);
    y += static_cast<float>(d);
    z += static_cast<float>(d);
    return *this;
}

// per Component addition with assignment
inline Vector &Vector::operator+=(const Vector &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

// per Component Subtraction with Assignment
inline Vector &Vector::operator-=(float f)
{
    x -= f;
    y -= f;
    z -= f;
    return *this;
}

// per Component Subtraction with Assignment
inline Vector &Vector::operator-=(double d)
{
    x -= static_cast<float>(d);
    y -= static_cast<float>(d);
    z -= static_cast<float>(d);
    return *this;
}

// per Component Subtraction with Assignment
inline Vector &Vector::operator-=(const Vector &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

// per Component multiplication with assignment
inline Vector &Vector::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

// per Component multiplication with assignment
inline Vector &Vector::operator*=(double d)
{
    x *= static_cast<float>(d);
    y *= static_cast<float>(d);
    z *= static_cast<float>(d);
    return *this;
}

// per Component multiplication with assignment
inline Vector &Vector::operator*=(const Vector &v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

// per Component division with assignment
inline Vector &Vector::operator/=(float f)
{
    const auto d = 1.0 / f;
    x = static_cast<float>(x * d);
    y = static_cast<float>(y * d);
    z = static_cast<float>(z * d);
    return *this;
}

// per Component division with assignment
inline Vector &Vector::operator/=(double d)
{
    d = 1.0 / d;
    x = static_cast<float>(x * d);
    y = static_cast<float>(y * d);
    z = static_cast<float>(z * d);
    return *this;
}

// per Component division with assignment
inline Vector &Vector::operator/=(const Vector &v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
}

// Scalar multiplication, the result is copied to all components
inline Vector &Vector::operator|=(const Vector &v)
{
    x = y = z = v.x * x + v.y * y + v.z * z;
    return *this;
}

// Vector multiplication
inline Vector &Vector::operator^=(const Vector &v)
{
    const auto tx = y * v.z - z * v.y;
    const auto ty = z * v.x - x * v.z;
    const auto tz = x * v.y - y * v.x;
    x = tx;
    y = ty;
    z = tz;
    return *this;
}

/*!\relates Vector
Addition
*/
inline Vector operator+(const Vector &v, float f)
{
    auto tv(v);
    tv += f;
    return tv;
}

/*!\relates Vector
Addition
*/
inline Vector operator+(float f, const Vector &v)
{
    auto tv(v);
    tv += f;
    return tv;
}

/*!\relates Vector
Addition
*/
inline Vector operator+(const Vector &v, double d)
{
    auto tv(v);
    tv += d;
    return tv;
}

/*!\relates Vector
Addition
*/
inline Vector operator+(double d, const Vector &v)
{
    auto tv(v);
    tv += d;
    return tv;
}

/*!\relates Vector
Addition
*/
inline Vector operator+(const Vector &v1, const Vector &v2)
{
    auto tv(v1);
    tv += v2;
    return tv;
}

/*!\relates Vector
Subtract
*/
inline Vector operator-(const Vector &v, float f)
{
    auto tv(v);
    tv -= f;
    return tv;
}

/*!\relates Vector
Subtract
*/
inline Vector operator-(float f, const Vector &v)
{
    auto tv(v);
    tv -= f;
    return tv;
}

/*!\relates Vector
Subtract
*/
inline Vector operator-(const Vector &v, double d)
{
    auto tv(v);
    tv -= d;
    return tv;
}

/*!\relates Vector
Subtract
*/
inline Vector operator-(double d, const Vector &v)
{
    auto tv(v);
    tv -= d;
    return tv;
}

/*!\relates Vector
Subtract
*/
inline Vector operator-(const Vector &v1, const Vector &v2)
{
    auto tv(v1);
    tv -= v2;
    return tv;
}

/*!\relates Vector
Multiply
*/
inline Vector operator*(const Vector &v, float f)
{
    auto tv(v);
    tv *= f;
    return tv;
}

/*!\relates Vector
Multiply
*/
inline Vector operator*(float f, const Vector &v)
{
    auto tv(v);
    tv *= f;
    return tv;
}

/*!\relates Vector
Multiply
*/
inline Vector operator*(const Vector &v, double d)
{
    auto tv(v);
    tv *= d;
    return tv;
}

/*!\relates Vector
Multiply
*/
inline Vector operator*(double d, const Vector &v)
{
    auto tv(v);
    tv *= d;
    return tv;
}

/*!\relates Vector
Multiply
*/
inline Vector operator*(const Vector &v1, const Vector &v2)
{
    auto tv(v1);
    tv *= v2;
    return tv;
}

/*!\relates Vector
Divide
*/
inline Vector operator/(const Vector &v, float f)
{
    auto tv(v);
    tv /= f;
    return tv;
}

/*!\relates Vector
Divide
*/
inline Vector operator/(float f, const Vector &v)
{
    Vector tv(f);
    tv /= v;
    return tv;
}

/*!\relates Vector
Divide
*/
inline Vector operator/(const Vector &v, double d)
{
    auto tv(v);
    tv /= d;
    return tv;
}

/*!\relates Vector
Divide
*/
inline Vector operator/(double d, const Vector &v)
{
    Vector tv(d);
    tv /= v;
    return tv;
}

/*!\relates Vector
Divide
*/
inline Vector operator/(const Vector &v1, const Vector &v2)
{
    auto tv(v1);
    tv /= v2;
    return tv;
}

/*!\relates Vector
Scalar multiplication
*/
inline float operator|(const Vector &v1, const Vector &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/*!\relates Vector
Vector multiplication
*/
inline Vector operator^(const Vector &v1, const Vector &v2)
{
    Vector v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator>(const Vector &v1, const Vector &v2)
{
    if (v1.x > v2.x && v1.y > v2.y && v1.z > v2.z)
        return true;
    return false;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator>=(const Vector &v1, const Vector &v2)
{
    if (v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z)
        return true;
    return false;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator<(const Vector &v1, const Vector &v2)
{
    if (v1.x < v2.x && v1.y < v2.y && v1.z < v2.z)
        return true;
    return false;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator<=(const Vector &v1, const Vector &v2)
{
    if (v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z)
        return true;
    return false;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator==(const Vector &v1, const Vector &v2)
{
    if (fabsf(v1.x - v2.x) < 1e-34f && fabsf(v1.y - v2.y) < 1e-34f && fabsf(v1.z - v2.z) < 1e-34f)
        return true;
    return false;
}

/*!\relates Vector
Compare component by component
*/
inline bool operator!=(const Vector &v1, const Vector &v2)
{
    return !(v1 == v2);
}

// ===========================================================
// Transformation
// ===========================================================

// Normalize the vector, and return it to its former length
inline float Vector::Normalize()
{
    const auto len =
        sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) +
             static_cast<double>(z) * static_cast<double>(z));
    if (len > 0.0)
    {
        const auto k = 1.0 / len;
        x = static_cast<float>(x * k);
        y = static_cast<float>(y * k);
        z = static_cast<float>(z * k);
    }
    return static_cast<float>(len);
}

// Project to XZ Plane and Normalize
inline float Vector::Normalize2D()
{
    y = 0.0f;
    const auto len =
        sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z));
    if (len > 0.0)
    {
        const auto k = 1.0 / len;
        x = static_cast<float>(x * k);
        z = static_cast<float>(z * k);
    }
    return static_cast<float>(len);
}

// Limit the length of the vector, and return the current length
inline float Vector::ClampLength(float clampValue)
{
    if (clampValue <= 0.0f)
    {
        x = y = z = 0.0f;
        return 0.0f;
    }
    const double len = sqrt(x * x + y * y + z * z);
    if (len > clampValue)
    {
        const auto k = clampValue / len;
        x = static_cast<float>(x * k);
        y = static_cast<float>(y * k);
        z = static_cast<float>(z * k);
        return clampValue;
    }
    return static_cast<float>(len);
}

// Limit X component to range
inline Vector &Vector::ClampX(float min, float max)
{
    if (x < min)
        x = min;
    if (x > max)
        x = max;
    return *this;
}

// Constrain Y component to range
inline Vector &Vector::ClampY(float min, float max)
{
    if (y < min)
        y = min;
    if (y > max)
        y = max;
    return *this;
}

// Constrain Z component to range
inline Vector &Vector::ClampZ(float min, float max)
{
    if (z < min)
        z = min;
    if (z > max)
        z = max;
    return *this;
}

// Limit components to a range
inline Vector &Vector::Clamp(float min, float max)
{
    if (x < min)
        x = min;
    if (x > max)
        x = max;
    if (y < min)
        y = min;
    if (y > max)
        y = max;
    if (z < min)
        z = min;
    if (z > max)
        z = max;
    return *this;
}

// Keep vector minimal components
inline Vector &Vector::Min(const Vector &v)
{
    if (x > v.x)
        x = v.x;
    if (y > v.y)
        y = v.y;
    if (z > v.z)
        z = v.z;
    return *this;
}

// Keep vector maximum components
inline Vector &Vector::Max(const Vector &v)
{
    if (x < v.x)
        x = v.x;
    if (y < v.y)
        y = v.y;
    if (z < v.z)
        z = v.z;
    return *this;
}

// ===========================================================
// Utilities
// ===========================================================

// Set new values
inline Vector &Vector::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
}

// Set new values
inline Vector &Vector::Set(const Vector &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

// Get the angle between vectors
inline float Vector::GetAngle(const Vector &v) const
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) +
               static_cast<double>(z) * static_cast<double>(z);
    len *= static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) +
           static_cast<double>(z) * static_cast<double>(z);
    if (len <= 0.0)
        return 0.0f;
    auto cs = (x * v.x + y * v.y + z * v.z) / sqrt(len);
    if (cs > 1.0)
        cs = 1.0;
    if (cs < -1.0)
        cs = -1.0;
    return static_cast<float>(acos(cs));
}

// Get the sign angle between vectors in the XZ plane
inline float Vector::GetAngle2D(const Vector &v) const
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z);
    len *= static_cast<double>(v.x) * static_cast<double>(v.x) + static_cast<double>(v.z) * static_cast<double>(v.z);
    if (len <= 0.0)
        return 0.0f;
    len = (x * v.x + z * v.z) / sqrt(len);
    if (len > 1.0)
        len = 1.0;
    if (len < -1.0)
        len = -1.0;
    len = acos(len);
    if (z * v.x - x * v.z < 0)
        len = -len;
    return static_cast<float>(len);
}

// Get the angle of rotation of a vector around the Y axis
inline float Vector::GetAY(float defAngle) const
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z);
    if (len > 0.00000000001)
    {
        len = z / sqrt(len);
        if (len > 1.0)
            len = 1.0;
        if (len < -1.0)
            len = -1.0;
        len = acos(len);
    }
    else
        return defAngle;
    if (x < 0)
        len = -len;
    return static_cast<float>(len);
}

// Get sine between 2D vectors in XZ plane
inline float Vector::Sin2D(const Vector &v) const
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z);
    len *= static_cast<double>(v.x) * static_cast<double>(v.x) + static_cast<double>(v.z) * static_cast<double>(v.z);
    if (len <= 0.0)
        return 0.0f;
    len = (z * v.x - x * v.z) / sqrt(len);
    return static_cast<float>(len);
}

// Get cosine between 2D vectors in XZ plane
inline float Vector::Cos2D(const Vector &v) const
{
    auto len = static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z);
    len *= static_cast<double>(v.x) * static_cast<double>(v.x) + static_cast<double>(v.z) * static_cast<double>(v.z);
    if (len <= 0.0)
        return 1.0f;
    len = (x * v.x + z * v.z) / sqrt(len);
    return static_cast<float>(len);
}

// Get the cross product of vectors in the XZ plane
inline float Vector::Cross2D(const Vector &v) const
{
    return z * v.x - x * v.z;
}

// Get a vector in the XZ plane
inline Vector Vector::Get2D() const
{
    return Vector(x, 0.0f, z);
}

// Get a vector in the XZ plane
inline Vector Vector::Get2D(float y) const
{
    return Vector(x, y, z);
}

// Generate vector in XZ plane by angle
inline Vector &Vector::Make2D(float ay)
{
    x = sinf(ay);
    y = 0.0f;
    z = cosf(ay);
    return *this;
}

// Get vector length
inline float Vector::GetLength() const
{
    return static_cast<float>(sqrt(static_cast<double>(x) * static_cast<double>(x) +
                                   static_cast<double>(y) * static_cast<double>(y) +
                                   static_cast<double>(z) * static_cast<double>(z)));
}

// Get vector length in 2D
inline float Vector::GetLength2D() const
{
    return static_cast<float>(
        sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(z) * static_cast<double>(z)));
}

// Get the squared length of a vector in 2D
inline float Vector::GetLength2D2() const
{
    return x * x + z * z;
}

// Rotate a vector in the XZ plane by an angle
inline Vector &Vector::Rotate(float angle)
{
    return Rotate(cosf(angle), sinf(angle));
}

// Rotate the vector in the XZ plane by an angle specified by cos, sin
inline Vector &Vector::Rotate(float vcos, float vsin)
{
    const auto tx = x * vcos + z * vsin;
    const auto tz = z * vcos - x * vsin;
    x = tx;
    z = tz;
    return *this;
}

// Rotate the vector clockwise in the XZ plane by an angle PI / 2
inline Vector &Vector::Rotate_PI2_CW()
{
    const auto t = x;
    x = z;
    z = -t;
    return *this;
}

// Rotate the vector counterclockwise in the XZ plane by an angle PI / 2
inline Vector &Vector::Rotate_PI2_CCW()
{
    const auto t = x;
    x = -z;
    z = t;
    return *this;
}

// Calculate linearly interpolated value
inline Vector &Vector::Lerp(const Vector &v1, const Vector &v2, float kBlend)
{
    x = v1.x + (v2.x - v1.x) * kBlend;
    y = v1.y + (v2.y - v1.y) * kBlend;
    z = v1.z + (v2.z - v1.z) * kBlend;
    return *this;
}

// Calculate the reflected vector
inline Vector &Vector::Reflection(const Vector &normal)
{
    const auto k = -2.0f * (x * normal.x + y * normal.y + z * normal.z);
    x += normal.x * k;
    y += normal.y * k;
    z += normal.z * k;
    return *this;
}

// Fill with a unit vector with a random direction
inline Vector &Vector::Rand()
{
    Make2D(rand() * ((2.0f * 3.141592654f) / RAND_MAX));
    y = z;
    z = 0.0f;
    return Rotate(rand() * ((2.0f * 3.141592654f) / RAND_MAX));
}

// Fill with unit vector with random direction in XZ
inline Vector &Vector::Rand2D()
{
    return Make2D(rand() * ((2.0f * 3.141592654f) / RAND_MAX));
}

// Fill with random values in a given ABB
inline Vector &Vector::Rand(const Vector &min, const Vector &max)
{
    x = min.x + rand() * ((max.x - min.x) * (1.0f / RAND_MAX));
    y = min.y + rand() * ((max.y - min.y) * (1.0f / RAND_MAX));
    z = min.z + rand() * ((max.z - min.z) * (1.0f / RAND_MAX));
    return *this;
}

// Fill with random values in a given sphere
inline Vector &Vector::Rand(const Vector &pos, float radius)
{
    Rand();
    *this *= rand() * (radius * (1.0f / RAND_MAX));
    return *this;
}

// Move the current vector to the specified point by the specified step
inline bool Vector::MoveByStep(const Vector &to, float step)
{
    if (step <= 0.0f)
        return false;
    // Distance to point
    const auto dx = to.x - x;
    const auto dy = to.y - y;
    const auto dz = to.z - z;
    auto dist = dx * dx + dy * dy + dz * dz;
    if (dist < 1e-20f)
    {
        x = to.x;
        y = to.y;
        z = to.z;
        return false;
    }
    dist = sqrtf(dist);
    // Moving to a point
    if (step >= dist)
    {
        x = to.x;
        y = to.y;
        z = to.z;
        return true;
    }
    step /= dist;
    x += dx * step;
    y += dy * step;
    z += dz * step;
    return true;
}

// Move the current vector to a given point by a given step in 2D
inline bool Vector::MoveByStep2D(const Vector &to, float step)
{
    if (step <= 0.0f)
        return false;
    // Distance to point
    const auto dx = to.x - x;
    const auto dz = to.z - z;
    auto dist = dx * dx + dz * dz;
    if (dist < 1e-20f)
    {
        x = to.x;
        z = to.z;
        return false;
    }
    dist = sqrtf(dist);
    // Moving to a point
    if (step >= dist)
    {
        x = to.x;
        z = to.z;
        return true;
    }
    step /= dist;
    x += dx * step;
    z += dz * step;
    return true;
}

// The point is in the box
inline bool Vector::InBox(const Vector &min, const Vector &max) const
{
    if (y < min.y || y > max.y)
        return false;
    if (x < min.x || x > max.x)
        return false;
    if (z < min.z || z > max.z)
        return false;
    return true;
}

// The point is in the sphere
inline bool Vector::InSphere(const Vector &pos, float rad) const
{
    double d = (x - pos.x) * (x - pos.x);
    d += (y - pos.y) * (y - pos.y);
    d += (z - pos.z) * (z - pos.z);
    return d < static_cast<double>(rad) * static_cast<double>(rad);
}

#pragma pack(pop)
