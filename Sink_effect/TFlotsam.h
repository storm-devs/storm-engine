#ifndef _TFLOTSAM_H_
#define _TFLOTSAM_H_

#include "..\common_h\sea_base.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\model.h"
#include "SinkSplashDefines.h"

enum tFlotsamState
{
	FLOTSAM_RISE,
	FLOTSAM_FLOAT,
	FLOTSAM_SINK
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TFlotsam  
{
public:
	TFlotsam();
	virtual ~TFlotsam();

	void Start(float _x, float _z, float _radius);
	void Initialize(SEA_BASE *_sea);
	void Process(dword _dTime);
	void Realize(dword _dTime);
	bool Enabled();
private:
	SEA_BASE *sea;

	float x, z, y, vX, vZ, vY, turnY, angY;
	tFlotsamState state;
	dword floatTime, maxFloatTime;
	bool enabled;
	//MODEL *model;
	ENTITY_ID ModelID;

	static MODEL * models[FLOTSAM_MODELS_COUNT];
	static char modelNames[FLOTSAM_MODELS_COUNT][128];
	static int modelsInitialized;
	static ENTITY_ID modelIDs[FLOTSAM_MODELS_COUNT];
};

#endif
