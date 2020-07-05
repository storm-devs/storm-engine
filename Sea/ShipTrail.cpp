#include "SEA.h"

void SEA::GetLineABC(const CVECTOR & v1, const CVECTOR & v2, float & A, float & B, float & C)
{
	A = (v2.z - v1.z);
	B = (v1.x - v2.x);
	C = -v1.x * v2.z + v1.z * v2.x;
}

void SEA::NewShipInfo(ENTITY_ID * eid, float fSpeed, CVECTOR *vNewPos, float fAngY)
{
	dword i;

	for (i=0; i<aShips.Size(); i++) if (eid->pointer == aShips[i].eID.pointer) break;
	if (i == aShips.Size()) return;

	seaship_t * pS = &aShips[i];

	pS->vCurPos = *vNewPos;
	if (pS->bFirstInfo)
	{
		pS->bFirstInfo = false;
		pS->vPos = pS->vCurPos;
	}

	float fTemp = sqrtf(SQR(vNewPos->x - pS->vPos.x) + SQR(vNewPos->z - pS->vPos.z));
	if (fTemp >= 3.0f)
	{
		// add new water trail part
		pS->pShipTrail = (shiptrail_t*)RESIZE(pS->pShipTrail,(pS->dwNumTrails+1) * sizeof(shiptrail_t));

		memmove(&pS->pShipTrail[1], &pS->pShipTrail[0], pS->dwNumTrails * sizeof(shiptrail_t));

		CVECTOR vDir = !(pS->vPos - *vNewPos);
		pS->pShipTrail[0].vPos = pS->vPos - 3.0f * vDir;
		pS->pShipTrail[0].fWidth = 2.0f;
		pS->pShipTrail[0].fSpeed = fSpeed / 5.0f;
		pS->pShipTrail[0].fAngY = fAngY;
		pS->pShipTrail[0].fTime = 20.0f;
		pS->pShipTrail[0].fVStart = (pS->dwNumTrails==0) ? 0.0f : pS->pShipTrail[1].fVStart - 0.25f;
		pS->pShipTrail[0].fVEnd = pS->pShipTrail[0].fVStart + 0.25f;
		pS->dwNumTrails++;
		
		pS->vPos = pS->pShipTrail[0].vPos;
	}
}

void SEA::ExecuteShipsTrails(dword DeltaTime)
{
	//return;
	dword i, j;
	float fTime = float(DeltaTime) * 0.001f;
	for (i=0; i<aShips.Size(); i++)
	{
		seaship_t * pS = &aShips[i];
		pS->fShipTrailVSize = 0.0f;
		if (pS->dwNumTrails)
		{
			pS->vCurPos.y = 0.0f;
			pS->pShipTrail[0].vPos.y = 0.0f;
			pS->fShipTrailVSize = sqrtf(~(pS->vCurPos - pS->pShipTrail[0].vPos));
		}
		for (j=0;j<pS->dwNumTrails;j++)
		{
			shiptrail_t *pST = &pS->pShipTrail[j];
			pST->fWidth += pST->fSpeed * fTime;
			//if (pST->fWidth>pST->fSpeed*8.0f) pST->fWidth = pST->fSpeed*8.0f;
			pST->fTime -= fTime;
			pST->fVPos = pS->fShipTrailVSize;
			pS->fShipTrailVSize += pST->fVEnd - pST->fVStart;
			if (pST->fTime<0)
			{
				// delete this trail part
				pS->dwNumTrails--;
				pS->pShipTrail = (shiptrail_t*)RESIZE(pS->pShipTrail,sizeof(shiptrail_t)*pS->dwNumTrails);
			}
		}
		CalculateShipTrailTrapezium(pS);
	}
}

void SEA::GetTrailUV(shiptrail_t * pST, CVECTOR * vPos, float * fU, float * fV)
{
	long iIDX = 0;
	float x[2], z[2], u[2], v[2];
	dword dColors[2] = {0xFF, 0xFF00};
	for (long i=0; i<4; i++)
	{
		long i1 = i;
		long i2 = (i==3) ? 0 : i+1;

		CVECTOR *v1 = &pST->v[i1];
		CVECTOR *v2 = &pST->v[i2];

		float dz1 = vPos->z - v1->z;
		float dz2 = vPos->z - v2->z;

		if (dz1*dz2 > 0.0f) continue;

		float k = fabsf(dz1 / (dz2-dz1));
		x[iIDX] = v1->x + k * (v2->x - v1->x);
		z[iIDX] = v1->z + k * (v2->z - v1->z);
		u[iIDX] = fTrackU[i1] + k * (fTrackU[i2] - fTrackU[i1]);
		v[iIDX] = fTrackV[i1] + k * (fTrackV[i2] - fTrackV[i1]);

		float fX = x[iIDX];
		float fZ = z[iIDX];

		//DrawLine(fX-0.1f,fZ-0.1f,fX+0.1f,fZ+0.1f,dColors[iIDX]);
		//DrawLine(fX+0.1f,fZ-0.1f,fX-0.1f,fZ+0.1f,dColors[iIDX]);
		
		iIDX++;
		if (iIDX==2) break;
	}

	float fLen = sqrtf(SQR(x[1]-x[0]) + SQR(z[1]-z[0]));
	float fLen1 = sqrtf(SQR(vPos->x-x[0]) + SQR(vPos->z-z[0]));

	float fK = fLen1 / fLen;//(vPos->x-x[0]) / (x[1]-x[0]);
	*fU = u[0] + fK * (u[1]-u[0]);
	*fV = pST->fVStart + (v[0] + fK * (v[1]-v[0])) * 0.25f;

	//DrawLine(x[0],z[0],x[1],z[1],0xFF00);
}

void SEA::CheckPointForShipTrail(SEAPENAVERTEX * pV, seaship_t * pS)
{
	float sum = 0.0f;
	CVECTOR & vPos = pV->pos;
	for (long i=0; i<(long)pS->dwNumTrails - 1; i++)
	{
		shiptrail_t & pST = pS->pShipTrail[i];
		//dword dw1;
		//RDTSC_B(dw1);

		float f1 = pST.fA[0] * vPos.x + pST.fB[0] * vPos.z + pST.fC[0];
		if (f1 < 0.0f) continue;
		float f3 = pST.fA[2] * vPos.x + pST.fB[2] * vPos.z + pST.fC[2];
		if (f3 < 0.0f) continue;
		float f2 = pST.fA[1] * vPos.x + pST.fB[1] * vPos.z + pST.fC[1];
		if (f2 < 0.0f) continue;
		float f4 = pST.fA[3] * vPos.x + pST.fB[3] * vPos.z + pST.fC[3];
		if (f4 < 0.0f) continue;
		if (f1 == 0.0f && f3 ==  0.0f) continue;
		//float fDist = (pST.A * pV->pos.x + pST.B * pV->pos.z + pST.C) / sqrtf(SQR(pST.A)+SQR(pST.B));
		//dword dwAlpha = dword(255.0+*f * (1.0f - fDist / pST.fWidth));
		GetTrailUV(&pST, &pV->pos, &pV->Texture.x, &pV->Texture.y);
		float	fAlpha = 64.0f;
		long	iAlpha;
		dword	dwAlpha;
		
		if (pV->Texture.x <= 0.0f) 
			fAlpha *= (1.0f + pV->Texture.x * 4.0f);
		else if (pV->Texture.x >= 1.0f) 
			fAlpha *= ((1.25f - pV->Texture.x) * 4.0f);
	
		// using fShipTrailVSize for alpha calculate
		/*float fVPos = pST.fVPos + fabsf(pV->Texture.y - pST.fVStart);
		float fK = 1.0f - (fVPos+1.0f) / pS->fShipTrailVSize;
		if (fK<0.0f) fK = 0.0f;
		fK = 1.0f - powf(0.0001f,fK);
		fAlpha *= fK;*/

		if (pST.fTime <= 12.0f)
		{
			float fAlpha1 = 1.0f,fAlpha2;
			if (pST.fTime <= 6.0f) fAlpha1 = pST.fTime / 6.0f;
			fAlpha2 = pST.fTime / 6.0f - 1.0f;
			if (fAlpha2<0.0f) fAlpha2 = 0.0f;
			if (fAlpha1<0.0f) fAlpha1 = 0.0f;

			float fK = fabsf(pV->Texture.y - pST.fVStart) / (pST.fVEnd - pST.fVStart);

			fAlpha *= (fAlpha1 + fK * (fAlpha2 - fAlpha1));
		}

		FTOL(iAlpha,fAlpha);
		dwAlpha = (iAlpha<0) ? 0 : dword(iAlpha);
		pV->Diffuse |= ARGB(dwAlpha,0,0,0);//0xff000000;//ARGB(dwAlpha,0,0,0);
		break;
	}
}

void SEA::CalculateShipTrailTrapezium(seaship_t * pS)
{
	dword i, j;
	float fMaxX = -100000.0f, fMaxZ = -100000.0f, fMinX = 100000.0f, fMinZ = 100000.0f;
	if (pS->dwNumTrails) for (i=0; i<pS->dwNumTrails - 1; i++)
	{
		shiptrail_t * pST0 = &pS->pShipTrail[i];
		shiptrail_t * pST1 = &pS->pShipTrail[i+1];

		float fAng0,fAng1;
		fAng0 = pST0->fAngY;
		fAng1 = pST1->fAngY;

		CVECTOR vP1, vP2, vP3, vP4;
 
		vP1 = CVECTOR(0.0f,0.0f,pST0->fWidth);
		RotateAroundY(vP1.x,vP1.z,cosf(fAng0-PId2),sinf(fAng0-PId2));
		vP2 = -vP1; 
		vP2 += pST0->vPos;	vP1 += pST0->vPos;

		vP3 = CVECTOR(0.0f,0.0f,-pST1->fWidth);
		RotateAroundY(vP3.x,vP3.z,cosf(fAng1-PId2),sinf(fAng1-PId2));
		vP4 = -vP3; 
		vP4 += pST1->vPos;	vP3 += pST1->vPos;

		GetLineABC(vP1, vP2, pST0->fA[0], pST0->fB[0], pST0->fC[0]);
		GetLineABC(vP2, vP3, pST0->fA[1], pST0->fB[1], pST0->fC[1]);
		GetLineABC(vP3, vP4, pST0->fA[2], pST0->fB[2], pST0->fC[2]);
		GetLineABC(vP4, vP1, pST0->fA[3], pST0->fB[3], pST0->fC[3]);

		pST0->v[0] = vP1;
		pST0->v[1] = vP2;
		pST0->v[2] = vP3;
		pST0->v[3] = vP4;

		GetLineABC((vP1+vP2)/2.0f, (vP4+vP3)/2.0f, pST0->A, pST0->B, pST0->C);
		for (j=0; j<4; j++)
		{
			if (fMaxX < pST0->v[j].x) fMaxX = pST0->v[j].x;
			if (fMaxZ < pST0->v[j].z) fMaxZ = pST0->v[j].z;
			if (fMinX > pST0->v[j].x) fMinX = pST0->v[j].x;
			if (fMinZ > pST0->v[j].z) fMinZ = pST0->v[j].z;
		}
	}
	pS->fRect.x1 = fMinX;
	pS->fRect.x2 = fMaxX;
	pS->fRect.y1 = fMinZ;
	pS->fRect.y2 = fMaxZ;
}
