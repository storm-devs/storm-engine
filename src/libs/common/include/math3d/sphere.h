//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Sphere
//============================================================================================

#pragma once

#pragma pack(push, 1)

#include "vector.h"
#include "vector4.h"

// Class for representing a sphere in 3D space
class Sphere
{
  public:
    union {
        struct
        {
            union {
                struct
                {
                    // X Position
                    float x;
                    // Y position
                    float y;
                    // Z Position
                    float z;
                };

                union {
                    struct
                    {
                        // Position
                        Vector p;
                    };

                    struct
                    {
                        // Position
                        Vector pos;
                    };
                };
            };

            union {
                // Radius
                float r;
                // Radius
                float radius;
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
    Sphere();
    // Copy constructor
    Sphere(const Sphere &s);
    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Point in sphere
    bool Intersection(const Vector &p);
    // Check intersection of line and sphere
    bool Intersection(const Vector &src, const Vector &dst);
    // Check ray and sphere intersection
    bool Intersection(const Vector &orig, const Vector &normdir, float *res);
    // Check sphere and sphere intersection
    bool Intersection(const Sphere &sph);

    // Set sphere in a point with 0 radius
    void Reset(const Vector &p);
    // Include a point in the enclosing sphere
    void AddPoint(const Vector &p);

    // Check ray and sphere intersection
    static bool Intersection(const Vector &orig, const Vector &normdir, const Vector &pos, float r, float *res);
};

// -----------------------------------------------------------
// Constructors
// -----------------------------------------------------------

// Empty constructor
inline Sphere::Sphere()
{
    v4 = Vector4();
};

// Copy constructor
inline Sphere::Sphere(const Sphere &s)
{
    v4 = s.v4;
};

// ===========================================================
// Utilities
// ===========================================================

// Point in sphere
inline bool Sphere::Intersection(const Vector &p)
{
    return ~(pos - p) <= radius * radius;
}

// Check intersection of line and sphere
inline bool Sphere::Intersection(const Vector &src, const Vector &dst)
{
    Vector dir = dst - src;
    const float len = dir.Normalize();
    if (len > 1e-10f)
    {
        float dist;
        if (!Intersection(src, dir, pos, r, &dist))
            return false;
        if (dist >= 0.0f)
        {
            return (dist <= len);
        }
        dir = -dir;
        if (!Intersection(dst, dir, pos, r, &dist))
            return false;
        if (dist >= 0.0f)
        {
            return (dist <= len);
        }
        return false;
    }
    return ~(pos - src) <= radius * radius;
}

// Check ray and sphere intersection
inline bool Sphere::Intersection(const Vector &orig, const Vector &normdir, float *res)
{
    return Intersection(orig, normdir, pos, r, res);
}

// Check sphere and sphere intersection
inline bool Sphere::Intersection(const Sphere &sph)
{
    return (~(p - sph.p) <= (r + sph.r) * (r + sph.r));
}

// Set sphere in a point with 0 radius
inline void Sphere::Reset(const Vector &p)
{
    pos = p;
    r = 0.0f;
}

// Include a point in the enclosing sphere
inline void Sphere::AddPoint(const Vector &p)
{
    // Vector from point to center
    const float dx = pos.x - p.x;
    const float dy = pos.y - p.y;
    const float dz = pos.z - p.z;
    float len = dx * dx + dy * dy + dz * dz;
    if (len <= r * r)
        return;
    len = sqrtf(len);
    // New radius
    r = (len + r) * 0.5f;
    // New position
    len = r / len;
    pos.x = p.x + dx * len;
    pos.y = p.y + dy * len;
    pos.z = p.z + dz * len;
}

// Check ray and sphere intersection
inline bool Sphere::Intersection(const Vector &orig, const Vector &normdir, const Vector &pos, float r, float *res)
{
    const Vector toCenter = pos - orig;
    const float distToOrtoPlane = normdir | toCenter;
    const float distFromOrtoPlaneToSphere2 = r * r - (~toCenter - distToOrtoPlane * distToOrtoPlane);
    if (distFromOrtoPlaneToSphere2 < 0.0f)
        return false;
    if (res)
    {
        *res = distToOrtoPlane - sqrtf(distFromOrtoPlaneToSphere2);
    }
    return true;
}

#pragma pack(pop)
