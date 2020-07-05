//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef BILLBOARD_PARTICLE_PROCESSOR
#define BILLBOARD_PARTICLE_PROCESSOR

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\..\..\common_h\exs.h"
#include "..\..\..\common_h\templates.h"
#include "..\..\..\common_h\dx8render.h"
#include "..\..\..\common_h\particles\gmx_QSort.h"


#include "..\..\icommon\particle.h"
#include "..\datasource\fieldlist.h"

class ParticleSystem;



class BillBoardProcessor
{

	struct RECT_VERTEX
	{
		Vector	vRelativePos;
		dword	dwColor;
		float	tu1, tv1;
		float	tu2, tv2;
		float angle;
		float BlendK;
		Vector	vParticlePos;
		float AddPowerK;
	};

	VDX8RENDER* pRS;
	//Буферы для рендера билбордов
	long pVBuffer;
	long pIBuffer;


	struct MemArrayItem
	{
		BB_ParticleData pData;
		bool Free;

		MemArrayItem ()
		{
			Free = true;
		}
	};

	MemArrayItem* pMemArray;

	array<BB_ParticleData*> Particles;

	GMXQSort<BB_ParticleData*> ParticleSorter;


	//Считает расстояние до билбоардов
	DWORD CalcDistanceToCamera ();

	//Функция сравнения при сортировке
	static BOOL CompareFunction (BB_ParticleData* e1, BB_ParticleData* e2);


	BB_ParticleData* AllocParticle ();
	void FreeParticle (BB_ParticleData* pItem);




 public:

  BillBoardProcessor (); 
  ~BillBoardProcessor (); 

	void AddParticle (ParticleSystem* pSystem, const Vector& velocity_dir, const Vector& pos, const Matrix& matWorld, float EmitterTime, float EmitterLifeTime, FieldList* pFields, DWORD* pActiveCount, DWORD dwGUID);

	void Process (float DeltaTime);
	void Draw ();

	DWORD GetCount ();

	void DeleteWithGUID (DWORD dwGUID, DWORD GUIDRange = GUIDSTEP);


	void Clear ();

};


#endif