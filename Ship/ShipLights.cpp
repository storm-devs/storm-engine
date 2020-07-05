#include "Ship.h"
#include "ShipLights.h"

VDX8RENDER * ShipLights::pRS = null;
COLLIDE * ShipLights::pCollide = null;

ShipLights::ShipLights() : aLights(_FL_, 128), aLightTypes(_FL_), aSelectedLights(_FL_)
{
	bLoadLights = false;
	bReflection = false;

	dwMaxD3DLights = 6;

	iFlareSunRoadTex = -1;
	iCoronaTex = -1;

	iMinLight = 1000;
	iMaxLight = -1;

	pSea = null;
}

ShipLights::~ShipLights()
{
	if (iCoronaTex >= 0) pRS->TextureRelease(iCoronaTex);
	if (iFlareSunRoadTex >= 0) pRS->TextureRelease(iFlareSunRoadTex);

	aLights.DelAll();
	aSelectedLights.DelAll();
	aLightTypes.DelAll();
	
	bLoadLights = false;
}

bool ShipLights::Init()
{
	ENTITY_ID sea_id;

	pRS = (VDX8RENDER *)api->CreateService("dx8render");	Assert(pRS);
	pCollide = (COLLIDE *)api->CreateService("coll");		Assert(pCollide);
	if (api->FindClass(&sea_id, "sea", 0)) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);
	return true;
}

void ShipLights::Release(VAI_OBJBASE * pObject)
{
}

float ShipLights::GetAttributeAsFloat(ATTRIBUTES * pA, const char * pName, float fDefault)
{
	if (!pName) return fDefault;
	ATTRIBUTES * pAF = pA->FindAClass(pA, (char*)pName);
	return pAF ? pAF->GetAttributeAsFloat() : fDefault;
}

bool ShipLights::LoadLights()
{
	aLightTypes.DelAll();

	bLoadLights = false;

	ATTRIBUTES * pA = this->AttributesPointer;
	ATTRIBUTES * pALights = pA->FindAClass(pA, "LightTypes");
	if (!pALights) return false;

	for (dword i=0; i<pALights->GetAttributesNum(); i++)
	{
		ATTRIBUTES * pAL = pALights->GetAttributeClass(i);

		LightType * pLT = &aLightTypes[aLightTypes.Add()];
		pLT->sLightType = pAL->GetThisName();
		pLT->cLightColor.r = GetAttributeAsFloat(pAL, "light.r", 1.0f);
		pLT->cLightColor.g = GetAttributeAsFloat(pAL, "light.g", 1.0f);
		pLT->cLightColor.b = GetAttributeAsFloat(pAL, "light.b", 1.0f);
		pLT->cLightColor.a = 1.0f;
		pLT->cCoronaColor.r = GetAttributeAsFloat(pAL, "corona.r", 1.0f);
		pLT->cCoronaColor.g = GetAttributeAsFloat(pAL, "corona.g", 1.0f);
		pLT->cCoronaColor.b = GetAttributeAsFloat(pAL, "corona.b", 1.0f);
		pLT->cCoronaColor.a = 1.0f;
		pLT->fCoronaRange = GetAttributeAsFloat(pAL, "corona.Range", 20.0f);
		pLT->fCoronaSize = GetAttributeAsFloat(pAL, "corona.Size", 1.0f);
		pLT->fRange = GetAttributeAsFloat(pAL, "light.range", 10.0f);
		pLT->fAttenuation0 = GetAttributeAsFloat(pAL, "light.Attenuation0", 1.0f);
		pLT->fAttenuation1 = GetAttributeAsFloat(pAL, "light.Attenuation1", 0.0f);
		pLT->fAttenuation2 = GetAttributeAsFloat(pAL, "light.Attenuation2", 0.0f);
		pLT->fFlicker = GetAttributeAsFloat(pAL, "Oscillator1.Flicker", 0.1f);
		pLT->fFreq = GetAttributeAsFloat(pAL, "Oscillator1.Freq", 15.0f);
		pLT->fFlickerSlow = GetAttributeAsFloat(pAL, "Oscillator2.Flicker", 0.5f);
		pLT->fFreqSlow = GetAttributeAsFloat(pAL, "Oscillator2.Freq", 1.0f);
		pLT->fLifeTime = GetAttributeAsFloat(pAL, "LifeTime", 0.5f);
		pLT->fUpTime = GetAttributeAsFloat(pAL, "UpTime", 0.2f);
		pLT->fSunRoadFlareFadeDistance = GetAttributeAsFloat(pAL, "SunRoadFlareFadeDistance", 100.0f);
	}

	dwMaxD3DLights = Min(dword(7), pA->GetAttributeAsDword("MaxD3DLights", 7));

	sCoronaTechnique = (pA->GetAttribute("CoronaTechnique")) ? pA->GetAttribute("CoronaTechnique") : "";
	iCoronaTex = (pA->GetAttribute("CoronaTexture")) ? pRS->TextureCreate(pA->GetAttribute("CoronaTexture")) : -1;
	iFlareSunRoadTex = (pA->GetAttribute("FlareSunRoadTexture")) ? pRS->TextureCreate(pA->GetAttribute("FlareSunRoadTexture")) : -1;
	dwCoronaSubTexX = pA->GetAttributeAsDword("CoronaTextureX", 1);
	dwCoronaSubTexY = pA->GetAttributeAsDword("CoronaTextureY", 1);
	fSunRoadFlareSize = pA->GetAttributeAsFloat("SunRoadFlareSize", 4.0);

	bLoadLights = true;
	return true;
}

ShipLights::LightType * ShipLights::FindLightType(string sLightType)
{
	for (dword i=0; i<aLightTypes.Size(); i++)
		if (aLightTypes[i].sLightType == sLightType) return &aLightTypes[i];

	return null;
}

void ShipLights::AddDynamicLights(VAI_OBJBASE * pObject, const CVECTOR & vPos)
{
	if (!bLoadLights && !LoadLights()) return;

	dword dwNum = 0;
	for (dword i=0; i<aLights.Size(); i++) if (aLights[i].pObject == pObject && aLights[i].bDynamicLight) dwNum++;
	if (dwNum >= 4) return;

	ShipLight * pL;
	string sLightType = "cannondefault";
	LightType * pLT = FindLightType(sLightType);
	if (!pLT) 
	{
		api->Trace("Can find ship light \"%s\"", (const char *)sLightType);
		return;
	}

	pL = &aLights[aLights.Add()];
	pL->bDynamicLight = true;
	pL->pObject = pObject;
	pL->vPos = vPos;
	pL->bOff = false;
	pL->bBrokenTimeOff = false;
	pL->fCurTime = 0.0;
	pL->fTotalTime = pLT->fLifeTime;
	pL->fUpTime = pLT->fUpTime;
	pL->vCurPos = vPos;
	pL->bCoronaOnly = false;
	pL->fFlareAlpha = 0.0f;
	pL->fFlareAlphaMax = 1.0f;
	pL->bVisible = false;
	pL->fBrokenTime = 0.0f;
	pL->bDead = false;
	pL->fTotalBrokenTime = 0.0f;

	ZERO(pL->Light);
	pL->Light.Type = D3DLIGHT_POINT;
	pL->Light.Diffuse.r = pLT->cLightColor.r;
	pL->Light.Diffuse.g = pLT->cLightColor.g;
	pL->Light.Diffuse.b = pLT->cLightColor.b;
	pL->Light.Diffuse.a = 1.0f;
	pL->Light.Range = pLT->fRange;
	pL->Light.Attenuation0 = pLT->fAttenuation0;
	pL->Light.Attenuation1 = pLT->fAttenuation1;
	pL->Light.Attenuation2 = pLT->fAttenuation2;

	pL->pLT = pLT;
}

bool ShipLights::SetLabel(ShipLight * pL, MODEL * pModel, const char * pStr)
{
	pL->pNode = pModel->FindNode(pStr);
	if (!pL->pNode) return false;

	CMatrix mNode = pL->pNode->glob_mtx;
	mNode.Transposition();
	pL->vPos = mNode * pL->vPos;
	return true;
}

void ShipLights::AddFlare(VAI_OBJBASE * pObject, bool bLight, MODEL * pModel, const GEOS::LABEL & label)
{
	CMatrix m;
	char str[256], str2[256];
	ZERO(str);
	if (!label.name) return;
	strcpy(str, label.name);
	strlwr(str);

	ShipLight * pL = &aLights[aLights.Add()];
	memcpy(m, label.m, sizeof(m));

	pL->pNode = null;
	pL->vPos = m.Pos();

	if (str[0] != 'f' && !bLight) return;
	if (str[0] != 'l' && bLight) return;

	// mast found
	if (str[1] == 'm')
	{
		str2[0] = str[2]; str2[1] = '\0';
		int iMastIndex = atoi(str2);

		sprintf(str2, "mast%d", iMastIndex);
		// rey found
		if (str[3] >= 'a' && str[3] <= 'z')
		{
			sprintf(str2, "rey_%c%d", str[3], iMastIndex);
		}

		if (!SetLabel(pL, pModel, str2))
		{
			aLights.DelIndex(aLights.Last());
			return;
		}
	}

	LightType * pLT = FindLightType((bLight) ? "default" : "flare");

	if (!pLT) return;

	pL->bDynamicLight = false;
	pL->pObject = pObject;
	pL->bCoronaOnly = true;
	pL->fFlareAlpha = 0.0f;
	pL->fFlareAlphaMax = 1.0f;
	pL->bVisible = true;
	pL->fBrokenTime = 0.0f;
	pL->fTotalBrokenTime = 0.0f;
	pL->bOff = false;
	pL->bBrokenTimeOff = false;
	pL->bDead = false;

	if (bLight)
	{
		ZERO(pL->Light);
		pL->bCoronaOnly = false;
		pL->Light.Type = D3DLIGHT_POINT;
		pL->Light.Diffuse.r = pLT->cLightColor.r;
		pL->Light.Diffuse.g = pLT->cLightColor.g;	
		pL->Light.Diffuse.b = pLT->cLightColor.b;
		pL->Light.Diffuse.a = 1.0f;
		pL->Light.Range = pLT->fRange;
		pL->Light.Attenuation0 = pLT->fAttenuation0;
		pL->Light.Attenuation1 = pLT->fAttenuation1;
		pL->Light.Attenuation2 = pLT->fAttenuation2;
	}
	
	pL->Osc[0].fStep = pLT->fFreq;
	pL->Osc[0].fAmp = pLT->fFlicker;
	pL->Osc[0].fOneDivAmp = 1.0f / pL->Osc[0].fAmp;
	pL->Osc[0].fK = 0.0f;
	pL->Osc[0].fOldValue = RRnd(-pL->Osc[0].fAmp, pL->Osc[0].fAmp);
	pL->Osc[0].fNewValue = RRnd(-pL->Osc[0].fAmp, pL->Osc[0].fAmp);

	pL->Osc[1].fStep = pLT->fFreqSlow;
	pL->Osc[1].fAmp = pLT->fFlickerSlow;
	pL->Osc[1].fOneDivAmp = 1.0f / pL->Osc[1].fAmp;
	pL->Osc[1].fK = 0.0f;
	pL->Osc[1].fOldValue = RRnd(-pL->Osc[1].fAmp, pL->Osc[1].fAmp);
	pL->Osc[1].fNewValue = RRnd(-pL->Osc[1].fAmp, pL->Osc[1].fAmp);

	pL->pLT = pLT;
}

void ShipLights::SetLightsOff(VAI_OBJBASE * pObject, float fTime, bool bLights, bool bFlares, bool bNow)
{
	for (dword i=0; i<aLights.Size(); i++) if (aLights[i].pObject == pObject && !aLights[i].bOff)
	{
		if (aLights[i].bCoronaOnly)
		{
			if (aLights[i].bOff == bFlares || (aLights[i].fTotalBrokenTime > 0.0f && aLights[i].bBrokenTimeOff == bFlares)) continue;
			aLights[i].bBrokenTimeOff = bFlares;
		}	
		else
		{
			if (aLights[i].bOff == bLights || (aLights[i].fTotalBrokenTime > 0.0f && aLights[i].bBrokenTimeOff == bLights)) continue;
			aLights[i].bBrokenTimeOff = bLights;
		}

		aLights[i].fBrokenTime = 0.0f;
		aLights[i].fTotalBrokenTime = Rnd(fTime);

		if (bNow)
		{
			aLights[i].fTotalBrokenTime = 0.0f;
			aLights[i].bOff = aLights[i].bBrokenTimeOff;
		}
	}
}

void ShipLights::KillMast(VAI_OBJBASE * pObject, NODE * pNode, bool bNow)
{
	for (dword i=0; i<aLights.Size(); i++) if (aLights[i].pObject == pObject && aLights[i].pNode)
	{
		if (aLights[i].bOff) continue;
		if (aLights[i].pNode->parent != pNode && aLights[i].pNode != pNode) continue;
		if (bNow)
		{
			aLights[i].pNode = null;
			aLights[i].bOff = true;
			aLights[i].bBrokenTimeOff = true;
			//aLights.ExtractNoShift(i);
			//i--;
		}
		else
		{
			// slow 
			aLights[i].fTotalBrokenTime = 0.1f + FRAND(3.0f);
			aLights[i].fBrokenTime = 0.0f;
			aLights[i].bBrokenTimeOff = true;
		}
	}
}

void ShipLights::AddLights(VAI_OBJBASE * pObject, MODEL * pModel, bool bLights, bool bFlares)
{
	if (!bLoadLights && !LoadLights()) return;

	string sLightType = "default";

	LightType * pLT = FindLightType(sLightType);
	if (!pLT) 
	{
		api->Trace("Can't find ship light \"%s\"", (const char *)sLightType);
		return;
	}

	GEOS::LABEL	label;
	GEOS::INFO info;
	NODE * pNode;

	NODE * pRoot = pModel->GetNode(0);

	string sFlares = "flares";
	string sLights = "lights";
	dword dwIdx = 0;
	while (pNode = pModel->GetNode(dwIdx))
	{
		pNode->geo->GetInfo(info);

		for (dword i=0; i<dword(info.nlabels); i++)
		{
			pNode->geo->GetLabel(i, label);

			if (bFlares && sFlares == label.group_name)
			{
				AddFlare(pObject, false, pModel, label);
				continue;
			}

			if (bLights && sLights == label.group_name)
			{
				AddFlare(pObject, true, pModel, label);
				continue;
			}
		}
		dwIdx++;
	}
}

void ShipLights::SetLights(VAI_OBJBASE * pObject)
{
	dword i;
	aSelectedLights.Empty();

	for (i=0; i<aLights.Size(); i++)
	{
		if (aLights[i].bOff) continue;
		if (!aLights[i].bDynamicLight)
		{
			if (aLights[i].pNode)
			{
				CVECTOR vPos = aLights[i].vPos;
				aLights[i].vCurPos = aLights[i].pNode->glob_mtx * vPos;
			}
			else
				aLights[i].vCurPos = *(aLights[i].pObject->GetMatrix()) * aLights[i].vPos;

			aLights[i].fCurDistance = sqrtf(~(pObject->GetPos() - aLights[i].vCurPos));
		}
		aLights[i].Light.Position.x = aLights[i].vCurPos.x;
		aLights[i].Light.Position.y = aLights[i].vCurPos.y;
		aLights[i].Light.Position.z = aLights[i].vCurPos.z;

		if (pObject == aLights[i].pObject || aLights[i].bCoronaOnly) continue;

		SelectedLight * pSL = &aSelectedLights[aSelectedLights.Add()];
	
		pSL->fDistance = aLights[i].fCurDistance;
		pSL->dwIndex = i;
	}

	aSelectedLights.Sort();

	for (i=0; i<aLights.Size(); i++) if (!aLights[i].bOff && aLights[i].pObject == pObject && !aLights[i].bCoronaOnly) 
	{
		aSelectedLights.Insert(SelectedLight(), 0);
		aSelectedLights[0].dwIndex = i;
		aSelectedLights[0].fDistance = aLights[i].fCurDistance;
	}

	for (i=0; i<dwMaxD3DLights; i++) 
	{
		if (i >= aSelectedLights.Size()) break;

		ShipLight * pL = &aLights[aSelectedLights[i].dwIndex];

		pRS->SetLight(i + 1, &pL->Light);
		pRS->LightEnable(i + 1, true);

		iMinLight = Min(iMinLight, long(i + 1));
		iMaxLight = Max(iMaxLight, long(i + 1));
	}
}

// update lights/flares parameters
void ShipLights::Execute(dword dwDeltaTime)
{
	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	VIDWALKER * vwSunTrace = api->LayerGetWalker("sun_trace");
	VIDWALKER * vwSailsTrace = api->LayerGetWalker("sails_trace");

	float fFov;
	CVECTOR vCamPos, vCamAng;
	pRS->GetCamera(vCamPos, vCamAng, fFov);

	for (dword i=0; i<aLights.Size(); i++)
	{
		ShipLight & L = aLights[i];

		//if (i==19) _asm int 3
		if (L.bOff) continue;

		if (!L.bDynamicLight)
		{
			if (L.pNode)
				L.vCurPos = L.pNode->glob_mtx * L.vPos;
			else
				L.vCurPos = *(L.pObject->GetMatrix()) * L.vPos;
		}

		if (!L.bOff && L.bDead && L.fTotalBrokenTime <= 0.0f && pSea && pSea->WaveXZ(L.vCurPos.x, L.vCurPos.x) > L.vCurPos.y)
		{
			L.fTotalBrokenTime = RRnd(1.0f, 4.0f);
			L.fBrokenTime = 0.0f;
			L.bBrokenTimeOff = true;
		}

		float fBroken = 1.0f;
		if (L.fTotalBrokenTime > 0.0f)
		{
			L.fBrokenTime += fDeltaTime;
			if (L.fBrokenTime >= L.fTotalBrokenTime)
			{
				L.bOff = L.bBrokenTimeOff;
				L.fTotalBrokenTime = 0.0f;
				//aLights.ExtractNoShift(i);
				//i--;
				continue;
			}

			fBroken = 1.0f - L.fBrokenTime / L.fTotalBrokenTime;
		}

		if (pCollide)
		{
			L.bVisible = true;

			if (vwSailsTrace)
			{
				float fDistance = pCollide->Trace(*vwSailsTrace, L.vCurPos, vCamPos, null, 0);
				L.fFlareAlphaMax = (fDistance >= 1.0f) ? 1.0f : 0.2f;
			}

			if (vwSunTrace)
			{
				float fDistance = pCollide->Trace(*vwSunTrace, L.vCurPos, vCamPos, null, 0);
				float fLen = fDistance * sqrtf(~(vCamPos - L.vCurPos));
				L.bVisible = fDistance >= 1.0f || (fLen < 0.6f);

				if (!L.bOff && L.bVisible)
				{
					float fDistance = pCollide->Trace(*vwSunTrace, vCamPos, L.vCurPos, null, 0);
					float fLen = (1.0f - fDistance) * sqrtf(~(vCamPos - L.vCurPos));

					L.bVisible = fLen < 0.6f;
				}
			}
		}

		L.fFlareAlpha = Min(Clamp(L.fFlareAlpha + 5.0f * fDeltaTime * ((L.bVisible) ? 1.0f : -1.0f)), L.fFlareAlphaMax);

		if (!L.bDynamicLight)
		{
			float fIntensity = 0.0f;
			float fKAmp = 1.0f;
			for (dword j=0; j<2; j++)
			{
				Oscillator & o = L.Osc[j];
				o.fK += o.fStep * fDeltaTime;
				if(o.fK >= 1.0f)
				{
					if (o.fK < 2.0f)
					{
						o.fK -= 1.0f;
						o.fOldValue = o.fNewValue;
						o.fNewValue = RRnd(-o.fAmp, o.fAmp);
					}else{
						o.fK = 0.0f;
						o.fOldValue = RRnd(-o.fAmp, o.fAmp);
						o.fNewValue = RRnd(-o.fAmp, o.fAmp);
					}
				}
				float fIns = o.fOldValue + (o.fNewValue - o.fOldValue) * o.fK;
				fIntensity += fIns * fKAmp;
				fKAmp -= fIns * o.fOneDivAmp;
				if(fKAmp < 0.0f) break;
			}

			fIntensity = 1.0f + Clampf(fIntensity, -1.0f, 1.0f);
			L.Light.Diffuse.r = L.pLT->cLightColor.r * fIntensity * fBroken;
			L.Light.Diffuse.g = L.pLT->cLightColor.g * fIntensity * fBroken;
			L.Light.Diffuse.b = L.pLT->cLightColor.b * fIntensity * fBroken;

			L.fCoronaIntensity = fBroken * (0.5f + fIntensity * 0.5f);
		}
		else
		{
			L.fCurTime += fDeltaTime;
			if (L.fCurTime >= L.fTotalTime)
			{
				aLights.ExtractNoShift(i);
				i--;
				continue;
			}

			float fIntensity = (L.fCurTime < L.fUpTime) ? L.fCurTime / L.fUpTime : (1.0f - (L.fCurTime - L.fUpTime) / (L.fTotalTime - L.fUpTime));
			fIntensity = 0.5f + Clampf(fIntensity, -0.5f, 0.5f);
			L.Light.Diffuse.r = L.pLT->cLightColor.r * fIntensity;
			L.Light.Diffuse.g = L.pLT->cLightColor.g * fIntensity;
			L.Light.Diffuse.b = L.pLT->cLightColor.b * fIntensity;
		}
	}

	DELETE(vwSunTrace);
	DELETE(vwSailsTrace);
}

// draw flares
void ShipLights::Realize(dword dwDeltaTime)
{
	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	float fFov;
	CVECTOR vCamPos, vCamAng; 
	pRS->GetCamera(vCamPos, vCamAng, fFov);

	static array<RS_RECT> aRects(_FL_, 32);
	aRects.Empty();

	float fReflSize = (bReflection) ? fSunRoadFlareSize : 1.0f;

	for (dword i=0; i<aLights.Size(); i++) 
	{
		ShipLight & L = aLights[i];
		
		if (L.bOff) continue;

		float fDistanceFade = 1.0f;

		if (bReflection)
		{
			float fDistance = sqrtf(~(L.vCurPos - vCamPos));
			fDistanceFade = 1.0f - Clamp(fDistance / L.pLT->fSunRoadFlareFadeDistance);
		}

		if (L.bDynamicLight && bReflection) 
		{
			RS_RECT & r = aRects[aRects.Add()];
			r.vPos = L.vCurPos;
			r.fAngle = 0.0f;
			r.dwSubTexture = 0;
			r.fSize = 10.0f;
			r.dwColor = ARGB(255, 255, 255, 255);
			continue;
		}

		RS_RECT & r = aRects[aRects.Add()];
		r.vPos = L.vCurPos;
		r.fSize = L.pLT->fCoronaSize * L.fCoronaIntensity * fReflSize;
		r.fAngle = 0.0f;
		r.dwSubTexture = 0;
		Color cColor = L.pLT->cCoronaColor * (L.fCoronaIntensity * fDistanceFade);
		cColor.Normalize();
		cColor = cColor * 255.0f * L.fFlareAlpha;
		cColor.a = Clamp(L.fFlareAlpha * fReflSize);
		r.dwColor = ARGB(cColor.a, cColor.r, cColor.g, cColor.b);
	}

	if (aRects.Size())
	{
		pRS->TextureSet(0, (bReflection) ? iFlareSunRoadTex : iCoronaTex);
		pRS->DrawRects(&aRects[0], aRects.Size(), sCoronaTechnique, dwCoronaSubTexX, dwCoronaSubTexY);
	}
}

void ShipLights::UnSetLights(VAI_OBJBASE * pObject)
{
	for (long i=iMinLight; i<=iMaxLight; i++)
		pRS->LightEnable(i, false);

	iMinLight = 1000;
	iMaxLight = -1;
}

dword _cdecl ShipLights::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
		case MSG_SEA_SUNROAD_DRAW:
			bReflection = true;
			Realize(0);
			bReflection = false;
		break;
	}

	return 0;
}

void ShipLights::SetDead(VAI_OBJBASE * pObject)
{
	for (dword i=0; i<aLights.Size(); i++) if (aLights[i].pObject == pObject)
		aLights[i].bDead = true;
}