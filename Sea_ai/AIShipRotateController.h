#ifndef AISHIPROTATECONTROLLER_HPP
#define AISHIPROTATECONTROLLER_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\character.h"
#include "..\common_h\sd2_h\CannonTrace.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "..\engine\program\sea_ai\Script_Defines.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

// ============================================================================
// master class AIShipRotateController
// Contain base virtual functions 
// ============================================================================
class AIShip;
class AIShipRotateController
{
private:
	AIShip			*pOurAIShip;	// our AI ship pointer

	dword			dwRotateNum;
	float			fRotateMode, fRotateTime, fRotateSmooth, fRotate;
	float			fGlobalMultiply;

public:

	void	AddRotate(float _fRotate);
	void	SetGlobalMultiply(float _fGlobalMul);

	// init/execute/realize section
	virtual	bool	Init();
	virtual void	Execute(float);
	virtual void	Realize(float);

	// set our ship pointer
	void	SetAIShip(AIShip *pShip) { pOurAIShip = pShip; }
	AIShip	*GetAIShip() { return pOurAIShip; }

	AIShipRotateController(AIShip *pShip);
	virtual ~AIShipRotateController();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif