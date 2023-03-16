//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Matrix
//============================================================================================

#pragma once

#include "plane.h"
#include "vector4.h"
#include <d3d9.h>

//============================================================================================

#pragma pack(push, 1)

//============================================================================================
/*!
<PRE>

Linear representation         2D array
                                vx     vy     vz    pos
    0  4  8 12                [0][0] [1][0] [2][0] [3][0]    x
    1  5  9 13                [0][1] [1][1] [2][1] [3][1]    y
    2  6 10 14                [0][2] [1][2] [2][2] [3][2]    z
    3  7 11 15                [0][3] [1][3] [2][3] [3][3]    w
*/
// Matrix class for 3D transformations.
// ============================================================================================

class Matrix
{
  public:
    union {
        // Linear array
        alignas(16) float matrix[16]; // espkk # remove inline asm # 30/Dec/2017
        // Two-dimensional array
        float m[4][4];

        struct
        {
            // X direction
            Vector vx;
            // Weight value of X
            float wx;
            // Y direction
            Vector vy;
            // Weight value of Y
            float wy;
            // Z direction
            Vector vz;
            // Weight value of Z
            float wz;
            // Position
            Vector pos;
            // Added weight value
            float w;
        } v;
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Construct the identity matrix
    Matrix();
    // Construct matrix without filling
    Matrix(bool empty);
    // Construct matrix using angles with position
    Matrix(float angX, float angY, float angZ, float x, float y, float z);
    // Construct matrix using angles
    Matrix(float angX, float angY, float angZ);
    // Construct matrix using angles with position
    Matrix(const Vector &ang, const Vector &pos);
    // Construct matrix using angles
    Matrix(const Vector &ang);
    // Construct a copy of the matrix
    Matrix(const Matrix &matrix);
    // Construct the result of matrix multiplication: this = m1 * m2
    Matrix(const Matrix &m1, const Matrix &m2);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Assign another matrix to a matrix
    Matrix &operator=(const Matrix &mtx);
    // Assign a number to matrix values
    Matrix &operator=(float f);
    // Assign a number to matrix values
    Matrix &operator=(double d);
    // Assign a vector to matrix values
    Matrix &operator=(const Vector &v);

    // Multiply matrices
    Matrix &operator*=(const Matrix &mtx);

    // -----------------------------------------------------------
    // Matrix filling
    // -----------------------------------------------------------
  public:
    // Set identity matrix
    Matrix &SetIdentity();

    // Set Matrix
    Matrix &Set(const Matrix &matrix);

    // Calculate matrix M = rotZ * rotX * rotY * Pos
    Matrix &Build(float angX, float angY, float angZ, float x, float y, float z);
    // Calculate matrix M = rotZ * rotX * rotY
    Matrix &Build(float angX, float angY, float angZ);
    // Calculate matrix M = rotZ * rotX * rotY * Pos
    Matrix &Build(const Vector &ang, const Vector &pos);
    // Calculate matrix M = rotZ * rotX * rotY
    Matrix &Build(const Vector &ang);

    // Calculate matrix M = rotX * rotY * rotZ * Pos
    Matrix &BuildXYZ(float angX, float angY, float angZ, float x, float y, float z);

    // Calculate rotation matrix around X
    Matrix &BuildRotateX(float ang);
    // Calculate rotation matrix around Y
    Matrix &BuildRotateY(float ang);
    // Calculate the rotation matrix around Z
    Matrix &BuildRotateZ(float ang);
    // Calculate position matrix
    Matrix &BuildPosition(float x, float y, float z);
    // Calculate position matrix
    Matrix &BuildPosition(const Vector &pos);

    // Calculate scaling matrix
    Matrix &BuildScale(float scale);
    // Calculate scaling matrix
    Matrix &BuildScale(float scaleX, float scaleY, float scaleZ);
    // Calculate scaling matrix
    Matrix &BuildScale(const Vector &scale);

    // Calculate projection matrix
    Matrix &BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);
    // Calculate camera matrix
    bool BuildView(Vector lookFrom, Vector lookTo, Vector upVector);
    // Calculate the orientation matrix of an object having a z direction and an up direction
    bool BuildOrient(Vector zAxisDirection, Vector upVector);
    // Calculate the matrix of the object having the position of the point where the object is directed and the
    // direction up
    bool BuildOriented(Vector position, Vector lookTo, Vector upVector);
    // Calculate matrix for mirroring geometry
    Matrix &BuildMirror(float Nx, float Ny, float Nz, float D);
    // Calculate Orthogonal Projection Matrix
    Matrix &BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar);
    // Calculate projection matrix for shadow
    Matrix &BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);

    // -----------------------------------------------------------
    // Matrix transformation
    // -----------------------------------------------------------
  public:
    // Rotate X
    Matrix &RotateX(float ang);
    // Rotate around Y
    Matrix &RotateY(float ang);
    // Rotate around Z
    Matrix &RotateZ(float ang);
    // Rotate around ZXY
    Matrix &Rotate(float angX, float angY, float angZ);
    // Rotate around ZXY
    Matrix &Rotate(const Vector &ang);

    // Move
    Matrix &Move(float dX, float dY, float dZ);
    // Move
    Matrix &Move(const Vector &pos);

    // Scale
    Matrix &Scale(float scale);
    // Scale rotation matrix
    Matrix &Scale3x3(float scale);
    // Scale
    Matrix &Scale(float scaleX, float scaleY, float scaleZ);
    // Scale rotation matrix
    Matrix &Scale3x3(float scaleX, float scaleY, float scaleZ);
    // Scale
    Matrix &Scale(const Vector &scale);
    // Scale rotation matrix
    Matrix &Scale3x3(const Vector &scale);

    // Calculating the inverse matrix
    Matrix &Inverse();
    // Calculating an inverse matrix from another
    Matrix &Inverse(const Matrix &mtx);
    // Calculating the scaled inverse matrix
    Matrix &InverseWhithScale();
    // Matrix Transposition
    Matrix &Transposition();
    // Rotation Matrix Transposition
    Matrix &Transposition3X3();

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Count rotation only
    Matrix &SetRotate(const Matrix &mtr);
    // Multiply matrices and place the result into the current one
    Matrix &EqMultiply(const Matrix &m1, const Matrix &m2);
    // Multiply matrices and place the result in the current m1 != this && m2 != this
    Matrix &EqMultiplyFast(const Matrix &m1, const Matrix &m2);

    // Multiply vertex by matrix
    Vector MulVertex(const Vector &v) const;
    // Multiply normal by matrix
    Vector MulNormal(const Vector &v) const;
    // Multiply vertex by inverse matrix
    Vector MulVertexByInverse(const Vector &v) const;
    // Multiply normal by inverse matrix
    Vector MulNormalByInverse(const Vector &v) const;

    // Get camera position from camera matrix
    Vector GetCamPos() const;
    // Identity matrix or not
    bool IsIdentity() const;
    // Scaling matrix or not
    bool IsScale() const;

    // Project Vertex (for projection matrix)
    Vector4 Projection(Vector vertex, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f) const;
    // Project vertex array (for projection matrix)
    void Projection(Vector4 *dstArray, Vector *srcArray, int32_t num, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f,
                    int32_t srcSize = sizeof(Vector), int32_t dstSize = sizeof(Vector4)) const;

    // Get angles from unscaled rotation matrix
    void GetAngles(float &ax, float &ay, float &az) const;
    // Get angles from unscaled rotation matrix
    void GetAngles(Vector &ang) const;

    // Access matrix elements with brackets
    float &operator()(int32_t i, int32_t j);

    // Get a pointer to a D3D matrix
    operator D3DMATRIX *() const;

    // Get a vector for calculating the X component
    Vector4 GetVectorX() const;
    // Get a vector for calculating the Y component
    Vector4 GetVectorY() const;
    // Get a vector for calculating the Z component
    Vector4 GetVectorZ() const;
    // Get a vector for calculating the W component
    Vector4 GetVectorW() const;
};

// ===========================================================
// Constructors
// ===========================================================

// Construct the identity matrix
inline Matrix::Matrix()
{
    SetIdentity();
}

// Construct matrix without filling
inline Matrix::Matrix(bool empty)
{
}

// Construct matrix using angles and position
inline Matrix::Matrix(float angX, float angY, float angZ, float x, float y, float z)
{
    Build(angX, angY, angZ, x, y, z);
}

// Construct matrix using angles
inline Matrix::Matrix(float angX, float angY, float angZ)
{
    Build(angX, angY, angZ);
}

// Construct matrix using angles and position
inline Matrix::Matrix(const Vector &ang, const Vector &pos)
{
    Build(ang, pos);
}

// Construct matrix using angles
inline Matrix::Matrix(const Vector &ang)
{
    Build(ang);
}

// Construct a copy of the matrix
inline Matrix::Matrix(const Matrix &matrix)
{
    Set(matrix);
}

// Construct the result of matrix multiplication this = m1 * m2
inline Matrix::Matrix(const Matrix &m1, const Matrix &m2)
{
    EqMultiply(m1, m2);
}

// ===========================================================
// Operators
// ===========================================================

// Assign another matrix to a matrix
inline Matrix &Matrix::operator=(const Matrix &mtx)
{
    Set(mtx);
    return *this;
}

// Assign a number to the position
inline Matrix &Matrix::operator=(float f)
{
    v.pos = f;
    return *this;
}

// Assign a number to the position
inline Matrix &Matrix::operator=(double d)
{
    v.pos = d;
    return *this;
}

// Assign a vector to the position
inline Matrix &Matrix::operator=(const Vector &vec)
{
    v.pos = vec;
    return *this;
}

// Multiply matrices
inline Matrix &Matrix::operator*=(const Matrix &mtx)
{
    EqMultiply(*this, mtx);
    return *this;
}

/*!\relates Matrix
Multiply matrices
*/
inline Matrix operator*(const Matrix &m1, const Matrix &m2)
{
    Matrix m;
    m.EqMultiplyFast(m1, m2);
    return m;
}

/*!\relates Matrix
Multiply vector by matrix
*/
inline Vector operator*(const Matrix &mtx, const Vector &v)
{
    return mtx.MulVertex(v);
}

/*!\relates Matrix
Multiply vector by matrix
*/
inline Vector operator*(const Vector &v, const Matrix &mtx)
{
    return mtx.MulVertex(v);
}

// ===========================================================
// Matrix filling
// ===========================================================

// Set identity matrix
inline Matrix &Matrix::SetIdentity()
{
    matrix[0] = 1.f;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 1.f;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 1.f;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1.f;

    /*_asm
    {
      mov        eax, this
      mov        ecx, 0x3f800000
      xor        ebx, ebx
      mov        [eax + 0*4], ecx
      mov        [eax + 1*4], ebx
      mov        [eax + 2*4], ebx
      mov        [eax + 3*4], ebx
      mov        [eax + 4*4], ebx
      mov        [eax + 5*4], ecx
      mov        [eax + 6*4], ebx
      mov        [eax + 7*4], ebx
      mov        [eax + 8*4], ebx
      mov        [eax + 9*4], ebx
      mov        [eax + 10*4], ecx
      mov        [eax + 11*4], ebx
      mov        [eax + 12*4], ebx
      mov        [eax + 13*4], ebx
      mov        [eax + 14*4], ebx
      mov        [eax + 15*4], ecx
    }*/
    return *this;
}

// Set Matrix
inline Matrix &Matrix::Set(const Matrix &matrix)
{
    this->matrix[0] = matrix.matrix[0];
    this->matrix[1] = matrix.matrix[1];
    this->matrix[2] = matrix.matrix[2];
    this->matrix[3] = matrix.matrix[3];
    this->matrix[4] = matrix.matrix[4];
    this->matrix[5] = matrix.matrix[5];
    this->matrix[6] = matrix.matrix[6];
    this->matrix[7] = matrix.matrix[7];
    this->matrix[8] = matrix.matrix[8];
    this->matrix[9] = matrix.matrix[9];
    this->matrix[10] = matrix.matrix[10];
    this->matrix[11] = matrix.matrix[11];
    this->matrix[12] = matrix.matrix[12];
    this->matrix[13] = matrix.matrix[13];
    this->matrix[14] = matrix.matrix[14];
    this->matrix[15] = matrix.matrix[15];

    /*    _asm
      {
        mov        esi, matrix
        mov        edi, this
        mov        eax, [esi + 0*4]
        mov        ebx, [esi + 1*4]
        mov        ecx, [esi + 2*4]
        mov        edx, [esi + 3*4]
        mov        [edi + 0*4], eax
        mov        [edi + 1*4], ebx
        mov        [edi + 2*4], ecx
        mov        [edi + 3*4], edx
        mov        eax, [esi + 4*4]
        mov        ebx, [esi + 5*4]
        mov        ecx, [esi + 6*4]
        mov        edx, [esi + 7*4]
        mov        [edi + 4*4], eax
        mov        [edi + 5*4], ebx
        mov        [edi + 6*4], ecx
        mov        [edi + 7*4], edx
        mov        eax, [esi + 8*4]
        mov        ebx, [esi + 9*4]
        mov        ecx, [esi + 10*4]
        mov        edx, [esi + 11*4]
        mov        [edi + 8*4], eax
        mov        [edi + 9*4], ebx
        mov        [edi + 10*4], ecx
        mov        [edi + 11*4], edx
        mov        eax, [esi + 12*4]
        mov        ebx, [esi + 13*4]
        mov        ecx, [esi + 14*4]
        mov        edx, [esi + 15*4]
        mov        [edi + 12*4], eax
        mov        [edi + 13*4], ebx
        mov        [edi + 14*4], ecx
        mov        [edi + 15*4], edx
      }*/
    return *this;
}

// Calculate matrix M = rotZ * rotX * rotY * Pos
inline Matrix &Matrix::Build(float angX, float angY, float angZ, float x, float y, float z)
{
    // Sines and cosines of rotation angles
    const auto sinAx = sinf(angX);
    const auto cosAx = cosf(angX);
    const auto sinAy = sinf(angY);
    const auto cosAy = cosf(angY);
    const auto sinAz = sinf(angZ);
    const auto cosAz = cosf(angZ);
    // Create a matrix with rotation order rz * rx * ry
    m[0][0] = cosAz * cosAy + sinAz * sinAx * sinAy;  // vx.x
    m[0][1] = sinAz * cosAx;                          // vx.y
    m[0][2] = cosAz * -sinAy + sinAz * sinAx * cosAy; // vx.z
    m[0][3] = 0.0f;
    m[1][0] = -sinAz * cosAy + cosAz * sinAx * sinAy;  // vy.x
    m[1][1] = cosAz * cosAx;                           // vy.y
    m[1][2] = -sinAz * -sinAy + cosAz * sinAx * cosAy; // vy.z
    m[1][3] = 0.0f;
    m[2][0] = cosAx * sinAy; // vz.x
    m[2][1] = -sinAx;        // vz.y
    m[2][2] = cosAx * cosAy; // vz.z
    m[2][3] = 0.0f;
    m[3][0] = x; // pos.x
    m[3][1] = y; // pos.y
    m[3][2] = z; // pos.z
    m[3][3] = 1.0f;
    return *this;
}

// Calculate matrix M = rotZ * rotX * rotY
inline Matrix &Matrix::Build(float angX, float angY, float angZ)
{
    Build(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
    return *this;
}

// Calculate matrix M = rotZ * rotX * rotY * Pos
inline Matrix &Matrix::Build(const Vector &ang, const Vector &pos)
{
    Build(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
    return *this;
}

// Calculate matrix M = rotZ * rotX * rotY
inline Matrix &Matrix::Build(const Vector &ang)
{
    Build(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
    return *this;
}

// Calculate matrix M = rotX * rotY * rotZ * Pos
inline Matrix &Matrix::BuildXYZ(float angX, float angY, float angZ, float x, float y, float z)
{
    // Sines and cosines of rotation angles
    const auto sinAx = sinf(angX);
    const auto cosAx = cosf(angX);
    const auto sinAy = sinf(angY);
    const auto cosAy = cosf(angY);
    const auto sinAz = sinf(angZ);
    const auto cosAz = cosf(angZ);
    // Create a matrix with rotation order rx * ry * rz
    m[0][0] = cosAy * cosAz;
    m[0][1] = cosAy * sinAz;
    m[0][2] = -sinAy;
    m[0][3] = 0.0f;
    m[1][0] = sinAx * sinAy * cosAz - cosAx * sinAz;
    m[1][1] = sinAx * sinAy * sinAz + cosAx * cosAz;
    m[1][2] = sinAx * cosAy;
    m[1][3] = 0.0f;
    m[2][0] = cosAx * sinAy * cosAz + sinAx * sinAz;
    m[2][1] = cosAx * sinAy * sinAz - sinAx * cosAz;
    m[2][2] = cosAx * cosAy;
    m[2][3] = 0.0f;
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
    m[3][3] = 1.0f;
    return *this;
}

// Calculate rotation matrix around X
inline Matrix &Matrix::BuildRotateX(float ang)
{
    SetIdentity();
    m[1][1] = cosf(ang);
    m[1][2] = sinf(ang);
    m[2][1] = -sinf(ang);
    m[2][2] = cosf(ang);
    return *this;
}

// Calculate rotation matrix around Y
inline Matrix &Matrix::BuildRotateY(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[0][2] = -sinf(ang);
    m[2][0] = sinf(ang);
    m[2][2] = cosf(ang);
    return *this;
}

// Calculate the rotation matrix around Z
inline Matrix &Matrix::BuildRotateZ(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[0][1] = sinf(ang);
    m[1][0] = -sinf(ang);
    m[1][1] = cosf(ang);
    return *this;
}

// Calculate position matrix
inline Matrix &Matrix::BuildPosition(float x, float y, float z)
{
    SetIdentity();
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
    return *this;
}

// Calculate position matrix
inline Matrix &Matrix::BuildPosition(const Vector &pos)
{
    BuildPosition(pos.x, pos.y, pos.z);
    return *this;
}

// Calculate scaling matrix
inline Matrix &Matrix::BuildScale(float scale)
{
    SetIdentity();
    m[0][0] = scale;
    m[1][1] = scale;
    m[2][2] = scale;
    return *this;
}

// Calculate scaling matrix
inline Matrix &Matrix::BuildScale(float scaleX, float scaleY, float scaleZ)
{
    SetIdentity();
    m[0][0] = scaleX;
    m[1][1] = scaleY;
    m[2][2] = scaleZ;
    return *this;
}

// Calculate scaling matrix
inline Matrix &Matrix::BuildScale(const Vector &scale)
{
    BuildScale(scale.x, scale.y, scale.z);
    return *this;
}

// Calculate projection matrix
inline Matrix &Matrix::BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
    // zero down the array
    matrix[0] = 0;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 0;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 0;

    /*    _asm
      {
        mov        eax, this
        xor        ebx, ebx
        xor        edx, edx
        mov        [eax + 0*4], ebx
        mov        [eax + 1*4], edx
        mov        [eax + 2*4], ebx
        mov        [eax + 3*4], edx
        mov        [eax + 4*4], ebx
        mov        [eax + 5*4], edx
        mov        [eax + 6*4], ebx
        mov        [eax + 7*4], edx
        mov        [eax + 8*4], ebx
        mov        [eax + 9*4], edx
        mov        [eax + 10*4], ebx
        mov        [eax + 11*4], edx
        mov        [eax + 12*4], ebx
        mov        [eax + 13*4], edx
        mov        [eax + 14*4], ebx
        mov        [eax + 15*4], edx
      }*/
    // Filling in the matrix
    const auto Q = static_cast<double>(zFar) / static_cast<double>(zFar - zNear);
    m[0][0] = static_cast<float>(1.0 / tan(viewAngle * 0.5));
    m[1][1] = static_cast<float>(1.0 / tan((vpHeight / vpWidth) * viewAngle * 0.5));
    m[2][2] = static_cast<float>(Q);
    m[2][3] = 1.0f;
    m[3][2] = static_cast<float>(-Q * zNear);
    return *this;
}

// Calculate Orthogonal Projection Matrix
inline Matrix &Matrix::BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar)
{
    // zero down the array
    matrix[0] = 0;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 0;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 0;
    /*    _asm
      {
        mov        eax, this
          xor        ebx, ebx
          xor        edx, edx
          mov        [eax + 0*4], ebx
          mov        [eax + 1*4], edx
          mov        [eax + 2*4], ebx
          mov        [eax + 3*4], edx
          mov        [eax + 4*4], ebx
          mov        [eax + 5*4], edx
          mov        [eax + 6*4], ebx
          mov        [eax + 7*4], edx
          mov        [eax + 8*4], ebx
          mov        [eax + 9*4], edx
          mov        [eax + 10*4], ebx
          mov        [eax + 11*4], edx
          mov        [eax + 12*4], ebx
          mov        [eax + 13*4], edx
          mov        [eax + 14*4], ebx
          mov        [eax + 15*4], edx
      }*/
    // Filling in the matrix
    const auto Q = 1.0 / static_cast<double>(zFar - zNear);
    m[0][0] = 2.0f / vpWidth;
    m[1][1] = 2.0f / vpHeight;
    m[2][2] = static_cast<float>(Q);
    m[3][2] = static_cast<float>(-Q * zNear);
    m[3][3] = 1.0f;

    return *this;
}

// Calculate projection matrix for shadow
inline Matrix &Matrix::BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
    // zero down the array
    matrix[0] = 0;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 0;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 0;
    /*    _asm
      {
        mov        eax, this
          xor        ebx, ebx
          xor        edx, edx
          mov        [eax + 0*4], ebx
          mov        [eax + 1*4], edx
          mov        [eax + 2*4], ebx
          mov        [eax + 3*4], edx
          mov        [eax + 4*4], ebx
          mov        [eax + 5*4], edx
          mov        [eax + 6*4], ebx
          mov        [eax + 7*4], edx
          mov        [eax + 8*4], ebx
          mov        [eax + 9*4], edx
          mov        [eax + 10*4], ebx
          mov        [eax + 11*4], edx
          mov        [eax + 12*4], ebx
          mov        [eax + 13*4], edx
          mov        [eax + 14*4], ebx
          mov        [eax + 15*4], edx
      }*/
    // Filling in the matrix
    const auto Q = 1.0 / static_cast<double>(zFar - zNear);
    m[0][0] = static_cast<float>(1.0 / tan(viewAngle * 0.5));
    m[1][1] = static_cast<float>(1.0 / tan((vpHeight / vpWidth) * viewAngle * 0.5));
    m[2][2] = static_cast<float>(Q);
    m[2][3] = 1.0f;
    m[3][2] = static_cast<float>(-Q * zNear);
    return *this;
}

// Calculate camera matrix
inline bool Matrix::BuildView(Vector lookFrom, Vector lookTo, Vector upVector)
{
    SetIdentity();
    // Normalize the look-at vector
    lookTo -= lookFrom;
    if (lookTo.Normalize() == 0.0f)
    {
        // Putting a position for a non-rotated matrix
        v.pos = -lookFrom;
        return false;
    }
    // Directing the vector up in the desired direction
    upVector -= lookTo * (lookTo | upVector);
    // Normalize the upward vector
    if (upVector.Normalize() == 0.0f)
        upVector.y = 1.0f;
    // looking for the third vector of the basis
    auto vec = upVector ^ lookTo;
    if (vec.Normalize() != 0.0f)
    {
        // set the rotation matrix
        m[0][0] = vec.x;
        m[1][0] = vec.y;
        m[2][0] = vec.z;
        m[0][1] = upVector.x;
        m[1][1] = upVector.y;
        m[2][1] = upVector.z;
        m[0][2] = lookTo.x;
        m[1][2] = lookTo.y;
        m[2][2] = lookTo.z;
    }
    else
    {
        // Setting a position for a non-rotated matrix
        v.pos = -lookFrom;
        return false;
    }
    // set the position
    // pos = -MulNormalByInverse (lookFrom);
    v.pos = -MulNormal(lookFrom);
    return true;
}

// Calculate the orientation matrix of an object having a z direction and an up direction
inline bool Matrix::BuildOrient(Vector zAxisDirection, Vector upVector)
{
    // Normalize the direction vector z
    if (zAxisDirection.Normalize() < 1e-37f || upVector.Normalize() < 1e-37f)
    {
        v.vx = Vector(1.0f, 0.0f, 0.0f);
        v.vy = Vector(0.0f, 1.0f, 0.0f);
        v.vz = Vector(0.0f, 0.0f, 1.0f);
        return false;
    }
    // calculate
    v.vx = zAxisDirection ^ upVector;
    if (v.vx.Normalize() == 0.0f)
    {
        v.vx = Vector(1.0f, 0.0f, 0.0f);
        v.vy = Vector(0.0f, 1.0f, 0.0f);
        v.vz = Vector(0.0f, 0.0f, 1.0f);
        return false;
    }
    v.vy = zAxisDirection ^ v.vx;
    v.vz = zAxisDirection;
    return true;
}

// Calculate the matrix of the object having the position of the point where the object is directed and the direction up
inline bool Matrix::BuildOriented(Vector position, Vector lookTo, Vector upVector)
{
    // Direction
    lookTo -= position;
    // Normalize the direction vector z
    if (lookTo.Normalize() == 0.0f || upVector.Normalize() == 0.0f)
    {
        v.vx = Vector(1.0f, 0.0f, 0.0f);
        v.wx = 0.0f;
        v.vy = Vector(0.0f, 1.0f, 0.0f);
        v.wy = 0.0f;
        v.vz = Vector(0.0f, 0.0f, 1.0f);
        v.wz = 0.0f;
        v.pos = position;
        v.w = 1.0f;
        return false;
    }
    // calculate
    v.vx = lookTo ^ upVector;
    v.wx = 0.0f;
    if (v.vx.Normalize() == 0.0f)
    {
        v.vx = Vector(1.0f, 0.0f, 0.0f);
        v.wx = 0.0f;
        v.vy = Vector(0.0f, 1.0f, 0.0f);
        v.wy = 0.0f;
        v.vz = Vector(0.0f, 0.0f, 1.0f);
        v.wz = 0.0f;
        v.pos = position;
        v.w = 1.0f;
        return false;
    }
    v.vy = lookTo ^ v.vx;
    v.wy = 0.0f;
    v.vz = lookTo;
    v.wz = 0.0f;
    v.pos = position;
    v.w = 1.0f;
    return true;
}

// Calculate matrix for mirroring geometry
inline Matrix &Matrix::BuildMirror(float Nx, float Ny, float Nz, float D)
{
    m[0][0] = -Nx * 2.0f * Nx + 1.0f;
    m[0][1] = -Ny * 2.0f * Nx;
    m[0][2] = -Nz * 2.0f * Nx;
    m[0][3] = 0.0f;
    m[1][0] = -Nx * 2.0f * Ny;
    m[1][1] = -Ny * 2.0f * Ny + 1.0f;
    m[1][2] = -Nz * 2.0f * Ny;
    m[1][3] = 0.0f;
    m[2][0] = -Nx * 2.0f * Nz;
    m[2][1] = -Ny * 2.0f * Nz;
    m[2][2] = -Nz * 2.0f * Nz + 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = -Nx * 2.0f * -D;
    m[3][1] = -Ny * 2.0f * -D;
    m[3][2] = -Nz * 2.0f * -D;
    m[3][3] = 1.0f;
    return *this;
}

// -----------------------------------------------------------
// Matrix transformation
// -----------------------------------------------------------

// Rotate around X
inline Matrix &Matrix::RotateX(float ang)
{
    Matrix m;
    m.BuildRotateX(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

// Rotate around Y
inline Matrix &Matrix::RotateY(float ang)
{
    Matrix m;
    m.BuildRotateY(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

// Rotate around Z
inline Matrix &Matrix::RotateZ(float ang)
{
    Matrix m;
    m.BuildRotateZ(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

// Rotate around ZXY
inline Matrix &Matrix::Rotate(float angX, float angY, float angZ)
{
    Matrix m;
    m.Build(angX, angY, angZ);
    EqMultiply(Matrix(*this), m);
    return *this;
}

// Rotate around ZXY
inline Matrix &Matrix::Rotate(const Vector &ang)
{
    Matrix m;
    m.Build(ang.x, ang.x, ang.z);
    EqMultiply(Matrix(*this), m);
    return *this;
}

// Move
inline Matrix &Matrix::Move(float dX, float dY, float dZ)
{
    v.pos.x += dX;
    v.pos.y += dY;
    v.pos.z += dZ;
    return *this;
}

// Move
inline Matrix &Matrix::Move(const Vector &pos)
{
    this->v.pos.x += pos.x;
    this->v.pos.y += pos.y;
    this->v.pos.z += pos.z;
    return *this;
}

// Scale
inline Matrix &Matrix::Scale(float scale)
{
    Scale(scale, scale, scale);
    return *this;
}

// Scale rotation matrix
inline Matrix &Matrix::Scale3x3(float scale)
{
    Scale3x3(scale, scale, scale);
    return *this;
}

// Scale
inline Matrix &Matrix::Scale(float scaleX, float scaleY, float scaleZ)
{
    m[0][0] *= scaleX;
    m[1][0] *= scaleX;
    m[2][0] *= scaleX;
    m[3][0] *= scaleX;
    m[0][1] *= scaleY;
    m[1][1] *= scaleY;
    m[2][1] *= scaleY;
    m[3][1] *= scaleY;
    m[0][2] *= scaleZ;
    m[1][2] *= scaleZ;
    m[2][2] *= scaleZ;
    m[3][2] *= scaleZ;
    return *this;
}

// Scale rotation matrix
inline Matrix &Matrix::Scale3x3(float scaleX, float scaleY, float scaleZ)
{
    m[0][0] *= scaleX;
    m[1][0] *= scaleX;
    m[2][0] *= scaleX;
    m[0][1] *= scaleY;
    m[1][1] *= scaleY;
    m[2][1] *= scaleY;
    m[0][2] *= scaleZ;
    m[1][2] *= scaleZ;
    m[2][2] *= scaleZ;
    return *this;
}

// Scale
inline Matrix &Matrix::Scale(const Vector &scale)
{
    Scale(scale.x, scale.y, scale.z);
    return *this;
}

// Scale Rotation
inline Matrix &Matrix::Scale3x3(const Vector &scale)
{
    Scale3x3(scale.x, scale.y, scale.z);
    return *this;
}

// Calculating the inverse matrix
inline Matrix &Matrix::Inverse()
{
    v.pos = Vector(-(v.pos | v.vx), -(v.pos | v.vy), -(v.pos | v.vz));
    Transposition3X3();
    return *this;
}

// Calculating an inverse matrix from another
inline Matrix &Matrix::Inverse(const Matrix &mtx)
{
    v.pos = Vector(-(mtx.v.pos | mtx.v.vx), -(mtx.v.pos | mtx.v.vy), -(mtx.v.pos | mtx.v.vz));
    matrix[0] = mtx.matrix[0];
    matrix[1] = mtx.matrix[4];
    matrix[2] = mtx.matrix[8];
    matrix[4] = mtx.matrix[1];
    matrix[5] = mtx.matrix[5];
    matrix[6] = mtx.matrix[9];
    matrix[8] = mtx.matrix[2];
    matrix[9] = mtx.matrix[6];
    matrix[10] = mtx.matrix[10];

    /*    _asm
      {
        mov        eax, mtx
        mov        ebx, this
        mov        edx, [eax + 0*4]
        mov        ecx, [eax + 1*4]
        mov        esi, [eax + 2*4]
        mov        [ebx + 0*4], edx
        mov        [ebx + 4*4], ecx
        mov        [ebx + 8*4], esi
        mov        edx, [eax + 4*4]
        mov        ecx, [eax + 5*4]
        mov        esi, [eax + 6*4]
        mov        [ebx + 1*4], edx
        mov        [ebx + 5*4], ecx
        mov        [ebx + 9*4], esi
        mov        edx, [eax + 8*4]
        mov        ecx, [eax + 9*4]
        mov        esi, [eax + 10*4]
        mov        [ebx + 2*4], edx
        mov        [ebx + 6*4], ecx
        mov        [ebx + 10*4], esi
      }*/
    return *this;
}

// Calculating the scaled inverse matrix
inline Matrix &Matrix::InverseWhithScale()
{
    // Rotation matrix
    double mtmp[3][3];
    mtmp[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    mtmp[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
    mtmp[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
    mtmp[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
    mtmp[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
    mtmp[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
    mtmp[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
    mtmp[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
    mtmp[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    auto det = m[0][0] * mtmp[0][0] + m[0][1] * mtmp[1][0] + m[0][2] * mtmp[2][0];
    if (fabs(det) > 1e-200)
    {
        det = 1.0 / det;
        m[0][0] = static_cast<float>(det * mtmp[0][0]);
        m[0][1] = static_cast<float>(det * mtmp[0][1]);
        m[0][2] = static_cast<float>(det * mtmp[0][2]);
        m[1][0] = static_cast<float>(det * mtmp[1][0]);
        m[1][1] = static_cast<float>(det * mtmp[1][1]);
        m[1][2] = static_cast<float>(det * mtmp[1][2]);
        m[2][0] = static_cast<float>(det * mtmp[2][0]);
        m[2][1] = static_cast<float>(det * mtmp[2][1]);
        m[2][2] = static_cast<float>(det * mtmp[2][2]);
    }
    else
    {
        for (int32_t i = 0; i < 16; i++)
            matrix[i] = 0.0f;
    }
    // Position
    v.pos = -(MulNormal(v.pos));
    return *this;
}

// Matrix Transposition
inline Matrix &Matrix::Transposition()
{
    float tmp;
    tmp = matrix[1];
    matrix[1] = matrix[4];
    matrix[4] = tmp;
    tmp = matrix[2];
    matrix[2] = matrix[8];
    matrix[8] = tmp;
    tmp = matrix[3];
    matrix[3] = matrix[12];
    matrix[12] = tmp;
    tmp = matrix[6];
    matrix[6] = matrix[9];
    matrix[9] = tmp;
    tmp = matrix[7];
    matrix[7] = matrix[13];
    matrix[13] = tmp;
    tmp = matrix[11];
    matrix[11] = matrix[14];
    matrix[14] = tmp;

    /*    _asm
      {
        mov        eax, this
        mov        ebx, [eax + 1*4]
        mov        ecx, [eax + 2*4]
        mov        esi, [eax + 4*4]
        mov        edi, [eax + 8*4]
        mov        [eax + 4*4], ebx
        mov        [eax + 8*4], ecx
        mov        [eax + 1*4], esi
        mov        [eax + 2*4], edi
        mov        ebx, [eax + 3*4]
        mov        ecx, [eax + 6*4]
        mov        esi, [eax + 12*4]
        mov        edi, [eax + 9*4]
        mov        [eax + 12*4], ebx
        mov        [eax + 9*4], ecx
        mov        [eax + 3*4], esi
        mov        [eax + 6*4], edi
        mov        ebx, [eax + 7*4]
        mov        ecx, [eax + 11*4]
        mov        esi, [eax + 13*4]
        mov        edi, [eax + 14*4]
        mov        [eax + 13*4], ebx
        mov        [eax + 14*4], ecx
        mov        [eax + 7*4], esi
        mov        [eax + 11*4], edi
      }*/
    return *this;
}

// Rotation Matrix Transposition
inline Matrix &Matrix::Transposition3X3()
{
    float tmp;
    tmp = matrix[1];
    matrix[1] = matrix[4];
    matrix[4] = tmp;
    tmp = matrix[2];
    matrix[2] = matrix[8];
    matrix[8] = tmp;
    tmp = matrix[6];
    matrix[6] = matrix[9];
    matrix[9] = tmp;

    /*    _asm
      {
        mov        eax, this
        mov        ebx, [eax + 4*4]
        mov        ecx, [eax + 8*4]
        mov        esi, [eax + 1*4]
        mov        edi, [eax + 2*4]
        mov        [eax + 1*4], ebx
        mov        [eax + 2*4], ecx
        mov        [eax + 4*4], esi
        mov        [eax + 8*4], edi
        mov        ebx, [eax + 6*4]
        mov        esi, [eax + 9*4]
        mov        [eax + 9*4], ebx
        mov        [eax + 6*4], esi
      }*/
    return *this;
}

// -----------------------------------------------------------
// Utilities
// -----------------------------------------------------------

// Set rotation only
inline Matrix &Matrix::SetRotate(const Matrix &mtx)
{
    matrix[0] = mtx.matrix[0];
    matrix[1] = mtx.matrix[1];
    matrix[2] = mtx.matrix[2];
    matrix[4] = mtx.matrix[4];
    matrix[5] = mtx.matrix[5];
    matrix[6] = mtx.matrix[6];
    matrix[8] = mtx.matrix[8];
    matrix[9] = mtx.matrix[9];
    matrix[10] = mtx.matrix[10];

    /*    _asm
      {
        mov        esi, mtx
        mov        edi, this
        mov        ebx, [esi + 0*4]
        mov        ecx, [esi + 1*4]
        mov        edx, [esi + 2*4]
        mov        [edi + 0*4], ebx
        mov        [edi + 1*4], ecx
        mov        [edi + 2*4], edx
        mov        ebx, [esi + 4*4]
        mov        ecx, [esi + 5*4]
        mov        edx, [esi + 6*4]
        mov        [edi + 4*4], ebx
        mov        [edi + 5*4], ecx
        mov        [edi + 6*4], edx
        mov        ebx, [esi + 8*4]
        mov        ecx, [esi + 9*4]
        mov        edx, [esi + 10*4]
        mov        [edi + 8*4], ebx
        mov        [edi + 9*4], ecx
        mov        [edi + 10*4], edx
      }*/
    return *this;
}

// Multiply matrices and place the result into the current one
inline Matrix &Matrix::EqMultiply(const Matrix &m1, const Matrix &m2)
{
    Matrix m;
    m.EqMultiplyFast(m1, m2);
    Set(m);
    return *this;
}

// Multiply matrices and place the result in the current m1 != this && m2 != this
inline Matrix &Matrix::EqMultiplyFast(const Matrix &m1, const Matrix &m2)
{
    m[0][0] = m2.m[0][0] * m1.m[0][0] + m2.m[1][0] * m1.m[0][1] + m2.m[2][0] * m1.m[0][2] + m2.m[3][0] * m1.m[0][3];
    m[0][1] = m2.m[0][1] * m1.m[0][0] + m2.m[1][1] * m1.m[0][1] + m2.m[2][1] * m1.m[0][2] + m2.m[3][1] * m1.m[0][3];
    m[0][2] = m2.m[0][2] * m1.m[0][0] + m2.m[1][2] * m1.m[0][1] + m2.m[2][2] * m1.m[0][2] + m2.m[3][2] * m1.m[0][3];
    m[0][3] = m2.m[0][3] * m1.m[0][0] + m2.m[1][3] * m1.m[0][1] + m2.m[2][3] * m1.m[0][2] + m2.m[3][3] * m1.m[0][3];
    m[1][0] = m2.m[0][0] * m1.m[1][0] + m2.m[1][0] * m1.m[1][1] + m2.m[2][0] * m1.m[1][2] + m2.m[3][0] * m1.m[1][3];
    m[1][1] = m2.m[0][1] * m1.m[1][0] + m2.m[1][1] * m1.m[1][1] + m2.m[2][1] * m1.m[1][2] + m2.m[3][1] * m1.m[1][3];
    m[1][2] = m2.m[0][2] * m1.m[1][0] + m2.m[1][2] * m1.m[1][1] + m2.m[2][2] * m1.m[1][2] + m2.m[3][2] * m1.m[1][3];
    m[1][3] = m2.m[0][3] * m1.m[1][0] + m2.m[1][3] * m1.m[1][1] + m2.m[2][3] * m1.m[1][2] + m2.m[3][3] * m1.m[1][3];
    m[2][0] = m2.m[0][0] * m1.m[2][0] + m2.m[1][0] * m1.m[2][1] + m2.m[2][0] * m1.m[2][2] + m2.m[3][0] * m1.m[2][3];
    m[2][1] = m2.m[0][1] * m1.m[2][0] + m2.m[1][1] * m1.m[2][1] + m2.m[2][1] * m1.m[2][2] + m2.m[3][1] * m1.m[2][3];
    m[2][2] = m2.m[0][2] * m1.m[2][0] + m2.m[1][2] * m1.m[2][1] + m2.m[2][2] * m1.m[2][2] + m2.m[3][2] * m1.m[2][3];
    m[2][3] = m2.m[0][3] * m1.m[2][0] + m2.m[1][3] * m1.m[2][1] + m2.m[2][3] * m1.m[2][2] + m2.m[3][3] * m1.m[2][3];
    m[3][0] = m2.m[0][0] * m1.m[3][0] + m2.m[1][0] * m1.m[3][1] + m2.m[2][0] * m1.m[3][2] + m2.m[3][0] * m1.m[3][3];
    m[3][1] = m2.m[0][1] * m1.m[3][0] + m2.m[1][1] * m1.m[3][1] + m2.m[2][1] * m1.m[3][2] + m2.m[3][1] * m1.m[3][3];
    m[3][2] = m2.m[0][2] * m1.m[3][0] + m2.m[1][2] * m1.m[3][1] + m2.m[2][2] * m1.m[3][2] + m2.m[3][2] * m1.m[3][3];
    m[3][3] = m2.m[0][3] * m1.m[3][0] + m2.m[1][3] * m1.m[3][1] + m2.m[2][3] * m1.m[3][2] + m2.m[3][3] * m1.m[3][3];
    return *this;
}

// Multiply vertex by matrix
inline Vector Matrix::MulVertex(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
    tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
    tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
    return tv;
}

// Multiply normal by matrix
inline Vector Matrix::MulNormal(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
    tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
    tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;
    return tv;
}

// Multiply vertex by inverse matrix
inline Vector Matrix::MulVertexByInverse(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * (v.x - m[3][0]) + m[0][1] * (v.y - m[3][1]) + m[0][2] * (v.z - m[3][2]);
    tv.y = m[1][0] * (v.x - m[3][0]) + m[1][1] * (v.y - m[3][1]) + m[1][2] * (v.z - m[3][2]);
    tv.z = m[2][0] * (v.x - m[3][0]) + m[2][1] * (v.y - m[3][1]) + m[2][2] * (v.z - m[3][2]);
    return tv;
}

// Multiply normal by inverse matrix
inline Vector Matrix::MulNormalByInverse(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
    tv.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
    tv.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
    return tv;
}

// Get camera position from camera matrix
inline Vector Matrix::GetCamPos() const
{
    return -MulNormalByInverse(v.pos);
}

// Identity matrix or not
inline bool Matrix::IsIdentity() const
{
    const auto eps = 1e-4f;
    if (fabs(m[0][0] - 1.0f) > eps)
        return false;
    if (fabs(m[0][1] - 0.0f) > eps)
        return false;
    if (fabs(m[0][2] - 0.0f) > eps)
        return false;
    if (fabs(m[0][3] - 0.0f) > eps)
        return false;
    if (fabs(m[1][0] - 0.0f) > eps)
        return false;
    if (fabs(m[1][1] - 1.0f) > eps)
        return false;
    if (fabs(m[1][2] - 0.0f) > eps)
        return false;
    if (fabs(m[1][3] - 0.0f) > eps)
        return false;
    if (fabs(m[2][0] - 0.0f) > eps)
        return false;
    if (fabs(m[2][1] - 0.0f) > eps)
        return false;
    if (fabs(m[2][2] - 1.0f) > eps)
        return false;
    if (fabs(m[2][3] - 0.0f) > eps)
        return false;
    if (fabs(m[3][0] - 0.0f) > eps)
        return false;
    if (fabs(m[3][1] - 0.0f) > eps)
        return false;
    if (fabs(m[3][2] - 0.0f) > eps)
        return false;
    if (fabs(m[3][3] - 1.0f) > eps)
        return false;
    return true;
}

// Scaling matrix or not
inline bool Matrix::IsScale() const
{
    const auto eps = 1e-4f;
    if (fabsf(~v.vx - 1.0f) > eps)
        return true;
    if (fabsf(~v.vy - 1.0f) > eps)
        return true;
    if (fabsf(~v.vz - 1.0f) > eps)
        return true;
    if (fabsf(v.w - 1.0f) > eps)
        return true;
    return false;
}

// Project Vertex (for projection matrix)
inline Vector4 Matrix::Projection(Vector vertex, float vphWidth05, float vphHeight05) const
{
    Vector4 res;
    // Transforming the vertex
    res.x = m[0][0] * vertex.x + m[1][0] * vertex.y + m[2][0] * vertex.z + m[3][0];
    res.y = m[0][1] * vertex.x + m[1][1] * vertex.y + m[2][1] * vertex.z + m[3][1];
    res.z = m[0][2] * vertex.x + m[1][2] * vertex.y + m[2][2] * vertex.z + m[3][2];
    res.w = m[0][3] * vertex.x + m[1][3] * vertex.y + m[2][3] * vertex.z + m[3][3];
    // Normalization factor
    const auto w = 1.0f / res.w;
    // Normalize
    res.x = (1.0f + res.x * w) * vphWidth05;
    res.y = (1.0f - res.y * w) * vphHeight05;
    res.z *= w;
    res.w = w;
    return res;
}

// Project vertex array (for projection matrix)
inline void Matrix::Projection(Vector4 *dstArray, Vector *srcArray, int32_t num, float vphWidth05, float vphHeight05,
                               int32_t srcSize, int32_t dstSize) const
{
    for (; num > 0; num--)
    {
        // Transforming the vertex
        dstArray->x = m[0][0] * srcArray->x + m[1][0] * srcArray->y + m[2][0] * srcArray->z + m[3][0];
        dstArray->y = m[0][1] * srcArray->x + m[1][1] * srcArray->y + m[2][1] * srcArray->z + m[3][1];
        dstArray->z = m[0][2] * srcArray->x + m[1][2] * srcArray->y + m[2][2] * srcArray->z + m[3][2];
        dstArray->w = m[0][3] * srcArray->x + m[1][3] * srcArray->y + m[2][3] * srcArray->z + m[3][3];
        // Normalization factor
        const auto w = 1.0f / dstArray->w;
        // Normalize
        dstArray->x = (1.0f + dstArray->x * w) * vphWidth05;
        dstArray->y = (1.0f - dstArray->y * w) * vphHeight05;
        dstArray->z *= w;
        dstArray->w = w;
        // Pointers to the next vertices
        srcArray = (Vector *)((char *)srcArray + srcSize);
        dstArray = (Vector4 *)((char *)dstArray + dstSize);
    }
}

// Get angles from unscaled rotation matrix
inline void Matrix::GetAngles(float &ax, float &ay, float &az) const
{
    if (v.vz.y < 1.0f)
    {
        if (v.vz.y > -1.0f)
        {
            ax = static_cast<float>(asin(-v.vz.y));
            ay = static_cast<float>(atan2(v.vz.x, v.vz.z));
            az = static_cast<float>(atan2(v.vx.y, v.vy.y));
            return;
        }
        ax = 3.141592654f * 0.5f;
        ay = 0.0f;
        az = static_cast<float>(atan2(v.vx.z, v.vx.x));
    }
    else
    {
        ax = -3.141592654f * 0.5f;
        ay = 0.0f;
        az = static_cast<float>(-atan2(v.vx.z, v.vx.x));
    }
}

// Get angles from unscaled rotation matrix
inline void Matrix::GetAngles(Vector &ang) const
{
    GetAngles(ang.x, ang.y, ang.z);
}

// Access matrix elements with brackets
inline float &Matrix::operator()(int32_t i, int32_t j)
{
    return m[i][j];
}

// Get a pointer to a D3D matrix
inline Matrix::operator D3DMATRIX *() const
{
    return ((D3DMATRIX *)matrix);
}

// Get a vector for calculating the X component
inline Vector4 Matrix::GetVectorX() const
{
    return Vector4(m[0][0], m[1][0], m[2][0], m[3][0]);
}

// Get a vector for calculating the Y component
inline Vector4 Matrix::GetVectorY() const
{
    return Vector4(m[0][1], m[1][1], m[2][1], m[3][1]);
}

// Get a vector for calculating the Z component
inline Vector4 Matrix::GetVectorZ() const
{
    return Vector4(m[0][2], m[1][2], m[2][2], m[3][2]);
}

// Get a vector for calculating the W component
inline Vector4 Matrix::GetVectorW() const
{
    return Vector4(m[0][3], m[1][3], m[2][3], m[3][3]);
}

#pragma pack(pop)
