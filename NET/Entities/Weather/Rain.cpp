#include "Rain.h"

NetRain::NetRain()
{
	pRainBlocks = null;
	dwNumDrops = 0;
	iVertexBuffer = -1;
	iRainbowTex = -1;
	bRainbowEnable = false;
}

NetRain::~NetRain()
{
	Release();
}

void NetRain::Release()
{
	DELETE(pRainBlocks);
	if (iVertexBuffer>=0) pRS->ReleaseVertexBuffer(iVertexBuffer); 
	if (iRainbowTex>=0) pRS->TextureRelease(iRainbowTex);
	iVertexBuffer = -1;
	iRainbowTex = -1;
}

void NetRain::GenerateRandomDrop(CVECTOR *vPos)
{
	float fDist = 1.5f + FRAND(fRainRadius);
	float fAngle = FRAND(PIm2);
	vPos->x = fDist * cosf(fAngle);
	vPos->z = fDist * sinf(fAngle);
	vPos->y = fRainHeight - FRAND(fRainHeight*2.0f);
}

void NetRain::SetDevice()
{
	pRS = (VDX8RENDER *)api->CreateService("dx8render"); Assert(pRS);

	VDATA * pSVWeather = (VDATA *)api->GetScriptVariable((IsServer()) ? "NSWeather" : "NCWeather"); Assert(pSVWeather);
	pAWeather = pSVWeather->GetAClass(); Assert(pAWeather);
}

void NetRain::GenerateRain()
{
	dword	i;

	Release();

	if (dwNumDrops==0) return;

	dwNumRainBlocks = 4;

	dwRainR = (dwRainColor>>0x10)&0xFF;
	dwRainG = (dwRainColor>>0x8)&0xFF;
	dwRainB = (dwRainColor)&0xFF;

	fWindPower = pAWeather->GetAttributeAsFloat("WindSpeed") / 18.0f;
	fWindAngle = pAWeather->GetAttributeAsFloat("WindAngle");

	fRainWindSpeed = fWindPower * 0.2f;

	pRainBlocks = NEW rainblock_t[dwNumRainBlocks];

	pRainBlocks[0].vPos = CVECTOR(0.0f, 0.0f, 0.0f);
	pRainBlocks[1].vPos = CVECTOR(0.0f, fRainHeight, 0.0f);
	pRainBlocks[2].vPos = CVECTOR(0.0f, fRainHeight / 2.0f, 0.0f);
	pRainBlocks[3].vPos = CVECTOR(0.0f, fRainHeight + fRainHeight / 2.0f, 0.0f);

	for (i=0;i<dwNumRainBlocks;i++)
	{
		InitialSomeBlockParameters(i);

		pRainBlocks[i].dwTime = dwRainTimeBlend;
		pRainBlocks[i].vAng.y = FRAND(PIm2);

		float jitter = fRainWindSpeedJitter; 
		pRainBlocks[i].fWindSpeedJitter = FRAND(jitter) - jitter / 2.0f;
	}

	iVertexBuffer = pRS->CreateVertexBuffer(D3DRAINVERTEX_FORMAT,dwNumDrops*2*sizeof(RAINVERTEX),D3DUSAGE_WRITEONLY);
	if (iVertexBuffer<0) return;

	RAINVERTEX * pVertBuf = (RAINVERTEX*)pRS->LockVertexBuffer(iVertexBuffer);
	if (!pVertBuf) return;

	for (i=0;i<dwNumDrops;i++) 
	{
		GenerateRandomDrop(&pVertBuf[i*2+0].vPos);
		pVertBuf[i*2+0].dwColor = 0xFFFFFFFF;
 
		pVertBuf[i*2+1].vPos = pVertBuf[i*2+0].vPos;
		pVertBuf[i*2+1].vPos.y += fDropLength;
		pVertBuf[i*2+1].dwColor = 0xFFFFFF;
	}
	pRS->UnLockVertexBuffer(iVertexBuffer);

	// rainbow
	if (bRainbowEnable)
	{
		iRainbowTex = pRS->TextureCreate(sRainbowTexture);
	}

}

bool NetRain::Init()
{
	pRS = 0;

	SetDevice();

	return true;
}

bool NetRain::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool NetRain::LoadState(ENTITY_STATE * state)
{
	return true;
}

void NetRain::Execute(dword Delta_Time)
{
}

void NetRain::InitialSomeBlockParameters(long iIdx)
{
	float fDist = 6.0f * 5.4f * fWindPower;
	pRainBlocks[iIdx].vPos.x = -fDist * sinf(fWindAngle);
	pRainBlocks[iIdx].vPos.z = -fDist * cosf(fWindAngle);
	pRainBlocks[iIdx].fWindFlaw	= 0.0f;
}

void NetRain::Realize(dword Delta_Time)
{
	dword	i;
	float	fFov;

	pRS->GetCamera(vCamPos,vCamAng,fFov);

	if (iVertexBuffer>=0)
	{
		bool bDraw = pRS->TechniqueExecuteStart("rain");

		if (bDraw) for (i=0;i<dwNumRainBlocks;i++)
		{
			dword dwAlpha = dword(255.0f * float(pRainBlocks[i].dwTime) / float(dwRainTimeBlend));
			if (dwAlpha>dwRainMaxBlend) dwAlpha = dwRainMaxBlend;
			dword dwColor = ARGB(dwAlpha,dwRainR,dwRainG,dwRainB);
			pRS->SetRenderState(D3DRS_TEXTUREFACTOR,dwColor);

			CMatrix mY1,mX,mY2,mWorld;

			mWorld.BuildPosition(vCamPos.x + pRainBlocks[i].vPos.x,pRainBlocks[i].vPos.y,vCamPos.z + pRainBlocks[i].vPos.z);

			mY1.BuildRotateY(pRainBlocks[i].vAng.y);
			mX.BuildRotateX(fWindPower + pRainBlocks[i].fWindSpeedJitter);
			mY2.BuildRotateY(PI+fWindAngle);

			mWorld = mY1 * mX * mY2 * mWorld;

			pRainBlocks[i].vPos.y -= fRainSpeed*0.2f * float(Delta_Time) / 60.0f;
			pRainBlocks[i].fWindFlaw += FRAND(fRainJitter) - fRainJitter/2.0f;
			float f = 0.0f; //pRainBlocks[i].fWindFlaw * float(Delta_Time) / 50.0f;
			pRainBlocks[i].vPos.x += (fRainSpeed*fRainWindSpeed+f) * float(Delta_Time) / 60.0f * sinf(fWindAngle);
			pRainBlocks[i].vPos.z += (fRainSpeed*fRainWindSpeed+f) * float(Delta_Time) / 60.0f * cosf(fWindAngle);
			pRainBlocks[i].dwTime += Delta_Time;
			if (pRainBlocks[i].vPos.y<vCamPos.y-fRainHeight) 
			{
				InitialSomeBlockParameters(i);
				pRainBlocks[i].vPos.y += fRainHeight*2.0f - 1.0f;
				pRainBlocks[i].vAng.y = FRAND(PIm2);
				pRainBlocks[i].dwTime = 0;
			}

			pRS->SetTransform(D3DTS_WORLD, mWorld);

			pRS->DrawPrimitive(D3DPT_LINELIST, iVertexBuffer, sizeof(RAINVERTEX), 0, dwNumDrops);
		}
		if (bDraw) while (pRS->TechniqueExecuteNext()) {};
	}
	// draw rainbow
	if (bRainbowEnable)
	{
		RS_RECT		rs_rect;
		CVECTOR vPos = vCamPos + CVECTOR(0.32f,0.0f,0.72f) * 1900.0f;
		vPos.y = 350.0f;
		rs_rect.dwColor = 0x0F0F0F;
		rs_rect.vPos = vPos;
		rs_rect.fSize = 1600.0f;
		rs_rect.fAngle = 0.0f;

		pRS->TextureSet(0,iRainbowTex);
		pRS->DrawRects(&rs_rect,1,"rainbow");
	}
}

void NetRain::ProcessMessage(dword iMsg,dword wParam,dword lParam)
{
}

dword NetRain::AttributeChanged(ATTRIBUTES * pAttribute)
{
	if (*pAttribute == "Clear")
	{
		Release();
		bRainbowEnable = false;
		dwNumDrops = 0;
		return 0;
	}

	if (*pAttribute == "isDone")
	{ 
		GenerateRain(); return 0; 
	}

	ATTRIBUTES * pParent = pAttribute->GetParent();
	if (*pParent == "Rainbow")
	{
		if (*pAttribute == "Enable")			{ bRainbowEnable = pAttribute->GetAttributeAsDword() == 1; return 0; }
		if (*pAttribute == "Texture")			{ sRainbowTexture = pAttribute->GetThisAttr(); return 0; }
	}
	else
	{
		if (*pAttribute == "color")				{ dwRainColor = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "NumDrops")			{ dwNumDrops = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "DropLength")		{ fDropLength = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Height")			{ fRainHeight = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Radius")			{ fRainRadius = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Speed")				{ fRainSpeed = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Jitter")			{ fRainJitter = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "WindSpeedJitter")	{ fRainWindSpeedJitter = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "TimeBlend")			{ dwRainTimeBlend = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "MaxBlend")			{ dwRainMaxBlend = pAttribute->GetAttributeAsDword(); return 0; }
	}

	return 0;
}
