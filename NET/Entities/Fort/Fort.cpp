#include "Fort.h"
#include "..\Ship\Ship.h"

NetFort::NetFort() : aCannons(_FL_, 16), aCulverins(_FL_, 16), aMortars(_FL_, 16)
{
	dtFiredTimer.Setup(FRAND(1.0f), 1.0f);
	pShipsLights = null;
	pVWShips = null;

	bInitCannons = false;
	bInitCulverins = false;
	bInitMortars = false;

	dwFriendlyTeam = 0;

	fMaxCannonDamageDistance = 10.0f;
}

NetFort::~NetFort()
{
	DELETE(pVWShips);
}

bool NetFort::AddFort(ATTRIBUTES * pIslandAP, ATTRIBUTES * pFortLabelAP, ENTITY_ID eidModel, ENTITY_ID eidBlot)
{
	Assert(pFortLabelAP);

	ATTRIBUTES * pFortAP = pFortLabelAP->FindAClass(pFortLabelAP, "fort"); Assert(pFortAP);
	ATTRIBUTES * pModelAP = pFortAP->FindAClass(pFortAP, "model"); Assert(pModelAP);
	ATTRIBUTES * pLocatorsAP = pFortAP->FindAClass(pFortAP, "locators"); Assert(pLocatorsAP);
	ATTRIBUTES * pModelsDirAP = pIslandAP->FindAClass(pIslandAP, "filespath.models"); Assert(pModelsDirAP);

	char * pModelName = pModelAP->GetThisAttr(); Assert(pModelName);
	char * pLocatorsName = pLocatorsAP->GetThisAttr(); Assert(pLocatorsName);
	char * pModelsDir = pModelsDirAP->GetThisAttr(); Assert(pModelsDir);

	this->eidModel = eidModel;

	if (IsServer()) 
	{
		ATTRIBUTES * pACannonsType1 = GetACharacter()->FindAClass(GetACharacter(), "Fort.Cannons.Type.Cannon"); Assert(pACannonsType1);
		ATTRIBUTES * pACannonsType2 = GetACharacter()->FindAClass(GetACharacter(), "Fort.Cannons.Type.Culverine"); Assert(pACannonsType2);
		ATTRIBUTES * pACannonsType3 = GetACharacter()->FindAClass(GetACharacter(), "Fort.Cannons.Type.Mortar"); Assert(pACannonsType3);

		dwCannonType = pACannonsType1->GetAttributeAsDword();
		dwCulverinType = pACannonsType2->GetAttributeAsDword();
		dwMortarType = pACannonsType3->GetAttributeAsDword();

		fMaxCannonDamageDistance = GetACharacter()->GetAttributeAsFloat("MaxCannonDamageDistance");

		ScanFortForCannons(pModelsDir, pLocatorsName);
	}

	dwFriendlyTeam = GetACharacter()->GetAttributeAsDword("FriendlyTeam");

	ATTRIBUTES * pALights = GetACharacter()->FindAClass(GetACharacter(), "ship.lights");
	ATTRIBUTES * pAFlares = GetACharacter()->FindAClass(GetACharacter(), "ship.flares");

	bool bLights = (pALights) ? pALights->GetAttributeAsDword() != 0 : false;
	bool bFlares = (pAFlares) ? pAFlares->GetAttributeAsDword() != 0 : false;

	if (IsClient()) 
	{
		ENTITY_ID eidTmp;
		if (NetFindClass(IsServer(), &eidTmp, "NetShipLights"))
		{
			pShipsLights = (NetShipLights*)api->GetEntityPointer(&eidTmp); Assert(pShipsLights);
		}

		if (pShipsLights)
		{
			pShipsLights->AddLights(this, GetModel(), bLights, bFlares);
			pShipsLights->Execute(0);
		}
	}

	return true;
}

void NetFort::Execute(dword dwDeltaTime)
{
	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	if (IsClient()) return;
	bool bTryFire = dtFiredTimer.Update(fDeltaTime);

	float fMinCannonDamageDistance = AttributesPointer->GetAttributeAsFloat("MinCannonDamageDistance");

	if (IsServer())
	{
		if (!pVWShips) pVWShips = (VIDWALKER*)api->LayerGetWalker("ship_cannon_trace");
	}

	for (long i=0; i<aCannons; i++)
	{
		SelectCannonType(dwCannonType);
		if (!bInitCannons) aCannons[i].QuickRecharge(true);
		aCannons[i].Execute(fDeltaTime);
		if (bTryFire) TryFire(&aCannons[i], fSpeedV0);
	}
	bInitCannons = true;

	for (long i=0; i<aCulverins; i++)
	{
		SelectCannonType(dwCulverinType);
		if (!bInitCulverins) aCulverins[i].QuickRecharge(true);
		aCulverins[i].Execute(fDeltaTime);
		if (bTryFire) TryFire(&aCulverins[i], fSpeedV0);
	}
	bInitCulverins = true;

	for (long i=0; i<aMortars; i++)
	{
		SelectCannonType(dwMortarType);
		if (!bInitMortars) aMortars[i].QuickRecharge(true);
		aMortars[i].Execute(fDeltaTime);
		if (bTryFire) TryFire(&aMortars[i], fSpeedV0);
	}
	bInitMortars = true;
}

void NetFort::SelectCannonType(dword dwCType)
{
	VDATA * pV = (VDATA *)api->Event("NetServer_FortSelectCannonType", "l", dwCType); Assert(pV);
	fSpeedV0 = pV->GetFloat();
}

void NetFort::TryFire(NetCannon * pC, float fSpeedV0)
{
	NetShip * pFireShip = null;
	if (pC->isDamaged() || pC->isFired() || !pC->isReady2Fire()) return;

	float fMinDistance = 1e10f;
	CVECTOR vCPos = pC->GetPos();
	float fMaxFireDistance = NetCannon::CalcMaxFireDistance(vCPos.y, fSpeedV0, 0.35f); 

	ENTITY_ID * pEID;
	if (pVWShips && (pEID = pVWShips->GetID()) != 0) do 
	{
		NetShip * pShip = (NetShip*)api->GetEntityPointer(pEID);
		if (pShip && pShip->IsServer())	// FIX-ME: add enemy test
		{
			dword dwTeam = pShip->GetACharacter()->GetAttributeAsDword("Team");
			if (pShip->isDead() || dwTeam == dwFriendlyTeam) continue;
			float fDistance = (vCPos - pShip->GetPos()).GetLength2D();
			if (fDistance > fMaxFireDistance) continue;
			if (fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				pFireShip = pShip;
			}
		}
	} while (pEID = pVWShips->GetIDNext());

	if (pFireShip) pC->Fire(fSpeedV0, pFireShip->GetPos());
}

void NetFort::Realize(dword dwDeltaTime)
{
	if (IsServer()) return;

	if (pShipsLights) pShipsLights->SetLights(this);
	if (GetModel()) GetModel()->Realize(dwDeltaTime);
	if (pShipsLights) pShipsLights->UnSetLights(this);
}

dword _cdecl NetFort::ProcessMessage(MESSAGE & message)
{
	ENTITY_ID	eidFortModel, eidBlot;
	ATTRIBUTES	* pFortAPLabel, * pIslandAP;

	long iCode = message.Long();

	switch (iCode)
	{
		case MSG_NET_ADD_FORT:
			pIslandAP = message.AttributePointer();
			pFortAPLabel = message.AttributePointer();
			eidFortModel = message.EntityID();
			eidBlot = message.EntityID();
			AddFort(pIslandAP, pFortAPLabel, eidFortModel, eidBlot);
		break;
	}
	return 0;
}

bool NetFort::ScanFortForCannons(char * pModelsDir, char * pLocatorsName)
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
			NetCannon * pCannon = null;

			if (strnicmp(label.name, "cannon", 6) == 0) 
				pCannon = &aCannons[aCannons.Add()];

			if (strnicmp(label.name, "culverin", 8) == 0) 
				pCannon = &aCulverins[aCulverins.Add()];

			if (strnicmp(label.name, "mortar", 6) == 0) 
				pCannon = &aMortars[aMortars.Add()];

			if (pCannon) 
			{
				pCannon->Init(this, label);
				//pCannon->QuickRecharge(true);
			}
		}
		dwIdx++;
	}

	iTotalCannons = aCannons.Len() + aCulverins.Len() + aMortars.Len();
	iDamagedCannons = 0;

	api->DeleteEntity(model_id);
	return true;
}

NetCannon * NetFort::GetCannon(dword dwCannonIndex)
{
	if (dwCannonIndex < aCannons.Size()) return &aCannons[dwCannonIndex];
	if (dwCannonIndex < aCannons.Size() + aCulverins.Size()) return &aCulverins[dwCannonIndex - aCannons.Size()];
	return &aMortars[dwCannonIndex - (aCannons.Size() + aCulverins.Size())];
}

void NetFort::CheckCannonForBoom(NetCannon * pC, long iBallOwner, dword dwCType, const CVECTOR & vPnt)
{
	Assert(IsServer());

	if (pC->isDamaged()) return;

	float fDistance = sqrtf(~(pC->GetPos() - vPnt));
	if (fDistance > fMaxCannonDamageDistance) return;

	float fDistanceRatio = 1.0f - fDistance / fMaxCannonDamageDistance;

	CVECTOR vCPos = pC->GetPos(); // vPnt

	VDATA * pV = api->Event("NetServer_OnFortCannonDamage", "llllfffff", long(dwCType), iTotalCannons, iDamagedCannons, iBallOwner, pC->GetDamage(), fDistanceRatio, vCPos.x, vCPos.y, vCPos.z); Assert(pV);
	pC->SetDamage(pV->GetFloat());
	if (pC->isDamaged()) iDamagedCannons++;
}

float NetFort::Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	Assert(IsServer());

	if (iBallOwner == GetNetID() || !GetModel()) return 2.0f;

	float fRes = GetModel()->Trace(vSrc, vDst);	if (fRes > 1.0f) return 2.0f;

	CVECTOR vPnt = vSrc + fRes * (vDst - vSrc);

	api->Event("NetServer_OnFortHit", "fff", vPnt.x, vPnt.y, vPnt.z);

	for (long i=0; i<aCannons; i++) CheckCannonForBoom(&aCannons[i], iBallOwner, dwCannonType, vPnt);
	for (long i=0; i<aCulverins; i++) CheckCannonForBoom(&aCulverins[i], iBallOwner, dwCulverinType, vPnt);
	for (long i=0; i<aMortars; i++) CheckCannonForBoom(&aMortars[i], iBallOwner, dwMortarType, vPnt);

	return fRes; 
}

float NetFort::Trace(const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	return GetModel()->Trace(vSrc, vDst);
}
