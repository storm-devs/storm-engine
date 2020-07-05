#ifndef _TSHARKS_H_
#define _TSHARKS_H_

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
class TShark: public TDynamicObject
{
public:
	float depth;
	dword time;
};

class TShip : public TDynamicObject
{
public:
	SHIP_BASE *ship;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TSharks : public TDynamicSystem
{
	friend class TShark;
	friend class TShip;
public:
	TSharks();
	virtual ~TSharks();

	dword ProcessMessage(long _code, MESSAGE & message);
	void Init();
	void Realize(dword _dTime);
	void Execute(dword _dTime);

private:
	void LoadSettings();

	VDX8RENDER *renderService;
	ENTITY_ID  sharkModel;
	TShark *sharks[SHARK_COUNT];
	TShip  *ships[SHARK_MAX_SHIPS];
	long shipsCount;
	long sharksCount;
	TDynamicObject cameraObject;

	float maxDistance;
	
	bool enabled;
	ENTITY_ID seaID;
	SEA_BASE *sea;
};

#endif // !defined
