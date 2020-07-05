#include "datasource.h"

#include "databool.h"
#include "datacolor.h"
#include "datafloat.h"
#include "datagraph.h"
#include "dataposition.h"
#include "datauv.h"
#include "datastring.h"


#include "..\..\icommon\memfile.h"
#include "..\..\..\common_h\vmodule_api.h"

#define HEADER "PSYS"
#define VERSION "v3.5"




BEGIN_DATA_DESC(PointEmitterDesc)
	DATA_STRING (EMITTER_NAME);
	DATA_FLOAT (EMITTER_LIFETIME);
	DATA_POSITION (EMITTER_POSITION);
	DATA_GRAPH (EMISSION_DIR_X);
	DATA_GRAPH (EMISSION_DIR_Y);
	DATA_GRAPH (EMISSION_DIR_Z);
	DATA_BOOL (EMITTER_LOOPING);
END_DATA_DESC(PointEmitterDesc)

BEGIN_DATA_DESC(BillboardParticleDesc)
	DATA_STRING(PARTICLE_NAME);
	DATA_GRAPH (PARTICLE_EMISSION_RATE);
	DATA_FLOAT (PARTICLE_MAX_COUNT);
	DATA_GRAPH (PARTICLE_TRACK_X);
	DATA_GRAPH (PARTICLE_TRACK_Y);
	DATA_GRAPH (PARTICLE_TRACK_Z);
	DATA_GRAPH (PARTICLE_LIFE_TIME);
	DATA_GRAPH (PARTICLE_MASS);
	DATA_GRAPH (PARTICLE_SPIN);
	DATA_GRAPH (PARTICLE_VELOCITY_POWER);
	DATA_GRAPH (PARTICLE_SPIN_DRAG);
	DATA_GRAPH (PARTICLE_SIZE);
	DATA_GRAPH (PARTICLE_ANIMFRAME);
	DATA_COLOR (PARTICLE_COLOR);
	DATA_UV (PARTICLE_FRAMES);
	DATA_GRAPH (PARTICLE_TRANSPARENCY);
	DATA_GRAPH (PARTICLE_DRAG);
	DATA_GRAPH (PARTICLE_PHYSIC_BLEND);
	DATA_STRING (ATTACHEDEMITTER_NAME);
	DATA_GRAPH (PARTICLE_GRAVITATION_K);
	DATA_BOOL (PARTICLE_DIR_ORIENT);
	DATA_GRAPH (PARTICLE_ADDPOWER);
END_DATA_DESC(BillboardParticleDesc)

BEGIN_DATA_DESC(ModelParticleDesc)
	DATA_STRING(PARTICLE_NAME);
	DATA_GRAPH (PARTICLE_EMISSION_RATE);
	DATA_FLOAT (PARTICLE_MAX_COUNT);
	DATA_GRAPH (PARTICLE_TRACK_X);
	DATA_GRAPH (PARTICLE_TRACK_Y);
	DATA_GRAPH (PARTICLE_TRACK_Z);
	DATA_GRAPH (PARTICLE_LIFE_TIME);
	DATA_GRAPH (PARTICLE_MASS);
	DATA_GRAPH (PARTICLE_SPIN_X);
	DATA_GRAPH (PARTICLE_SPIN_Y);
	DATA_GRAPH (PARTICLE_SPIN_Z);
	DATA_GRAPH (PARTICLE_VELOCITY_POWER);
	DATA_GRAPH (PARTICLE_SPIN_DRAGX);
	DATA_GRAPH (PARTICLE_SPIN_DRAGY);
	DATA_GRAPH (PARTICLE_SPIN_DRAGZ);
	DATA_GRAPH (PARTICLE_DRAG);
	DATA_GRAPH (PARTICLE_PHYSIC_BLEND);
	DATA_STRING (ATTACHEDEMITTER_NAME);
	DATA_GRAPH (PARTICLE_GRAVITATION_K);
	DATA_STRING (PARTICLE_GEOM_NAMES);
END_DATA_DESC(ModelParticleDesc)


//---------- Создание/удаление --------------------
DataSource::DataSource (IParticleManager* Master) : Emitters(_FL_)
{
}

DataSource::~DataSource ()
{
	Destroy ();
}
 
bool DataSource::Release ()
{
	delete this;
	return true;
}


// ========================= Load & Save =======================================
	//Сохранить/восстановить из файла
void DataSource::Write (MemFile* pMemFile)
{
	pMemFile->Write(HEADER, 4);
	pMemFile->Write(VERSION, 4);

	DWORD dwEmittersCount = Emitters.Size();
	pMemFile->WriteType(dwEmittersCount);

	for (DWORD n = 0; n < dwEmittersCount; n++)
	{
		pMemFile->WriteType(Emitters[n].Type);
		Emitters[n].Fields.Write(pMemFile);
		DWORD dwParticlesSize = Emitters[n].Particles.Size();
		pMemFile->WriteType(dwParticlesSize);
		for (DWORD i = 0; i < dwParticlesSize; i++)
		{
			pMemFile->WriteType(Emitters[n].Particles[i].Type);
			Emitters[n].Particles[i].Fields.Write(pMemFile);
		}
	}
}

void DataSource::Load (MemFile* pMemFile)
{
	//Проверяем ID
	char Id[5];
	Id[4] = 0;
	pMemFile->Read(Id, 4);
	if (strcmp (Id, HEADER) != 0)
	{
		api->Trace ("Particles: Incorrect file type");
		return;
	}

	//Проверяем Версию
	char Ver[5];
	Ver[4] = 0;
	pMemFile->Read(Ver, 4);

/* // show warnings
	if (strcmp (Ver, VERSION) != 0)	
          api->Trace ("Particles: Warning !!! Incorrect file version %s, must be %s", Ver, VERSION);
*/          

	//Кол-во эмиттеров...
	DWORD EmiterCount = 0;
	pMemFile->ReadType(EmiterCount);

	for (DWORD n = 0; n < EmiterCount; n++)
	{
		EmitterType emType = UNKNOWN_EMITTER;
		pMemFile->ReadType(emType);

		switch (emType)
		{
			case POINT_EMITTER:
			{
//				api->Trace ("Particles info: Point emitter");
				CreatePointEmitter (pMemFile);
				break;
			}

			default:
			{
				_THROW ("Particles: Unknown emitter type !");
				return;
			}
		} // switch
	} // for all saved emitters...
}


void DataSource::CreatePointEmitter (MemFile* pMemFile)
{
	//api->Trace ("Particles info: Point emitter");
	EmitterDesc* PointEmitter = &Emitters[Emitters.Add()];
	PointEmitter->Fields.Load(pMemFile);

	PointEmitter->Fields.Convert (&PointEmitterDesc);
	PointEmitter->Type = POINT_EMITTER;

	DWORD ParticlesCount = 0;
	pMemFile->ReadType(ParticlesCount);

	for (DWORD n = 0; n < ParticlesCount; n++)
	{
		ParticleType ptType = UNKNOWN_PARTICLE;
		pMemFile->ReadType(ptType);

		switch(ptType) 
		{
			case BILLBOARD_PARTICLE:
			{	
				//api->Trace ("Particles info: Billboard particle");
				CreateBillBoardParticle (PointEmitter->Particles, pMemFile);
				break;
			}
			case MODEL_PARTICLE:
			{	
				//api->Trace ("Particles info: Model particle");
				CreateModelParticle (PointEmitter->Particles, pMemFile);
				break;
			}
			default:
			{
				_THROW ("Particles: Unknown particle type !!!!");
			}
		} // SWITCH
	} // For all particles
}

//Создает BillBoard парикл
void DataSource::CreateBillBoardParticle (array<ParticleDesc> &Particles, MemFile* pMemFile)
{
	ParticleDesc *pDesc = &Particles[Particles.Add()];
	pDesc->Type = BILLBOARD_PARTICLE;
	pDesc->Fields.Load(pMemFile);
	pDesc->Fields.Convert(&BillboardParticleDesc);
}

//Создает Model парикл
void DataSource::CreateModelParticle (array<ParticleDesc> &Particles, MemFile* pMemFile)
{
	ParticleDesc *pDesc = &Particles[Particles.Add()];
	pDesc->Type = MODEL_PARTICLE;
	pDesc->Fields.Load(pMemFile);
	pDesc->Fields.Convert(&ModelParticleDesc);
}


void DataSource::Destroy ()
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].Fields.DelAll();
		for (DWORD i = 0; i < Emitters[n].Particles.Size(); i++)
		{
			Emitters[n].Particles[i].Fields.DelAll();
		}
	}

	Emitters.DelAll();


}


int DataSource::GetEmitterCount ()
{
	return Emitters.Size();
}

DataSource::EmitterDesc* DataSource::GetEmitterDesc (int Index)
{
	return &Emitters[Index];
}

FieldList* DataSource::CreateEmptyPointEmitter (const char* EmitterName)
{
	EmitterDesc* PointEmitter = &Emitters[Emitters.Add()];
	PointEmitter->Fields.Convert (&PointEmitterDesc);
	PointEmitter->Type = POINT_EMITTER;

	DataString* pEmitterName = PointEmitter->Fields.FindString(EMITTER_NAME);

	if (pEmitterName)
	{
		pEmitterName->SetValue(EmitterName);
	}

	DataFloat* pEmitterLifeTime = PointEmitter->Fields.FindFloat(EMITTER_LIFETIME);

	if (pEmitterLifeTime)
	{
		pEmitterLifeTime->SetValue(1.0f);
	}

	DataGraph* pAngleX = PointEmitter->Fields.FindGraph(EMISSION_DIR_X);
	if (pAngleX) pAngleX->SetNegative(true);
	
	DataGraph* pAngleY = PointEmitter->Fields.FindGraph(EMISSION_DIR_Y);
	if (pAngleY) pAngleY->SetNegative(true);
	
	DataGraph* pAngleZ = PointEmitter->Fields.FindGraph(EMISSION_DIR_Z);
	if (pAngleZ) pAngleZ->SetNegative(true);

	return &PointEmitter->Fields;
}

int DataSource::FindEmitter (const char* Name)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		DataString* pString = Emitters[n].Fields.FindString(EMITTER_NAME);
		if (pString)
		{
			if (stricmp (pString->GetValue(), Name) == 0)
			{
				return n;
			}
		}
	}

	return -1;
}

FieldList* DataSource::CreateBillBoardParticle (const char* ParticleName, const char* EmitterName)
{
	int EmitterIndex = FindEmitter(EmitterName);
	if (EmitterIndex == -1) return NULL;

	ParticleDesc *pDesc = &Emitters[EmitterIndex].Particles[Emitters[EmitterIndex].Particles.Add()];
	pDesc->Type = BILLBOARD_PARTICLE;
	pDesc->Fields.Convert(&BillboardParticleDesc);

	DataString* pParticleName = pDesc->Fields.FindString(PARTICLE_NAME);
	if (pParticleName) pParticleName->SetValue(ParticleName);

	DataString* pAttachedEmitter = pDesc->Fields.FindString(ATTACHEDEMITTER_NAME);
	if (pAttachedEmitter) pAttachedEmitter->SetValue("none");

	DataGraph* pSize = pDesc->Fields.FindGraph(PARTICLE_SIZE);
	if (pSize) pSize->SetDefaultValue(3.0f, 2.0f);

	DataGraph* pEmissionRate = pDesc->Fields.FindGraph(PARTICLE_EMISSION_RATE);
	if (pEmissionRate) pEmissionRate->SetDefaultValue(10.0f, 10.0f);

	DataGraph* pLifeTime = pDesc->Fields.FindGraph(PARTICLE_LIFE_TIME);
	if (pLifeTime) pLifeTime->SetDefaultValue(10.0f, 10.0f);


	DataFloat* pMaxCount = pDesc->Fields.FindFloat(PARTICLE_MAX_COUNT);
	if (pMaxCount) pMaxCount->SetValue(100);

	DataColor* pColorG = pDesc->Fields.FindColor(PARTICLE_COLOR);
	if (pColorG) pColorG->SetDefaultValue(Color(1.0f, 1.0f, 1.0f, 1.0f));

	DataGraph* pGravityK = pDesc->Fields.FindGraph(PARTICLE_GRAVITATION_K);
	if (pGravityK) pGravityK->SetDefaultValue(100.0f, 100.0f);


	DataGraph* pTrackX = pDesc->Fields.FindGraph(PARTICLE_TRACK_X);
	if (pTrackX) pTrackX->SetNegative(true);

	DataGraph* pTrackY = pDesc->Fields.FindGraph(PARTICLE_TRACK_Y);
	if (pTrackY) pTrackY->SetNegative(true);

	DataGraph* pTrackZ = pDesc->Fields.FindGraph(PARTICLE_TRACK_Z);
	if (pTrackZ) pTrackZ->SetNegative(true);

	DataGraph* pVelocity = pDesc->Fields.FindGraph(PARTICLE_VELOCITY_POWER);
	if (pVelocity) pVelocity->SetNegative(true);

	DataGraph* pMass = pDesc->Fields.FindGraph(PARTICLE_MASS);
	if (pMass) pMass->SetNegative(true);

	DataGraph* pSpin = pDesc->Fields.FindGraph(PARTICLE_SPIN);
	if (pSpin) pSpin->SetNegative(true);

	return &pDesc->Fields;
}

FieldList* DataSource::CreateModelParticle (const char* ParticleName, const char* EmitterName)
{
	int EmitterIndex = FindEmitter(EmitterName);
	if (EmitterIndex == -1) return NULL;

	ParticleDesc *pDesc = &Emitters[EmitterIndex].Particles[Emitters[EmitterIndex].Particles.Add()];
	pDesc->Type = MODEL_PARTICLE;
	pDesc->Fields.Convert(&ModelParticleDesc);

	DataString* pParticleName = pDesc->Fields.FindString(PARTICLE_NAME);
	if (pParticleName) pParticleName->SetValue(ParticleName);

	DataString* pAttachedEmitter = pDesc->Fields.FindString(ATTACHEDEMITTER_NAME);
	if (pAttachedEmitter) pAttachedEmitter->SetValue("none");

	DataFloat* pMaxCount = pDesc->Fields.FindFloat(PARTICLE_MAX_COUNT);
	if (pMaxCount) pMaxCount->SetValue(100);

	DataGraph* pEmissionRate = pDesc->Fields.FindGraph(PARTICLE_EMISSION_RATE);
	if (pEmissionRate) pEmissionRate->SetDefaultValue(10.0f, 10.0f);

	DataGraph* pLifeTime = pDesc->Fields.FindGraph(PARTICLE_LIFE_TIME);
	if (pLifeTime) pLifeTime->SetDefaultValue(10.0f, 10.0f);

	DataGraph* pGravityK = pDesc->Fields.FindGraph(PARTICLE_GRAVITATION_K);
	if (pGravityK) pGravityK->SetDefaultValue(100.0f, 100.0f);

	DataGraph* pTrackX = pDesc->Fields.FindGraph(PARTICLE_TRACK_X);
	if (pTrackX) pTrackX->SetNegative(true);

	DataGraph* pTrackY = pDesc->Fields.FindGraph(PARTICLE_TRACK_Y);
	if (pTrackY) pTrackY->SetNegative(true);

	DataGraph* pTrackZ = pDesc->Fields.FindGraph(PARTICLE_TRACK_Z);
	if (pTrackZ) pTrackZ->SetNegative(true);

	DataGraph* pVelocity = pDesc->Fields.FindGraph(PARTICLE_VELOCITY_POWER);
	if (pVelocity) pVelocity->SetNegative(true);

	DataGraph* pMass = pDesc->Fields.FindGraph(PARTICLE_MASS);
	if (pMass) pMass->SetNegative(true);


	DataGraph* pSpinX = pDesc->Fields.FindGraph(PARTICLE_SPIN_X);
	if (pSpinX) pSpinX->SetNegative(true);

	DataGraph* pSpinY = pDesc->Fields.FindGraph(PARTICLE_SPIN_Y);
	if (pSpinY) pSpinY->SetNegative(true);

	DataGraph* pSpinZ = pDesc->Fields.FindGraph(PARTICLE_SPIN_Z);
	if (pSpinZ) pSpinZ->SetNegative(true);

	return &pDesc->Fields;
}


void DataSource::DeletePointEmitter (FieldList* pEmitter)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (&Emitters[n].Fields == pEmitter)
		{
			Emitters[n].Fields.DelAll();
			Emitters.ExtractNoShift(n);
			break;
		}
	}
}

void DataSource::DeleteBillboard (FieldList* pEmitter, FieldList* pParticles)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (&Emitters[n].Fields == pEmitter)
		{
			for (DWORD i = 0; i < Emitters[n].Particles.Size(); i++)
			{
				if (&Emitters[n].Particles[i].Fields == pParticles)
				{
					Emitters[n].Particles[i].Fields.DelAll();
					Emitters[n].Particles.Extract(i);
				}
			}
		}
	}
}

void DataSource::DeleteModel (FieldList* pEmitter, FieldList* pParticles)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (&Emitters[n].Fields == pEmitter)
		{
			for (DWORD i = 0; i < Emitters[n].Particles.Size(); i++)
			{
				if (&Emitters[n].Particles[i].Fields == pParticles)
				{
					Emitters[n].Particles[i].Fields.DelAll();
					Emitters[n].Particles.Extract(i);
				}
			}
		}
	}
}
