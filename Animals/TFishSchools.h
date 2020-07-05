#ifndef _TFISHSCHOOLS_H_
#define _TFISHSCHOOLS_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\ship_base.h"
#include "AnimalsDefines.h"
#include "TDynamicObject.h"
#include "TDynamicSystem.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
class TFishSchool: public TDynamicObject
{
public:
	float depth;
	dword time;
	float timeDivider;
	float amplitude;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TFishSchools : public TDynamicSystem
{
	friend class TFishSchool;
public:
	TFishSchools();
	virtual ~TFishSchools();

	dword ProcessMessage(long _code, MESSAGE & message);
	void Init();
	void Realize(dword _dTime);
	void Execute(dword _dTime);

private:
	void LoadSettings();

	VDX8RENDER *renderService;
	ENTITY_ID  fishSchoolModel;
	TFishSchool *fishSchools[FISHSCHOOL_COUNT];
	long shipsCount;
	long fishSchoolsCount;
	TDynamicObject cameraObject;

	float maxDistance;
	
	bool enabled;
	ENTITY_ID seaID;
	SEA_BASE *sea;
};

#endif // !defined
