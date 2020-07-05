#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\net.h"
#include "SeafoamDefines.h"

#include "SEAFOAM.h"

INTERFACE_FUNCTION
CREATE_CLASS(SEAFOAM)	

//ENTITY_ID arrowModel;

#define U_SPEED_K 24e-4f
#define V_SPEED_K 10e-5f
#define START_FADE_SPEED 5.f

//--------------------------------------------------------------------
SEAFOAM::SEAFOAM()
	:shipsCount(0)
	,isStorm(false)
{
	psIni = null;
	renderer = null;
}

//--------------------------------------------------------------------
SEAFOAM::~SEAFOAM()
{
	GUARD(SEAFOAM::~SEAFOAM)

	ReleaseShipFoam();
	delete psIni;
	if (renderer && (carcassTexture >= 0))
		renderer->TextureRelease(carcassTexture);
	UNGUARD
}

//--------------------------------------------------------------------
bool SEAFOAM::Init()
{
	GUARD(SEAFOAM::Init)

	if (api->IsNetActive())
	{
		NetFindClass(false, &seaID, "NetSea");
		sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);
	}
	else
	{
		_CORE_API->FindClass(&seaID, "sea", 0);
		sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);
	}

	renderer = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	soundService = (VSoundService *)_CORE_API->CreateService("SoundService");

	psIni = _CORE_API->fio->OpenIniFile("resource\\ini\\particles.ini");

	InitializeShipFoam();

	//_CORE_API->CreateEntity(&arrowModel,"MODELR");
	//_CORE_API->Send_Message(arrowModel,"ls",MSG_MODEL_LOAD_GEO, "fish01");

	carcassTexture = renderer->TextureCreate("seafoam_2.tga");
	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void SEAFOAM::InitializeShipFoam()
{
	ENTITY_ID shipID;
	array<ENTITY_ID> aShipsID(_FL_);

	if (api->FindClass(&shipID, "ship", 0)) do
	{
		aShipsID.Add(shipID);
	} while (api->FindClassNext(&shipID));

	for (long i=0; i<aShipsID; i++) AddShip(&aShipsID[i]);
}

void SEAFOAM::AddShip(ENTITY_ID * pShipEID)
{
	tShipFoamInfo * foamInfo = &shipFoamInfo[shipsCount++];

	foamInfo->ship = (SHIP_BASE*) _CORE_API->GetEntityPointer(pShipEID);
	foamInfo->shipModel = foamInfo->ship->GetModel();
	foamInfo->shipModel->GetNode(0)->geo->GetInfo(foamInfo->hullInfo);
	foamInfo->enabled = true;
	foamInfo->frontEmitter[0] = NEW SEAFOAM_PS();
	foamInfo->frontEmitter[0]->Init(psIni, "seafoam");
	foamInfo->frontEmitter[1] = NEW SEAFOAM_PS();
	foamInfo->frontEmitter[1]->Init(psIni, "seafoam2");
	foamInfo->frontEmitter[2] = NEW SEAFOAM_PS();
	foamInfo->frontEmitter[2]->Init(psIni, "seafoam_front");

	CreateTracePoints(foamInfo);
	float wideK = sqrtf(foamInfo->hullInfo.boxsize.y / 17.f);
	foamInfo->carcass[0] = NEW TCarcass(TRACE_STEPS_Z, MEASURE_POINTS, renderer, true);
	foamInfo->carcass[0]->Initialize();
	foamInfo->carcass[0]->InitCircleMeasure(wideK*1.4f, wideK*-1.f, .55f);

	foamInfo->carcass[1] = NEW TCarcass(TRACE_STEPS_Z, MEASURE_POINTS, renderer, false);
	foamInfo->carcass[1]->Initialize();
	foamInfo->carcass[1]->InitCircleMeasure(wideK*1.4f, wideK*1.f, .55f);

	foamInfo->firstSoundPlay = true;
	foamInfo->doSplash = false;
	foamInfo->sound = 0;
}

//--------------------------------------------------------------------
void SEAFOAM::ReleaseShipFoam()
{
	tShipFoamInfo *foamInfo = 0;

	for (int ship = 0; ship < shipsCount; ship++)
	{
		foamInfo = &shipFoamInfo[ship];

		foamInfo->carcass[0]->Uninitialize();
		delete foamInfo->carcass[0];
		foamInfo->carcass[1]->Uninitialize();
		delete foamInfo->carcass[1];

		delete foamInfo->frontEmitter[0];
		delete foamInfo->frontEmitter[1];
		delete foamInfo->frontEmitter[2];
	}
	shipsCount = 0;
}

//--------------------------------------------------------------------
void SEAFOAM::CreateTracePoints(tShipFoamInfo *_shipFoamInfo)
{
	float yStep =  0.9f * _shipFoamInfo->hullInfo.boxsize.y / (TRACE_STEPS_Y - 1);
	float zStep =  .15f * _shipFoamInfo->hullInfo.boxsize.z / TRACE_STEPS_Z;
	float curY, curZ;
	GEOS::VERTEX startSrcV, startDestV;
	float startZ[TRACE_STEPS_Y];

	// <find_startZ>
	curY = _shipFoamInfo->hullInfo.boxcenter.y + (_shipFoamInfo->hullInfo.boxsize.y / 2.0f);
	
	startSrcV.x = _shipFoamInfo->hullInfo.boxcenter.x;
	startSrcV.z = _shipFoamInfo->hullInfo.boxcenter.z + _shipFoamInfo->hullInfo.boxsize.z / 2.0f;
	
	startDestV.x = _shipFoamInfo->hullInfo.boxcenter.x;
	startDestV.z = _shipFoamInfo->hullInfo.boxcenter.z - _shipFoamInfo->hullInfo.boxsize.z / 2.0f;
	
	for (int y = 0; y < TRACE_STEPS_Y; y++, curY -= yStep)
	{
		startSrcV.y = curY;
		startDestV.y = curY;
		float d = _shipFoamInfo->shipModel->GetNode(0)->geo->Trace(startSrcV, startDestV);
		if (d <= 1.0f)
		{
			startZ[y] = d * _shipFoamInfo->hullInfo.boxsize.z/*(startSrcV.z - 2.0f * _shipFoamInfo->hullInfo.boxcenter.z) + _shipFoamInfo->hullInfo.boxcenter.z*/;
			if (startZ[y] > (_shipFoamInfo->hullInfo.boxsize.z / 4.0f))
				startZ[y] = 0.0f;
		}
		else
			startZ[y] = 0.0f;
	}

	// <trace_from_sides>
	float halfZ = (_shipFoamInfo->hullInfo.boxsize.z / 2.0f);
	curZ = _shipFoamInfo->hullInfo.boxcenter.z + halfZ;
	for (int z = 0; z < TRACE_STEPS_Z; z++, curZ -= zStep*(1+8*sinf(PId2*(_shipFoamInfo->hullInfo.boxcenter.z + halfZ - curZ)/halfZ)))
	{
		curY = _shipFoamInfo->hullInfo.boxcenter.y + (_shipFoamInfo->hullInfo.boxsize.y / 2.0f);
		CVECTOR meanPointSum(0.0f, 0.0f, 0.0f);
		int meanCount = 0;
		int firstEnabledY = 0;
		float deltaZ;
		float kSum = 0.0f;

		for (y = 0; y < TRACE_STEPS_Y; y++, curY -= yStep)
		{
			deltaZ = startZ[y];
			CVECTOR srcLeft(_shipFoamInfo->hullInfo.boxcenter.x - _shipFoamInfo->hullInfo.boxsize.x / 2.0f, curY, curZ-deltaZ);
			CVECTOR srcRight(_shipFoamInfo->hullInfo.boxcenter.x + _shipFoamInfo->hullInfo.boxsize.x / 2.0f, curY, curZ-deltaZ);
			CVECTOR dst(_shipFoamInfo->hullInfo.boxcenter.x, curY, curZ-deltaZ);
			GEOS::VERTEX srcLeftV, srcRightV, dstV;
			srcLeftV.x = srcLeft.x; srcLeftV.y = srcLeft.y; srcLeftV.z = srcLeft.z;
			srcRightV.x = srcRight.x; srcRightV.y = srcRight.y; srcRightV.z = srcRight.z;
			dstV.x = dst.x; dstV.y = dst.y; dstV.z = dst.z;

			_shipFoamInfo->hull[0][z].center[y].y = curY;
			_shipFoamInfo->hull[0][z].center[y].z = curZ-deltaZ;
			_shipFoamInfo->hull[1][z].center[y].y = curY;
			_shipFoamInfo->hull[1][z].center[y].z = curZ-deltaZ;
			// <from_left>
			float d = _shipFoamInfo->shipModel->GetNode(0)->geo->Trace(srcLeftV, dstV);
			if (d > 1.0f)
				_shipFoamInfo->hull[0][z].center[y].x = _shipFoamInfo->hullInfo.boxcenter.x;
			else
				_shipFoamInfo->hull[0][z].center[y].x = -0.0f + (1.0f - d) * (srcLeft.x - _shipFoamInfo->hullInfo.boxcenter.x) + _shipFoamInfo->hullInfo.boxcenter.x;

			// <from_right>
			d = _shipFoamInfo->shipModel->GetNode(0)->geo->Trace(srcRightV, dstV);
			if (d > 1.0f)
				_shipFoamInfo->hull[1][z].center[y].x = _shipFoamInfo->hullInfo.boxcenter.x;
			else
				_shipFoamInfo->hull[1][z].center[y].x =  0.0f + (1.0f - d) * (srcRight.x - _shipFoamInfo->hullInfo.boxcenter.x) + _shipFoamInfo->hullInfo.boxcenter.x;
		}
	}

}

//--------------------------------------------------------------------
void SEAFOAM::InterpolateLeftParticle(tShipFoamInfo &_shipFoamInfo, int z, dword _dTime)
{
	CVECTOR ang, finalAng, testPoint;
	CMatrix finalMatrix;
	float seaY, interpK;
	int curY;
	CVECTOR lowPoint, highPoint, lastTestPoint, finalPos;
	int testY, lastTestY;
	float lowSeaY, highSeaY, seaK;

	lowPoint = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.hull[0][z].center[TRACE_STEPS_Y-1]);
	highPoint = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.hull[0][z].center[0]);
	lowSeaY = sea->WaveXZ(lowPoint.x, lowPoint.z);
	highSeaY = sea->WaveXZ(highPoint.x, highPoint.z);

	seaK = (highSeaY - lowSeaY) / (_shipFoamInfo.hull[0][z].center[0].y - _shipFoamInfo.hull[0][z].center[TRACE_STEPS_Y].y); // k < 0
	if ((lowSeaY > lowPoint.y) && (highSeaY > highPoint.y))
	{ // above sea
		_shipFoamInfo.levelStarts[0][z] = _shipFoamInfo.hull[0][z].center[0];
		CVECTOR tempVector;
		_shipFoamInfo.shipModel->mtx.MulToInv(CVECTOR(highPoint.x, 
													  (lowSeaY + highSeaY) * 0.5f, 
													  highPoint.z), 
											  tempVector);
		_shipFoamInfo.levelStarts[0][z].y = tempVector.y;
		return;
	}

	if ((lowSeaY < lowPoint.y) && (highSeaY < highPoint.y))
	{ // under sea
		_shipFoamInfo.levelStarts[0][z] = _shipFoamInfo.hull[0][z].center[TRACE_STEPS_Y-1];
		CVECTOR tempVector;
		_shipFoamInfo.shipModel->mtx.MulToInv(CVECTOR(lowPoint.x, 
													  (lowSeaY + highSeaY) * 0.5f, 
													  lowPoint.z), 
											  tempVector);
		_shipFoamInfo.levelStarts[0][z].y = tempVector.y;
		return;
	}

	curY = 0;
	testPoint = _shipFoamInfo.hull[0][z].center[curY];
	lastTestPoint = testPoint;
	testY = curY;
	lastTestY = testY;

	seaY = /*__max(lowSeaY, highSeaY)*/(lowSeaY + highSeaY) / 2.0f;
	for (curY = 0; curY < TRACE_STEPS_Y; ++curY)
	{
		testPoint = _shipFoamInfo.shipModel->mtx * _shipFoamInfo.hull[0][z].center[curY];

		if (testPoint.y < seaY)
			break;
		lastTestPoint = testPoint;
		lastTestY = testY;
		testY = curY;
	}

	if (curY < TRACE_STEPS_Y)
	{ //successful search
		interpK = (seaY - lastTestPoint.y) / (testPoint.y - lastTestPoint.y);
		testPoint = _shipFoamInfo.hull[0][z].center[curY];
		lastTestPoint = _shipFoamInfo.hull[0][z].center[curY-1];
		_shipFoamInfo.levelStarts[0][z] = lastTestPoint + interpK*(testPoint - lastTestPoint);//interpK*(testPoint - lastTestPoint);
	}
	else
	{
		_shipFoamInfo.levelStarts[0][z] = _shipFoamInfo.hull[0][z].center[TRACE_STEPS_Y-1];
	}
}

//--------------------------------------------------------------------
void SEAFOAM::InterpolateRightParticle(tShipFoamInfo &_shipFoamInfo, int z, dword _dTime)
{
	CVECTOR ang, finalAng, testPoint;
	CMatrix finalMatrix;
	float seaY, interpK;
	int curY;
	CVECTOR lowPoint, highPoint, lastTestPoint, finalPos;
	int testY, lastTestY;
	float lowSeaY, highSeaY, seaK;

	lowPoint = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.hull[1][z].center[TRACE_STEPS_Y-1]);
	highPoint = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.hull[1][z].center[1]);
	lowSeaY = sea->WaveXZ(lowPoint.x, lowPoint.z);
	highSeaY = sea->WaveXZ(highPoint.x, highPoint.z);

	seaK = (highSeaY - lowSeaY) / (_shipFoamInfo.hull[1][z].center[1].y - _shipFoamInfo.hull[1][z].center[TRACE_STEPS_Y].y); // k < 0
	if ((lowSeaY > lowPoint.y) && (highSeaY > highPoint.y))
	{ // above sea
		_shipFoamInfo.levelStarts[1][z] = _shipFoamInfo.hull[1][z].center[1];
		CVECTOR tempVector;
		_shipFoamInfo.shipModel->mtx.MulToInv(CVECTOR(highPoint.x, 
													  (lowSeaY + highSeaY) * 0.5f, 
													  highPoint.z), 
											  tempVector);
		_shipFoamInfo.levelStarts[1][z].y = tempVector.y;
		return;
	}

	if ((lowSeaY < lowPoint.y) && (highSeaY < highPoint.y))
	{ // under sea
		_shipFoamInfo.levelStarts[1][z] = _shipFoamInfo.hull[1][z].center[TRACE_STEPS_Y-1];
		CVECTOR tempVector;
		_shipFoamInfo.shipModel->mtx.MulToInv(CVECTOR(lowPoint.x, 
													  (lowSeaY + highSeaY) * 0.5f, 
													  lowPoint.z), 
											  tempVector);
		_shipFoamInfo.levelStarts[1][z].y = tempVector.y;
		return;
	}

	curY = 0;
	testPoint = _shipFoamInfo.hull[1][z].center[curY];
	lastTestPoint = testPoint;
	testY = curY;
	lastTestY = testY;

	seaY = /*__max(lowSeaY, highSeaY)*/(lowSeaY + highSeaY) / 2.0f;
	for (curY = 0; curY < TRACE_STEPS_Y; ++curY)
	{
		testPoint = _shipFoamInfo.shipModel->mtx * _shipFoamInfo.hull[1][z].center[curY];

		if (testPoint.y < seaY)
			break;
		lastTestPoint = testPoint;
		lastTestY = testY;
		testY = curY;
	}

	if (curY < TRACE_STEPS_Y)
	{ //successful search
		interpK = (seaY - lastTestPoint.y) / (testPoint.y - lastTestPoint.y);
		testPoint = _shipFoamInfo.hull[1][z].center[curY];
		lastTestPoint = _shipFoamInfo.hull[1][z].center[curY-1];
		_shipFoamInfo.levelStarts[1][z] = lastTestPoint + interpK*(testPoint - lastTestPoint);//interpK*(testPoint - lastTestPoint);
	}
	else
	{
		_shipFoamInfo.levelStarts[1][z] = _shipFoamInfo.hull[1][z].center[TRACE_STEPS_Y-1];
	}
}

//--------------------------------------------------------------------
void SEAFOAM::RealizeShipFoam_Particles(tShipFoamInfo &_shipFoamInfo, dword _dTime)
{
	//MODEL *arrow = (MODEL*)_CORE_API->GetEntityPointer(&arrowModel);

	for (int z = 0; z < TRACE_STEPS_Z; ++z)
	{
		/*
		for (int y=0; y<TRACE_STEPS_Y; ++y)
		{
			CVECTOR fPos = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.hull[0][z].center[y]);
			arrow->mtx.BuildMatrix(CVECTOR(1.0f, 0.0f, 0.0f), fPos);
			arrow->Realize(_dTime);
			
			fPos = _shipFoamInfo.shipModel->mtx * (_shipFoamInfo.levelStarts[0][z]);
			arrow->mtx.BuildMatrix(CVECTOR(0.0f, 0.0f, 0.0f), fPos);
			arrow->Realize(_dTime);
			arrow->mtx.BuildMatrix(CVECTOR(0.0f, PId2, 0.0f), fPos);
			arrow->Realize(_dTime);
			arrow->mtx.BuildMatrix(CVECTOR(PId2, 0.0f, 0.0f), fPos);
			arrow->Realize(_dTime);

		}
		*/
		InterpolateLeftParticle(_shipFoamInfo, z, _dTime);
		InterpolateRightParticle(_shipFoamInfo, z, _dTime);
	}

	dword ticks = 0;
	RDTSC_B(ticks)

	CVECTOR frontEmitterPos = 0.5f*(_shipFoamInfo.levelStarts[0][0] + _shipFoamInfo.levelStarts[1][0]);
	float shipSpeed = _shipFoamInfo.ship->GetCurrentSpeed();
	float speedDeltaY = 0.f;
	if (shipSpeed < START_FADE_SPEED)
		speedDeltaY = 1.f*(1.f-shipSpeed/START_FADE_SPEED);
	frontEmitterPos.z += FOAM_SHIFT_Z + .25f * shipSpeed;
	frontEmitterPos.y -= speedDeltaY;
	frontEmitterPos.x -= FOAM_SHIFT_X;
	_shipFoamInfo.frontEmitter[0]->SetEmitter(_shipFoamInfo.shipModel->mtx * frontEmitterPos
											 ,CVECTOR(0.f, 1.f, 0.f));
	_shipFoamInfo.frontEmitter[0]->Realize(_dTime);

	frontEmitterPos.x += 2*FOAM_SHIFT_X;
	_shipFoamInfo.frontEmitter[1]->SetEmitter(_shipFoamInfo.shipModel->mtx * frontEmitterPos
											 ,CVECTOR(0.f, 1.f, 0.f));
	_shipFoamInfo.frontEmitter[1]->Realize(_dTime);
 
	frontEmitterPos.x -= FOAM_SHIFT_X;

	if (isStorm)
	{
		static float oldFrontEmitterPosY = frontEmitterPos.y;
		if ((frontEmitterPos.y-oldFrontEmitterPosY)/_dTime < 5e-4f) //0.025
		{
			_shipFoamInfo.frontEmitter[2]->EnableEmit(false);
			_shipFoamInfo.doSplash = false;
		}
		else
		{
			_shipFoamInfo.frontEmitter[2]->EnableEmit(true);
			_shipFoamInfo.doSplash = true;
		}
		oldFrontEmitterPosY = frontEmitterPos.y;
		CMatrix mtx2 = _shipFoamInfo.shipModel->mtx;
		mtx2.Pos() = 0.f;
		CVECTOR a = mtx2 * CVECTOR(0.f, 1.0f, -.15f);
		_shipFoamInfo.frontEmitter[2]->SetEmitter(_shipFoamInfo.shipModel->mtx * frontEmitterPos
												 ,a);
		_shipFoamInfo.frontEmitter[2]->Realize(_dTime);
	}

	if (soundService && (_shipFoamInfo.doSplash))
	{
		if (!_shipFoamInfo.sound || !soundService->SoundIsPlaying(_shipFoamInfo.sound))
			_shipFoamInfo.sound = soundService->SoundPlay("ship_bow", PCM_3D, VOLUME_FX, false, false, true);
		//_shipFoamInfo.sound = soundService->SoundPlay("ship_bow", PCM_3D, VOLUME_FX, false, true, true);
		if (_shipFoamInfo.sound)
		{
			CVECTOR pos = _shipFoamInfo.shipModel->mtx*CVECTOR(0.f, 0.f, _shipFoamInfo.hullInfo.boxsize.z/2.f);
			pos.y = sea->WaveXZ(pos.x,pos.z);
			//soundService->SoundSetVolume(_shipFoamInfo.sound, _shipFoamInfo.splashFactor);
			soundService->SoundSet3DParam(_shipFoamInfo.sound, SM_POSITION, &pos);
		}
	}

	RDTSC_E(ticks)
	//api->Trace("Seafoam realize(carcass->Execute) = %d", ticks);
}

void SEAFOAM::RealizeShipFoam_Mesh(tShipFoamInfo &_shipFoamInfo, dword _dTime)
{
	_shipFoamInfo.carcass[0]->Execute(_dTime, _shipFoamInfo.shipModel->mtx, _shipFoamInfo.levelStarts[0]);
	_shipFoamInfo.carcass[1]->Execute(_dTime, _shipFoamInfo.shipModel->mtx, _shipFoamInfo.levelStarts[1]);

	_shipFoamInfo.carcass[0]->Realize(0);
	_shipFoamInfo.carcass[1]->Realize(0);
}

//--------------------------------------------------------------------
dword _cdecl SEAFOAM::ProcessMessage(MESSAGE & message)
{
	GUARD(SEAFOAM::ProcessMessage)

	long code = message.Long();
	dword outValue = 0;

	switch (code)
	{
	case MSG_SHIP_DELETE:
		{
			ATTRIBUTES *attrs = message.AttributePointer();
			tShipFoamInfo *foamInfo = 0;
			if (attrs)
				for (int ship = 0; ship < shipsCount; ship++)
				{
					if (attrs == shipFoamInfo[ship].ship->GetACharacter())
					{
						shipFoamInfo[ship].enabled = false;

						//shipFoamInfo[ship].carcass[0]->Uninitialize();
						//delete shipFoamInfo[ship].carcass[0];
						//shipFoamInfo[ship].carcass[1]->Uninitialize();
						//delete shipFoamInfo[ship].carcass[1];

						return outValue;
					}
				}
		}
		break;
	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void SEAFOAM::Realize(dword _dTime)
{
	GUARD(SEAFOAM::Realize)
	
	dword ticks = 0;
	RDTSC_B(ticks)

	tShipFoamInfo *foamInfo = 0;
	int ship;

	for (ship = 0; ship < shipsCount; ship++)
	{
		foamInfo = &shipFoamInfo[ship];
		if (!foamInfo->enabled)
			continue;
		RealizeShipFoam_Particles(*foamInfo, _dTime);
	}

	static CMatrix wMatrix;
	renderer->SetTransform(D3DTS_WORLD, (D3DMATRIX *) wMatrix);

	renderer->TextureSet(0, carcassTexture);
	bool techniqueStarted = renderer->TechniqueExecuteStart("new_seafoam");
	for (ship = 0; ship < shipsCount; ship++)
	{
		foamInfo = &shipFoamInfo[ship];
		if (!foamInfo->enabled)
			continue;
		RealizeShipFoam_Mesh(*foamInfo, _dTime);
	}
	if(techniqueStarted) 
		while (renderer->TechniqueExecuteNext())
			;

	RDTSC_E(ticks)
	//api->Trace("Seafoam realize = %d", ticks);
	UNGUARD
}

//--------------------------------------------------------------------
void SEAFOAM::Execute(dword _dTime)
{
	GUARD(SEAFOAM::Execute);
	tShipFoamInfo *foamInfo = 0;
	float shipSpeed, speedA;

	for (int ship = 0; ship < shipsCount; ship++)
	{
		foamInfo = &shipFoamInfo[ship];
		if (!foamInfo->enabled)
			continue;

		shipSpeed = foamInfo->ship->GetCurrentSpeed();
		if (shipSpeed < 0.f)
			shipSpeed = 0.f;
		if (shipSpeed > START_FADE_SPEED)
			speedA = 1.f;
		else
			speedA = shipSpeed / START_FADE_SPEED;

		foamInfo->carcass[0]->SetSpeed(U_SPEED_K * shipSpeed / shipFoamInfo->hullInfo.boxsize.z
									  ,V_SPEED_K * shipSpeed 
									  ,speedA);
		foamInfo->carcass[1]->SetSpeed(U_SPEED_K * shipSpeed / shipFoamInfo->hullInfo.boxsize.z
									  ,V_SPEED_K * shipSpeed 
									  ,speedA);
	}

	UNGUARD
}

//--------------------------------------------------------------------
dword SEAFOAM::AttributeChanged(ATTRIBUTES * pA)
{
	const char *nm = pA->GetThisName();

	if(!stricmp(nm,"storm"))
	{
		if (!strcmp(pA->GetThisAttr(), "true"))
			isStorm = true;
		else
			isStorm = false;
	}

	if (stricmp(nm, "AddNetShip") == 0)
	{
		ENTITY_ID shipID;
		dword dwShipNetID = pA->GetAttributeAsDword();
		if (api->IsNetActive())
		{
			if (NetFindClass(false, &shipID, "NetShip")) do
			{
				ENTITY * pE = api->GetEntityPointer(&shipID);
				if (pE->GetNetID() == dwShipNetID)
				{
					AddShip(&shipID);
					break;
				}
			} while (NetFindClassNext(false, &shipID));
		}
	}

	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
