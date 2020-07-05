#include "TFlotsam.h"
#include "..\\common_h\\rands.h"

int		  TFlotsam::modelsInitialized = 0;
MODEL *   TFlotsam::models[FLOTSAM_MODELS_COUNT];
char	  TFlotsam::modelNames[FLOTSAM_MODELS_COUNT][128];
ENTITY_ID TFlotsam::modelIDs[FLOTSAM_MODELS_COUNT];

//--------------------------------------------------------------------
TFlotsam::TFlotsam()
	:sea(0)
	,enabled(false)
{

}

//--------------------------------------------------------------------
TFlotsam::~TFlotsam()
{
	--modelsInitialized;

	if (!modelsInitialized)
		for (int i=0; i<FLOTSAM_MODELS_COUNT; i++)
			_CORE_API->DeleteEntity(modelIDs[i]);
}

//--------------------------------------------------------------------
void TFlotsam::Start(float _x, float _z, float _radius)
{
	enabled = true;
	floatTime = 0;
	x = _x + randCentered(_radius);
	z = _z + randCentered(_radius);
	y = sea->WaveXZ(x, z)-FLOTSAM_START_DELTAY; 
	vX = randCentered(1.0f) * FLOTSAM_H_SPEED;
	vY = randUpper(1.0f) * FLOTSAM_V_SPEED;
	vZ = randCentered(1.0f) * FLOTSAM_H_SPEED;
	angY = rand(PIm2);
	turnY = randCentered(1.0f) * FLOTSAM_TURN_SPEED;
	maxFloatTime = (dword) randUpper(FLOTSAM_FLOAT_TIME);
	state = FLOTSAM_RISE;
	//model = models[rand() % FLOTSAM_MODELS_COUNT];
	ModelID = modelIDs[rand() % FLOTSAM_MODELS_COUNT];
	
}

//--------------------------------------------------------------------
void TFlotsam::Initialize(SEA_BASE *_sea)
{
	if (!modelsInitialized)
	{
		//modelsInitialized = true;
		strcpy(modelNames[0], "particles\\palka01");
		strcpy(modelNames[1], "particles\\palka02");
		strcpy(modelNames[2], "particles\\palka03");
		strcpy(modelNames[3], "particles\\palka04");

		for (int i=0; i<FLOTSAM_MODELS_COUNT; i++)
		{
			_CORE_API->CreateEntity(&modelIDs[i],"MODELR");
			_CORE_API->Send_Message(modelIDs[i],"ls",MSG_MODEL_LOAD_GEO, modelNames[i]);
			models[i] = (MODEL*)_CORE_API->GetEntityPointer(&modelIDs[i]);
		}
	}

	++modelsInitialized;
	sea = _sea;
}

//--------------------------------------------------------------------
void TFlotsam::Process(dword _dTime)
{
	if (!enabled)
		return;

	float dTime = _dTime / 1000.0f; // msec -> sec

	switch (state)
	{
	case FLOTSAM_RISE:
		x += dTime * vX;
		z += dTime * vZ;
		y += dTime * vY;
		if (y > (sea->WaveXZ(x,z) + FLOTSAM_DY))
			state = FLOTSAM_FLOAT;
		break;
	case FLOTSAM_FLOAT:
		x += dTime * vX;
		z += dTime * vZ;
		floatTime += _dTime;
		angY = (float) fmod(angY + dTime * turnY, PIm2);
		if (floatTime > maxFloatTime)
			state = FLOTSAM_SINK;
		break;
	case FLOTSAM_SINK:
		x += dTime * vX;
		z += dTime * vZ;
		y -= dTime * vY;
		if (y <= sea->WaveXZ(x,z) - FLOTSAM_START_DELTAY)
			enabled = false;
		break;
	}

}

//--------------------------------------------------------------------
void TFlotsam::Realize(dword _dTime)
{
	if (!enabled)
		return;

	CVECTOR pos(x, 0, z);
	if (state == FLOTSAM_FLOAT)
		pos.y = sea->WaveXZ(x,z) + FLOTSAM_DY;
	else
		pos.y = y;
	CVECTOR ang(0.0f, angY, 0.0f);
	MODEL * model;
	model = (MODEL *)api->GetEntityPointer(&ModelID);
	if(model)
	{
		model->mtx.BuildMatrix(ang, pos);
		model->Realize(_dTime);
	}
}

//--------------------------------------------------------------------
bool TFlotsam::Enabled()
{
	return enabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
