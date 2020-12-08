#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "cvector.h"

#include <cstring>

//============================================================================================

/*
    0  4  8 12	   [0][0] [1][0] [2][0] [3][0]
    1  5  9 13	   [0][1] [1][1] [2][1] [3][1]
    2  6 10 14	   [0][2] [1][2] [2][2] [3][2]
    3  7 11 15	   [0][3] [1][3] [2][3] [3][3]
*/

//============================================================================================

struct MTX_PRJ_VECTOR
{
    float x, y, z;
    float rhw;
};

struct D3DXMATRIX;

//============================================================================================

class CMatrix
{
  public:
    union {
        float matrix[16];
        float m[4][4];
        struct
        {
            ///Направление по X
            CVECTOR vx;
            ///Весовое значение по X
            float wx;
            ///Направление по Y
            CVECTOR vy;
            ///Весовое значение по Y
            float wy;
            ///Направление по Z
            CVECTOR vz;
            ///Весовое значение по Z
            float wz;
            ///Позиция
            CVECTOR pos;
            //Добавляемое весовое значение
            float w;
        };
    };

    //-----------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------
    CMatrix(); // Identity matrix
    CMatrix(float angX, float angY, float angZ, float x, float y, float z);
    CMatrix(float angX, float angY, float angZ);
    CMatrix(const CVECTOR &ang, const CVECTOR &pos);
    CMatrix(CVECTOR &ang);
    CMatrix(const CMatrix &matrix);
    CMatrix(CMatrix *matrix);
    // this = m1*m2
    CMatrix(CMatrix &m1, CMatrix &m2);

    //-----------------------------------------------------------
    // Main
    //-----------------------------------------------------------
    // Create identity matrix
    void SetIdentity();

    // Build matrix
    // M = rotZ*rotX*rotY*Pos
    void BuildMatrix(float angX, float angY, float angZ, float x, float y, float z);
    void BuildMatrix(float angX, float angY, float angZ);
    void BuildMatrix(CVECTOR &ang, CVECTOR &pos);
    void BuildMatrix(CVECTOR &ang);
    // M = rotX*rotY*rotZ*Pos
    void BuildMatrixXYZ(float angX, float angY, float angZ, float x, float y, float z);

    // Build rotate matrix
    void BuildRotateX(float ang);
    void BuildRotateY(float ang);
    void BuildRotateZ(float ang);
    // Build position matrix
    void BuildPosition(float x, float y, float z);

    ///Посчитать матрицу масштабирования
    CMatrix &BuildScale(float scale);
    //Посчитать матрицу масштабирования
    CMatrix &BuildScale(float scaleX, float scaleY, float scaleZ);
    ///Посчитать матрицу масштабирования
    CMatrix &BuildScale(const CVECTOR &scale);

    // Equal
    void operator=(const CMatrix &matrix);

    // Multiply
    void operator*=(CMatrix &matrix);
    void operator*=(float k);
    CMatrix operator*(CMatrix &matrix);
    CVECTOR operator*(CVECTOR &vector);

    // this = m1*m2, (m1 != this, m2 != this)
    void EqMultiply(CMatrix &m1, CMatrix &m2);
    // Transform vertex to local coordinate system
    void MulToInv(const CVECTOR &srcVrt, CVECTOR &resVrt);
    // Transform normal to local coordinate system
    void MulToInvNorm(CVECTOR &srcNorm, CVECTOR &resNorm);

    CMatrix &Inverse();

    // Transposition
    void Transposition();
    void Transposition3X3();
    ///Транспанирование матрицы
    CMatrix &TranspositionX();
    CMatrix &__fastcall Transposition4x4();

    // Rotate
    void RotateX(float ang);
    void RotateY(float ang);
    void RotateZ(float ang);
    // Move
    void Move(float dX, float dY, float dZ);
    // Move(-(pos * this))
    void MoveInversePosition(float x, float y, float z);
    // Set new position
    void SetPosition(float x, float y, float z);
    void SetPosition(CVECTOR &pos);
    // SetPosition(-(pos * this))
    void SetInversePosition(float x, float y, float z);

    //-----------------------------------------------------------
    // Utils
    //-----------------------------------------------------------

    // Access to axis vectors
    CVECTOR &Vx() const;  //Вектор по X (ссылка)
    CVECTOR &Vy() const;  //Вектор по Y (ссылка)
    CVECTOR &Vz() const;  //Вектор по Z (ссылка)
    CVECTOR &Pos() const; //Позиция (ссылка)

    // Access to matrix element
    float &operator()(long i, long j);

    // Create only rotate matrix
    void Get3X3(CMatrix &mtr);
    void Get3X3(CMatrix *mtr);

    // Projection
    CMatrix &BuildProjectionMatrix(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);
    void Projection(CVECTOR *srcArray, MTX_PRJ_VECTOR *dstArray, long num, float vphWidth05, float vphHeight05,
                    long srcStrcSize, long dstStrcSize);

    // View
    bool BuildViewMatrix(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR upVector);

    // Mirror
    void BuildMirrorMatrix(float Nx, float Ny, float Nz, float D);

    // D3D extends (return (D3DXMATRIX *)pointer)
    operator D3DXMATRIX *() const;

    //Получить углы из нескалированной матрицы поворота
    __forceinline void GetAngles(float &ax, float &ay, float &az)
    {
        if (vz.y < 1.0f)
        {
            if (vz.y > -1.0f)
            {
                ax = (float)asin(-vz.y);
                ay = (float)atan2(vz.x, vz.z);
                az = (float)atan2(vx.y, vy.y);
                return;
            }
            else
            {
                ax = 3.141592654f * 0.5f;
                ay = 0.0f;
                az = (float)atan2(vx.z, vx.x);
            }
        }
        else
        {
            ax = -3.141592654f * 0.5f;
            ay = 0.0f;
            az = (float)-atan2(vx.z, vx.x);
        }
    }

    //Умножить нормаль на матрицу
    __forceinline CVECTOR MulNormal(const CVECTOR &v) const
    {
        CVECTOR tv;
        tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
        tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
        tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;
        return tv;
    }
};

__forceinline CVECTOR operator*(const CVECTOR &v, const CMatrix &mtx)
{
    CVECTOR tv;
    tv.x = mtx.m[0][0] * v.x + mtx.m[1][0] * v.y + mtx.m[2][0] * v.z + mtx.m[3][0];
    tv.y = mtx.m[0][1] * v.x + mtx.m[1][1] * v.y + mtx.m[2][1] * v.z + mtx.m[3][1];
    tv.z = mtx.m[0][2] * v.x + mtx.m[1][2] * v.y + mtx.m[2][2] * v.z + mtx.m[3][2];
    return tv;
}

//============================================================================================
// Constructors
//============================================================================================

__forceinline CMatrix::CMatrix()
{
    SetIdentity();
}

__forceinline CMatrix::CMatrix(float angX, float angY, float angZ)
{
    BuildMatrix(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
}

__forceinline CMatrix::CMatrix(float angX, float angY, float angZ, float x, float y, float z)
{
    BuildMatrix(angX, angY, angZ, x, y, z);
}

__forceinline CMatrix::CMatrix(const CVECTOR &ang, const CVECTOR &pos)
{
    BuildMatrix(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
}

__forceinline CMatrix::CMatrix(CVECTOR &ang)
{
    BuildMatrix(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
}

__forceinline CMatrix::CMatrix(const CMatrix &matrix)
{
    *this = matrix;
}

__forceinline CMatrix::CMatrix(CMatrix *matrix)
{
    *this = *matrix;
}

// this = m1*m2
__forceinline CMatrix::CMatrix(CMatrix &m1, CMatrix &m2)
{
    EqMultiply(m1, m2);
}

//============================================================================================
// Main
//============================================================================================

// Create identity matrix
__forceinline void CMatrix::SetIdentity()
{
    m[0][0] = 1.0f;
    m[1][0] = 0.0f;
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;

    m[0][1] = 0.0f;
    m[1][1] = 1.0f;
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;

    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = 1.0f;
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

// Build matrix
__forceinline void CMatrix::BuildMatrix(float angX, float angY, float angZ, float x, float y, float z)
{
    float sinAx = sinf(angX);
    float cosAx = cosf(angX);
    float sinAy = sinf(angY);
    float cosAy = cosf(angY);
    float sinAz = sinf(angZ);
    float cosAz = cosf(angZ);

    //Создаём матрицу с порядком вращений rz*rx*ry
    m[0][0] = cosAz * cosAy + sinAz * sinAx * sinAy;
    m[1][0] = -sinAz * cosAy + cosAz * sinAx * sinAy;
    m[2][0] = cosAx * sinAy;
    m[3][0] = x;

    m[0][1] = sinAz * cosAx;
    m[1][1] = cosAz * cosAx;
    m[2][1] = -sinAx;
    m[3][1] = y;

    m[0][2] = cosAz * -sinAy + sinAz * sinAx * cosAy;
    m[1][2] = -sinAz * -sinAy + cosAz * sinAx * cosAy;
    m[2][2] = cosAx * cosAy;
    m[3][2] = z;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

__forceinline void CMatrix::BuildMatrixXYZ(float angX, float angY, float angZ, float x, float y, float z)
{
    float sinAx = sinf(angX);
    float cosAx = cosf(angX);
    float sinAy = sinf(angY);
    float cosAy = cosf(angY);
    float sinAz = sinf(angZ);
    float cosAz = cosf(angZ);

    m[0][0] = cosAy * cosAz;
    m[1][0] = sinAx * sinAy * cosAz - cosAx * sinAz;
    m[2][0] = cosAx * sinAy * cosAz + sinAx * sinAz;
    m[3][0] = x;

    m[0][1] = cosAy * sinAz;
    m[1][1] = sinAx * sinAy * sinAz + cosAx * cosAz;
    m[2][1] = cosAx * sinAy * sinAz - sinAx * cosAz;
    m[3][1] = y;

    m[0][2] = -sinAy;
    m[1][2] = sinAx * cosAy;
    m[2][2] = cosAx * cosAy;
    m[3][2] = z;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

__forceinline void CMatrix::BuildMatrix(float angX, float angY, float angZ)
{
    BuildMatrix(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
}

__forceinline void CMatrix::BuildMatrix(CVECTOR &ang, CVECTOR &pos)
{
    BuildMatrix(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
}

__forceinline void CMatrix::BuildMatrix(CVECTOR &ang)
{
    BuildMatrix(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
}

// Build rotate matrix
__forceinline void CMatrix::BuildRotateX(float ang)
{
    SetIdentity();
    m[1][1] = cosf(ang);
    m[2][1] = -sinf(ang);
    m[1][2] = sinf(ang);
    m[2][2] = cosf(ang);
}

__forceinline void CMatrix::BuildRotateY(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[2][0] = sinf(ang);
    m[0][2] = -sinf(ang);
    m[2][2] = cosf(ang);
}

__forceinline void CMatrix::BuildRotateZ(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[1][0] = -sinf(ang);
    m[0][1] = sinf(ang);
    m[1][1] = cosf(ang);
}

// Build position matrix
__forceinline void CMatrix::BuildPosition(float x, float y, float z)
{
    SetIdentity();
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

// Equal
__forceinline void CMatrix::operator=(const CMatrix &matrix)
{
    memcpy(this->matrix, matrix.matrix, sizeof(this->matrix));
}

// Multiply
__forceinline void CMatrix::operator*=(CMatrix &matrix)
{
    CMatrix tmp;
    tmp.EqMultiply(*this, matrix);
    *this = tmp;
}

__forceinline void CMatrix::operator*=(float k)
{
    m[0][0] *= k;
    m[1][0] *= k;
    m[2][0] *= k;
    m[3][0] *= k;
    m[0][1] *= k;
    m[1][1] *= k;
    m[2][1] *= k;
    m[3][1] *= k;
    m[0][2] *= k;
    m[1][2] *= k;
    m[2][2] *= k;
    m[3][2] *= k;
    m[0][3] *= k;
    m[1][3] *= k;
    m[2][3] *= k;
    m[3][3] *= k;
}

__forceinline CMatrix CMatrix::operator*(CMatrix &matrix)
{
    CMatrix tmp;
    tmp.EqMultiply(*this, matrix);
    return tmp;
}

__forceinline CVECTOR CMatrix::operator*(CVECTOR &vector)
{
    CVECTOR tmp;
    tmp.x = matrix[0] * vector.x + matrix[4] * vector.y + matrix[8] * vector.z + matrix[12];
    tmp.y = matrix[1] * vector.x + matrix[5] * vector.y + matrix[9] * vector.z + matrix[13];
    tmp.z = matrix[2] * vector.x + matrix[6] * vector.y + matrix[10] * vector.z + matrix[14];
    return tmp;
}

__forceinline CVECTOR operator*(const CMatrix &mtx, const CVECTOR &vector)
{
    CVECTOR tmp;
    tmp.x = mtx.matrix[0] * vector.x + mtx.matrix[4] * vector.y + mtx.matrix[8] * vector.z + mtx.matrix[12];
    tmp.y = mtx.matrix[1] * vector.x + mtx.matrix[5] * vector.y + mtx.matrix[9] * vector.z + mtx.matrix[13];
    tmp.z = mtx.matrix[2] * vector.x + mtx.matrix[6] * vector.y + mtx.matrix[10] * vector.z + mtx.matrix[14];
    return tmp;
}

// this = m1*m2, (m1 != this, m2 != this)
__forceinline void CMatrix::EqMultiply(CMatrix &m1, CMatrix &m2)
{
    matrix[0] = m2.matrix[0] * m1.matrix[0] + m2.matrix[4] * m1.matrix[1] + m2.matrix[8] * m1.matrix[2] +
                m2.matrix[12] * m1.matrix[3];
    matrix[1] = m2.matrix[1] * m1.matrix[0] + m2.matrix[5] * m1.matrix[1] + m2.matrix[9] * m1.matrix[2] +
                m2.matrix[13] * m1.matrix[3];
    matrix[2] = m2.matrix[2] * m1.matrix[0] + m2.matrix[6] * m1.matrix[1] + m2.matrix[10] * m1.matrix[2] +
                m2.matrix[14] * m1.matrix[3];
    matrix[3] = m2.matrix[3] * m1.matrix[0] + m2.matrix[7] * m1.matrix[1] + m2.matrix[11] * m1.matrix[2] +
                m2.matrix[15] * m1.matrix[3];

    matrix[4] = m2.matrix[0] * m1.matrix[4] + m2.matrix[4] * m1.matrix[5] + m2.matrix[8] * m1.matrix[6] +
                m2.matrix[12] * m1.matrix[7];
    matrix[5] = m2.matrix[1] * m1.matrix[4] + m2.matrix[5] * m1.matrix[5] + m2.matrix[9] * m1.matrix[6] +
                m2.matrix[13] * m1.matrix[7];
    matrix[6] = m2.matrix[2] * m1.matrix[4] + m2.matrix[6] * m1.matrix[5] + m2.matrix[10] * m1.matrix[6] +
                m2.matrix[14] * m1.matrix[7];
    matrix[7] = m2.matrix[3] * m1.matrix[4] + m2.matrix[7] * m1.matrix[5] + m2.matrix[11] * m1.matrix[6] +
                m2.matrix[15] * m1.matrix[7];

    matrix[8] = m2.matrix[0] * m1.matrix[8] + m2.matrix[4] * m1.matrix[9] + m2.matrix[8] * m1.matrix[10] +
                m2.matrix[12] * m1.matrix[11];
    matrix[9] = m2.matrix[1] * m1.matrix[8] + m2.matrix[5] * m1.matrix[9] + m2.matrix[9] * m1.matrix[10] +
                m2.matrix[13] * m1.matrix[11];
    matrix[10] = m2.matrix[2] * m1.matrix[8] + m2.matrix[6] * m1.matrix[9] + m2.matrix[10] * m1.matrix[10] +
                 m2.matrix[14] * m1.matrix[11];
    matrix[11] = m2.matrix[3] * m1.matrix[8] + m2.matrix[7] * m1.matrix[9] + m2.matrix[11] * m1.matrix[10] +
                 m2.matrix[15] * m1.matrix[11];

    matrix[12] = m2.matrix[0] * m1.matrix[12] + m2.matrix[4] * m1.matrix[13] + m2.matrix[8] * m1.matrix[14] +
                 m2.matrix[12] * m1.matrix[15];
    matrix[13] = m2.matrix[1] * m1.matrix[12] + m2.matrix[5] * m1.matrix[13] + m2.matrix[9] * m1.matrix[14] +
                 m2.matrix[13] * m1.matrix[15];
    matrix[14] = m2.matrix[2] * m1.matrix[12] + m2.matrix[6] * m1.matrix[13] + m2.matrix[10] * m1.matrix[14] +
                 m2.matrix[14] * m1.matrix[15];
    matrix[15] = m2.matrix[3] * m1.matrix[12] + m2.matrix[7] * m1.matrix[13] + m2.matrix[11] * m1.matrix[14] +
                 m2.matrix[15] * m1.matrix[15];
}

// Transform vertex to local coordinate system
__forceinline void CMatrix::MulToInv(const CVECTOR &src, CVECTOR &res)
{
    res.x = (src.x - matrix[12]) * matrix[0] + (src.y - matrix[13]) * matrix[1] + (src.z - matrix[14]) * matrix[2];
    res.y = (src.x - matrix[12]) * matrix[4] + (src.y - matrix[13]) * matrix[5] + (src.z - matrix[14]) * matrix[6];
    res.z = (src.x - matrix[12]) * matrix[8] + (src.y - matrix[13]) * matrix[9] + (src.z - matrix[14]) * matrix[10];
}

// Transform normal to local coordinate system
__forceinline void CMatrix::MulToInvNorm(CVECTOR &src, CVECTOR &res)
{
    res.x = src.x * matrix[0] + src.y * matrix[1] + src.z * matrix[2];
    res.y = src.x * matrix[4] + src.y * matrix[5] + src.z * matrix[6];
    res.z = src.x * matrix[8] + src.y * matrix[9] + src.z * matrix[10];
}

// Transposition
__forceinline void CMatrix::Transposition()
{
    float x = Pos() | Vx();
    float y = Pos() | Vy();
    float z = Pos() | Vz();
    Pos().x = -x;
    Pos().y = -y;
    Pos().z = -z;
    Transposition3X3();
}

//Транспанирование матрицы
__forceinline CMatrix &CMatrix::TranspositionX()
{
    float ebx = matrix[1];
    float ecx = matrix[2];
    float esi = matrix[4];
    float edi = matrix[8];
    matrix[4] = ebx;
    matrix[8] = ecx;
    matrix[1] = esi;
    matrix[2] = edi;

    ebx = matrix[3];
    ecx = matrix[6];
    esi = matrix[12];
    edi = matrix[9];
    matrix[12] = ebx;
    matrix[9] = ecx;
    matrix[3] = esi;
    matrix[6] = edi;

    ebx = matrix[7];
    ecx = matrix[11];
    esi = matrix[13];
    edi = matrix[14];
    matrix[13] = ebx;
    matrix[14] = ecx;
    matrix[7] = esi;
    matrix[11] = edi;

    return *this;
}

//Расчёт обратной матрицы
__forceinline CMatrix &CMatrix::Inverse()
{
    pos = CVECTOR(-(pos | vx), -(pos | vy), -(pos | vz));
    Transposition3X3();
    return *this;
}

__forceinline void CMatrix::Transposition3X3()
{
    float m01 = m[0][1];
    float m02 = m[0][2];
    float m12 = m[1][2];

    m[0][1] = m[1][0];
    m[0][2] = m[2][0];
    m[1][2] = m[2][1];

    m[1][0] = m01;
    m[2][0] = m02;
    m[2][1] = m12;
}

__forceinline CMatrix &__fastcall CMatrix::Transposition4x4()
{
    float m01 = m[0][1];
    float m02 = m[0][2];
    float m03 = m[0][3];
    float m12 = m[1][2];
    float m13 = m[1][3];
    float m23 = m[2][3];

    m[0][1] = m[1][0];
    m[0][2] = m[2][0];
    m[0][3] = m[3][0];
    m[1][2] = m[2][1];
    m[1][3] = m[3][1];
    m[2][3] = m[3][2];

    m[1][0] = m01;
    m[2][0] = m02;
    m[3][0] = m03;
    m[2][1] = m12;
    m[3][1] = m13;
    m[3][2] = m23;

    return *this;
}

// Rotate
__forceinline void CMatrix::RotateX(float ang)
{
    CMatrix mtr;
    mtr.BuildRotateX(ang);
    *this *= mtr;
}

__forceinline void CMatrix::RotateY(float ang)
{
    CMatrix mtr;
    mtr.BuildRotateY(ang);
    *this *= mtr;
}

__forceinline void CMatrix::RotateZ(float ang)
{
    CMatrix mtr;
    mtr.BuildRotateZ(ang);
    *this *= mtr;
}

// Move
__forceinline void CMatrix::Move(float dX, float dY, float dZ)
{
    matrix[12] += dX;
    matrix[13] += dY;
    matrix[14] += dZ;
}

// Move(-(pos * this))
__forceinline void CMatrix::MoveInversePosition(float x, float y, float z)
{
    m[3][0] -= m[0][0] * x + m[1][0] * y + m[2][0] * z;
    m[3][1] -= m[0][1] * x + m[1][1] * y + m[2][1] * z;
    m[3][2] -= m[0][2] * x + m[1][2] * y + m[2][2] * z;
}

// Set new position
__forceinline void CMatrix::SetPosition(float x, float y, float z)
{
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

__forceinline void CMatrix::SetPosition(CVECTOR &pos)
{
    matrix[12] = pos.x;
    matrix[13] = pos.y;
    matrix[14] = pos.z;
}

// SetPosition(-(pos * this))
__forceinline void CMatrix::SetInversePosition(float x, float y, float z)
{
    m[3][0] = -(m[0][0] * x + m[1][0] * y + m[2][0] * z);
    m[3][1] = -(m[0][1] * x + m[1][1] * y + m[2][1] * z);
    m[3][2] = -(m[0][2] * x + m[1][2] * y + m[2][2] * z);
}

//============================================================================================
// Utils
//============================================================================================

// Access to axis vectors
__forceinline CVECTOR &CMatrix::Vx() const
{
    return *(CVECTOR *)(matrix);
}

__forceinline CVECTOR &CMatrix::Vy() const
{
    return *(CVECTOR *)(matrix + 4);
}

__forceinline CVECTOR &CMatrix::Vz() const
{
    return *(CVECTOR *)(matrix + 8);
}

__forceinline CVECTOR &CMatrix::Pos() const
{
    return *(CVECTOR *)(matrix + 12);
}

// Access to matrix element
__forceinline float &CMatrix::operator()(long i, long j)
{
    return m[i][j];
}

// Create only rotate matrix
__forceinline void CMatrix::Get3X3(CMatrix &mtr)
{
    float ebx = matrix[0];
    float ecx = matrix[1];
    float edx = matrix[2];
    mtr.matrix[0] = ebx;
    mtr.matrix[1] = ecx;
    mtr.matrix[2] = edx;

    ebx = matrix[4];
    ecx = matrix[5];
    edx = matrix[6];
    mtr.matrix[4] = ebx;
    mtr.matrix[5] = ecx;
    mtr.matrix[6] = edx;

    ebx = matrix[8];
    ecx = matrix[9];
    edx = matrix[10];
    mtr.matrix[8] = ebx;
    mtr.matrix[9] = ecx;
    mtr.matrix[10] = edx;
}

// Projection
__forceinline CMatrix &CMatrix::BuildProjectionMatrix(float viewAngle, float vpWidth, float vpHeight, float zNear,
                                                      float zFar)
{
    memset(matrix, 0, sizeof(matrix));

    float cs = cosf(viewAngle * 0.5f);
    float sn = sinf(viewAngle * 0.5f);
    double Q = (double)zFar / (double)(zFar - zNear);

    matrix[0] = 1.0f / tanf(viewAngle * 0.5f);
    matrix[5] = 1.0f / tanf((vpHeight / vpWidth) * viewAngle * 0.5f);
    matrix[10] = (float)Q;
    matrix[11] = 1.0f;
    matrix[14] = -(float)Q * zNear;

    return (*this);
}

__forceinline void CMatrix::Projection(CVECTOR *srcArray, MTX_PRJ_VECTOR *dstArray, long num, float vphWidth05,
                                       float vphHeight05, long srcStrcSize, long dstStrcSize)
{
    float k;
    for (; num > 0; num--)
    {
        dstArray->x = matrix[0] * srcArray->x + matrix[4] * srcArray->y + matrix[8] * srcArray->z + matrix[12];
        dstArray->y = matrix[1] * srcArray->x + matrix[5] * srcArray->y + matrix[9] * srcArray->z + matrix[13];
        dstArray->z = matrix[2] * srcArray->x + matrix[6] * srcArray->y + matrix[10] * srcArray->z + matrix[14];
        dstArray->rhw = matrix[3] * srcArray->x + matrix[7] * srcArray->y + matrix[11] * srcArray->z + matrix[15];

        k = 1.0f / dstArray->rhw;

        dstArray->x = (1.0f + dstArray->x * k) * vphWidth05;
        dstArray->y = (1.0f - dstArray->y * k) * vphHeight05;
        dstArray->z *= k;

        dstArray->rhw = k;

        srcArray = (CVECTOR *)((long)srcArray + srcStrcSize);
        dstArray = (MTX_PRJ_VECTOR *)((long)dstArray + dstStrcSize);
    }
}

// View
__forceinline bool CMatrix::BuildViewMatrix(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR upVector)
{
    SetIdentity();
    //Нормализуем вектор смотрения
    lookTo -= lookFrom;
    float l = ~lookTo;
    if (l == 0.0f)
    {
        //Ставим позицию для неповёрнутой матрици
        Pos() = -lookFrom;
        return false;
    }
    lookTo *= 1.0f / sqrtf(l);
    //Направляем вектор вверх в нужном направлении
    upVector -= lookTo * (lookTo | upVector);
    //Нормализуем вертор направленный вверх
    l = ~upVector;
    if (l == 0.0f)
        upVector.y = 1.0f;
    else
        upVector *= 1.0f / sqrtf(l);
    //Ищем третий вектор базиса
    CVECTOR v = upVector ^ lookTo;
    l = ~v;
    if (l != 0.0f)
    {
        v *= (1.0f / sqrtf(l));
        //Ставим матрицу поворота
        Vx() = upVector;
        Vy() = lookTo;
        Vz() = v;

        m[0][0] = v.x;
        m[1][0] = v.y;
        m[2][0] = v.z;
        m[0][1] = upVector.x;
        m[1][1] = upVector.y;
        m[2][1] = upVector.z;
        m[0][2] = lookTo.x;
        m[1][2] = lookTo.y;
        m[2][2] = lookTo.z;
    }
    //Ставим позицию
    SetInversePosition(lookFrom.x, lookFrom.y, lookFrom.z);
    return true;
}

// Mirror
//Формирование матрицы отражение по плану
__forceinline void CMatrix::BuildMirrorMatrix(float Nx, float Ny, float Nz, float D)
{
    m[0][0] = -Nx * 2.0f * Nx + 1.0f;
    m[1][0] = -Nx * 2.0f * Ny;
    m[2][0] = -Nx * 2.0f * Nz;
    m[3][0] = -Nx * 2.0f * -D;

    m[0][1] = -Ny * 2.0f * Nx;
    m[1][1] = -Ny * 2.0f * Ny + 1.0f;
    m[2][1] = -Ny * 2.0f * Nz;
    m[3][1] = -Ny * 2.0f * -D;

    m[0][2] = -Nz * 2.0f * Nx;
    m[1][2] = -Nz * 2.0f * Ny;
    m[2][2] = -Nz * 2.0f * Nz + 1.0f;
    m[3][2] = -Nz * 2.0f * -D;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

// D3D extends (return (D3DXMATRIX *)pointer)
__forceinline CMatrix::operator D3DXMATRIX *() const
{
    return ((D3DXMATRIX *)matrix);
};

//============================================================================================

__forceinline CMatrix &CMatrix::BuildScale(float scale)
{
    SetIdentity();
    m[0][0] = scale;
    m[1][1] = scale;
    m[2][2] = scale;
    return *this;
}

//Посчитать матрицу масштабирования
__forceinline CMatrix &CMatrix::BuildScale(float scaleX, float scaleY, float scaleZ)
{
    SetIdentity();
    m[0][0] = scaleX;
    m[1][1] = scaleY;
    m[2][2] = scaleZ;
    return *this;
}

//Посчитать матрицу масштабирования
__forceinline CMatrix &CMatrix::BuildScale(const CVECTOR &scale)
{
    BuildScale(scale.x, scale.y, scale.z);
    return *this;
}

#endif
