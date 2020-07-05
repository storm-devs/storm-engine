#include "mdl_processor.h"
#include "..\..\icommon\names.h"
#include "..\..\icommon\IEmitter.h"
#include "..\datasource\datagraph.h"
#include "..\datasource\datacolor.h"
#include "..\datasource\datauv.h"
#include "..\..\..\common_h\dx8render.h"
#include "..\..\..\common_h\exs.h"
#include "..\..\..\common_h\geos.h"
#include "..\particlesystem\particlesystem.h"
#include "..\..\manager\particlemanager.h"
#include "physic.h"



//Сколько всего может быть моделей
#define MAX_MODELS 8192


ModelProcessor::ModelProcessor (ParticleManager* pManager) : Particles(_FL_, MAX_MODELS)
{
	pMasterManager = pManager;
	pMemArray = NEW MemArrayItem[MAX_MODELS];

	for (DWORD n = 0; n < MAX_MODELS; n++)
	{
		pMemArray[n].Free = true;
	}

	pRS = (VDX8RENDER*)api->CreateService("DX8Render");
	Assert (pRS);

}
 
ModelProcessor::~ModelProcessor ()
{
	delete pMemArray;
} 

MDL_ParticleData* ModelProcessor::AllocParticle ()
{
	for (DWORD n = 0; n < MAX_MODELS; n++)
	{
		if (pMemArray[n].Free)
		{
			pMemArray[n].Free = false;
			return &pMemArray[n].pData;
		}
	}

	return NULL;
}

void ModelProcessor::FreeParticle (MDL_ParticleData* pItem)
{
	for (DWORD n = 0; n < MAX_MODELS; n++)
	{
		if	(&pMemArray[n].pData == pItem)
		{
			pMemArray[n].Free = true;
			return;
		}
	}
}


void ModelProcessor::AddParticle (ParticleSystem* pSystem, const Vector& velocity_dir, const Vector& pos, const Matrix& matWorld, float EmitterTime, float EmitterLifeTime, FieldList* pFields, DWORD* pActiveCount, DWORD dwGUID)
{
	MDL_ParticleData* pData = AllocParticle ();

	//Сработает если партиклов будет > MAX_BILLBOARDS, столько их быть не должно :))))
	if (!pData)
	{
		*(pActiveCount) = (*(pActiveCount)-1);
		return;
	}

	const char* GeomNames = pFields->GetString(PARTICLE_GEOM_NAMES);
	const char* pGeomName = Parser.GetRandomName(GeomNames);
	pData->pScene = pMasterManager->GetModel(pGeomName);


	if (!pData->pScene)
	{
		//api->Trace("Cant create particle. Reason geometry '%s', '%s' not found !!!", GeomNames, pGeomName);
		*(pActiveCount) = (*(pActiveCount)-1);
		FreeParticle (pData);
		return;
	}

	pData->Graph_TrackX = pFields->FindGraph(PARTICLE_TRACK_X);
	pData->Graph_TrackY = pFields->FindGraph(PARTICLE_TRACK_Y);
	pData->Graph_TrackZ = pFields->FindGraph(PARTICLE_TRACK_Z);

	Vector PositionOffset;
	PositionOffset.x = pData->Graph_TrackX->GetRandomValue(0.0f, 100.0f);
	PositionOffset.y = pData->Graph_TrackY->GetRandomValue(0.0f, 100.0f);
	PositionOffset.z = pData->Graph_TrackZ->GetRandomValue(0.0f, 100.0f);


	pData->EmitterGUID = dwGUID;
	pData->ActiveCount = pActiveCount;
	pData->RenderPos = (pos+PositionOffset) * matWorld;
	pData->Velocity = matWorld.MulNormal(velocity_dir);
	pData->ElapsedTime = 0.0f;
	pData->matWorld = matWorld;



	pData->Angle = Vector (0.0f);
	pData->RenderAngle = Vector (0.0f);
	pData->ExternalForce = Vector(0.0f, 0.0f, 0.0f);
	pData->PhysPos = pData->RenderPos;

	pData->OldRenderPos = pData->RenderPos;
	pData->OldRenderAngle = pData->RenderAngle;

	pData->LifeTime = pFields->GetRandomGraphVal(PARTICLE_LIFE_TIME, EmitterTime, EmitterLifeTime);
	pData->Mass = pFields->GetRandomGraphVal(PARTICLE_MASS, EmitterTime, EmitterLifeTime);
	pData->Spin.x = pFields->GetRandomGraphVal(PARTICLE_SPIN_X, EmitterTime, EmitterLifeTime);
	pData->Spin.y = pFields->GetRandomGraphVal(PARTICLE_SPIN_Y, EmitterTime, EmitterLifeTime);
	pData->Spin.z = pFields->GetRandomGraphVal(PARTICLE_SPIN_Z, EmitterTime, EmitterLifeTime);
	pData->Spin = pData->Spin * MUL_DEGTORAD;
	//api->Trace("spin %3.2f, %3.2f, %3.2f [%3.2f, %3.2f]", pData->Spin.x, pData->Spin.y, pData->Spin.z, EmitterTime, EmitterLifeTime);

	float VelocityPower = pFields->GetRandomGraphVal(PARTICLE_VELOCITY_POWER, EmitterTime, EmitterLifeTime);
	pData->Velocity = pData->Velocity * VelocityPower;
	pData->UMass = fabsf (pData->Mass);


	pData->Graph_SpinDragX = pFields->FindGraph(PARTICLE_SPIN_DRAGX);
	pData->Graph_SpinDragY = pFields->FindGraph(PARTICLE_SPIN_DRAGY);
	pData->Graph_SpinDragZ = pFields->FindGraph(PARTICLE_SPIN_DRAGZ);
	pData->Graph_Drag = pFields->FindGraph(PARTICLE_DRAG);
	pData->Graph_PhysBlend = pFields->FindGraph(PARTICLE_PHYSIC_BLEND);
	pData->graph_GravK = pFields->FindGraph(PARTICLE_GRAVITATION_K);




	pData->DragK = FRAND (1.0f);
	pData->SpinDragK_X = FRAND (1.0f);
	pData->SpinDragK_Y = FRAND (1.0f);
	pData->SpinDragK_Z = FRAND (1.0f);
	pData->GravKK = FRAND (1.0f);

	pData->KPhysBlend = FRAND (1.0f);
	pData->KTrackX = FRAND (1.0f);
	pData->KTrackY = FRAND (1.0f);
	pData->KTrackZ = FRAND (1.0f);




	const char* pEmitterName = pFields->GetString(ATTACHEDEMITTER_NAME);
	if (stricmp (pEmitterName, "none") == 0)
	{
		pData->AttachedEmitter = NULL;
	} else
	{
		pData->AttachedEmitter = pSystem->FindEmitter(pEmitterName);
		if (pData->AttachedEmitter) pData->AttachedEmitter->SetAttachedFlag(true);
	}

	Particles.Add(pData);
}


//Считает физику, треки  и т.д.
void ModelProcessor::Process (float DeltaTime)
{
	//DWORD t;
	//RDTSC_B (t);

	for (DWORD n = 0; n < Particles.Size(); n++)
	{
		Particles[n]->ElapsedTime += DeltaTime;

		float Time = Particles[n]->ElapsedTime;
		float LifeTime = Particles[n]->LifeTime;

//		_mm_prefetch ((const char *)Particles[n+1], _MM_HINT_T0);

		//Сразу убиваем дохлые...
		if (Time > LifeTime)
		{
			*(Particles[n]->ActiveCount) = (*(Particles[n]->ActiveCount)-1);
			FreeParticle (Particles[n]);
			Particles.ExtractNoShift(n);
			n--;
			continue;
		}

		float Drag = Particles[n]->Graph_Drag->GetValue(Time, LifeTime, Particles[n]->DragK);
		Drag = 1.0f -(Drag * 0.01f);
		if (Drag < 0.0f) Drag = 0.0f;
		if (Drag > 1.0f) Drag = 1.0f;

		float GravK = Particles[n]->graph_GravK->GetValue(Time, LifeTime, Particles[n]->GravKK);

		AddGravityForce (Particles[n]->ExternalForce, Particles[n]->Mass, GravK);
		SolvePhysic (Particles[n]->PhysPos, Particles[n]->Velocity, Particles[n]->ExternalForce, Particles[n]->UMass, Drag, DeltaTime);
		Particles[n]->ExternalForce = Vector(0.0f);

		//FIX ME !!!
		Vector SpinDrag;
		SpinDrag.x = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_X);
		SpinDrag.x = 1.0f - (SpinDrag.x * 0.01f);
		if (SpinDrag.x < 0.0f) SpinDrag.x = 0.0f;
		if (SpinDrag.x > 1.0f) SpinDrag.x = 1.0f;

		SpinDrag.y = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_Y);
		SpinDrag.y = 1.0f - (SpinDrag.y * 0.01f);
		if (SpinDrag.y < 0.0f) SpinDrag.y = 0.0f;
		if (SpinDrag.y > 1.0f) SpinDrag.y = 1.0f;

		SpinDrag.z = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_Z);
		SpinDrag.z = 1.0f - (SpinDrag.z * 0.01f);
		if (SpinDrag.z < 0.0f) SpinDrag.z = 0.0f;
		if (SpinDrag.z > 1.0f) SpinDrag.z = 1.0f;

		Particles[n]->Angle += (Particles[n]->Spin * SpinDrag) * DeltaTime;


		Vector TrackPos;
		TrackPos.x = Particles[n]->Graph_TrackX->GetValue(Time, LifeTime, Particles[n]->KTrackX);
		TrackPos.y = Particles[n]->Graph_TrackY->GetValue(Time, LifeTime, Particles[n]->KTrackY);
		TrackPos.z = Particles[n]->Graph_TrackZ->GetValue(Time, LifeTime, Particles[n]->KTrackZ);
		TrackPos = TrackPos * Particles[n]->matWorld;

		//FIX ME !!!
		float BlendPhys = Particles[n]->Graph_PhysBlend->GetValue(Time, LifeTime, Particles[n]->KPhysBlend);
		BlendPhys = 1.0f - (BlendPhys * DeltaTime);
		if (BlendPhys < 0.0f) BlendPhys = 0.0f;
		if (BlendPhys > 1.0f) BlendPhys = 1.0f;


		//Save old positions
		Particles[n]->OldRenderPos = Particles[n]->RenderPos;
		Particles[n]->OldRenderAngle = Particles[n]->RenderAngle;


		Particles[n]->RenderPos.Lerp(TrackPos, Particles[n]->PhysPos, BlendPhys);
		Particles[n]->PhysPos = Particles[n]->RenderPos;

		//Particles[n]->RenderPos	= Particles[n]->PhysPos;
		Particles[n]->RenderAngle = Particles[n]->Angle;
	}


	//Рождаем партиклы, которые привязанны к нашему партиклу...

	for (DWORD n = 0; n < Particles.Size(); n++)
	{
		if (Particles[n]->AttachedEmitter)
		{
			//api->Trace("%d, %3.2f, %3.2f, %3.2f", n, Particles[n]->RenderPos.x, Particles[n]->RenderPos.y, Particles[n]->RenderPos.z);
			//Particles[n]->AttachedEmitter->SaveTime();
			Particles[n]->AttachedEmitter->Teleport(Matrix(Particles[n]->OldRenderAngle, Particles[n]->OldRenderPos));
			Particles[n]->AttachedEmitter->SetTransform(Matrix(Particles[n]->RenderAngle, Particles[n]->RenderPos));
			Particles[n]->AttachedEmitter->BornParticles(DeltaTime);
			
			//if (n < Particles.Size()-1)  Particles[n]->AttachedEmitter->RestoreTime();
		}
	}

	//RDTSC_E (t);
	//api->Trace("Time - %d", t);
}


DWORD ModelProcessor::GetCount ()
{
	return Particles.Size();
}

void ModelProcessor::DeleteWithGUID (DWORD dwGUID, DWORD GUIDRange)
{
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		if (Particles[j]->EmitterGUID >= dwGUID && Particles[j]->EmitterGUID < dwGUID+GUIDSTEP)
		{
			*(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount)-1);
			FreeParticle (Particles[j]);
			Particles.ExtractNoShift(j);
			j--;
		}
	}
}


//Рисует все плашки...
void ModelProcessor::Draw()
{
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		MDL_ParticleData* pR = Particles[j];

		
		pMasterManager->Render()->SetTransform(D3DTS_WORLD, Matrix (pR->RenderAngle, pR->RenderPos));
		pR->pScene->Draw(NULL, 0, NULL);
		
	}

	//api->Trace ("PSYS 2.0 : Draw %d model particles", Particles.Size());

}


void ModelProcessor::Clear ()
{
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
			*(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount)-1);
			FreeParticle (Particles[j]);
	}
	Particles.DelAll();
}