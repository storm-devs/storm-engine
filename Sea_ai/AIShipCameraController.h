#ifndef AISHIPCAMERACONTROLLER_HPP
#define AISHIPCAMERACONTROLLER_HPP

class AIShip;
class AIShipCameraController
{
private:
	ATTRIBUTES		* pTargetAPointer;
	dword			dwTarget;
	float			fDelta;
	long			iCrosshairTex;
	bool			bCameraOutside;

	dword			Colors[4];

	dword			dwSubTexturesX, dwSubTexturesY;

	AIShip			* pOurAIShip;
	ATTRIBUTES		* pACrosshair;

	bool			isCameraOutside() { return bCameraOutside; };

public:
	void Realize(float fDeltaTime);
	void Execute(float fDeltaTime);

	bool Fire();
	bool Init();

	AIShipCameraController(AIShip *);
	~AIShipCameraController();

	void	SetAIShip(AIShip *pShip) { pOurAIShip = pShip; }
	AIShip	*GetAIShip() { return pOurAIShip; }

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif