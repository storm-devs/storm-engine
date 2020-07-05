#ifndef SEA_LOCATOR_SHOW_HPP
#define SEA_LOCATOR_SHOW_HPP

#include "AIHelper.h"

class SeaLocatorShow : public ENTITY
{
	ATTRIBUTES	* pALocators;
	bool		bShow;
	float		fScale;

	float		fWidth, fHeight;
	CMatrix		view,mtx;

	struct SphVertex
	{
		CVECTOR v;
		dword	c;
	};

	dword		sphereNumTrgs;
	SphVertex * sphereVertex;

	void		CreateSphere();

	bool		isLocator(ATTRIBUTES * pA);
	CVECTOR		GetLocatorPos(ATTRIBUTES * pA);
	float		GetLocatorAng(ATTRIBUTES * pA);
	float		GetLocatorRadius(ATTRIBUTES * pA);
	char		* GetRealLocatorName(ATTRIBUTES * pA);
	char		* GetLocatorName(ATTRIBUTES * pA);
	char		* GetLocatorGroupName(ATTRIBUTES * pA);

	void		PrintLocator(ATTRIBUTES * pA);
	void		ProcessLocators(ATTRIBUTES * pA);

public:
	SeaLocatorShow();
	~SeaLocatorShow();

	bool Init();
	void SetDevice();

	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
	
	void ProcessMessage(dword iMsg,dword wParam,dword lParam);
	dword _cdecl ProcessMessage(MESSAGE & message);

	dword AttributeChanged(ATTRIBUTES *pAttribute);
};

#endif
