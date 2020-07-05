#ifndef AISHIPTASKCONTROLLER_HPP
#define AISHIPTASKCONTROLLER_HPP

// ============================================================================
// master class AIShipTaskController
// Contain base virtual functions 
// ============================================================================

class AIShip;
class VAI_INNEROBJ;

class AITask
{
	bool			bActive;

public:
	AITask() : bActive(false) { dwTaskType = AITASK_NONE; pATaskCharacter = null; vTaskPnt = 0.0f; };

	dword			dwTaskType;

	ATTRIBUTES		* pATaskCharacter;
	CVECTOR			vTaskPnt;

	void	SetActive(bool _bActive) { bActive = _bActive; };
	bool	isActive() { return bActive; };

	void Save(CSaveLoad * pSL)
	{
		pSL->SaveDword(bActive);
		pSL->SaveDword(dwTaskType);
		pSL->SaveAPointer("character", pATaskCharacter);
		pSL->SaveVector(vTaskPnt);
	}

	void Load(CSaveLoad * pSL)
	{
		bActive = pSL->LoadDword() != 0;
		dwTaskType = pSL->LoadDword();
		pATaskCharacter = pSL->LoadAPointer("character");
		vTaskPnt = pSL->LoadVector();
	}
};

class AIShipTaskController
{
private:
	float			fZapasDistance;

	AITask			Primary,Secondary;

	AITask			* GetTask(dword dwPriority) { return (dwPriority==PRIMARY_TASK) ? &Primary : &Secondary; };
	VAI_INNEROBJ	* GetCurrentTaskAIObj() { return AIHelper::FindAIInnerObj(GetCurrentTask()->pATaskCharacter); };
	bool			isCurrentTaskSecondary() { return Secondary.isActive(); };
	bool			isCurrentTaskPrimary() { return !Secondary.isActive(); };
	
	AIShip			* pOurAIShip;	// our AI ship pointer

	void DoAttackRotate();
	void DoTask(float fDeltaTime);
	bool CheckCurrentTaskComplete();
	void SetDestinationPoint(CVECTOR vDestPnt);
	void FindRunAwayPoint();

public:

	bool isAttack(ATTRIBUTES * pAOtherCharacter);

	bool Init() { return true; };

	AITask	* GetCurrentTask() { return (Secondary.isActive()) ? &Secondary : &Primary; };

	void SetNewTask(dword dwPriority, dword _dwNewTaskType, CVECTOR & vPnt);
	void SetNewTask(dword dwPriority, dword _dwNewTaskType, ATTRIBUTES * _pATaskCharacter);

	// controller execute 
	void	Execute(float);

	// set our ship pointer
	void	SetAIShip(AIShip *pShip) { pOurAIShip = pShip; };
	AIShip	* GetAIShip() { return pOurAIShip; };

	AIShipTaskController(AIShip *pShip);
	virtual ~AIShipTaskController();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif