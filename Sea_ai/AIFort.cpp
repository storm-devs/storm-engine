#include "AIFort.h"
#include "AIShip.h"

AIFort	* AIFort::pAIFort = null;

AIFort::AIFort() : aForts(_FL_, 2)
{
	dtFiredTimer.Setup(FRAND(1.0f), 1.0f);
	pLastTraceFort = null;
	pShipsLights = null;
	pAIFort = this;
}

AIFort::~AIFort()
{
	pAIFort = null;
	for (dword i=0; i<aForts.Size(); i++) DELETE(aForts[i]);
}

bool AIFort::Init()
{
	return true;
}

void AIFort::SetDevice()
{
}

float AIFort::GetSpeedV0(dword dwFortIndex)
{
	ATTRIBUTES * pACannon, * pACharacter;
	pACharacter = aForts[dwFortIndex]->GetACharacter();
	pACannon = pACharacter->FindAClass(pACharacter, "Ship.Cannons"); Assert(pACannon);
	return pACannon->GetAttributeAsFloat("SpeedV0");
}

//float fAngF = 0.0f;
//float yyy = 0.0f;

void AIFort::Execute(dword Delta_Time)
{
	/*CVECTOR vSrc = CVECTOR(-10000.0f * sinf(fAngF), yyy, -10000.0f * cosf(fAngF));
	CVECTOR vDst = -CVECTOR(vSrc.x, yyy, vSrc.z);
	vDst.y = yyy;
	Trace(vSrc, vDst);*/
	fMinCannonDamageDistance = AttributesPointer->GetAttributeAsFloat("MinCannonDamageDistance");

	float fDeltaTime = float(Delta_Time) * 0.001f;
	if (!aForts.Size()) return;

	bool bFiredTimer = dtFiredTimer.Update(fDeltaTime);

	for (dword k=0; k<aForts.Size(); k++)
	{
		AI_FORT	* pF = aForts[k];
		if (pF->isDead()) continue;
		if (!pF->isNormalMode()) continue;

		float fSpeedV0 = 0.0f;
		dword dwCurrentCannonType = 0xFFFFFFFF;
		dword iMax = pF->GetAllCannonsNum(); // boal fix
		for (dword i=0; i<iMax; i++)
		{
			AICannon * pC = pF->GetCannon(i);
			
			dword dwNewCurrentCannonType = pF->GetCannonType(i);

			// update cannons parameters
			if (dwCurrentCannonType != dwNewCurrentCannonType)
			{
				dwCurrentCannonType = dwNewCurrentCannonType;
				ATTRIBUTES * pACannons = pF->GetACharacter()->FindAClass(pF->GetACharacter(), "Ship.Cannons"); 
				if (!pACannons)	pACannons = pF->GetACharacter()->CreateSubAClass(pF->GetACharacter(), "Ship.Cannons"); Assert(pACannons);
				pACannons->SetAttributeUseDword("type", dwCurrentCannonType);
				api->Event(CANNON_RECALCULATE_PARAMETERS, "l", GetIndex(pF->GetACharacter()));

				fSpeedV0 = GetSpeedV0(k);
			}

			if (!pC->isFired() && pC->isReady2Fire() && bFiredTimer) 
			{
				AIShip	* pFireAIShip = null;
				float	fMinDistance = 1e10f;
				CVECTOR vCPos = pC->GetPos();
				float fMaxFireDistance = AICannon::CalcMaxFireDistance(vCPos.y, fSpeedV0, 0.35f); // FIX-ME
				for (dword j=0; j<AIShip::AIShips.Size(); j++) if (!AIShip::AIShips[j]->isDead() && Helper.isEnemy(pF->GetACharacter(), AIShip::AIShips[j]->GetACharacter()))
				{
					float fDistance = AIShip::AIShips[j]->GetDistance(vCPos);
					if (fDistance > fMaxFireDistance) continue;
					if (fDistance < fMinDistance)
					{
						fMinDistance = fDistance;
						pFireAIShip = AIShip::AIShips[j];
					}
				}
				if (pFireAIShip) 
					pC->Fire(fSpeedV0, pFireAIShip->GetPos());
			}
			pC->Execute(fDeltaTime);
		}
	}
}

void AIFort::Realize(dword Delta_Time)
{
	/*ENTITY_ID eidIsland;
	float fCurrentImmersion = 0.0f;
	if (api->FindClass(&eidIsland, "Island", 0))
	{
		ISLAND_BASE * pIsland = (ISLAND_BASE *)api->GetEntityPointer(&eidIsland);
		fCurrentImmersion = pIsland->GetCurrentImmersion();
	}*/

	/*for (dword k=0; k<aForts.Size(); k++)
	{
		CMatrix mTemp;
		mTemp.BuildPosition(0.0f, -fCurrentImmersion, 0.0f);
		aForts[k]->GetModel()->mtx = aForts[k]->mOldMatrix * mTemp;

		pShipsLights->SetLights(&aForts[k]->tmpObject);
		aForts[k]->GetModel()->Realize(Delta_Time);
		pShipsLights->UnSetLights(&aForts[k]->tmpObject);

		aForts[k]->GetModel()->mtx = aForts[k]->mOldMatrix;
	}*/

	/*   // boal del_cheat
#ifndef XBOX
	if (api->Controls->GetDebugAsyncKeyState('X') < 0)
	{
		CMatrix mI; mI.SetIdentity();
		AIHelper::pRS->SetTransform(D3DTS_WORLD, mI);

		long iNumCannons = 0;
		for (dword k=0; k<aForts.Size(); k++)
		{
			AI_FORT	* pF = aForts[k];
			for (dword i=0; i<pF->GetAllCannonsNum(); i++)
			{
				AICannon * pC = pF->GetCannon(i);
				CVECTOR vPos = pC->GetPos();
				CMatrix mRot(0.0f, pC->GetDirY(), 0.0f);
				AIHelper::pRS->DrawVector(vPos, vPos + mRot * CVECTOR(0.0f, 0.0f, 3.0f), 0xFFFFFFFF);
				AIHelper::pRS->DrawSphere(vPos, 0.3f, 0xFF00FF00);
				iNumCannons++;
			}
		}
	}
#endif
*/
}

bool AIFort::CreateState(ENTITY_STATE_GEN * state_gen) { return true; }
bool AIFort::LoadState(ENTITY_STATE * state) { return true; }

bool AIFort::AddFort(ATTRIBUTES * pIslandAP, ATTRIBUTES * pFortLabelAP, ATTRIBUTES * pFortCharacter, ENTITY_ID eidModel, ENTITY_ID eidBlot)
{
	Assert(pFortLabelAP);

	ATTRIBUTES * pFortAP = pFortLabelAP->FindAClass(pFortLabelAP, "fort"); Assert(pFortAP);
	ATTRIBUTES * pModelAP = pFortAP->FindAClass(pFortAP, "model"); Assert(pModelAP);
	ATTRIBUTES * pLocatorsAP = pFortAP->FindAClass(pFortAP, "locators"); Assert(pLocatorsAP);
	ATTRIBUTES * pModelsDirAP = pIslandAP->FindAClass(pIslandAP, "filespath.models"); Assert(pModelsDirAP);

	char * pModelName = pModelAP->GetThisAttr(); Assert(pModelName);
	char * pLocatorsName = pLocatorsAP->GetThisAttr(); Assert(pLocatorsName);
	char * pModelsDir = pModelsDirAP->GetThisAttr(); Assert(pModelsDir);

	AI_FORT * pFort = NEW AI_FORT(pFortLabelAP);
	pFort->SetACharacter(pFortCharacter);
	pFort->SetModelEID(eidModel);
	pFort->SetBlotEID(eidBlot);
	//pFort->pFortLabelAP = pFortLabelAP;
	aForts.Add(pFort);

	ATTRIBUTES * pACannonsType1 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.1"); Assert(pACannonsType1);
	ATTRIBUTES * pACannonsType2 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.2"); Assert(pACannonsType2);
	ATTRIBUTES * pACannonsType3 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.3"); Assert(pACannonsType3);
	pFort->dwCannonType = pACannonsType1->GetAttributeAsDword();
	pFort->dwCulverinType = pACannonsType2->GetAttributeAsDword();
	pFort->dwMortarType = pACannonsType3->GetAttributeAsDword();

	pFort->mOldMatrix = pFort->GetModel()->mtx;

	ScanFortForCannons(pFort, pModelsDir, pLocatorsName);

	ATTRIBUTES * pALights = pFortCharacter->FindAClass(GetACharacter(),"ship.lights");
	ATTRIBUTES * pAFlares = pFortCharacter->FindAClass(GetACharacter(),"ship.flares");

	bool bLights = (pALights) ? pALights->GetAttributeAsDword() != 0 : false;
	bool bFlares = (pAFlares) ? pAFlares->GetAttributeAsDword() != 0 : false;

	ENTITY_ID eidTmp;
	api->FindClass(&eidTmp, "shiplights", 0);
	pShipsLights = (IShipLights*)api->GetEntityPointer(&eidTmp); Assert(pShipsLights);

	pShipsLights->AddLights(&pFort->tmpObject, pFort->GetModel(), bLights, bFlares);
	pShipsLights->Execute(0);

	api->Event(FORT_CREATE, "al", pFortCharacter, pFort->GetAllCannonsNum());

	return true;
}

void AIFort::AddFortHit(long iCharacterIndex, CVECTOR & vHitPos)
{
	dword	i, j, iMax;
	for (i=0; i<aForts.Size(); i++)
	{
		AI_FORT	* pF = aForts[i];
		
		iMax = pF->GetAllCannonsNum(); // boal fix
		for (j=0; j<iMax; j++)
		{
			AICannon * pC = pF->GetCannon(j);

			if (pC->isDamaged()) continue;
			
			CVECTOR vPos = pC->GetPos();
			float fDistance = sqrtf(~(vPos - vHitPos));
			if (fDistance > fMinCannonDamageDistance) continue;

			//VDATA * pVData = api->Event(FORT_CANNON_DAMAGE, "llallfffff", iCharacterIndex, GetIndex(pF->GetACharacter()), pF->pFortLabelAP, pF->GetAllCannonsNum(), pF->GetDamagedCannonsNum(), vPos.x, vPos.y, vPos.z, fDistance, pC->GetDamage()); Assert(pVData);
			VDATA * pVData = api->Event(FORT_CANNON_DAMAGE, "llallfffff", iCharacterIndex, GetIndex(pF->GetACharacter()), pF->pFortLabelAP, iMax, pF->GetDamagedCannonsNum(), vPos.x, vPos.y, vPos.z, fDistance, pC->GetDamage()); Assert(pVData);
			
			pC->SetDamage(pVData->GetFloat());
		}
	}
}

AIFort::AI_FORT * AIFort::FindFort(ENTITY_ID eidModel)
{
	for (long i=0; i<aForts.Len(); i++)
	{
		if (aForts[i]->GetModelEID().pointer == eidModel.pointer) return aForts[i];
	}
	return null;
}

dword AIFort::ProcessMessage(MESSAGE & message)
{
	CVECTOR		vHit;
	long		iCharacterIndex;
	ENTITY_ID	eidFortModel, eidBlot;
	ATTRIBUTES	* pFortAPLabel, * pCharacter, * pIslandAP;
	AI_FORT		* pFort = null;

	switch (message.Long())
	{
		case AI_MESSAGE_FORT_HIT:
			iCharacterIndex = message.Long();
			vHit.x = message.Float(); vHit.y = message.Float();	vHit.z = message.Float();
			AddFortHit(iCharacterIndex, vHit);
		break;
		case AI_MESSAGE_ADD_FORT:
			pIslandAP = message.AttributePointer();
			pFortAPLabel = message.AttributePointer();
			pCharacter = message.AttributePointer();
			eidFortModel = message.EntityID();
			eidBlot = message.EntityID();
			AddFort(pIslandAP, pFortAPLabel, pCharacter, eidFortModel, eidBlot);
			Helper.AddCharacter(pCharacter, pCharacter);
		break;
		case AI_MESSAGE_FORT_SET_LIGHTS:
			pFort = FindFort(message.EntityID());
			if (pShipsLights && pFort) 
				pShipsLights->SetLights(&pFort->tmpObject);
		break;
		case AI_MESSAGE_FORT_UNSET_LIGHTS:
			pFort = FindFort(message.EntityID());
			if (pShipsLights && pFort) 
				pShipsLights->UnSetLights(&pFort->tmpObject);
		break;
	}
	return 0;
}

dword AIFort::AttributeChanged(ATTRIBUTES * pAttribute)
{
	return 0;
}

bool AIFort::Mount(ATTRIBUTES * pAttribute)
{
	return true;
}

bool AIFort::ScanFortForCannons(AI_FORT * pFort, char * pModelsDir, char * pLocatorsName)
{
	ENTITY_ID	model_id;
	GEOS::LABEL	label;
	GEOS::INFO	info;
	NODE		* pNode;
	string		sLocatorsName;

	sLocatorsName.Format("%s\\%s", pModelsDir, pLocatorsName);
	api->CreateEntity(&model_id, "MODELR");
	api->Send_Message(model_id, "ls", MSG_MODEL_LOAD_GEO, (char*)sLocatorsName.GetBuffer());

	MODEL * pModel = (MODEL*)api->GetEntityPointer(&model_id); Assert(pModel);

	// search and add cannons & culverins
	dword dwIdx = 0;
	while (pNode = pModel->GetNode(dwIdx))
	{
		pNode->geo->GetInfo(info);
		for (long i=0; i<info.nlabels; i++)
		{
			pNode->geo->GetLabel(i, label);
			AICannon * pCannon = null;
			
			if (strnicmp(label.name, "cannon", 6) == 0 && pFort->dwCannonType != -1) 
				pCannon = &pFort->aCannons[pFort->aCannons.Add()];

			if (strnicmp(label.name, "culverin", 8) == 0 && pFort->dwCulverinType != -1) 
				pCannon = &pFort->aCulverins[pFort->aCulverins.Add()];

			if (strnicmp(label.name, "mortar", 6) == 0 && pFort->dwMortarType != -1) 
			{
				pCannon = &pFort->aMortars[pFort->aMortars.Add()];
				pCannon->SetType(AICannon::CANNONTYPE::CANNONTYPE_MORTAR);
			}

			if (pCannon) 
			{
				pCannon->Init(pFort, GetID(), label);
				pCannon->QuickRecharge(true);
			}
		}
		dwIdx++;
	}

	api->DeleteEntity(model_id);
	return true;
}

bool AIFort::isAttack(AIGroup * pGroup)
{
	return false;
}

bool AIFort::isDead()
{
	return false;
}

float AIFort::GetPower()
{
	return 10000.0f;
}

ATTRIBUTES * AIFort::GetACharacter()
{
	if (pLastTraceFort) return pLastTraceFort->GetACharacter();
	return null;
}

float AIFort::Trace(const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	pLastTraceFort = null;

	float fBestRes = 2.0f;
	float fBestDistance = 1e10f; 
	for (dword i=0; i<GetNumForts(); i++)
	{
		MODEL * pModel = (MODEL*)api->GetEntityPointer(&GetFort(i)->GetModelEID()); Assert(pModel);
		float fRes = pModel->Trace(vSrc, vDst);
		if (fRes > 1.0f) continue;
		float fDistance = fRes * sqrtf(~(vDst - vSrc));
		if (fDistance < fBestDistance)
		{
			pLastTraceFort = GetFort(i);
			fBestDistance = fDistance;
			fBestRes = fRes;
		}
	}

	return fBestRes; 
}

float AIFort::Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	float fBestRes = 2.0;
	for (dword i=0; i<GetNumForts(); i++)
	{
		MODEL * pModel = (MODEL*)api->GetEntityPointer(&GetFort(i)->GetModelEID());

		float fRes = pModel->Trace(vSrc, vDst);
		if (fRes < fBestRes) fBestRes = fRes;
		if (fRes <= 1.0f)
		{
			CVECTOR vTemp = vSrc + (vDst - vSrc) * fRes;
			api->Event(BALL_FORT_HIT, "lfff", iBallOwner, vTemp.x, vTemp.y, vTemp.z);
	
			CVECTOR vDir = !(vDst - vSrc);
			api->Send_Message(GetFort(i)->GetBlotEID(), "lffffff", MSG_BLOTS_HIT, vTemp.x, vTemp.y, vTemp.z, vDir.x, vDir.y, vDir.z);
		}
		
	}
	return fBestRes; 
}

AIFort::AI_FORT	* AIFort::FindFort(ATTRIBUTES * pACharacter)
{
	for (dword i=0; i<GetNumForts(); i++)
	{
		if (GetFort(i)->GetACharacter() == pACharacter) return GetFort(i);
	}
	return null;
}

CVECTOR AIFort::AI_FORT::GetAttackPoint(VAI_INNEROBJ * pObj) 
{
	float fDistance = pObj->GetMaxFireDistance();
    dword iMax = GetAllCannonsNum(); // boal fix
	for (dword i=0; i<iMax; i++)
	{
		AICannon * pC = GetCannon(i);
		if (pC->isDamaged()) continue;

		CVECTOR vCPos = pC->GetPos();
		float fDirY = pC->GetDirY();

		CVECTOR vRes = vCPos + fDistance / 1.4f * CVECTOR(sinf(fDirY), 0.0f, cosf(fDirY));

		// check for ship can place here
		//pObj->

		return (vRes - GetPos());
	}
	
	return 0.0f;
}

void AIFort::Save(CSaveLoad * pSL)
{
	pSL->SaveFloat(fMinCannonDamageDistance);
	for (dword i=0; i<aForts.Size(); i++) aForts[i]->Save(pSL);
}

void AIFort::Load(CSaveLoad * pSL)
{
	fMinCannonDamageDistance = pSL->LoadFloat();
	for (dword i=0; i<aForts.Size(); i++) aForts[i]->Load(pSL, GetID());
}

void AIFort::AI_FORT::Save(CSaveLoad * pSL)
{
	pSL->SaveVector(vPos);

	dword i;
	for (i=0; i<aCannons.Size(); i++) aCannons[i].Save(pSL);
	for (i=0; i<aCulverins.Size(); i++) aCulverins[i].Save(pSL);
	for (i=0; i<aMortars.Size(); i++) aMortars[i].Save(pSL);
}

void AIFort::AI_FORT::Load(CSaveLoad * pSL, ENTITY_ID eid)
{
	vPos = pSL->LoadVector();

	dword i;
	for (i=0; i<aCannons.Size(); i++) 
	{
		aCannons[i].Load(pSL, this, eid);
		if (aCannons[i].isDamaged())
		{
			CVECTOR vPos = aCannons[i].GetPos();
			api->Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
		}
	}

	for (i=0; i<aCulverins.Size(); i++) 
	{
		aCulverins[i].Load(pSL, this, eid);
		if (aCulverins[i].isDamaged())
		{
			CVECTOR vPos = aCulverins[i].GetPos();
			api->Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
		}
	}

	for (i=0; i<aMortars.Size(); i++) 
	{
		aMortars[i].Load(pSL, this, eid);
		if (aMortars[i].isDamaged())
		{
			CVECTOR vPos = aMortars[i].GetPos();
			api->Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
		}
	}
}

void AIFort::Fire(const CVECTOR & vPos)
{
	for (long i=0; i<aForts.Len(); i++)
		pShipsLights->AddDynamicLights(&aForts[i]->tmpObject, vPos);
}