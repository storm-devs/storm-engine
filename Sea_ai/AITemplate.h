#ifndef SEA_AI_TEMPLATE_HPP
#define SEA_AI_TEMPLATE_HPP

#include "..\common_h\vmodule_api.h"
#include "templates\array.h"
#include "templates\string.h"
#include "..\engine\program\sea_ai\Script_Defines.h"

class AITemplate
{
public:
	struct info_t
	{
		float			fPower, fPowerDelta;
		long			iOurNation,iEnemyNation;
		long			iOurWarNum,iOurTradeNum;
		long			iEnemyWarNum,iEnemyTradeNum;
	};

	struct phase_t
	{
		long			iAction;
		long			iTarget;
		long			iUntil;
		float			fUntilState,fUntilDelta;

		array<dword>	aNum;
	};

	struct group_t
	{
		array<phase_t>	aPhases;
	};

	struct action_t
	{
		array<group_t>		aTrade;
		array<group_t>		aWar;
	};

	action_t		Actions;
	info_t			Info;

	static array<AITemplate*>	AITemplates;

	bool	Compile(ATTRIBUTES * pAAITemplate);
	float	GetPercent(info_t * pI);

	AITemplate();
	~AITemplate();

	// static functions
		static void		 CompileTemplate(ATTRIBUTES * pAAITemplate);


private:

	bool	CompileGroup(group_t * pG, ATTRIBUTES * pAGroup);
	bool	CompilePhase(phase_t * pP, ATTRIBUTES * pAPhase);

};

#endif