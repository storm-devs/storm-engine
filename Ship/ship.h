#ifndef _SHIP_H_
#define _SHIP_H_

#include <stdio.h>
#include "..\common_h\ship_base.h"
#include "..\common_h\collide.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"
#include "..\common_h\Island_Base.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\character.h"
#include "..\common_h\vdata.h"
#include "..\common_h\mast_msg.h"
#include "..\common_h\geometry.h"

#include "fireplace.h"
#include "..\common_h\ShipLights.h"

#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\dtimer.h"

#include "..\common_h\sd2_h\SaveLoad.h"

#define DELTA_TIME(x)			((x) * 0.001f)
#define DELTA_TIME_ROTATE(x)	((x) * 1.0f / 10.0f)

#define MAX_STRENGTH		16
#define STRENGTH_MAIN		0
#define RESERVED_STRENGTH	1

#define MAX_KEEL_POINTS		5

class SHIP : public SHIP_BASE {
protected:
// struct section
struct mast_t
{
	NODE		* pNode;			// node pointer in model
	CVECTOR		vSrc, vDst;			// src and dest vectors
	long		iMastNum;			// mast number
	bool		bBroken;			// if mast is broken then pNode = 0
	float		fDamage;
};

struct ship_point_t
{
	float	fSpeed;
	float	fGravity;			// acceleration
	float	fExpulsiveForce;
	bool	bUnderWater;

	float	fY;
	float	fA;
	float	fV;
	
};

// init parameters
	static string	sRealizeLayer, sExecuteLayer;
	char			cShipIniName[256];
	long			iShipPriorityExecute, iShipPriorityRealize;
	float			fGravity;

	float			fRockingY, fRockingAZ;

// entity_id section
	ENTITY_ID		model_id, sphere[36];
	ENTITY_ID		sail_id, rope_id, flag_id, cannon_id,
					vant_id, touch_id, sea_id, blots_id;

	static VDX8RENDER		* pRS;
	static SEA_BASE			* pSea;
	static ISLAND_BASE		* pIsland;
	static COLLIDE			* pCollide;
	static VGEOMETRY		* pGS;

	CMatrix				mRoot;
	CVECTOR				vSpeed, vSpeedsA;
	float				fMinusVolume;
	float				fXOffset, fZOffset;

	bool				bModelUpperShip;
	MODEL				* pModelUpperShip;
	float				fUpperShipAY, fUpperShipY;

// Ships lights 
	IShipLights			* pShipsLights;

// Fire places 
	array<FirePlace>	aFirePlaces;

// Sound section

// temporary used
	bool			bMassaShow;
	long			uniIDX;
	bool			bUse;
	ship_point_t	ShipPoints[16][16];

// executed parameters
	CVECTOR			vSpeedAccel;
	ship_t			SP;
	CVECTOR			vPos, vAng;
	float			fSailState;
	float			fWaterLine;
	bool			bDead, bVisible;
	CVECTOR			vDeadDir, vCurDeadDir;
	CVECTOR			vKeelContour[MAX_KEEL_POINTS];
	long			iNumMasts;
	mast_t			* pMasts;
	bool			bShip2Strand;
	bool			bMounted;
	bool			bKeelContour;
	CVECTOR			vOldAng, vOldPos;
	bool			bSetFixed;
	float			fFixedSpeed;

	// fast turn perk parameters
	bool			bPerkTurnActive;
	float			fInitialPerkAngle, fResultPerkAngle;

	STRENGTH		Strength[MAX_STRENGTH];

	DTimer			dtMastTrace, dtUpdateParameters;

// executed functions
	CVECTOR		ShipRocking(float fDeltaTime);
	BOOL		ApplyStrength(float dtime, BOOL bCollision);
	BOOL		CalculateNewSpeedVector(CVECTOR *Speed, CVECTOR *Rotate);
	void		CalculateImmersion();						//
	void		CheckShip2Strand(float fDeltaTime);							
	void		MastFall(mast_t * pM);

	CMatrix		UpdateModelMatrix();
	void		RecalculateWorldOffset();
	
// init section
	void		ScanShipForFirePlaces();
	void		LoadPositionFromAttributes();
	BOOL		LoadShipParameters();
	void		CalcRealBoxSize();

	void		SetDead();
	bool		isDead() { return bDead; };
	bool		isVisible() { return bVisible; };

public:

	virtual ~SHIP();
	SHIP();

	float		GetMaxSpeedZ();
	float		GetMaxSpeedY();
	long		AddStrength(STRENGTH *strength);
	bool		DelStrength(long iIdx);
	
	BOOL		BuildContour(CVECTOR *vContour,long &iNumVContour);
	bool		BuildMasts();

	BOOL		Move(DWORD DeltaTime, BOOL bCollision);
	BOOL		TouchMove(DWORD DeltaTime, TOUCH_PARAMS *pTPOld, TOUCH_PARAMS *pTPNew);

	void		LoadServices();

// inherit functions SHIP_BASE
	bool			bSetLightAndFog;
	dword			dwSaveAmbient, dwSaveFogColor;
	D3DLIGHT8		saveLight;

	virtual void	SetLightAndFog(bool bSetLight);
	virtual void	RestoreLightAndFog();
	
	virtual void	SetSpeed(float fSpeed);
	virtual float	GetSpeed();

	virtual void	SetRotate(float fRotSpeed);
	virtual float	GetRotate();

	virtual float	GetBrakingDistance(float * pfTime = 0);
	virtual float	GetRotationAngle(float * pfTime = 0);

	virtual float 	GetCurrentSpeed();

	virtual void	SetLights();
	virtual void	UnSetLights();
	virtual void	Fire(const CVECTOR & vPos);

	virtual void	SetFixedSpeed(bool bSetFixed, float fFixedSpeed);

// inherit functions COLLISION_OBJECT
	virtual float Trace(const CVECTOR &src, const CVECTOR &dst);
	virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) { return false; };

	virtual const char *GetCollideMaterialName() { return 0; };
	virtual bool GetCollideTriangle(TRIANGLE &triangle) { return false; };

// inherit functions CANNON_TRACE_BASE
    float	Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst);

// inherit functions VAI_OBJBASE
	void		SetACharacter(ATTRIBUTES * pAP);

	CMatrix *	GetMatrix();
	void		SetMatrix(CMatrix & mtx);
	MODEL *		GetModel();
	ENTITY_ID	GetModelEID();
	CVECTOR		GetPos();
	CVECTOR		GetAng();
	CVECTOR		GetBoxSize();

	void		SetPos(CVECTOR & vNewPos);

	bool		Mount(ATTRIBUTES*);

// inherit functions ENTITY
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE & message);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);	
};

#endif