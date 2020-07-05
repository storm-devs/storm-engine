#include "AIGroup.h"
#include "AIFort.h"

array<AIGroup*>	AIGroup::AIGroups(_FL_, 4);
float AIGroup::fDistanceBetweenGroupShips = 250.0f;

AIGroup::AIGroup(const char * pGroupName) : aGroupShips(_FL_, 4)
{
	Assert(pGroupName);

	bFirstExecute = true;
	pACommander = null;

	sGroupName = pGroupName;
	
	dtCheckTask.Setup(FRAND(4.0f), 2.0f, 4.0f);
}

AIGroup::~AIGroup()
{
	dword i;
	for (i=0;i<aGroupShips.Size();i++) DELETE(aGroupShips[i]);
	aGroupShips.DelAll();
}

void AIGroup::AddShip(ENTITY_ID eidShip, ATTRIBUTES * pACharacter, ATTRIBUTES * pAShip)
{
	ATTRIBUTES * pAMode = pACharacter->FindAClass(pACharacter, "Ship.Mode");
	AIShip * pShip = null;
	if (pAMode)
	{
		if (string("war") == pAMode->GetThisAttr()) pShip = NEW AIShipWar();
		if (string("trade") == pAMode->GetThisAttr()) pShip = NEW AIShipTrade();
		if (string("boat") == pAMode->GetThisAttr()) pShip = NEW AIShipBoat();
	}
	if (!pShip) pShip = NEW AIShipWar();

	CVECTOR vShipPos = CVECTOR(vInitGroupPos.x, vInitGroupPos.y, vInitGroupPos.z) - (aGroupShips.Size() * AIGroup::fDistanceBetweenGroupShips) * CVECTOR(sinf(vInitGroupPos.y), 0.0f, cosf(vInitGroupPos.y));

	pShip->CreateShip(eidShip, pACharacter, pAShip, &vShipPos);
	pShip->SetGroupName(GetName());

	AIShip::AIShips.Add(pShip);		// add to global array
	aGroupShips.Add(pShip);			// add to local group array

	//pACharacter->Dump(pACharacter, 0);
	Helper.AddCharacter(pACharacter, GetCommanderACharacter());

	//pShip->CheckStartPosition();
}

bool AIGroup::isMainGroup()
{
	for (dword i=0;i<aGroupShips.Size();i++)
		if (aGroupShips[i]->isMainCharacter()) return true;

	return false;
}

AIShip * AIGroup::GetMainShip()
{
	AIShip * pShip = AIShip::FindShip(GetCommanderACharacter());
	//return AIShip::FindShip(GetCommanderACharacter());
	/*if (aGroupShips.Size() == 0) return null;
	if (isMainGroup()) 
		for (dword i=0;i<aGroupShips.Size();i++) if (aGroupShips[i]->isMainCharacter()) return aGroupShips[i];
*/
	return (pShip) ? pShip : aGroupShips[0];
}

void AIGroup::Execute(float fDeltaTime)
{
	//if (isDead()) return;

	if (bFirstExecute)
	{
		if (!sLocationNearOtherGroup.IsEmpty())
		{
			float fNewAng = FRAND(PIm2);
			AIGroup * pG = FindGroup(sLocationNearOtherGroup); 
			CVECTOR vNewGroupPos = pG->vInitGroupPos + ((100.0f + FRAND(200.0f)) * CVECTOR(sinf(fNewAng), 0.0f, cosf(fNewAng)));
			if (pG) for (dword i=0; i<aGroupShips.Size(); i++)
			{
				aGroupShips[i]->SetPos(vNewGroupPos);
				aGroupShips[i]->CheckStartPosition();
			}
		}
		
		if (isMainGroup()) 
		{
			for (dword i=0;i<aGroupShips.Size();i++) if (!aGroupShips[i]->isMainCharacter())
				aGroupShips[i]->GetTaskController()->SetNewTask(PRIMARY_TASK, AITASK_DEFEND, GetCommanderACharacter());
		}

		bFirstExecute = false;
	}
	/*if (!isMainGroup()) 
	{
		if (sCommand == "runaway")
			for (dword i=0; i<aGroupShips.Size(); i++) if (!aGroupShips[i]->isMainCharacter())
				aGroupShips[i]->GetTaskController()->SetNewTask(PRIMARY_TASK, AITASK_RUNAWAY, null);

		if (sCommand == "move")
			for (dword i=0; i<aGroupShips.Size(); i++) if (!aGroupShips[i]->isMainCharacter())
			{
				CVECTOR vDir = CVECTOR(vInitGroupPos.x, 0.0f, vInitGroupPos.z) + 20000.0f * CVECTOR(sinf(vInitGroupPos.y),0.0f,cosf(vInitGroupPos.y));
				aGroupShips[i]->GetTaskController()->SetNewTask(PRIMARY_TASK, AITASK_MOVE, vDir);
			}

		sCommand = "";
	}*/

	if (!isDead() && dtCheckTask.Update(fDeltaTime))
		api->Event(GROUP_CHECKTASKEVENT,"s",(char*)GetName().GetBuffer());

	if (!isMainGroup())
	{
		float fMinimalSpeed = 1e+10f;	
		for (dword i=0; i<aGroupShips.Size(); i++)
		{
			float fCurSpeed = aGroupShips[i]->GetShipBasePointer()->GetCurrentSpeed();
			if (fCurSpeed < fMinimalSpeed) fMinimalSpeed = fCurSpeed;
		}

		bool bSetFixedSpeed = sCommand == "move";

		for (dword i=0; i<aGroupShips.Size(); i++)
			aGroupShips[i]->GetShipBasePointer()->SetFixedSpeed(bSetFixedSpeed, fMinimalSpeed);
	}

	for (dword i=0;i<aGroupShips.Size();i++)
		aGroupShips[i]->Execute(fDeltaTime);
}

void AIGroup::Realize(float fDeltaTime)
{
	dword i;

#ifndef _XBOX
	/*if (GetAsyncKeyState('G')<0)
	{
		float fScale = 0.6f;
		long fh = long(AIHelper::pRS->CharHeight(FONT_DEFAULT) * fScale);
		for (i=0;i<AIGroups.Size();i++) if (this==AIGroups[i])
		{
			//char str[256];
			//AIHelper::pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, fScale, 0, 0, 0, i * fh, str);
		}
	}*/
#endif
	for (i=0;i<aGroupShips.Size();i++)
	{
		aGroupShips[i]->Realize(fDeltaTime);
	}
}

bool AIGroup::isDead()
{
	for (dword i=0;i<aGroupShips.Size();i++) if (!aGroupShips[i]->isDead()) return false;
	return true;
}

float AIGroup::GetPower()
{
	float fPower = 0.0f;
	for (dword i=0; i<aGroupShips.Size(); i++) fPower += aGroupShips[i]->GetPower();
	return fPower;
}

AIGroup * AIGroup::CreateNewGroup(const char * pGroupName)
{
	Assert(pGroupName);
	AIGroup * pAIGroup = NEW AIGroup(pGroupName);
	AIGroup::AIGroups.Add(pAIGroup);

	return pAIGroup;
}

AIGroup * AIGroup::FindGroup(ATTRIBUTES * pACharacter)
{
	if (!pACharacter) return null;
	for (dword i=0;i<AIGroup::AIGroups.Size();i++) 
	{
		AIGroup * pGroup = AIGroup::AIGroups[i];
		for (dword j=0;j<pGroup->aGroupShips.Size();j++)
			if (*pGroup->aGroupShips[j] == pACharacter) return pGroup;
	}
	return null;
}

AIGroup * AIGroup::FindGroup(const char * pGroupName)
{
	for (dword i=0;i<AIGroup::AIGroups.Size();i++) if (AIGroup::AIGroups[i]->GetName() == pGroupName) return AIGroup::AIGroups[i];
	return null;
}

AIGroup * AIGroup::FindMainGroup()
{
	for (dword i=0;i<AIGroup::AIGroups.Size();i++) if (AIGroups[i]->isMainGroup()) return AIGroups[i];
	return null;
}

void AIGroup::SailMainGroup(CVECTOR vPos, float fAngle, ATTRIBUTES * pACharacter)
{
	AIGroup * pMG = FindMainGroup(); Assert(pMG);
	AIGroup * pG1 = FindGroup(pACharacter);

	ENTITY_ID	eidSea;
	api->FindClass(&eidSea, "sea", 0);

	for (dword i=0; i<pMG->aGroupShips.Size(); i++)
	{
		AIShip * pAIShip = pMG->aGroupShips[i];

		if (pAIShip->isDead()) continue;

		dword dwTaskType = AITASK_NONE;
		AITask * pTask = pAIShip->GetTaskController()->GetCurrentTask();
		if (pTask) dwTaskType = pTask->dwTaskType;
		
		if (dwTaskType == AITASK_RUNAWAY) continue;
		if (dwTaskType == AITASK_DRIFT) continue;
		if (dwTaskType == AITASK_MOVE) continue;

		pAIShip->SetPos(vPos);
		if (fAngle > -10000.0f)
			pAIShip->SetAngleY(fAngle);
		pAIShip->CheckStartPosition();
		
		// clear foam
		api->Send_Message(eidSea, "lic", MSG_SHIP_CREATE, pAIShip->GetShipEID(), pAIShip->GetShipBasePointer()->State.vPos);
	}
}

AIGroup * AIGroup::FindOrCreateGroup(const char * pGroupName)
{
	AIGroup * pAIGroup = FindGroup(pGroupName);
	return (pAIGroup) ? pAIGroup : CreateNewGroup(pGroupName);
}

void AIGroup::GroupSetMove(const char * pGroupName, CVECTOR & vMovePoint)
{
	AIGroup * pAIGroup = FindOrCreateGroup(pGroupName);
	pAIGroup->sCommand = "move";
	pAIGroup->vMovePoint = vMovePoint;
}

void AIGroup::GroupSetRunAway(const char * pGroupName)
{
	FindOrCreateGroup(pGroupName)->sCommand = "runaway";
}

void AIGroup::GroupSetAttack(AIShip * pS1, AIShip * pS2)
{
	AIGroup * pG1 = FindGroup(pS1->GetGroupName()); Assert(pG1);
	AIGroup * pG2 = FindGroup(pS2->GetGroupName()); Assert(pG2);
	
	pG2->sCommand = "attack";
	pG2->sCommandGroup = pG1->GetName();
}

float AIGroup::GetAttackHP(const char * pGroupName, float fDistance)
{
	AIGroup * pG = FindGroup(pGroupName); Assert(pG);
	return pG->GetMainShip()->GetAttackHP(fDistance);
}

void AIGroup::GroupSetAttack(const char * pGroupName, const char * pGroupAttackingName)
{
	AIGroup * pG = FindOrCreateGroup(pGroupName);
	pG->sCommand = "attack";
	pG->sCommandGroup = pGroupAttackingName;
}

void AIGroup::SetXYZ_AY(const char * pGroupName, CVECTOR vPos, float _fAY)
{
	AIGroup * pAIGroup = FindOrCreateGroup(pGroupName);

	pAIGroup->vInitGroupPos.x = vPos.x;
	pAIGroup->vInitGroupPos.y = _fAY;
	pAIGroup->vInitGroupPos.z = vPos.z;
}

AIShip * AIGroup::ExtractShip(ATTRIBUTES * pACharacter)
{
	for (dword i=0;i<aGroupShips.Size();i++)
	{
		if (*aGroupShips[i] == pACharacter) 
		{
			AIShip * pAIShip = aGroupShips[i];
			aGroupShips.Extract(i);
			return pAIShip;
		}
	}
	return null;
}

ATTRIBUTES * AIGroup::GetCommanderACharacter()
{
	Assert(pACommander); return pACommander;
}

void AIGroup::InsertShip(AIShip * pAIShip)
{
	pAIShip->SetGroupName(sGroupName);

	aGroupShips.Add(pAIShip);			// add to local group array

	Helper.AddCharacter(pAIShip->GetACharacter(), GetCommanderACharacter());
}

void AIGroup::GroupSetType(const char * pGroupName, const char * cGroupType)
{
	AIGroup * pG = FindOrCreateGroup(pGroupName); Assert(pG);
	pG->sGroupType = cGroupType;
}

void AIGroup::GroupHelpMe(const char * pGroupName, AIShip * pMe, AIShip * pEnemy)
{
	AIGroup * pG = FindGroup(pMe->GetACharacter()); Assert(pG);
}

void AIGroup::ShipChangeGroup(ATTRIBUTES * pACharacter, const char * pGroupName)
{
	AIGroup * pGOld = FindGroup(pACharacter);
	if (!pGOld)
	{
		api->Trace("AIGroup::ShipChangeGroup: Can't find group with character id = %s",pACharacter->GetAttribute("id"));
		return;
	}
	AIGroup * pGNew = FindOrCreateGroup(pGroupName);

	AIShip	* pAIShip = pGOld->ExtractShip(pACharacter);
	if (pAIShip) pGNew->InsertShip(pAIShip);
}

void AIGroup::SwapCharactersShips(ATTRIBUTES * pACharacter1, ATTRIBUTES * pACharacter2)
{
	AIGroup * pG1 = FindGroup(pACharacter1);
	AIGroup * pG2 = FindGroup(pACharacter2);

	AIShip	* pAIShip1 = pG1->ExtractShip(pACharacter1);
	AIShip	* pAIShip2 = pG2->ExtractShip(pACharacter2);

	//pAIShip1->SetACharacter(pACharacter2);
	//pAIShip2->SetACharacter(pACharacter1);

	pAIShip1->SwapShips(pAIShip2);

	pG1->InsertShip(pAIShip2);
	pG2->InsertShip(pAIShip1);
}

void AIGroup::SetOfficerCharacter2Ship(ATTRIBUTES * pOfficerCharacter, ATTRIBUTES * pReplacedACharacter)
{
	AIGroup * pG1 = FindMainGroup();
	AIGroup * pG2 = FindGroup(pReplacedACharacter);

	AIShip	* pAIShip1 = pG2->ExtractShip(pReplacedACharacter);
	pAIShip1->SetACharacter(pOfficerCharacter);
	pG1->InsertShip(pAIShip1);
}

void AIGroup::GroupSetCommander(const char * pGroupName, ATTRIBUTES * _pACommander)
{
	AIGroup * pG = FindOrCreateGroup(pGroupName); Assert(pG);

	pG->pACommander = _pACommander;
}

void AIGroup::GroupSetLocationNearOtherGroup(const char * pGroupName, const char * pOtherGroupName)
{
	AIGroup * pG = FindOrCreateGroup(pGroupName); Assert(pG);

	pG->sLocationNearOtherGroup = pOtherGroupName;
}
		
void AIGroup::Save(CSaveLoad * pSL)
{
	pSL->SaveAPointer("character", pACommander);

	pSL->SaveString(sGroupName);
	pSL->SaveString(sCommand);
	pSL->SaveString(sCommandGroup);
	pSL->SaveString(sLocationNearOtherGroup);
	pSL->SaveString(sGroupType);
	pSL->SaveVector(vInitGroupPos);
	pSL->SaveVector(vMovePoint);
	pSL->SaveDword(bFirstExecute);

	pSL->SaveDword(aGroupShips.Size());
	for (dword i=0; i<aGroupShips.Size(); i++) aGroupShips[i]->Save(pSL);
}

void AIGroup::Load(CSaveLoad * pSL)
{
	pACommander = pSL->LoadAPointer("character");

	sGroupName = pSL->LoadString();
	sCommand = pSL->LoadString();
	sCommandGroup = pSL->LoadString();
	sLocationNearOtherGroup = pSL->LoadString();
	sGroupType = pSL->LoadString();
	vInitGroupPos = pSL->LoadVector();
	vMovePoint = pSL->LoadVector();
	bFirstExecute = pSL->LoadDword() != 0;
	dword dwNum = pSL->LoadDword();
	for (dword i=0; i<dwNum; i++)
	{
		AIShip * pShip = NEW AIShipWar();
		AIShip::AIShips.Add(pShip);		// add to global array
		aGroupShips.Add(pShip);			// add to local group array
		pShip->Load(pSL);
	}
}