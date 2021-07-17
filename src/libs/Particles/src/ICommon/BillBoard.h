#pragma once

#include "../math3D.h"

// Particle
struct ParticleBillboard
{
    Vector vPos;
    float fSizeX, fSizeY;
    float fAngle;
    uint32_t dwColor;
    float DistanceToCamera;
    bool Visible;

    // For the first frame ------------------------------------
    // Top left corner in UV
    float U11;
    float V11;

    // Bottom right corner in UV
    float U12;
    float V12;

    // For the second frame ------------------------------------
    // Top left corner in UV
    float U21;
    float V21;

    // Bottom right corner in UV
    float U22;
    float V22;

    // Blending ratio between the first and second frames
    float BlendK;

    // Constructor-------------------------------------------
    ParticleBillboard()
    {
        DistanceToCamera = 0.0f;
        Visible = false;
    }
};
