#pragma once

#include "math3d.h"

class DataGraph;
class DataColor;
class DataUV;
class IEmitter;
class GEOS;

struct BB_ParticleData
{
    bool SpeedOriented; // Turn along the velocity vector ...

    // Pointer to the number of particles of this type, when removing a particle, you need to decrease it !!!
    uint32_t *ActiveCount;

    // Physical position
    Vector PhysPos;

    // Angle of rotation
    float Angle;

    // Twisting speed, radians per second.
    float Spin;

    // Direction and "strength" of speed (NOT Normalized)
    Vector Velocity;

    // External force (for applying it to a particle)
    Vector ExternalForce;

    // Lifetime
    float LifeTime;

    // How long did it live
    float ElapsedTime;

    // Transformation matrix, at emission of a particle (for a track)
    Matrix matWorld;

    // Weight
    float Mass;

    // fabsf(Mass)
    float UMass;

    // ===========================================================================
    // Final position / angle for rendering
    Vector RenderPos;
    float RenderAngle;
    Vector OldRenderPos;
    float OldRenderAngle;

    // ===========================================================================
    // Distance to camera
    float CamDistance;
    bool Visible;

    // ===========================================================================
    // Graphs

    DataGraph *Graph_SpinDrag;
    DataGraph *Graph_Drag;
    DataGraph *Graph_Size;
    DataGraph *Graph_Frames;
    DataColor *Graph_Color;
    DataUV *Graph_UV;
    DataGraph *Graph_Transparency;
    DataGraph *Graph_TrackX;
    DataGraph *Graph_TrackY;
    DataGraph *Graph_TrackZ;
    DataGraph *Graph_PhysBlend;
    DataGraph *graph_GravK;
    DataGraph *graph_AddPower;

    // ===========================================================================
    // Coefficients for randomization
    float DragK;
    float SpinDragK;
    float SizeK;
    float ColorK;
    float AlphaK;
    float FrameK;
    float GravKK;
    float AddPowerK;
    float KTrackX;
    float KTrackY;
    float KTrackZ;
    float KPhysBlend;

    // ===========================================================================
    // Pointer to the emitter that is attached to the particle
    IEmitter *AttachedEmitter;

    // ===============================================
    // ID of the emitter to which the particle belongs
    uint32_t EmitterGUID;
};

struct MDL_ParticleData
{
    // Pointer to the number of particles of this type, when removing a particle, you need to decrease it !!!
    uint32_t *ActiveCount;

    // Physical position
    Vector PhysPos;

    // Angle of rotation
    Vector Angle;

    // Twisting speed, radians per second.
    Vector Spin;

    // Direction and "strength" of speed (NOT Normalized)
    Vector Velocity;

    // External force (for applying it to a particle)
    Vector ExternalForce;

    // Lifetime
    float LifeTime;

    // How long did it live
    float ElapsedTime;

    // Transformation matrix, at emission of a particle (for a track)
    Matrix matWorld;

    // Weight
    float Mass;

    // fabsf(Mass)
    float UMass;

    // ===========================================================================
    // Final position / angle for rendering
    Vector RenderPos;
    Vector RenderAngle;
    Vector OldRenderPos;
    Vector OldRenderAngle;

    // ===========================================================================
    // Graphs

    DataGraph *Graph_SpinDragX;
    DataGraph *Graph_SpinDragY;
    DataGraph *Graph_SpinDragZ;
    DataGraph *Graph_Drag;
    DataGraph *Graph_TrackX;
    DataGraph *Graph_TrackY;
    DataGraph *Graph_TrackZ;
    DataGraph *Graph_PhysBlend;
    DataGraph *graph_GravK;

    // ===========================================================================
    // Coefficients for randomization
    float DragK;
    float SpinDragK_X;
    float SpinDragK_Y;
    float SpinDragK_Z;
    float GravKK;
    float KTrackX;
    float KTrackY;
    float KTrackZ;
    float KPhysBlend;

    // ===========================================================================
    // Pointer to the model to render
    GEOS *pScene;

    // ===========================================================================
    // Pointer to the emitter that is attached to the particle
    IEmitter *AttachedEmitter;

    // ===============================================
    // ID of the emitter to which the particle belongs
    uint32_t EmitterGUID;
};
