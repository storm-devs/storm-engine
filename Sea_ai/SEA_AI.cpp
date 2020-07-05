#include "SEA_AI.h"

INTERFACE_FUNCTION
CREATE_CLASS(SEA_AI)
CREATE_CLASS(AIFort)
CREATE_CLASS(AIBalls)
CREATE_CLASS(AISeaGoods)
CREATE_CLASS(SeaLocatorShow)

SEA_AI::SEA_AI()
{
	bFirstInit = true;
}

SEA_AI::~SEA_AI()
{
	for (dword i=0;i<AIGroup::AIGroups.Size();i++) DELETE(AIGroup::AIGroups[i]);
	AIGroup::AIGroups.DelAll();
	AIShip::AIShips.DelAll();
	Helper.Uninit();
}

bool SEA_AI::Init()
{
	Helper.SetDevice();
	return true;
}

void SEA_AI::SetDevice()
{
}

dword dwRDTSC;

void SEA_AI::Execute(dword Delta_Time)
{
	RDTSC_B(dwRDTSC);
	float fDeltaTime = 0.001f * float(Delta_Time);

	if (bFirstInit)
	{
		Helper.CalculateRelations();
		bFirstInit = false;
	}
	
	for (dword i=0;i<AIGroup::AIGroups.Size();i++)
	{
		AIGroup::AIGroups[i]->Execute(fDeltaTime);
	}

	RDTSC_E(dwRDTSC);
}

extern dword dwTotal;

void SEA_AI::Realize(dword Delta_Time)
{
	float fDeltaTime = 0.001f * float(Delta_Time);
	for (dword i=0;i<AIGroup::AIGroups.Size();i++)
	{
		AIGroup::AIGroups[i]->Realize(fDeltaTime);
	}
	//api->Trace("%d",dwTotal);
	//AIHelper::pRS->Print(0,90,"%d",dwRDTSC);//dwTotal);
}

bool SEA_AI::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool SEA_AI::LoadState(ENTITY_STATE * state)
{
	return true;
}

void SEA_AI::ProcessMessage(dword iMsg, dword wParam, dword lParam)
{
}

dword _cdecl SEA_AI::ProcessMessage(MESSAGE & message)
{
	char	cGroupName[256], cOtherGroupName[256], cTemp[256];

	long iCode = message.Long();
	switch (iCode)
	{
		case AI_MESSAGE_UNLOAD:
		{
			dword	i;
			for (i=0; i<AIGroup::AIGroups.Size(); i++) AIGroup::AIGroups[i]->Unload();
			for (i=0; i<AIShip::AIShips.Size(); i++) AIShip::AIShips[i]->Unload();
		}
		break;
		case AI_MESSAGE_CANNON_RELOAD:
			AIShip::ReloadCannons(message.AttributePointer());
		break;
		case AI_MESSAGE_SET_CAMERAS_ATTRIBUTE:
			AIHelper::pASeaCameras = message.AttributePointer();
		break;
		case AI_MESSAGE_ADD_SHIP:
		{
			ENTITY_ID  eidShip = message.EntityID();
			ATTRIBUTES * pACharacter = message.AttributePointer();
			ATTRIBUTES * pAShip = message.AttributePointer();
			AddShip(eidShip, pACharacter, pAShip);
		}
		break;
		case AI_MESSAGE_GROUP_SET_LOCATION_OTHER_GROUP:
			message.String(sizeof(cGroupName),cGroupName);
			message.String(sizeof(cOtherGroupName),cOtherGroupName);
			AIGroup::GroupSetLocationNearOtherGroup(cGroupName,cOtherGroupName);
		break;
		case AI_MESSAGE_GROUP_SET_TYPE:
			message.String(sizeof(cGroupName),cGroupName);
			message.String(sizeof(cTemp),cTemp);
			AIGroup::GroupSetType(cGroupName, cTemp);
		break;
		case AI_MESSAGE_GROUP_SET_COMMANDER:
		{
			message.String(sizeof(cGroupName),cGroupName);
			ATTRIBUTES * pCharacter = message.AttributePointer();
			AIGroup::GroupSetCommander(cGroupName, pCharacter);
		}
		break;
		case AI_MESSAGE_GROUP_GET_ATTACK_HP:
		{
			message.String(sizeof(cGroupName),cGroupName);
			float fDistance = message.Float();
			VDATA * pVData = message.ScriptVariablePointer();
			pVData->Set(AIGroup::GetAttackHP(cGroupName, fDistance));
		}
		break;
		case AI_MESSAGE_GROUP_SET_XYZ_AY:
		{
			float	x, y, z, ay;
			message.String(sizeof(cGroupName),cGroupName);
			x = message.Float(); y = message.Float(); z = message.Float();
			ay = message.Float();
			AIGroup::SetXYZ_AY(cGroupName,CVECTOR(x,y,z),ay);
		}
		break;
		case AI_MESSAGE_SHIP_SET_SAIL_STATE:
		{
			ATTRIBUTES * pCharacter = message.AttributePointer();
			float fSailState = message.Float();
			AIShip * pAIShip = AIShip::FindShip(pCharacter); 
			if (!pAIShip)
			{
				api->BTrace("SeaAI err: SetSailState, can't find ship for character = %s", pCharacter->GetAttribute("id"));
				return 0;
			}
			api->Send_Message(pAIShip->GetShipEID(), "lf", MSG_SHIP_SET_SAIL_STATE, fSailState);
		}
		break;
		case AI_MESSAGE_SHIP_SET_TASK:
		{
			ATTRIBUTES * pCharacter1, * pCharacter2;
			dword	dwCommand = message.Long();
			dword	dwTaskPriority = message.Long();
			switch (dwCommand)
			{
				case AITASK_ATTACK:
					pCharacter1 = message.AttributePointer();
					pCharacter2 = message.AttributePointer();
					AIShip::ShipSetAttack(dwTaskPriority, pCharacter1, pCharacter2);
				break;
				case AITASK_RUNAWAY:
					pCharacter1 = message.AttributePointer();
					AIShip::ShipSetRunAway(dwTaskPriority, pCharacter1);
				break;
				case AITASK_MOVE:
					pCharacter1 = message.AttributePointer();
					if (message.GetCurrentFormatType() == 'a')
					{
						pCharacter2 = message.AttributePointer();
						AIShip::ShipSetMove(dwTaskPriority, pCharacter1, pCharacter2);
					}
					else
					{
						CVECTOR vPos = 0.0f;
						vPos.x = message.Float();
						vPos.z = message.Float();
						AIShip::ShipSetMove(dwTaskPriority, pCharacter1, vPos);
					}
				break;
				case AITASK_DRIFT:
					pCharacter1 = message.AttributePointer();
					AIShip::ShipSetDrift(dwTaskPriority, pCharacter1);
				break;
				case AITASK_DEFEND:
					pCharacter1 = message.AttributePointer();
					pCharacter2 = message.AttributePointer();
					AIShip::ShipSetDefend(dwTaskPriority, pCharacter1, pCharacter2);
				break;
				case AITASK_BRANDER:
					pCharacter1 = message.AttributePointer();
					pCharacter2 = message.AttributePointer();
					AIShip::ShipSetBrander(dwTaskPriority, pCharacter1, pCharacter2);
				break;
				case AITASK_ABORDAGE:
					pCharacter1 = message.AttributePointer();
					pCharacter2 = message.AttributePointer();
					AIShip::ShipSetAbordage(dwTaskPriority, pCharacter1, pCharacter2);
					break;
			}
		}
		break;
		case AI_MESSAGE_GROUP_SET_TASK:
		{
			CVECTOR	vPnt;
			char	cGroupAttackingName[256];
			dword	dwCommand = message.Long();
			switch (dwCommand)
			{
				case AITASK_MOVE:
					message.String(sizeof(cGroupName),cGroupName);
					vPnt.x = message.Float();
					vPnt.y = message.Float();
					vPnt.z = message.Float();
					AIGroup::GroupSetMove(cGroupName,vPnt);
				break;
				case AITASK_ATTACK:
					message.String(sizeof(cGroupName),cGroupName);
					message.String(sizeof(cGroupAttackingName),cGroupAttackingName);
					AIGroup::GroupSetAttack(cGroupName,cGroupAttackingName);
				break;
				case AITASK_RUNAWAY:
					message.String(sizeof(cGroupName),cGroupName);
					AIGroup::GroupSetRunAway(cGroupName);
				break;
			}
		}
		break;
		case AI_MESSAGE_UPDATE_RELATIONS:
			Helper.CalculateRelations();
		break;
		case AI_MESSAGE_SHIP_GET_ATTACK_HP:
		{
			ATTRIBUTES * pACharacter = message.AttributePointer();
			float fDistance = message.Float();
			VDATA * pVD = message.ScriptVariablePointer();
			pVD->Set(AIShip::FindShip(pACharacter)->GetAttackHP(fDistance));
		}
		break;
		case AI_MESSAGE_SHIP_CHANGE_GROUP:
		{
			ATTRIBUTES * pACharacter = message.AttributePointer();
			message.String(sizeof(cGroupName), cGroupName);
			AIGroup::ShipChangeGroup(pACharacter, cGroupName);
		}
		break;
		case AI_MESSAGE_CANNON_FIRE:
		{
			ATTRIBUTES * pACharacter = message.AttributePointer();
			bool bCameraOutside = (message.Long() == 1);
			AIShip::ShipFire(pACharacter, bCameraOutside);
		}	
		break;
		case AI_MESSAGE_SET_OFFICER_2_SHIP:
		{
			ATTRIBUTES * pACharacter1 = message.AttributePointer();
			ATTRIBUTES * pACharacter2 = message.AttributePointer();
			AIGroup::SetOfficerCharacter2Ship(pACharacter1, pACharacter2);
		}
		break;
		case AI_MESSAGE_SWAP_SHIPS:
		{
			ATTRIBUTES * pACharacter1 = message.AttributePointer();
			ATTRIBUTES * pACharacter2 = message.AttributePointer();
			AIGroup::SwapCharactersShips(pACharacter1, pACharacter2);
		}
		break;
		case AI_MESSAGE_SAIL_2_LOCATOR:
		{
			float x, y, z, ay;
			x = message.Float();
			y = message.Float();
			z = message.Float();
			ay = message.Float();
			
			AIGroup::SailMainGroup(CVECTOR(x,y,z), ay, null);
		}
		break;
		case AI_MESSAGE_SAIL_2_CHARACTER:
		{
			ATTRIBUTES * pACharacter = message.AttributePointer();
			float fDistance = message.Float();
			float fAngle = message.Float();
			AIShip * pAIShip = AIShip::FindShip(pACharacter); Assert(pAIShip);
			CVECTOR vPos = pAIShip->GetPos() + (fDistance * CVECTOR(sinf(fAngle), 0.0f, cosf(fAngle)));

			AIGroup::SailMainGroup(vPos, -10000.0f, pACharacter);
		}
		break;
		case AI_MESSAGE_CHARACTER_DEAD:
		{
			VAI_INNEROBJ * pAIObj = AIHelper::FindAIInnerObj(message.AttributePointer());
			if (pAIObj) 
			{
				pAIObj->SetDead(true);
				if (pAIObj->GetObjType() != AIOBJ_FORT) ((AIShip*)pAIObj)->GetShipBasePointer()->SetDead();
			}
		}
		break;
		case AI_MESSAGE_GET_RELATION:
		{
			ATTRIBUTES	* pACharacter1 = message.AttributePointer();
			ATTRIBUTES	* pACharacter2 = message.AttributePointer();
			VDATA		* pVData = message.ScriptVariablePointer();
			pVData->Set((long)Helper.GetRelationSafe(pACharacter1, pACharacter2));
		}
		break;
		case AI_MESSAGE_SET_COMPANION_ENEMY:
		{
			ATTRIBUTES	* pACharacter = message.AttributePointer();
			SetCompanionEnemy(pACharacter);
		}
		break;
		case AI_MESSAGE_CANNONS_PARAMS:
			AIShipCannonController::fMaxCannonDamageDistance = message.Float();
			AIShipCannonController::fMaxCannonDamageRadiusPoint = message.Float();
		break;
		case AI_MESSAGE_CANNONS_BOOM_CHECK:
		{
			float	fTmpCannonDamage, x, y, z;

			ATTRIBUTES * pACharacter = message.AttributePointer();
			fTmpCannonDamage = message.Float();
			x = message.Float();
			y = message.Float(); 
			z = message.Float();

			AIShip * pAIShip = AIShip::FindShip(pACharacter); Assert(pAIShip);
			pAIShip->GetCannonController()->CheckCannonsBoom(fTmpCannonDamage, CVECTOR(x, y, z));
		}
		break;
		// boal 08.08.06 метод пересчета орудий на корабле -->
		case AI_MESSAGE_RESEARCH_CANNONS:
		{
			ATTRIBUTES * pACharacter = message.AttributePointer();

			AIShip * pAIShip = AIShip::FindShip(pACharacter); Assert(pAIShip);
			pAIShip->GetCannonController()->ResearchCannons();
		}
		break;
		// boal 08.08.06 метод пересчета орудий на корабле <--
		case AI_MESSAGE_SEASAVE:
		{
			char str[256];
			Save(str);
		}
		break;
		case AI_MESSAGE_SEALOAD:
		{
			char str[256];
			Load(str);
		}
		break;
	}
	return 0;
}

void SEA_AI::Save(const char * pStr)
{
	CSaveLoad SL;

	SL.CreateWrite();

	Helper.Save(&SL);
	ENTITY_ID eidCamera;
	api->FindClass(&eidCamera, "SEA_CAMERAS", 0);
	api->Send_Message(eidCamera, "lp", AI_MESSAGE_SEASAVE, &SL);

	AIBalls::pAIBalls->Save(&SL);
	
	SL.SaveDword(AIGroup::AIGroups.Size());
	for (dword i=0; i<AIGroup::AIGroups.Size(); i++) AIGroup::AIGroups[i]->Save(&SL);

	if (AIFort::pAIFort) AIFort::pAIFort->Save(&SL);
}

void SEA_AI::Load(const char * pStr)
{
	Helper.Uninit();

	CSaveLoad SL;

	SL.CreateLoad();

	Helper.Load(&SL);
	ENTITY_ID eidCamera;
	api->FindClass(&eidCamera, "SEA_CAMERAS", 0);
	api->Send_Message(eidCamera, "lp", AI_MESSAGE_SEALOAD, &SL);

	AIBalls::pAIBalls->Load(&SL);
	
	dword dwNumGroups = SL.LoadDword();
	for (dword i=0; i<dwNumGroups; i++) 
	{
		AIGroup * pG = AIGroup::AIGroups[AIGroup::AIGroups.Add(NEW AIGroup)];
		pG->Load(&SL);
	}

	if (AIFort::pAIFort) AIFort::pAIFort->Load(&SL);
	Helper.Init();
}

dword SEA_AI::AttributeChanged(ATTRIBUTES * pAttribute)
{
	dword	i;

	if (*pAttribute == "isDone")
	{
		// delete all old groups and ships
		Helper.Init();

		for (i=0; i<AIShip::AIShips.Size(); i++) AIShip::AIShips[i]->CheckStartPosition();
	}

	if (*pAttribute == "DistanceBetweenGroupShips")
	{
		AIGroup::fDistanceBetweenGroupShips = pAttribute->GetAttributeAsFloat();
	}

	return 0;
}

void SEA_AI::AddShip(ENTITY_ID eidShip, ATTRIBUTES * pCharacter, ATTRIBUTES * pAShip)
{
	Assert(pCharacter && pAShip);
	ATTRIBUTES * pG = pCharacter->FindAClass(pCharacter,"SeaAI.Group");	Assert(pG);
	
	// search group
	char * pGName = pG->GetAttribute("Name"); Assert(pGName);

	AIGroup::FindOrCreateGroup(pGName)->AddShip(eidShip, pCharacter, pAShip);
}

void SEA_AI::SetCompanionEnemy(ATTRIBUTES * pACharacter)
{
	Assert(pACharacter);

	// delete from old group
	AIGroup * pG = AIGroup::FindGroup(pACharacter); Assert(pG); 
	AIShip * pS = pG->ExtractShip(pACharacter);

	// create and add to new group
	ATTRIBUTES * pSeaAIG = pACharacter->FindAClass(pACharacter, "SeaAI.Group");	Assert(pSeaAIG);
	char * pGName = pSeaAIG->GetAttribute("Name"); Assert(pGName);

	AIGroup::FindOrCreateGroup(pGName)->InsertShip(pS);
}