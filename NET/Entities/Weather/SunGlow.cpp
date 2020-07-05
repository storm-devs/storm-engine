#include "sunglow.h"

NetSunGlow::NetSunGlow() : aRSR(_FL_, 16)
{
	fAlpha = 0.0f;
	fAlphaFlare = 0.0f;
	fAlphaOverflow = 0.0f;
	fMinAlphaValue = 0.0f;
	fMaxOverflowAlphaValue = 0.0f;
	Flares.fFlareScale = 1.0f;
	bVisible = false;
	
	iGlowTex = -1;
	iFlareTex = -1;
	iOverflowTex = -1;
	iReflTexture = -1;

	pVWSunTrace = null;
	pVWSailsTrace = null;

	bSimpleSea = false;
	bHaveFlare = false;
	bHaveGlow = false;
	bHaveOverflow = false;
	bHaveReflection = false;
}

NetSunGlow::~NetSunGlow()
{
	Release();
}

bool NetSunGlow::Init()
{
	pRS = 0;

	SetDevice();

	return true;
}

void NetSunGlow::SetDevice()
{
	pRS = (VDX8RENDER *)_CORE_API->CreateService("dx8render"); Assert(pRS);
	pCollide = (COLLIDE*)_CORE_API->CreateService("COLL"); Assert(pCollide);
}

void NetSunGlow::Release()
{
	if (iGlowTex>=0)		pRS->TextureRelease(iGlowTex);		iGlowTex = -1;
	if (iFlareTex>=0)		pRS->TextureRelease(iFlareTex);		iFlareTex = -1;
	if (iOverflowTex>=0)	pRS->TextureRelease(iOverflowTex);	iOverflowTex = -1;
	if (iReflTexture>=0)	pRS->TextureRelease(iReflTexture);	iReflTexture = -1;
	
	DELETE(pVWSunTrace);
	DELETE(pVWSailsTrace);
}

void NetSunGlow::GenerateSunGlow()
{
	Release();

	if (Flares.sTexture.Len())		iFlareTex = pRS->TextureCreate(Flares.sTexture);
	if (Glow.sTexture.Len())		iGlowTex = pRS->TextureCreate(Glow.sTexture);
	if (Overflow.sTexture.Len())	iOverflowTex = pRS->TextureCreate(Overflow.sTexture);
	if (Reflection.sTexture.Len())	iReflTexture = pRS->TextureCreate(Reflection.sTexture);
}

void NetSunGlow::Execute(dword Delta_Time)
{
	if (bHaveGlow)
	{
		fAlpha += ((bVisible) ? 1.0f : -1.0f) * float(Delta_Time) * Glow.fDecayTime * 0.001f;
		fAlpha = CLAMP(fAlpha);
		if (fAlpha < fMinAlphaValue) fAlpha = fMinAlphaValue;
	}

	if (bHaveFlare)
	{
		fAlphaFlare += ((bVisibleFlare) ? 1.0f : -1.0f) * float(Delta_Time) * 0.008f;
		fAlphaFlare = CLAMP(fAlphaFlare);
	}

	if (bHaveOverflow)
	{
		fAlphaOverflow += ((bVisible) ? 1.0f : -1.0f) * float(Delta_Time) * 0.008f;
		fAlphaOverflow = CLAMP(fAlphaOverflow);
		if (fAlphaOverflow < fMaxOverflowAlphaValue * fMinAlphaValue) fAlphaOverflow = fMaxOverflowAlphaValue * fMinAlphaValue;
		if (fAlphaOverflow > fMaxOverflowAlphaValue) fAlphaOverflow = fMaxOverflowAlphaValue;
	}
}

float NetSunGlow::LayerTrace(CVECTOR & vSrc, VIDWALKER * pVW)
{
	CVECTOR vDst;

	if (!pVW) return 2.0f;

	vDst = vSrc + (!vSunPos) * 10000.0f;
	return pCollide->Trace(*pVW, vSrc, vDst, 0, 0);
}

void NetSunGlow::Realize(dword Delta_Time)
{
	vSunPos = 5000.0f;
	ATTRIBUTES * pASunPos = pAWeather->FindAClass(pAWeather, "Sun");
	if (pASunPos)
	{
		vSunPos.x = pASunPos->GetAttributeAsFloat("x", 5000.0f);
		vSunPos.y = pASunPos->GetAttributeAsFloat("y", 5000.0f);
		vSunPos.z = pASunPos->GetAttributeAsFloat("z", 5000.0f);
	}

	CMatrix OldMatrix, IMatrix, View;
	pRS->GetTransform(D3DTS_VIEW, OldMatrix);
	pRS->GetTransform(D3DTS_VIEW, View);

	float	fFov;
	CVECTOR vSrc, vDst;
	CVECTOR vSun, vSunDir;
	CVECTOR vCamPos, vCamAng, vCamDir; 
	
	pRS->GetCamera(vCamPos, vCamAng, fFov);
	vSunDir = !vSunPos;
	vSun = vCamPos + (vSunDir * Glow.fDist);

	bool bTempVisibleFlare = true;
	PLANE *pPlane = pRS->GetPlanes();
	if (pPlane)
	{
		for (long i=0;i<4;i++)
		{
			CVECTOR vpn = CVECTOR(pPlane[i].Nx,pPlane[i].Ny,pPlane[i].Nz);
			if ((vpn|vSunPos) < 0.0f) { bTempVisibleFlare = false; break; };
		}
	}
	if (Delta_Time) bVisibleFlare = bTempVisibleFlare;

	bVisible = true;
	fMinAlphaValue = 0.0f;

	if (!pVWSunTrace)	pVWSunTrace = api->LayerGetWalker("sun_trace");
	if (!pVWSailsTrace) pVWSailsTrace = api->LayerGetWalker("sails_trace");

	float fSunTrace = LayerTrace(vCamPos, pVWSunTrace);
	float fSailTrace = LayerTrace(vCamPos, pVWSailsTrace);

	if (fSunTrace<=1.0f || fSailTrace<=1.0f) bVisible = false;
	if (fSailTrace<=1.0f && fSunTrace>1.0f)  
	{
		bVisible = false;
		fMinAlphaValue = 0.2f;
	}

	RS_RECT rs_rect;

	// calculate angle between camera dir.y and sun dir.y
	vSunDir.y = 0.0f;
	vCamDir = CVECTOR(cosf(vCamAng.y), 0.0f, sinf(vCamAng.y));
	float fAngle = vSunDir | vCamDir;

	if (bHaveGlow)
	{
		CVECTOR vGlowColor = (1.0f - fAlpha) * 0.5f * COLOR2VECTOR(Glow.dwColor);
		pRS->TextureSet(0,iGlowTex);

		rs_rect.dwColor = RGB(vGlowColor.x,vGlowColor.y,vGlowColor.z);
		rs_rect.vPos = vSun;
		rs_rect.fSize = Glow.fSize;
		rs_rect.fAngle = fAngle * Glow.fRotateSpeed;

		pRS->DrawRects(&rs_rect, 1, Glow.sTechniqueZ);

		vGlowColor = fAlpha * COLOR2VECTOR(Glow.dwColor);
		rs_rect.dwColor = RGB(vGlowColor.x,vGlowColor.y,vGlowColor.z);
		rs_rect.vPos = vSun;
		rs_rect.fSize = Glow.fSize;
		rs_rect.fAngle = fAngle * 1.0f;

		pRS->DrawRects(&rs_rect, 1, Glow.sTechniqueNoZ);
	}

	View.Transposition(); 
	CVECTOR vCamera = View.Vz();
	float fDot = vCamera | (!vSun);

	fMaxOverflowAlphaValue = (fDot > Overflow.fStart) ? (fDot - Overflow.fStart) / (1.0f - Overflow.fStart) : 0.0f;
	if (bHaveOverflow)
	{
		pRS->TextureSet(0, iOverflowTex);

		CVECTOR vOverflowColor = fAlphaOverflow * COLOR2VECTOR(Overflow.dwColor);

		rs_rect.dwColor = RGB(vOverflowColor.x, vOverflowColor.y, vOverflowColor.z);
		rs_rect.vPos = vSun;
		rs_rect.fSize = Overflow.fSize;
		rs_rect.fAngle = 0.0f;

		pRS->DrawRects(&rs_rect, 1, Overflow.sTechnique);
	}

	// calculate and draw flares
	aRSR.Empty();
	if (Delta_Time && Flares.aFlares.Size() && bHaveFlare)
	{
		CMatrix mCam = OldMatrix;
		mCam.Transposition();
		
		//CVECTOR vCenPos = CVECTOR(0.0f, 0.0f, Flares.fDist / 2.0f);
		CVECTOR vCenPos = mCam.Vz() * Flares.fDist / 2.0f + mCam.Pos();
		CVECTOR vDelta = Flares.fDist * !(vCenPos - vSun);
		for (dword i=0;i<Flares.aFlares.Size();i++)
		{
			flare_t * pF = &Flares.aFlares[i];
			dword r = dword(fAlpha * fAlphaFlare * float((pF->dwColor&0xFF0000)>>16L));
			dword g = dword(fAlpha * fAlphaFlare * float((pF->dwColor&0xFF00)>>8L));
			dword b = dword(fAlpha * fAlphaFlare * float((pF->dwColor&0xFF)>>0L));
			RS_RECT * pRSR = &aRSR[aRSR.Add()];
			pRSR->dwColor = RGB(r,g,b);
			pRSR->fAngle = 0.0f;
			pRSR->dwSubTexture = pF->dwSubTexIndex;
			pRSR->fSize = pF->fSize * Flares.fFlareScale;
			pRSR->vPos = vSun + vDelta * (1.0f - pF->fDist);
		}

		pRS->TextureSet(0,iFlareTex);
		pRS->DrawRects(&aRSR[0],aRSR.Size(),Flares.sTechnique,Flares.dwTexSizeX,Flares.dwTexSizeY);
	}
}

dword NetSunGlow::AttributeChanged(ATTRIBUTES * pAttribute)
{
	if (*pAttribute == "isDone")
	{ 
		VDATA * pSVWeather = (VDATA *)api->GetScriptVariable((IsServer()) ? "NSWeather" : "NCWeather"); Assert(pSVWeather);
		pAWeather = pSVWeather->GetAClass(); Assert(pAWeather);

		GenerateSunGlow(); 
		return 0; 
	}

	if (*pAttribute == "clear")	
	{ 
		bHaveFlare = false;
		bHaveGlow = false;
		bHaveOverflow = false;
		Flares.aFlares.DelAll();
		return 0; 
	}

	ATTRIBUTES * pParent = pAttribute->GetParent();

	if (*pParent == "Flares")
	{
		bHaveFlare = true;
		char * pTemp = pAttribute->GetThisAttr();
		flare_t * pFlare = &Flares.aFlares[Flares.aFlares.Add()];
		sscanf(pTemp,"%f,%f,%d,%x",&pFlare->fDist,&pFlare->fSize,&pFlare->dwSubTexIndex,&pFlare->dwColor);
		return 0;
	}

	if (*pParent == "Glow")
	{
		bHaveGlow = true;
		if (*pAttribute == "Dist")			{ Glow.fDist = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Size")			{ Glow.fSize = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "RotateSpeed")	{ Glow.fRotateSpeed = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Color")			{ Glow.dwColor = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "Texture")		{ Glow.sTexture = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "TechniqueZ")	{ Glow.sTechniqueZ = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "TechniqueNoZ")	{ Glow.sTechniqueNoZ = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "DecayTime")		{ Glow.fDecayTime = pAttribute->GetAttributeAsFloat(); return 0; }
		return 0;
	}

	if (*pParent == "Flare")
	{
		if (*pAttribute == "Dist")		{ Flares.fDist = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Texture")	{ Flares.sTexture = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "Technique")	{ Flares.sTechnique = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "TexSizeX")	{ Flares.dwTexSizeX = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "TexSizeY")	{ Flares.dwTexSizeY = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "Scale")		{ Flares.fFlareScale = pAttribute->GetAttributeAsFloat(); return 0; }
		return 0;
	}

	if (*pParent == "Overflow")
	{
		bHaveOverflow = true;
		if (*pAttribute == "Texture")	{ Overflow.sTexture = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "Technique")	{ Overflow.sTechnique = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "Size")		{ Overflow.fSize = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Start")		{ Overflow.fStart = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Color")		{ Overflow.dwColor = pAttribute->GetAttributeAsDword(); return 0; }
		return 0;
	}

	if (*pParent == "Reflection")
	{
		bHaveReflection = true;
		if (*pAttribute == "Texture")	{ Reflection.sTexture = pAttribute->GetThisAttr(); return 0; }
		if (*pAttribute == "Size")		{ Reflection.fSize = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Dist")		{ Reflection.fDist = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Color")		{ Reflection.dwColor = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "Technique")	{ Reflection.sTechnique = pAttribute->GetThisAttr(); return 0; }
		return 0;
	}

	return 0;
}

void NetSunGlow::DrawReflection()
{
	if (!bHaveReflection) return;

	vSunPos = 5000.0f;
	float fSunHeightAngle = 1.0f;
	ATTRIBUTES * pASunPos = pAWeather->FindAClass(pAWeather, "Sun");
	if (pASunPos)
	{
		vSunPos.x = pASunPos->GetAttributeAsFloat("x", 5000.0f);
		vSunPos.y = pASunPos->GetAttributeAsFloat("y", 5000.0f);
		vSunPos.z = pASunPos->GetAttributeAsFloat("z", 5000.0f);

		fSunHeightAngle = pASunPos->GetAttributeAsFloat("HeightAngle", 1.0f);
	}

	CMatrix OldMatrix, IMatrix, View;
	pRS->GetTransform(D3DTS_VIEW, OldMatrix);
	pRS->GetTransform(D3DTS_VIEW, View);

	float	fFov;
	CVECTOR vSrc, vDst;
	CVECTOR vSun, vSunDir;
	CVECTOR vCamPos, vCamAng, vCamDir; 

	pRS->GetCamera(vCamPos, vCamAng, fFov);
	vSunDir = !vSunPos;
	vSun = vCamPos + (vSunDir * Reflection.fDist);

	RS_RECT r_spr;
	r_spr.dwColor = Reflection.dwColor;
	r_spr.dwSubTexture = 0;
	r_spr.fAngle = 0.0f;
	r_spr.fSize = Reflection.fSize;
	r_spr.vPos = vSun;

	float fCoeffX = Bring2Range(1.0f, 0.6f, 0.0f, 1.0f, fSunHeightAngle);
	float fCoeffY = Bring2Range(2.0f, 1.0f, 0.0f, 1.0f, fSunHeightAngle);
	
	Render().TextureSet(0, iReflTexture);
	Render().DrawRects(&r_spr, 1, Reflection.sTechnique, 0, 0, (bSimpleSea) ? fCoeffX : 1.0f, (bSimpleSea) ? fCoeffY : 1.0f);
}

dword _cdecl NetSunGlow::ProcessMessage(MESSAGE & message)
{
	long iCode = message.Long();

	switch (iCode)
	{
		case MSG_SEA_SUNROAD_DRAW:
			bSimpleSea = message.Long() != 0;
			DrawReflection();
		break;
	}

	return 0;
}
