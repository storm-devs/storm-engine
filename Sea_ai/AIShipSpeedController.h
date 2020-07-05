#ifndef AISHIPSPEEDCONTROLLER_HPP
#define AISHIPSPEEDCONTROLLER_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\character.h"
#include "..\common_h\sd2_h\CannonTrace.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "..\engine\program\sea_ai\Script_Defines.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

// ============================================================================
// master class AIShipSpeedController
// Contain base virtual functions 
// ============================================================================
class AIShip;
class AIShipSpeedController
{
private:
	AIShip			* pOurAIShip;	// our AI ship pointer

	dword			dwSpeedNum;
	float			fSpeedSmooth, fSpeed, fSpeedTime, fTopSpeed;
	float			fGlobalMultiply;

public:

	void	TopSpeed(float _fSpeed);
	void	MulSpeed(float _fSpeed);
	void	AddSpeed(float _fSpeed);
	void	SetGlobalMultiply(float _fGlobalMul);

	// init/execute/realize section
	virtual	bool	Init();
	virtual void	Execute(float);
	virtual void	Realize(float);

	// set our ship pointer
	void	SetAIShip(AIShip *pShip) { pOurAIShip = pShip; }
	AIShip	*GetAIShip() { return pOurAIShip; }

	AIShipSpeedController(AIShip *pShip);
	virtual ~AIShipSpeedController();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif