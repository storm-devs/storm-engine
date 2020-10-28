#ifndef _SEAFOAM_H_
#define _SEAFOAM_H_

#include "object.h"
#include "matrix.h"
#include "animation.h"
#include "messages.h"
#include "geos.h"
#include "geometry.h"
#include "dx8render.h"
#include "ship_base.h"
#include "model.h"
#include "geos.h"
#include "sea_base.h"
#include "..\SoundService\VSoundService.h"
#include "SeafoamDefines.h"
#include "seafoam_ps.h"
#include "TCarcass.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct tFoamEmitter
{
	CVECTOR center[TRACE_STEPS_Y];
	CVECTOR meanPoint;
	float k;
	CVECTOR finalPos;
};

struct tShipFoamInfo
{
	tFoamEmitter hull[2][TRACE_STEPS_Z];
	CVECTOR levelStarts[2][TRACE_STEPS_Z];
	GEOS::INFO hullInfo;
	SHIP_BASE *ship;
	TCarcass *carcass[2];
	SEAFOAM_PS *frontEmitter[3];
	MODEL *shipModel;
	TSD_ID sound;
	bool doSplash;
	bool firstSoundPlay;
	bool enabled;
};
 
class SEAFOAM: public ENTITY
{
public:
	SEAFOAM();
	virtual ~SEAFOAM();

	virtual bool Init();
	dword AttributeChanged(ATTRIBUTES * pA);
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);

private:
	void InitializeShipFoam();
	void ReleaseShipFoam();
	void RealizeShipFoam_Particles(tShipFoamInfo &_shipFoamInfo, dword _dTime);
	void RealizeShipFoam_Mesh(tShipFoamInfo &_shipFoamInfo, dword _dTime);
	void CreateTracePoints(tShipFoamInfo *_shipFoamInfo);
	void InterpolateLeftParticle(tShipFoamInfo &_shipFoamInfo, int z, dword _dTime);
	void InterpolateRightParticle(tShipFoamInfo &_shipFoamInfo, int z, dword _dTime);
	void AddShip(ENTITY_ID * pShipEID);

	VDX8RENDER *renderer;
	ENTITY_ID seaID;
	SEA_BASE *sea;
	tShipFoamInfo shipFoamInfo[MAX_SHIPS];
	int shipsCount;
	INIFILE *psIni;
	long carcassTexture;
	bool isStorm;
	VSoundService *soundService;
};

#endif