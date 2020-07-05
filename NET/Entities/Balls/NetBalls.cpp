#include "NetBalls.h"
#include "..\..\Server.h"

NetBalls::NetBalls() : aBallRects(_FL_, 128), aBallTypes(_FL_)
{
	wBallIndex = 0;
	pSail = null;
	pSea = null;
	pIsland = null;
	pNetFort = null;
	pVWShips = null;
	pShipsLights = null;
}

NetBalls::~NetBalls()
{
	Render().TextureRelease(iTextureIndex);

	for (dword i=0; i<aBallTypes.Size(); i++)
	{	
		BALL_TYPE * pBallsType = &aBallTypes[i];
		for (dword j=0; j<pBallsType->Balls.Size(); j++)
		{
			BALL_PARAMS * pBall = &pBallsType->Balls[j];
			if (pBall->pParticle)
			{
				DELETE(pBall->pParticle);
			}
		}
		aBallTypes[i].Balls.DelAll();
	}
	aBallTypes.DelAll();

	DELETE(pVWShips);
}

bool NetBalls::Init()
{
	return true;
}

void NetBalls::Execute(dword Delta_Time)
{
	dword						i, j;
	CVECTOR						vSrc, vDst;
	ENTITY_ID					EID, *pEID;

	VDATA * pServerCurrentBallIndex = (VDATA *)api->GetScriptVariable("iServerCurrentBallIndex");

	CMatrix mView = Render().GetView();

	aBallRects.Empty();

	if (IsServer())
	{
		if (!pSea && NetFindClass(true, &EID, "NetSea"))		pSea = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);
		if (!pSail && NetFindClass(true, &EID, "NetSail"))		pSail = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);
		if (!pIsland && NetFindClass(true, &EID, "NetIsland"))	pIsland = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);
		if (!pNetFort && NetFindClass(true, &EID, "NetFort"))	pNetFort = (CANNON_TRACE_BASE*)api->GetEntityPointer(&EID);

		if (!pVWShips) pVWShips = (VIDWALKER*)api->LayerGetWalker("ship_cannon_trace");
	}

	float fDeltaTime = 0.001f * float(Delta_Time);

	for (i=0; i<aBallTypes.Size(); i++)
	{	
		BALL_TYPE * pBallsType = &aBallTypes[i];

		AttributesPointer->SetAttribute("CurrentBallType", (char*)pBallsType->sName.GetBuffer());

		for (j=0; j<pBallsType->Balls.Size(); j++)
		{
			BALL_PARAMS * pBall = &pBallsType->Balls[j];

			vSrc = pBall->vPos;

			if (IsServer()) AttributesPointer->SetAttributeUseFloat("CurrentBallDistance", sqrtf(~(pBall->vPos - pBall->vFirstPos)));

			// update ball time
			pBall->fTime += fDeltaTime * fDeltaTimeMultiplyer;
			// update positions
			float fsX = pBall->fSpeedV0 * pBall->fTime * pBall->fCosAngle;
			float fsY = pBall->fSpeedV0 * pBall->fTime * pBall->fSinAngle - 9.81f * SQR(pBall->fTime) / 2.0f;
			pBall->vPos = CVECTOR(0.0f, fsY, fsX);
			RotateAroundY(pBall->vPos.x, pBall->vPos.z, pBall->fDirX, pBall->fDirZ);
			pBall->vPos += pBall->vFirstPos;

			vDst = pBall->vPos;

			if (pBall->pParticle)
			{
				CVECTOR vDir = !(vSrc - vDst);
				pBall->pParticle->SetEmitter(pBall->vPos, vDir);
			}

			if (IsClient())
			{
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

						api->Event("NetClient_BallFlyNearCamera", "fff", vRes.x, vRes.y, vRes.z); 
					}
				}
			}

			if (IsServer())
			{
				if (pServerCurrentBallIndex)
					pServerCurrentBallIndex->Set(long(pBall->wBallIndex));

				float fRes = 2.0f;

				// sail trace
				if (pSail) 
					pSail->Cannon_Trace(pBall->wBallOwner, vSrc, vDst);

				// ship trace
				if (pVWShips && (pEID = pVWShips->GetID()) != 0) do 
				{
					CANNON_TRACE_BASE * pShip = (CANNON_TRACE_BASE*)api->GetEntityPointer(pEID);
					if (pShip && pShip->IsServer())
					{
						fRes = pShip->Cannon_Trace(pBall->wBallOwner, vSrc, vDst);
						if (fRes <= 1.0f) break;
					}
				} while (pEID = pVWShips->GetIDNext());

				// fort trace
				if (fRes > 1.0f && pNetFort)
					pNetFort->Cannon_Trace(pBall->wBallOwner, vSrc, vDst);

				// island trace
				if (fRes > 1.0f && pIsland) 
					fRes = pIsland->Cannon_Trace(pBall->wBallOwner, vSrc, vDst);

				// sea trace
				if (fRes > 1.0f && pSea) 
					fRes = pSea->Cannon_Trace(pBall->wBallOwner, vSrc, vDst);

				// delete ball
				if (fRes <= 1.0f)
				{
					//api->Event("NetServer_OnDelBall", "l", long(pBall->wBallIndex));

					if (pBall->pParticle)
					{
						pBall->pParticle->Stop();
						DELETE(pBall->pParticle);
					}
					pBallsType->Balls.ExtractNoShift(j); j--;

					continue;
				}
			}

			RS_RECT & B = aBallRects[aBallRects.Add()];
			B.vPos = pBall->vPos;
			B.dwColor = 0xFFFFFF;
			B.dwSubTexture = pBallsType->dwSubTexIndex;
			B.fAngle = pBall->fTime * 3.0f;
			B.fSize = pBallsType->fSize;
		}
	}
}

void NetBalls::Realize(dword Delta_Time)
{
	if (aBallRects.Size())
	{
		Render().TextureSet(0, iTextureIndex);
		Render().DrawRects(&aBallRects[0], aBallRects.Size(), "Cannonballs", dwSubTexX, dwSubTexY);
		aBallRects.Empty();
	}
}

void NetBalls::DelBall(dword dwBallIndex)
{
	for (dword i=0; i<aBallTypes.Size(); i++)
	{	
		BALL_TYPE * pBallsType = &aBallTypes[i];

		for (dword j=0; j<pBallsType->Balls.Size(); j++)
		{
			BALL_PARAMS * pBall = &pBallsType->Balls[j];
			if (pBall->wBallIndex == word(dwBallIndex))
			{
				if (pBall->pParticle)
				{
					pBall->pParticle->Stop();
					DELETE(pBall->pParticle);
				}
				pBallsType->Balls.ExtractNoShift(j);
				return;
			}
		}
	}
}

void NetBalls::AddBall(ATTRIBUTES * pABall)
{
	char * pBallName = pABall->GetAttribute("Type"); Assert(pBallName);

	for (dword i=0; i<aBallTypes.Size(); i++) if (stricmp(aBallTypes[i].sName, pBallName) == 0) break;
	if (i == aBallTypes.Size()) return;

	BALL_PARAMS * pBall = &aBallTypes[i].Balls[aBallTypes[i].Balls.Add()];

	pBall->wBallOwner = word(pABall->GetAttributeAsDword("ClientID"));

#define GetAFloat(x)	pABall->GetAttributeAsFloat(x)

	pBall->wBallIndex = wBallIndex++;
	pBall->fTime = 0.0f;
	pBall->vPos = pBall->vFirstPos = CVECTOR(GetAFloat("x"), GetAFloat("y"), GetAFloat("z"));
	pBall->fSpeedV0 = GetAFloat("SpdV0");
	float fAngle = GetAFloat("Ang");
	pBall->fCosAngle = cosf(fAngle);
	pBall->fSinAngle = sinf(fAngle);
	float fDir = GetAFloat("Dir");
	pBall->fDirX = cosf(fDir);
	pBall->fDirZ = sinf(fDir);

	pBall->pParticle = null;
	if (IsClient() && aBallTypes[i].sParticleName.Len())
	{
		ENTITY_ID eidParticle;
		if (NetFindClass(false, &eidParticle, "particles"))
		{
			pBall->pParticle = (VPARTICLE_SYSTEM *)api->Send_Message(eidParticle, "lsffffffl", PS_CREATE_RIC, (char*)aBallTypes[i].sParticleName.GetBuffer(), pBall->vPos.x,pBall->vPos.y,pBall->vPos.z,0.0f,1.0f,0.0f,100000);
		}
	}

	if (IsClient())
	{
		if (!pShipsLights)
		{
			ENTITY_ID eidTmp;
			if (NetFindClass(IsServer(), &eidTmp, "NetShipLights"))
				pShipsLights = (NetShipLights*)api->GetEntityPointer(&eidTmp); 
		}
		if (pShipsLights) pShipsLights->AddDynamicLights(null, pBall->vPos);

	}
}

dword NetBalls::AttributeChanged(ATTRIBUTES * pA)
{
	if (*pA == "ServerAdd" && IsServer())
	{
		AddBall(AttributesPointer);
	}

	if (*pA == "ClientAdd" && IsClient())
	{
		AddBall(AttributesPointer);
		return 0;
	}

	if (*pA == "ClientDel" && IsClient())
	{
		DelBall(pA->GetAttributeAsDword());
		return 0;
	}

	if (*pA == "isDone")
	{
		// load common parameters
		fDeltaTimeMultiplyer = AttributesPointer->GetAttributeAsFloat("SpeedMultiply");
		sTextureName = AttributesPointer->GetAttribute("Texture");
		dwSubTexX = AttributesPointer->GetAttributeAsDword("SubTexX");
		dwSubTexY = AttributesPointer->GetAttributeAsDword("SubTexY");
		fBallFlySoundDistance = AttributesPointer->GetAttributeAsFloat("BallFlySoundDistance", 2.0f);
		fBallFlySoundStereoMultiplyer = AttributesPointer->GetAttributeAsFloat("BallFlySoundStereoMultiplyer", 1.0f);

		iTextureIndex = (IsClient()) ? Render().TextureCreate(sTextureName) : -1;

		// install balls 
		ATTRIBUTES * pAPBalls = AttributesPointer->GetAttributeClass("Balls");
		dword dwIdx = 0;
		while (pAPBalls && true)
		{
			char * pName = pAPBalls->GetAttributeName(dwIdx);		if (!pName) break;
			ATTRIBUTES * pAP = pAPBalls->GetAttributeClass(pName);	if (!pAP) break;

			BALL_TYPE & BT = aBallTypes[aBallTypes.Add()];

			BT.sName = pName;
			BT.dwSubTexIndex = pAP->GetAttributeAsDword("SubTexIndex");
			BT.dwGoodIndex = pAP->GetAttributeAsDword("GoodIndex");
			BT.fSize = pAP->GetAttributeAsFloat("Size");

			if (pAP->GetAttribute("Particle"))
				BT.sParticleName = pAP->GetAttribute("Particle");

			dwIdx++;
		}

		return 0;
	}

	return 0;
}