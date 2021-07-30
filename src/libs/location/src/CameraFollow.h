//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    CameraFollow
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "model.h"

class LocationCamera;

class CameraFollow
{
    struct EdgeInfo
    {
        Vector n; // Normal to plane
        float d;   // Distance to the plane
        Vector s; // Start of the edge
        Vector e; // End of the edge
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    CameraFollow();
    virtual ~CameraFollow();
    void SetLocationCamera(LocationCamera *locCamera);

    //--------------------------------------------------------------------------------------------
  public:
    // Make calculations
    void Update(float dltTime);

    // Find current camera position
    void MoveCamera(float dltTime);
    // Reinitialize camera position
    void BornCamera();
    // Calculate camera position for a given angle
    void CalcPosition(float ang, float radius, float dax, Vector &pos) const;
    // Estimate the radius in the desired direction for 1 sec
    float FindRadius(float curAng) const;
    // Consider polygon in radius search
    static bool ApplyPoly(const Vector *v, long n);
    static void ApplyPolyFindNearest(EdgeInfo *e, long ne, const Vector &pos);
    // Change radius
    void ChangeRadius(float dltTime, float radius);
    // Find the acceleration of camera stop when corner collision with patch
    void FindRotAccelCam();

    // Trace the ray taking into account cull and camera size
    float Trace(const Vector &src, const Vector &dst);
    // Trace a ray with cull
    float SubTrace(const Vector &src, const Vector &dst) const;

    void DrawDebug();

    // private:
  public:
    LocationCamera *lc;

    Vector camPos, lookTo;
    // Camera following the character
    float camay;        // Current camera direction
    float camradius;    // Current radius
    bool isBrn;         // If set, then need to respawn the camera
    float rotSpd;       // Camera movement speed at the time of stop
    float rotAcc;       // Stop acceleration
    float kRadInert;    // Dependence of the inertia of the radius change on the turning speed
    float kRadInertCur; // Current dependence of inertia

    static float fndRadius;     // Current radius when searching
    static float fndMaxRadius;  // Maximum search radius
    static Vector fndCamDir;   // Normalized camera heading
    static Vector fndCamDirXZ; // Normalized heading to camera in XZ
    static Vector fndCamDirY;  // Normalized camera direction in Y
    static Vector fndCamPos;   // Observation point
    static float fndcsAx;       // Cosine of the bounding angle in x
    static float fndkAx;        // 1/(1 - cos(ax))
    static float fndcsAy;       // Cosine of the y bounding angle
    static float fndkAy;        // 1/(1 - cos(ay))
    static Plane fndPlanes[32]; // Temporarily used face planes
};
