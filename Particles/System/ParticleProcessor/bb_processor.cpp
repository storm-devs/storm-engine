#include "bb_processor.h"
#include "..\..\icommon\names.h"
#include "..\..\icommon\IEmitter.h"
#include "..\datasource\datagraph.h"
#include "..\datasource\datacolor.h"
#include "..\datasource\datauv.h"
#include "..\..\..\common_h\dx8render.h"
#include "..\..\..\common_h\exs.h"
#include "..\..\..\common_h\math3d.h"
#include "..\particlesystem\particlesystem.h"
#include "physic.h"

//Сколько всего может быть плашек
#define MAX_BILLBOARDS 4096



#define UV_TX1 0
#define UV_TX2 2
#define UV_TY1 3
#define UV_TY2 1

/* 
	 Коэфицент лодирования партиклов, чем выше тем меньше нагрузка на Filrate
   Должен быть > 0 !!!!!!
	 0.001 нет лодов .. 10000000 супер лоды (все партиклы в 1 пиксель)
*/
//=============================================================
#define PLOD 5.0f 
//=============================================================



BillBoardProcessor::BillBoardProcessor () : Particles(_FL_, MAX_BILLBOARDS)
{
	pMemArray = NEW MemArrayItem[MAX_BILLBOARDS];

	for (DWORD n = 0; n < MAX_BILLBOARDS; n++)
	{
		pMemArray[n].Free = true;
	}

	pRS = (VDX8RENDER*)api->CreateService("DX8Render");
	Assert (pRS);

	int RectVertexSize =  sizeof(RECT_VERTEX);

	pVBuffer = pRS->CreateVertexBuffer(0, MAX_BILLBOARDS * RectVertexSize* 4, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
	Assert (pVBuffer != -1);

	pIBuffer = pRS->CreateIndexBuffer(MAX_BILLBOARDS * 6 * sizeof(WORD));
	Assert (pIBuffer != -1);
	WORD * pTrgs = (WORD*)pRS->LockIndexBuffer(pIBuffer);
	Assert(pTrgs != NULL);

	for (long i = 0; i < MAX_BILLBOARDS; i++)
	{
		pTrgs[i * 6 + 0] = WORD(i * 4 + 0);
		pTrgs[i * 6 + 1] = WORD(i * 4 + 1);
		pTrgs[i * 6 + 2] = WORD(i * 4 + 2);
		pTrgs[i * 6 + 3] = WORD(i * 4 + 0);
		pTrgs[i * 6 + 4] = WORD(i * 4 + 2);
		pTrgs[i * 6 + 5] = WORD(i * 4 + 3);
	}
	pRS->UnLockIndexBuffer(pIBuffer);
	
}
 
BillBoardProcessor::~BillBoardProcessor ()
{

	delete pMemArray;

	pRS->ReleaseVertexBuffer(pVBuffer);
	pRS->ReleaseIndexBuffer(pIBuffer);

	pVBuffer = -1;
	pIBuffer = -1;

} 


//"Выделить" память для хранения партикла
BB_ParticleData* BillBoardProcessor::AllocParticle ()
{
	for (DWORD n = 0; n < MAX_BILLBOARDS; n++)
	{
		if (pMemArray[n].Free)
		{
			pMemArray[n].Free = false;
			return &pMemArray[n].pData;
		}
	}

	return NULL;
}

//"Убить" партикл
void BillBoardProcessor::FreeParticle (BB_ParticleData* pItem)
{
	for (DWORD n = 0; n < MAX_BILLBOARDS; n++)
	{
		if	(&pMemArray[n].pData == pItem)
		{
			pMemArray[n].Free = true;
			return;
		}
	}
}


void BillBoardProcessor::AddParticle (ParticleSystem* pSystem, const Vector& velocity_dir, const Vector& pos, const Matrix& matWorld, float EmitterTime, float EmitterLifeTime, FieldList* pFields, DWORD* pActiveCount, DWORD dwGUID)
{
	BB_ParticleData* pData = AllocParticle ();

	//Сработает если партиклов будет > MAX_BILLBOARDS, столько их быть не должно :))))
	if (!pData)
	{
		*(pActiveCount) = (*(pActiveCount)-1);
		return;
	}

	pData->Graph_TrackX = pFields->FindGraph(PARTICLE_TRACK_X);
	pData->Graph_TrackY = pFields->FindGraph(PARTICLE_TRACK_Y);
	pData->Graph_TrackZ = pFields->FindGraph(PARTICLE_TRACK_Z);

	Vector PositionOffset;
	PositionOffset.x = pData->Graph_TrackX->GetRandomValue(0.0f, 100.0f);
	PositionOffset.y = pData->Graph_TrackY->GetRandomValue(0.0f, 100.0f);
	PositionOffset.z = pData->Graph_TrackZ->GetRandomValue(0.0f, 100.0f);



	pData->SpeedOriented = pFields->GetBool(PARTICLE_DIR_ORIENT, false);
	pData->EmitterGUID = dwGUID;
	pData->ActiveCount = pActiveCount;
	pData->RenderPos = (pos+PositionOffset) * matWorld;
	pData->Velocity = matWorld.MulNormal(velocity_dir);
	pData->ElapsedTime = 0.0f;
	pData->matWorld = matWorld;

	
	
	pData->Angle = 0.0f;
	pData->RenderAngle = 0.0f;
	pData->ExternalForce = Vector(0.0f, 0.0f, 0.0f);
	pData->PhysPos = pData->RenderPos;

	pData->OldRenderPos = pData->RenderPos;
	pData->OldRenderAngle = pData->RenderAngle;

	pData->LifeTime = pFields->GetRandomGraphVal(PARTICLE_LIFE_TIME, EmitterTime, EmitterLifeTime);
	pData->Mass = pFields->GetRandomGraphVal(PARTICLE_MASS, EmitterTime, EmitterLifeTime);
	pData->Spin = pFields->GetRandomGraphVal(PARTICLE_SPIN, EmitterTime, EmitterLifeTime);
	pData->Spin = pData->Spin * MUL_DEGTORAD;

	float VelocityPower = pFields->GetRandomGraphVal(PARTICLE_VELOCITY_POWER, EmitterTime, EmitterLifeTime);
	pData->Velocity = pData->Velocity * VelocityPower;
	pData->UMass = fabsf (pData->Mass);


	pData->Graph_SpinDrag = pFields->FindGraph(PARTICLE_SPIN_DRAG);
	pData->Graph_Size = pFields->FindGraph(PARTICLE_SIZE);
	pData->Graph_Frames = pFields->FindGraph(PARTICLE_ANIMFRAME);
	pData->Graph_Color = pFields->FindColor(PARTICLE_COLOR);
	pData->Graph_UV = pFields->FindUV(PARTICLE_FRAMES);
	pData->Graph_Transparency = pFields->FindGraph(PARTICLE_TRANSPARENCY);
	pData->Graph_Drag = pFields->FindGraph(PARTICLE_DRAG);
	pData->Graph_PhysBlend = pFields->FindGraph(PARTICLE_PHYSIC_BLEND);
	pData->graph_GravK = pFields->FindGraph(PARTICLE_GRAVITATION_K);
	pData->graph_AddPower = pFields->FindGraph(PARTICLE_ADDPOWER);

	




	pData->DragK = FRAND (1.0f);
	pData->SpinDragK = FRAND (1.0f);
	pData->SizeK = FRAND (1.0f);
	pData->ColorK = FRAND (1.0f);
	pData->AlphaK = FRAND (1.0f);
	pData->FrameK = FRAND (1.0f);
	pData->GravKK = FRAND (1.0f);
	pData->AddPowerK = FRAND (1.0f);

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
void BillBoardProcessor::Process (float DeltaTime)
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
		float SpinDrag = Particles[n]->Graph_SpinDrag->GetValue(Time, LifeTime, Particles[n]->SpinDragK);
		SpinDrag = 1.0f -(SpinDrag * 0.01f);
		if (SpinDrag < 0.0f) SpinDrag = 0.0f;
		if (SpinDrag > 1.0f) SpinDrag = 1.0f;
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
		//Particles[n]->OldRenderAngle = Particles[n]->RenderAngle;


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
//			api->Trace("%d, %3.2f, %3.2f, %3.2f", n, Particles[n]->RenderPos.x, Particles[n]->RenderPos.y, Particles[n]->RenderPos.z);
//			Particles[n]->AttachedEmitter->SaveTime();
			Particles[n]->AttachedEmitter->Teleport(Matrix(Particles[n]->OldRenderAngle, Particles[n]->OldRenderPos));
			Particles[n]->AttachedEmitter->SetTransform(Matrix(Particles[n]->RenderAngle, Particles[n]->RenderPos));
			Particles[n]->AttachedEmitter->BornParticles(DeltaTime);
			
			//if (n < Particles.Size()-1)	Particles[n]->AttachedEmitter->RestoreTime();
		}
	}


	//RDTSC_E (t);
	//api->Trace("Time - %d", t);
}

//Считает расстояние до билбоардов
DWORD BillBoardProcessor::CalcDistanceToCamera ()
{
	DWORD VisParticles = 0;
	Matrix mView;
	pRS->GetTransform(D3DTS_VIEW, mView);
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		Particles[j]->CamDistance = Vector(Particles[j]->RenderPos * mView).z;
		
//		_mm_prefetch ((const char *)Particles[j+1], _MM_HINT_T0);

		if (Particles[j]->CamDistance > 0)
		{
			Particles[j]->Visible = true;
			VisParticles++;
		} else
				{
					Particles[j]->Visible = false;
				}
	}
	return VisParticles;
}

//Функция сравнения при сортировке
BOOL BillBoardProcessor::CompareFunction (BB_ParticleData* e1, BB_ParticleData* e2)
{
	if (e1->CamDistance > e2->CamDistance) return true;
	return false;
}


//Рисует все плашки...
void BillBoardProcessor::Draw()
{
	if (CalcDistanceToCamera() == 0)	return;
	ParticleSorter.QSort(CompareFunction, &Particles[0], Particles.Size());

	RECT_VERTEX * pVerts = (RECT_VERTEX*)pRS->LockVertexBuffer(pVBuffer, D3DLOCK_DISCARD);
	//RECT_VERTEX * pVerts = (RECT_VERTEX*)pVBuffer->Lock(0, 0, D3DLOCK_DISCARD);
	//RECT_VERTEX * pVerts = (RECT_VERTEX*)pVBuffer->Lock();

	long Index = 0;
	DWORD ParticlesCount = 0;
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		BB_ParticleData* pR = Particles[j];
		
		if (!pR->Visible) continue;

		bool SpeedOriented = pR->SpeedOriented;
		float fSize = pR->Graph_Size->GetValue(pR->ElapsedTime, pR->LifeTime, pR->SizeK);
		if (fSize <= 0.000001f) continue;

//		_mm_prefetch ((const char *)Particles[j+1], _MM_HINT_T0);

		
		float fAngle = pR->RenderAngle;
		Vector vPos = pR->RenderPos;
		DWORD dwColor = pR->Graph_Color->GetValue(pR->ElapsedTime, pR->LifeTime, pR->ColorK);

		float Alpha = pR->Graph_Transparency->GetValue(pR->ElapsedTime, pR->LifeTime, pR->AlphaK);
		Alpha = Alpha * 0.01f;
		Alpha = 1.0f - Alpha;
		if (Alpha < 0.0f) Alpha = 0.0f;
		if (Alpha > 1.0f) Alpha = 1.0f;
		Alpha = Alpha * 255.0f;


		float AddPower = pR->graph_AddPower->GetValue(pR->ElapsedTime, pR->LifeTime, pR->AddPowerK);
		AddPower = AddPower * 0.01f;
		AddPower = 1.0f - AddPower;
		if (AddPower < 0.0f) AddPower = 0.0f;
		if (AddPower > 1.0f) AddPower = 1.0f;

		//AddPower = 0.0f;

		
		

		float FrameIndex = pR->Graph_Frames->GetValue(pR->ElapsedTime, pR->LifeTime, pR->FrameK);
		long FrameIndexLong = fftol(FrameIndex);
		float FrameBlendK = 1.0f - (FrameIndex - FrameIndexLong);
		const Vector4& UV_WH1 = pR->Graph_UV->GetValue(FrameIndexLong);
		const Vector4& UV_WH2 = pR->Graph_UV->GetValue(FrameIndexLong+1);

		//Ограничитель максимального размера партиклов...
		//=============================================================
		float SizeK = pR->CamDistance / fSize;
		if (SizeK < PLOD)	fSize = pR->CamDistance / PLOD;
		//=============================================================

		RECT_VERTEX	* pV = &pVerts[Index * 4];
		Index++;
	 
		float DirAngle = 0.0f;
		float ScaleF = 1.0f;
		
		if (SpeedOriented)
		{
			Matrix matView;
			pRS->GetTransform(D3DTS_VIEW, matView);
			Vector SpeedVector = pR->Velocity;
				//pR->RenderPos - pR->OldRenderPos;
			SpeedVector = matView.MulNormal(SpeedVector);
			SpeedVector.Normalize();
			ScaleF = 1.0f - fabsf (SpeedVector.z);
			
			if (ScaleF < 0.3f) ScaleF = 0.3f;
			Alpha *= ScaleF;

			SpeedVector.z = SpeedVector.y;
			DirAngle = SpeedVector.GetAY(pR->OldRenderAngle);

			pR->OldRenderAngle = DirAngle;
		}
		
		DWORD dwAlpha = (BYTE)Alpha << 24;
		dwColor = dwColor & 0x00FFFFFF;
		dwColor = dwColor | dwAlpha;

		//if (j == 0)	api->Trace("fAngle[0]: %3.2f", fAngle);


		pV[0].vRelativePos = Vector(-fSize, -fSize, 0.0f);
		pV[0].dwColor = dwColor;
		pV[0].tu1 = UV_WH1.v4[UV_TX1]; pV[0].tv1 = UV_WH1.v4[UV_TY1];
		pV[0].tu2 = UV_WH2.v4[UV_TX1]; pV[0].tv2 = UV_WH2.v4[UV_TY1];
		pV[0].angle = fAngle; 
		pV[0].BlendK = FrameBlendK;
		pV[0].vParticlePos = vPos;
		pV[0].AddPowerK = AddPower;


		//if (SpeedOriented) pV[0].DirK = 0.0f; else pV[0].DirK = 1.0f; 

		if (SpeedOriented)
		{
			pV[0].angle = DirAngle;
			pV[0].vRelativePos.y *= ScaleF;
		}

		


		pV[1].vRelativePos = Vector(-fSize, fSize, 0.0f);
		pV[1].dwColor = dwColor;
		pV[1].tu1 = UV_WH1.v4[UV_TX1]; pV[1].tv1 = UV_WH1.v4[UV_TY2];
		pV[1].tu2 = UV_WH2.v4[UV_TX1]; pV[1].tv2 = UV_WH2.v4[UV_TY2];
		pV[1].angle = fAngle; 
		pV[1].BlendK = FrameBlendK;
		pV[1].vParticlePos = vPos;
		pV[1].AddPowerK = AddPower;
		//if (SpeedOriented) pV[1].DirK = 0.0f; else pV[1].DirK = 1.0f; 

		if (SpeedOriented)
		{
			pV[1].angle = DirAngle;
			pV[1].vRelativePos.y *= ScaleF;
		}




		pV[2].vRelativePos = Vector(fSize, fSize, 0.0f);
		pV[2].dwColor = dwColor;
		pV[2].tu1 = UV_WH1.v4[UV_TX2]; pV[2].tv1 = UV_WH1.v4[UV_TY2];
		pV[2].tu2 = UV_WH2.v4[UV_TX2]; pV[2].tv2 = UV_WH2.v4[UV_TY2];
		pV[2].angle = fAngle; 
		pV[2].BlendK = FrameBlendK;
		pV[2].vParticlePos = vPos;
		pV[2].AddPowerK = AddPower;
		//if (SpeedOriented) pV[2].DirK = 0.0f; else pV[2].DirK = 1.0f; 

		if (SpeedOriented)
		{
			pV[2].angle = DirAngle;
			pV[2].vRelativePos.y *= ScaleF;
		}



		pV[3].vRelativePos = Vector(fSize, -fSize, 0.0f);
		pV[3].dwColor = dwColor;
		pV[3].tu1 = UV_WH1.v4[UV_TX2]; pV[3].tv1 = UV_WH1.v4[UV_TY1];
		pV[3].tu2 = UV_WH2.v4[UV_TX2]; pV[3].tv2 = UV_WH2.v4[UV_TY1];
		pV[3].angle = fAngle; 
		pV[3].BlendK = FrameBlendK;
		pV[3].vParticlePos = vPos;
		pV[3].AddPowerK = AddPower;
		//if (SpeedOriented) pV[3].DirK = 0.0f; else pV[3].DirK = 1.0f; 

		if (SpeedOriented)
		{
			pV[3].angle = DirAngle;
			pV[3].vRelativePos.y *= ScaleF;
		}

		ParticlesCount++;
	}

	pRS->UnLockVertexBuffer(pVBuffer);

	Vector4 const1(0.0416666f, 1.0f, 0.0f, -0.5f);
	Vector4 const2(0.159155f, 0.5f, 0.25f, 6.28319f);
	Vector4 const3(-3.14159f, 0.0000247609f, -0.00138884f, -0.000000252399f);

	Vector4 cGlobal(0.0f, 1.0f, 0.5f, 0.0f);


	pRS->SetVertexShaderConstant(0, const1.v4, 1);
	pRS->SetVertexShaderConstant(1, const2.v4, 1);
	pRS->SetVertexShaderConstant(2, const3.v4, 1);
	pRS->SetVertexShaderConstant(13, cGlobal.v4, 1);

	Matrix matOldView, matView, matProjection;
	pRS->GetTransform(D3DTS_VIEW, matView);
	pRS->GetTransform(D3DTS_PROJECTION, matProjection);

	matOldView = matView;
	matView.Transposition();
	matProjection.Transposition();

	pRS->SetVertexShaderConstant(3, matView.matrix, 4);
	pRS->SetVertexShaderConstant(7, matProjection.matrix, 4);


	pRS->SetTransform(D3DTS_VIEW, Matrix());
	pRS->SetTransform(D3DTS_WORLD, Matrix());
	pRS->DrawBuffer(pVBuffer, sizeof (RECT_VERTEX), pIBuffer, 0, ParticlesCount * 4, 0, ParticlesCount * 2, "AdvancedParticles");

	pRS->SetTransform(D3DTS_VIEW, matOldView);

	//pRS->Print(20, 20, "PSYS 2.0 : Draw %d billboard particles", ParticlesCount);

}

DWORD BillBoardProcessor::GetCount ()
{
	return Particles.Size();
}

void BillBoardProcessor::DeleteWithGUID (DWORD dwGUID, DWORD GUIDRange)
{
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		if (Particles[j]->EmitterGUID >= dwGUID && Particles[j]->EmitterGUID < dwGUID+GUIDRange)
		{
			*(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount)-1);
			FreeParticle (Particles[j]);
			Particles.ExtractNoShift(j);
			j--;
		}
	}
}

void BillBoardProcessor::Clear ()
{
	for (DWORD j = 0; j <  Particles.Size(); j++)
	{
		*(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount)-1);
		FreeParticle (Particles[j]);
	}
	Particles.DelAll();
}