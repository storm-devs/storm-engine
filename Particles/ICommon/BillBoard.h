#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_


#include "..\..\common_h\math3d.h"

//Партикл
struct ParticleBillboard
{
	Vector	vPos;
	float		fSizeX, fSizeY;
	float		fAngle;
	dword		dwColor;
	float		DistanceToCamera;
	bool		Visible;


//Для первого кадра------------------------------------
	//Верхний левый угол в UV
	float		U11;
	float		V11;

	//Нижний правый угол в UV
	float		U12;
	float		V12;

//Для второго кадра------------------------------------
	//Верхний левый угол в UV
	float		U21;
	float		V21;

	//Нижний правый угол в UV
	float		U22;
	float		V22;

//Коэфицент блендинга между первым и вторым кадром
	float BlendK;


//Конструктор-------------------------------------------
	ParticleBillboard()
	{
		DistanceToCamera = 0.0f;
		Visible = false;
	}
};


#endif