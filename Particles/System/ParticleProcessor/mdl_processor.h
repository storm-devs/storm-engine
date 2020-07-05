//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef MODEL_PARTICLE_PROCESSOR
#define MODEL_PARTICLE_PROCESSOR

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\..\..\common_h\exs.h"
#include "..\..\..\common_h\templates.h"
#include "..\..\..\common_h\dx8render.h"
#include "..\..\..\common_h\particles\gmx_QSort.h"


#include "..\..\icommon\particle.h"
#include "..\datasource\fieldlist.h"
#include "nameparser.h"

class ParticleSystem;
class ParticleManager;

class ModelProcessor
{
	VDX8RENDER* pRS;
	ParticleManager* pMasterManager;
	GeomNameParser Parser;

	struct MemArrayItem
	{
		MDL_ParticleData pData;
		bool Free;

		MemArrayItem ()
		{
			Free = true;
		}
	};

	MemArrayItem* pMemArray;

	array<MDL_ParticleData*> Particles;


	MDL_ParticleData* AllocParticle ();
	void FreeParticle (MDL_ParticleData* pItem);



 public:

  ModelProcessor (ParticleManager* pManager); 
  ~ModelProcessor (); 

	void AddParticle (ParticleSystem* pSystem, const Vector& velocity_dir, const Vector& pos, const Matrix& matWorld, float EmitterTime, float EmitterLifeTime, FieldList* pFields, DWORD* pActiveCount, DWORD dwGUID);

	void Process (float DeltaTime);
	void Draw ();

	DWORD GetCount ();

	void DeleteWithGUID (DWORD dwGUID, DWORD GUIDRange = GUIDSTEP);

	void Clear ();


};


#endif