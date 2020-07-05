//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	Bone
//--------------------------------------------------------------------------------------------
//	Кость модели, содержищая свою анимацию
//============================================================================================

#ifndef _Bone_H_
#define _Bone_H_

//============================================================================================

#include "..\Common_h\Matrix.h"
#include "..\Common_h\d_types.h"
#include "..\Common_h\memop.h"
#include "..\Common_h\exs.h"


#include <d3dx8.h>
//============================================================================================

#ifndef PI
#define PI	3.141592653589793238f
#endif

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


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Bone();
	virtual ~Bone();
	//Установить родителя
	void SetParent(Bone * parentBone);
	//Сказать сколько будет кадров анимации
	void SetNumFrames(long num, CVECTOR & sPos, bool isRoot = false);
	//Установить позиции анимации
	void SetPositions(const CVECTOR * pArray, long numPos);
	//Установить углы анимации
	void SetAngles(const D3DXQUATERNION * aArray, long numAng);
	//Инициализировать стартовую матрицу
	void BuildStartMatrix();


//--------------------------------------------------------------------------------------------
//Работа с костью
//--------------------------------------------------------------------------------------------
public:	
	//Добавить кадры анимации
	void BlendFrame(long frame, float kBlend, D3DXQUATERNION & res);
	//void BlendFrame(float frame);
	//Создать матрицу для полученного положения
	void BuildMatrix();
	//Создать матрицу для 0 кадра
	//void BuildMatrixZero();
	//Получить матрицу положения
	CMatrix & Matrix();
	//Получить стартовуцю матрицу положения
	CMatrix & StartMatrix();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void GetFrame(long f, D3DXQUATERNION & qt);
	float Clamp(float v, const char * str);
	//Линейная интерполяция позиции
	float LerpPos(float a, float b, float k);
	//Линейная интерполяция угла
	float LerpAng(float a, float b, float k);

public:
	Bone * parent;			//Родительская кость

#ifdef ANI_COMPRESS_ENABLE
	COMP_QUATERNION * ang;	//Компрессированые углы кадров анимации
#else
	D3DXQUATERNION * ang;	//Углы кадров анимации
#endif

	CVECTOR * pos;			//Позиции кадров анимации
	long numFrames;			//Количество кадров анимации
	CVECTOR pos0;			//Позиция кости, если нет позиций кадров анимации

	CVECTOR p;				//Позиция кости в локальных координатах
	D3DXQUATERNION a;				//Углы поворота кости в локальных координатах
	CMatrix matrix;			//Матрица положения кости
	CMatrix start;			//Матрица 0 кадра
};

//============================================================================================
//inline
//============================================================================================

//Установить родителя
inline void Bone::SetParent(Bone * parentBone)
{
	parent = parentBone;
}

//Получить матрицу положения
inline CMatrix & Bone::Matrix()
{
	return matrix;
}

//Получить стартовуцю матрицу положения
inline CMatrix & Bone::StartMatrix()
{
	return start;
}

//Линейная интерполяция позиции
inline float Bone::LerpPos(float a, float b, float k)
{
	return a + k*(b - a);
}

//Линейная интерполяция угла
inline float Bone::LerpAng(float a, float b, float k)
{
	float delta = b - a;
	if(delta < -PI) delta = 2*PI + delta;
	if(delta >  PI) delta =-2*PI + delta;
	return a + k*delta;	
}


#endif

