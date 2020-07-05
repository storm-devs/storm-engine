#include "Track.h"

VDX8RENDER * ShipTracks::ShipTrack::pRS = null;
SEA_BASE * ShipTracks::ShipTrack::pSea = null;
long ShipTracks::ShipTrack::iRefCount = 0;
dword ShipTracks::ShipTrack::dwMaxBufferSize1 = 0, ShipTracks::ShipTrack::dwMaxBufferSize2 = 0;
long ShipTracks::ShipTrack::iVTmpBuffer1 = -1, ShipTracks::ShipTrack::iVTmpBuffer2 = -1;
long ShipTracks::ShipTrack::iITmpBuffer1 = -1, ShipTracks::ShipTrack::iITmpBuffer2 = -1;

ShipTracks::ShipTracks() : aShips(_FL)
{
}

ShipTracks::~ShipTracks()
{
	aShips.DelAllWithPointers();
}

bool ShipTracks::Init()
{
	ENTITY_ID sea_id;

	ShipTrack::pRS = (VDX8RENDER *)api->CreateService("dx8render");	Assert(ShipTrack::pRS);
	if (api->FindClass(&sea_id, "sea", 0)) ShipTrack::pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);
	return true;
}

void ShipTracks::ResetTrack(SHIP_BASE * pShip)
{
	for (long i=0; i<aShips; i++) if (aShips[i]->pShip == pShip)
	{
		aShips[i]->Reset();
		break;
	}
}

void ShipTracks::AddShip(SHIP_BASE * pShip)
{
	ShipTrack * pST = NEW ShipTrack;
	
	if (pST->Update(pShip))
		aShips.Add(pST);
	else
		delete pST;
}

void ShipTracks::DelShip(SHIP_BASE * pShip)
{
	for (long i=0; i<aShips; i++) if (aShips[i]->pShip == pShip)
	{
		DELETE(aShips[i]);
		aShips.Extract(i);
		break;
	}
}

void ShipTracks::Execute(dword dwDeltaTime)
{
}

void ShipTracks::Realize(dword dwDeltaTime)
{
	for (long i=0; i<aShips; i++)
	{
		aShips[i]->Execute(float(dwDeltaTime) * 0.001f);
		aShips[i]->Realize(float(dwDeltaTime) * 0.001f);
	}
}

ShipTracks::ShipTrack::ShipTrack() : aTrack1(_FL_, 128), aTrack2(_FL_, 32)
{
	bFirstExecute = true;
	iTrackTexture1 = -1;
	iTrackTexture2 = -1;
	iRefCount++;
}

ShipTracks::ShipTrack::~ShipTrack()
{
	iRefCount--;
	if (iRefCount <= 0)
	{
		iRefCount = 0;

		pRS->ReleaseVertexBuffer(iVTmpBuffer1); iVTmpBuffer1 = -1;
		pRS->ReleaseVertexBuffer(iVTmpBuffer2); iVTmpBuffer2 = -1;

		pRS->ReleaseIndexBuffer(iITmpBuffer1);	iITmpBuffer1 = -1;
		pRS->ReleaseIndexBuffer(iITmpBuffer2);	iITmpBuffer2 = -1;

		dwMaxBufferSize1 = 0;
		dwMaxBufferSize2 = 0;
	}

	pRS->TextureRelease(iTrackTexture1); iTrackTexture1 = -1;
	pRS->TextureRelease(iTrackTexture2); iTrackTexture2 = -1;
}

bool ShipTracks::ShipTrack::Update(SHIP_BASE * pShip)
{
	ATTRIBUTES * pAChar = pShip->GetACharacter(); Assert(pAChar);
	ATTRIBUTES * pATrack = pAChar->FindAClass(pAChar, "Ship.Track");
	ATTRIBUTES * pATrack1 = pAChar->FindAClass(pAChar, "Ship.Track1");
	ATTRIBUTES * pATrack2 = pAChar->FindAClass(pAChar, "Ship.Track2");

	if (!pATrack || !pATrack1 || !pATrack2) return false;
	if (!pATrack->GetAttributeAsDword("Enable", 0)) return false;

	fTrackDistance = pATrack->GetAttributeAsFloat("TrackDistance");
	fUP1 = pATrack->GetAttributeAsFloat("WaveHeight1");
	fUP2 = pATrack->GetAttributeAsFloat("WaveHeight2");

	string sTex1 = pATrack1->GetAttribute("Texture");
	fZStart1 = pATrack1->GetAttributeAsFloat("ZStart");
	fLifeTime1 = pATrack1->GetAttributeAsFloat("LifeTime");
	sscanf(pATrack1->GetAttribute("Width"), "%f, %f", &fWidth11, &fWidth12);
	sscanf(pATrack1->GetAttribute("Speed"), "%f, %f", &fSpeed11, &fSpeed12);
	fTrackStep1 = pATrack1->GetAttributeAsFloat("TrackWidthSteps");

	string sTex2 = pATrack2->GetAttribute("Texture");
	fZStart2 = pATrack2->GetAttributeAsFloat("ZStart");
	fLifeTime2 = pATrack2->GetAttributeAsFloat("LifeTime");
	sscanf(pATrack2->GetAttribute("Width"), "%f, %f", &fWidth21, &fWidth22);
	sscanf(pATrack2->GetAttribute("Speed"), "%f, %f", &fSpeed21, &fSpeed22);
	fTrackStep2 = pATrack2->GetAttributeAsFloat("TrackWidthSteps");

	dwTrackStep1 = long(fTrackStep1);
	dwTrackStep2 = long(fTrackStep2);

	pRS->TextureRelease(iTrackTexture1); iTrackTexture1 = -1;
	pRS->TextureRelease(iTrackTexture2); iTrackTexture2 = -1;

	this->pShip = pShip;
	this->fTrackDistance = fTrackDistance;

	if (this->pShip)
	{
		iTrackTexture1 = pRS->TextureCreate(sTex1);
		iTrackTexture2 = pRS->TextureCreate(sTex2);

		this->vLastPos = pShip->GetPos();
		this->vLastAng = pShip->GetAng();
		this->fCurTV = 0.0f;
	}

	aTrack1.DelAll();
	aTrack2.DelAll();

	return true;
}

bool ShipTracks::ShipTrack::Reserve1(dword dwSize)
{
	dword dwNewSize = (dwSize / (100) + 1) * (100);

	if (dwMaxBufferSize1 >= dwNewSize) return iVTmpBuffer1 != -1 && iITmpBuffer1 != -1;

	dwMaxBufferSize1 = dwNewSize;

	pRS->ReleaseVertexBuffer(iVTmpBuffer1); iVTmpBuffer1 = -1;
	pRS->ReleaseIndexBuffer(iITmpBuffer1);	iITmpBuffer1 = -1;

	iVTmpBuffer1 = pRS->CreateVertexBuffer(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, dwMaxBufferSize1 * dwTrackStep1 * sizeof(TrackVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
	iITmpBuffer1 = pRS->CreateIndexBuffer(dwMaxBufferSize1 * dwTrackStep1 * 6 * sizeof(word), D3DUSAGE_WRITEONLY);

	if (iVTmpBuffer1==-1 || iITmpBuffer1==-1)
	{
		dwMaxBufferSize1 = 0;
		pRS->ReleaseVertexBuffer(iVTmpBuffer1); iVTmpBuffer1 = -1;
		pRS->ReleaseIndexBuffer(iITmpBuffer1);	iITmpBuffer1 = -1;
		return false;
	}

	word * pI = (word*)pRS->LockIndexBuffer(iITmpBuffer1);
	for (dword y=0; y<dwNewSize; y++)
		for (dword x=0; x<dwTrackStep1 - 1; x++)
		{
			*pI++ = word((y + 0) * dwTrackStep1 + x);
			*pI++ = word((y + 1) * dwTrackStep1 + x);
			*pI++ = word((y + 0) * dwTrackStep1 + x + 1);

			*pI++ = word((y + 1) * dwTrackStep1 + x);
			*pI++ = word((y + 1) * dwTrackStep1 + x + 1);
			*pI++ = word((y + 0) * dwTrackStep1 + x + 1);
		}
	pRS->UnLockIndexBuffer(iITmpBuffer1);

	return true;
}

bool ShipTracks::ShipTrack::Reserve2(dword dwSize)
{
	dword dwNewSize = (dwSize / (20) + 1) * (20);

	if (dwMaxBufferSize2 >= dwNewSize) return iVTmpBuffer2 != -1 && iITmpBuffer2 != -1;

	dwMaxBufferSize2 = dwNewSize;

	pRS->ReleaseVertexBuffer(iVTmpBuffer2); iVTmpBuffer2 = -1;
	pRS->ReleaseIndexBuffer(iITmpBuffer2);	iITmpBuffer2 = -1;

	iVTmpBuffer2 = pRS->CreateVertexBuffer(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, dwMaxBufferSize2 * dwTrackStep2 * sizeof(TrackVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
	iITmpBuffer2 = pRS->CreateIndexBuffer(dwMaxBufferSize2 * dwTrackStep2 * 6 * sizeof(word), D3DUSAGE_WRITEONLY);

	if (iVTmpBuffer2 == -1 || iITmpBuffer2 == -1)
	{
		dwMaxBufferSize2 = 0;
		pRS->ReleaseVertexBuffer(iVTmpBuffer2); iVTmpBuffer2 = -1;
		pRS->ReleaseIndexBuffer(iITmpBuffer2);	iITmpBuffer2 = -1;
		return false;
	}

	word * pI = (word*)pRS->LockIndexBuffer(iITmpBuffer2);
	for (dword y=0; y<dwNewSize; y++)
		for (dword x=0; x<dwTrackStep2 - 1; x++)
		{
			*pI++ = word((y + 0) * dwTrackStep2 + x);
			*pI++ = word((y + 1) * dwTrackStep2 + x);
			*pI++ = word((y + 0) * dwTrackStep2 + x + 1);

			*pI++ = word((y + 1) * dwTrackStep2 + x);
			*pI++ = word((y + 1) * dwTrackStep2 + x + 1);
			*pI++ = word((y + 0) * dwTrackStep2 + x + 1);
		}
		pRS->UnLockIndexBuffer(iITmpBuffer2);

		return true;
}

void ShipTracks::ShipTrack::Execute(float fDeltaTime)
{
	if (!pShip || !pSea) return;

	if (bFirstExecute)
	{
		vLastPos = pShip->GetPos();
		vLastAng = pShip->GetAng();
		fCurTV = 0.0f;
		bFirstExecute = false;
	}

	float fFov;
	CVECTOR vCamPos, vCamAng;
	pRS->GetCamera(vCamPos, vCamAng, fFov);
	float fCamDist = Clamp(sqrtf(~vCamPos) / 10000.0f);
	float fWaveUP = fUP1 + fCamDist * (fUP2 - fUP1);

	CVECTOR vCurPos = pShip->GetPos();
	CVECTOR vCurAng = pShip->GetAng();

	CVECTOR vBoxSize = pShip->GetBoxSize();

	CVECTOR vDist = vCurPos - vLastPos; vDist.y = 0.0;
	float fCurrentDistance = sqrtf(~vDist);
	if (fCurrentDistance > 100.0f)
	{
		fCurrentDistance = 0.0f;
		vLastPos = vCurPos;

		aTrack1.DelAll();
		aTrack2.DelAll();
	}
	if (fCurrentDistance > fTrackDistance)
	{
		float fSpeed = Min(1.0f, pShip->GetCurrentSpeed() / 20.0f);
		for (long i=0; i<long(fCurrentDistance / fTrackDistance); i++)
		{
			float fDistance = (i + 1) * fTrackDistance;

			aTrack1.Insert(0);
			CVECTOR vDir = (vCurPos - vLastPos); vDir.y = 0.0f;	vDir = !vDir;
			aTrack1[0].vPos = vLastPos + vDir * (vBoxSize.z * fZStart1 + fDistance);
			aTrack1[0].fCos = cosf(vCurAng.y);
			aTrack1[0].fSin = sinf(vCurAng.y);
			aTrack1[0].fTime = 0.0f;
			aTrack1[0].fTV = fCurTV;
			aTrack1[0].fWidth = RRnd(fWidth11, fWidth12); // 3.0f;
			aTrack1[0].fSpeed = fSpeed * RRnd(fSpeed11, fSpeed12); // 4.5f + (FRAND(1.0f) - 0.5f);
			aTrack1[0].fInitialAlpha = fSpeed;

			aTrack2.Insert(0);
			aTrack2[0].vPos = vLastPos + vDir * (vBoxSize.z * fZStart2 + fDistance);
			aTrack2[0].fCos = cosf(vCurAng.y);
			aTrack2[0].fSin = sinf(vCurAng.y);
			aTrack2[0].fTime = 0.0f;
			aTrack2[0].fTV = fCurTV;
			aTrack2[0].fWidth = RRnd(fWidth21, fWidth22); // 12.0f;
			aTrack2[0].fSpeed = fSpeed * RRnd(fSpeed21, fSpeed22); // 1.5f + (FRAND(0.4f) - 0.2f);
			aTrack2[0].fInitialAlpha = fSpeed;

			fCurTV += 0.05f;
		}
		vLastPos = vCurPos;
		vLastAng = vCurAng;
	}

	for (long i=0; i<aTrack1; i++)
	{
		Track & T = aTrack1[i];

		T.fTime += fDeltaTime;
		T.fAlpha = T.fInitialAlpha * Clamp(1.0f - T.fTime / fLifeTime1); //22.0f);
		T.fWidth += fDeltaTime * (T.fSpeed * (1.0f - T.fTime / fLifeTime1));

		if (T.fTime >= fLifeTime1)
		{
			aTrack1.Extract(i); i--;
			continue;
		}
	}

	if (Reserve1(aTrack1()))
		if (aTrack1.Len() > 1) 
		{
			TrackVertex * pV = (TrackVertex *)pRS->LockVertexBuffer(iVTmpBuffer1, D3DLOCK_DISCARD);
			for (long i=0; i<aTrack1; i++)
			{
				Track & T = aTrack1[i];
				long xxx = 0;
				for (float xx=0; xx<fTrackStep1; xx++)
				{
					float k = xx / (fTrackStep1 - 1.0f);
					float x = T.fWidth * (k - 0.5f);
					float z = 0.0f;
					RotateAroundY(x, z, T.fCos, T.fSin);
					x += T.vPos.x; z += T.vPos.z;
					CVECTOR vPos = CVECTOR(x, fWaveUP * (1.4f - fabsf((k * 2.0f) - 1.0f)) + pSea->WaveXZ(x, z), z);
					pV[i * dwTrackStep1 + xxx].vPos = vPos-vCurPos;
					pV[i * dwTrackStep1 + xxx].tu = xx / (fTrackStep1 - 1.0f);
					pV[i * dwTrackStep1 + xxx].tv = T.fTV;
					pV[i * dwTrackStep1 + xxx].dwColor = ARGB(T.fAlpha * 255.0f, 0, 0, 0);
					xxx++;
				}
			}
			pRS->UnLockVertexBuffer(iVTmpBuffer1);
		}

	for (long i=0; i<aTrack2; i++)
	{
		Track & T = aTrack2[i];

		T.fTime += fDeltaTime;
		T.fAlpha = T.fInitialAlpha * Clamp(1.0f - T.fTime / fLifeTime2); // 10.0f);
		T.fWidth -= fDeltaTime * (T.fSpeed * (1.0f - T.fTime / fLifeTime2));

		if (T.fTime >= fLifeTime2)
		{
			aTrack2.Extract(i); i--;
			continue;
		}
	}

	if (Reserve2(aTrack2()))
		if (aTrack2.Len() > 1)
		{
			TrackVertex * pV = (TrackVertex *)pRS->LockVertexBuffer(iVTmpBuffer2, D3DLOCK_DISCARD);
			for (long i=0; i<aTrack2; i++)
			{
				Track & T = aTrack2[i];
				long xxx = 0;
				for (float xx=0; xx<fTrackStep2; xx++)
				{
					float k = xx / (fTrackStep2 - 1.0f);
					float x = T.fWidth * (k - 0.5f);
					float z = 0.0f;
					RotateAroundY(x, z, T.fCos, T.fSin);
					x += T.vPos.x; z += T.vPos.z;
					CVECTOR vPos = CVECTOR(x, fWaveUP + pSea->WaveXZ(x, z), z);
					pV[i * dwTrackStep2 + xxx].vPos = vPos-vCurPos;
					pV[i * dwTrackStep2 + xxx].tu = float(xx) / (fTrackStep2 - 1.0f);
					pV[i * dwTrackStep2 + xxx].tv = T.fTV * 6.0f;
					pV[i * dwTrackStep2 + xxx].dwColor = ARGB(T.fAlpha * 255.0f, 0, 0, 0);
					xxx++;
				}
			}
			pRS->UnLockVertexBuffer(iVTmpBuffer2);
		}
}

void ShipTracks::ShipTrack::Reset()
{
	bFirstExecute = true;

	aTrack1.DelAll();
	aTrack2.DelAll();
}

dword ShipTracks::AttributeChanged(ATTRIBUTES * pA)
{
	if (*pA == "ResetTracks")
	{
		for (long i=0; i<aShips; i++) { aShips[i]->Reset();	}
	}
	return 0;
}

void ShipTracks::ShipTrack::Realize(float fDeltaTime)
{
	if (!pShip || !pSea) return;

	//pRS->SetTransform(D3DTS_WORLD, CMatrix());
	CMatrix m;
	m.Pos() = pShip->GetPos();
	pRS->SetTransform(D3DTS_WORLD, m);

	if (aTrack1.Len() > 1) 
	{
		pRS->TextureSet(0, iTrackTexture1);
		pRS->DrawBuffer(iVTmpBuffer1, sizeof(TrackVertex), iITmpBuffer1, 0, aTrack1.Len() * dwTrackStep1, 0, (dwTrackStep1 - 1) * (aTrack1.Len() - 1) * 2, "ShipTrack");
	}

	if (aTrack2.Len() > 1)
	{
		pRS->TextureSet(0, iTrackTexture2);
		pRS->DrawBuffer(iVTmpBuffer2, sizeof(TrackVertex), iITmpBuffer2, 0, aTrack2.Len() * dwTrackStep2, 0, (dwTrackStep2 - 1) * (aTrack2.Len() - 1) * 2, "ShipTrack");
	}
}



