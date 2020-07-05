#include "AITemplate.h"

array<AITemplate*>	AITemplate::AITemplates;

AITemplate::AITemplate()
{
}

AITemplate::~AITemplate()
{
}

bool AITemplate::CompilePhase(phase_t * pP, ATTRIBUTES * pAPhase)
{
	char		cTemp[256];
	ATTRIBUTES	* pATemp;
	
	// get action
	pP->iAction = pAPhase->GetAttributeAsDword("Action",-1);
	//if (pP->iAction == -1) { _CORE_API->Trace("AITemplate error: Action not found in phase!"); return false; };
	
	// get target
	pP->iTarget = pAPhase->GetAttributeAsDword("Target",-1);
	//if (pP->iTarget == -1) { _CORE_API->Trace("AITemplate error: Target not found in phase!"); return false; };

	pP->iUntil = -1;
	pATemp = pAPhase->FindAClass(pAPhase,"Until");
	if (pATemp)
	{
		pP->iUntil = pATemp->GetAttributeAsDword(null,-1);
		if (pP->iUntil >= 0)
		{
			pP->fUntilState = pATemp->GetAttributeAsFloat("State",-1.0f);
			pP->fUntilDelta = pATemp->GetAttributeAsFloat("Delta",0.0f);
		}
	}

	pATemp = pAPhase->FindAClass(pAPhase,"Num");
	if (pATemp)
	{
		long iNum = pATemp->GetAttributeAsDword(null,-1);
		switch(iNum)
		{
			case AI_SELECT:
				while(true)
				{
					sprintf(cTemp,"%d",pP->aNum.Size()+1);
					ATTRIBUTES * pANum = pATemp->GetAttributeClass(cTemp);
					if (!pANum) break;
					pP->aNum.Add(pANum->GetAttributeAsDword(0,-1));
				}
			break;
			default:
				pP->aNum.Add(iNum);
		};
	}
	return true;
}

bool AITemplate::CompileGroup(group_t * pG, ATTRIBUTES * pAGroup)
{
	ATTRIBUTES	* pAPhase;
	char		cTemp[256];

	while (true)
	{
		sprintf(cTemp,"p%d",pG->aPhases.Size()+1);
		pAPhase = pAGroup->GetAttributeClass(cTemp);
		if (!pAPhase) break;
		phase_t *pP = &pG->aPhases[pG->aPhases.Add()];
		if (!CompilePhase(pP,pAPhase)) return false;
	}	
	return true;
}

bool AITemplate::Compile(ATTRIBUTES * pAAITemplate)
{
	char		cTemp[256];
	string		sTemp;
	ATTRIBUTES	*pATemp, *pAGroup;

	// process main attributes
	Info.iOurNation		= pAAITemplate->GetAttributeAsDword("OurNation",-1);
	Info.iEnemyNation	= pAAITemplate->GetAttributeAsDword("EnemyNation",-1);
	Info.iOurWarNum		= pAAITemplate->GetAttributeAsDword("OurWarNum",0);
	Info.iOurTradeNum	= pAAITemplate->GetAttributeAsDword("OurTradeNum",0);
	Info.iEnemyWarNum	= pAAITemplate->GetAttributeAsDword("EnemyWarNum",0);
	Info.iEnemyTradeNum	= pAAITemplate->GetAttributeAsDword("EnemyTradeNum",0);

	// process actions trade
	pATemp = pAAITemplate->FindAClass(pAAITemplate,"Actions.Trade");
	if (pATemp) while (true)
	{
		sprintf(cTemp,"g%d",Actions.aTrade.Size()+1);
		pAGroup = pATemp->GetAttributeClass(cTemp);
		if (!pAGroup) break;
		group_t *pG = &Actions.aTrade[Actions.aTrade.Add()];
		if (!CompileGroup(pG,pAGroup)) return false;
	}

	// process actions war
	pATemp = pAAITemplate->FindAClass(pAAITemplate,"Actions.War");
	if (pATemp) while (true)
	{
		sprintf(cTemp,"g%d",Actions.aWar.Size()+1);
		pAGroup = pATemp->GetAttributeClass(cTemp);
		if (!pAGroup) break;
		group_t *pG = &Actions.aWar[Actions.aWar.Add()];
		if (!CompileGroup(pG,pAGroup)) return false;
	}

	return true;
}

float AITemplate::GetPercent(info_t * pI)
{
	float fMinus, fPercent = 0.0f;

	if (Info.iOurNation < 0 || Info.iOurNation == pI->iOurNation) fPercent++;
	if (Info.iEnemyNation < 0 || Info.iEnemyNation == pI->iEnemyNation) fPercent++;
	
	// trade ships
	if (Info.iOurTradeNum < 0 || Info.iOurTradeNum == pI->iOurTradeNum) fPercent++;
	else
	{
		fMinus = 1.0f - fabsf((Info.iOurTradeNum - pI->iOurTradeNum) / 10.0f);
		fPercent += (fMinus < 0.0f) ? 0.0f : fMinus;
	}
	if (Info.iEnemyTradeNum < 0 || Info.iEnemyTradeNum == pI->iEnemyTradeNum) fPercent++;
	else
	{
		fMinus = 1.0f - fabsf((Info.iEnemyTradeNum - pI->iEnemyTradeNum) / 10.0f);
		fPercent += (fMinus < 0.0f) ? 0.0f : fMinus;
	}

	// war ships
	if (Info.iOurWarNum < 0 || Info.iOurWarNum == pI->iOurWarNum) fPercent++;
	else
	{
		fMinus = 1.0f - fabsf((Info.iOurWarNum - pI->iOurWarNum) / 10.0f);
		fPercent += (fMinus < 0.0f) ? 0.0f : fMinus;
	}
	if (Info.iEnemyWarNum < 0 || Info.iEnemyWarNum == pI->iEnemyWarNum) fPercent++;
	else
	{
		fMinus = 1.0f - fabsf((Info.iEnemyWarNum - pI->iEnemyWarNum) / 10.0f);
		fPercent += (fMinus < 0.0f) ? 0.0f : fMinus;
	}

	return fPercent / 6.0f;
}

void AITemplate::CompileTemplate(ATTRIBUTES * pAAITemplate)
{
	AITemplate * pT = new AITemplate();
	pT->Compile(pAAITemplate);
	AITemplates.Add(pT);
}
