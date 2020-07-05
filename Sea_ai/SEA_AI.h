#ifndef SEA_AI_HPP
#define SEA_AI_HPP

#include "AIHelper.h"
#include "AIGroup.h"
#include "AIFort.h"
#include "AIBalls.h"
#include "AISeaGoods.h"
#include "LocatorShow.h"

class SEA_AI : public ENTITY
{
private:
	bool		bFirstInit;

	void	AddShip(ENTITY_ID _eidShip, ATTRIBUTES *pCharacter, ATTRIBUTES *pAShip);
	void	SetCompanionEnemy(ATTRIBUTES * pACharacter);

public:
	SEA_AI();
	~SEA_AI();

	bool Init();
	void SetDevice();
	void Save(const char * pName);
	void Load(const char * pName);

	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
	
	void ProcessMessage(dword iMsg,dword wParam,dword lParam);
	dword _cdecl ProcessMessage(MESSAGE & message);

	dword AttributeChanged(ATTRIBUTES *pAttribute);
};

#endif