#include "CannonController.h"
#include "..\..\Server.h"
#include "..\Ship\Ship.h"
#include "..\Cannons\Cannon.h"

float NetShipCannonController::fMaxCannonDamageDistance = 1.0f;
float NetShipCannonController::fMaxCannonDamageRadiusPoint = 1.0f;

NetShipCannonController::NetShipCannonController(NetShip * _pShip) : aShipBorts(_FL_, 4)
{
	SetShip(_pShip);
	bReload = false;
	bTempFlag = false;
	bNotEnoughBalls = false;

	rs.dwColor = 0xFFFFFF;
	rs.vPos = CVECTOR(10000.0f, 0.0f, 0.0f);
	rs.dwSubTexture = -1;
	rs.fAngle = 0.0f;
	rs.fSize = 1.0f;
}

NetShipCannonController::~NetShipCannonController()
{
}

bool NetShipCannonController::Fire2Position(dword dwBort, CVECTOR & vFirePos, float fFireHeight)
{
	Assert(dwBort < aShipBorts.Size());
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	bool bEnoughBalls = isHaveEnoughtBallsForBort(dwBort);
	if (!bEnoughBalls) bNotEnoughBalls = true;
	if (!pBort->isCharged() || !pBort->aCannons.Size() || pBort->isBortDamaged()) return false;

	CVECTOR vFireDir = !(vFirePos - GetShip()->GetPos());
	float fFireDist = sqrtf(~(vFirePos - GetShip()->GetPos()));

	CVECTOR vTempFirePos = vFirePos + CVECTOR(0.0f, fFireHeight, 0.0f);

	/*VDATA * pVData = api->Event(SHIP_GET_BORT_FIRE_DELTA, "afff", GetShip()->GetACharacter(), vTempFirePos.x, vTempFirePos.y, vTempFirePos.z);
	float fRadius = pVData->GetFloat();
	float fAng = FRAND(PIm2);
	vTempFirePos += CVECTOR(fRadius * sinf(fAng), 0.0f, fRadius * cosf(fAng));*/

	CVECTOR vTempFireDir = !(vTempFirePos - GetShip()->GetPos());

	for (dword j=0;j<pBort->aCannons.Size();j++)
	{
		NetCannon *pCannon = &pBort->aCannons[j];
		if (pCannon->isDamaged()) continue;
		pCannon->SetRechargeEnable(bEnoughBalls);
		pCannon->Fire(pBort->fSpeedV0, vTempFirePos);
	}
	pBort->ClearCharge();	// FIX-ME
	
	return true;
}

bool NetShipCannonController::Fire(CVECTOR & vFireCamPos, CVECTOR & vFireDir)
{
	CVECTOR vOurPos = GetShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();
	
	CVECTOR vTempDir = !CVECTOR(vFireDir.x,0.0f,vFireDir.z);
	float fAngle = safeACos(Clamp(vTempDir | vFireDir));
	if (vFireDir.y<0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	if (dwBort != INVALID_BORT_INDEX) do
	{
		float fMaxDist = NetCannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

		CVECTOR vFirePos = vFireCamPos + fMaxDist * !vFireDir; 
		vFirePos.y = 0.0f;

		Fire2Position(dwBort, vFirePos, 0.0f);
	} while ((dwBort = GetNextFireBort(dwBort,vTempFirePos)) != INVALID_BORT_INDEX);

	return true;
}

bool NetShipCannonController::Fire(CVECTOR & vFirePos)
{
	dword dwBort = GetFirstFireBort(vFirePos);
	if (dwBort != INVALID_BORT_INDEX) do
	{
		Fire2Position(dwBort, vFirePos, -1.0f);
	} while ((dwBort = GetNextFireBort(dwBort,vFirePos)) != INVALID_BORT_INDEX);
	return true;
}

/*bool NetShipCannonController::Fire(AIShip * pEnemy)
{
	Assert(pEnemy);

	float fFireHeight;

	VDATA * pVData = api->Event(CANNON_GET_FIRE_HEIGHT, "aa", GetShip()->GetACharacter(), pEnemy->GetACharacter()); Assert(pVData);
	fFireHeight = pVData->GetFloat();

	float fDistance = GetShip()->GetDistance(*pEnemy);
	float fSpeedZ = pEnemy->GetShipBasePointer()->GetCurrentSpeed();
	float fAng = pEnemy->GetShipBasePointer()->GetAng().y;
	CVECTOR vFirePos = (fSpeedZ * fDistance / GetSpeedV0()) * CVECTOR(sinf(fAng), 0.0f, cosf(fAng));
	vFirePos += pEnemy->GetPos();

	rs.vPos = vFirePos;

	dword dwBort = GetFirstFireBort(vFirePos);
	if (dwBort != INVALID_BORT_INDEX) do
	{
		Fire2Position(dwBort, vFirePos, fFireHeight);
	} while ((dwBort = GetNextFireBort(dwBort, vFirePos)) != INVALID_BORT_INDEX);
	return true;
}*/

void NetShipCannonController::Execute(float fDeltaTime)
{
	dword i, j;

	UpdateParameters();

	if (!IsServer()) return;

	if (bReload)
	{
		bNotEnoughBalls = false;
		
		for (i=0; i<aShipBorts.Size(); i++) 
		{
			bool bEnoughBalls = isHaveEnoughtBallsForBort(i);
			if (!bEnoughBalls) bNotEnoughBalls = true;
			for (j=0; j<aShipBorts[i].aCannons.Size(); j++)
			{
				NetCannon * pC = &aShipBorts[i].aCannons[j];
				if (pC->isDamaged()) continue;
				pC->SetRechargeEnable(bEnoughBalls);
				pC->Recharge();
			}
		}
		bReload = false;
	}

	bNotEnoughBalls = false;
	for (i=0; i<aShipBorts.Size(); i++) 
	{
		if (isHaveEnoughtBallsForBort(i)) continue;
		bNotEnoughBalls = true;
		break;
	}

	ATTRIBUTES * pAPlayer = GetShip()->GetACharacter(); 
	ATTRIBUTES * pABorts = pAPlayer->FindAClass(pAPlayer, "Ship.Cannons.Borts"); Assert(pABorts);

	for (i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		pBort->fChargePercent = 0.0f;
		for (dword j=0; j<pBort->aCannons.Size(); j++)
		{
			NetCannon * pC = &pBort->aCannons[j];
			if (pC->isDamaged()) continue;
			pC->Execute(fDeltaTime);
			pBort->fChargePercent += pC->GetRechargePercent();
		}
		pBort->fChargePercent /= GetBortIntactCannonsNum(i);//pBort->aCannons.Size();
		
		pABorts->SetAttribute((char*)pBort->sName.GetBuffer(), "");
		ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);

		pACurBort->SetAttributeUseFloat("MaxFireDistance", pBort->fMaxFireDistance);
		pACurBort->SetAttributeUseFloat("ChargeRatio", pBort->fChargePercent);
		pACurBort->SetAttributeUseFloat("DamageRatio", 1.0f - (float)GetBortIntactCannonsNum(i) / (float)pBort->aCannons.Size());
	}
	else
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		pABorts->SetAttribute((char*)pBort->sName.GetBuffer(), "");
		ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);

		pACurBort->SetAttributeUseFloat("MaxFireDistance", 0.0f);
		pACurBort->SetAttributeUseFloat("ChargeRatio", 0.0f);
		pACurBort->SetAttributeUseFloat("DamageRatio", 0.0f);
	}
}

// return (fire distance) angle
float NetShipCannonController::GetBortHeightAngle(long iBortIndex)
{
	CMatrix m;
	CVECTOR v,vZ;

	GetShip()->GetMatrix()->Get3X3(&m);
	
	v.y = 0.0f;
	v.x = sinf(aShipBorts[iBortIndex].fFireDir);
	v.z = cosf(aShipBorts[iBortIndex].fFireDir);

	return  atan2f((m * v).y,1.0f);
}

float NetShipCannonController::GetFireDistance(bool bMaxFireDistance)
{
	float fDistance = (bMaxFireDistance) ? 0.0f : 1e10f;
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vOurPos = GetShip()->GetPos();

	for (dword i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];
		float fMaxFireDistance = NetCannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
		if (bMaxFireDistance && fDistance < fMaxFireDistance) fDistance = fMaxFireDistance;
		if (!bMaxFireDistance && fDistance > fMaxFireDistance) fDistance = fMaxFireDistance;
	}
	return fDistance;
}

bool NetShipCannonController::UpdateParameters()
{
	CVECTOR v;

	CVECTOR vOurPos = GetShip()->GetPos();
	CVECTOR vZ = !(GetShip()->GetMatrix()->Vz());

	// updates borts parameters
	float fSpeedV0 = GetSpeedV0();
	for (dword i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		pBort->fSpeedV0 = fSpeedV0;
		pBort->fMaxFireDistance = NetCannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);

		float fDir	= pBort->fFireDir;
		float fZone = pBort->fFireZone;

		v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
		RotateAroundY(v.x, v.z, vZ.z, vZ.x);
		pBort->vDirection = v;
	}
	return true;
}

CVECTOR NetShipCannonController::GetBortDirection(dword dwBort)
{
	Assert(dwBort != INVALID_BORT_INDEX && dwBort < aShipBorts.Size());
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	float fDir	= pBort->fFireDir;
	CVECTOR v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
	CVECTOR vZ = !(GetShip()->GetMatrix()->Vz());		// FIX-ME?
	RotateAroundY(v.x, v.z, vZ.z, vZ.x);
	return v;
}

CVECTOR NetShipCannonController::GetFirePos(CVECTOR & vFireDir, float fDistance)
{
	CVECTOR vOurPos = GetShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fAngle = safeACos(Clamp(vTempDir | vFireDir));
	if (vFireDir.y<0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	Assert(dwBort != INVALID_BORT_INDEX);

	float fMaxDist = NetCannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);
	if (fDistance > fMaxDist) fDistance = fMaxDist;

	float t = fDistance / (fSpeedV0 * cosf(fAngle));
	float y = fSpeedV0 * t * sinf(fAngle) - NetServer::fGravity * SQR(t) / 2.0f;

	CVECTOR vPos = vOurPos + fDistance * vTempDir + CVECTOR(0.0f, y, 0.0f);

	return vPos; 
}

CVECTOR NetShipCannonController::GetFirePos(CVECTOR & vFireDir)
{
	CVECTOR vOurPos = GetShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fAngle = safeACos(Clamp(vTempDir | vFireDir));
	if (vFireDir.y < 0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	if (dwBort == INVALID_BORT_INDEX)		// FIX-ME
	{
		_asm int 3			 
		bTempFlag = true;
		dwBort = GetFirstFireBort(vTempFirePos);
		Assert(dwBort != INVALID_BORT_INDEX);
	}

	float fMaxDist = NetCannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

	return vOurPos + fMaxDist * !vFireDir; 
}

bool NetShipCannonController::isCanFire(CVECTOR & vCamDir)
{
	CVECTOR vEnemyDir = !CVECTOR(vCamDir.x, 0.0f, vCamDir.z);
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
	{
		if (aShipBorts[i].isBortDamaged()) continue;

		AISHIP_BORT * pBort = &aShipBorts[i];
	
		// check angle limit
		float fY0;
		fY0 = vEnemyDir | pBort->vDirection;
		if (fY0 < pBort->fCosFireZone && fabsf(fY0 - pBort->fCosFireZone) > 1e-20f) continue;

		// check fire height angle
		CVECTOR v1 = pBort->vDirection;
		v1 = !(CVECTOR(v1.x, vCamDir.y, v1.z));
		CVECTOR v2 = !(CVECTOR(v1.x, sinf(GetBortHeightAngle(i)), v1.z));
		float fSub = v2.y - v1.y;
		float fAng = safeACos(Clamp(v1 | v2));
		if (fSub>0.0f && fAng > fabsf(pBort->fFireAngMin)) continue;
		if (fSub<0.0f && fAng > fabsf(pBort->fFireAngMax)) continue;

 		return true;
	}
	return false;
}

bool NetShipCannonController::isCanFireBort(dword dwBort, CVECTOR & vFirePos, float * pfZapasDistance)
{
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	if (!pBort->aCannons.Size()) return false;
	if (pBort->isBortDamaged()) return false;

	CVECTOR vFireDir = (vFirePos - GetShip()->GetPos());
	vFireDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fFireDist = sqrtf(~(vFirePos - GetShip()->GetPos()));

	// check distance limit
	if (fFireDist > pBort->fMaxFireDistance) return false;	
	
	// check angle limit
	float fY0;//,fY1;
	fY0 = vFireDir | pBort->vDirection;
	if (fY0 < pBort->fCosFireZone && fabsf(fY0 - pBort->fCosFireZone) > 1e-20f) return false;
	/*fY0 = (vFireDir ^ pBort->vAngleVectors[0]).y;
	fY1 = (vFireDir ^ pBort->vAngleVectors[1]).y;
	if (bTempFlag)
	{
		api->Trace("AIShipCannonController: bort = %d, fY0 = %.8f, fY1 = %.8f", dwBort, fY0, fY1);
	}
	if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) return false;*/

	if (pfZapasDistance) *pfZapasDistance = pBort->fMaxFireDistance - fFireDist;

 	return true;
}

dword NetShipCannonController::GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance)
{
	float fFireDist = sqrtf(~(vFirePos - GetShip()->GetPos()));
	dword dwBestBort = INVALID_BORT_INDEX;
	CVECTOR vOurPos = GetShip()->GetPos();
	CVECTOR vOurDir = CVECTOR(sinf(GetShip()->GetAng().y),0.0f,cosf(GetShip()->GetAng().y));
	float fBestAng = 1e8f;
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		if (!pBort->isCharged() || pBort->isBortDamaged()) continue;

		float fSpeedV0 = GetSpeedV0();
		float fMaxFireDistance = NetCannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
		if (fFireDist > fMaxFireDistance) continue;	
		if (fMaxFireDistance - fFireDist < fZapasDistance) continue;

		CVECTOR vBortDir = GetBortDirection(i);
		float fAng = safeACos(Clamp(vOurDir | vBortDir));
		if (fAng < fBestAng)
		{
			fBestAng = fAng;
			dwBestBort = i;
		}
	}
	return dwBestBort;
}

dword NetShipCannonController::GetFirstFireBort(CVECTOR & vFirePos, float * pfZapasDistance)
{
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size() && isCanFireBort(i,vFirePos,pfZapasDistance) && !aShipBorts[i].isBortDamaged()) return i;

	bTempFlag = false;
	return INVALID_BORT_INDEX;
}

dword NetShipCannonController::GetNextFireBort(dword dwPrevBort, CVECTOR & vFirePos, float * pfZapasDistance)
{
	Assert(dwPrevBort != INVALID_BORT_INDEX && dwPrevBort<aShipBorts.Size());
	for (dword i=dwPrevBort+1;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size() && isCanFireBort(i,vFirePos,pfZapasDistance) && !aShipBorts[i].isBortDamaged()) return i;

	return INVALID_BORT_INDEX;
}

bool NetShipCannonController::isCanFirePos(CVECTOR & vFirePos)
{
	return (GetFirstFireBort(vFirePos)!=INVALID_BORT_INDEX);
}

/*bool NetShipCannonController::isCanFire(AIShip * pEnemy)
{
	Assert(pEnemy);
	return (GetFirstFireBort(pEnemy->GetPos())!=INVALID_BORT_INDEX);
}*/

float NetShipCannonController::GetSpeedV0()
{
	ATTRIBUTES * pACannon;
	ATTRIBUTES * pACharacter = GetShip()->GetACharacter();
	pACannon = pACharacter->FindAClass(pACharacter,"Ship.Cannons"); Assert(pACannon);
	return pACannon->GetAttributeAsFloat("SpeedV0");
}

void NetShipCannonController::Realize(float fDeltaTime)
{
	if (IsClient()) return;

	/*for (dword i=0; i<aShipBorts.Size(); i++)
	{
		AISHIP_BORT * pB = &aShipBorts[i];
		Render().Print(0, 140 + i * 20, "%s, %d, %.2f, ", (char*)pB->sName.GetBuffer(), pB->aCannons.Size(), pB->fChargePercent);
	}*/
}

bool NetShipCannonController::Init(ATTRIBUTES *_pABaseShip)
{
	pABaseShip = _pABaseShip;

	ATTRIBUTES	* pACharacter = GetShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	// init borts from ShipsTypes attributes
	ATTRIBUTES * pAP = pABaseShip->FindAClass(pABaseShip,"Cannons.Borts"); if (!pAP) return false;
	dword dwIdx = 0;
	while(true)
	{
		ATTRIBUTES * pBortAttribute = pAP->GetAttributeClass(dwIdx);
		if (!pBortAttribute) break;
		AISHIP_BORT * pBort = &aShipBorts[aShipBorts.Add()];
		pBort->sName = pBortAttribute->GetThisName();
		pBort->fFireZone = pBortAttribute->GetAttributeAsFloat("FireZone");
		pBort->fFireAngMin = pBortAttribute->GetAttributeAsFloat("FireAngMin");
		pBort->fFireAngMax = pBortAttribute->GetAttributeAsFloat("FireAngMax");
		pBort->fFireDir = pBortAttribute->GetAttributeAsFloat("FireDir");
		pBort->dwNumDamagedCannons = 0;

		pBort->fCosFireZone = cosf(pBort->fFireZone / 2.0f);
		dwIdx++;

		// create damages
		char str[512];
		sprintf(str, "%s.damages", (char*)pBort->sName.GetBuffer());
		pABorts->CreateSubAClass(pABorts, str);
	}

	bool bSuccess = ScanShipForCannons();
	if (bSuccess) { Execute(0.0f); }

	return bSuccess;
}

bool NetShipCannonController::ScanShipForCannons()
{
	char		str[512];
	dword		i, j;
	CMatrix		m;
	GEOS::LABEL	label;
	GEOS::INFO	info;
	NODE		*pNode;

	ATTRIBUTES	* pACharacter = GetShip()->GetACharacter();
	MODEL		* pModel = GetShip()->GetModel(); Assert(pModel);
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	// search and add cannons
	dword dwIdx = 0;
	while (pNode = pModel->GetNode(dwIdx))
	{
		pNode->geo->GetInfo(info);
		for (i=0;i<dword(info.nlabels);i++)
		{
			pNode->geo->GetLabel(i,label);
			for (j=0;j<aShipBorts.Size();j++) if (aShipBorts[j] == label.group_name)
			{
				NetCannon * pCannon = &aShipBorts[j].aCannons[aShipBorts[j].aCannons.Add()];
				memcpy(m,label.m,sizeof(m));
				aShipBorts[j].fOurBortFireHeight += m.Pos().y;
				pCannon->Init(GetShip(), label);

				sprintf(str, "%s.damages", label.group_name);
				ATTRIBUTES * pADamages = pABorts->FindAClass(pABorts, str);
				sprintf(str, "c%d", aShipBorts[j].aCannons.Size() - 1);
				float fDamage = pADamages->GetAttributeAsFloat(str, 0.0f);
				pADamages->SetAttributeUseFloat(str, fDamage);
				pCannon->SetDamage(fDamage);
				if (pCannon->isDamaged()) aShipBorts[j].dwNumDamagedCannons++;

				break;
			}
		}
		dwIdx++;
	}

	// calculate medium fire point for this ship
	for (i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
	{
		aShipBorts[i].fOurBortFireHeight /= float(aShipBorts[i].aCannons.Size());
		
		bool bHaveEnoughBalls = isHaveEnoughtBallsForBort(i);
		if (!bHaveEnoughBalls) bNotEnoughBalls = true;

		//for (j=0;j<aShipBorts[i].aCannons.Size();j++)
			//aShipBorts[i].aCannons[j].Recharge();
		for (j=0;j<aShipBorts[i].aCannons.Size();j++)
			aShipBorts[i].aCannons[j].QuickRecharge(bHaveEnoughBalls);
	}

	// TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	/*api->Trace("NumBorts = %d",aShipBorts.Size());
	for (dword i=0;i<aShipBorts.Size();i++)
		api->Trace("Bort = %s, Num cannons = %d",aShipBorts[i].sName.GetBuffer(),aShipBorts[i].aCannons.Size());*/
	// TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	return true;
}

dword NetShipCannonController::GetCannonsNum()
{
	dword i, dwCannonsNum = 0;
	for (i=0;i<aShipBorts.Size();i++) dwCannonsNum += GetBortIntactCannonsNum(i);
	return dwCannonsNum;
}

dword NetShipCannonController::GetBortIntactCannonsNum(dword dwBortIdx)
{
	Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.Size());
	dword dwCannons = 0;
	for (dword j=0; j<aShipBorts[dwBortIdx].aCannons.Size(); j++) if (!aShipBorts[dwBortIdx].aCannons[j].isDamaged()) dwCannons++;
	return dwCannons;
}

bool NetShipCannonController::isHaveEnoughtBallsForBort(dword dwBortIdx)
{
	Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.Size());

	dword dwIntactCannonsNum = GetBortIntactCannonsNum(dwBortIdx);
	if (dwIntactCannonsNum == 0) return true;

	ATTRIBUTES * pACharge = GetShip()->GetACharacter()->FindAClass(GetShip()->GetACharacter(), "Ship.Cannons.Charge"); Assert(pACharge);
	ATTRIBUTES * pAGoods = GetShip()->GetACharacter()->FindAClass(GetShip()->GetACharacter(), "Ship.Goods"); Assert(pAGoods);
	ATTRIBUTES * pAGood = pAGoods->GetAttributeClass(pACharge->GetAttributeAsDword("Type")); Assert(pAGood);
	dword dwGoodNum = pAGood->GetAttributeAsDword();
	return (dwIntactCannonsNum < dwGoodNum);
}

void NetShipCannonController::Unload()
{
	dword i, j;
	for (i=0; i<aShipBorts.Size(); i++) 
	{
		for (j=0; j<aShipBorts[i].aCannons.Size(); j++) 
		{
			NetCannon * pC = &aShipBorts[i].aCannons[j];
			if (!pC->isDamaged()) pC->Unload();
		}
	}
}

void NetShipCannonController::Reload()
{
	Unload();
	bReload = true;
}

/*void NetShipCannonController::CheckCannonsBoom(float fTmpCannonDamage, const CVECTOR & vPnt)
{
	char	str[512];
	dword	i, j;

	ATTRIBUTES	* pACharacter = GetShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	for (i=0; i<aShipBorts.Size(); i++) 
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		sprintf(str, "%s.damages", (char*)pBort->sName.GetBuffer());
		ATTRIBUTES * pADamages = pABorts->FindAClass(pABorts, str); Assert(pADamages);
		ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);

		for (j=0; j<pBort->aCannons.Size(); j++) 
		{
			NetCannon * pC = &pBort->aCannons[j];
			if (pC->isDamaged()) continue;
			CVECTOR vCPos = pC->GetPos() - pC->GetDir() * fMaxCannonDamageRadiusPoint;
			float fDistance = sqrtf(~(vPnt - vCPos));

			if (fDistance > fMaxCannonDamageDistance) continue;
			VDATA * pVData = api->Event(CANNON_DAMAGE, "affffff", GetShip()->GetACharacter(), fTmpCannonDamage, pC->GetDamage(), fDistance, vPnt.x, vPnt.y, vPnt.z);

			sprintf(str, "c%d", j);
			
			pC->SetDamage(pVData->GetFloat());
			pADamages->SetAttributeUseFloat(str, pC->GetDamage());
			if (pC->isDamaged())
			{
				//pBort->dwNumDamagedCannons++;
				pACurBort->SetAttributeUseFloat("DamageRatio", 1.0f - (float)GetBortIntactCannonsNum(i) / (float)pBort->aCannons.Size());
			}
		}
	}
}*/
