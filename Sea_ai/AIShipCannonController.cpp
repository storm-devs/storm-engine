#include "AIShip.h"

float AIShipCannonController::fMaxCannonDamageDistance = 1.0f;
float AIShipCannonController::fMaxCannonDamageRadiusPoint = 1.0f;

AIShipCannonController::AIShipCannonController(AIShip * _pShip) : aShipBorts(_FL_, 4)
{
	SetAIShip(_pShip);
	bReload = false;
	bTempFlag = false;
	bNotEnoughBalls = false;

	rs.dwColor = 0xFFFFFF;
	rs.vPos = CVECTOR(10000.0f, 0.0f, 0.0f);
	rs.dwSubTexture = -1;
	rs.fAngle = 0.0f;
	rs.fSize = 1.0f;
}

AIShipCannonController::~AIShipCannonController()
{
}

bool AIShipCannonController::Fire2Position(dword dwBort, CVECTOR & vFirePos, float fFireHeight)
{
	Assert(dwBort < aShipBorts.Size());
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	bool bEnoughBalls = isHaveEnoughtBallsForBort(dwBort);
	if (!bEnoughBalls) bNotEnoughBalls = true;
	if (!pBort->isCharged() || !pBort->aCannons.Size() || pBort->isBortDamaged()) return false;

	CVECTOR vFireDir = !(vFirePos - pOurAIShip->GetPos());
	float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));

	CVECTOR vTempFirePos = vFirePos + CVECTOR(0.0f, fFireHeight, 0.0f);

	VDATA * pVData = api->Event(SHIP_GET_BORT_FIRE_DELTA, "afff", GetAIShip()->GetACharacter(), vTempFirePos.x, vTempFirePos.y, vTempFirePos.z);
	float fRadius = pVData->GetFloat();
	float fAng = FRAND(PIm2);
	vTempFirePos += CVECTOR(fRadius * sinf(fAng), 0.0f, fRadius * cosf(fAng));

	CVECTOR vTempFireDir = !(vTempFirePos - pOurAIShip->GetPos());

	api->Event(SHIP_BORT_FIRE, "lisffffff", GetIndex(GetAIShip()->GetACharacter()), GetAIShip()->GetShipEID(),(char*)aShipBorts[dwBort].sName.GetBuffer(),vTempFirePos.x,vTempFirePos.y,vTempFirePos.z,vTempFireDir.x,vTempFireDir.y,vTempFireDir.z);

	for (dword j=0;j<pBort->aCannons.Size();j++)
	{
		AICannon *pCannon = &pBort->aCannons[j];
		if (pCannon->isDamaged()) continue;
		pCannon->SetRechargeEnable(bEnoughBalls);
		pCannon->Fire(pBort->fSpeedV0, vTempFirePos);
	}
	pBort->ClearCharge();	// FIX-ME
	
	return true;
}

bool AIShipCannonController::Fire(CVECTOR & vFireCamPos, CVECTOR & vFireDir)
{
	CVECTOR vOurPos = GetAIShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();
	
	CVECTOR vTempDir = !CVECTOR(vFireDir.x,0.0f,vFireDir.z);
	float fAngle = acosf(Clamp(vTempDir | vFireDir));
	if (vFireDir.y<0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	if (dwBort != INVALID_BORT_INDEX) do
	{
		float fMaxDist = AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

		CVECTOR vFirePos = vFireCamPos + fMaxDist * !vFireDir; 
		vFirePos.y = 0.0f;

		Fire2Position(dwBort, vFirePos, 0.0f);
	} while ((dwBort = GetNextFireBort(dwBort,vTempFirePos)) != INVALID_BORT_INDEX);

	return true;
}

bool AIShipCannonController::Fire(CVECTOR & vFirePos)
{
	dword dwBort = GetFirstFireBort(vFirePos);
	if (dwBort != INVALID_BORT_INDEX) do
	{
		Fire2Position(dwBort, vFirePos, -1.0f);
	} while ((dwBort = GetNextFireBort(dwBort,vFirePos)) != INVALID_BORT_INDEX);
	return true;
}

bool AIShipCannonController::Fire(AIShip * pEnemy)
{
	Assert(pEnemy);

	float fFireHeight;

	VDATA * pVData = api->Event(CANNON_GET_FIRE_HEIGHT, "aa", pOurAIShip->GetACharacter(), pEnemy->GetACharacter()); Assert(pVData);
	fFireHeight = pVData->GetFloat();

	float fDistance = GetAIShip()->GetDistance(*pEnemy);
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
}

void AIShipCannonController::Execute(float fDeltaTime)
{
	dword i, j;

	UpdateParameters();

	CVECTOR vOurPos = GetAIShip()->GetPos();

	if (bReload)
	{
		bNotEnoughBalls = false;
		
		for (i=0; i<aShipBorts.Size(); i++) 
		{
			bool bEnoughBalls = isHaveEnoughtBallsForBort(i);
			if (!bEnoughBalls) bNotEnoughBalls = true;
			for (j=0; j<aShipBorts[i].aCannons.Size(); j++)
			{
				AICannon * pC = &aShipBorts[i].aCannons[j];
				if (pC->isDamaged()) continue;
				pC->SetRechargeEnable(bEnoughBalls);
				pC->Recharge();
			}
		}
		bReload = false;
	}

	if (GetAIShip()->isMainCharacter())
	{
		bNotEnoughBalls = false;
		for (i=0; i<aShipBorts.Size(); i++) 
		{
			if (isHaveEnoughtBallsForBort(i)) continue;
			bNotEnoughBalls = true;
			break;
		}
		api->Event(SHIP_NOT_ENOUGH_BALLS, "l", bNotEnoughBalls);
	}

	for (i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];
		
		// set maximum MaxFireDistance for all cannons
		float fMaxBortFireDistance = AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, GetSpeedV0(), pBort->fFireAngMax);
		for (dword j=0; j<pBort->aCannons.Size(); j++)
		{
			pBort->aCannons[j].SetMaxFireDistance(fMaxBortFireDistance);
		}
		
		bool bReloaded = pBort->fChargePercent < 0.999f;

		long dwReadyCannonsAfter = 0, dwReadyCannonsBefore = 0;

		pBort->fChargePercent = 0.0f;
		for (dword j=0; j<pBort->aCannons.Size(); j++)
		{
			AICannon * pC = &pBort->aCannons[j];

			if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty())) dwReadyCannonsBefore++;

			if (pC->isDamaged()) continue;

			pC->Execute(fDeltaTime);
			pBort->fChargePercent += pC->GetRechargePercent();

			if (!(pC->isFired() || pC->isRecharged() || pC->isDamaged() || pC->isEmpty())) dwReadyCannonsAfter++;
		}
		pBort->fChargePercent /= GetBortIntactCannonsNum(i);

		if (dwReadyCannonsBefore != dwReadyCannonsAfter && dwReadyCannonsAfter == GetBortIntactCannonsNum(i))//pBort->fChargePercent >= 0.999f)
		{
			api->Event(SHIP_BORT_RELOADED, "as", GetAIShip()->GetACharacter(), pBort->sName.c_str());
			// api->Trace("%s bort is reloaded", pBort->sName.c_str());
		}
		
		// update borts parameters for script
		// if (GetAIShip()->isMainCharacter())
		{
			ATTRIBUTES * pAPlayer = GetAIShip()->GetACharacter(); 
			ATTRIBUTES * pABorts = pAPlayer->FindAClass(pAPlayer, "Ship.Cannons.Borts"); Assert(pABorts);
			pABorts->SetAttribute((char*)pBort->sName.GetBuffer(), "");
			ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);

			pACurBort->SetAttributeUseFloat("MaxFireDistance", pBort->fMaxFireDistance);
			pACurBort->SetAttributeUseFloat("ChargeRatio", pBort->fChargePercent);
			pACurBort->SetAttributeUseFloat("DamageRatio", 1.0f - (float)GetBortIntactCannonsNum(i) / (float)pBort->aCannons.Size());
		}
	}
}

struct tr_vertex
{
	CVECTOR vPos;
	dword	dwColor;
};

array<tr_vertex> Verts(_FL_);
#define TR_FORMAT	(D3DFVF_XYZ|D3DFVF_DIFFUSE) 

void AIShipCannonController::AddTrg(CVECTOR *pVerts, dword dwColor)
{
	tr_vertex *pV;
	for (dword i=0;i<3;i++)
	{
		pV = &Verts[Verts.Add()]; 
		pV->vPos = pVerts[i]; 
		pV->dwColor = dwColor;
	}
}

// return (fire distance) angle
float AIShipCannonController::GetBortHeightAngle(long iBortIndex)
{
	CMatrix m;
	CVECTOR v,vZ;

	GetAIShip()->GetMatrix()->Get3X3(&m);
	
	v.y = 0.0f;
	v.x = sinf(aShipBorts[iBortIndex].fFireDir);
	v.z = cosf(aShipBorts[iBortIndex].fFireDir);

	return  atan2f((m * v).y,1.0f);
}

float AIShipCannonController::GetFireDistance(bool bMaxFireDistance)
{
	float fDistance = (bMaxFireDistance) ? 0.0f : 1e10f;
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vOurPos = GetAIShip()->GetPos();

	for (dword i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];
		float fMaxFireDistance = AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
		if (bMaxFireDistance && fDistance < fMaxFireDistance) fDistance = fMaxFireDistance;
		if (!bMaxFireDistance && fDistance > fMaxFireDistance) fDistance = fMaxFireDistance;
	}
	return fDistance;
}

bool AIShipCannonController::UpdateParameters()
{
	CVECTOR v;

	CVECTOR vOurPos = GetAIShip()->GetPos();
	CVECTOR vZ = !(GetAIShip()->GetMatrix()->Vz());

	// updates borts parameters
	float fSpeedV0 = GetSpeedV0();
	for (dword i=0; i<aShipBorts.Size(); i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		pBort->fSpeedV0 = fSpeedV0;
		pBort->fMaxFireDistance = AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);

		float fDir	= pBort->fFireDir;
		float fZone = pBort->fFireZone;

		v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
		RotateAroundY(v.x, v.z, vZ.z, vZ.x);
		pBort->vDirection = v;

		/*v = CVECTOR(sinf(fDir - fZone/2.0f), 0.0f, cosf(fDir - fZone/2.0f));
		RotateAroundY(v.x,v.z,vZ.z,vZ.x);
		pBort->vAngleVectors[0] = v;

		v = CVECTOR(sinf(fDir + fZone/2.0f), 0.0f, cosf(fDir + fZone/2.0f));
		RotateAroundY(v.x,v.z,vZ.z,vZ.x);
		pBort->vAngleVectors[1] = v;*/
	}
	return true;
}

CVECTOR AIShipCannonController::GetBortDirection(dword dwBort)
{
	Assert(dwBort != INVALID_BORT_INDEX && dwBort < aShipBorts.Size());
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	float fDir	= pBort->fFireDir;
	CVECTOR v = CVECTOR(sinf(fDir), 0.0f, cosf(fDir));
	CVECTOR vZ = !(GetAIShip()->GetMatrix()->Vz());		// FIX-ME?
	RotateAroundY(v.x, v.z, vZ.z, vZ.x);
	return v;
}

CVECTOR AIShipCannonController::GetFirePos(CVECTOR & vFireDir, float fDistance)
{
	CVECTOR vOurPos = GetAIShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fAngle = acosf(Clamp(vTempDir | vFireDir));
	if (vFireDir.y<0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	if (dwBort == INVALID_BORT_INDEX)	
	{
		return CVECTOR(1e5f, 0.0f, 1e5f);
		Assert(dwBort != INVALID_BORT_INDEX);
	}

	float fMaxDist = AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);
	if (fDistance > fMaxDist) fDistance = fMaxDist;

	float t = fDistance / (fSpeedV0 * cosf(fAngle));
	float y = fSpeedV0 * t * sinf(fAngle) - AIHelper::fGravity * SQR(t) / 2.0f;

	CVECTOR vPos = vOurPos + fDistance * vTempDir + CVECTOR(0.0f, y, 0.0f);

	return vPos; 
}

CVECTOR AIShipCannonController::GetFirePos(CVECTOR & vFireDir)
{
	CVECTOR vOurPos = GetAIShip()->GetPos();
	float fSpeedV0 = GetSpeedV0();

	CVECTOR vTempDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fAngle = acosf(Clamp(vTempDir | vFireDir));
	if (vFireDir.y < 0) fAngle = -fAngle;

	CVECTOR vTempFirePos = vOurPos + !vTempDir; vTempFirePos.y = 0.0f;

	dword dwBort = GetFirstFireBort(vTempFirePos);
	if (dwBort == INVALID_BORT_INDEX)		// FIX-ME
	{
		return CVECTOR(1e5f, 0.0f, 1e5f);

		_asm int 3			 
		bTempFlag = true;
		dwBort = GetFirstFireBort(vTempFirePos);
		Assert(dwBort != INVALID_BORT_INDEX);
	}

	float fMaxDist = AICannon::CalcMaxFireDistance(aShipBorts[dwBort].fOurBortFireHeight + vOurPos.y, fSpeedV0, fAngle);

	return vOurPos + fMaxDist * !vFireDir; 
}

bool AIShipCannonController::isCanFire(CVECTOR & vCamDir)
{
	CVECTOR vEnemyDir = !CVECTOR(vCamDir.x, 0.0f, vCamDir.z);
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
	{
		if (aShipBorts[i].isBortDamaged()) continue;

		AISHIP_BORT * pBort = &aShipBorts[i];
	
		// check angle limit
		float fY0;//, fY1;
		fY0 = vEnemyDir | pBort->vDirection;
		//fY0 = (vEnemyDir ^ pBort->vAngleVectors[1]).y;
		//fY1 = (vEnemyDir ^ pBort->vAngleVectors[1]).y;
		if (fY0 <= pBort->fCosFireZone) continue;
		//if ((SIGN(fY0) == SIGN(fY1)) || (SIGN(fY0) == 1)) continue;

		// check fire height angle
		CVECTOR v1 = pBort->vDirection; //(pBort->vAngleVectors[0] + pBort->vAngleVectors[1]) / 2.0f;
		v1 = !(CVECTOR(v1.x, vCamDir.y, v1.z));
		CVECTOR v2 = !(CVECTOR(v1.x, sinf(GetBortHeightAngle(i)), v1.z));
		float fSub = v2.y - v1.y;
		float fAng = acosf(Clamp(v1 | v2));
		if (fSub>0.0f && fAng > fabsf(pBort->fFireAngMin)) continue;
		if (fSub<0.0f && fAng > fabsf(pBort->fFireAngMax)) continue;

 		return true;
	}
	return false;
}

bool AIShipCannonController::isCanFireBort(dword dwBort, CVECTOR & vFirePos, float * pfZapasDistance)
{
	AISHIP_BORT * pBort = &aShipBorts[dwBort];

	if (!pBort->aCannons.Size()) return false;
	if (pBort->isBortDamaged()) return false;

	CVECTOR vFireDir = (vFirePos - pOurAIShip->GetPos());
	vFireDir = !CVECTOR(vFireDir.x, 0.0f, vFireDir.z);
	float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));

	// check distance limit
	if (fFireDist > pBort->fMaxFireDistance) return false;	
	
	// check angle limit
	float fY0;//,fY1;
	fY0 = vFireDir | pBort->vDirection;
	if (fY0 <= pBort->fCosFireZone) return false;
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

dword AIShipCannonController::GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance)
{
	float fFireDist = sqrtf(~(vFirePos - pOurAIShip->GetPos()));
	dword dwBestBort = INVALID_BORT_INDEX;
	CVECTOR vOurPos = GetAIShip()->GetPos();
	CVECTOR vOurDir = CVECTOR(sinf(GetAIShip()->GetAng().y),0.0f,cosf(GetAIShip()->GetAng().y));
	float fBestAng = 1e8f;
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		if (!pBort->isCharged() || pBort->isBortDamaged()) continue;

		float fSpeedV0 = GetSpeedV0();
		float fMaxFireDistance = AICannon::CalcMaxFireDistance(pBort->fOurBortFireHeight + vOurPos.y, fSpeedV0, GetBortHeightAngle(i) + pBort->fFireAngMax);
		if (fFireDist > fMaxFireDistance) continue;	
		if (fMaxFireDistance - fFireDist < fZapasDistance) continue;

		CVECTOR vBortDir = GetBortDirection(i);
		float fAng = acosf(Clamp(vOurDir | vBortDir));
		if (fAng < fBestAng)
		{
			fBestAng = fAng;
			dwBestBort = i;
		}
	}
	return dwBestBort;
}

dword AIShipCannonController::GetFirstFireBort(CVECTOR & vFirePos, float * pfZapasDistance)
{
	for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size() && isCanFireBort(i,vFirePos,pfZapasDistance) && !aShipBorts[i].isBortDamaged()) return i;

	bTempFlag = false;
	return INVALID_BORT_INDEX;
}

dword AIShipCannonController::GetNextFireBort(dword dwPrevBort, CVECTOR & vFirePos, float * pfZapasDistance)
{
	Assert(dwPrevBort != INVALID_BORT_INDEX && dwPrevBort<aShipBorts.Size());
	for (dword i=dwPrevBort+1;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size() && isCanFireBort(i,vFirePos,pfZapasDistance) && !aShipBorts[i].isBortDamaged()) return i;

	return INVALID_BORT_INDEX;
}

bool AIShipCannonController::isCanFirePos(CVECTOR & vFirePos)
{
	return (GetFirstFireBort(vFirePos)!=INVALID_BORT_INDEX);
}

bool AIShipCannonController::isCanFire(AIShip * pEnemy)
{
	Assert(pEnemy);
	return (GetFirstFireBort(pEnemy->GetPos())!=INVALID_BORT_INDEX);
}

float AIShipCannonController::GetSpeedV0()
{
	ATTRIBUTES * pACannon;
	ATTRIBUTES * pACharacter = GetAIShip()->GetACharacter();
	pACannon = pACharacter->FindAClass(pACharacter,"Ship.Cannons"); Assert(pACannon);
	return pACannon->GetAttributeAsFloat("SpeedV0");
}

void AIShipCannonController::Realize(float fDeltaTime)
{
	dword i, j;

	return;

	for (i=0; i<aShipBorts.Size(); i++) 
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		for (j=0; j<pBort->aCannons.Size(); j++) 
		{
			AICannon * pC = &pBort->aCannons[j];
			if (pC->isDamaged()) continue;
			CVECTOR vCPos = pC->GetPos() - pC->GetDir() * 1.0f;
			AIHelper::pRS->DrawSphere(vCPos, fMaxCannonDamageDistance, 0xFFFFFF);
		}
	}
	//AIHelper::pRS->DrawRects(&rs, 1, "AILine");
	/*char tmp[256], str[256];
	str[0] = 0;
	for (dword i=0;i<aShipBorts.Size();i++)
	{
		sprintf(tmp,"%.3f, ",GetBortHeightAngle(i));
		strcat(str,tmp);
	}
	pRS->Print(0,20,str);*/
#ifndef _XBOX
	if (api->Controls->GetDebugAsyncKeyState('H')<0)
	{
		Verts.DelAll();
		CMatrix m;

		CVECTOR vPosTemp = GetAIShip()->GetPos();
		GetAIShip()->GetMatrix()->Get3X3(m);
		CVECTOR vZ = !(m.Vz());
		for (dword i=0;i<aShipBorts.Size();i++) if (aShipBorts[i].aCannons.Size())
		{
			AICannon *pCannon = &aShipBorts[i].aCannons[0];
			// calc fire angle
			float fDist = SQR(GetSpeedV0()) / AIHelper::fGravity;
			fDist *= sinf(2.0f * (GetBortHeightAngle(i) + aShipBorts[i].fFireAngMax));
			// draw baba
			float fDir	= aShipBorts[i].fFireDir;
			float fRastr = aShipBorts[i].fFireZone;
			float fDA = PId2 / 36.0f;
			for (float fAng = fDir - fRastr/2.0f; fAng < fDir + fRastr/2.0f; fAng += fDA)
			{
				CVECTOR v[3];
				ZERO(v);
				float R = fDist;
				v[0].x = vPosTemp.x;
				v[0].z = vPosTemp.z;
				
				v[1].x = R * sinf(fAng);
				v[1].z = R * cosf(fAng);
				RotateAroundY(v[1].x,v[1].z,vZ.z,vZ.x);
				v[1] += v[0];

				v[2].x = R * sinf(fAng + fDA);
				v[2].z = R * cosf(fAng + fDA);
				RotateAroundY(v[2].x,v[2].z,vZ.z,vZ.x);
				v[2] += v[0];
				AddTrg(&v[0],0x0F007F00);
			}

			CMatrix m;
			AIHelper::pRS->SetTransform(D3DTS_WORLD,m);
			if (Verts.Size()) AIHelper::pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST,TR_FORMAT,Verts.Size() / 3,&Verts[0], sizeof(tr_vertex),"ship_baba");
		}
	}
#endif
}

bool AIShipCannonController::Init(ATTRIBUTES *_pAShip)
{
	pAShip = _pAShip;

	ATTRIBUTES	* pACharacter = GetAIShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	// init borts from ShipsTypes attributes
	ATTRIBUTES * pAP = pAShip->FindAClass(pAShip,"Cannons.Borts"); if (!pAP) return false;
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

	return ScanShipForCannons();
}

bool AIShipCannonController::ScanShipForCannons()
{
	char		str[512];
	dword		i, j;
	CMatrix		m;
	GEOS::LABEL	label;
	GEOS::INFO	info;
	NODE		*pNode;

	ATTRIBUTES	* pACharacter = GetAIShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);
	MODEL		* pModel = GetAIShip()->GetModel(); Assert(pModel);

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
				AICannon * pCannon = &aShipBorts[j].aCannons[aShipBorts[j].aCannons.Add()];
				memcpy(m,label.m,sizeof(m));
				aShipBorts[j].fOurBortFireHeight += m.Pos().y;
				pCannon->Init(GetAIShip(), GetAIShip()->GetShipEID(), label);

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

dword AIShipCannonController::GetCannonsNum()
{
	dword i, dwCannonsNum = 0;
	for (i=0;i<aShipBorts.Size();i++) dwCannonsNum += GetBortIntactCannonsNum(i);
	return dwCannonsNum;
}

dword AIShipCannonController::GetBortIntactCannonsNum(dword dwBortIdx)
{
	Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.Size());
	dword dwCannons = 0;
	for (dword j=0; j<aShipBorts[dwBortIdx].aCannons.Size(); j++) if (!aShipBorts[dwBortIdx].aCannons[j].isDamaged()) dwCannons++;
	return dwCannons;
}

bool AIShipCannonController::isHaveEnoughtBallsForBort(dword dwBortIdx)
{
	Assert(dwBortIdx != INVALID_BORT_INDEX && dwBortIdx < aShipBorts.Size());
	VDATA * pvData = api->Event(SHIP_GET_CURRENT_BALLS_NUM, "a", GetAIShip()->GetACharacter()); Assert(pvData);
	if ((long)GetBortIntactCannonsNum(dwBortIdx) > pvData->GetLong()) return false;
	return true;
}

void AIShipCannonController::Unload()
{
	dword i, j;
	for (i=0; i<aShipBorts.Size(); i++) 
	{
		for (j=0; j<aShipBorts[i].aCannons.Size(); j++) 
		{
			AICannon * pC = &aShipBorts[i].aCannons[j];
			if (!pC->isDamaged()) pC->Unload();
		}
	}
}

void AIShipCannonController::Reload()
{
	Unload();
	bReload = true;
}

void AIShipCannonController::CheckCannonsBoom(float fTmpCannonDamage, const CVECTOR & vPnt)
{
	char	str[512];
	dword	i, j;

	ATTRIBUTES	* pACharacter = GetAIShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	for (i=0; i<aShipBorts.Size(); i++) 
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		sprintf(str, "%s.damages", (char*)pBort->sName.GetBuffer());
		ATTRIBUTES * pADamages = pABorts->FindAClass(pABorts, str); Assert(pADamages);
		ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);

		for (j=0; j<pBort->aCannons.Size(); j++) 
		{
			AICannon * pC = &pBort->aCannons[j];
			if (pC->isDamaged()) continue;
			CVECTOR vCPos = pC->GetPos() - pC->GetDir() * fMaxCannonDamageRadiusPoint;
			float fDistance = sqrtf(~(vPnt - vCPos));

			if (fDistance > fMaxCannonDamageDistance) continue;
			VDATA * pVData = api->Event(CANNON_DAMAGE, "affffff", GetAIShip()->GetACharacter(), fTmpCannonDamage, pC->GetDamage(), fDistance, vPnt.x, vPnt.y, vPnt.z);

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
}

// boal 08.08.06 метод пересчета орудий на корабле -->
void AIShipCannonController::ResearchCannons()
{
	char	str[512];
	dword	i, j;

	ATTRIBUTES	* pACharacter = GetAIShip()->GetACharacter();
	ATTRIBUTES	* pABorts = pACharacter->FindAClass(pACharacter, "Ship.Cannons.Borts"); Assert(pABorts);

	for (i=0; i<aShipBorts.Size(); i++) 
	{
		AISHIP_BORT * pBort = &aShipBorts[i];

		sprintf(str, "%s.damages", (char*)pBort->sName.GetBuffer());
		ATTRIBUTES * pADamages = pABorts->FindAClass(pABorts, str); Assert(pADamages);
		ATTRIBUTES * pACurBort = pABorts->FindAClass(pABorts, (char*)pBort->sName.GetBuffer()); Assert(pACurBort);
        pBort->dwNumDamagedCannons = 0;   // нигде не используется, может рудимент?
        
		for (j=0; j<pBort->aCannons.Size(); j++) 
		{
			AICannon * pC = &pBort->aCannons[j];
			sprintf(str, "c%d", j);
			float fDamage = pADamages->GetAttributeAsFloat(str, 0.0f);
			pC->SetDamage(fDamage);
			pADamages->SetAttributeUseFloat(str, pC->GetDamage());
			if (pC->isDamaged())
			{
				pBort->dwNumDamagedCannons++;
				pACurBort->SetAttributeUseFloat("DamageRatio", 1.0f - (float)GetBortIntactCannonsNum(i) / (float)pBort->aCannons.Size());
			}
		}
	}
}
// boal 08.08.06 метод пересчета орудий на корабле <--

void AIShipCannonController::Save(CSaveLoad * pSL)
{
	pSL->SaveDword(bReload);
	pSL->SaveDword(bNotEnoughBalls);
	pSL->SaveDword(bTempFlag);

	pSL->SaveDword(aShipBorts.Size());
	for (dword i=0; i<aShipBorts.Size(); i++)
	{
		AISHIP_BORT & B = aShipBorts[i];
		
		pSL->SaveString(B.sName);
		pSL->SaveFloat(B.fFireZone);
		pSL->SaveFloat(B.fFireAngMin);
		pSL->SaveFloat(B.fFireAngMax);
		pSL->SaveFloat(B.fFireDir);
		pSL->SaveFloat(B.fOurBortFireHeight);
		pSL->SaveFloat(B.fChargePercent);
		pSL->SaveFloat(B.fCosFireZone);

		pSL->SaveDword(B.dwNumDamagedCannons);
		pSL->SaveFloat(B.fSpeedV0);
		pSL->SaveFloat(B.fMaxFireDistance);
		pSL->SaveVector(B.vDirection);

		pSL->SaveDword(B.aCannons.Size());
		for (dword j=0; j<B.aCannons.Size(); j++) 
			B.aCannons[j].Save(pSL);
	}
}

void AIShipCannonController::Load(CSaveLoad * pSL)
{
	bReload = pSL->LoadDword() != 0;
	bNotEnoughBalls = pSL->LoadDword() != 0;
	bTempFlag = pSL->LoadDword() != 0;

	dword dwNum = pSL->LoadDword();
	for (dword i=0; i<dwNum; i++)
	{
		AISHIP_BORT & B = aShipBorts[aShipBorts.Add()];
		
		B.sName = pSL->LoadString();
		B.fFireZone = pSL->LoadFloat();
		B.fFireAngMin = pSL->LoadFloat();
		B.fFireAngMax = pSL->LoadFloat();
		B.fFireDir = pSL->LoadFloat();
		B.fOurBortFireHeight = pSL->LoadFloat();
		B.fChargePercent = pSL->LoadFloat();
		B.fCosFireZone = pSL->LoadFloat();

		B.dwNumDamagedCannons = pSL->LoadDword();
		B.fSpeedV0 = pSL->LoadFloat();
		B.fMaxFireDistance = pSL->LoadFloat();
		B.vDirection = pSL->LoadVector();

		dword dwNumCannons = pSL->LoadDword();
		for (dword j=0; j<dwNumCannons; j++) 
			B.aCannons[B.aCannons.Add()].Load(pSL, GetAIShip(), GetAIShip()->GetShipEID());
	}
}