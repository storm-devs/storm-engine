#ifndef SEA_CANNON_AI_HPP
#define SEA_CANNON_AI_HPP

#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "AIBalls.h"
#include "AIHelper.h"

// ============================================================================
// Master class AICannon
// Contain functions and parameters for cannon
// ============================================================================
class AICannon
{
private:
	AIAttributesHolder	* pAHolder;
	ENTITY_ID	eidParent;				// parent for cannon (ship, or fort entity)
	CVECTOR		vPos, vDir;				// Cannon position and direction(relative to parent)
	float		fTime2Action;			// timeout before action(calculated in script)
	float		fTotalTime2Action;		// total timeout before action(calculated in script)(unchangeable)

	float		fSpeedV0;		// saved current ball speed
	CVECTOR		vEnemyPos;

	bool		bEmpty;			// is empty cannon(no balls available)
	float		fDamaged;		// is cannon damaged(if fDamaged >= 100.0f)
	bool		bFired;			// is cannon begin fire process
	bool		bReady2Fire;	// is cannon ready to fire
	bool		bRecharged;		// is cannon in recharge process
	bool		bLoad;			// is cannon must be loaded in next frame
	bool		bCanRecharge;

	float		fMaxFireDistance;

	void	RealFire();		// immediatelly fire!

public:
	enum CANNONTYPE
	{
		CANNONTYPE_NORMAL = 0,
		CANNONTYPE_MORTAR,
		CANNONTYPE_FORCEDWORD = 0x7FFFFFFF
	};

	void SetType(CANNONTYPE Type) { CannonType = Type; };

	AICannon();
	virtual ~AICannon();

	void	Init(AIAttributesHolder	* _pAHolder, const ENTITY_ID & eid, GEOS::LABEL & lbl);

	void	Execute(float fDeltaTime);

	static float	CalcMaxFireDistance(float fFireHeight, float fSpeedV0, float fAngle);

	ENTITY_ID		GetParentEID() const { return eidParent; };
	VAI_OBJBASE		* GetAIObjPointer() const;

	float	CalcHeightFireAngle(float _fSpeedV0, CVECTOR & vOur, CVECTOR & vEnemy);
	bool	Fire(float fSpeedV0, CVECTOR & vFirePos);
	void	Load();
	void	Unload();
	void	Recharge();
	void	QuickRecharge(bool bHaveEnoughBalls);

	float	GetRechargePercent();
	void	AddDamage(float _fDamage) { fDamaged += _fDamage; };
	void	SetDamage(float _fDamage) { fDamaged = _fDamage; };
	float	GetDamage() { return fDamaged; };
	CVECTOR	GetPos();
	CVECTOR	GetDir();
	float	GetDirY();
	float	GetDistance(CVECTOR & vPos) { return sqrtf(~(vPos - GetPos())); };

	void	SetMaxFireDistance(float fMaxFireDistance) { this->fMaxFireDistance = fMaxFireDistance; };

	inline void	SetRechargeEnable(bool bRechargeEnable) { bCanRecharge = bRechargeEnable; }
	inline bool isLoad() { return bLoad; }
	inline bool isEmpty() { return bEmpty; }
	inline bool isReady2Fire() { return (bReady2Fire & (!isDamaged())); }	// CHECK-ME
	inline bool isDamaged() { return fDamaged >= 1.0f; }
	inline bool isFired() { return bFired; }
	inline bool isRecharged() { return bRecharged; }

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL, AIAttributesHolder * _pAHolder, const ENTITY_ID & eid);

private:
	CANNONTYPE	CannonType;
};

#endif