//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Matrix
//============================================================================================

#ifndef _Matrix_h_
#define _Matrix_h_

#include "Plane.h"
#include "Vector4.h"

//============================================================================================

struct D3DXMATRIX;

#pragma pack(push, 1)

//============================================================================================
/*!
<PRE>

Линейное представление        В виде двухмерного массива
                                vx     vy     vz    pos
    0  4  8 12                [0][0] [1][0] [2][0] [3][0]    x
    1  5  9 13                [0][1] [1][1] [2][1] [3][1]    y
    2  6 10 14                [0][2] [1][2] [2][2] [3][2]    z
    3  7 11 15                [0][3] [1][3] [2][3] [3][3]    w
*/
///Класс матрицы для преобразований в 3D.
//============================================================================================

class Matrix
{
  public:
    union {
        ///Линейный массив
        alignas(16) float matrix[16]; // espkk # remove inline asm # 30/Dec/2017
        ///Двумерный массив
        float m[4][4];

        struct
        {
            ///Направление по X
            Vector vx;
            ///Весовое значение по X
            float wx;
            ///Направление по Y
            Vector vy;
            ///Весовое значение по Y
            float wy;
            ///Направление по Z
            Vector vz;
            ///Весовое значение по Z
            float wz;
            ///Позиция
            Vector pos;
            //Добавляемое весовое значение
            float w;
        };
    };

    //-----------------------------------------------------------
    //Конструкторы
    //-----------------------------------------------------------
  public:
    ///Сконструировать единичную матрицу
    Matrix();
    ///Сконструировать матрицу без заполнения
    Matrix(bool empty);
    ///Сконструировать матрицу по углам с позицией
    Matrix(float angX, float angY, float angZ, float x, float y, float z);
    ///Сконструировать матрицу по углам
    Matrix(float angX, float angY, float angZ);
    ///Сконструировать матрицу по углам с позицией
    Matrix(const Vector &ang, const Vector &pos);
    ///Сконструировать матрицу по углам
    Matrix(const Vector &ang);
    ///Сконструировать копию матрицы
    Matrix(const Matrix &matrix);
    ///Сконструировать результат перемножения матриц this = m1*m2
    Matrix(const Matrix &m1, const Matrix &m2);

    //-----------------------------------------------------------
    //Операторы
    //-----------------------------------------------------------
  public:
    //Присвоить матрице другую матрицу
    Matrix &operator=(const Matrix &mtx);
    //Присвоить позиции матрицы число
    Matrix &operator=(float f);
    //Присвоить позиции матрицы число
    Matrix &operator=(double d);
    //Присвоить позиции матрицы вектор
    Matrix &operator=(const Vector &v);

    //Перемножить матрицы
    Matrix &operator*=(const Matrix &mtx);

    //-----------------------------------------------------------
    //Заполнение матрицы
    //-----------------------------------------------------------
  public:
    ///Установить единичную матрицу
    Matrix &SetIdentity();

    ///Установить матрицу
    Matrix &Set(const Matrix &matrix);

    ///Посчитать матрицу M = rotZ*rotX*rotY*Pos
    Matrix &Build(float angX, float angY, float angZ, float x, float y, float z);
    ///Посчитать матрицу M = rotZ*rotX*rotY
    Matrix &Build(float angX, float angY, float angZ);
    ///Посчитать матрицу M = rotZ*rotX*rotY*Pos
    Matrix &Build(const Vector &ang, const Vector &pos);
    ///Посчитать матрицу M = rotZ*rotX*rotY
    Matrix &Build(const Vector &ang);

    ///Посчитать матрицу M = rotX*rotY*rotZ*Pos
    Matrix &BuildXYZ(float angX, float angY, float angZ, float x, float y, float z);

    ///Посчитать матрицу поворота вокруг X
    Matrix &BuildRotateX(float ang);
    ///Посчитать матрицу поворота вокруг Y
    Matrix &BuildRotateY(float ang);
    ///Посчитать матрицу поворота вокруг Z
    Matrix &BuildRotateZ(float ang);
    ///Посчитать матрицу позиции
    Matrix &BuildPosition(float x, float y, float z);
    ///Посчитать матрицу позиции
    Matrix &BuildPosition(const Vector &pos);

    ///Посчитать матрицу масштабирования
    Matrix &BuildScale(float scale);
    ///Посчитать матрицу масштабирования
    Matrix &BuildScale(float scaleX, float scaleY, float scaleZ);
    ///Посчитать матрицу масштабирования
    Matrix &BuildScale(const Vector &scale);

    ///Посчитать матрицу проекции
    Matrix &BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);
    ///Посчитать матрицу камеры
    bool BuildView(Vector lookFrom, Vector lookTo, Vector upVector);
    ///Посчитать матрицу ориентации объекта имея направление по z и направление вверх
    bool BuildOrient(Vector zAxisDirection, Vector upVector);
    ///Посчитать матрицу объекта имея позицию точку куда направлен объект и направление вверх
    bool BuildOriented(Vector position, Vector lookTo, Vector upVector);
    ///Посчитать матрицу для отзеркаливания геометрии
    Matrix &BuildMirror(float Nx, float Ny, float Nz, float D);
    ///Посчитать ортоганальную матрицу проекции
    Matrix &BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar);
    //Посчитать матрицу проекции для тени
    Matrix &BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);

    //-----------------------------------------------------------
    //Преобразование матрицы
    //-----------------------------------------------------------
  public:
    ///Повернуть вокруг X
    Matrix &RotateX(float ang);
    ///Повернуть вокруг Y
    Matrix &RotateY(float ang);
    ///Повернуть вокруг Z
    Matrix &RotateZ(float ang);
    ///Повернуть вокруг ZXY
    Matrix &Rotate(float angX, float angY, float angZ);
    ///Повернуть вокруг ZXY
    Matrix &Rotate(const Vector &ang);

    ///Переместить
    Matrix &Move(float dX, float dY, float dZ);
    ///Переместить
    Matrix &Move(const Vector &pos);

    ///Отмасштабировать
    Matrix &Scale(float scale);
    ///Отмасштабировать матрицу поворота
    Matrix &Scale3x3(float scale);
    ///Отмасштабировать
    Matrix &Scale(float scaleX, float scaleY, float scaleZ);
    ///Отмасштабировать матрицу поворота
    Matrix &Scale3x3(float scaleX, float scaleY, float scaleZ);
    ///Отмасштабировать
    Matrix &Scale(const Vector &scale);
    ///Отмасштабировать матрицу поворота
    Matrix &Scale3x3(const Vector &scale);

    ///Расчёт обратной матрицы
    Matrix &Inverse();
    ///Расчёт обратной матрицы из другой
    Matrix &Inverse(const Matrix &mtx);
    ///Расчёт масштабированной обратной матрицы
    Matrix &InverseWhithScale();
    ///Транспанирование матрицы
    Matrix &Transposition();
    ///Транспанирование элементов поворота
    Matrix &Transposition3X3();

    //-----------------------------------------------------------
    //Утилитные
    //-----------------------------------------------------------
  public:
    ///Считать только вращение
    Matrix &SetRotate(const Matrix &mtr);
    ///Перемножить матрицы и результат поместить в текущую
    Matrix &EqMultiply(const Matrix &m1, const Matrix &m2);
    ///Перемножить матрицы и результат поместить в текущую m1 != this && m2 != this
    Matrix &EqMultiplyFast(const Matrix &m1, const Matrix &m2);

    ///Умножить вершину на матрицу
    Vector MulVertex(const Vector &v) const;
    ///Умножить нормаль на матрицу
    Vector MulNormal(const Vector &v) const;
    ///Умножить вершину на инверстую матрицу
    Vector MulVertexByInverse(const Vector &v) const;
    ///Умножить нормаль на инверстую матрицу
    Vector MulNormalByInverse(const Vector &v) const;

    ///Получить позицию камеры из матрицы камеры
    Vector GetCamPos() const;
    ///Единичная матрица или нет
    bool IsIdentity() const;
    ///Скалирования матрица или нет
    bool IsScale() const;

    ///Споецировать вершину (для матрицы проекции)
    Vector4 Projection(Vector vertex, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f) const;
    ///Споецировать массив вершин (для матрицы проекции)
    void Projection(Vector4 *dstArray, Vector *srcArray, long num, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f,
                    long srcSize = sizeof(Vector), long dstSize = sizeof(Vector4)) const;

    //Получить углы из нескалированной матрицы поворота
    void GetAngles(float &ax, float &ay, float &az) const;
    //Получить углы из нескалированной матрицы поворота
    void GetAngles(Vector &ang) const;

    ///Доступиться до элементов матрицы через скобки
    float &operator()(long i, long j);

    ///Получить указатель на матрицу D3D
    operator D3DXMATRIX *() const;

    ///Получить вектор для расчёта X компоненты
    Vector4 GetVectorX() const;
    ///Получить вектор для расчёта Y компоненты
    Vector4 GetVectorY() const;
    ///Получить вектор для расчёта Z компоненты
    Vector4 GetVectorZ() const;
    ///Получить вектор для расчёта W компоненты
    Vector4 GetVectorW() const;
};

//===========================================================
//Конструкторы
//===========================================================

//Сконструировать единичную матрицу
inline Matrix::Matrix()
{
    SetIdentity();
}

///Сконструировать матрицу без заполнения
inline Matrix::Matrix(bool empty)
{
}

//Сконструировать матрицу по углам с позицией
inline Matrix::Matrix(float angX, float angY, float angZ, float x, float y, float z)
{
    Build(angX, angY, angZ, x, y, z);
}

//Сконструировать матрицу по углам
inline Matrix::Matrix(float angX, float angY, float angZ)
{
    Build(angX, angY, angZ);
}

//Сконструировать матрицу по углам с позицией
inline Matrix::Matrix(const Vector &ang, const Vector &pos)
{
    Build(ang, pos);
}

//Сконструировать матрицу по углам
inline Matrix::Matrix(const Vector &ang)
{
    Build(ang);
}

//Сконструировать копию матрицы
inline Matrix::Matrix(const Matrix &matrix)
{
    Set(matrix);
}

//Сконструировать результат перемножения матриц this = m1*m2
inline Matrix::Matrix(const Matrix &m1, const Matrix &m2)
{
    EqMultiply(m1, m2);
}

//===========================================================
//Операторы
//===========================================================

//Присвоить матрице другую матрицу
inline Matrix &Matrix::operator=(const Matrix &mtx)
{
    Set(mtx);
    return *this;
}

//Присвоить позиции матрицы число
inline Matrix &Matrix::operator=(float f)
{
    pos = f;
    return *this;
}

//Присвоить позиции матрицы число
inline Matrix &Matrix::operator=(double d)
{
    pos = d;
    return *this;
}

//Присвоить позиции матрицы вектор
inline Matrix &Matrix::operator=(const Vector &v)
{
    pos = v;
    return *this;
}

//Перемножить матрицы
inline Matrix &Matrix::operator*=(const Matrix &mtx)
{
    EqMultiply(*this, mtx);
    return *this;
}

/*!\relates Matrix
Перемножить матрицы
*/
inline Matrix operator*(const Matrix &m1, const Matrix &m2)
{
    Matrix m;
    m.EqMultiplyFast(m1, m2);
    return m;
}

/*!\relates Matrix
Умножить вектор на матрицу
*/
inline Vector operator*(const Matrix &mtx, const Vector &v)
{
    return mtx.MulVertex(v);
}

/*!\relates Matrix
Умножить вектор на матрицу
*/
inline Vector operator*(const Vector &v, const Matrix &mtx)
{
    return mtx.MulVertex(v);
}

//===========================================================
//Заполнение матрицы
//===========================================================

//Установить единичную матрицу
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
      mov		eax, this
      mov		ecx, 0x3f800000
      xor		ebx, ebx
      mov		[eax + 0*4], ecx
      mov		[eax + 1*4], ebx
      mov		[eax + 2*4], ebx
      mov		[eax + 3*4], ebx
      mov		[eax + 4*4], ebx
      mov		[eax + 5*4], ecx
      mov		[eax + 6*4], ebx
      mov		[eax + 7*4], ebx
      mov		[eax + 8*4], ebx
      mov		[eax + 9*4], ebx
      mov		[eax + 10*4], ecx
      mov		[eax + 11*4], ebx
      mov		[eax + 12*4], ebx
      mov		[eax + 13*4], ebx
      mov		[eax + 14*4], ebx
      mov		[eax + 15*4], ecx
    }*/
    return *this;
}

//Установить матрицу
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

    /*	_asm
      {
        mov		esi, matrix
        mov		edi, this
        mov		eax, [esi + 0*4]
        mov		ebx, [esi + 1*4]
        mov		ecx, [esi + 2*4]
        mov		edx, [esi + 3*4]
        mov		[edi + 0*4], eax
        mov		[edi + 1*4], ebx
        mov		[edi + 2*4], ecx
        mov		[edi + 3*4], edx
        mov		eax, [esi + 4*4]
        mov		ebx, [esi + 5*4]
        mov		ecx, [esi + 6*4]
        mov		edx, [esi + 7*4]
        mov		[edi + 4*4], eax
        mov		[edi + 5*4], ebx
        mov		[edi + 6*4], ecx
        mov		[edi + 7*4], edx
        mov		eax, [esi + 8*4]
        mov		ebx, [esi + 9*4]
        mov		ecx, [esi + 10*4]
        mov		edx, [esi + 11*4]
        mov		[edi + 8*4], eax
        mov		[edi + 9*4], ebx
        mov		[edi + 10*4], ecx
        mov		[edi + 11*4], edx
        mov		eax, [esi + 12*4]
        mov		ebx, [esi + 13*4]
        mov		ecx, [esi + 14*4]
        mov		edx, [esi + 15*4]
        mov		[edi + 12*4], eax
        mov		[edi + 13*4], ebx
        mov		[edi + 14*4], ecx
        mov		[edi + 15*4], edx
      }*/
    return *this;
}

//Посчитать матрицу M = rotZ*rotX*rotY*Pos
inline Matrix &Matrix::Build(float angX, float angY, float angZ, float x, float y, float z)
{
    //Синусы и косинусы углов поворота
    const auto sinAx = sinf(angX);
    const auto cosAx = cosf(angX);
    const auto sinAy = sinf(angY);
    const auto cosAy = cosf(angY);
    const auto sinAz = sinf(angZ);
    const auto cosAz = cosf(angZ);
    //Создаём матрицу с порядком вращений rz*rx*ry
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

//Посчитать матрицу M = rotZ*rotX*rotY
inline Matrix &Matrix::Build(float angX, float angY, float angZ)
{
    Build(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
    return *this;
}

//Посчитать матрицу M = rotZ*rotX*rotY*Pos
inline Matrix &Matrix::Build(const Vector &ang, const Vector &pos)
{
    Build(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
    return *this;
}

//Посчитать матрицу M = rotZ*rotX*rotY
inline Matrix &Matrix::Build(const Vector &ang)
{
    Build(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
    return *this;
}

//Посчитать матрицу M = rotX*rotY*rotZ*Pos
inline Matrix &Matrix::BuildXYZ(float angX, float angY, float angZ, float x, float y, float z)
{
    //Синусы и косинусы углов поворота
    const auto sinAx = sinf(angX);
    const auto cosAx = cosf(angX);
    const auto sinAy = sinf(angY);
    const auto cosAy = cosf(angY);
    const auto sinAz = sinf(angZ);
    const auto cosAz = cosf(angZ);
    //Создаём матрицу с порядком вращений rx*ry*rz
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

//Посчитать матрицу поворота вокруг X
inline Matrix &Matrix::BuildRotateX(float ang)
{
    SetIdentity();
    m[1][1] = cosf(ang);
    m[1][2] = sinf(ang);
    m[2][1] = -sinf(ang);
    m[2][2] = cosf(ang);
    return *this;
}

//Посчитать матрицу поворота вокруг Y
inline Matrix &Matrix::BuildRotateY(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[0][2] = -sinf(ang);
    m[2][0] = sinf(ang);
    m[2][2] = cosf(ang);
    return *this;
}

//Посчитать матрицу поворота вокруг Z
inline Matrix &Matrix::BuildRotateZ(float ang)
{
    SetIdentity();
    m[0][0] = cosf(ang);
    m[0][1] = sinf(ang);
    m[1][0] = -sinf(ang);
    m[1][1] = cosf(ang);
    return *this;
}

//Посчитать матрицу позиции
inline Matrix &Matrix::BuildPosition(float x, float y, float z)
{
    SetIdentity();
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
    return *this;
}

//Посчитать матрицу позиции
inline Matrix &Matrix::BuildPosition(const Vector &pos)
{
    BuildPosition(pos.x, pos.y, pos.z);
    return *this;
}

//Посчитать матрицу масштабирования
inline Matrix &Matrix::BuildScale(float scale)
{
    SetIdentity();
    m[0][0] = scale;
    m[1][1] = scale;
    m[2][2] = scale;
    return *this;
}

//Посчитать матрицу масштабирования
inline Matrix &Matrix::BuildScale(float scaleX, float scaleY, float scaleZ)
{
    SetIdentity();
    m[0][0] = scaleX;
    m[1][1] = scaleY;
    m[2][2] = scaleZ;
    return *this;
}

//Посчитать матрицу масштабирования
inline Matrix &Matrix::BuildScale(const Vector &scale)
{
    BuildScale(scale.x, scale.y, scale.z);
    return *this;
}

//Посчитать матрицу проекции
inline Matrix &Matrix::BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
    //Обнулим массив
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

    /*	_asm
      {
        mov		eax, this
        xor		ebx, ebx
        xor		edx, edx
        mov		[eax + 0*4], ebx
        mov		[eax + 1*4], edx
        mov		[eax + 2*4], ebx
        mov		[eax + 3*4], edx
        mov		[eax + 4*4], ebx
        mov		[eax + 5*4], edx
        mov		[eax + 6*4], ebx
        mov		[eax + 7*4], edx
        mov		[eax + 8*4], ebx
        mov		[eax + 9*4], edx
        mov		[eax + 10*4], ebx
        mov		[eax + 11*4], edx
        mov		[eax + 12*4], ebx
        mov		[eax + 13*4], edx
        mov		[eax + 14*4], ebx
        mov		[eax + 15*4], edx
      }*/
    //Заполняем матрицу
    const auto Q = static_cast<double>(zFar) / static_cast<double>(zFar - zNear);
    m[0][0] = static_cast<float>(1.0 / tan(viewAngle * 0.5));
    m[1][1] = static_cast<float>(1.0 / tan((vpHeight / vpWidth) * viewAngle * 0.5));
    m[2][2] = static_cast<float>(Q);
    m[2][3] = 1.0f;
    m[3][2] = static_cast<float>(-Q * zNear);
    return *this;
}

///Посчитать ортоганальную матрицу проекции
inline Matrix &Matrix::BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar)
{
    //Обнулим массив
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
    /*	_asm
      {
        mov		eax, this
          xor		ebx, ebx
          xor		edx, edx
          mov		[eax + 0*4], ebx
          mov		[eax + 1*4], edx
          mov		[eax + 2*4], ebx
          mov		[eax + 3*4], edx
          mov		[eax + 4*4], ebx
          mov		[eax + 5*4], edx
          mov		[eax + 6*4], ebx
          mov		[eax + 7*4], edx
          mov		[eax + 8*4], ebx
          mov		[eax + 9*4], edx
          mov		[eax + 10*4], ebx
          mov		[eax + 11*4], edx
          mov		[eax + 12*4], ebx
          mov		[eax + 13*4], edx
          mov		[eax + 14*4], ebx
          mov		[eax + 15*4], edx
      }*/
    //Заполняем матрицу
    const auto Q = 1.0 / static_cast<double>(zFar - zNear);
    m[0][0] = 2.0f / vpWidth;
    m[1][1] = 2.0f / vpHeight;
    m[2][2] = static_cast<float>(Q);
    m[3][2] = static_cast<float>(-Q * zNear);
    m[3][3] = 1.0f;

    return *this;
}

//Посчитать матрицу проекции для тени
inline Matrix &Matrix::BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
    //Обнулим массив
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
    /*	_asm
      {
        mov		eax, this
          xor		ebx, ebx
          xor		edx, edx
          mov		[eax + 0*4], ebx
          mov		[eax + 1*4], edx
          mov		[eax + 2*4], ebx
          mov		[eax + 3*4], edx
          mov		[eax + 4*4], ebx
          mov		[eax + 5*4], edx
          mov		[eax + 6*4], ebx
          mov		[eax + 7*4], edx
          mov		[eax + 8*4], ebx
          mov		[eax + 9*4], edx
          mov		[eax + 10*4], ebx
          mov		[eax + 11*4], edx
          mov		[eax + 12*4], ebx
          mov		[eax + 13*4], edx
          mov		[eax + 14*4], ebx
          mov		[eax + 15*4], edx
      }*/
    //Заполняем матрицу
    const auto Q = 1.0 / static_cast<double>(zFar - zNear);
    m[0][0] = static_cast<float>(1.0 / tan(viewAngle * 0.5));
    m[1][1] = static_cast<float>(1.0 / tan((vpHeight / vpWidth) * viewAngle * 0.5));
    m[2][2] = static_cast<float>(Q);
    m[2][3] = 1.0f;
    m[3][2] = static_cast<float>(-Q * zNear);
    return *this;
}

//Посчитать матрицу камеры
inline bool Matrix::BuildView(Vector lookFrom, Vector lookTo, Vector upVector)
{
    SetIdentity();
    //Нормализуем вектор смотрения
    lookTo -= lookFrom;
    if (lookTo.Normalize() == 0.0f)
    {
        //Ставим позицию для неповёрнутой матрици
        pos = -lookFrom;
        return false;
    }
    //Направляем вектор вверх в нужном направлении
    upVector -= lookTo * (lookTo | upVector);
    //Нормализуем вертор направленный вверх
    if (upVector.Normalize() == 0.0f)
        upVector.y = 1.0f;
    //Ищем третий вектор базиса
    auto v = upVector ^ lookTo;
    if (v.Normalize() != 0.0f)
    {
        //Ставим матрицу поворота
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
    else
    {
        //Ставим позицию для неповёрнутой матрици
        pos = -lookFrom;
        return false;
    }
    //Ставим позицию
    // pos = -MulNormalByInverse(lookFrom);
    pos = -MulNormal(lookFrom);
    return true;
}

//Посчитать матрицу ориентации объекта имея направление по z и направление вверх
inline bool Matrix::BuildOrient(Vector zAxisDirection, Vector upVector)
{
    //Нормализуем вектор направления z
    if (zAxisDirection.Normalize() < 1e-37f || upVector.Normalize() < 1e-37f)
    {
        vx = Vector(1.0f, 0.0f, 0.0f);
        vy = Vector(0.0f, 1.0f, 0.0f);
        vz = Vector(0.0f, 0.0f, 1.0f);
        return false;
    }
    //Считаем
    vx = zAxisDirection ^ upVector;
    if (vx.Normalize() == 0.0f)
    {
        vx = Vector(1.0f, 0.0f, 0.0f);
        vy = Vector(0.0f, 1.0f, 0.0f);
        vz = Vector(0.0f, 0.0f, 1.0f);
        return false;
    }
    vy = zAxisDirection ^ vx;
    vz = zAxisDirection;
    return true;
}

//Посчитать матрицу объекта имея позицию точку куда направлен объект и направление вверх
inline bool Matrix::BuildOriented(Vector position, Vector lookTo, Vector upVector)
{
    //Направление
    lookTo -= position;
    //Нормализуем вектор направления z
    if (lookTo.Normalize() == 0.0f || upVector.Normalize() == 0.0f)
    {
        vx = Vector(1.0f, 0.0f, 0.0f);
        wx = 0.0f;
        vy = Vector(0.0f, 1.0f, 0.0f);
        wy = 0.0f;
        vz = Vector(0.0f, 0.0f, 1.0f);
        wz = 0.0f;
        pos = position;
        w = 1.0f;
        return false;
    }
    //Считаем
    vx = lookTo ^ upVector;
    wx = 0.0f;
    if (vx.Normalize() == 0.0f)
    {
        vx = Vector(1.0f, 0.0f, 0.0f);
        wx = 0.0f;
        vy = Vector(0.0f, 1.0f, 0.0f);
        wy = 0.0f;
        vz = Vector(0.0f, 0.0f, 1.0f);
        wz = 0.0f;
        pos = position;
        w = 1.0f;
        return false;
    }
    vy = lookTo ^ vx;
    wy = 0.0f;
    vz = lookTo;
    wz = 0.0f;
    pos = position;
    w = 1.0f;
    return true;
}

//Посчитать матрицу для отзеркаливания геометрии
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

//-----------------------------------------------------------
//Преобразование матрицы
//-----------------------------------------------------------

//Повернуть вокруг X
inline Matrix &Matrix::RotateX(float ang)
{
    Matrix m;
    m.BuildRotateX(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

//Повернуть вокруг Y
inline Matrix &Matrix::RotateY(float ang)
{
    Matrix m;
    m.BuildRotateY(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

//Повернуть вокруг Z
inline Matrix &Matrix::RotateZ(float ang)
{
    Matrix m;
    m.BuildRotateZ(ang);
    EqMultiply(Matrix(*this), m);
    return *this;
}

//Повернуть вокруг ZXY
inline Matrix &Matrix::Rotate(float angX, float angY, float angZ)
{
    Matrix m;
    m.Build(angX, angY, angZ);
    EqMultiply(Matrix(*this), m);
    return *this;
}

//Повернуть вокруг ZXY
inline Matrix &Matrix::Rotate(const Vector &ang)
{
    Matrix m;
    m.Build(ang.x, ang.x, ang.z);
    EqMultiply(Matrix(*this), m);
    return *this;
}

//Переместить
inline Matrix &Matrix::Move(float dX, float dY, float dZ)
{
    pos.x += dX;
    pos.y += dY;
    pos.z += dZ;
    return *this;
}

//Переместить
inline Matrix &Matrix::Move(const Vector &pos)
{
    this->pos.x += pos.x;
    this->pos.y += pos.y;
    this->pos.z += pos.z;
    return *this;
}

//Отмасштабировать
inline Matrix &Matrix::Scale(float scale)
{
    Scale(scale, scale, scale);
    return *this;
}

//Отмасштабировать матрицу поворота
inline Matrix &Matrix::Scale3x3(float scale)
{
    Scale3x3(scale, scale, scale);
    return *this;
}

//Отмасштабировать
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

//Отмасштабировать матрицу поворота
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

//Отмасштабировать
inline Matrix &Matrix::Scale(const Vector &scale)
{
    Scale(scale.x, scale.y, scale.z);
    return *this;
}

//Отмасштабировать поворота
inline Matrix &Matrix::Scale3x3(const Vector &scale)
{
    Scale3x3(scale.x, scale.y, scale.z);
    return *this;
}

//Расчёт обратной матрицы
inline Matrix &Matrix::Inverse()
{
    pos = Vector(-(pos | vx), -(pos | vy), -(pos | vz));
    Transposition3X3();
    return *this;
}

///Расчёт обратной матрицы из другой
inline Matrix &Matrix::Inverse(const Matrix &mtx)
{
    pos = Vector(-(mtx.pos | mtx.vx), -(mtx.pos | mtx.vy), -(mtx.pos | mtx.vz));
    matrix[0] = mtx.matrix[0];
    matrix[1] = mtx.matrix[4];
    matrix[2] = mtx.matrix[8];
    matrix[4] = mtx.matrix[1];
    matrix[5] = mtx.matrix[5];
    matrix[6] = mtx.matrix[9];
    matrix[8] = mtx.matrix[2];
    matrix[9] = mtx.matrix[6];
    matrix[10] = mtx.matrix[10];

    /*	_asm
      {
        mov		eax, mtx
        mov		ebx, this
        mov		edx, [eax + 0*4]
        mov		ecx, [eax + 1*4]
        mov		esi, [eax + 2*4]
        mov		[ebx + 0*4], edx
        mov		[ebx + 4*4], ecx
        mov		[ebx + 8*4], esi
        mov		edx, [eax + 4*4]
        mov		ecx, [eax + 5*4]
        mov		esi, [eax + 6*4]
        mov		[ebx + 1*4], edx
        mov		[ebx + 5*4], ecx
        mov		[ebx + 9*4], esi
        mov		edx, [eax + 8*4]
        mov		ecx, [eax + 9*4]
        mov		esi, [eax + 10*4]
        mov		[ebx + 2*4], edx
        mov		[ebx + 6*4], ecx
        mov		[ebx + 10*4], esi
      }*/
    return *this;
}

///Расчёт масштабированной обратной матрицы
inline Matrix &Matrix::InverseWhithScale()
{
    //Матрица поворота
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
        for (long i = 0; i < 16; i++)
            matrix[i] = 0.0f;
    }
    //Позиция
    pos = -(MulNormal(pos));
    return *this;
}

//Транспанирование матрицы
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

    /*	_asm
      {
        mov		eax, this
        mov		ebx, [eax + 1*4]
        mov		ecx, [eax + 2*4]
        mov		esi, [eax + 4*4]
        mov		edi, [eax + 8*4]
        mov		[eax + 4*4], ebx
        mov		[eax + 8*4], ecx
        mov		[eax + 1*4], esi
        mov		[eax + 2*4], edi
        mov		ebx, [eax + 3*4]
        mov		ecx, [eax + 6*4]
        mov		esi, [eax + 12*4]
        mov		edi, [eax + 9*4]
        mov		[eax + 12*4], ebx
        mov		[eax + 9*4], ecx
        mov		[eax + 3*4], esi
        mov		[eax + 6*4], edi
        mov		ebx, [eax + 7*4]
        mov		ecx, [eax + 11*4]
        mov		esi, [eax + 13*4]
        mov		edi, [eax + 14*4]
        mov		[eax + 13*4], ebx
        mov		[eax + 14*4], ecx
        mov		[eax + 7*4], esi
        mov		[eax + 11*4], edi
      }*/
    return *this;
}

//Транспанирование элементов поворота
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

    /*	_asm
      {
        mov		eax, this
        mov		ebx, [eax + 4*4]
        mov		ecx, [eax + 8*4]
        mov		esi, [eax + 1*4]
        mov		edi, [eax + 2*4]
        mov		[eax + 1*4], ebx
        mov		[eax + 2*4], ecx
        mov		[eax + 4*4], esi
        mov		[eax + 8*4], edi
        mov		ebx, [eax + 6*4]
        mov		esi, [eax + 9*4]
        mov		[eax + 9*4], ebx
        mov		[eax + 6*4], esi
      }*/
    return *this;
}

//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------

//Считать только вращение
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

    /*	_asm
      {
        mov		esi, mtx
        mov		edi, this
        mov		ebx, [esi + 0*4]
        mov		ecx, [esi + 1*4]
        mov		edx, [esi + 2*4]
        mov		[edi + 0*4], ebx
        mov		[edi + 1*4], ecx
        mov		[edi + 2*4], edx
        mov		ebx, [esi + 4*4]
        mov		ecx, [esi + 5*4]
        mov		edx, [esi + 6*4]
        mov		[edi + 4*4], ebx
        mov		[edi + 5*4], ecx
        mov		[edi + 6*4], edx
        mov		ebx, [esi + 8*4]
        mov		ecx, [esi + 9*4]
        mov		edx, [esi + 10*4]
        mov		[edi + 8*4], ebx
        mov		[edi + 9*4], ecx
        mov		[edi + 10*4], edx
      }*/
    return *this;
}

//Перемножить матрицы и результат поместить в текущую
inline Matrix &Matrix::EqMultiply(const Matrix &m1, const Matrix &m2)
{
    Matrix m;
    m.EqMultiplyFast(m1, m2);
    Set(m);
    return *this;
}

//Перемножить матрицы и результат поместить в текущую m1 != this && m2 != this
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

//Умножить вершину на матрицу
inline Vector Matrix::MulVertex(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
    tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
    tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
    return tv;
}

//Умножить нормаль на матрицу
inline Vector Matrix::MulNormal(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
    tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
    tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;
    return tv;
}

//Умножить вершину на инверстую матрицу
inline Vector Matrix::MulVertexByInverse(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * (v.x - m[3][0]) + m[0][1] * (v.y - m[3][1]) + m[0][2] * (v.z - m[3][2]);
    tv.y = m[1][0] * (v.x - m[3][0]) + m[1][1] * (v.y - m[3][1]) + m[1][2] * (v.z - m[3][2]);
    tv.z = m[2][0] * (v.x - m[3][0]) + m[2][1] * (v.y - m[3][1]) + m[2][2] * (v.z - m[3][2]);
    return tv;
}

//Умножить нормаль на инверстую матрицу
inline Vector Matrix::MulNormalByInverse(const Vector &v) const
{
    Vector tv;
    tv.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
    tv.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
    tv.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
    return tv;
}

///Получить позицию камеры из матрицы камеры
inline Vector Matrix::GetCamPos() const
{
    return -MulNormalByInverse(pos);
}

//Единичная матрица или нет
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

//Скалирования матрица или нет
inline bool Matrix::IsScale() const
{
    const auto eps = 1e-4f;
    if (fabsf(~vx - 1.0f) > eps)
        return true;
    if (fabsf(~vy - 1.0f) > eps)
        return true;
    if (fabsf(~vz - 1.0f) > eps)
        return true;
    if (fabsf(w - 1.0f) > eps)
        return true;
    return false;
}

//Споецировать вершину (для матрицы проекции)
inline Vector4 Matrix::Projection(Vector vertex, float vphWidth05, float vphHeight05) const
{
    Vector4 res;
    //Преобразуем вершину
    res.x = m[0][0] * vertex.x + m[1][0] * vertex.y + m[2][0] * vertex.z + m[3][0];
    res.y = m[0][1] * vertex.x + m[1][1] * vertex.y + m[2][1] * vertex.z + m[3][1];
    res.z = m[0][2] * vertex.x + m[1][2] * vertex.y + m[2][2] * vertex.z + m[3][2];
    res.w = m[0][3] * vertex.x + m[1][3] * vertex.y + m[2][3] * vertex.z + m[3][3];
    //Коэфициент нормализации
    const auto w = 1.0f / res.w;
    //Нормализуем
    res.x = (1.0f + res.x * w) * vphWidth05;
    res.y = (1.0f - res.y * w) * vphHeight05;
    res.z *= w;
    res.w = w;
    return res;
}

//Споецировать массив вершин (для матрицы проекции)
inline void Matrix::Projection(Vector4 *dstArray, Vector *srcArray, long num, float vphWidth05, float vphHeight05,
                               long srcSize, long dstSize) const
{
    for (; num > 0; num--)
    {
        //Преобразуем вершину
        dstArray->x = m[0][0] * srcArray->x + m[1][0] * srcArray->y + m[2][0] * srcArray->z + m[3][0];
        dstArray->y = m[0][1] * srcArray->x + m[1][1] * srcArray->y + m[2][1] * srcArray->z + m[3][1];
        dstArray->z = m[0][2] * srcArray->x + m[1][2] * srcArray->y + m[2][2] * srcArray->z + m[3][2];
        dstArray->w = m[0][3] * srcArray->x + m[1][3] * srcArray->y + m[2][3] * srcArray->z + m[3][3];
        //Коэфициент нормализации
        const auto w = 1.0f / dstArray->w;
        //Нормализуем
        dstArray->x = (1.0f + dstArray->x * w) * vphWidth05;
        dstArray->y = (1.0f - dstArray->y * w) * vphHeight05;
        dstArray->z *= w;
        dstArray->w = w;
        //Указатели на следующие вершины
        srcArray = (Vector *)((char *)srcArray + srcSize);
        dstArray = (Vector4 *)((char *)dstArray + dstSize);
    }
}

//Получить углы из нескалированной матрицы поворота
inline void Matrix::GetAngles(float &ax, float &ay, float &az) const
{
    if (vz.y < 1.0f)
    {
        if (vz.y > -1.0f)
        {
            ax = static_cast<float>(asin(-vz.y));
            ay = static_cast<float>(atan2(vz.x, vz.z));
            az = static_cast<float>(atan2(vx.y, vy.y));
            return;
        }
        ax = 3.141592654f * 0.5f;
        ay = 0.0f;
        az = static_cast<float>(atan2(vx.z, vx.x));
    }
    else
    {
        ax = -3.141592654f * 0.5f;
        ay = 0.0f;
        az = static_cast<float>(-atan2(vx.z, vx.x));
    }
}

//Получить углы из нескалированной матрицы поворота
inline void Matrix::GetAngles(Vector &ang) const
{
    GetAngles(ang.x, ang.y, ang.z);
}

//Доступиться до элементов матрицы через скобки
inline float &Matrix::operator()(long i, long j)
{
    return m[i][j];
}

//Получить указатель на матрицу D3D
inline Matrix::operator D3DXMATRIX *() const
{
    return ((D3DXMATRIX *)matrix);
}

//Получить вектор для расчёта X компоненты
inline Vector4 Matrix::GetVectorX() const
{
    return Vector4(m[0][0], m[1][0], m[2][0], m[3][0]);
}

//Получить вектор для расчёта Y компоненты
inline Vector4 Matrix::GetVectorY() const
{
    return Vector4(m[0][1], m[1][1], m[2][1], m[3][1]);
}

//Получить вектор для расчёта Z компоненты
inline Vector4 Matrix::GetVectorZ() const
{
    return Vector4(m[0][2], m[1][2], m[2][2], m[3][2]);
}

//Получить вектор для расчёта W компоненты
inline Vector4 Matrix::GetVectorW() const
{
    return Vector4(m[0][3], m[1][3], m[2][3], m[3][3]);
}

#pragma pack(pop)

#endif
