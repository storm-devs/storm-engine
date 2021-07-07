#ifndef _MATRIX_H_
#define _MATRIX_H_


#include "cvector.h"


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
	union
	{
		float matrix[16];
		float m[4][4];
	};

//-----------------------------------------------------------
//Constructors
//-----------------------------------------------------------
	CMatrix();//Identity matrix
	CMatrix(float angX, float angY, float angZ, float x, float y, float z);
	CMatrix(float angX, float angY, float angZ);
	CMatrix(CVECTOR & ang, CVECTOR & pos);
	CMatrix(CVECTOR & ang);	
	CMatrix(const CMatrix & matrix);
	CMatrix(CMatrix * matrix);
	//this = m1*m2
	CMatrix(CMatrix & m1, CMatrix & m2);

	
//-----------------------------------------------------------
//Main
//-----------------------------------------------------------
	//Create identity matrix
	void SetIdentity();
	
	//Build matrix
	//M = rotZ*rotX*rotY*Pos
	void BuildMatrix(float angX, float angY, float angZ, float x, float y, float z);
	void BuildMatrix(float angX, float angY, float angZ);
	void BuildMatrix(CVECTOR & ang, CVECTOR & pos);
	void BuildMatrix(CVECTOR & ang);
	//M = rotX*rotY*rotZ*Pos
	void BuildMatrixXYZ(float angX, float angY, float angZ, float x, float y, float z);

	//Build rotate matrix
	void BuildRotateX(float ang);
	void BuildRotateY(float ang);
	void BuildRotateZ(float ang);
	//Build position matrix
	void BuildPosition(float x, float y, float z);
	
	//Equal
	void operator = (const CMatrix & matrix);

	//Multiply
	void operator *= (CMatrix & matrix);
	void operator *= (float k);
	CMatrix operator * (CMatrix & matrix);
	CVECTOR operator * (CVECTOR & vector);
	

	//this = m1*m2, (m1 != this, m2 != this)
	void EqMultiply(CMatrix & m1, CMatrix & m2);
	//Transform vertex to local coordinate system
	void MulToInv(const CVECTOR & srcVrt, CVECTOR & resVrt);
	//Transform normal to local coordinate system
	void MulToInvNorm(CVECTOR & srcNorm, CVECTOR & resNorm);
	
	//Transposition
	void Transposition();
	void Transposition3X3();	
	
	//Rotate
	void RotateX(float ang);
	void RotateY(float ang);
	void RotateZ(float ang);
	//Move
	void Move(float dX, float dY, float dZ);
	//Move(-(pos * this))
	void MoveInversePosition(float x, float y, float z);
	//Set new position
	void SetPosition(float x, float y, float z);
	void SetPosition(CVECTOR & pos);
	//SetPosition(-(pos * this))
	void SetInversePosition(float x, float y, float z);
	
//-----------------------------------------------------------
//Utils
//-----------------------------------------------------------
	
	//Access to axis vectors
	CVECTOR & Vx() const;		//Вектор по X (ссылка)
	CVECTOR & Vy() const;		//Вектор по Y (ссылка)
	CVECTOR & Vz() const;		//Вектор по Z (ссылка)
	CVECTOR & Pos() const;	//Позиция (ссылка)
	
	//Access to matrix element
	float & operator () (long i, long j);


	//Create only rotate matrix
	void Get3X3(CMatrix & mtr);
	void Get3X3(CMatrix * mtr);

	//Projection
	void BuildProjectionMatrix(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);
	void Projection(CVECTOR * srcArray, MTX_PRJ_VECTOR * dstArray, long num, float vphWidth05, float vphHeight05, long srcStrcSize, long dstStrcSize);

	//View
	bool BuildViewMatrix(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR upVector);

	//Mirror
	void BuildMirrorMatrix(float Nx, float Ny, float Nz, float D);
	
	//D3D extends (return (D3DXMATRIX *)pointer)
	operator D3DXMATRIX * ();
	
};

//============================================================================================
//Constructors
//============================================================================================

inline CMatrix::CMatrix()
{
	SetIdentity();
}

inline CMatrix::CMatrix(float angX, float angY, float angZ)
{
	BuildMatrix(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
}

inline CMatrix::CMatrix(float angX, float angY, float angZ, float x, float y, float z)
{
	BuildMatrix(angX, angY, angZ, x, y, z);
}

inline CMatrix::CMatrix(CVECTOR & ang, CVECTOR & pos)
{
	BuildMatrix(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);	
}

inline CMatrix::CMatrix(CVECTOR & ang)
{
	BuildMatrix(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
}

inline CMatrix::CMatrix(const CMatrix & matrix)
{
	*this = matrix;
}

inline CMatrix::CMatrix(CMatrix * matrix)
{
	*this = *matrix;
}

//this = m1*m2
inline CMatrix::CMatrix(CMatrix & m1, CMatrix & m2)
{
	EqMultiply(m1, m2);
}

//============================================================================================
//Main
//============================================================================================

//Create identity matrix
inline void CMatrix::SetIdentity()
{
	_asm
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
	}
}

//Build matrix
inline void CMatrix::BuildMatrix(float angX, float angY, float angZ, float x, float y, float z)
{
	register float sinAx = sinf(angX);
	register float cosAx = cosf(angX);
	register float sinAy = sinf(angY);
	register float cosAy = cosf(angY);
	register float sinAz = sinf(angZ);
	register float cosAz = cosf(angZ);
	
	//Создаём матрицу с порядком вращений rz*rx*ry
	m[0][0] = cosAz*cosAy + sinAz*sinAx*sinAy;
	m[1][0] = -sinAz*cosAy + cosAz*sinAx*sinAy;
	m[2][0] = cosAx*sinAy;
	m[3][0] = x;

	m[0][1] = sinAz*cosAx;
	m[1][1] = cosAz*cosAx;
	m[2][1] = -sinAx;
	m[3][1] = y;

	m[0][2] = cosAz*-sinAy + sinAz*sinAx*cosAy;
	m[1][2] = -sinAz*-sinAy + cosAz*sinAx*cosAy;
	m[2][2] = cosAx*cosAy;
	m[3][2] = z;

	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;	
	m[3][3] = 1.0f;
}

inline void CMatrix::BuildMatrixXYZ(float angX, float angY, float angZ, float x, float y, float z)
{
	register float sinAx = sinf(angX);
	register float cosAx = cosf(angX);
	register float sinAy = sinf(angY);
	register float cosAy = cosf(angY);
	register float sinAz = sinf(angZ);
	register float cosAz = cosf(angZ);
	
	m[0][0] = cosAy*cosAz;
	m[1][0] = sinAx*sinAy*cosAz-cosAx*sinAz;
	m[2][0] = cosAx*sinAy*cosAz+sinAx*sinAz;
	m[3][0] = x;
	
	m[0][1] = cosAy*sinAz;
	m[1][1] = sinAx*sinAy*sinAz+cosAx*cosAz;
	m[2][1] = cosAx*sinAy*sinAz-sinAx*cosAz;
	m[3][1] = y;

	m[0][2] = -sinAy;
	m[1][2] = sinAx*cosAy;
	m[2][2] = cosAx*cosAy;
	m[3][2] = z;

	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;	
	m[3][3] = 1.0f;
}


inline void CMatrix::BuildMatrix(float angX, float angY, float angZ)
{
	BuildMatrix(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
}

inline void CMatrix::BuildMatrix(CVECTOR & ang, CVECTOR & pos)
{
	BuildMatrix(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
}

inline void CMatrix::BuildMatrix(CVECTOR & ang)
{
	BuildMatrix(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
}

//Build rotate matrix
inline void CMatrix::BuildRotateX(float ang)
{
	SetIdentity();
	m[1][1] = cosf(ang);
	m[2][1] = -sinf(ang);
	m[1][2] = sinf(ang);
	m[2][2] = cosf(ang);
}

inline void CMatrix::BuildRotateY(float ang)
{
	SetIdentity();
	m[0][0] = cosf(ang);
	m[2][0] = sinf(ang);
	m[0][2] = -sinf(ang);
	m[2][2] = cosf(ang);
}

inline void CMatrix::BuildRotateZ(float ang)
{
	SetIdentity();
	m[0][0] = cosf(ang);
	m[1][0] = -sinf(ang);
	m[0][1] = sinf(ang);
	m[1][1] = cosf(ang);
}

//Build position matrix
inline void CMatrix::BuildPosition(float x, float y, float z)
{
	SetIdentity();
	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
}

//Equal
inline void CMatrix::operator = (const CMatrix & matrix)
{
	_asm
	{
		cld
		mov		edi, this
		mov		esi, dword ptr matrix
		mov		ecx, 16
		rep		movsd
	}
}

//Multiply
inline void CMatrix::operator *= (CMatrix & matrix)
{
	CMatrix tmp;
	tmp.EqMultiply(*this, matrix);
	*this = tmp;
}


inline void CMatrix::operator *= (float k)
{
	m[0][0] *= k; m[1][0] *= k; m[2][0] *= k; m[3][0] *= k;
	m[0][1] *= k; m[1][1] *= k; m[2][1] *= k; m[3][1] *= k;
	m[0][2] *= k; m[1][2] *= k; m[2][2] *= k; m[3][2] *= k;
	m[0][3] *= k; m[1][3] *= k; m[2][3] *= k; m[3][3] *= k;
}

inline CMatrix CMatrix::operator * (CMatrix & matrix)
{
	CMatrix tmp;
	tmp.EqMultiply(*this, matrix);
	return tmp;
}

inline CVECTOR CMatrix::operator * (CVECTOR & vector)
{
	CVECTOR tmp;
	tmp.x = matrix[0]*vector.x + matrix[4]*vector.y + matrix[8]*vector.z + matrix[12];
	tmp.y = matrix[1]*vector.x + matrix[5]*vector.y + matrix[9]*vector.z + matrix[13];
	tmp.z = matrix[2]*vector.x + matrix[6]*vector.y + matrix[10]*vector.z + matrix[14];
	return tmp;
}

//this = m1*m2, (m1 != this, m2 != this)
inline void CMatrix::EqMultiply(CMatrix & m1, CMatrix & m2)
{	
	matrix[0] = m2.matrix[0]*m1.matrix[0] + m2.matrix[4]*m1.matrix[1] + m2.matrix[8]*m1.matrix[2] + m2.matrix[12]*m1.matrix[3];
	matrix[1] = m2.matrix[1]*m1.matrix[0] + m2.matrix[5]*m1.matrix[1] + m2.matrix[9]*m1.matrix[2] + m2.matrix[13]*m1.matrix[3];
	matrix[2] = m2.matrix[2]*m1.matrix[0] + m2.matrix[6]*m1.matrix[1] + m2.matrix[10]*m1.matrix[2] + m2.matrix[14]*m1.matrix[3];
	matrix[3] = m2.matrix[3]*m1.matrix[0] + m2.matrix[7]*m1.matrix[1] + m2.matrix[11]*m1.matrix[2] + m2.matrix[15]*m1.matrix[3];

	matrix[4] = m2.matrix[0]*m1.matrix[4] + m2.matrix[4]*m1.matrix[5] + m2.matrix[8]*m1.matrix[6] + m2.matrix[12]*m1.matrix[7];
	matrix[5] = m2.matrix[1]*m1.matrix[4] + m2.matrix[5]*m1.matrix[5] + m2.matrix[9]*m1.matrix[6] + m2.matrix[13]*m1.matrix[7];
	matrix[6] = m2.matrix[2]*m1.matrix[4] + m2.matrix[6]*m1.matrix[5] + m2.matrix[10]*m1.matrix[6] + m2.matrix[14]*m1.matrix[7];
	matrix[7] = m2.matrix[3]*m1.matrix[4] + m2.matrix[7]*m1.matrix[5] + m2.matrix[11]*m1.matrix[6] + m2.matrix[15]*m1.matrix[7];

	matrix[8] = m2.matrix[0]*m1.matrix[8] + m2.matrix[4]*m1.matrix[9] + m2.matrix[8]*m1.matrix[10] + m2.matrix[12]*m1.matrix[11];
	matrix[9] = m2.matrix[1]*m1.matrix[8] + m2.matrix[5]*m1.matrix[9] + m2.matrix[9]*m1.matrix[10] + m2.matrix[13]*m1.matrix[11];
	matrix[10] = m2.matrix[2]*m1.matrix[8] + m2.matrix[6]*m1.matrix[9] + m2.matrix[10]*m1.matrix[10] + m2.matrix[14]*m1.matrix[11];
	matrix[11] = m2.matrix[3]*m1.matrix[8] + m2.matrix[7]*m1.matrix[9] + m2.matrix[11]*m1.matrix[10] + m2.matrix[15]*m1.matrix[11];

	matrix[12] = m2.matrix[0]*m1.matrix[12] + m2.matrix[4]*m1.matrix[13] + m2.matrix[8]*m1.matrix[14] + m2.matrix[12]*m1.matrix[15];
	matrix[13] = m2.matrix[1]*m1.matrix[12] + m2.matrix[5]*m1.matrix[13] + m2.matrix[9]*m1.matrix[14] + m2.matrix[13]*m1.matrix[15];
	matrix[14] = m2.matrix[2]*m1.matrix[12] + m2.matrix[6]*m1.matrix[13] + m2.matrix[10]*m1.matrix[14] + m2.matrix[14]*m1.matrix[15];
	matrix[15] = m2.matrix[3]*m1.matrix[12] + m2.matrix[7]*m1.matrix[13] + m2.matrix[11]*m1.matrix[14] + m2.matrix[15]*m1.matrix[15];
}

//Transform vertex to local coordinate system
inline void CMatrix::MulToInv(const CVECTOR & src, CVECTOR & res)
{
	res.x = (src.x - matrix[12])*matrix[0] + (src.y - matrix[13])*matrix[1] + (src.z - matrix[14])*matrix[2];
	res.y = (src.x - matrix[12])*matrix[4] + (src.y - matrix[13])*matrix[5] + (src.z - matrix[14])*matrix[6];
	res.z = (src.x - matrix[12])*matrix[8] + (src.y - matrix[13])*matrix[9] + (src.z - matrix[14])*matrix[10];
}

//Transform normal to local coordinate system
inline void CMatrix::MulToInvNorm(CVECTOR & src, CVECTOR & res)
{
	res.x = src.x*matrix[0] + src.y*matrix[1] + src.z*matrix[2];
	res.y = src.x*matrix[4] + src.y*matrix[5] + src.z*matrix[6];
	res.z = src.x*matrix[8] + src.y*matrix[9] + src.z*matrix[10];
}

//Transposition
inline void CMatrix::Transposition()
{
	float x = Pos()|Vx();
	float y = Pos()|Vy();
	float z = Pos()|Vz();
	Pos().x = -x;
	Pos().y = -y;
	Pos().z = -z;
	Transposition3X3();	
}

inline void CMatrix::Transposition3X3()
{
	_asm
	{
		mov		eax, this
		mov		ebx, [eax + 4*4]
		mov		ecx, [eax + 8*4]
		mov		esi, [eax + 1*4]
		mov		edi, [eax + 2*4]
		mov		[eax + 4*4], esi
		mov		[eax + 8*4], edi
		mov		[eax + 1*4], ebx
		mov		[eax + 2*4], ecx
		mov		ebx, [eax + 6*4]
		mov		esi, [eax + 9*4]
		mov		[eax + 6*4], esi		
		mov		[eax + 9*4], ebx
	}
}

//Rotate
inline void CMatrix::RotateX(float ang)
{
	CMatrix mtr;
	mtr.BuildRotateX(ang);
	*this *= mtr;
}

inline void CMatrix::RotateY(float ang)
{
	CMatrix mtr;
	mtr.BuildRotateY(ang);
	*this *= mtr;
}

inline void CMatrix::RotateZ(float ang)
{
	CMatrix mtr;
	mtr.BuildRotateZ(ang);
	*this *= mtr;
}

//Move
inline void CMatrix::Move(float dX, float dY, float dZ)
{
	matrix[12] += dX; matrix[13] += dY; matrix[14] += dZ;
}

//Move(-(pos * this))
inline void CMatrix::MoveInversePosition(float x, float y, float z)
{
	m[3][0] -= m[0][0]*x + m[1][0]*y + m[2][0]*z;
	m[3][1] -= m[0][1]*x + m[1][1]*y + m[2][1]*z;
	m[3][2] -= m[0][2]*x + m[1][2]*y + m[2][2]*z;
}

//Set new position
inline void CMatrix::SetPosition(float x, float y, float z)
{
	matrix[12] = x; matrix[13] = y; matrix[14] = z;
}

inline void CMatrix::SetPosition(CVECTOR & pos)
{
	matrix[12] = pos.x; matrix[13] = pos.y; matrix[14] = pos.z;
}

//SetPosition(-(pos * this))
inline void CMatrix::SetInversePosition(float x, float y, float z)
{
	m[3][0] = -(m[0][0]*x + m[1][0]*y + m[2][0]*z);
	m[3][1] = -(m[0][1]*x + m[1][1]*y + m[2][1]*z);
	m[3][2] = -(m[0][2]*x + m[1][2]*y + m[2][2]*z);
}

//============================================================================================
//Utils
//============================================================================================
	
//Access to axis vectors
inline CVECTOR & CMatrix::Vx() const
{
	return *(CVECTOR *)(matrix);
}

inline CVECTOR & CMatrix::Vy() const
{
	return *(CVECTOR *)(matrix + 4);
}

inline CVECTOR & CMatrix::Vz() const
{
	return *(CVECTOR *)(matrix + 8);
}

inline CVECTOR & CMatrix::Pos() const
{
	return *(CVECTOR *)(matrix + 12);
}

//Access to matrix element
inline float & CMatrix::operator () (long i, long j)
{
	return m[i][j];
}

//Create only rotate matrix
inline void CMatrix::Get3X3(CMatrix & mtr)
{
	_asm
	{
		mov		esi, this
		mov		edi, dword ptr mtr
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
	}
}

inline void CMatrix::Get3X3(CMatrix * mtr)
{
	_asm
	{
		mov		esi, this
		mov		edi, mtr
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
	}
}

//Projection
inline void CMatrix::BuildProjectionMatrix(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
	_asm
	{
		cld
		xor		eax, eax
		mov		edi, this
		mov		ecx, 16
		rep		stosd
	}
	float cs = cosf(viewAngle*0.5f); float sn = sinf(viewAngle*0.5f);
	double Q = (double)zFar/(double)(zFar - zNear);

	matrix[0] = (vpHeight/vpWidth)*(cs/sn);
	matrix[5] = 1.0f*(cs/sn);
	matrix[10] = (float)Q;
	matrix[11] = 1.0f;
	matrix[14] = -(float)Q*zNear;
}

inline void CMatrix::Projection(CVECTOR * srcArray, MTX_PRJ_VECTOR * dstArray, long num, float vphWidth05, float vphHeight05, long srcStrcSize, long dstStrcSize)
{
	register float k;
	for(; num > 0; num--)
	{
		dstArray->x = matrix[0]*srcArray->x + matrix[4]*srcArray->y + matrix[8]*srcArray->z + matrix[12];
		dstArray->y = matrix[1]*srcArray->x + matrix[5]*srcArray->y + matrix[9]*srcArray->z + matrix[13];
		dstArray->z = matrix[2]*srcArray->x + matrix[6]*srcArray->y + matrix[10]*srcArray->z + matrix[14];
		dstArray->rhw = matrix[3]*srcArray->x + matrix[7]*srcArray->y + matrix[11]*srcArray->z + matrix[15];

		k = 1.0f/dstArray->rhw;

		dstArray->x = (1.0f + dstArray->x*k)*vphWidth05;
		dstArray->y = (1.0f - dstArray->y*k)*vphHeight05;
		dstArray->z *= k;
		
		dstArray->rhw = k;

		srcArray = (CVECTOR *)((long)srcArray + srcStrcSize);
		dstArray = (MTX_PRJ_VECTOR *)((long)dstArray + dstStrcSize);
	}
}

//View
inline bool CMatrix::BuildViewMatrix(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR upVector)
{
	SetIdentity();
	//Нормализуем вектор смотрения
	lookTo -= lookFrom;
	float l = ~lookTo;
	if(l == 0.0f)
	{
		//Ставим позицию для неповёрнутой матрици
		Pos() = -lookFrom;
		return false;
	}
	lookTo *= 1.0f/sqrtf(l);
	//Направляем вектор вверх в нужном направлении
	upVector -= lookTo*(lookTo | upVector);
	//Нормализуем вертор направленный вверх
	l = ~upVector;
	if(l == 0.0f) upVector.y = 1.0f;
		else upVector *= 1.0f/sqrtf(l);
	//Ищем третий вектор базиса
	CVECTOR v = upVector ^ lookTo;
	l = ~v;
	if(l != 0.0f)
	{
		v *= (1.0f/sqrtf(l));
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

//Mirror
//Формирование матрицы отражение по плану
inline void CMatrix::BuildMirrorMatrix(float Nx, float Ny, float Nz, float D)
{
	m[0][0] = -Nx*2.0f*Nx + 1.0f;
	m[1][0] = -Nx*2.0f*Ny;
	m[2][0] = -Nx*2.0f*Nz;
	m[3][0] = -Nx*2.0f*-D;

	m[0][1] = -Ny*2.0f*Nx;
	m[1][1] = -Ny*2.0f*Ny + 1.0f;
	m[2][1] = -Ny*2.0f*Nz;
	m[3][1] = -Ny*2.0f*-D;

	m[0][2] = -Nz*2.0f*Nx;
	m[1][2] = -Nz*2.0f*Ny;
	m[2][2] = -Nz*2.0f*Nz + 1.0f;
	m[3][2] = -Nz*2.0f*-D;

	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][3] = 1.0f;
}

//D3D extends (return (D3DXMATRIX *)pointer)
inline CMatrix::operator D3DXMATRIX * ()
{
	return ((D3DXMATRIX*)matrix);
};

//============================================================================================

#endif
