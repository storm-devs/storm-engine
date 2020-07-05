#ifndef _NETSHIP_H_
#define _NETSHIP_H_

#include "..\..\common.h"

#include "FirePlace.h"
#include "ShipLights.h"

#define DELTA_TIME(x)			((x) * 0.001f)
#define DELTA_TIME_ROTATE(x)	((x) * 1.0f / 10.0f)

#define MAX_STRENGTH		16
#define STRENGTH_MAIN		0
#define RESERVED_STRENGTH	1

#define MAX_KEEL_POINTS		5

class NetShipCannonController;
class NetTouch;
class NetCannon;
class NetShip : public SHIP_BASE
{
protected:

// struct section
struct mast_t
{
	NODE		* pNode;				// node pointer in model
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
	float	fY;
	bool	bUnderWater;
};

struct ship_curve_t
{
	Curve5	curve;
	long	iTime;
	CVECTOR	vTime[5];
};

struct can_fire_t
{
	NetShip		* pShip;
	NetCannon	* pFortCannon;
	float		fDistance;

	bool operator < (can_fire_t & other) { return fDistance < other.fDistance; };
};

	long			iLastMsgServerTime;
	ship_curve_t	curvePosition;
	ship_curve_t	curveSpeed;

// init parameters
	string			sShipName;
	long			iShipPriority;

	NetTouch		* pNetTouch;

// entity_id section
	ENTITY_ID		model_id, sphere[36];
	ENTITY_ID		sail_id, rope_id, flag_id, cannon_id,
					vant_id, touch_id, sea_id, blots_id;

	static SEA_BASE			* pSea;
	static ISLAND_BASE		* pIsland;

// Ships lights static
	IShipLights			* pShipsLights;

// Fire places 
	array<FirePlace>	aFirePlaces;

// temporary used
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
	array<dword>	aMastFalls;
	array<NetShip*>	aShips;
	bool			bShip2Strand;
	bool			bMounted;
	bool			bKeelContour;
	CVECTOR			vOldAng, vOldPos;
	bool			bServerShip;

	float			fRockingY, fRockingAZ, fRockingFactor;

	// fast turn perk parameters
	bool			bPerkTurnActive;
	float			fInitialPerkAngle, fResultPerkAngle;

	STRENGTH		Strength[MAX_STRENGTH];

	DTimer			dtMastTrace, dtUpdateParameters;

	// cannons
	NetShipCannonController * pCannonController;

// executed functions
	CVECTOR		ShipRocking(float dtime);				// 
	bool		ApplyStrength(float dtime, bool bCollision);
	bool		CalculateNewSpeedVector(CVECTOR *Speed, CVECTOR *Rotate);
	float		GetImmersion();						//
	void		CheckShip2Strand(float fDeltaTime);							
	void		MastFall(mast_t * pM);

	CMatrix		UpdateModelMatrix();
	
	void		UpdateCurves(long iMsgServerTime, const CVECTOR & vPos, const CVECTOR & vSpeed);

// init section
	void		ScanShipForFirePlaces();
	void		LoadPositionFromAttributes();
	bool		IsCanPlace(CVECTOR & vNewPos);
	bool		LoadShipParameters();
	void		CalcRealBoxSize();

	void		SetDead();
	bool		isVisible() { return bVisible; };

public:

	virtual ~NetShip();
	NetShip();

	float		GetMaxSpeedZ();
	float		GetMaxSpeedY();
	long		AddStrength(STRENGTH *strength);
	bool		DelStrength(long iIdx);
	
	BOOL		BuildContour(CVECTOR * vContour, long &iNumVContour);
	bool		BuildMasts();

	bool		Move(DWORD DeltaTime, bool bCollision);
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

	virtual void	SetFixedSpeed(bool bSetFixed, float fFixedSpeed) {};

// inherit functions COLLISION_OBJECT
	virtual float Trace(const CVECTOR &src, const CVECTOR &dst);
	virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) { return false; };

	virtual const char * GetCollideMaterialName() { return 0; };
	virtual bool GetCollideTriangle(TRIANGLE &triangle) { return false; };

// inherit functions CANNON_TRACE_BASE
    float	Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst);

	ATTRIBUTES *	AP();

// inherit functions VAI_OBJBASE
	CMatrix *	GetMatrix();
	void		SetMatrix(CMatrix & mtx);
	MODEL *		GetModel();
	ENTITY_ID	GetModelEID();
	CVECTOR		GetPos();
	CVECTOR		GetAng();
	CVECTOR		GetBoxSize();

	void		SetPos(CVECTOR & vNewPos);

	bool		Mount();

	bool		isDead() { return bDead; };

// inherit functions ENTITY
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE & message);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);

	void Save(CSaveLoad * pSL) {};
	void Load(CSaveLoad * pSL) {};	
};

#endif