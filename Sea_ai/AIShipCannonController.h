#ifndef AISHIPCANNONCONTROLLER_HPP
#define AISHIPCANNONCONTROLLER_HPP

#define INVALID_BORT_INDEX		0xFFFFFFFF

class AIShip;
class VAI_INNEROBJ;
// ============================================================================
// master class AIShipCannonController
// Contain base virtual functions 
// ============================================================================
class AIShipCannonController
{
private:
	AIShip			* pOurAIShip;

	ATTRIBUTES		* pAShip;

	bool			bReload;						// we must start reload at next frame
	bool			bNotEnoughBalls;				// if we haven't enough balls
	bool			bTempFlag;

	RS_RECT			rs;

private:
	struct AISHIP_BORT
	{
		string				sName;						// bort name(for compare with label name)
		float				fFireZone;					// bort fire angle
		float				fFireAngMin, fFireAngMax;	// bort height fire angle
		float				fFireDir;					// bort fire direction
		float				fOurBortFireHeight;			// bort fire height
		float				fChargePercent;				// Recharge percent
		float				fCosFireZone;

		dword				dwNumDamagedCannons;

		// each frame parameters
		float				fSpeedV0;				// current fire speed
		float				fMaxFireDistance;		// maximum fire distance
		//CVECTOR				vAngleVectors[2];		// fire cone (2d(x,z))
		CVECTOR				vDirection;

		array<AICannon>		aCannons;				// cannons container for this bort 

		void ClearCharge() { fChargePercent = 0.0f; };
		bool isCharged() { return fChargePercent >= 1.0f; };
		bool isBortDamaged() { return dwNumDamagedCannons == aCannons.Size(); }
		AISHIP_BORT() : aCannons(_FL_, 8), sName(8)
		{
			ClearCharge();
			fOurBortFireHeight = 0.0f;
		};
		operator == (const char *pStr)
		{
			Assert(pStr && pStr[0]);
			return (stricmp(sName.GetBuffer(),pStr)==0);
		}
	};

	// borts container
		array<AISHIP_BORT>	aShipBorts;

	// update borts parameters
		bool	UpdateParameters();			

		bool	ScanShipForCannons();

		bool	Fire2Position(dword dwBort, CVECTOR & vFirePos, float fFireHeight);
	
		float	GetSpeedV0();

public:
		dword	GetCannonsNum();

		float	GetFireDistance(bool bMaxFireDistance);

	// bort section
		float	GetBortHeightAngle(long iBortIndex);
		bool	isCanFireBort(dword dwBort, CVECTOR & vFirePos, float * pfZapasDistance = null);
		dword	GetFirstFireBort(CVECTOR & vFirePos, float * pfZapasDistance = null);
		bool	isHaveEnoughtBallsForBort(dword dwBortIdx);
		dword	GetNextFireBort(dword dwPrevBort, CVECTOR & vFirePos, float * pfZapasDistance = null);
		CVECTOR	GetBortDirection(dword dwBort);
		dword	GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance);
		CVECTOR GetFirePos(CVECTOR & vFireDir);
		CVECTOR GetFirePos(CVECTOR & vFireDir, float fDistance);
		dword	GetBortIntactCannonsNum(dword dwBortIdx);

	// fire test
		bool	isCanFirePos(CVECTOR & vFirePos);		// is we can fire to position
		bool	isCanFire(AIShip * pEnemy);				// is we can fire to enemy ship	
		bool	isCanFire(CVECTOR & vCamDir);			// is we can fire to camera direction

	// fire section
		bool	Fire(CVECTOR & vFirePos);							// fire to position
		bool	Fire(AIShip * pEnemy);								// fire to ship
		bool	Fire(CVECTOR & vFireCamPos, CVECTOR & vFireDir);	// manual fire

	// reload section
		void	Unload();							// unload cannons 
		void	Reload();							// set flag to reload

	// temp
		void	AddTrg(CVECTOR * pVerts, dword dwColor);
	// temp

	// Cannon boom check
		void	CheckCannonsBoom(float fTmpCannonDamage, const CVECTOR & vPnt);
        void	ResearchCannons(); // boal 08.08.06 метод пересчета орудий на корабле
        
	// execute/realize section
		void Execute(float fDeltaTime);
		void Realize(float fDeltaTime);

	// init section
		bool	Init(ATTRIBUTES * _pAShip);

		void	SetAIShip(AIShip * pShip) { pOurAIShip = pShip; }
		AIShip	*GetAIShip() { return pOurAIShip; }

	// default constructor/destructor
		AIShipCannonController(AIShip *);
		~AIShipCannonController();

	static float fMaxCannonDamageDistance;
	static float fMaxCannonDamageRadiusPoint;

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif