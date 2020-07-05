#ifndef AISHIPTOUCHCONTROLLER_HPP
#define AISHIPTOUCHCONTROLLER_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\character.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

class AIShip;
class AIShipTouchController
{
private:
	struct ray_t
	{
		float		fMinDist;
		float		fDist, fEarthDist;
	};
	
	DTimer			dtTouchTime;
	AIShip			* pOurAIShip;					// our AI ship pointer
	array<ray_t>	aTouchRays;						// touch rays
	float			fLeftRaysFree, fRightRaysFree;	// 
	float			fRaySize;						// ray size
	float			fSpeedFactor, fRotateFactor;
	CVECTOR			vBox[5];

public:

		virtual void	TestCollision(AIShip * pOtherShip);
	
	// init/execute/realize section
		virtual	bool	Init();
		virtual void	Execute(float);
		virtual void	Realize(float);

	// 
		float		GetBestRotateDirection();
		CVECTOR		* GetPentagonBox();
		bool		isCollision2D(const CVECTOR & vSrc, const CVECTOR & vDst);

	// get/set our ship pointer
		void		SetAIShip(AIShip *pShip)	{ pOurAIShip = pShip; }
		AIShip		* GetAIShip()				{ return pOurAIShip; }

	// ctor/dtor section
		AIShipTouchController(AIShip * _pAIShip);
		virtual ~AIShipTouchController();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};
#endif