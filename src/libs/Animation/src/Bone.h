// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// Bone
// --------------------------------------------------------------------------------------------
// Model bone containing its animation
// ============================================================================================

#pragma once

//============================================================================================

#include "Matrix.h"
#include "defines.h"

#include <d3dx9.h>
//============================================================================================

#define ANI_COMPRESS_ENABLE

//============================================================================================

class Bone
{

#pragma pack(push, 1)

    struct COMP_QUATERNION
    {
        short x, y, z;
        short w;
    };

#pragma pack(pop)

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Bone();
    virtual ~Bone();
    // Set parent
    void SetParent(Bone *parentBone);
    // how many frames of animation there will be
    void SetNumFrames(long num, CVECTOR &sPos, bool isRoot = false);
    // Set animation positions
    void SetPositions(const CVECTOR *pArray, long numPos);
    // Set animation angles
    void SetAngles(const D3DXQUATERNION *aArray, long numAng);
    // Initialize start matrix
    void BuildStartMatrix();

    // --------------------------------------------------------------------------------------------
    // Working with a bone
    // --------------------------------------------------------------------------------------------
  public:
    // Add animation frames
    void BlendFrame(long frame, float kBlend, D3DXQUATERNION &res);
    // void BlendFrame(float frame);
    // Create a matrix for the resulting position
    void BuildMatrix();
    // Create matrix for frame 0
    // void BuildMatrixZero();
    // Get position matrix
    CMatrix &Matrix();
    // Get the starting position matrix
    CMatrix &StartMatrix();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void GetFrame(long f, D3DXQUATERNION &qt);
    float Clamp(float v, const char *str);
    // Linear position interpolation
    float LerpPos(float a, float b, float k);
    // Linear Angle Interpolation
    float LerpAng(float a, float b, float k);

  public:
    Bone *parent; // Parent bone

#ifdef ANI_COMPRESS_ENABLE
    COMP_QUATERNION *ang; // Compressed angles of animation frames
#else
    D3DXQUATERNION *ang; // Animation frame angles
#endif

    CVECTOR *pos;   // Animation frame positions
    long numFrames; // Number of animation frames
    CVECTOR pos0;   // Bone position if there are no animation frame positions

    CVECTOR p;        // Bone position in local coordinates
    D3DXQUATERNION a; // Bone rotation angles in local coordinates
    CMatrix matrix;   // Bone position matrix
    CMatrix start;    // Frame 0 matrix
};

//============================================================================================
// inline
//============================================================================================

// Set parent
inline void Bone::SetParent(Bone *parentBone)
{
    parent = parentBone;
}

// Get position matrix
inline CMatrix &Bone::Matrix()
{
    return matrix;
}

// Get the starting position matrix
inline CMatrix &Bone::StartMatrix()
{
    return start;
}

// Linear position interpolation
inline float Bone::LerpPos(float a, float b, float k)
{
    return a + k * (b - a);
}

// Linear Angle Interpolation
inline float Bone::LerpAng(float a, float b, float k)
{
    float delta = b - a;
    if (delta < -PI)
        delta = 2 * PI + delta;
    if (delta > PI)
        delta = -2 * PI + delta;
    return a + k * delta;
}
