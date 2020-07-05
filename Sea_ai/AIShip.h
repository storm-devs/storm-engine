#ifndef SEA_SHIP_AI_HPP
#define SEA_SHIP_AI_HPP

#include "AICannon.h"
#include "AIShipMoveController.h"
#include "AIShipTaskController.h"
#include "AIShipCannonController.h"
#include "AIShipCameraController.h"
#include "AIShipTouchController.h"
#include "AIShipRotateController.h"
#include "AIShipSpeedController.h"
#include "..\common_h\Ship_Base.h"
//#include "AITemplate.h"
#include "AIFlowGraph.h"
#include "AIHelper.h"

enum AI_OBJTYPE
{
	AIOBJ_SHIP_WAR = 0,
	AIOBJ_SHIP_TRADE,
	AIOBJ_SHIP_BOAT,
	AIOBJ_FORT,
	AIOBJ_UNKNOWN = 0x7FFFFFFF
};

// ============================================================================
// virtual AI Class
// Contain base virtual functions 
// ============================================================================

class VAI_INNEROBJ : public AIAttributesHolder
{
protected:
	AI_OBJTYPE		ObjType;
	bool			bDead;

public:
	VAI_INNEROBJ() { pACharacter = null; bDead = false; ObjType = AIOBJ_UNKNOWN; };
	virtual ~VAI_INNEROBJ() {};

	// check for fire possibility
	virtual bool		isCanFire(const CVECTOR & vFirePos) const { return false; };
	// set / get type of AI object
	virtual void		SetObjType(AI_OBJTYPE _ObjType) { ObjType = _ObjType; };
	virtual AI_OBJTYPE	GetObjType() const { return ObjType; };

	// enemy / friend / neutral / main character
	virtual bool isMainCharacter() { return GetACharacter()->GetAttributeAsDword("MainCharacter") != 0; };
	virtual bool isFriend(ATTRIBUTES * pAOtherCharacter) const
	{
		if (isDead() || isDead(pAOtherCharacter)) return true;
		return Helper.isFriend(GetACharacter(),pAOtherCharacter);
	}
	virtual bool isEnemy(ATTRIBUTES * pAOtherCharacter) const
	{ 
		if (isDead() || isDead(pAOtherCharacter)) return false;
		return Helper.isEnemy(GetACharacter(),pAOtherCharacter);
	};
	virtual bool isNeutral(ATTRIBUTES * pAOtherCharacter) const
	{ 
		if (isDead() || isDead(pAOtherCharacter)) return false;
		return Helper.isNeutral(GetACharacter(),pAOtherCharacter); 
	};
	virtual bool isFriend(const VAI_INNEROBJ & OtherAIObj) const { return isFriend(OtherAIObj.GetACharacter()); };
	virtual bool isEnemy(const VAI_INNEROBJ & OtherAIObj) const { return isEnemy(OtherAIObj.GetACharacter()); };
	virtual bool isNeutral(const VAI_INNEROBJ & OtherAIObj) const { return isNeutral(OtherAIObj.GetACharacter()); };
	virtual bool isDead() const { return bDead;	};
	virtual bool isDead(ATTRIBUTES * pACharacter) const
	{ 
		VAI_INNEROBJ * pAIObj = AIHelper::FindAIInnerObj(pACharacter); Assert(pAIObj);
		return pAIObj->isDead();
	};

	virtual bool SetDead(bool bNewDead) { bool bOldDead = bDead; bDead = bNewDead; return bOldDead; };

	bool operator == (const ATTRIBUTES * pOtherAICharacter) { return (GetACharacter() == pOtherAICharacter); };
	bool operator == (const VAI_INNEROBJ & OtherAIObj) { return GetACharacter() == OtherAIObj.GetACharacter(); };

	virtual void		ReleasePoint(VAI_INNEROBJ *) {};
	virtual CVECTOR		GetAttackPoint(VAI_INNEROBJ *) = 0;
	virtual CVECTOR		GetFollowPoint(VAI_INNEROBJ *) = 0;
	virtual CVECTOR		GetAbordagePoint(VAI_INNEROBJ * pOtherObj) = 0;

	virtual float		GetMaxFireDistance() = 0;
	virtual float		GetMinFireDistance() = 0;

	virtual float		GetDistance(const VAI_INNEROBJ & OtherAIObj) const { return sqrtf(~(GetPos() - OtherAIObj.GetPos())); };
	virtual float		GetDistance(const CVECTOR & vOtherPos) const { return sqrtf(~(GetPos() - vOtherPos)); };
	virtual CVECTOR		GetBoxSize() const { return 0.0f; };
	virtual CVECTOR		GetPos() const { return 0.0f; };
	virtual CVECTOR		GetAng() const { return 0.0f; };
	virtual void		SetPos(CVECTOR & vNewPos) {};
	virtual void		SetAngleY(float fAngleY) {};
};

// ============================================================================
// master class AIShip
// Contain base virtual functions 
// ============================================================================
class AIShip : public VAI_INNEROBJ
{
private:
	struct can_fire_t
	{
		AIShip		* pShip;
		AICannon	* pFortCannon;
		float		fDistance;

		bool operator < (can_fire_t & other) { return fDistance < other.fDistance; };
	};

	struct AI_POINT
	{
		AI_POINT() {};
		AI_POINT(VAI_INNEROBJ * _pObj) { pObj = _pObj; };
		bool operator == (const AI_POINT & pOtherPoint) const { return pOtherPoint.pObj == pObj; };

		VAI_INNEROBJ	* pObj;		// attached object
		float			fAngle;		// angle of attack
	};

	ATTRIBUTES				* pAShipBase;

	AIShipTaskController	* pTaskController;
	AIShipMoveController	* pMoveController;
	AIShipCannonController	* pCannonController;
	AIShipCameraController	* pCameraController;
	AIShipTouchController	* pTouchController;
	AIShipRotateController	* pRotateController;
	AIShipSpeedController	* pSpeedController;


	DTimer			dtFireTime, dtCheckSituation, dtUpdateSeaAIAttributes;

	string			sGroupName;
	ENTITY_ID		eidShip;

	float fAbordageDistance, fFollowDistance, fAttackDistance;

	static array<can_fire_t>	aShipFire;
	array<AI_POINT>				aFollowPoints, aAttackPoints;

	void	SetSeaAIAttributes(ATTRIBUTES * pAAttr, VAI_INNEROBJ * pObj);

public:

		ATTRIBUTES	* GetAShip() const { return GetACharacter()->FindAClass(GetACharacter(),"Ship"); };

	// some static functions
		static AIShip	* FindShip(ATTRIBUTES * pACharacter);

		static bool		ShipFire(ATTRIBUTES * pACharacter, bool bCameraOutside);

		static void		ShipSetAttack(dword dwPriority, ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2);
		static void		ShipSetRunAway(dword dwPriority, ATTRIBUTES * pACharacter1);
		static void		ShipSetMove(dword dwPriority, ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2);
		static void		ShipSetMove(dword dwPriority, ATTRIBUTES * pACharacter1, CVECTOR & vPnt);
		static void		ShipSetDrift(dword dwPriority, ATTRIBUTES * pACharacter1);
		static void		ShipSetDefend(dword dwPriority, ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2);
		static void		ShipSetBrander(dword dwPriority, ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2);
		static void		ShipSetAbordage(dword dwPriority, ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2);

		static void		ReloadCannons(ATTRIBUTES * pACharacter);

	// Group section
		void	SetGroupName(string & _sGroupName)	{ sGroupName = _sGroupName; };
		string	& GetGroupName() { return sGroupName; };
		
	// AI section
		void	SwapShips(AIShip * pOtherShip);

		bool	isAttack(ATTRIBUTES * pAOtherCharacter);
		void	CheckSituation();
		float	GetPower();
		float	GetShipHP();
		float	GetShipBaseHP();
		float	GetAttackHP(float fDistance);
		float	GetDefendHP();

		virtual	void		GetPrediction(float fTime, CVECTOR * vPos, CVECTOR * vAng);
		// 

	// controllers
		AIShipMoveController	* GetMoveController()	const { return pMoveController; };
		AIShipCannonController	* GetCannonController()	const { return pCannonController; };
		AIShipTaskController	* GetTaskController()	const { return pTaskController; };
		AIShipCameraController	* GetCameraController()	const { return pCameraController; };
		AIShipTouchController	* GetTouchController()	const { return pTouchController; };
		AIShipRotateController	* GetRotateController()	const { return pRotateController; };
		AIShipSpeedController	* GetSpeedController()	const { return pSpeedController; };

	// global ship container, accessible for AIShip, AIGroup and SEA_AI.
		static array<AIShip*>	AIShips;

	// inherit functions from VAI_INNEROBJ
		virtual void		SetACharacter(ATTRIBUTES * pAP);
		virtual void		ReleasePoint(VAI_INNEROBJ *);

		virtual CVECTOR		GetAttackPoint(VAI_INNEROBJ *);
		virtual CVECTOR		GetFollowPoint(VAI_INNEROBJ *);
		virtual CVECTOR		GetAbordagePoint(VAI_INNEROBJ * pOtherObj);
	
		virtual float		GetMaxFireDistance() { return GetCannonController()->GetFireDistance(true); };
		virtual float		GetMinFireDistance() { return GetCannonController()->GetFireDistance(false); };

	// ENTITY_ID functions
		ENTITY_ID		GetShipEID() const { return eidShip; };
		ENTITY			* GetShipPointer() const { return _CORE_API->GetEntityPointer(&GetShipEID()); };
		SHIP_BASE		* GetShipBasePointer() const { return (SHIP_BASE*)GetShipPointer(); };
		VAI_OBJBASE		* GetAIObjShipPointer() const { return (VAI_OBJBASE*)GetShipPointer(); };

	// inherit functions from VAI_OBJBASE
		CMatrix		* GetMatrix()	const { return GetAIObjShipPointer()->GetMatrix(); };
		MODEL		* GetModel()	const { return GetAIObjShipPointer()->GetModel(); };
		ENTITY_ID	GetModelEID()	const { return GetAIObjShipPointer()->GetModelEID(); };

		CVECTOR		GetBoxSize()	const { return GetAIObjShipPointer()->GetBoxSize(); };
		CVECTOR		GetPos()		const { return GetAIObjShipPointer()->GetPos(); };
		CVECTOR		GetAng()		const { return GetAIObjShipPointer()->GetAng(); };

		void		SetPos(CVECTOR & vNewPos)	{ GetAIObjShipPointer()->SetPos(vNewPos); };
		void		SetAngleY(float fAngleY)	{ GetShipBasePointer()->State.vAng.y = fAngleY; }

	// battle section
		virtual bool	isCanFire(const CVECTOR & vFirePos) const;
		virtual bool	Fire(bool bCameraOutside);

	// execute/realize section
		virtual void	Execute(float fDeltaTime);
		virtual void	Realize(float fDeltaTime);

	// default constructor/destructor
		AIShip(AI_OBJTYPE);
		virtual ~AIShip();

		void	Unload();
		void	CreateShip(ENTITY_ID _eidShip, ATTRIBUTES * _pACharacter, ATTRIBUTES * _pAShip, CVECTOR * vInitPos);
		void	CheckStartPosition();
		bool	isCanPlace(CVECTOR vNewPos) const;

		void Save(CSaveLoad * pSL);
		void Load(CSaveLoad * pSL);
};

// ============================================================================
// child of AIShip : war type ship
// ============================================================================
class AIShipWar : public AIShip
{
public:

	AIShipWar();
	virtual ~AIShipWar();
};

// ============================================================================
// child of AIShip : trade type ship
// ============================================================================
class AIShipTrade : public AIShip
{
public:

	AIShipTrade();
	virtual ~AIShipTrade();
};

// ============================================================================
// child of AIShip : boat type ship
// ============================================================================
class AIShipBoat : public AIShip
{
public:

	AIShipBoat();
	virtual ~AIShipBoat();
};

#endif