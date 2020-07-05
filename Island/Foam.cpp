#include "Foam.h"

CoastFoam::CoastFoam() : aFoams(_FL_)
{
	pSea = null;
	iVBuffer = -1;
	iIBuffer = -1;
	iCursorTex = -1;

	bEditMode = false;
	bCanEdit = false;
	bMoved = false;

	fMaxFoamDistance = 1000.0f;
	fFoamDeltaY = 0.2f;
	iFoamDivides = 4;

	fCursorPosX = 400.0f;
	fCursorPosY = 300.0f;
}

CoastFoam::~CoastFoam()
{
	Save();
	DelAll();
	
	if (iVBuffer >= 0) Render().ReleaseVertexBuffer(iVBuffer); iVBuffer = -1;
	if (iIBuffer >= 0) Render().ReleaseIndexBuffer(iIBuffer); iIBuffer = -1;
}

bool CoastFoam::Init()
{
	iVBuffer = Render().CreateVertexBuffer(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2, sizeof(FoamVertex) * 5000, D3DUSAGE_WRITEONLY);
	iIBuffer = Render().CreateIndexBuffer(10000 * 2 * 3);

	iCursorTex = Render().TextureCreate("cursor.tga");

	Load();

	bCanEdit = AttributesPointer->GetAttributeAsDword("edit", 0) != 0;
	return true;
}

void CoastFoam::Execute(dword Delta_Time)
{
	bEditMode = (bCanEdit) ? LOWORD(api->Controls->GetDebugKeyState(VK_NUMLOCK)) != 0 : false;
	float fDeltaTime = float(Delta_Time) * 0.001f;
}

void CoastFoam::ExtractRay (const D3DVIEWPORT8 & viewport, float fCursorX, float fCursorY, CVECTOR & raystart, CVECTOR & rayend)
{
	CMatrix matProj = Render().GetProjection();
	CVECTOR v;
	v.x =  ( ( ( 2.0f * fCursorX ) / viewport.Width  ) - 1 ) / matProj.m[0][0];
	v.y = -( ( ( 2.0f * fCursorY ) / viewport.Height ) - 1 ) / matProj.m[1][1];
	v.z =  1.0f;

	CMatrix mView3x3;
	CMatrix mView = Render().GetView();
	mView.Transposition ();
	mView.Get3X3(&mView3x3);

	CVECTOR raydir;
	CVECTOR rayorig;
	raydir = mView3x3 * v;
	rayorig = mView.Pos();

	raystart = rayorig;
	rayend = (rayorig + (raydir * 5000.f));
}

void CoastFoam::Realize(dword Delta_Time)
{
	pFrustumPlanes = Render().GetPlanes();

	ENTITY_ID sea_id;
	if (!pSea && api->FindClass(&sea_id, "sea", 0)) 
	{
		pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);
		if (!pSea) return;
	}

	float fDeltaTime = float(Delta_Time) * 0.001f;
	D3DVIEWPORT8 vp;
	Render().GetViewport(&vp);

	CMatrix mWorld; mWorld.SetIdentity(); //Render().GetWorld();
	CMatrix mView = Render().GetView();
	CMatrix mProjection = Render().GetProjection();

	CMatrix mWV, mWVP;
	mWV.EqMultiply(mWorld, mView);
	mWVP.EqMultiply(mWV, mProjection);

	dword dw1;
	dwNumPenasExecuted = 0;
	RDTSC_B(dw1);
	for (long i=0; i<aFoams; i++)
	{
		Foam * pF = aFoams[i];

		if (pF->Type == FOAM_TYPE_1)
			ExecuteFoamType1(pF, fDeltaTime);
		else
			ExecuteFoamType2(pF, fDeltaTime);
	}
	RDTSC_E(dw1);

	//Render().Print(0, 300, "dwNumPenasExecuted = %d, ticks = %d", dwNumPenasExecuted, dw1);

	if (!bCanEdit) return;

	array<RS_LINE> aLines(_FL_);	aLines.Empty(); 
	array<RS_RECT> aRects(_FL_);	aRects.Empty();
	
	if (bEditMode) for (long i=0; i<aFoams; i++)
	{
		aLines.Empty(); 
		aRects.Empty();

		Foam * pF = aFoams[i];
		for (long j=0; j<pF->aFoamParts; j++)
		{
			RS_LINE rl, r2;
			rl.dwColor = 0xFFFFFFFF;
			r2.dwColor = 0xFFFFFFFF;

			rl.vPos = pF->aFoamParts[j].v[0]; 
			r2.vPos = pF->aFoamParts[j].v[1]; 
			aLines.Add(rl); aLines.Add(r2);

			if (j != pF->aFoamParts.Len() - 1)
			{
				rl.vPos = pF->aFoamParts[j].v[0]; 
				r2.vPos = pF->aFoamParts[j + 1].v[0]; 
				aLines.Add(rl); aLines.Add(r2);

				rl.vPos = pF->aFoamParts[j].v[1]; 
				r2.vPos = pF->aFoamParts[j + 1].v[1]; 
				aLines.Add(rl); aLines.Add(r2);
			}

			rl.vPos = pF->aFoamParts[j].v[0]; 
			r2.vPos = pF->aFoamParts[j].v[1]; 
			aLines.Add(rl); aLines.Add(r2);

			if (bEditMode)
			{
				CVECTOR v1 = pF->aFoamParts[j].v[0];
				CVECTOR v2 = pF->aFoamParts[j].v[1];
				MTX_PRJ_VECTOR vP1, vP2;
				mWVP.Projection(&v1, &vP1, 1, float(vp.Width) * 0.5f, float(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
				mWVP.Projection(&v2, &vP2, 1, float(vp.Width) * 0.5f, float(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
				float fR1 = 0.15f;
				float fR2 = 0.15f;
				if (SQR(vP1.x - fCursorPosX) + SQR(vP1.y - fCursorPosY) < 100.0f) fR1 = 0.3f;
				if (SQR(vP2.x - fCursorPosX) + SQR(vP2.y - fCursorPosY) < 100.0f) fR2 = 0.3f;

				RS_RECT & r1 = aRects[aRects.Add()];
				r1.vPos = v1; r1.dwColor = 0xFFFFFF00; r1.fAngle = 0.0f; r1.fSize = fR1;
				
				RS_RECT & r2 = aRects[aRects.Add()];
				r2.vPos = v2; r2.dwColor = 0xFF00FF00; r2.fAngle = 0.0f; r2.fSize = fR2;
			}
		}

		CMatrix mI; mI.SetIdentity();
		Render().SetWorld(mI);
		if (bEditMode) Render().DrawLines(aLines.GetBuffer(), aLines.Len() / 2, "AILine");

		Render().DrawRects(aRects.GetBuffer(), aRects.Size(), "islanded_points");
	}

	bool bShift = api->Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0;
	bool bMenu = api->Controls->GetDebugAsyncKeyState(VK_MENU) < 0;
	if (bShift && api->Controls->GetDebugAsyncKeyState('L') < 0) Load();
	if (bShift && api->Controls->GetDebugAsyncKeyState('S') < 0) Save();

	if (bEditMode)
	{
		RS_SPRITE spr[4];

		float fMinDistance = 1e10f;
		if (!bMoved) 
		{
			iEditFoam = -1;
			iEditFoamPart = -1;
			iEditFoamVertex = -1;
			for (long i=0; i<aFoams; i++)
			{
				Foam * pF = aFoams[i];
				for (long j=0; j<pF->aFoamParts; j++)
				{
					for (long k=0; k<2; k++)
					{
						MTX_PRJ_VECTOR vP;
						CVECTOR v = pF->aFoamParts[j].v[k];

						mWVP.Projection(&v, &vP, 1, float(vp.Width) * 0.5f, float(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
						float fD = SQR(vP.x - fCursorPosX) + SQR(vP.y - fCursorPosY);
						if (fD < 100.0f && fD < fMinDistance)
						{
							fMinDistance = fD;

							iEditFoam = i;
							iEditFoamPart = j;
							iEditFoamVertex = k;
						}
					}
				}
			}
		}
		float x = fCursorPosX, dx = 32.0f;
		float y = fCursorPosY, dy = 32.0f;
		FillSpriteVertex(spr[0], x, y,				0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
		FillSpriteVertex(spr[1], x, y + dy,			0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
		FillSpriteVertex(spr[2], x + dx, y + dy,	0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
		FillSpriteVertex(spr[3], x + dx, y,			0.1f, 0xFFFFFFFF, 1.0f, 0.0f);

		Render().TextureSet(0, iCursorTex);
		Render().DrawSprites(spr, 1, "AISpite");

		bool bSelected = iEditFoam >= 0 && iEditFoamPart >= 0 && iEditFoamVertex >= 0;

		CONTROL_STATE cs, csH, csV;

		api->Controls->GetControlState("Turn H", csH);	fCursorPosX += csH.lValue;
		api->Controls->GetControlState("Turn V", csV);	fCursorPosY += csV.lValue;

		fCursorPosX = Max(0.0f, Min(fCursorPosX, float(vp.Width)));
		fCursorPosY = Max(0.0f, Min(fCursorPosY, float(vp.Height)));

		api->Controls->GetControlState("CoastFoamLB", cs);

		if (cs.state == CST_ACTIVE && !bMoved)
		{
			bMoved = true;
		}

		if (cs.state == CST_INACTIVE && bMoved)
		{
			bMoved = false;
		}
		CVECTOR vStart, vEnd;
		ExtractRay(vp, fCursorPosX, fCursorPosY, vStart, vEnd);

		if (bMoved && (csH.lValue || csV.lValue))
		{
			if (bSelected && (vStart.y > 0.0f && vStart.y > vEnd.y))
			{
				FoamPart * pFP = &aFoams[iEditFoam]->aFoamParts[iEditFoamPart];
				Plane pln(Vector(0.0f, 1.0f, 0.0f), Vector(0.0f));
				Vector vS1 = Vector(vStart.x, vStart.y, vStart.z);
				Vector vE1 = Vector(vEnd.x, vEnd.y, vEnd.z);
				Vector vR1;
				if (pln.Intersection(vS1, vE1, vR1))
				{
					CVECTOR vDiff = pFP->v[1] - pFP->v[0];
					CVECTOR vNewPos = CVECTOR(vR1.x, vR1.y, vR1.z);
					CVECTOR vDir = vNewPos - pFP->v[iEditFoamVertex];
					if (bMenu)
					{
						for (long i=0; i<aFoams[iEditFoam]->aFoamParts; i++)
						{
							aFoams[iEditFoam]->aFoamParts[i].v[0] += vDir;
							aFoams[iEditFoam]->aFoamParts[i].v[1] += vDir;
						}
					}
					else 
					{
						pFP->v[iEditFoamVertex] = vNewPos;

						if (iEditFoamVertex == 0 && bShift)
							pFP->v[1] = pFP->v[0] + vDiff;
					}

					RecalculateFoam( iEditFoam );
				}
			}
		}

		CONTROL_STATE csIns, csDel, csCopy;
		api->Controls->GetControlState("CoastFoamINS", csIns);
		api->Controls->GetControlState("CoastFoamDEL", csDel);
		api->Controls->GetControlState("CoastFoamCopy", csCopy);

		if (bShift && csCopy.state == CST_ACTIVATED && bSelected)
		{
			Foam * pF = NEW Foam();
			*pF = *aFoams[iEditFoam];
			
			for (long i=0; i<pF->aFoamParts; i++)
			{
				pF->aFoamParts[i].v[0] += CVECTOR(10.0f, 0.0f, 10.0f);
				pF->aFoamParts[i].v[1] += CVECTOR(10.0f, 0.0f, 10.0f);
			} 
			RecalculateFoam( aFoams.Add(pF) );
		}

		if (csIns.state == CST_ACTIVATED)
		{
			if (bShift)
			{
				// insert new foam
				CMatrix mIView = Render().GetView(); mIView.Transposition();
				Foam * pF = NEW Foam();

				pF->aFoamParts.Add();
				pF->aFoamParts.Add();

				InitNewFoam(pF);

				CVECTOR vPos = mIView.Pos(); vPos.y = 0.0f;
				CVECTOR vZ = mIView.Vz(); vZ.y = 0.0; vZ = !vZ;
				CVECTOR vX = mIView.Vx(); vX.y = 0.0; vX = !vX;

				pF->aFoamParts[0].v[0] = vPos;
				pF->aFoamParts[0].v[1] = vPos + vZ * 6.0f;

				pF->aFoamParts[1].v[0] = vPos + vX * 3.0f;
				pF->aFoamParts[1].v[1] = vPos + vX * 3.0f + vZ * 6.0f;

				RecalculateFoam( aFoams.Add(pF) );
			}
			else if (bSelected)
			{
				Foam * pF = aFoams[iEditFoam];
				if (iEditFoamPart == pF->aFoamParts.Len() - 1)
				{
					FoamPart * pFP = &pF->aFoamParts[pF->aFoamParts.Add()];
					FoamPart * pFP1 = &pF->aFoamParts[pF->aFoamParts.Last() - 2];
					FoamPart * pFP2 = &pF->aFoamParts[pF->aFoamParts.Last() - 1];
					pFP->v[0] = 2.0f * pFP2->v[0] - pFP1->v[0];
					pFP->v[1] = 2.0f * pFP2->v[1] - pFP1->v[1];
				}
				else
				{
					pF->aFoamParts.Insert(iEditFoamPart);
					pF->aFoamParts[iEditFoamPart] = pF->aFoamParts[iEditFoamPart + 1];
					pF->aFoamParts[iEditFoamPart + 1].v[0] = 0.5f * (pF->aFoamParts[iEditFoamPart].v[0] + pF->aFoamParts[iEditFoamPart + 2].v[0]);
					pF->aFoamParts[iEditFoamPart + 1].v[1] = 0.5f * (pF->aFoamParts[iEditFoamPart].v[1] + pF->aFoamParts[iEditFoamPart + 2].v[1]);
				}
				RecalculateFoam(iEditFoam);
			}
		}

		if (csDel.state == CST_ACTIVATED && bSelected)
		{
			if (bShift)
				aFoams.DelIndex(iEditFoam);
			else
			{
				aFoams[iEditFoam]->aFoamParts.DelIndex(iEditFoamPart);
				if (aFoams[iEditFoam]->aFoamParts.Len() <= 1)
					aFoams.DelIndex(iEditFoam);
				else
					RecalculateFoam(iEditFoam);
			}
		}
	}
}

void CoastFoam::InitNewFoam(Foam * pF)
{
	pF->fAlphaMin = 148.0f;		pF->fAlphaMax = 196.0f;
	pF->fAppearMax = 0.0f;		pF->fAppearMin = 0.0f;
	pF->fSpeedMin = 0.2f;		pF->fSpeedMax = 0.25f;
	pF->fTexScaleX = 0.1f;
	pF->Type = FOAM_TYPE_2;
	pF->sTexture = "foam.tga";
	pF->iNumFoams = 2;

	pF->iTexture = Render().TextureCreate(string("weather\\coastfoam\\") + pF->sTexture);
}

void CoastFoam::ExecuteFoamType2(Foam * pF, float fDeltaTime)
{
	long iLen = pF->aWorkParts.Len(); if (!iLen) return;

	CVECTOR vCamPos, vCamAng;
	float fPerspective;
	Render().GetCamera(vCamPos, vCamAng, fPerspective);

	float fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
	if (fDistance > fMaxFoamDistance) return;

	if (IsClipped(pF)) return;

	dwNumPenasExecuted++;

	for (long k=0; k<pF->iNumFoams; k++)
	{
		long kk = (k == 1) ? 0 : 1;

		if (pF->fMove[k] < 0.0f)
		{
			pF->fMove[k] += fDeltaTime;
			if (pF->fMove[k] < 0.0f) continue;
			pF->fMove[k] = 0.0f;
		}

		switch (pF->iMode[k])
		{
			case 0:
				pF->fAlpha[k] = Clampf(pF->fAlpha[k] + fDeltaTime * 0.15f);
				if (pF->fMove[k] > 0.6f) pF->iMode[k] = 1;
			break;
			case 1:
				pF->fAlpha[k] = Clampf(pF->fAlpha[k] + fDeltaTime * 0.15f);
				pF->fSpeed[k] *= (1.0f - Clampf(fDeltaTime * 0.8f));
				if (pF->fSpeed[k] < 0.01f && pF->fMove[kk] < 0.0f)
				{
					pF->fSX[kk] = Rnd(100.0f);
					pF->fAlpha[kk] = 0.0f;
					pF->iMode[kk] = 0; 
					pF->fSpeed[kk] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
					pF->fMove[kk] = 0.0f;
					pF->fAlphaColor[k] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
				}
				if (pF->fSpeed[k] < 0.01f)
				{
					pF->iMode[k] = 2;
				}
			break;
			case 2:
				pF->fSpeed[k] -= fDeltaTime * 0.035f;
				pF->fSpeed[k] = Max(-0.04f, pF->fSpeed[k]);
				pF->fAlpha[k] = Clampf(pF->fAlpha[k] - fDeltaTime * 0.2f);
				if (pF->fAlpha[k] <= 0.0f)
				{
					//pF->fMove[k] = -10000.0f;
					pF->fSX[k] = Rnd(100.0f);
					pF->fAlpha[k] = 0.0f;
					pF->iMode[k] = 0; 
					pF->fSpeed[k] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
					pF->fMove[k] = -RRnd(pF->fAppearMin, pF->fAppearMax);
					pF->fAlphaColor[k] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
				}
			break;
		}

		pF->fMove[k] += pF->fSpeed[k] * fDeltaTime;

		FoamVertex * pFV = (FoamVertex *)Render().LockVertexBuffer(iVBuffer, D3DLOCK_DISCARD);
		word * pI = (word *)Render().LockIndexBuffer(iIBuffer, D3DLOCK_DISCARD);

		long iNumVertices = 0;

		for (long y=0; y<8; y++)
		{
			float dy = y / 7.0f;
			float fAlpha = pF->fAlpha[k] * Clampf(2.5f * (1.0f - dy) * dy);
			for (long x=0; x<iLen; x++)
			{
				WorkPart * pWP = &pF->aWorkParts[x];

				float fAlpha1 = 1.0f;
				if (x <= 4) fAlpha1 = float(x) / 4.0f;
				if (x >= iLen - 5) fAlpha1 = float((iLen - 1) - x) / 4.0f;
				dword dwColor = ARGB(dword(pF->fAlphaColor[k] * fAlpha * fAlpha1 * 255.0f), 255, 255, 255);

				CVECTOR vPos = pWP->v[0] + (pF->fMove[k] * float(y) / 7.0f) * (pWP->v[1] - pWP->v[0]);
				vPos.y = fFoamDeltaY + pSea->WaveXZ(vPos.x, vPos.z);
				//vPos.y += 3.0f * fAmp * sinf(float(y) / 7.0f * PI);
				pFV[x + y * iLen].vPos = vPos;
				pFV[x + y * iLen].dwColor = dwColor;
				pFV[x + y * iLen].tu = pF->fSX[k] + pWP->tu;
				pFV[x + y * iLen].tv = 1.0f - dy;
				iNumVertices++;
			}
		}

		// setup ibuffer
		for (long y=0; y<7; y++)
		{
			for (long x=0; x<iLen - 1; x++)
			{
				*pI++ = word( (y + 0) * iLen + x );
				*pI++ = word( (y + 1) * iLen + x );
				*pI++ = word( (y + 0) * iLen + x + 1 );

				*pI++ = word( (y + 1) * iLen + x );
				*pI++ = word( (y + 1) * iLen + x + 1 );
				*pI++ = word( (y + 0) * iLen + x + 1 );
			}	
		}

		Render().UnLockIndexBuffer(iIBuffer);
		Render().UnLockVertexBuffer(iVBuffer);

		CMatrix mI; mI.SetIdentity();
		Render().SetWorld(mI);
		Render().TextureSet(0, pF->iTexture);
		Render().DrawBuffer(iVBuffer, sizeof(FoamVertex), iIBuffer, 0, iNumVertices, 0, 7 * 2 * (iLen - 1), "CoastFoam");
	}
}

bool CoastFoam::IsClipped(Foam * pF)
{
	if (!pF) return true;

	float fPerspective;
	CVECTOR vCamPos, vCamAng;
	Render().GetCamera(vCamPos, vCamAng, fPerspective);

	CVECTOR		vP[4];
	dword		dwPlanesPoints[4];
	dword		dwSize = pF->aFoamParts.Size();

	for (dword k=0; k<4; k++) 
	{
		dwPlanesPoints[k] = 0;
		vP[k] = CVECTOR(pFrustumPlanes[k].Nx, pFrustumPlanes[k].Ny, pFrustumPlanes[k].Nz);
	}

	for (dword i=0; i<dwSize; i++) 
	{
		CVECTOR v0 = pF->aFoamParts[i].v[0];
		CVECTOR v1 = pF->aFoamParts[i].v[1];

		for (dword j=0; j<4; j++)
		{
			float fD1 = (v0 | vP[j]) - pFrustumPlanes[j].D;
			float fD2 = (v1 | vP[j]) - pFrustumPlanes[j].D;

			if (fD1 < 0.0f) dwPlanesPoints[j]++;
			if (fD2 < 0.0f) dwPlanesPoints[j]++;
		}
	}

	for (dword z=0; z<4; z++)
		if (dwPlanesPoints[z] == dwSize * 2) return true;

	return false;
}

void CoastFoam::ExecuteFoamType1(Foam * pF, float fDeltaTime)
{
	long iLen = pF->aWorkParts.Len(); if (!iLen) return;

	CVECTOR vCamPos, vCamAng;
	float fPerspective;
	Render().GetCamera(vCamPos, vCamAng, fPerspective);

	float fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
	if (fDistance > fMaxFoamDistance) return;

	if (IsClipped(pF)) return;

	FoamVertex * pFV = (FoamVertex *)Render().LockVertexBuffer(iVBuffer, D3DLOCK_DISCARD);
	word * pI = (word *)Render().LockIndexBuffer(iIBuffer, D3DLOCK_DISCARD);

	long iNumVertices = 0;

	dwNumPenasExecuted++;

	for (long y=0; y<8; y++)
	{
		float dy = y / 7.0f;
		float fAlpha = Clampf(2.5f * (1.0f - dy) * dy);
		dword dwColor = ARGB(dword(fAlpha * 255.0f), 255, 255, 255);
		for (long x=0; x<iLen; x++)
		{
			WorkPart * pWP = &pF->aWorkParts[x];

			pWP->p[y].fPos += pWP->p[y].fSpeed * fDeltaTime * 0.1f;

			float fAlpha1 = fAlpha;
			if (pWP->p[y].fPos > 0.9f)
			{
				fAlpha1 = fAlpha * Clampf(1.0f - pWP->p[y].fPos) / 0.1f;
			}
			if (x <= 4)
				fAlpha1 *= float(x) / 4.0f;

			if (x >= iLen - 4)
				fAlpha1 *= float((iLen - 1) - x) / 4.0f;

			dword dwColor = ARGB(dword(fAlpha1 * 255.0f), 255, 255, 255);

			float fAmp = (1.0f - pWP->p[y].fPos) / 4.0f;
			if (y == 0 && pWP->p[y].fPos >= 0.6f)
			{
				//pWP->p[y].fSpeed *= 0.95f;
				for (long k=0; k<8; k++)
					pWP->p[k].fSpeed *= (1.0f - Clampf(fDeltaTime * 2.0f));
			}

			CVECTOR vPos = pWP->v[0] + pWP->p[y].fPos * (pWP->v[1] - pWP->v[0]);
			vPos.y = fFoamDeltaY + pSea->WaveXZ(vPos.x, vPos.z);
			//vPos.y += 3.0f * fAmp * sinf(float(y) / 7.0f * PI);
			pFV[x + y * iLen].vPos = vPos;
			pFV[x + y * iLen].dwColor = dwColor;
			pFV[x + y * iLen].tu = pWP->tu;
			pFV[x + y * iLen].tv = dy * 2.0f;
			iNumVertices++;
		}
	}

	// setup ibuffer
	for (long y=0; y<7; y++)
	{
		for (long x=0; x<iLen - 1; x++)
		{
			*pI++ = word( (y + 0) * iLen + x );
			*pI++ = word( (y + 1) * iLen + x );
			*pI++ = word( (y + 0) * iLen + x + 1 );

			*pI++ = word( (y + 1) * iLen + x );
			*pI++ = word( (y + 1) * iLen + x + 1 );
			*pI++ = word( (y + 0) * iLen + x + 1 );
		}	
	}

	Render().UnLockIndexBuffer(iIBuffer);
	Render().UnLockVertexBuffer(iVBuffer);

	CMatrix mI; mI.SetIdentity();
	Render().SetWorld(mI);
	Render().TextureSet(0, pF->iTexture);
	Render().DrawBuffer(iVBuffer, sizeof(FoamVertex), iIBuffer, 0, iNumVertices, 0, 7 * 2 * (iLen - 1), "CoastFoam");
}

void CoastFoam::RecalculateFoam(long iFoam)
{
	Foam * pF = aFoams[iFoam];

	pF->aWorkParts.DelAll();

	pF->fMove[0] = -RRnd(pF->fAppearMin, pF->fAppearMax);
	pF->fMove[1] = -100000.0f;//RRnd(pF->fAppearMin, pF->fAppearMax);
	pF->fSpeed[0] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
	pF->fSpeed[1] = RRnd(pF->fSpeedMin, pF->fSpeedMax);

	pF->fAlphaColor[0] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
	pF->fAlphaColor[1] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;

	pF->fSX[0] = Rnd(100.0f);		pF->fSX[0] = Rnd(100.0f);
	pF->fAlpha[0] = 0.0f;			pF->fAlpha[1] = 0.0f;
	pF->iMode[0] = 0;				pF->iMode[1] = 0;

	float sx = 0.0f;
	float ii = 0.0f;
	for (long i=0; i<pF->aFoamParts.Len() - 1; i++)
	{
		FoamPart * pF1 = &pF->aFoamParts[i];
		FoamPart * pF2 = &pF->aFoamParts[i + 1];

		float dx = (pF1->v[0] - pF2->v[0]).GetLength() / float(iFoamDivides);
		for (long j=0; j<iFoamDivides; j++)
		{
			if (j == 0 && i != 0) continue;

			WorkPart * pWP = &pF->aWorkParts[pF->aWorkParts.Add()];
			pWP->tu = sx * pF->fTexScaleX;
			pWP->v[0] = pF1->v[1] + float(j) / float(iFoamDivides - 1) * (pF2->v[1] - pF1->v[1]);
			pWP->v[1] = pF1->v[0] + float(j) / float(iFoamDivides - 1) * (pF2->v[0] - pF1->v[0]);

			float fStartPos = sinf(ii / 14.0f * PI) * 0.1f;
			for (long k=0; k<8; k++)
			{
				pWP->p[k].fPos = fStartPos + (float(k) / 7.0f) * 0.4f;
				pWP->p[k].fSpeed = 2.0f;//RRnd(pF->fSpeedMin, pF->fSpeedMax);
				//pWP->p[k].fA = 0.0f;
			}

			sx += dx;
			ii++;
		}
	}
}

void CoastFoam::Save()
{
	if (!bCanEdit) return;

	char cKey[128], cSection[128], cTemp[1024];
	string sID = string("resource\\foam\\locations\\") + AttributesPointer->GetAttribute("id") + ".ini";
	fio->_DeleteFile(sID);
	
	INIFILE * pI = fio->CreateIniFile(sID.c_str(), false);
	if (!pI) return;

	pI->WriteLong(null, "NumFoams", aFoams.Len());
	pI->WriteDouble(null, "MaxFoamDistance", fMaxFoamDistance);
	pI->WriteDouble(null, "FoamDeltaY", fFoamDeltaY);
	pI->WriteLong(null, "FoamDivides", iFoamDivides);

	for (long i=0; i<aFoams; i++)
	{
		Foam * pF = aFoams[i];
		sprintf(cSection, "foam_%d", i);
		pI->WriteLong(cSection, "NumParts", pF->aFoamParts.Len());
		
		sprintf(cTemp, "%.0f, %.0f", pF->fAlphaMin, pF->fAlphaMax);
		pI->WriteString(cSection, "Alpha", cTemp);
		
		sprintf(cTemp, "%.3f, %.3f", pF->fSpeedMin, pF->fSpeedMax);
		pI->WriteString(cSection, "Speed", cTemp);
		
		sprintf(cTemp, "%.3f, %.3f", pF->fBrakingMin, pF->fBrakingMax);
		pI->WriteString(cSection, "Braking", cTemp);

		sprintf(cTemp, "%.3f, %.3f", pF->fAppearMin, pF->fAppearMax);
		pI->WriteString(cSection, "Appear", cTemp);

		sprintf(cTemp, "%.3f", pF->fTexScaleX);
		pI->WriteString(cSection, "TexScaleX", cTemp);

		pI->WriteLong(cSection, "NumFoams", pF->iNumFoams);

		pI->WriteString(cSection, "Texture", (char*)pF->sTexture.c_str());
		pI->WriteLong(cSection, "Type", pF->Type);

		for (long j=0; j<pF->aFoamParts; j++)
		{
			FoamPart * pFP = &pF->aFoamParts[j];
			sprintf(cKey, "key_%d", j);
			sprintf(cTemp, "%.4f, %.4f, %.4f, %.4f", pFP->v[0].x, pFP->v[0].z, pFP->v[1].x, pFP->v[1].z);
			pI->WriteString(cSection, cKey, cTemp);
		}
	}
	DELETE(pI);
	_flushall();
}

void CoastFoam::DelAll()
{
	for (long i=0; i<aFoams; i++) if (aFoams[i]->iTexture >= 0) Render().TextureRelease(aFoams[i]->iTexture);
	aFoams.DelAllWithPointers();
}

void CoastFoam::Load()
{
	char cSection[256], cKey[256], cTemp[1024];

	string sID = string("resource\\foam\\locations\\") + AttributesPointer->GetAttribute("id") + ".ini";
	INIFILE * pI = fio->OpenIniFile(sID.c_str());
	if (!pI) return;
	
	DelAll();
	long iNumFoams = pI->GetLong(null, "NumFoams", 0);
	fMaxFoamDistance = pI->GetFloat(null, "MaxFoamDistance", 1000.0f);
	fFoamDeltaY = pI->GetFloat(null, "FoamDeltaY", 0.2f);
	iFoamDivides = pI->GetLong(null, "FoamDivides", 4);
	for (long i=0; i<iNumFoams; i++)
	{
		Foam * pF = aFoams[aFoams.Add(NEW Foam)];
		sprintf(cSection, "foam_%d", i);

		long iNumParts = pI->GetLong(cSection, "NumParts", 0);

		pI->ReadString(cSection, "Alpha", cTemp, sizeof(cTemp), "148, 196");
		sscanf(cTemp, "%f, %f", &pF->fAlphaMin, &pF->fAlphaMax);

		pI->ReadString(cSection, "Speed", cTemp, sizeof(cTemp), "0.200, 0.30");
		sscanf(cTemp, "%f, %f", &pF->fSpeedMin, &pF->fSpeedMax);

		pI->ReadString(cSection, "Braking", cTemp, sizeof(cTemp), "0.000, 0.000");
		sscanf(cTemp, "%f, %f", &pF->fBrakingMin, &pF->fBrakingMax);

		pI->ReadString(cSection, "Appear", cTemp, sizeof(cTemp), "0.000, 0.200");
		sscanf(cTemp, "%f, %f", &pF->fAppearMin, &pF->fAppearMax);

		pI->ReadString(cSection, "TexScaleX", cTemp, sizeof(cTemp), "0.050");
		sscanf(cTemp, "%f", &pF->fTexScaleX);

		pF->iNumFoams = (pI->GetLong(cSection, "NumFoams", 2) == 2) ? 2 : 1;

		pI->ReadString(cSection, "Texture", cTemp, sizeof(cTemp), "foam.tga");
		pF->sTexture = cTemp;
		pF->iTexture = Render().TextureCreate(string("weather\\coastfoam\\") + cTemp);
		pF->Type = (FOAMTYPE)pI->GetLong(cSection, "Type", FOAM_TYPE_2);

		for (long j=0; j<((iNumParts) ? iNumParts : 100000); j++)
		{
			sprintf(cKey, "key_%d", j);
			CVECTOR v1, v2; v1.y = v2.y = 0.0f;
			pI->ReadString(cSection, cKey, cTemp, sizeof(cTemp), "");
			if (!cTemp[0]) break;
			sscanf(cTemp, "%f, %f, %f, %f", &v1.x, &v1.z, &v2.x, &v2.z);
			pF->aFoamParts.Add();
			pF->aFoamParts.LastE().v[0] = v1;
			pF->aFoamParts.LastE().v[1] = v2;
		}
		pF->fMove[0] = -RRnd(pF->fAppearMin, pF->fAppearMax);
		pF->fMove[1] = -100000.0f;//RRnd(pF->fAppearMin, pF->fAppearMax);
		pF->fSpeed[0] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
		pF->fSpeed[1] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
		pF->fSX[0] = Rnd(100.0f);
		pF->fSX[1] = Rnd(100.0f);
		//pF->fBraking[0] = RRnd(pF->fBrakingMin, pF->fBrakingMax);
		//pF->fBraking[1] = RRnd(pF->fBrakingMin, pF->fBrakingMax);
		pF->fAlphaColor[0] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
		pF->fAlphaColor[1] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
		pF->fAlpha[0] = 0.0f;
		pF->fAlpha[1] = 0.0f;
		pF->iMode[0] = 0;
		pF->iMode[1] = 0;

		RecalculateFoam(i);
	}
	DELETE(pI);
}

dword CoastFoam::AttributeChanged(ATTRIBUTES * pA)
{
	return 0;
}
