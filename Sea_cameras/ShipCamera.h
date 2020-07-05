#ifndef SEA_SHIP_CAMERA_HPP
#define SEA_SHIP_CAMERA_HPP

#include "..\common_h\dx8render.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\model.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\Island_Base.h"
#include "Common_Camera.h"

#define MIN_DIST	70.f
#define MAX_DIST	120.f
#define MIN_XANG	PI / 6
#define MAX_XANG	(PI / 2 + PI / 12)
#define CAMERA_HEIGHT	1.0f
#define CAMERA_ANGLE	PI / 18

class SHIP_CAMERA : public COMMON_CAMERA
{
private:
	long		iLockX, iLockY;

	float		fMinHeightOnSea, fMaxHeightOnShip;
	float		fDistance, fMaxDistance, fMinDistance;
	float		fDistanceDlt, fDistanceInertia;
	float		fMinAngleX, fMaxAngleX;
	float		fAngleXDlt, fAngleXInertia;
	float		fAngleYDlt, fAngleYInertia;
	float		fSensivityDistance, fSensivityAzimuthAngle, fSensivityHeightAngle;
	float		fSensivityHeightAngleOnShip;
	float		fInvertMouseX, fInvertMouseY;
	CVECTOR		vCenter, vAng;
	float		fModelAy;
	long		shipcode;

	//long minHeightOnSea,maxHeightOnSea;



private:
	SEA_BASE	*	pSea;
	ISLAND_BASE *	pIsland;
	long			lIlsInitCnt;
	VDX8RENDER	*	pRS;

	void Move(float fDeltaTime);

	bool Init();
	void SetDevices();
	void Execute(dword dwDeltaTime);
	void Realize(dword dwDeltaTime);

	void SetCharacter(ATTRIBUTES *_pACharacter);

	dword AttributeChanged(ATTRIBUTES *);
	void ShipsCollision(CVECTOR & pos);
	bool IslandCollision(CVECTOR & pos);

public:
	SHIP_CAMERA();
	virtual ~SHIP_CAMERA();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif