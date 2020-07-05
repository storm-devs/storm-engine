#include "Foam.h"

NetCoastFoam::NetCoastFoam() : aFoams(_FL_)
{
	pSea = null;
	iVBuffer = -1;
	iIBuffer = -1;
	iCursorTex = -1;

	bEditMode = false;
	bMoved = false;

	fMaxFoamDistance = 1000.0f;
	fFoamDeltaY = 0.2f;
	iFoamDivides = 4;

	fCursorPosX = 400.0f;
	fCursorPosY = 300.0f;
}

NetCoastFoam::~NetCoastFoam()
{
	DelAll();
	
	if (iVBuffer >= 0) Render().ReleaseVertexBuffer(iVBuffer); iVBuffer = -1;
	if (iIBuffer >= 0) Render().ReleaseIndexBuffer(iIBuffer); iIBuffer = -1;
}

bool NetCoastFoam::Init()
{
	iVBuffer = Render().CreateVertexBuffer(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2, sizeof(FoamVertex) * 5000, D3DUSAGE_WRITEONLY);
	iIBuffer = Render().CreateIndexBuffer(10000 * 2 * 3);

	iCursorTex = Render().TextureCreate("cursor.tga");

	Load();

	return true;
}

void NetCoastFoam::Execute(dword Delta_Time)
{
	bEditMode = LOWORD(api->Controls->GetDebugKeyState(VK_NUMLOCK)) != 0;

	float fDeltaTime = float(Delta_Time) * 0.001f;
}

void NetCoastFoam::Realize(dword Delta_Time)
{
	ENTITY_ID sea_id;
	if (!pSea && NetFindClass(IsServer(), &sea_id, "NetSea")) 
	{
		pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);
		if (!pSea) return;
	}

	pFrustumPlanes = Render().GetPlanes();

	float fDeltaTime = float(Delta_Time) * 0.001f;

	D3DVIEWPORT8 vp;
	Render().GetViewport(&vp);

	CMatrix mWorld = Render().GetWorld();
	CMatrix mView = Render().GetView();
	CMatrix mProjection = Render().GetProjection();

	CMatrix mWV, mWVP;
	mWV.EqMultiply(mWorld, mView);
	mWVP.EqMultiply(mWV, mProjection);

	for (long i=0; i<aFoams; i++)
	{
		Foam * pF = aFoams[i];

		if (pF->Type == FOAM_TYPE_1)
			ExecuteFoamType1(pF, fDeltaTime);
		else
			ExecuteFoamType2(pF, fDeltaTime);
	}
}

void NetCoastFoam::InitNewFoam(Foam * pF)
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

void NetCoastFoam::ExecuteFoamType2(Foam * pF, float fDeltaTime)
{
	long iLen = pF->aWorkParts.Len();

	CVECTOR vCamPos, vCamAng;
	float fPerspective;
	Render().GetCamera(vCamPos, vCamAng, fPerspective);

	float fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
	if (fDistance > fMaxFoamDistance) return;

	if (IsClipped(pF)) return;

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
					pF->fMove[k] = -10000.0f;
					/*pF->fSX[k] = Rnd(100.0f);
					pF->fAlpha[k] = 0.0f;
					pF->iMode[k] = 0; 
					pF->fSpeed[k] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
					pF->fMove[k] = -RRnd(pF->fAppearMin, pF->fAppearMax);
					pF->fAlphaColor[k] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;*/
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

bool NetCoastFoam::IsClipped(Foam * pF)
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

void NetCoastFoam::ExecuteFoamType1(Foam * pF, float fDeltaTime)
{
	long iLen = pF->aWorkParts.Len();

	CVECTOR vCamPos, vCamAng;
	float fPerspective;
	Render().GetCamera(vCamPos, vCamAng, fPerspective);

	float fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
	if (fDistance > fMaxFoamDistance) return;

	if (IsClipped(pF)) return;

	FoamVertex * pFV = (FoamVertex *)Render().LockVertexBuffer(iVBuffer, D3DLOCK_DISCARD);
	word * pI = (word *)Render().LockIndexBuffer(iIBuffer, D3DLOCK_DISCARD);

	long iNumVertices = 0;

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
			vPos.y = 0.1f + pSea->WaveXZ(vPos.x, vPos.z);
			vPos.y += 3.0f * fAmp * sinf(float(y) / 7.0f * PI);
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

void NetCoastFoam::RecalculateFoam(long iFoam)
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

void NetCoastFoam::DelAll()
{
	for (long i=0; i<aFoams; i++) if (aFoams[i]->iTexture >= 0) Render().TextureRelease(aFoams[i]->iTexture);
	aFoams.DelAllWithPointers();
}

void NetCoastFoam::Load()
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

dword NetCoastFoam::AttributeChanged(ATTRIBUTES * pA)
{
	return 0;
}
