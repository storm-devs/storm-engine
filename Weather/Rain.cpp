#include "Rain.h"

RAIN::RAIN() :
	aRects(_FL_, 512),
	aDrops(_FL_, 512),
	aSeaDrops(_FL_, 512),
	aShips(_FL_, 16)
{
	pWeather = null;
	pRainBlocks = null;
	dwNumDrops = 0;
	iVertexBuffer = -1;
	iRainbowTex = -1;
	iRainDropsTexture = -1;
	bRainbowEnable = false;
	bShow = true;

	fDropsDeltaTime = 0.0f;

	dwDropsNearNum = 0;
	dwDropsFarNum = 0;

	fDropsNearRadius = 25.0f;
	fDropsFarRadius = 75.0f;

	fDropsLifeTime = 0.5f;
	fDropsSize = 0.1f;
	dwDropsColor = 0x3FFFFFFF;

	iIBSeaDrops = -1;
	iVBSeaDrops = -1;
	iSeaDropTex = -1;

	sDropsTexture = "weather\\rain_drops.tga";
	sSeaDropsTexture = "weather\\sea_rain_drops.tga";
}

RAIN::~RAIN()
{
	Release();
}

void RAIN::Release()
{
	DELETE(pRainBlocks);
	if (iVertexBuffer >= 0) Render().ReleaseVertexBuffer(iVertexBuffer); 
	if (iRainbowTex >= 0) Render().TextureRelease(iRainbowTex);
	if (iRainDropsTexture >= 0) Render().TextureRelease(iRainDropsTexture);
	if (iSeaDropTex >= 0) Render().TextureRelease(iSeaDropTex);
	iVertexBuffer = -1;
	iRainbowTex = -1;
	iSeaDropTex = -1;

	if (iIBSeaDrops >= 0) Render().ReleaseIndexBuffer(iIBSeaDrops); 
	if (iVBSeaDrops >= 0) Render().ReleaseVertexBuffer(iVBSeaDrops ); 

	iIBSeaDrops = -1;
	iVBSeaDrops = -1;

}

void RAIN::GenerateRandomDrop(CVECTOR *vPos)
{
	float fDist = 1.5f + FRAND(fRainRadius);
	float fAngle = FRAND(PIm2);
	vPos->x = fDist * cosf(fAngle);
	vPos->z = fDist * sinf(fAngle);
	vPos->y = fRainHeight - FRAND(fRainHeight*2.0f);
}

void RAIN::SetDevice()
{
}

void RAIN::GenerateRain()
{
	dword	i;

	ENTITY_ID	ent;
	if (!api->FindClass(&ent,"Weather",0)) _THROW("No found WEATHER entity!");
	pWeather = (WEATHER_BASE*)api->GetEntityPointer(&ent); Assert(pWeather);

	Release();

	/*DELETE(pRainBlocks);
	if (iVertexBuffer>=0) RS->ReleaseVertexBuffer(iVertexBuffer);
	iVertexBuffer = -1;*/

	//fRainHeight = Weather->GetFloat(whf_rain_height);
	//fRainRadius = Weather->GetFloat(whf_rain_radius);
	//float fRainRadiusD2 = fRainRadius / 2.0f;
	//dwNumDrops = Weather->GetLong(whi_rain_drops_num);

	if (dwNumDrops==0) return;

	dwNumRainBlocks = 4;

	dwRainR = (dwRainColor>>0x10)&0xFF;
	dwRainG = (dwRainColor>>0x8)&0xFF;
	dwRainB = (dwRainColor)&0xFF;

	fWindPower = pWeather->GetFloat(whf_wind_speed) / 18.0f;
	fWindAngle = pWeather->GetFloat(whf_wind_angle);
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

		float jitter = fRainWindSpeedJitter; //Weather->GetFloat(whf_rain_wind_speed_jitter);
		pRainBlocks[i].fWindSpeedJitter = FRAND(jitter) - jitter / 2.0f;
	}

	iVertexBuffer = Render().CreateVertexBuffer(D3DRAINVERTEX_FORMAT,dwNumDrops*2*sizeof(RAINVERTEX),D3DUSAGE_WRITEONLY);
	if (iVertexBuffer<0) return;

	RAINVERTEX * pVertBuf = (RAINVERTEX*)Render().LockVertexBuffer(iVertexBuffer);
	if (!pVertBuf) return;

	for (i=0;i<dwNumDrops;i++) 
	{
		GenerateRandomDrop(&pVertBuf[i*2+0].vPos);
		pVertBuf[i*2+0].dwColor = 0xFFFFFFFF;
 
		pVertBuf[i*2+1].vPos = pVertBuf[i*2+0].vPos;
		pVertBuf[i*2+1].vPos.y += fDropLength;
		pVertBuf[i*2+1].dwColor = 0xFFFFFF;
	}
	Render().UnLockVertexBuffer(iVertexBuffer);

	iIBSeaDrops = Render().CreateIndexBuffer(NUM_SEA_DROPS * 2 * 3 * sizeof(word), D3DUSAGE_WRITEONLY); 
	iVBSeaDrops = Render().CreateVertexBuffer(D3DSEADROPVERTEX_FORMAT, NUM_SEA_DROPS * 4 * sizeof(SEADROPVERTEX), D3DUSAGE_WRITEONLY); 

	word * pI = (word *)Render().LockIndexBuffer(iIBSeaDrops);
	if (pI) 
	{
		for (long i=0; i<NUM_SEA_DROPS; i++)
		{
			*pI++ = word(i * 4 + 0);
			*pI++ = word(i * 4 + 3);
			*pI++ = word(i * 4 + 2);

			*pI++ = word(i * 4 + 0);
			*pI++ = word(i * 4 + 2);
			*pI++ = word(i * 4 + 1);
		}
		Render().UnLockIndexBuffer(iIBSeaDrops);
	}

	// rainbow
	if (bRainbowEnable)
	{
		iRainbowTex = Render().TextureCreate(sRainbowTexture);
	}

	iRainDropsTexture = Render().TextureCreate(sDropsTexture);

	iSeaDropTex = Render().TextureCreate(sSeaDropsTexture);

	fDropsDeltaTime = 0.0f;
}

bool RAIN::Init()
{
	SetDevice();

	return true;
}

bool RAIN::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool RAIN::LoadState(ENTITY_STATE * state)
{
	return true;
}

void RAIN::Execute(dword Delta_Time)
{
}

void RAIN::InitialSomeBlockParameters(long iIdx)
{
	float fDist = 6.0f * 5.4f * fWindPower;
	pRainBlocks[iIdx].vPos.x = -fDist * sinf(fWindAngle);
	pRainBlocks[iIdx].vPos.z = -fDist * cosf(fWindAngle);
	pRainBlocks[iIdx].fWindFlaw	= 0.0f;
}

void RAIN::RealizeDrops(dword Delta_Time)
{
	float fDeltaTime = float(Delta_Time) * 0.001f;

	dword dwShipName = api->Class_Name2Code("SHIP");

	CMatrix mView;
	Render().GetTransform(D3DTS_VIEW, (D3DXMATRIX*)&mView);
	mView.Transposition();

	float fFov;
	Render().GetCamera(vCamPos, vCamAng, fFov);

	vCamPos = mView.Pos();

	ENTITY_ID sea_id;
	SEA_BASE * pSea = null;
	if (api->FindClass(&sea_id, "sea", 0)) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);

	VIDWALKER * pVW = api->LayerGetWalker("rain_drops");

	fDropsDeltaTime += fDeltaTime;

	long iNumNewDrops1 = long(fDropsDeltaTime * float(dwDropsNearNum));
	long iNumNewDrops2 = long(fDropsDeltaTime * float(dwDropsFarNum));
	fDropsDeltaTime -= float(double(iNumNewDrops1 + iNumNewDrops2) / double(dwDropsNearNum + dwDropsFarNum));
	if (fDropsDeltaTime < 0.0f) fDropsDeltaTime = 0.0f;

	if (pVW) for (long i=0; i<iNumNewDrops1 + iNumNewDrops2; i++)
	{
		SHIP_BASE * pShip = null;
		float fA, fS, fR;
		CVECTOR vSrc, vDst;

		fS = 0.0f;
		fR = fDropsNearRadius;
		if (i >= iNumNewDrops1)
		{
			fS = fDropsNearRadius;
			fR = fDropsFarRadius;
		}

		fA = FRAND(PIm2);
		fR = fS + FRAND(fR);
		
		vSrc = CVECTOR(vCamPos.x + fR * sinf(fA), vCamPos.y + 75.0f, vCamPos.z + fR * cosf(fA));
		vDst = CVECTOR(vSrc.x, vCamPos.y - 75.0f, vSrc.z);

		float fTest1 = Collide().Trace(*pVW, vSrc, vDst, null, 0);
		float fTest2 = 2.0f;

		if (pSea) 
		{
			float y = pSea->WaveXZ(vSrc.x, vSrc.z);
			if (y >= vDst.y && y <= vSrc.y)
				fTest2 = (vSrc.y - y) / (vSrc.y - vDst.y);
		}
		float fTest = fTest2;
		if (fTest1 <= 1.0f && fTest1 < fTest2) 
		{
			fTest = fTest1;

			//проверим - если это корабль
			ENTITY_ID eid = Collide().GetObjectID();
			if (eid.class_code == dwShipName)
			{
				pShip = (SHIP_BASE*)eid.pointer;
			}
		}
		else if (fTest2 <= 1.0f)
		{
			seadrop_t & sea_drop = aSeaDrops[aSeaDrops.Add()];
			
			sea_drop.vPos = vSrc + fTest * (vDst - vSrc);
			sea_drop.fTime = 1.0f;
			sea_drop.fLifeTime = 1.0f;
		}

		if (fTest <= 1.0f)
		{
			// Добавляем каплю
			drop_t & drop = aDrops[aDrops.Add()];
			drop.vPos = vSrc + fTest * (vDst - vSrc);
			drop.fLifeTime = fDropsLifeTime + fDeltaTime;
			drop.iShip = -1;
			if (pShip)
			{
				long k;
				for (k=0; k<aShips; k++) if (aShips[k].pShip == pShip) break;
				if (k == aShips.Len())
				{
					ship_t & ship = aShips[aShips.Add()];
					ship.eid = pShip->GetID();
					ship.pShip = pShip;
				}
				CMatrix mShip = *pShip->GetMatrix();
				mShip.Transposition();
				drop.iShip = k;
				drop.vPos = mShip * drop.vPos;
			}
		}
	}

	aRects.Empty();

	for (long i=0; i<aShips; i++)
	{
		if (!api->ValidateEntity(&aShips[i].eid))
		{
			aShips[i].pShip = null;
		}
	}

	for (long i=0; i<aDrops.Len(); i++)
	{
		drop_t & drop = aDrops[i];

		drop.fLifeTime -= fDeltaTime;
		if (drop.fLifeTime <= 0.0f || (drop.iShip >= 0 && !aShips[drop.iShip].pShip))
		{
			aDrops.ExtractNoShift(i);
			i--;
			continue;
		}

		RS_RECT & r = aRects[aRects.Add()];

		if (drop.iShip >= 0)
			r.vPos = (*aShips[drop.iShip].pShip->GetMatrix()) * drop.vPos;
		else
			r.vPos = drop.vPos;

		float fDistance = sqrtf(SQR(r.vPos.x - vCamPos.x) + SQR(r.vPos.z - vCamPos.z));
		float fK = Bring2Range(0.4f, 2.0f, 0.0f, 30.0f, fDistance);

		r.vPos.y += fK * fDropsSize;
		r.fAngle = 0.0f;
		r.fSize = fK * fDropsSize;
		r.dwColor = dwDropsColor;
		r.dwSubTexture = dword(8.0f * drop.fLifeTime / fDropsLifeTime);
	}

	Render().TextureSet(0, iRainDropsTexture);
	//Render().SetTexture(0, null);
	Render().DrawRects(aRects.GetBuffer(), aRects.Size(), "rain_drops", 8, 1);

	// рисуем круги на воде
	CMatrix IMatrix;
	IMatrix.SetIdentity();
	Render().SetWorld(IMatrix);
	Render().TextureSet(0, iSeaDropTex);

	SEADROPVERTEX * pVSeaDropBuffer = (SEADROPVERTEX *)Render().LockVertexBuffer(iVBSeaDrops, D3DLOCK_DISCARD);
	long n = 0;
	if (pVSeaDropBuffer) for (long i=0; i<aSeaDrops.Len(); i++)
	{
		seadrop_t & drop = aSeaDrops[i];

		drop.fTime -= fDeltaTime;
		if (drop.fTime <= 0.0f)
		{
			aSeaDrops.ExtractNoShift(i);
			i--;
			continue;
		}

		CVECTOR v = drop.vPos;
		v.y = pSea->WaveXZ(v.x, v.z) + 0.015f;

		SEADROPVERTEX * pV = &pVSeaDropBuffer[n * 4];

		float fSize = 0.15f;
		long frame = long((1.0f - drop.fTime / drop.fLifeTime) * 8.0f);
		float du = 1.0f / 8.0f;
		float u = float(frame) * du; 

		pV[0].vPos = v + CVECTOR(-fSize, 0.0f, -fSize);
		pV[0].dwColor = 0xFFFFFFFF;
		pV[0].tu = u; 
		pV[0].tv = 0.0f;

		pV[1].vPos = v + CVECTOR(fSize, 0.0f, -fSize);
		pV[1].dwColor = 0xFFFFFFFF;
		pV[1].tu = u + du; 
		pV[1].tv = 0.0f;

		pV[2].vPos = v + CVECTOR(fSize, 0.0f, fSize);
		pV[2].dwColor = 0xFFFFFFFF;
		pV[2].tu = u + du; 
		pV[2].tv = 1.0f;

		pV[3].vPos = v + CVECTOR(-fSize, 0.0f, fSize);
		pV[3].dwColor = 0xFFFFFFFF;
		pV[3].tu = u; 
		pV[3].tv = 1.0f;

		n++;

		if (n >= NUM_SEA_DROPS)
		{
			Render().UnLockVertexBuffer(iVBSeaDrops);
			Render().DrawBuffer(iVBSeaDrops, sizeof(SEADROPVERTEX), iIBSeaDrops, 0, n * 4, 0, n * 2, "sea_rain_drops");

			n = 0;
			pVSeaDropBuffer = (SEADROPVERTEX *)Render().LockVertexBuffer(iVBSeaDrops, D3DLOCK_DISCARD);
		}
	}

	if (n > 0)
	{
		Render().UnLockVertexBuffer(iVBSeaDrops);
		Render().DrawBuffer(iVBSeaDrops, sizeof(SEADROPVERTEX), iIBSeaDrops, 0, n * 4, 0, n * 2, "sea_rain_drops");
	}

	delete pVW;
}

void RAIN::Realize(dword Delta_Time)
{
	if( !bShow ) return;

	dword	i;
	float	fFov;

	Render().GetCamera(vCamPos,vCamAng,fFov);

	if (iVertexBuffer>=0)
	{
		bool bDraw = Render().TechniqueExecuteStart("rain");

		if (bDraw) for (i=0;i<dwNumRainBlocks;i++)
		{
			dword dwAlpha = dword(255.0f * float(pRainBlocks[i].dwTime) / float(dwRainTimeBlend));
			if (dwAlpha>dwRainMaxBlend) dwAlpha = dwRainMaxBlend;
			dword dwColor = ARGB(dwAlpha,dwRainR,dwRainG,dwRainB);
			Render().SetRenderState(D3DRS_TEXTUREFACTOR,dwColor);

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

			Render().SetTransform(D3DTS_WORLD, mWorld);

			Render().DrawPrimitive(D3DPT_LINELIST, iVertexBuffer, sizeof(RAINVERTEX), 0, dwNumDrops);
		}
		if (bDraw) while (Render().TechniqueExecuteNext()) {};
	
		RealizeDrops(Delta_Time);
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

		Render().TextureSet(0,iRainbowTex);
		Render().DrawRects(&rs_rect,1,"rainbow");
	}
}

dword _cdecl RAIN::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case 1:
		bShow = (0!=message.Long());
		break;
	}
	return 0;
}

dword RAIN::AttributeChanged(ATTRIBUTES * pAttribute)
{
	if (*pAttribute == "Clear")
	{
		Release();
		bRainbowEnable = false;
		dwNumDrops = 0;
		fDropsDeltaTime = 0.0f;
		return 0;
	}

	if (*pAttribute == "isDone")
	{ 
		GenerateRain(); return 0; 
	}

	ATTRIBUTES * pParent = pAttribute->GetParent();
	if (*pParent == "RainDrops")
	{
		if (*pAttribute == "DropsNearNum")		{ dwDropsNearNum = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "DropsFarNum")		{ dwDropsFarNum = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "DropsNearRadius")	{ fDropsNearRadius = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "DropsFarRadius")	{ fDropsFarRadius = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "DropsLifeTime")		{ fDropsLifeTime = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "DropsSize")			{ fDropsSize = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "DropsColor")		{ dwDropsColor = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "DropsTexture")		{ sDropsTexture = pAttribute->GetThisAttr(); return 0; }
	}
	else if (*pParent == "Rainbow")
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
