#include "particlesystem.h"
#include "..\emitters\point.h"



//Глобальный GUID для присваивания эмиттерам
DWORD EmitterID = 0;





//Создание/удаление 
ParticleSystem::ParticleSystem(ParticleManager* serv) : Emitters(_FL_)
{
	AutoDeleted = false;
	pMaster = serv;
	EmissionPause = false;
}

ParticleSystem::~ParticleSystem()
{
	pMaster->RemoveResource(this);
	DeleteAllEmitters ();
}


bool ParticleSystem::Release ()
{
	delete this;
	return false;
}

//Отработать всем партиклам
DWORD ParticleSystem::Execute (float DeltaTime)
{
	if (AutoDeleted)
	{
		if (DeleteIfNeed ())
		{
			//api->Trace("AUTO DELETE !!!!\n");
			return 0;
		}
	}

	if (EmissionPause) return Emitters.Size();

	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->Execute(DeltaTime);
	}

	return Emitters.Size();
}


void ParticleSystem::CreateFromDataSource (DataSource* pDataSource)
{
	int EmitterCount = pDataSource->GetEmitterCount();
	if (EmitterCount <= 0) return;

	for (int n = 0; n < EmitterCount; n++)
	{
		DataSource::EmitterDesc* pEmitterDecription = pDataSource->GetEmitterDesc(n);
		IEmitter* pEmitter = NULL;
		switch (pEmitterDecription->Type)
		{
			case POINT_EMITTER:
				pEmitter = CreatePointEmitter (pEmitterDecription);
				break;
			default:
				_THROW ("Particles: Unknown emitter type !!!!");
		}

		//Assert (pEmitter);

		//CreateParticles (pEmitter, pEmitterDecription->Particles);
	}
}

IEmitter* ParticleSystem::CreatePointEmitter (DataSource::EmitterDesc* pEmitter)
{

	
//	api->Trace("Create 'Point' emitter\n");
	PointEmitter* pNewEmitter = NEW PointEmitter(this);
	pNewEmitter->AttachToDataSource(pEmitter);
	pNewEmitter->SetGUID (EmitterID);

	EmitterDesc NewEmitter;
	NewEmitter.Type = POINT_EMITTER;
	NewEmitter.pEmitter = pNewEmitter;
	Emitters.Add(NewEmitter);

	EmitterID+=GUIDSTEP;

	return pNewEmitter;
}


void ParticleSystem::DeleteAllEmitters ()
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (Emitters[n].pEmitter)
		{
			delete Emitters[n].pEmitter;
		}
	}

	Emitters.DelAll();
}

ParticleManager* ParticleSystem::GetMaster ()
{
	return pMaster;
}



//Перезапустить партикловую систему  
void ParticleSystem::Restart (DWORD RandomSeed)
{
	srand (RandomSeed);
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->Restart();
	}

	PauseEmission (false);
}

//Запаузить испускание партиклов  
void ParticleSystem::PauseEmission (bool bPause)
{
	EmissionPause = bPause;
}

//Узнать на паузе эмиссия или нет  
bool ParticleSystem::IsEmissionPaused ()
{
	return EmissionPause;
}


bool ParticleSystem::DeleteIfNeed ()
{
	if (!IsAlive())
	{
		pMaster->DefferedDelete(this);
		return true;
	}
	return false;
}


//Установить автоудаляемая система или обычная...  
void ParticleSystem::AutoDelete (bool Enabled)
{
	AutoDeleted = Enabled;
}

//Узнаять автоудаляемая система или нет  
bool ParticleSystem::IsAutoDeleted ()
{
	return AutoDeleted;
}

//Установить матрицу трансформации для системы 
void ParticleSystem::SetTransform (const Matrix& transform)
{
	matWorld = transform;
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->SetTransform (transform);
	}
}

void ParticleSystem::Teleport (const Matrix &transform)
{
	matWorld = transform;
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->Teleport (transform);
	}
}

void ParticleSystem::GetTransform (Matrix& _matWorld)
{
	_matWorld = matWorld;
}


IEmitter* ParticleSystem::FindEmitterByData (FieldList* Data)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (Emitters[n].pEmitter->GetData() == Data) return Emitters[n].pEmitter;
	}

	return NULL;
}

IEmitter* ParticleSystem::FindEmitter (const char* name)
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		if (stricmp (Emitters[n].pEmitter->GetName(), name) == 0) return Emitters[n].pEmitter;
	}

	return NULL;
}


DWORD ParticleSystem::GetEmittersCount ()
{
	return Emitters.Size();
}

IEmitter* ParticleSystem::GetEmitterByIndex (DWORD Index)
{
	return Emitters[Index].pEmitter;
}

EmitterType ParticleSystem::GetEmitterTypeByIndex  (DWORD Index)
{
	return Emitters[Index].Type;
}

bool ParticleSystem::IsAlive ()
{
	DWORD pCount = 0;
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		pCount += Emitters[n].pEmitter->GetParticleCount();
		if (!Emitters[n].pEmitter->IsStoped())	return true;
	}

	if (pCount > 0) return true;

	return false;
}

const char* ParticleSystem::GetName ()
{
	return SystemName.GetBuffer();
}

void ParticleSystem::SetName (const char* Name)
{
	SystemName = Name;
}




void ParticleSystem::Editor_UpdateCachedData ()
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->Editor_UpdateCachedData ();
	}
}


void ParticleSystem::Stop ()
{
	for (DWORD n = 0; n < Emitters.Size(); n++)
	{
		Emitters[n].pEmitter->Stop ();
	}
}