#include "Astronomy.h"
#include "..\common_h\Weather_Base.h"

Astronomy::STARS::STARS() : aStars(_FL_, 16384)
{
	bEnable = false;

	dwShader = 0;
	iTexture = -1;
	iVertexBuffer = -1;
	iVertexBufferColors = -1;
	fPrevFov = -1.0f;

	ZERO(Spectr);

	fFadeValue = 1.f;
	fFadeTimeStart = -1.f;
	fFadeTime = 0.2f;
	m_fTwinklingTime = 0.f;
}

Astronomy::STARS::~STARS()
{
	if (iTexture >= 0) Astronomy::pRS->TextureRelease(iTexture);
	if (iVertexBuffer >= 0) Astronomy::pRS->ReleaseVertexBuffer(iVertexBuffer);
	if (iVertexBufferColors >= 0) Astronomy::pRS->ReleaseVertexBuffer(iVertexBufferColors);
	if (dwShader) Astronomy::pRS->DeleteVertexShader(dwShader);
}

void Astronomy::STARS::Init(ATTRIBUTES * pAP)
{
	aStars.DelAll();

	if (iTexture >= 0) Astronomy::pRS->TextureRelease(iTexture);
	if (iVertexBuffer >= 0) Astronomy::pRS->ReleaseVertexBuffer(iVertexBuffer);
	if (iVertexBufferColors >= 0) Astronomy::pRS->ReleaseVertexBuffer(iVertexBufferColors);
	if (dwShader) Astronomy::pRS->DeleteVertexShader(dwShader);

	bEnable = false;

	iTexture = -1;
	iVertexBuffer = -1;
	iVertexBufferColors = -1;

	pAP = pAP->FindAClass(pAP, "Stars"); if (!pAP) return;
	ATTRIBUTES * pASpectrs = pAP->FindAClass(pAP, "Spectr");

	if (pASpectrs)
	{
		for (dword i=0; i<pASpectrs->GetAttributesNum(); i++)
		{
			ATTRIBUTES * pAS = pASpectrs->GetAttributeClass(i);
			char str[2];
			str[0] = pAS->GetThisName()[0]; str[1] = 0; 
			strupr(str); Spectr[str[0]] = pAS->GetAttributeAsDword();
			strlwr(str); Spectr[str[0]] = pAS->GetAttributeAsDword();
		}
	}

	bEnable = pAP->GetAttributeAsDword("Enable", 0) != 0;
	sCatalog = pAP->GetAttribute("Catalog");
	sTexture = pAP->GetAttribute("Texture");
	fRadius = pAP->GetAttributeAsFloat("Radius", 2000.0f);
	fSize = pAP->GetAttributeAsFloat("Size", 20.0f);
	fHeightFade = pAP->GetAttributeAsFloat("HeightFade", 100.0f);
	fSunFade = pAP->GetAttributeAsFloat("SunFade", 1.0f);

	fVisualMagnitude = pAP->GetAttributeAsFloat("VisualMagnitude", 8.5f);
	fTelescopeMagnitude = pAP->GetAttributeAsFloat("TelescopeMagnitude", 13.0f);

	fFadeValue = pAP->GetAttributeAsFloat("FadeValue", 1.f);
	fFadeTimeStart = pAP->GetAttributeAsFloat("FadeStartTime", -1.f);
	fFadeTime = pAP->GetAttributeAsFloat("FadeTime", 0.2f);

	fPrevFov = -1.0f;

	//if (!bEnable) return;

	iTexture = sTexture.IsEmpty() ? -1 : Astronomy::pRS->TextureCreate(sTexture);

	/*char * pBuffer = null;
	dword dwSize = 0;
	if (fio->LoadFile("resource\\hic.txt", (void**)&pBuffer, &dwSize))
	{
	char str[1024], str2[128]; str[0] = 0;
	dword dwPos = 0;
	while (dwPos < dwSize)
	{
	dword dwStr = 0;
	while (dwPos < dwSize && (pBuffer[dwPos] != 0xA && pBuffer[dwPos] != 0xd)) { str[dwStr++] = pBuffer[dwPos++]; str[dwStr + 1] = 0; }
	while (dwPos < dwSize && (pBuffer[dwPos] == 0xA || pBuffer[dwPos] == 0xd)) dwPos++;

	Star & s = aStars[aStars.Add()];

	strncpy(str2, &str[0], 10);		str2[10] = 0; sscanf(str2, "%f", &s.fRA);
	strncpy(str2, &str[16], 10);	str2[10] = 0; sscanf(str2, "%f", &s.fDec);
	strncpy(str2, &str[56], 6);		str2[6] = 0; sscanf(str2, "%f", &s.fMag);
	strncpy(s.cSpectr, &str[63], 2);

	s.dwSubTexture = rand()%4;

	s.fRA = PIm2 * s.fRA / 360.0f;
	s.fDec = PIm2 * s.fDec / 360.0f;
	double RA = 0.0;
	}
	}

	HANDLE hFile = fio->_CreateFile("resource\\hic.dat", GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS);
	if (INVALID_HANDLE_VALUE != hFile)
	{
	dword dwSize = aStars.Size();
	fio->_WriteFile(hFile, &dwSize, sizeof(dwSize), null);
	for (dword i=0; i<aStars.Size(); i++)
	{
	fio->_WriteFile(hFile, &aStars[i].fRA, sizeof(aStars[i].fRA), null);
	fio->_WriteFile(hFile, &aStars[i].fDec, sizeof(aStars[i].fDec), null);
	fio->_WriteFile(hFile, &aStars[i].fMag, sizeof(aStars[i].fMag), null);
	fio->_WriteFile(hFile, &aStars[i].cSpectr[0], sizeof(aStars[i].cSpectr), null);
	}
	fio->_CloseHandle(hFile);
	}*/

	HANDLE hFile = fio->_CreateFile(sCatalog);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		dword dwSize;
		fio->_ReadFile(hFile, &dwSize, sizeof(dwSize), null);

		DWORD decl[] =
		{
			D3DVSD_STREAM( 0 ),
				D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
			D3DVSD_STREAM( 1 ),
				D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
			D3DVSD_END()
		};

		Astronomy::pRS->CreateVertexShader( decl, NULL, &dwShader, 0 );

		iVertexBuffer = Astronomy::pRS->CreateVertexBuffer(0, dwSize * sizeof(CVECTOR), D3DUSAGE_WRITEONLY);
		iVertexBufferColors = Astronomy::pRS->CreateVertexBuffer(0, dwSize * sizeof(dword), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);

		CVECTOR * pVPos = (CVECTOR *)Astronomy::pRS->LockVertexBuffer(iVertexBuffer);
		dword * pVColors = (dword *)Astronomy::pRS->LockVertexBuffer(iVertexBufferColors);

		bool bRecalculateData = true;
		HANDLE hOutFile = fio->_CreateFile("resource\\star.dat");
		if(INVALID_HANDLE_VALUE != hOutFile)
		{
			dword dwFileLen;
			dwFileLen = fio->_GetFileSize(hOutFile,null);
			if( dwFileLen == dwSize*(sizeof(Astronomy::STARS::Star)+sizeof(CVECTOR)+sizeof(DWORD)) )
			{
				aStars.AddElements(dwSize);
				fio->_SetFilePointer(hOutFile,0,0,FILE_BEGIN);
				fio->_ReadFile(hOutFile, aStars.GetBuffer(), sizeof(Astronomy::STARS::Star)*dwSize, null);
				fio->_ReadFile(hOutFile, pVPos, sizeof(CVECTOR)*dwSize, null);
				fio->_ReadFile(hOutFile, pVColors, sizeof(DWORD)*dwSize, null);
				bRecalculateData = false;
			}
			fio->_CloseHandle(hOutFile);
		}

		if(bRecalculateData)
		{
			float fMinMag = -100.0f, fMaxMag = 100.0f;
			for (dword i=0; i<dwSize; i++)
			{
				Star & s = aStars[aStars.Add()];

				fio->_ReadFile(hFile, &s.fRA, sizeof(s.fRA), null);
				fio->_ReadFile(hFile, &s.fDec, sizeof(s.fDec), null);
				fio->_ReadFile(hFile, &s.fMag, sizeof(s.fMag), null);
				fio->_ReadFile(hFile, &s.cSpectr[0], sizeof(s.cSpectr), null);
				s.dwColor = Spectr[s.cSpectr[0]];

				if (s.fMag < fMaxMag) fMaxMag = s.fMag;
				if (s.fMag > fMinMag) fMinMag = s.fMag;

				s.vPos = CVECTOR(cosf(s.fDec) * cosf(s.fRA), cosf(s.fDec) * sinf(s.fRA), sinf(s.fDec));
				CVECTOR vPos = fRadius * s.vPos;
				s.fAlpha = (vPos.y < fHeightFade) ? Clamp(vPos.y / fHeightFade) : 1.0f;

				pVPos[i] = vPos;
				pVColors[i] = ARGB(s.fAlpha * 255.0f, 255, 255, 255);
			}
			//api->Trace("Stars: min = %.3f, max = %.3f", fMinMag, fMaxMag);

			// запишем все буферы в файл для того что бы в след раз не пересчитывать
			hOutFile = fio->_CreateFile("resource\\star.dat",GENERIC_WRITE,0,CREATE_ALWAYS);
			if( INVALID_HANDLE_VALUE != hOutFile )
			{
				fio->_WriteFile(hOutFile, aStars.GetBuffer(), sizeof(Astronomy::STARS::Star)*dwSize, null);
				fio->_WriteFile(hOutFile, pVPos, sizeof(CVECTOR)*dwSize, null);
				fio->_WriteFile(hOutFile, pVColors, sizeof(DWORD)*dwSize, null);
				fio->_CloseHandle(hOutFile);
			}
		}

		Astronomy::pRS->UnLockVertexBuffer(iVertexBuffer);
		Astronomy::pRS->UnLockVertexBuffer(iVertexBufferColors);
		fio->_CloseHandle(hFile);
	}
}

void Astronomy::STARS::Realize(double dDeltaTime, double dHour)
{
	// update fadein\fadeout
	if( fFadeTimeStart>=0.f )
	{
		if( (fFadeTime>0.f && fFadeValue<1.f) || (fFadeTime<0.f && fFadeValue>0.f) )
		{
			ENTITY_ID eid;
			if( api->FindClass(&eid,"Weather",0) )
			{
				float fTime = ((WEATHER_BASE*)eid.pointer)->GetFloat(whf_time_counter);
				if( fTime > fFadeTimeStart )
				{
					float fOldFadeValue = fFadeValue;

					if( fFadeTime>0.f ) fFadeValue = (fTime - fFadeTimeStart) / fFadeTime;
					else fFadeValue = 1.f + (fTime - fFadeTimeStart) / fFadeTime;
					if( fFadeValue<0.f ) fFadeValue = 0.f;
					if( fFadeValue>1.f ) fFadeValue = 1.f;

					if( (long)(20.f*fOldFadeValue) != (long)(20.f*fFadeValue) )
						fPrevFov = -1.f;

					bEnable = fFadeValue > 0.f;
				}
			}
		}
	}

	if (!IsEnable()) return;
	if( iVertexBufferColors==-1 ) return;

	CVECTOR vCamPos, vCamAng; float fFov;
	CMatrix	mView, IMatrix;
	RS_RECT rr[1000];

	Astronomy::pRS->GetCamera(vCamPos, vCamAng, fFov);

	dword dw1;
	RDTSC_B(dw1);

	float fMaxMag = Bring2Range(fTelescopeMagnitude, fVisualMagnitude, 0.14f, 1.285f, fFov);

	//if (fabsf(fFov - fPrevFov) > 1e-5f)
	{
		float fTmpK[5];
		float fTmpRnd[7];
		m_fTwinklingTime += api->GetDeltaTime() * 0.001f * (0.8f + FRAND(0.2f));
		if( m_fTwinklingTime > PI*2*3*5*7 ) m_fTwinklingTime -= PI*2*3*5*7;
		fTmpK[0] = 0.7f + 0.3f * sinf(m_fTwinklingTime*0.5f);
		fTmpK[1] = 0.75f + 0.25f * sinf(m_fTwinklingTime*3.f);
		fTmpK[2] = 0.75f + 0.25f * sinf(m_fTwinklingTime*2.f);
		fTmpK[3] = 0.8f + 0.2f * sinf(m_fTwinklingTime*5.f);
		fTmpK[4] = 0.85f + 0.15f * sinf(m_fTwinklingTime*7.f);
		for (long n=0; n<7; n++) fTmpRnd[n] = 0.8f + FRAND(0.2f);
		dword * pVColors = (dword *)Astronomy::pRS->LockVertexBuffer(iVertexBufferColors, D3DLOCK_DISCARD);
		for (dword i=0; i<aStars.Size(); i++)
		{
			Star & s = aStars[i];

			float fAlpha = fFadeValue * fTmpK[i%5]*fTmpRnd[i%7] * s.fAlpha * 255.0f * Bring2Range(1.0f, 0.01f, -2.0f, fMaxMag, s.fMag);

			dword dwAlpha; FTOL(dwAlpha, fAlpha);
			pVColors[i] = (dwAlpha << 24L) | s.dwColor;
		}
		Astronomy::pRS->UnLockVertexBuffer(iVertexBufferColors);
	}

	Astronomy::pRS->SetRenderState( D3DRS_POINTSPRITEENABLE, true );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSCALEENABLE,  true );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSIZE,     F2DW(fSize) );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSIZE_MIN, F2DW(0.0f) );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSCALE_A,  F2DW(0.0f) );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSCALE_B,  F2DW(0.0f) );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSCALE_C,  F2DW(1.0f) );

	CMatrix mWorld;
	mWorld.BuildPosition(vCamPos.x, vCamPos.y, vCamPos.z);
	Astronomy::pRS->SetTransform(D3DTS_WORLD, mWorld);
	Astronomy::pRS->TextureSet(0, iTexture);
	Astronomy::pRS->SetVertexShader(dwShader);
	Astronomy::pRS->SetStreamSource(0, Astronomy::pRS->GetVertexBuffer(iVertexBuffer), sizeof(CVECTOR));
	Astronomy::pRS->SetStreamSource(1, Astronomy::pRS->GetVertexBuffer(iVertexBufferColors), sizeof(dword));

	if (Astronomy::pRS->TechniqueExecuteStart("Stars")) do {
		Astronomy::pRS->DrawPrimitive(D3DPT_POINTLIST, 0, aStars.Size());
	} while (Astronomy::pRS->TechniqueExecuteNext());

	Astronomy::pRS->SetRenderState( D3DRS_POINTSPRITEENABLE, false );
	Astronomy::pRS->SetRenderState( D3DRS_POINTSCALEENABLE,  false );

	Astronomy::pRS->SetStreamSource(1, null, 0);

	/*Astronomy::pRS->GetTransform(D3DTS_VIEW, mView);
	Astronomy::pRS->GetCamera(vCamPos, vCamAng, fFov);
	float fScaleY = Astronomy::pRS->GetHeightDeformator();

	float fFV = Bring2Range(0.01f, 1.0f, 0.01f, 1.285f, fFov);
	float fMaxMag = Bring2Range(fTelescopeMagnitude, fVisualMagnitude, 0.14f, 1.285f, fFov);

	float fRAHour = float(double(PIm2) * dHour * 15.0 / 360.0);

	float fTexDX = 1.0f / float(dwSubTexturesX);
	float fTexDY = 1.0f / float(dwSubTexturesY);

	StarVertex * pV = null;

	Astronomy::pRS->TextureSet(0, iTexture);
	Astronomy::pRS->SetTransform(D3DTS_VIEW, IMatrix);
	Astronomy::pRS->SetTransform(D3DTS_WORLD, IMatrix);

	dword dwStars = 0;
	long idx = 0;
	for (dword i=0; i<aStars.Size(); i+=1)
	{
		if (idx == 0 && !pV)
		{
			pV = (StarVertex*)Astronomy::pRS->LockVertexBuffer(iVertexBuffer, D3DLOCK_DISCARD);
			if (!pV) break;
		}

		Star & s = aStars[i];
		RS_RECT & r = rr[idx];

		long iRA;
		float fRa = float(COS_TABLE_SIZE) * (fRAHour + s.fRA); FTOL(iRA, fRa);

		float fCosRA = fCosTable[iRA & (COS_TABLE_SIZE - 1)];
		float fSinRA = fCosTable[(iRA + COS_TABLE_SIZE / 4) & (COS_TABLE_SIZE - 1)];

		CVECTOR vPos = vCamPos + fRadius * CVECTOR(s.fCosDec * fCosRA, s.fCosDec * fSinRA, s.fSinDec);
		if (vPos.y < 0.0f) continue;

		float fHA = (vPos.y < fHeightFade) ? vPos.y / fHeightFade : 1.0f;

		float fAlpha = 255.0f * fHA * Bring2Range(1.0f, 0.01f, -0.1f, fMaxMag, s.fMag);
		if (fAlpha <= (0.010001f * 255.0f)) continue;

		// fill vertex buffer
		CVECTOR vPos1 = mView * vPos;
		//if (vPos.z < 0.0f) continue;
		dword dwAlpha; FTOL(dwAlpha, fAlpha);
		dword dwStarsColor = (dwAlpha << 24L) | dwColor;		
		float fStarsSize = fSize * fFV;

		pV[0].vPos = vPos1 + CVECTOR(-fStarsSize, -fStarsSize * fScaleY, 0.0f);
		pV[0].dwColor = dwStarsColor;
		pV[0].tu = s.fTexX; pV[0].tv = s.fTexY; 

		pV[1].vPos = vPos1 + CVECTOR(-fStarsSize, fStarsSize * fScaleY, 0.0f);
		pV[1].dwColor = dwStarsColor;
		pV[1].tu = s.fTexX; pV[1].tv = s.fTexY + fTexDY; 

		pV[2].vPos = vPos1 + CVECTOR(fStarsSize, -fStarsSize * fScaleY, 0.0f);
		pV[2].dwColor = dwStarsColor;
		pV[2].tu = s.fTexX + fTexDX; pV[2].tv = s.fTexY; 

		pV[3].vPos = vPos1 + CVECTOR(fStarsSize, fStarsSize * fScaleY, 0.0f);
		pV[3].dwColor = dwStarsColor;
		pV[3].tu = s.fTexX + fTexDX; pV[3].tv = s.fTexY + fTexDY; 

		pV += 4;

		idx++;
		dwStars++;
		if (idx == MAX_RECTS)
		{
			Astronomy::pRS->UnLockVertexBuffer(iVertexBuffer);
			Astronomy::pRS->DrawBuffer(iVertexBuffer, sizeof(StarVertex), iIndexBuffer, 0, idx * 4, 0, idx * 2, "Stars");
			pV = null;
			idx = 0;
		}
	}

	if (idx && pV)
	{
		Astronomy::pRS->UnLockVertexBuffer(iVertexBuffer);
		Astronomy::pRS->DrawBuffer(iVertexBuffer, sizeof(StarVertex), iIndexBuffer, 0, idx * 4, 0, idx * 2, "Stars");
	}*/
	RDTSC_E(dw1);

	fPrevFov = fFov;
	
	//api->Trace("RDTSC = %d", dw1);
	//Astronomy::pRS->SetTransform(D3DTS_VIEW, mView);
}

dword Astronomy::STARS::AttributeChanged(ATTRIBUTES * pA)
{
	return 0;
}

void Astronomy::STARS::TimeUpdate(ATTRIBUTES * pAP)
{
	bEnable = false;
	pAP = pAP->FindAClass(pAP, "Stars"); if (!pAP) return;

	bEnable = pAP->GetAttributeAsDword("Enable", 0) != 0;
	fRadius = pAP->GetAttributeAsFloat("Radius", 2000.0f);
	fHeightFade = pAP->GetAttributeAsFloat("HeightFade", 100.0f);

	fSize = pAP->GetAttributeAsFloat("Size", 20.0f);
	fSunFade = pAP->GetAttributeAsFloat("SunFade", 1.0f);

	fFadeValue = pAP->GetAttributeAsFloat("FadeValue", 1.f);
	fFadeTimeStart = pAP->GetAttributeAsFloat("FadeStartTime", -1.f);
	fFadeTime = pAP->GetAttributeAsFloat("FadeTime", 0.2f);

	if (!bEnable) return;
	if( iVertexBuffer==-1 )
	{
		bEnable=false;
		api->Trace("Warning! Weather has not stars parameters");
		return;
	}

	CVECTOR * pVPos = (CVECTOR *)Astronomy::pRS->LockVertexBuffer(iVertexBuffer);
	if( !pVPos ) {
		bEnable=false;
		return;
	}

	fPrevFov = -1.0f;
	for (dword i=0; i<aStars.Size(); i++)
	{
		Star & s = aStars[i];
		CVECTOR vPos = fRadius * s.vPos;
		s.fAlpha = (vPos.y < fHeightFade) ? Clamp(vPos.y / fHeightFade) : 1.0f;
		pVPos[i] = vPos;
	}

	Astronomy::pRS->UnLockVertexBuffer(iVertexBuffer);
}
