#include "base.h"
#include "..\DataSource\databool.h"
#include "..\DataSource\datacolor.h"
#include "..\DataSource\datafloat.h"
#include "..\DataSource\datagraph.h"
#include "..\DataSource\dataposition.h"
#include "..\DataSource\datauv.h"
#include "..\DataSource\datastring.h"
#include "..\..\..\common_h\vmodule_api.h"
#include "..\..\icommon\names.h"


#define INTERPOLATION_STEPS 32.0f




// Конструктор / деструктор
BaseEmitter::BaseEmitter(ParticleSystem* pSystem) : ParticleTypes(_FL_)
{
	pEmitter = NULL;
	Visible = true;
	pFields = NULL;
	IsAttachedFlag = false;
	OldMatrixNotInitialized = true;
	LifeTime = 0.0f;
	pMaster = pSystem;
	ElapsedTime = 0.0f;
	Position = Vector (0.0f);
	EmissionDirX = NULL;
	EmissionDirY = NULL;
	EmissionDirZ = NULL;
	Stoped = false;
}

BaseEmitter::~BaseEmitter()
{
	GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
	GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
	Stoped = true;
}

//Родить новые партиклы 
void BaseEmitter::BornParticles (float DeltaTime)
{
	float SavedTime = ElapsedTime;
	if (!Visible) return;
	Matrix matTransform;

	float MatrixBlend = 0.0f;
	float MatrixBlendInc = 1.0f / INTERPOLATION_STEPS;

	for (int i = 0; i < (int)INTERPOLATION_STEPS; i++)
	{
		BlendMatrix(matWorldTransform, matWorldTransformOld, matWorldTransformNew, MatrixBlend);

		Vector TransformPos = Position * matWorldTransform;
		matWorldTransform.pos = TransformPos;
		MatrixBlend += MatrixBlendInc;

		float DeltaTimeDiv = DeltaTime / INTERPOLATION_STEPS;
		IncreaseTime (DeltaTimeDiv);

		// Если запаузился эмиттер досрочный выход
		if (Stoped)
		{
			ElapsedTime = SavedTime;
			return;
		}


		for (DWORD n = 0; n < ParticleTypes.Size(); n++)
		{
			if (!ParticleTypes[n].Visible) continue;


			float EmissionRate = ParticleTypes[n].EmissionRate->GetRandomValue(ElapsedTime, LifeTime);
			EmissionRate *= DeltaTimeDiv;

			// How many particles remain unemissed from last frame
			float ParticlesRemain = ParticleTypes[n].Remain;	

			ParticlesRemain += EmissionRate;
			ParticleTypes[n].Remain = ParticlesRemain;

			while (ParticleTypes[n].Remain >= 1.0f)
			{
				ParticleTypes[n].Remain -= 1.0f;
				if (ParticleTypes[n].ActiveCount < ParticleTypes[n].MaxParticlesCount)
				{
					Vector ParticlePos = GetNewParticlePosition (DeltaTime);
					GetEmissionDirection (matTransform);
					Vector VelDir = matTransform.vy;
					switch(ParticleTypes[n].Type)
					{
					case BILLBOARD_PARTICLE:
						ParticleTypes[n].ActiveCount++;
						GetManager()->GetBBProcessor()->AddParticle(pMaster, VelDir, ParticlePos, matWorldTransform, ElapsedTime, LifeTime, ParticleTypes[n].pFields, &ParticleTypes[n].ActiveCount, (Unique_GUID+n));
						break;
					case MODEL_PARTICLE:
						ParticleTypes[n].ActiveCount++;
						GetManager()->GetMDLProcessor()->AddParticle(pMaster, VelDir, ParticlePos, matWorldTransform, ElapsedTime, LifeTime, ParticleTypes[n].pFields, &ParticleTypes[n].ActiveCount, (Unique_GUID+n));
						//ParticleTypes[n].ActiveCount++;
						//GetMaster()->GetMaster()->GetMDLProcessor()->AddParticle(ParticleTypes[n].pFields);
						break;
					default:
						_THROW("Try to emmit unknown particle type");
					} // switch
				} // Active < Max
			} // While Remain
		} // For all types
	}	 // INTERPOLATION_STEPS iterations

	matWorldTransformOld = matWorldTransformNew;

	ElapsedTime = SavedTime;
}
  

//Исполнить
void BaseEmitter::Execute (float DeltaTime)
{
	
	if (!Stoped && !IsAttachedFlag) BornParticles (DeltaTime);

	IncreaseTime (DeltaTime);

/*
	for (DWORD n = 0; n < ParticleTypes.Size(); n++)
	{
		switch (ParticleTypes[n].Type)
		{
			case BILLBOARD_PARTICLE:
				break;
			case MODEL_PARTICLE:
				break;
		}
	}
*/
}

//Присоединиться к источнику данных
void BaseEmitter::AttachToDataSource (DataSource::EmitterDesc* pEmitter)
{
	this->pEmitter = pEmitter;
	pFields = &pEmitter->Fields;
	Type = pEmitter->Type;

	Editor_UpdateCachedData ();

	EmissionDirX = pEmitter->Fields.FindGraph(EMISSION_DIR_X);
	EmissionDirY = pEmitter->Fields.FindGraph(EMISSION_DIR_Y);
	EmissionDirZ = pEmitter->Fields.FindGraph(EMISSION_DIR_Z);


	for (DWORD n = 0; n < pEmitter->Particles.Size(); n++)
	{
		DataSource::ParticleDesc* pDesc = &pEmitter->Particles[n];
		switch (pDesc->Type)
		{
			case BILLBOARD_PARTICLE:
				CreateBillBoardParticle (pDesc->Fields);
				break;
			case MODEL_PARTICLE:
				CreateModelParticle (pDesc->Fields);
				break;
			default:
				_THROW ("Particles: Unknown particle type !!!!");
		}

	}
}

void BaseEmitter::IncreaseTime (float DeltaTime)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime > LifeTime)
	{
		if (!Looped) Stoped = true;
		ElapsedTime -= LifeTime;
	}
}

void BaseEmitter::CreateBillBoardParticle (FieldList &Fields)
{
//	api->Trace("Create BB Particle\n");

	structParticleType* NewBillBoard = &ParticleTypes[ParticleTypes.Add()];
	NewBillBoard->Type = BILLBOARD_PARTICLE;
	NewBillBoard->EmissionRate = Fields.FindGraph(EMISSION_RATE);
	NewBillBoard->MaxParticlesCount = Fields.GetFloatAsInt(MAX_PARTICLES_COUNT);
	NewBillBoard->ActiveCount = 0;
	NewBillBoard->Remain = 0.0f;
	NewBillBoard->pFields = &Fields;
	NewBillBoard->Visible = true;

//	api->Trace("%d", NewBillBoard->MaxParticlesCount);
	
}

void BaseEmitter::CreateModelParticle (FieldList &Fields)
{
//	api->Trace("Create MODEL Particle\n");

	structParticleType* NewModel = &ParticleTypes[ParticleTypes.Add()];
	NewModel->Type = MODEL_PARTICLE;
	NewModel->EmissionRate = Fields.FindGraph(PARTICLE_EMISSION_RATE);
	NewModel->MaxParticlesCount = Fields.GetFloatAsInt(PARTICLE_MAX_COUNT);
	NewModel->ActiveCount = 0;
	NewModel->Remain = 0.0f;
	NewModel->pFields = &Fields;
	NewModel->Visible = true;
}

ParticleSystem* BaseEmitter::GetMaster ()
{
	return pMaster;
}

ParticleManager* BaseEmitter::GetManager ()
{
	return pMaster->GetMaster();
}

void BaseEmitter::GetEmissionDirection (Matrix &matWorld)
{
	Vector DirAngles;
	DirAngles.x = EmissionDirX->GetRandomValue(ElapsedTime, LifeTime);
	DirAngles.y = EmissionDirY->GetRandomValue(ElapsedTime, LifeTime);
	DirAngles.z = EmissionDirZ->GetRandomValue(ElapsedTime, LifeTime);

	DirAngles *= MUL_DEGTORAD;

	
	matWorld = Matrix(DirAngles, Vector(0.0f));
}

void BaseEmitter::Restart ()
{
	//Удаляем свои партиклы...
	GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
	GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
	Stoped = false;
	ElapsedTime = 0.0f;
}

DWORD BaseEmitter::GetParticleCount ()
{
	DWORD Count = 0;
	for (DWORD n = 0; n < ParticleTypes.Size(); n++)
	{
		Count += ParticleTypes[n].ActiveCount;
	}

	return Count;
}

bool BaseEmitter::IsStoped ()
{
	return Stoped;
}

void BaseEmitter::SetTransform (const Matrix& matWorld)
{
	if (OldMatrixNotInitialized)
	{
		matWorldTransformOld = matWorld;
		OldMatrixNotInitialized = false;
	} 

	matWorldTransformNew = matWorld;
	//matWorldTransform = matWorld;
}

void BaseEmitter::Teleport (const Matrix &matWorld)
{
	matWorldTransformOld = matWorld;
	matWorldTransformNew = matWorld;
}


void BaseEmitter::BlendMatrix (Matrix& result, const Matrix& mat1, const Matrix& mat2, float BlendK)
{
	Quaternion qRot1(mat1);
	Quaternion qRot2(mat2);
	Vector vPos1 = mat1.pos;
	Vector vPos2 = mat2.pos;

	Quaternion qBlend;
	qBlend.SLerp(qRot1, qRot2, BlendK);

	Vector vBlend;
	vBlend.Lerp(vPos1, vPos2, BlendK);

	qBlend.GetMatrix(result);
	result.pos = vBlend;
}

const char* BaseEmitter::GetName ()
{
	return Name.GetBuffer();
}

void BaseEmitter::SetAttachedFlag (bool Flag)
{
	IsAttachedFlag = Flag;
}

bool BaseEmitter::IsAttached ()
{
	return IsAttachedFlag;
}

float BaseEmitter::GetTime ()
{
	return ElapsedTime;
}

void BaseEmitter::SetTime (float Time)
{
	ElapsedTime = Time;
}

DWORD BaseEmitter::GetParticleTypesCount ()
{
	return ParticleTypes.Size();
}

FieldList* BaseEmitter::GetParticleTypeDataByIndex (DWORD Index)
{
	return ParticleTypes[Index].pFields;
}

ParticleType BaseEmitter::GetParticleTypeByIndex  (DWORD Index)
{
	return ParticleTypes[Index].Type;
}

FieldList* BaseEmitter::GetData ()
{
	return pFields;
}

bool BaseEmitter::SetEnable (bool bVisible)
{
	Visible = bVisible;

	if (Visible == false)
	{
		GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
		GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
	}

	for (DWORD n = 0; n < ParticleTypes.Size(); n++)
							ParticleTypes[n].Visible = bVisible;


	return Visible;
}

bool BaseEmitter::GetEnable ()
{
	return Visible;
}

int BaseEmitter::GetParticleTypeIndex (FieldList* pFields)
{
	for (DWORD n = 0; n < ParticleTypes.Size(); n++)
	{
		if (ParticleTypes[n].pFields == pFields) return n;
	}
	return -1;
}

bool BaseEmitter::SetParticleTypeEnable (bool bVisible, DWORD Index)
{
	ParticleTypes[Index].Visible = bVisible;

	if (bVisible == false)
	{
		GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID+Index, 1);
		GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID+Index, 1);
	}

	return ParticleTypes[Index].Visible;
}

bool BaseEmitter::GetParticleTypeEnable (DWORD Index)
{
	return ParticleTypes[Index].Visible;
}

void BaseEmitter::Editor_UpdateCachedData ()
{
	Name = pEmitter->Fields.GetString(EMITTER_NAME, "NoName");
	LifeTime = pEmitter->Fields.GetFloat(EMITTER_LIFETIME);
	Assert (LifeTime > 0);
	Position = pEmitter->Fields.GetPosition(EMITTER_POSITION);
	Looped = pEmitter->Fields.GetBool(EMITTER_LOOPING, false);

	for (DWORD n = 0; n < ParticleTypes.Size(); n++)
	{
		ParticleTypes[n].MaxParticlesCount = ParticleTypes[n].pFields->GetFloatAsInt(MAX_PARTICLES_COUNT);
	}
	

}

void BaseEmitter::SetName (const char* Name)
{
	DataString* EmitterName = pEmitter->Fields.FindString(EMITTER_NAME);
	Assert (EmitterName);
	EmitterName->SetValue(Name);
	Editor_UpdateCachedData ();
}

void BaseEmitter::Stop ()
{
	Stoped = true;
}