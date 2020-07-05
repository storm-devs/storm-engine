#ifndef _TBUTTERFLIES_H_
#define _TBUTTERFLIES_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"
#include "..\common_h\collide.h"
#include "AnimalsDefines.h"
#include "TButterfly.h"
#include "TIVBufferManager.h"

#define Y_REDEFINE_TIME		250

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TButterflies
{
public:
	TButterflies();
	virtual ~TButterflies();

	dword ProcessMessage(long _code, MESSAGE & message);
	void Init();
	void Realize(dword _dTime);
	void Execute(dword _dTime);

private:
	void LoadSettings();

	VDX8RENDER *renderService;
	COLLIDE *collide;
	VIDWALKER *walker;
	TIVBufferManager *ivManager;
	ENTITY_ID  butterflyModel;
	TButterfly butterflies[BUTTERFLY_COUNT];
	long butterfliesCount;

	float maxDistance;
	
	bool enabled;
	long yDefineTime;
	long texture;
};

#endif // !defined
