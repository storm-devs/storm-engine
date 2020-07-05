#ifndef _PARTICLE_DATA_SOURCE_H_
#define _PARTICLE_DATA_SOURCE_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\datadesc\data_desc.h"
#include "..\..\icommon\names.h"
#include "..\..\..\common_h\exs.h"
#include "..\..\..\common_h\templates.h"
#include "..\..\icommon\types.h"
#include "fieldlist.h"




class DataObject;
class DataColor;
class DataBool;
class DataFloat;
class DataGraph;
class DataString;
class DataPosition;
class DataUV;

class MemFile;

class IParticleManager;

//Источник данных для партиклов...
class DataSource
{

public:

	//Описание партикла (используеться при создании системы)
	struct ParticleDesc
	{
		ParticleType Type;
		FieldList Fields;

		ParticleDesc ()
		{
			Type = UNKNOWN_PARTICLE;
		}
	};

	//Описание эмиттера (используеться при создании системы)
	struct EmitterDesc
	{
		EmitterType Type;
		FieldList Fields;
		array<ParticleDesc> Particles;

		EmitterDesc () : Particles(_FL_)
		{
			Type = UNKNOWN_EMITTER;
		}
	};

private:
	array<EmitterDesc> Emitters;

	//Загрузить точечный эмиттер
	void CreatePointEmitter (MemFile* pMemFile);
	
	//Загрузить BillBoard партикл
	void CreateBillBoardParticle (array<ParticleDesc> &Particles, MemFile* pMemFile);

	//Загрузить Model партикл
	void CreateModelParticle (array<ParticleDesc> &Particles, MemFile* pMemFile);


	int FindEmitter (const char* Name);
	

protected:

 virtual ~DataSource ();

public:

	FieldList* CreateEmptyPointEmitter (const char* EmitterName);
	FieldList* CreateBillBoardParticle (const char* ParticleName, const char* EmitterName);
	FieldList* CreateModelParticle (const char* ParticleName, const char* EmitterName);

	void DeletePointEmitter (FieldList* pEmitter);
	void DeleteBillboard (FieldList* pEmitter, FieldList* pParticles);
	void DeleteModel (FieldList* pEmitter, FieldList* pParticles);
 
	void Destroy ();

	//---------- Создание/удаление --------------------
	DataSource (IParticleManager* Master);
	bool Release ();


// ========================= Load & Save =======================================
	//Сохранить/восстановить из файла
	virtual void Write (MemFile* pMemFile);
	virtual void Load (MemFile* pMemFile);



	virtual int GetEmitterCount ();
	DataSource::EmitterDesc* GetEmitterDesc (int Index);
};







#endif