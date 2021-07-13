//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Plane
//============================================================================================

#pragma once

#pragma pack(push, 1)

#include "Vector.h"

// Class for representing a plane in 3D space
class Plane
{
  public:
    union {
        struct
        {
            // Normal
            Vector n;
        };

        struct
        {
            // Normal
            Vector normal;
        };

        struct
        {
            // Normal
            Vector N;
        };
    };

    union {
        // Distance from center
        float d;
        // Distance from center
        float dist;
        // Distance from center
        float D;
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Empty constructor
    Plane();
    // Set direction
    Plane(float Nx, float Ny, float Nz);
    // Set direction
    Plane(const Vector &normal);
    // Create plane
    Plane(const Vector &normal, const Vector &point);
    // Copy constructor
    Plane(const Plane &plane);

    // -----------------------------------------------------------
    // Transformation
    // -----------------------------------------------------------
  public:
    // Normalize
    Plane &Normalize();
    // Move plane to specified point
    Plane &Move(const Vector &point);

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Find distance to plane (*)
    float Dist(const Vector &point) const;
    // Check for intersection of line and plane
    bool Intersection(const Vector &src, const Vector &dst) const;
    // Find the intersection point of a line and a plane
    bool Intersection(const Vector &src, const Vector &dst, Vector &res) const;
    // Check for line and plane intersection
    bool IntersectionLine(const Vector &src, const Vector &dst, float &k) const;
};

// ===========================================================
// Constructors
// ===========================================================

// Empty constructor
inline Plane::Plane()
{
}

// Set direction
inline Plane::Plane(float Nx, float Ny, float Nz)
{
    N.x = Nx;
    N.y = Ny;
    N.z = Nz;
    D = 0.0f;
}

// Set direction
inline Plane::Plane(const Vector &normal)
{
    N = normal;
}

// Create plane
inline Plane::Plane(const Vector &normal, const Vector &point)
{
    N = normal;
    D = normal | point;
}

// Copy constructor
inline Plane::Plane(const Plane &plane)
{
    N = plane.N;
    D = plane.D;
}

// ===========================================================
// Operators
// ===========================================================

/*!\relates Plane
Distance from point to plane
*/
inline float operator*(const Vector &point, const Plane &plane)
{
    return (plane.N | point) - plane.D;
}

/*!\relates Plane
Distance from point to plane
*/
inline float operator*(const Plane &plane, const Vector &point)
{
    return (plane.N | point) - plane.D;
}

// ===========================================================
// Transformation
// ===========================================================

// Normalize
inline Plane &Plane::Normalize()
{
    const auto d = normal.Normalize();
    if (d != 0.0f)
        dist /= d;
    else
        dist = 0.0f;
    return *this;
}

// Move plane to specified point
inline Plane &Plane::Move(const Vector &point)
{
    D = (N | point);
    return *this;
}

// ===========================================================
// Utilities
// ===========================================================

// Find distance to plane (*)
inline float Plane::Dist(const Vector &point) const
{
    return *this * point;
}

// Check for intersection of line and plane
inline bool Plane::Intersection(const Vector &src, const Vector &dst) const
{
    const auto dsrc = *this * src;
    const auto ddst = *this * dst;
    return (dsrc * ddst <= 0.0f);
}

// Find the intersection point of a line and a plane
inline bool Plane::Intersection(const Vector &src, const Vector &dst, Vector &res) const
{
    const auto dsrc = *this * src;
    auto ddst = *this * dst;
    if (dsrc * ddst > 0.0f)
        return false;
    ddst = dsrc - ddst;
    res = src;
    if (ddst != 0.0f)
        res += (dst - src) * dsrc / ddst;
    return true;
}

// Check for line and plane intersection
inline bool Plane::IntersectionLine(const Vector &src, const Vector &dst, float &k) const
{
    const auto dsrc = *this * src;
    auto ddst = *this * dst;
    ddst = dsrc - ddst;
    if (fabsf(ddst) <= 1e-30f)
        return false;
    k = dsrc / ddst;
    return true;
}

#pragma pack(pop)
