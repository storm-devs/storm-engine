#ifndef _SEAFOAM_H_
#define _SEAFOAM_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\island_base.h"
#include "..\common_h\collide.h"

#define SHOW_OBJECT_TIME 1000
#define MAX_SHIPS        32

enum tTelescopeStatus
{
	INACTIVE,
	MAKING_ACTIVE,
	ACTIVE,
	MAKING_INACTIVE
};

struct tVisualizationInfo
{	// type, name, nation, damage, speed, crew, cannon
	char strings[7][128];
	int  shownItems;
};

struct tTelescopeConfig
{
	char texture[256];
	float zoom;
	int activateTime;
	int updateTiming;
	int nShownItems;
};

struct tShipInfo
{
	//GEOS::INFO hullInfo;
	SHIP_BASE *ship;
	MODEL *shipModel;
	bool enabled;
	bool myShip;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TELESCOPE: public ENTITY
{
public:
	TELESCOPE();
	virtual ~TELESCOPE();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);

private:
	void HandleStatusChange(dword _dTime);
	void SetActive(bool _active);
	void HandleTargetTest(dword _dTime);
	void SetType (const tTelescopeConfig &config);
	void VisualizeInfo();

	VDX8RENDER *renderer;

	tTelescopeStatus status;
	tVisualizationInfo visualizationInfo;
	float originalPerspective;
	dword changeTime;

	long mainTextureID;
	RS_SPRITE mainSprite[4];

	COLLIDE *collide;
	VIDWALKER *shipWalker;
	bool entityFound;
	MODEL *islandModel;

	float zoomK;
	dword shipClassID;
	dword changeStatusTime, updateTime, idleTime;
	VDATA *nationsArray, *shipTypesArray, *goodsArray;
	ISLAND_BASE *island;
	ENTITY_ID fortID;
	VAI_OBJBASE * fort;

	tShipInfo shipInfo[MAX_SHIPS];
	int shipsCount;
protected:
	void HandleAutoAim();
};

#endif