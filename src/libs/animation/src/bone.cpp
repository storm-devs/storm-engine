// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// Bone
// --------------------------------------------------------------------------------------------
// Model bone containing its animation
// ============================================================================================

#include "bone.h"

#include "core.h"
#include "storm_assert.h"
#include "v_module_api.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

Bone::Bone()
{
    parent = nullptr;
    ang = nullptr;
    pos = nullptr;
    numFrames = 0;
}

Bone::~Bone()
{
    delete ang;
    delete pos;
}

// how many frames of animation there will be
void Bone::SetNumFrames(int32_t num, CVECTOR &sPos, bool isRoot)
{
    delete ang;
    delete pos;
    ang = nullptr;
    pos = nullptr;
    numFrames = num;
    if (numFrames <= 0)
    {
        numFrames = 0;
        return;
    }
#ifdef ANI_COMPRESS_ENABLE
    ang = new COMP_QUATERNION[num];
    memset(ang, 0, numFrames * sizeof(ang[0]));
#else
    ang = new D3DXQUATERNION[num];
    memset(ang, 0, numFrames * sizeof(ang[0]));
#endif

    if (isRoot)
    {
        pos = new CVECTOR[num];
        memset(pos, 0, numFrames * sizeof(CVECTOR));
    }
    pos0 = sPos;
}

// Set animation positions
void Bone::SetPositions(const CVECTOR *pArray, int32_t numPos)
{
    Assert(numPos == numFrames);
    Assert(pArray);
    Assert(pos);
    memcpy(pos, pArray, numFrames * sizeof(CVECTOR));
}

//=====================================================================================

#ifdef ANI_COMPRESS_ENABLE

//-------------------------------------
// Compressed animation
//-------------------------------------

// Set animation angles
void Bone::SetAngles(const D3DXQUATERNION *aArray, int32_t numAng)
{
    Assert(numAng == numFrames);
    Assert(aArray);
    Assert(ang);
    for (int32_t i = 0; i < numAng; i++)
    {
        auto x = Clamp(aArray[i].x, "Animation is break: qt.x < -1.0f or qt.x > 1.0f !!!");
        auto y = Clamp(aArray[i].y, "Animation is break: qt.y < -1.0f or qt.y > 1.0f !!!");
        auto z = Clamp(aArray[i].z, "Animation is break: qt.z < -1.0f or qt.z > 1.0f !!!");
        auto w = Clamp(aArray[i].w, "Animation is break: qt.w < -1.0f or qt.w > 1.0f !!!");
        x = static_cast<float>(asin(x) / (PI * 0.5)) * 32767.0f;
        y = static_cast<float>(asin(y) / (PI * 0.5)) * 32767.0f;
        z = static_cast<float>(asin(z) / (PI * 0.5)) * 32767.0f;
        w = static_cast<float>(asin(w) / (PI * 0.5)) * 32767.0f;
        ang[i].x = static_cast<short>(static_cast<int32_t>(x));
        ang[i].y = static_cast<short>(static_cast<int32_t>(y));
        ang[i].z = static_cast<short>(static_cast<int32_t>(z));
        ang[i].w = static_cast<short>(static_cast<int32_t>(w));
    }
}

inline void Bone::GetFrame(int32_t f, D3DXQUATERNION &qt)
{
    qt.x = sinf((ang[f].x * (1.0f / 32767.0f)) * PI * 0.5f);
    qt.y = sinf((ang[f].y * (1.0f / 32767.0f)) * PI * 0.5f);
    qt.z = sinf((ang[f].z * (1.0f / 32767.0f)) * PI * 0.5f);
    qt.w = sinf((ang[f].w * (1.0f / 32767.0f)) * PI * 0.5f);
}

#else

//-------------------------------------
// Uncompressed animation
//-------------------------------------

// Set animation angles
void Bone::SetAngles(const D3DXQUATERNION *aArray, int32_t numAng)
{
    Assert(numAng == numFrames);
    Assert(aArray);
    Assert(ang);
    memcpy(ang, aArray, numFrames * sizeof(*ang));
}

inline void Bone::GetFrame(int32_t f, D3DXQUATERNION &qt)
{
    qt = ang[f];
}

#endif

inline float Bone::Clamp(float v, const char *str)
{
    auto isErr = false;
    if (v < -1.0f)
    {
        v = -1.0f;
        isErr = true;
    }
    if (v > 1.0f)
    {
        v = 1.0f;
        isErr = true;
    }
    if (isErr && str)
        core.Trace(str);
    return v;
}

//=====================================================================================

// Initialize start matrix
void Bone::BuildStartMatrix()
{
    if (numFrames == 0 || !ang)
        return;
    const CMatrix inmtx;
    D3DXQUATERNION a;
    GetFrame(0, a);
    D3DXMatrixRotationQuaternion(inmtx, &a);
    inmtx.Pos() = pos0;
    if (parent)
        start.EqMultiply(inmtx, parent->start);
    else
        start = inmtx;
}

// --------------------------------------------------------------------------------------------
// Working with a bone
// --------------------------------------------------------------------------------------------

// Add animation frames
void Bone::BlendFrame(int32_t frame, float kBlend, D3DXQUATERNION &res)
{
    if (numFrames <= 0)
        return;
    const auto f0 = frame;
    const auto f1 = frame + 1;
    if (f0 >= numFrames || f1 >= numFrames)
    {
        GetFrame(numFrames - 1, res);
        return;
    }
    D3DXQUATERNION q0, q1;
    GetFrame(f0, q0);
    GetFrame(f1, q1);
    if (kBlend < 0.0f)
        kBlend = 0.0f;
    if (kBlend > 1.0f)
        kBlend = 1.0f;
    D3DXQuaternionSlerp(&res, &q0, &q1, kBlend);
}

// Create a matrix for the resulting position
void Bone::BuildMatrix()
{
    // Turns
    // matrix = a.BuildMatrix();
    D3DXMatrixRotationQuaternion(matrix, &a);
    matrix.Pos() = p;
    // Multiply by the parent matrix
    if (parent)
        matrix.EqMultiply(CMatrix(matrix), parent->matrix);
}
