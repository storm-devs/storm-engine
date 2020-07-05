#include "TFishSchools.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\rands.h"

#pragma warning (disable : 4244)

//--------------------------------------------------------------------
TFishSchools::TFishSchools()
	:enabled(false)
{
	ZeroMemory(fishSchools, FISHSCHOOL_COUNT << 2);
}

//--------------------------------------------------------------------
TFishSchools::~TFishSchools()
{
	for (int i=0; i<fishSchoolsCount; i++)
	{
		if (fishSchools[i])
			delete fishSchools[i];
	}

	_CORE_API->DeleteEntity(fishSchoolModel);
}

//--------------------------------------------------------------------
void TFishSchools::LoadSettings()
{
	INIFILE * ini = _CORE_API->fio->OpenIniFile(ANIMALS_INI_FILENAME);
	if (!ini)
		return;

	fishSchoolsCount = ini->GetLong(ANIMALS_FISHSCHOOLS_SECTION, "count", FISHSCHOOL_COUNT);
	maxDistance = ini->GetFloat(ANIMALS_FISHSCHOOLS_SECTION, "distance", FISHSCHOOL_DISTANCE);

	delete ini;
}

//--------------------------------------------------------------------
void TFishSchools::Init()
{
	LoadSettings();

	renderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!renderService)	
		_THROW("!FishSchools: No service 'dx8render'");

	_CORE_API->FindClass(&seaID, "sea", 0);
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);
	if (!sea)
	{
		enabled = false;
		return;
	}

	for (int i=0; i<fishSchoolsCount; i++)
	{
		fishSchools[i] = NEW TFishSchool();
		fishSchools[i]->TDynamicObject::Initialize(CVECTOR(0.0f, 0.0f, 0.0f), maxDistance);
		fishSchools[i]->depth = 0.05f;
		fishSchools[i]->time = 0;
		fishSchools[i]->timeDivider = randUpper(100.0f);
		fishSchools[i]->amplitude = rand(0.01f);
		TDynamicSystem::AddDeflector(fishSchools[i]);
	}

	TDynamicSystem::AddAttractor(&cameraObject);

	_CORE_API->CreateEntity(&fishSchoolModel,"MODELR");
	_CORE_API->Send_Message(fishSchoolModel,"ls",MSG_MODEL_LOAD_GEO, ANIMALS_FISHSCHOOL_FILENAME);
}

//--------------------------------------------------------------------
dword TFishSchools::ProcessMessage(long _code, MESSAGE & message)
{
	GUARD(TFishSchools::ProcessMessage)

	dword outValue = 0;

	switch (_code)
	{
	case MSG_ANIMALS_FISHSCHOOLS_SHOW:
		enabled = true;
		break;

	case MSG_ANIMALS_FISHSCHOOLS_HIDE:
		enabled = false;
		break;

	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void TFishSchools::Execute(dword _dTime)
{
	GUARD(ANIMALS::Execute)

	if (!enabled)
		return;

	CVECTOR pos, ang, fishPos;
	float   persp;
	renderService->GetCamera(pos, ang, persp);

	cameraObject.SetXYZ(pos);
	float speedK = ((float) _dTime) / 1000.0f;
	for (int i = 0; i<fishSchoolsCount; i++)
	{
		//respawn near camera if needed
		fishPos = fishSchools[i]->GetXYZ();
		if ((abs(fishPos.x - pos.x) + fabs(fishPos.x - pos.x)) > RESPAWN_DISTANCE)
			fishSchools[i]->TDynamicObject::Initialize(pos, maxDistance);

		//recalculate all fishes
		fishSchools[i]->Calculate(attractors, MAX_DYNAMIC_OBJECTS,deflectors, MAX_DYNAMIC_OBJECTS, speedK);
		fishSchools[i]->time += _dTime;
	}


	UNGUARD
}

//--------------------------------------------------------------------
void TFishSchools::Realize(dword _dTime)
{
	GUARD(ANIMALS::Realize)

	if (!enabled)
		return;
/*
	CVECTOR cameraPos, cameraAng;
	float   cameraPersp;
	renderService->GetCamera(cameraPos, cameraAng, cameraPersp);
*/
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);
	if (!sea)
	{
		enabled = false;
		return;
	}

	MODEL *fishSchool = (MODEL*) _CORE_API->GetEntityPointer(&fishSchoolModel);
	if (!fishSchool)
		return;

	for (int i = 0; i<fishSchoolsCount; i++)
	{
		static const float OSC_AMPLITUDE = 0.1f;
		float fishSchoolAngle = fishSchools[i]->GetAngle();
		CVECTOR fishSchoolPos = fishSchools[i]->GetXYZ();
		float fishSchoolTime = fishSchools[i]->time / fishSchools[i]->timeDivider;
		CVECTOR ang(0.0f, PId2-fishSchoolAngle - (fishSchools[i]->amplitude*(PId2 / 10.0f) / 0.1f)*cosf(fishSchoolTime), 0.0f);
		CVECTOR pos;
		pos.x = fishSchoolPos.x + fishSchools[i]->amplitude*cosf(fishSchoolAngle + PId2)*sinf(fishSchoolTime);
		pos.z = fishSchoolPos.z + fishSchools[i]->amplitude*sinf(fishSchoolAngle + PId2)*sinf(fishSchoolTime);
		pos.y = sea->WaveXZ(pos.x, pos.z) - fishSchools[i]->depth;
		fishSchool->mtx.BuildMatrix(ang,pos);
		fishSchool->Realize(_dTime);
	}

	UNGUARD
}

//--------------------------------------------------------------------
