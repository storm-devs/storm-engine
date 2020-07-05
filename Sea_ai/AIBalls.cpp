#include "AIBalls.h"
#include "AIFort.h"

AIBalls * AIBalls::pAIBalls = null;

AIBalls::AIBalls() : aBallTypes(_FL_, 4), aBallRects(_FL_, 256)
{
	pSail = null;
	pSea = null;
	pFort = null;
	pIsland = null;

	pVWShips = null;
	//pVWForts = null;
	pAIBalls = this;

	fBallFlySoundDistance = 1.0f;

	fDeltaTimeMultiplyer = 1.0f;
	dwFireBallFromCameraTime = 0;
}

AIBalls::~AIBalls()
{
	dword	i, j;

	pAIBalls = null;

	AIHelper::pRS->TextureRelease(dwTextureIndex);

	for (i=0;i<aBallTypes.Size();i++)
	{	
		BALL_TYPE * pBallsType = &aBallTypes[i];
		for (j=0;j<pBallsType->Balls.Size();j++)
		{
			BALL_PARAMS * pBall = &pBallsType->Balls[j];
			if (pBall->pParticle)
			{
				DELETE(pBall->pParticle);
			}
			pBall->sBallEvent.DelAll();
		}
		aBallTypes[i].Balls.DelAll();
	}
	aBallTypes.DelAll();

	DELETE(pVWShips);
}

bool AIBalls::Init()
{
	SetDevice();
	return true;
}

void AIBalls::SetDevice()
{
}

void AIBalls::FireBallFromCamera()
{
	VDATA * pMainCharIndex = (VDATA *)api->GetScriptVariable("nMainCharacterIndex"); if (!pMainCharIndex) return;
	long iMainCharIndex = pMainCharIndex->GetLong(); if (iMainCharIndex < 0) return;
	VDATA * pMainCharacter = (VDATA *)api->GetScriptVariable("Characters"); if (!pMainCharacter) return;
	ATTRIBUTES * pAMainCharacter = pMainCharacter->GetAClass(iMainCharIndex); if (!pAMainCharacter) return;
	ATTRIBUTES * pACannonType = pAMainCharacter->FindAClass(pAMainCharacter, "Ship.Cannons.Type"); if (!pACannonType) return;
	dword dwCannonType = pACannonType->GetAttributeAsDword();

	ATTRIBUTES * pABall = pAMainCharacter->CreateAttribute("_err324__", ""); if (!pABall) return;

	/*AIHelper::pRS->GetTransform(D3DTS_VIEW, mView);
	CMatrix mIView = mView;
	mIView.Transposition3X3();
	float fY = atan2f(mIView.Vz().x, mIView.Vz().z);
	float fX = SIGN(mIView.Vz().y) * acosf(mIView.Vz() | CVECTOR(mIView.Vz().x, 0.0f, mIView.Vz().z));*/

	CVECTOR vCamPos, vCamAng;
	float fFov;
	AIHelper::pRS->GetCamera(vCamPos, vCamAng, fFov);

	CMatrix mView(vCamAng, vCamPos);
	CMatrix mIView = mView;
	//mIView.Transposition3X3();
	float fY = atan2f(mIView.Vz().x, mIView.Vz().z);
	float fX = SIGN(mIView.Vz().y) * acosf(mIView.Vz() | CVECTOR(mIView.Vz().x, 0.0f, mIView.Vz().z));

	pABall->SetAttribute("Type", "Balls");
	pABall->SetAttributeUseDword("CharacterIndex", dword(iMainCharIndex));
	pABall->SetAttributeUseFloat("x", mIView.Pos().x);
	pABall->SetAttributeUseFloat("y", mIView.Pos().y);
	pABall->SetAttributeUseFloat("z", mIView.Pos().z);
	pABall->SetAttributeUseFloat("SpdV0", 100.0f);
	pABall->SetAttributeUseFloat("HeightMultiply", 1.0f);
	pABall->SetAttributeUseFloat("SizeMultiply", 1.0f);
	pABall->SetAttributeUseFloat("TimeSpeedMultiply", 1.0f);
	pABall->SetAttributeUseDword("CannonType", dwCannonType);
	pABall->SetAttributeUseFloat("MaxFireDistance", 15000.0f);

	pABall->SetAttributeUseFloat("Ang", fX);
	pABall->SetAttributeUseFloat("Dir", fY);

	AddBall(pABall);

	pAMainCharacter->DeleteAttributeClassX(pABall);
}

void AIBalls::AddBall(ATTRIBUTES * pABall)
{
	char *pBallName = pABall->GetAttribute("Type"); Assert(pBallName);
	
	for (dword i=0;i<aBallTypes.Size();i++) if (stricmp(aBallTypes[i].sName,pBallName)==0) break;
	if (i == aBallTypes.Size()) return;

	BALL_PARAMS * pBall = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];

	pBall->iBallOwner = pABall->GetAttributeAsDword("CharacterIndex");

#define GetAFloat(x)	pABall->GetAttributeAsFloat(x)
#define GetADword(x)	pABall->GetAttributeAsDword(x)

	pBall->fTime = 0.0f;
	pBall->vPos = pBall->vFirstPos = CVECTOR(GetAFloat("x"), GetAFloat("y"), GetAFloat("z"));
	pBall->fSpeedV0 = GetAFloat("SpdV0");
	pBall->fHeightMultiply = GetAFloat("HeightMultiply");
	pBall->fSizeMultiply = GetAFloat("SizeMultiply");
	pBall->fTimeSpeedMultiply = GetAFloat("TimeSpeedMultiply");
	pBall->dwCannonType = GetADword("CannonType");
	pBall->fMaxFireDistance = GetAFloat("MaxFireDistance");
	float fAngle = GetAFloat("Ang");
	pBall->fCosAngle = cosf(fAngle);
	pBall->fSinAngle = sinf(fAngle);
	float fDir = GetAFloat("Dir");
	pBall->fDirX = cosf(fDir);
	pBall->fDirZ = sinf(fDir);
	pBall->pParticle = null;

	pBall->sBallEvent = pABall->GetAttribute("Event");

	if (aBallTypes[i].sParticleName.Len())
	{
		ENTITY_ID eidParticle;
		if (api->FindClass(&eidParticle,"particles",0))
		{
			pBall->pParticle = (VPARTICLE_SYSTEM *)api->Send_Message(eidParticle,"lsffffffl",PS_CREATE_RIC,(char*)aBallTypes[i].sParticleName.GetBuffer(),pBall->vPos.x,pBall->vPos.y,pBall->vPos.z,0.0f,1.0f,0.0f,100000);
		}
	}
}

void AIBalls::Execute(dword Delta_Time)
{
	dword						i, j;
	CVECTOR						vSrc, vDst;
	ENTITY_ID					EID, *pEID;

	if (!pIsland && api->FindClass(&EID,"island", 0))
		pIsland = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);
	if (!pSail && api->FindClass(&EID,"sail", 0))
		pSail = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);
	if (!pSea && api->FindClass(&EID,"sea", 0))
		pSea = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);

	aBallRects.Empty();

	//if (!pVWForts) pVWForts = (VIDWALKER*)api->LayerGetWalker("fort_cannon_trace");
	if (!pVWShips) pVWShips = (VIDWALKER*)api->LayerGetWalker("ship_cannon_trace");

	CMatrix mView = Render().GetView();

	float fDeltaTime = 0.001f * float(Delta_Time);

	for (i=0;i<aBallTypes.Size();i++)
	{	
		BALL_TYPE * pBallsType = &aBallTypes[i];

		AttributesPointer->SetAttributeUseDword("CurrentBallType", pBallsType->dwGoodIndex);

		for (j=0;j<pBallsType->Balls.Size();j++)
		{
			BALL_PARAMS * pBall = &pBallsType->Balls[j];

			vSrc = pBall->vPos;

			AttributesPointer->SetAttributeUseDword("CurrentBallCannonType", pBall->dwCannonType);
			AttributesPointer->SetAttributeUseFloat("CurrentBallDistance", sqrtf(~(pBall->vPos - pBall->vFirstPos)));
			AttributesPointer->SetAttributeUseFloat("CurrentMaxBallDistance", pBall->fMaxFireDistance);

			// update ball time
			pBall->fTime += fDeltaTime * fDeltaTimeMultiplyer * pBall->fTimeSpeedMultiply;
			// update positions
			float fsX = pBall->fSpeedV0 * pBall->fTime * pBall->fCosAngle;
			float fsY = pBall->fHeightMultiply * (pBall->fSpeedV0 * pBall->fTime * pBall->fSinAngle - AIHelper::fGravity * SQR(pBall->fTime) / 2.0f);
			pBall->vPos = CVECTOR(0.0f, fsY, fsX);
			RotateAroundY(pBall->vPos.x, pBall->vPos.z, pBall->fDirX, pBall->fDirZ);
			pBall->vPos += pBall->vFirstPos;

			vDst = pBall->vPos;

			if (pBall->sBallEvent.Len())
				api->Event((char*)pBall->sBallEvent.GetBuffer(), "lllffffffs", pBall->iBallOwner, (dword)1, pBallsType->dwGoodIndex, pBall->vPos.x, pBall->vPos.y, pBall->vPos.z, vSrc.x, vSrc.y, vSrc.z);
			
			if (pBall->pParticle)
			{
				CVECTOR vDir = !(vSrc - vDst);
				pBall->pParticle->SetEmitter(pBall->vPos, vDir);
			}

			float fRes = 2.0f;
			
			CVECTOR v1 = mView * vSrc;
			CVECTOR v2 = mView * vDst;
			
			if (SIGN(v1.z) != SIGN(v2.z))
			{
				float fDelta = fabsf(v1.z / (v2.z - v1.z));
				float x = v1.x + (v2.x - v1.x) * fDelta;
				float y = v1.y + (v2.y - v1.y) * fDelta;

				if (Sqr(x) + Sqr(y) <= Sqr(fBallFlySoundDistance))
				{
					CVECTOR vRes, v = fBallFlySoundStereoMultiplyer * CVECTOR(x, y, 0.0f);
					mView.MulToInv(v, vRes);

					api->Event(BALL_FLY_NEAR_CAMERA, "fff", vRes.x, vRes.y, vRes.z); 
				}
			}

			// sail trace
			if (pSail) 
				pSail->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

			// ship trace
			if (pVWShips && (pEID = pVWShips->GetID()) != 0) do 
			{
				CANNON_TRACE_BASE * pShip = (CANNON_TRACE_BASE*)api->GetEntityPointer(pEID);
				fRes = pShip->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
				if (fRes <= 1.0f) break;
			} while (pEID = pVWShips->GetIDNext());

			// fort trace
			if (fRes > 1.0f && AIFort::pAIFort)
			{
				fRes = AIFort::pAIFort->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);
			}

			// island trace
			if (fRes>1.0f && pIsland) 
				fRes = pIsland->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

			// sea trace
			if (fRes>1.0f && pSea) 
				fRes = pSea->Cannon_Trace(pBall->iBallOwner, vSrc, vDst);

			// delete ball
			if (fRes<=1.0f)
			{
				if (pBall->sBallEvent.Len())
				{
					api->Event((char*)pBall->sBallEvent.GetBuffer(),"lllffffff", pBall->iBallOwner, (dword)0, pBallsType->dwGoodIndex, pBall->vPos.x, pBall->vPos.y, pBall->vPos.z, vSrc.x, vSrc.y, vSrc.z);
					pBall->sBallEvent.DelAll();
				} 

				if (pBall->pParticle)
				{
					pBall->pParticle->Stop();
					DELETE(pBall->pParticle);
				}
				pBallsType->Balls.ExtractNoShift(j); j--;

				continue;
			}

			if (!pBall->sBallEvent.Len())
			{
				RS_RECT * pRSR = &aBallRects[aBallRects.Add()];
				pRSR->vPos = pBall->vPos;
				pRSR->dwColor = 0xFFFFFF;
				pRSR->dwSubTexture = pBallsType->dwSubTexIndex;
				pRSR->fAngle = pBall->fTime * 3.0f;
				pRSR->fSize = pBallsType->fSize * pBall->fSizeMultiply;
			}
		}
	}
}

void AIBalls::Realize(dword Delta_Time)
{
	if (aBallRects.Size())
	{
		AIHelper::pRS->TextureSet(0, dwTextureIndex);
		AIHelper::pRS->DrawRects(&aBallRects[0], aBallRects.Size(), "Cannonballs", dwSubTexX, dwSubTexY);
		aBallRects.Empty();
	}

	dwFireBallFromCameraTime += Delta_Time;
	/*
#ifndef _XBOX
	if (api->Controls->GetDebugAsyncKeyState('C') < 0 && dwFireBallFromCameraTime > 30 && AttributesPointer->GetAttributeAsDword("FireBallFromCamera", 0) != 0) 
	{
		dwFireBallFromCameraTime = 0;
		FireBallFromCamera();
	}
#endif
*/
}

dword AIBalls::AttributeChanged(ATTRIBUTES * pAttributeChanged)
{
	if (*pAttributeChanged == "clear")
	{
		for (dword i=0; i<aBallTypes.Size(); i++)
		{	
			BALL_TYPE * pBallsType = &aBallTypes[i];

			for (dword j=0; j<pBallsType->Balls.Size(); j++)
			{
				BALL_PARAMS * pBall = &pBallsType->Balls[j];

				if (pBall->sBallEvent.Len())
					pBall->sBallEvent.DelAll();

				if (pBall->pParticle)
				{
					pBall->pParticle->Stop();
					DELETE(pBall->pParticle);
				}
			}

			pBallsType->Balls.DelAll();
		}
			
		return 0;
	}

	if (*pAttributeChanged == "add")
	{
		AddBall(AttributesPointer);
		return 0;
	}

	if (*pAttributeChanged == "isDone")
	{
		// load common parameters
		fBallFlySoundDistance = AttributesPointer->GetAttributeAsFloat("BallFlySoundDistance");
		fBallFlySoundStereoMultiplyer = AttributesPointer->GetAttributeAsFloat("BallFlySoundStereoMultiplyer");
		fDeltaTimeMultiplyer = AttributesPointer->GetAttributeAsFloat("SpeedMultiply");
		sTextureName = AttributesPointer->GetAttribute("Texture");
		dwSubTexX = AttributesPointer->GetAttributeAsDword("SubTexX");
		dwSubTexY = AttributesPointer->GetAttributeAsDword("SubTexY");
		
		dwTextureIndex = AIHelper::pRS->TextureCreate(sTextureName);

		// install balls 
		ATTRIBUTES *pAPBalls = AttributesPointer->GetAttributeClass("Balls");
		dword dwIdx = 0;
		while(pAPBalls && true)
		{
			char * pName = pAPBalls->GetAttributeName(dwIdx);		if (!pName) break;
			ATTRIBUTES * pAP = pAPBalls->GetAttributeClass(pName);	if (!pAP) break;
			
			dword dwBallNum = aBallTypes.Add();

			aBallTypes[dwBallNum].sName = pName;
			aBallTypes[dwBallNum].dwSubTexIndex = pAP->GetAttributeAsDword("SubTexIndex");
			aBallTypes[dwBallNum].dwGoodIndex = pAP->GetAttributeAsDword("GoodIndex");
			aBallTypes[dwBallNum].fSize = pAP->GetAttributeAsFloat("Size");
			aBallTypes[dwBallNum].fWeight = pAP->GetAttributeAsFloat("Weight");

			if (pAP->GetAttribute("Particle"))
				aBallTypes[dwBallNum].sParticleName = pAP->GetAttribute("Particle");
			
			dwIdx++;
		}
		return 0;
	}
	return 0;
}

dword AIBalls::ProcessMessage(MESSAGE & message)
{
	if (message.Long() == MSG_MODEL_RELEASE)
	{
		for (dword i=0; i<aBallTypes.Size(); i++)
			for (dword j=0; j<aBallTypes[i].Balls.Size(); j++)
			{
				BALL_PARAMS * pBall = &aBallTypes[i].Balls[j];

				if (pBall->pParticle)
				{
					pBall->pParticle->Stop();
					DELETE(pBall->pParticle);
				}
			}
	}
	return 0;
}

void AIBalls::Save(CSaveLoad * pSL)
{
	for (dword i=0; i<aBallTypes.Size(); i++)
	{
		pSL->SaveDword(aBallTypes[i].Balls.Size());
	
		for (dword j=0; j<aBallTypes[i].Balls.Size(); j++)
		{
			pSL->SaveBuffer((const char *)&aBallTypes[i].Balls[j], sizeof(BALL_PARAMS));
		}
	}
}

void AIBalls::Load(CSaveLoad * pSL)
{
	for (dword i=0; i<aBallTypes.Size(); i++)
	{
		dword dwNum = pSL->LoadDword();
	
		for (dword j=0; j<dwNum; j++)
		{
			BALL_PARAMS * pB = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];
			pSL->Load2Buffer((char *)pB);
			if (pB->pParticle)
			{
				pB->pParticle = null;
				ENTITY_ID eidParticle;
				if (api->FindClass(&eidParticle,"particles",0))
				{
					pB->pParticle = (VPARTICLE_SYSTEM *)api->Send_Message(eidParticle,"lsffffffl",PS_CREATE_RIC,(char*)aBallTypes[i].sParticleName.GetBuffer(),pB->vPos.x,pB->vPos.y,pB->vPos.z,0.0f,1.0f,0.0f,100000);
				}
			}
		}
	}
}