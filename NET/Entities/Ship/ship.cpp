#include "ship.h" 
#include "CannonController.h" 
#include "Track.h" 
#include "..\Touch\Touch.h" 
#include "..\Fort\Fort.h" 
#include "..\..\..\engine\program\sound.h"

SEA_BASE					* NetShip::pSea = null;
ISLAND_BASE					* NetShip::pIsland = null;

NetShip::NetShip() : aFirePlaces(_FL_, 16), aMastFalls(_FL_), aShips(_FL_)
{
	pShipsLights = null;
	pCannonController = null;
	vSpeedAccel = 0.0f;
	iShipPriority = 31;
	bDead = false;
	bServerShip = false;
	fSailState = 0.0f;
	bShip2Strand = false;
	bSetLightAndFog = false;
	iNumMasts = 0;

	dtMastTrace.Setup(FRAND(0.5f), 0.5f);
	dtUpdateParameters.Setup(FRAND(1.0f), 1.0f);

	State.vPos = 0.0f;

	fRockingY = 0.02f;
	fRockingAZ = 0.02f;
	fRockingFactor = 0.025f;

	bVisible = true;
	bMounted = false;
	bKeelContour = false;
	bPerkTurnActive = false;

	pMasts = null;
	pNetTouch = null;

	iLastMsgServerTime = 0;

	curvePosition.iTime = 0;
	curveSpeed.iTime = 0;
	for (long i=0; i<5; i++)
	{
		curvePosition.vTime[i] = 0.0f;
		curveSpeed.vTime[i] = 0.0f;
	}

	curvePosition.curve.SetLine(CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f));
	curveSpeed.curve.SetLine(CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f), CVECTOR(0.0f));
}

NetShip::~NetShip()
{
	if (IsServer())
	{
		if (NetFindClass(IsServer(), &touch_id, "NetTouch"))
			pNetTouch = (NetTouch*)api->GetEntityPointer(&touch_id);
		if (pNetTouch) pNetTouch->DelShip(this);
	}

	api->DeleteEntity(GetModelEID());
	api->Send_Message(sail_id, "li", MSG_SAIL_DEL_GROUP, GetID());
	api->Send_Message(rope_id, "li", MSG_ROPE_DEL_GROUP, GetModelEID());
	api->Send_Message(flag_id, "li", MSG_FLAG_DEL_GROUP, GetModelEID());
	api->Send_Message(vant_id, "li", MSG_VANT_DEL_GROUP, GetModelEID());
	api->DeleteEntity(blots_id);
	DELETE(pMasts);

	DELETE(pCannonController);
	//aFirePlaces.DelAll();

	ENTITY_ID eidTmp;
	if (NetFindClass(IsServer(), &eidTmp, "NetShipTracks"))
	{
		NetShipTracks * pST = (NetShipTracks*)api->GetEntityPointer(&eidTmp); 
		if (pST) pST->DelShip(this);
	}
}

//##################################################################
bool NetShip::Init()
{
	ZERO3(State, SP, vPos);
	ZERO3(vAng, ShipPoints, Strength);

	Strength[STRENGTH_MAIN].bInertia = true;
	Strength[STRENGTH_MAIN].vSpeed = 0.0f;
	Strength[STRENGTH_MAIN].vRotate = 0.0f;

	srand(GetTickCount());

	LoadServices();

	SetACharacter(AttributesPointer);
	return true;
}

void NetShip::LoadServices()
{
	ENTITY_ID ent;

	pIsland = null;
	pSea = null;

	if (IsServer())
	{
		if (NetFindClass(IsServer(), &touch_id, "NetTouch"))
			pNetTouch = (NetTouch*)api->GetEntityPointer(&touch_id);
		if (pNetTouch) pNetTouch->AddShip(this);
	}

	if (NetFindClass(IsServer(), &ent, "NetIsland")) pIsland = (ISLAND_BASE*)api->GetEntityPointer(&ent);
	if (NetFindClass(IsServer(), &sea_id, "NetSea")) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);

	api->FindClass(&FirePlace::eidSound, "sound", 0);	
}

/*void NetShip::ShipRocking(float ftime)
{
	CMatrix mat;
    mat = *GetMatrix();

	if (!pSea) return;
	//api->Send_Message(model,"ll",2,(long)&mat);

	CVECTOR point, fang, fpos;
	fang.x = 0.0f;	fang.y = 0.0f;	fang.z = 0.0f;

	long	ix, iz;

	float fCos = cosf(State.vAng.y);
	float fSin = sinf(State.vAng.y);

	for(ix=0; ix<6; ix++)
	{
		float x = (float(ix)*State.vBoxSize.x*0.2f - 0.5f*State.vBoxSize.x);
		for(iz=0; iz<6; iz++)
		{
			ship_point_t * pspt = &ShipPoints[ix][iz];
			float z = (float(iz) * State.vBoxSize.z * 0.2f - 0.5f * State.vBoxSize.z);

			float xx = x, zz = z;
			RotateAroundY(xx,zz,fCos,fSin);
			point.x = xx + State.vPos.x;
			point.y = mat.m[0][1] * x + mat.m[2][1] * z + State.vPos.y;
			point.z = zz + State.vPos.z;
			
			float wave_y = pSea->WaveXZ(point.x, point.z);
			float f = (wave_y - point.y);
			
			pspt->fY = wave_y;
		}
	}

	State.vPos.y = (ShipPoints[2][2].fY + ShipPoints[3][2].fY + ShipPoints[2][3].fY + ShipPoints[3][3].fY) / 4.0f;

	// calculate ang.x and ang.z
	float fAng = 0.0f;
	for(iz=0; iz<3; iz++)
	{
		float fHeight = 0.0f, fHeight1 = 0.0f;
		for(ix=0; ix<6; ix++) 
		{
			fHeight += ShipPoints[ix][iz].fY;
			fHeight1 += ShipPoints[ix][5-iz].fY;
		}
		fHeight /= 6.0f;
		fHeight1 /= 6.0f;

		float z = 1.0f / (0.5f * State.vBoxSize.z - float(iz) * State.vBoxSize.z * 0.2f);
		fAng += (float)atan((fHeight-fHeight1) / 2.0f * z);
	}
	State.vAng.x = fAng / 3.0f;

	fAng = 0.0f;
	for(ix=0; ix<3; ix++)
	{
		float fHeight = 0.0f, fHeight1 = 0.0f;
		for(iz=0; iz<6; iz++) 
		{
			fHeight += ShipPoints[ix][iz].fY;
			fHeight1 += ShipPoints[5-ix][iz].fY;
		}
		fHeight /= 6.0f;
		fHeight1 /= 6.0f;

		float x = 1.0f / (0.5f * State.vBoxSize.x - float(ix) * State.vBoxSize.x * 0.2f);
		fAng += (float)atan((fHeight-fHeight1) / 2.0f * x);
	}

	State.vAng.z = -fAng / 3.0f;
}*/

CVECTOR NetShip::ShipRocking(float fDeltaTime)
{
	CMatrix mat;
	mat = *GetMatrix();

	//fDeltaTime = Min(fDeltaTime, 0.1f);
	float fDelta = (fDeltaTime / fRockingFactor);

	if (!pSea) return vAng;
	//api->Send_Message(model,"ll",2,(long)&mat);

	CVECTOR vAng2 = vAng;

	CVECTOR point, fang, fpos;
	fang.x = 0.0f;	fang.y = 0.0f;	fang.z = 0.0f;
	float fFullY = 0.0f, fup = 0.0f;

	long	ix, iz;

	float fCos = cosf(State.vAng.y);
	float fSin = sinf(State.vAng.y);

	for(ix=0; ix<6; ix++)
	{
		float x = (float(ix)*State.vBoxSize.x*0.2f - 0.5f*State.vBoxSize.x);
		for(iz=0; iz<6; iz++)
		{
			ship_point_t	*pspt = &ShipPoints[ix][iz];
			float z = (float(iz)*State.vBoxSize.z*0.2f - 0.5f*State.vBoxSize.z);

			float xx = x, zz = z;
			RotateAroundY(xx,zz,fCos,fSin);
			point.x = xx + State.vPos.x;
			point.y = mat.m[0][1]*x + mat.m[2][1]*z + State.vPos.y;
			point.z = zz + State.vPos.z;

			float wave_y = pSea->WaveXZ(point.x, point.z);
			float f = (wave_y - point.y);

			pspt->fY = wave_y;

			fFullY += pspt->fY;
		}
	}

	float fNewPos = fFullY / 36.0f;//(ShipPoints[2][2].fY + ShipPoints[3][2].fY + ShipPoints[2][3].fY + ShipPoints[3][3].fY) / 4.0f;
	State.vPos.y = State.vPos.y + Min(fDelta * fRockingY, 1.0f) * (fNewPos - State.vPos.y);//(ShipPoints[2][2].fY + ShipPoints[3][2].fY + ShipPoints[2][3].fY + ShipPoints[3][3].fY) / 4.0f;

	// calculate ang.x and ang.z
	float fAng = 0.0f;
	for(iz=0; iz<3; iz++)
	{
		float fHeight = 0.0f, fHeight1 = 0.0f;
		for(ix=0; ix<6; ix++) 
		{
			fHeight += ShipPoints[ix][iz].fY;
			fHeight1 += ShipPoints[ix][5-iz].fY;
		}
		fHeight /= 6.0f;
		fHeight1 /= 6.0f;

		float z = 1.0f / (0.5f * State.vBoxSize.z - float(iz) * State.vBoxSize.z * 0.2f);
		fAng += (float)atan((fHeight - fHeight1) / 2.0f * z);
	}
	vAng2.x = fAng / 3.0f;

	fAng = 0.0f;
	for(ix=0; ix<3; ix++)
	{
		float fHeight = 0.0f, fHeight1 = 0.0f;
		for(iz=0; iz<6; iz++) 
		{
			fHeight += ShipPoints[ix][iz].fY;
			fHeight1 += ShipPoints[5-ix][iz].fY;
		}
		fHeight /= 6.0f;
		fHeight1 /= 6.0f;

		float x = 1.0f / (0.5f * State.vBoxSize.x - float(ix) * State.vBoxSize.x * 0.2f);
		fAng += (float)atan((fHeight - fHeight1) / 2.0f * x);
	}
	vAng2.z = -fAng / 3.0f;

	return vAng + Min(fRockingAZ * fDelta, 1.0f) * (vAng2 - vAng);
}

bool NetShip::CalculateNewSpeedVector(CVECTOR * Speed, CVECTOR * Rotate)
{
	long	i;
	CVECTOR result(0.0f,0.0f,0.0f);
	for (i=0;i<RESERVED_STRENGTH;i++)
	{
		*Speed	+= Strength[i].vSpeed;
		*Rotate += Strength[i].vRotate;
	}
	
	Speed->z *= GetMaxSpeedZ();
	Rotate->y *= GetMaxSpeedY();

	return true;
}

bool NetShip::ApplyStrength(float dtime, bool bCollision)
{
	float	sign,fK; 
	long	i;
	
	// apply impulse strength
	for (i=RESERVED_STRENGTH; i<MAX_STRENGTH; i++) if (Strength[i].bUse && !Strength[i].bInertia)
	{
		//State.vRotate.x += Strength[i].vSpeed.z * 1.3f;
		State.vSpeed += Strength[i].vSpeed;
		State.vRotate += Strength[i].vRotate;
		if (!bCollision) Strength[i].bUse = false;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !                   Limitation  Section                      !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// must use calculated parameters!!!!!
	//if (State.vSpeed.x>5.0f) State.vSpeed.x = 5.0f;
	//if (State.vSpeed.x<-5.0f) State.vSpeed.x = -5.0f;
	//if (State.vSpeed.z>10.0f) State.vSpeed.z = 10.0f;
	//if (State.vSpeed.z<-10.0f) State.vSpeed.z = -10.0f;
	//if (State.vRotate.y>0.1f) State.vRotate.y = 0.1f;
	//if (State.vRotate.y<-0.1f) State.vRotate.y = -0.1f;

	// calculate new speed,rotate vector
	CVECTOR new_speed = CVECTOR(0.0f, 0.0f, 0.0f),
			new_rotate = CVECTOR(0.0f, 0.0f, 0.0f);

	CalculateNewSpeedVector(&new_speed, &new_rotate);
	if (bPerkTurnActive) { new_rotate.y = SIGN(fInitialPerkAngle) * GetMaxSpeedY(); }

#define SMALL_DELTA(x) (fabsf((x) - 0.00001f) > 0.00001f)
	for (i=0; i<3; i++)
	{
		fK = dtime * State.fMassInertia;
		if (i==0)		// x moving
		{
			float force = new_speed.v[i] - State.vSpeed.v[i];
			if (SMALL_DELTA(new_speed.v[i]))
				State.vSpeed.v[i] += fK * force * State.vInertiaAccel.x;
			else
				State.vSpeed.v[i] += fK * -(State.vSpeed.v[i]) * State.vInertiaBrake.x;
		}
		if (i==2)		// z moving
		{
			vSpeedAccel.z = State.vSpeed.v[i];

			float force = new_speed.v[i] - State.vSpeed.v[i];
			if (SMALL_DELTA(new_speed.v[i]))
				State.vSpeed.v[i] += fK * force * State.vInertiaAccel.z;
			else
			{
				State.vSpeed.v[i] += fK * -(State.vSpeed.v[i]) * State.vInertiaBrake.z;
			}
			// apply water resistance

			/*State.vSpeed.v[i] += dtime * State.fMassInertia * State.vWaterResis.x * force;*/
			vSpeedAccel.z -= State.vSpeed.v[i];
		}

		sign = SIGNZ(new_rotate.v[i]);
		if (i==1)		// y rotating
		{
			float prev_rot = State.vRotate.v[i];
			float force = new_rotate.v[i] - State.vRotate.v[i];

			if (SMALL_DELTA(new_rotate.v[i]))
				State.vRotate.v[i] += fK * force * State.vInertiaAccel.y;
			else
				State.vRotate.v[i] += fK * -State.vRotate.v[i] * State.vInertiaBrake.y;
		}
	}

	return true;
}

BOOL NetShip::TouchMove(DWORD DeltaTime, TOUCH_PARAMS * pTPOld, TOUCH_PARAMS * pTPNew)
{
	if (!pTPOld && !pTPNew) return false;
	SHIP_STATE old_state = State;

	if (pTPOld)
	{
		pTPOld->vSpeed	= State.vSpeed;		pTPOld->vRotate	= State.vRotate;
		pTPOld->vPos	= State.vPos;		pTPOld->vAng	= State.vAng;
	}

	Move(DeltaTime,true);
	
	if (pTPNew)
	{
		pTPNew->vSpeed	= State.vSpeed;		pTPNew->vRotate	= State.vRotate;
		pTPNew->vPos	= State.vPos;		pTPNew->vAng	= State.vAng;
	}
	
	State = old_state; 

	return true;
}

bool NetShip::Move(DWORD DeltaTime, bool bCollision)
{
	float dtime = DELTA_TIME(DeltaTime);
	/*if (IsServer()) */ApplyStrength(dtime, bCollision);
	if (!bCollision && !isDead()) State.vAng = ShipRocking(dtime);

	// Update position
	//if (!isDead())
	{
		float k = KNOTS2METERS(State.vSpeed.z) * dtime;
		State.vPos.z += k * cosf(State.vAng.y);
		State.vPos.x += k * sinf(State.vAng.y);

		k = KNOTS2METERS(State.vSpeed.x) * dtime;
		State.vPos.z += k * cosf(State.vAng.y + PId2);
		State.vPos.x += k * sinf(State.vAng.y + PId2);

		State.vAng.y += State.vRotate.y * dtime;
	}

	return true;
}

#define SHIP_TEST_Z_TIME	1.0f
#define SHIP_TEST_Y_TIME	1.0f

float NetShip::GetRotationAngle(float * pfTime)
{
	if (pfTime) *pfTime = 0.0f;
	if (fabsf(State.vRotate.y)<0.001f) return 0.0f;
	float fAngle = 0.0f;

	float fDist = 0.0f, fTime = SHIP_TEST_Y_TIME;
	float fSpeedY = State.vRotate.y;
	float fK = SHIP_TEST_Y_TIME * State.fMassInertia * State.vInertiaBrake.y;
	
	while (true)		// not funny!!! IT'S SLOW!!!!
	{
		fSpeedY += fK * -(fSpeedY);
		fAngle += fSpeedY;
		if (fabsf(fSpeedY)<0.001f) break;
		fTime += SHIP_TEST_Y_TIME;
	}
	if (pfTime) *pfTime = fTime;
	return fabsf(fAngle);
}

float NetShip::GetBrakingDistance(float * pfTime)
{
	if (pfTime) *pfTime = 0.0f;
	if (State.vSpeed.z<0.01f) return 0.0f;
	float fDist = 0.0f, fTime = SHIP_TEST_Z_TIME;
	float fSpeedZ = State.vSpeed.z;
	float fK = SHIP_TEST_Z_TIME * State.fMassInertia * State.vInertiaBrake.z;

	while (true)		// not funny!!! IT'S SLOW!!!!
	{
		fSpeedZ += fK * -(fSpeedZ);
		fDist += KNOTS2METERS(fSpeedZ);
		if (fSpeedZ<0.001f) break;
		fTime += SHIP_TEST_Z_TIME;
	}
	if (pfTime) *pfTime = fTime;
	return fDist;
}

// calculate ship immersion
float NetShip::GetImmersion()		
{
	ATTRIBUTES * pAImmersion = AP()->FindAClass(AP(), "Ship.Immersion");
	return (pAImmersion) ? pAImmersion->GetAttributeAsFloat() : 0.0f;
}

void NetShip::CheckShip2Strand(float fDeltaTime)
{
	if (!pIsland) return;

	MODEL * pModel = GetModel(); Assert(pModel);
	bool bNewShip2Strand = false;
	
	for (long i=0;i<MAX_KEEL_POINTS;i++)
	{
		float fRes;
		CVECTOR vTmp = pModel->mtx * vKeelContour[i];
		if (!pIsland->GetDepth(vTmp.x, vTmp.z, &fRes)) continue;
		if (fRes < -10.0f && (fRes + 10.0f) >= vTmp.y)
		{
			bNewShip2Strand = true;
			break;
		}
	}

	if (bShip2Strand != bNewShip2Strand)
	{
		bShip2Strand = bNewShip2Strand;
		//api->Event(SHIP_TO_STRAND, "ll", GetIndex(GetACharacter()), bShip2Strand);
	}
}

void NetShip::SetDead()
{
	if (!isDead())
	{
		State.vAng = vAng;

		vOldAng = State.vAng;
		vOldPos = State.vPos;

		ATTRIBUTES * pASinkSpeed = AP()->FindAClass(AP(), "ship.sink.speed"); Assert(pASinkSpeed);

		vDeadDir.x = pASinkSpeed->GetAttributeAsFloat("x");
		vDeadDir.y = pASinkSpeed->GetAttributeAsFloat("y");
		vDeadDir.z = pASinkSpeed->GetAttributeAsFloat("z");

		vCurDeadDir = 0.0f;

		bDead = true;
		api->LayerDel("sea_reflection", GetID());
	}
}

void NetShip::Execute(DWORD __DeltaTime)
{
	ATTRIBUTES * pASpeed = AP()->FindAClass(AP(), "ship.speed"); 
	if (!pASpeed) { pASpeed = AP()->CreateSubAClass(AP(), "ship.speed"); Assert(pASpeed); }

	while (aMastFalls.Size())
	{
		MastFall(&pMasts[aMastFalls[0]]);
		aMastFalls.ExtractNoShift(0);
	}

	ATTRIBUTES * pARocking = GetAShip()->GetAttributeClass("Rocking");
	if (pARocking)
	{
		fRockingY = pARocking->GetAttributeAsFloat("y", 1.0f);
		fRockingAZ = pARocking->GetAttributeAsFloat("az", 1.0f);
		fRockingFactor = pARocking->GetAttributeAsFloat("factor", 0.025f);
	}

	/*ATTRIBUTES * pAPerks = GetACharacter()->FindAClass(GetACharacter(), "TmpPerks");

	// check fast perk turn
	if (pAPerks && !bPerkTurnActive)
	{
		bPerkTurnActive = pAPerks->GetAttributeAsDword("turn") == 1;
		if (bPerkTurnActive)
		{
			fInitialPerkAngle = (pAPerks->FindAClass(pAPerks, "turn"))->GetAttributeAsFloat("Angle");
			fResultPerkAngle = State.vAng.y + fInitialPerkAngle;
		}
	}

	// check for end of fast turn perk 
	if (bPerkTurnActive)
	{
		CVECTOR vCurDir = CVECTOR(sinf(State.vAng.y), 0.0f, cosf(State.vAng.y));
		CVECTOR vResDir = CVECTOR(sinf(fResultPerkAngle), 0.0f, cosf(fResultPerkAngle));
		if ((vResDir | vCurDir) >= 0.9f) 
		{
			bPerkTurnActive = false;
			pAPerks->SetAttributeUseDword("turn", -1);
		}
	}*/

	dword DeltaTime;

	if (IsServer())
		DeltaTime = __DeltaTime;
	else
	{
		VDATA * pVDeltaTime = (VDATA*)api->GetScriptVariable("iClientDeltaTime"); Assert(pVDeltaTime);
		DeltaTime = dword(pVDeltaTime->GetLong());
	}

	float fDeltaTime = float(DeltaTime) * 0.001f;

	if (!bMounted) return;

	/*if (dtUpdateParameters.Update(fDeltaTime))
	{
		if (!pAShipStopped || pAShipStopped->GetAttributeAsDword() == 0)
		{
			api->Event(SHIP_UPDATE_PARAMETERS, "lf", GetIndex(GetACharacter()), fSailState);
		}
	}*/

	// check impulse
	ATTRIBUTES * pAImpulse = AP()->FindAClass(AP(), "ship.Impulse");
	if (pAImpulse && !isDead())
	{
		CVECTOR vRotate = 0.0f, vSpeed = 0.0f;
		ATTRIBUTES * pARotate = pAImpulse->FindAClass(pAImpulse, "Rotate");
		ATTRIBUTES * pASpeed = pAImpulse->FindAClass(pAImpulse, "Speed");
		if (pARotate)
		{
			vRotate.x = pARotate->GetAttributeAsFloat("x", 0.0f);
			vRotate.y = pARotate->GetAttributeAsFloat("y", 0.0f);
			vRotate.z = pARotate->GetAttributeAsFloat("z", 0.0f);
		}
		if (pASpeed)
		{
			vSpeed.x = pARotate->GetAttributeAsFloat("x", 0.0f);
			vSpeed.y = pARotate->GetAttributeAsFloat("y", 0.0f);
			vSpeed.z = pARotate->GetAttributeAsFloat("z", 0.0f);
		}
		STRENGTH st;
		st.bInertia = false;
		st.bUse = true;
		st.vRotate = vRotate * fDeltaTime;
		st.vSpeed = vSpeed * fDeltaTime;
		AddStrength(&st);
	}

	State.fShipImmersion = GetImmersion();		// 

	if (IsServer())
	{
		ATTRIBUTES * pAS = AP()->FindAClass(AP(), "Ship"); Assert(pAS);

		SetSpeed(pAS->GetAttributeAsFloat("SailState"));
	    api->Send_Message(sail_id, "lipf", MSG_SAIL_GET_SPEED, GetID(), &Strength[STRENGTH_MAIN].vSpeed.z, GetSpeed());
		
		Strength[STRENGTH_MAIN].vRotate.y = pAS->GetAttributeAsFloat("TurnState");
		//Strength[STRENGTH_MAIN].vSpeed.z = GetSpeed();//pAS->GetAttributeAsFloat("SailState");
	}
	else
	{
		ATTRIBUTES * pAS = AP()->FindAClass(AP(), "Ship"); Assert(pAS);
		SetSpeed(pAS->GetAttributeAsFloat("SailState"));
		api->Send_Message(sail_id, "lipf", MSG_SAIL_GET_SPEED, GetID(), &Strength[STRENGTH_MAIN].vSpeed.z, GetSpeed());

		//Strength[STRENGTH_MAIN].vSpeed.z = 2.0f;
		State.vSpeed.x = pASpeed->GetAttributeAsFloat("x");
		State.vRotate.y = pASpeed->GetAttributeAsFloat("y");
		State.vSpeed.z = pASpeed->GetAttributeAsFloat("z");

		//Strength[STRENGTH_MAIN].vRotate.y = pASpeed->GetAttributeAsFloat("y");
		//Strength[STRENGTH_MAIN].vSpeed.z = pASpeed->GetAttributeAsFloat("z");
	}

	if (isDead()) Strength[STRENGTH_MAIN].vSpeed = 0.0f;

	vPos = State.vPos;
	vAng = State.vAng;

	Move(DeltaTime, false);

	if (IsClient())
	{
		ATTRIBUTES * pASPos = AP()->FindAClass(AP(), "Ship.NewPos"); 
		if (pASPos)
		{
			float x = pASPos->GetAttributeAsFloat("x");
			float y = pASPos->GetAttributeAsFloat("y");
			float z = pASPos->GetAttributeAsFloat("z");

			//State.vPos.x += (x - State.vPos.x) * fDeltaTime;
			//State.vAng.y += (y - State.vAng.y) * fDeltaTime;
			//State.vPos.z += (z - State.vPos.z) * fDeltaTime;

			State.vPos.x = x;
			State.vAng.y = y;
			State.vPos.z = z;

			AP()->DeleteAttributeClassX(pASPos);
		}
	}

	if (!isDead()) 
	{
		float fRotate = State.vRotate.y;
		if (fRotate > 0.25f) fRotate = 0.25f;
		if (fRotate < -0.25f) fRotate = -0.25f;
		//vAng.z += fRotate;
	}

	pASpeed->SetAttributeUseFloat("x", State.vSpeed.x);
	pASpeed->SetAttributeUseFloat("y", State.vRotate.y);
	pASpeed->SetAttributeUseFloat("z", State.vSpeed.z);

	if (isDead())
	{
		float fScale = Bring2Range(0.0f, 1.0f, -15.0f, 0.0f, vPos.y);

		State.vAng = vOldAng;
		State.vPos = vOldPos;

		State.vAng = ShipRocking(fDeltaTime);

		vOldAng = State.vAng;
		vOldPos = State.vPos;

		vCurDeadDir.x += vDeadDir.x * fDeltaTime;
		vCurDeadDir.y += vDeadDir.y * fDeltaTime;
		vCurDeadDir.z += vDeadDir.z * fDeltaTime;

		State.vPos.y -= vCurDeadDir.y;
		//State.vAng.x += vCurDeadDir.x;	

		vAng.z = State.vAng.z;
		vAng.x = State.vAng.x;
		vPos.y = State.vPos.y;

		if (isVisible() && vPos.y < -50.0f)
		{
			bVisible = false;

			// stop all fireplaces
			for (dword i=0; i<aFirePlaces.Size(); i++) aFirePlaces[i].Stop();

			// del vant,flags,sail and ropes
			api->Send_Message(sail_id, "li", MSG_SAIL_DEL_GROUP, GetID());
			api->Send_Message(rope_id, "li", MSG_ROPE_DEL_GROUP, GetModelEID());
			api->Send_Message(flag_id, "li", MSG_FLAG_DEL_GROUP, GetModelEID());
			api->Send_Message(vant_id, "li", MSG_VANT_DEL_GROUP, GetModelEID());

			api->LayerDel("net_realize", GetID());
			api->LayerDel("ship_cannon_trace", GetID());
			
			//api->Event("Net_ShipDelete", "ai", AP(), GetID());
		}
	}
	else
	{
	}

	// set attributes for script
	ATTRIBUTES * pAPos = AP()->FindAClass(AP(), "ship.pos");
	ATTRIBUTES * pAAng = AP()->FindAClass(AP(), "ship.ang");
	ATTRIBUTES * pABox = AP()->FindAClass(AP(), "ship.boxsize");
	if (!pAPos) { pAPos = AP()->CreateSubAClass(AP(), "ship.pos"); Assert(pAPos); }
	if (!pAAng) { pAAng = AP()->CreateSubAClass(AP(), "ship.ang"); Assert(pAAng); }
	if (!pABox) { pABox = AP()->CreateSubAClass(AP(), "ship.boxsize"); Assert(pABox); }
	pAPos->SetAttributeUseFloat("x", State.vPos.x);	pAPos->SetAttributeUseFloat("y", State.vPos.y);	pAPos->SetAttributeUseFloat("z", State.vPos.z);
	pAAng->SetAttributeUseFloat("x", State.vAng.x);	pAAng->SetAttributeUseFloat("y", State.vAng.y);	pAAng->SetAttributeUseFloat("z", State.vAng.z);
	pABox->SetAttributeUseFloat("x", State.vRealBoxSize.x);	pABox->SetAttributeUseFloat("y", State.vRealBoxSize.y);	pABox->SetAttributeUseFloat("z", State.vRealBoxSize.z);

	// check strand
	//if (!bDead && bKeelContour) CheckShip2Strand(fDeltaTime);

	CMatrix m = CMatrix(vAng.x, vAng.y, vAng.z, vPos.x, vPos.y - State.fShipImmersion - SP.fWaterLine, vPos.z);
	SetMatrix(m);

	api->Send_Message(sea_id, "lifcf", MSG_SHIP_SEA_INFO, GetID(), KNOTS2METERS(State.vSpeed.z), vPos, State.vAng.y);
	
	CMatrix matrix = UpdateModelMatrix();

	// activate mast tracer
	/*if (dtMastTrace.Update(fDeltaTime))
	{
		VIDWALKER * pVWShip = api->LayerGetWalker("mast_ship_trace");
		VIDWALKER * pVWIsland = api->LayerGetWalker("mast_island_trace");
		if (pVWShip || pVWIsland) 
		{
			for (long i=0; i<iNumMasts; i++) if (!pMasts[i].bBroken)
			{
				CVECTOR		v1, v2;
				VDATA		* pV;
				float		fShipRes = 2.0f, fIslRes = 2.0f;

				mast_t * pM = &pMasts[i];
				v1 = matrix * pM->vSrc;
				v2 = matrix * pM->vDst;
				
				if (pVWShip)	// from ship damage
				{
					fShipRes = pCollide->Trace(*pVWShip, v1, v2, &GetID(), 1);
					if (fShipRes <= 1.0f) 
					{
						ATTRIBUTES * pACollideCharacter = GetACharacter();
						SHIP * pShip = (SHIP*)api->GetEntityPointer(&pCollide->GetObjectID());
						if (pShip) pACollideCharacter = pShip->GetACharacter();
						pV = api->Event(SHIP_MAST_DAMAGE, "llffffaa", SHIP_MAST_TOUCH_SHIP, pM->iMastNum, v1.x, v1.y, v1.z, pM->fDamage, GetACharacter(), pACollideCharacter);
						pM->fDamage = Clamp(pV->GetFloat());
					}
				}
				if (pVWIsland)	// from island damage
				{
					fIslRes	= pCollide->Trace(*pVWIsland, v1, v2, &GetModelEID(), 1);
					if (fIslRes <= 1.0f)	
					{
						pV = api->Event(SHIP_MAST_DAMAGE, "llffffa", SHIP_MAST_TOUCH_ISLAND, pM->iMastNum, v1.x, v1.y, v1.z, pM->fDamage, GetACharacter());
						pM->fDamage = Clamp(pV->GetFloat());
					}
				}

				MastFall(pM);
			}
		}
		DELETE(pVWShip);
		DELETE(pVWIsland);
	}*/

	// execute fire places
	for (dword i=0; i<aFirePlaces.Size(); i++) aFirePlaces[i].Execute(fDeltaTime);

	// execute cannons controller
	if (pCannonController) pCannonController->Execute(fDeltaTime);
	// execute cannons controller
	if (pCannonController) pCannonController->Realize(fDeltaTime);

	// water sound: set position and volume
	ATTRIBUTES * pASounds = AttributesPointer->FindAClass(AttributesPointer, "Ship.Sounds");

	if (pASounds)
	{
		ATTRIBUTES * pAWaterID = pASounds->GetAttributeClass("WaterID");
		ATTRIBUTES * pATurnID = pASounds->GetAttributeClass("TurnID");
		ATTRIBUTES * pASailsID = pASounds->GetAttributeClass("SailsID");

		CMatrix mRotate; m.Get3X3(mRotate);
		CVECTOR vBoxSize = State.vBoxSize / 2.0f;

		float fWaterSpeed = pASounds->GetAttributeAsFloat("WaterSpeed", 40.0f);
		long iWaterSound = long(pASounds->GetAttributeAsDword("WaterID", -1));
		if (iWaterSound > 0)
		{
			float x = pAWaterID->GetAttributeAsFloat("x", 0.0f);
			float y = pAWaterID->GetAttributeAsFloat("y", 0.0f);
			float z = pAWaterID->GetAttributeAsFloat("z", 0.0f);
			CVECTOR vPos = State.vPos + (mRotate * CVECTOR(vBoxSize.x * x, vBoxSize.y * y, vBoxSize.z * z));
			float fWaterSoundVolume = Min(KNOTS2METERS(fabsf(State.vSpeed.z)), fWaterSpeed) / fWaterSpeed;
			api->Send_Message(FirePlace::eidSound, "llf", MSG_SOUND_SET_VOLUME, iWaterSound, fWaterSoundVolume);
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iWaterSound, SOUND_PARAM_POSITION, vPos.x, vPos.y, vPos.z);
		}

		float fTurnSpeed = pASounds->GetAttributeAsFloat("TurnSpeed", 20.0f);
		long iTurnSound = long(pASounds->GetAttributeAsDword("TurnID", -1));
		if (iTurnSound > 0)
		{
			float x = pATurnID->GetAttributeAsFloat("x", 0.0f);
			float y = pATurnID->GetAttributeAsFloat("y", 0.0f);
			float z = pATurnID->GetAttributeAsFloat("z", 0.0f);
			CVECTOR vPos = State.vPos + (mRotate * CVECTOR(vBoxSize.x * x, vBoxSize.y * y, vBoxSize.z * z));
			float fTurnSoundVolume = Min(fabsf(State.vRotate.y), fTurnSpeed) / fTurnSpeed;
			api->Send_Message(FirePlace::eidSound, "llf", MSG_SOUND_SET_VOLUME, iTurnSound, fTurnSoundVolume);
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iTurnSound, SOUND_PARAM_POSITION, vPos.x, vPos.y, vPos.z);
		}

		long iSailSound = long(pASounds->GetAttributeAsDword("SailsID", -1));
		if (iSailSound > 0)
		{
			float x = pASailsID->GetAttributeAsFloat("x", 0.0f);
			float y = pASailsID->GetAttributeAsFloat("y", 0.0f);
			float z = pASailsID->GetAttributeAsFloat("z", 0.0f);
			CVECTOR vPos = State.vPos + (mRotate * CVECTOR(vBoxSize.x * x, vBoxSize.y * y, vBoxSize.z * z));
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iSailSound, SOUND_PARAM_POSITION, vPos.x, vPos.y, vPos.z);
		}
	}
}

void NetShip::UpdateCurves(long iMsgServerTime, const CVECTOR & vPos, const CVECTOR & vSpeed)
{
	//if (iMsgServerTime / 1000 <= iLastMsgServerTime / 1000) return;

	int iTime = iMsgServerTime / 1000;
	int iShift = iTime - curvePosition.iTime;
	//api->Trace("iShift = %d", iShift);
	if (iShift == 5)
	{
		for (long i=0; i<4; i++)
		{
			curvePosition.vTime[i] = curvePosition.vTime[i + 1];
			curveSpeed.vTime[i] = curveSpeed.vTime[i + 1];
		}
	}
	/*else if (iShift > 5)
	{
		CVECTOR vTempPos[5], vTempSpeed[5];
		int iNewTime = iTime - 4;

		for (long i=0; i<4; i++)
		{
			if (iNewTime + i < curvePosition.iTime + 5)
			{
				vTempPos[i] = curvePosition.vTime[iNewTime + i - curvePosition.iTime];
				vTempSpeed[i] = curveSpeed.vTime[iNewTime + i - curvePosition.iTime];
			}
			else
			{
				float fPos = float(iNewTime + i - curvePosition.iTime) * 0.25f;
				vTempPos[i] = curvePosition.curve.GetPoint(fPos);
				vTempSpeed[i] = curveSpeed.curve.GetPoint(fPos);
			}
		}

		for (long i=0; i<4; i++)
		{
			curvePosition.vTime[i] = vTempPos[i];
			curveSpeed.vTime[i] = vTempSpeed[i];
		}
	}
	else
	{
		// error
	}*/

	curvePosition.vTime[4] = vPos;		curveSpeed.vTime[4] = vSpeed;
	curvePosition.iTime = iTime - 4;	curveSpeed.iTime = iTime - 4;

	for (long i=0; i<5; i++)
	{
		curvePosition.curve.SetLine(curvePosition.vTime[1], curvePosition.vTime[1], curvePosition.vTime[2], curvePosition.vTime[3], curvePosition.vTime[3]);
		curveSpeed.curve.SetLine(curveSpeed.vTime[0], curveSpeed.vTime[1], curveSpeed.vTime[2], curveSpeed.vTime[3], curveSpeed.vTime[4]);
	}

	iLastMsgServerTime = iMsgServerTime;
}

void NetShip::MastFall(mast_t * pM)
{
	if (pM && pM->pNode && pM->fDamage >= 1.0f) 
	{
		ENTITY_ID ent;
		api->CreateEntity(&ent, "NetMast"); ENTITY * pE = (ENTITY *)ent.pointer; if (pE) pE->SetServer(IsServer());
		api->Send_Message(ent, "lpii", MSG_MAST_SETGEOMETRY, pM->pNode, GetID(), GetModelEID());
		api->LayerAdd("net_execute", ent, 100);
		if (IsClient()) api->LayerAdd("net_realize", ent, 100);

		if (pShipsLights) pShipsLights->KillMast(this, pM->pNode, false);

		char str[256];
		sprintf(str, "%s", pM->pNode->GetName());
		ATTRIBUTES * pAMasts = AP()->FindAClass(AP(), "Ship.Masts");
		if (!pAMasts)
			pAMasts = AP()->CreateSubAClass(AP(), "Ship.Masts");
		Assert(pAMasts);
		pAMasts->SetAttributeUseFloat(str, 1.0f);
		pM->bBroken = true;
	}
}

CMatrix NetShip::UpdateModelMatrix()
{
	MODEL * pModel = GetModel(); Assert(pModel);
	pModel->Update();
	return pModel->mtx;
}

//##################################################################

void NetShip::SetLightAndFog(bool bSetLight)
{
	if (isDead() && isVisible())
	{
		bSetLightAndFog = true;

		// ambient
		float fScale = Bring2Range(0.0f, 1.0f, -35.0f, 0.0f, State.vPos.y);
		Render().GetRenderState(D3DRS_AMBIENT, &dwSaveAmbient);
		CVECTOR vAmbient = fScale * COLOR2VECTOR(dwSaveAmbient);
		Render().SetRenderState(D3DRS_AMBIENT, ARGB((dwSaveAmbient >> 24L), vAmbient.x, vAmbient.y, vAmbient.z));

		// light
		D3DLIGHT8 newLight;
		Render().GetLight(0, &saveLight);
		newLight = saveLight;
		newLight.Diffuse.r *= fScale;	newLight.Diffuse.g *= fScale;	newLight.Diffuse.b *= fScale;	newLight.Diffuse.a *= fScale; 
		newLight.Specular.r *= fScale;	newLight.Specular.g *= fScale;	newLight.Specular.b *= fScale;	newLight.Specular.a *= fScale; 
		newLight.Ambient.r *= fScale;	newLight.Ambient.g *= fScale;	newLight.Ambient.b *= fScale;	newLight.Ambient.a *= fScale; 
		Render().SetLight(0, &newLight);

		// fog
		float fFogScale = Bring2Range(0.0f, 1.0f, -30.0f, 0.0f, State.vPos.y);
		Render().GetRenderState(D3DRS_FOGCOLOR, &dwSaveFogColor);
		CVECTOR vFogColor = fFogScale * COLOR2VECTOR(dwSaveFogColor);
		Render().SetRenderState(D3DRS_FOGCOLOR, ARGB((dwSaveFogColor >> 24L), vFogColor.x, vFogColor.y, vFogColor.z));
	}
}

void NetShip::RestoreLightAndFog()
{
	if (bSetLightAndFog)
	{
		bSetLightAndFog = false;
		Render().SetRenderState(D3DRS_AMBIENT, dwSaveAmbient);
		Render().SetRenderState(D3DRS_FOGCOLOR, dwSaveFogColor);
		Render().SetLight(0, &saveLight);
	}
}

void NetShip::Realize(DWORD dtime)
{
	if (!bMounted) return;

	MODEL * pM = GetModel(); Assert(pM);

	bSetLightAndFog = false;

	SetLightAndFog(true);
	SetLights();

	Render().SetRenderState(D3DRS_LIGHTING, true);
	pM->Realize(dtime);
	Render().SetRenderState(D3DRS_LIGHTING, false);

	UnSetLights();
	RestoreLightAndFog();

	/*if (dtime)
	{
		VDATA * pVClientID = (VDATA *)api->GetScriptVariable("wClientID"); Assert(pVClientID);

		if (IsClient() && GetNetID() == pVClientID->GetLong())
		{
			float fHP = AttributesPointer->FindAClass(AttributesPointer, "Ship.HP")->GetAttributeAsFloat();
			float fSP = AttributesPointer->FindAClass(AttributesPointer, "Ship.SP")->GetAttributeAsFloat();
			float fCrew = AttributesPointer->FindAClass(AttributesPointer, "Ship.Crew.Quantity")->GetAttributeAsFloat();

			Render().ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(100), "HP: %.1f, SP: %.1f, Crew: %.1f", fHP, fSP, fCrew);
			Render().ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(120), "sZ: %.2f, rY: %.2f", State.vSpeed.z, State.vRotate.y);

			VDATA * pVSTime = (VDATA *)api->GetScriptVariable("iClientServerTime"); Assert(pVSTime);
			VDATA * pVCTime = (VDATA *)api->GetScriptVariable("iClientTime"); Assert(pVCTime);
			Render().ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_LEFT, 0, 0.85f, 0, 0, long(0), long(60), "STime: %d, CTime: %d", pVSTime->GetLong(), pVCTime->GetLong());
		}
	}*/

	/*for (long i=0; i<15; i++)
	{
		CVECTOR v1 = curvePosition.curve.GetPoint(float(i) * 0.25f);
		CVECTOR v2 = curvePosition.curve.GetPoint(float(i + 1) * 0.25f);
		v1.y = v2.y = 0.0f;

		if (i<5) Render().DrawSphere(v1, 0.3f, 0xFF00FF00);

		Render().DrawVector(v1, v2, 0xFFFFFFFF);
	}*/

	/*CMatrix m1 = *GetMatrix();
	VDATA * pVCTime = (VDATA *)api->GetScriptVariable("iClientTime"); Assert(pVCTime);
	float fPos = (float(pVCTime->GetLong() - curvePosition.iTime * 1000) / 1000.0f) * 0.25f;
	CVECTOR v1 = curvePosition.curve.GetPoint(fPos);
	CMatrix m2 = m1;
	v1.y = State.vPos.y;
	m2.SetPosition(v1);
	SetMatrix(m2);
	Render().SetRenderState(D3DRS_LIGHTING, true);
	pM->Realize(dtime);
	Render().SetRenderState(D3DRS_LIGHTING, false);
	SetMatrix(m1);*/
}

void NetShip::SetLights()
{
	if (pShipsLights) pShipsLights->SetLights(this);
}

void NetShip::UnSetLights()
{
	if (pShipsLights) pShipsLights->UnSetLights(this);
}

void NetShip::Fire(const CVECTOR & vPos)
{
	if (pShipsLights) pShipsLights->AddDynamicLights(this, vPos);
}

float NetShip::GetMaxSpeedZ()
{
	ATTRIBUTES * pAMaxSpeedZ = AP()->FindAClass(AP(), "Ship.MaxSpeedZ");
	return (pAMaxSpeedZ) ? pAMaxSpeedZ->GetAttributeAsFloat() : 0.0f;
}

float NetShip::GetMaxSpeedY()
{
	ATTRIBUTES * pAMaxSpeedY = AP()->FindAClass(AP(), "Ship.MaxSpeedY");
	return (pAMaxSpeedY) ? pAMaxSpeedY->GetAttributeAsFloat() : 0.0f;
}

bool NetShip::DelStrength(long iIdx)
{
	if (iIdx<RESERVED_STRENGTH || iIdx>=MAX_STRENGTH) return false;
	Strength[iIdx].bUse = false;
	return true;
}

long NetShip::AddStrength(STRENGTH *strength)
{
	long i;
	for (i=RESERVED_STRENGTH;i<MAX_STRENGTH;i++) if (!Strength[i].bUse)
	{
		Strength[i] = *strength;
		Strength[i].bUse = true;
		return i;
	}
	return -1;
}

ATTRIBUTES * NetShip::AP()
{
	return AttributesPointer;
}

dword _cdecl NetShip::ProcessMessage(MESSAGE & message)
{
	ENTITY_ID	entity_id;
	long		code = message.Long();
	char		str[256], str1[256], str2[256];

	switch (code)
	{
		case MSG_SEA_REFLECTION_DRAW:
			if (IsClient()) Realize(0);
		break;
		case MSG_SHIP_CREATE:
			pAShip = message.AttributePointer();
			Mount();
			LoadPositionFromAttributes();
		break;
		case MSG_SHIP_ACTIVATE_FIRE_PLACE:
		{
			dword dwFPIndex = dword(message.Long());
			message.String(sizeof(str), str);
			message.String(sizeof(str1), str1);
			message.String(sizeof(str2), str2);
			float fRunTime = message.Float();
			Assert(dwFPIndex != INVALID_ARRAY_INDEX && dwFPIndex < aFirePlaces.Size());
			aFirePlaces[dwFPIndex].Run(str, str1, str2, fRunTime);
		}
		break;
		case MSG_SHIP_GET_NUM_FIRE_PLACES:
		{
			VDATA * pVData = message.ScriptVariablePointer();
			pVData->Set((long)aFirePlaces.Size());
		}
		break;
		case MSG_SHIP_ADD_MOVE_IMPULSE:
		{
			STRENGTH st;

			st.bInertia = message.Long() != 0;

			CVECTOR vSpeed;

			vSpeed.x = message.Float();
			vSpeed.y = message.Float();
			vSpeed.z = message.Float();

			RotateAroundY(vSpeed.x, vSpeed.z, cosf(-State.vAng.y), sinf(-State.vAng.y));

			st.vSpeed = vSpeed;

			st.vRotate.x = message.Float();
			st.vRotate.y = message.Float();
			st.vRotate.z = message.Float();
			
			AddStrength(&st);
		}
		break;
		case MSG_MAST_DELGEOMETRY:
		{
			NODE * pNode = (NODE *)message.Pointer();
			pShipsLights->KillMast(this, pNode, true);
		}
		case MSG_SHIP_CURVES:
		{
			int iMsgServerTime;
			float x, y, z, sx, sy, sz;
			iMsgServerTime = message.Long();
			x = message.Float();
			y = message.Float();
			z = message.Float();
			sx = message.Float();
			sy = message.Float();
			sz = message.Float();
			UpdateCurves(iMsgServerTime, CVECTOR(x, y, z), CVECTOR(sx, sy, sz));
		}
		break;
	}
	return 0;
}

bool NetShip::IsCanPlace(CVECTOR & vNewPos)
{
	CVECTOR vBoxSize = GetBoxSize();
	vBoxSize.x += 20.0f;
	vBoxSize.z += 20.0f;

	for (dword i=0; i<aShips.Size(); i++) if (aShips[i] && this != aShips[i])
	{
		float fMinDist = (vBoxSize.z + aShips[i]->GetBoxSize().z) / 2.0f;
		CVECTOR vOtherPos = aShips[i]->GetPos();
		if ((vOtherPos - vNewPos).GetLength2D() < fMinDist) return false;
	}

	if (pIsland && pIsland->Check2DBoxDepth(vNewPos, vBoxSize, 0.0f, -14.0f)) return false;

	return true;
}

void NetShip::LoadPositionFromAttributes()
{
	ATTRIBUTES * pAPos = AP()->FindAClass(AP(), "ship.pos");
	ATTRIBUTES * pAAng = AP()->FindAClass(AP(), "ship.ang");
	Assert(pAPos && pAAng);
	
	State.vPos.x = pAPos->GetAttributeAsFloat("x");	
	State.vPos.z = pAPos->GetAttributeAsFloat("z");
	State.vAng.y = pAAng->GetAttributeAsFloat("y");
	State.vPos.y = pSea->WaveXZ(State.vPos.x, State.vPos.z);

	if (IsServer())
	{
		ENTITY_ID eid;
		aShips.DelAll();
		if (NetFindClass(true, &eid, "NetShip")) do
		{
			aShips.Add((NetShip*)api->GetEntityPointer(&eid));
		} while (NetFindClassNext(true, &eid));

		// check position
		CVECTOR vOurPos = GetPos(), vNewPos = vOurPos, vBoxSize = GetBoxSize();
		float fRadius = vBoxSize.z, fAng = 0.0f;
		while (!IsCanPlace(vNewPos))
		{
			vNewPos = vOurPos + fRadius * CVECTOR(sinf(fAng), 0.0f, cosf(fAng));
			fAng += PI / 8.0f;
			if (fAng >= PIm2)
			{
				fAng = 0.0f;
				fRadius += vBoxSize.z;
			}
		}
		State.vPos.x = vNewPos.x;
		State.vPos.z = vNewPos.z;

		pAPos->SetAttributeUseFloat("x", State.vPos.x);
		pAPos->SetAttributeUseFloat("z", State.vPos.z);
	}

	for (long i=0; i<5; i++) { curvePosition.vTime[i] = State.vPos; }
	curvePosition.curve.SetLine(State.vPos, State.vPos, State.vPos, State.vPos, State.vPos);
}

bool NetShip::Mount()
{
	SetNetID(word(AP()->GetAttributeAsDword("index", DST_INVALID)));

	// setup textures path, etc
	api->Event("Net_ShipStartLoad", "a", AP());

	sShipName = AP()->GetAttribute("Model");

	ATTRIBUTES * pASpeedZ = AP()->FindAClass(AP(), "Ship.Speed.z");
	float fNewSailState = (pASpeedZ) ? pASpeedZ->GetAttributeAsFloat() : 0.0f;
	SetSpeed(fNewSailState);

	State.vPos = 0.0f;

	char temp_str[256];
	sprintf(temp_str, "ships\\%s\\%s", (const char *)sShipName, (const char *)sShipName);

	api->CreateEntity(&model_id, "MODELR");
	api->Send_Message(GetModelEID(), "ls", MSG_MODEL_LOAD_GEO, temp_str);
	api->LayerAdd("hull_trace", GetModelEID(), 10);
	api->LayerAdd("sun_trace", GetModelEID(), 10);
	api->LayerAdd("mast_ship_trace", GetID(), 10);

	// sails
	NetFindClass(IsServer(), &sail_id, "NetSail"); 
	api->Send_Message(sail_id, "liil", MSG_SAIL_INIT,GetID(), GetModelEID(), GetSpeed() ? 1 : 0); 
    
	// ropes
	NetFindClass(IsServer(), &rope_id, "NetRope"); 
	api->Send_Message(rope_id, "lii", MSG_ROPE_INIT, GetID(), GetModelEID()); 

	// flags
	NetFindClass(IsServer(), &flag_id, "NetFlag"); 
	api->Send_Message(flag_id, "lili", MSG_FLAG_INIT, GetModelEID(), 0, GetID()); 

	// vants
	NetFindClass(IsServer(), &vant_id, "NetVant"); 
	api->Send_Message(vant_id, "lii", MSG_VANT_INIT, GetID(), GetModelEID()); 
	
	// blots
	/*api->CreateEntity(&blots_id, "blots");
	api->Send_Message(blots_id, "lia", MSG_BLOTS_SETMODEL, GetModelEID(), GetACharacter()); 
	api->LayerAdd("NetRealize", blots_id, iShipPriority+1);
	api->LayerAdd("NetExecute", blots_id, iShipPriority+1);*/

	LoadShipParameters();

	ENTITY_ID temp_id = GetID();
	//api->Send_Message(touch_id, "li", MSG_SHIP_CREATE, temp_id);
	//api->Send_Message(sea_id, "lic", MSG_SHIP_CREATE, temp_id, State.vPos);

	GEOS::INFO ginfo;
	MODEL * pModel = GetModel(); Assert(pModel);
	NODE * pNode = pModel->GetNode(0); Assert(pNode);
	pNode->geo->GetInfo(ginfo);

	CalcRealBoxSize();

	State.vBoxSize.x = ginfo.boxsize.x;
	State.vBoxSize.y = ginfo.boxsize.y;
	State.vBoxSize.z = ginfo.boxsize.z;

	State.fMassInertia = 0.10f; 
	
	ATTRIBUTES * pALights = GetACharacter()->FindAClass(GetACharacter(),"ship.lights");
	ATTRIBUTES * pAFlares = GetACharacter()->FindAClass(GetACharacter(),"ship.flares");

	bool bLights = (pALights) ? pALights->GetAttributeAsDword() != 0 : false;
	bool bFlares = (pAFlares) ? pAFlares->GetAttributeAsDword() != 0 : false;

	NODE * pFonarDay = pModel->FindNode("fd");
	NODE * pFonarNight = pModel->FindNode("fn");

	if (bLights)
	{
		if (pFonarDay) pFonarDay->flags &= ~NODE::VISIBLE_TREE;
		if (pFonarNight) pFonarNight->flags |= NODE::VISIBLE_TREE;
	}
	else
	{
		if (pFonarNight) pFonarNight->flags &= ~NODE::VISIBLE_TREE;
		if (pFonarDay) pFonarDay->flags |= NODE::VISIBLE_TREE;
	}

	// Add lights and flares
	ENTITY_ID eidTmp;
	if (NetFindClass(IsServer(), &eidTmp, "NetShipLights"))
	{
		pShipsLights = (IShipLights*)api->GetEntityPointer(&eidTmp); 
	}

	if (pShipsLights) pShipsLights->AddLights(this, GetModel(), bLights, bFlares);
	if (pShipsLights) pShipsLights->Execute(0);

	// add fireplaces
	ScanShipForFirePlaces();
	// add masts
	BuildMasts();

	//VDATA * pVClientID = (VDATA *)api->GetScriptVariable("wClientID"); Assert(pVClientID);
	if (IsServer()/* || NetID() == pVClientID->GetLong()*/)
	{
		pCannonController = NEW NetShipCannonController(this);
		pCannonController->SetServer(IsServer());
		pCannonController->Init(pAShip);
	}

	// add to ship tracks
	if (NetFindClass(IsServer(), &eidTmp, "NetShipTracks"))
	{
		NetShipTracks * pST = (NetShipTracks*)api->GetEntityPointer(&eidTmp); 
		if (pST) pST->AddShip(this);
	}

	api->Event("Net_ShipEndLoad", "a", AP());

	bMounted = true;
	return true;
}

void NetShip::CalcRealBoxSize()
{
	GEOS::INFO	ginfo;
	float		x1 = 1e+8f, x2 = -1e+8f, y1 = 1e+8f, y2 = -1e+8f, z1 = 1e+8f, z2 = -1e+8f;

	MODEL * pM = GetModel(); Assert(pM);

	for (long i=0; ; i++)
	{
		NODE * pN = pM->GetNode(i); i++;
		if (!pN) break;
		pN->geo->GetInfo(ginfo);
		CVECTOR vGlobPos = pN->glob_mtx.Pos();
		CVECTOR vBC = vGlobPos + CVECTOR(ginfo.boxcenter.x, ginfo.boxcenter.y, ginfo.boxcenter.z);
		CVECTOR vBS = CVECTOR(ginfo.boxsize.x, ginfo.boxsize.y, ginfo.boxsize.z) / 2.0f;
		if (vBC.x - vBS.x < x1) x1 = vBC.x - vBS.x;
		if (vBC.x + vBS.x > x2) x2 = vBC.x + vBS.x;
		if (vBC.y - vBS.y < y1) y1 = vBC.y - vBS.y;
		if (vBC.y + vBS.y > y2) y2 = vBC.y + vBS.y;
		if (vBC.z - vBS.z < z1) z1 = vBC.z - vBS.z;
		if (vBC.z + vBS.z > z2) z2 = vBC.z + vBS.z;
	}

	State.vRealBoxSize = CVECTOR(x2 - x1, y2 - y1, z2 - z1);
}

void NetShip::ScanShipForFirePlaces()
{
	GEOS::LABEL	label;
	GEOS::INFO	info;
	NODE		* pNode;

	MODEL * pModel = GetModel(); Assert(pModel);

	// search and add fire places
	string sFirePlace = "fireplace";
	string sFirePlaces = "fireplaces";
	dword dwIdx = 0;
	while (pNode = pModel->GetNode(dwIdx))
	{
		pNode->geo->GetInfo(info);
		for (dword i=0; i<dword(info.nlabels); i++)
		{
			pNode->geo->GetLabel(i, label);
			if (sFirePlace == label.group_name || sFirePlaces == label.group_name)
			{
				FirePlace * pFP = &aFirePlaces[aFirePlaces.Add()];
				pFP->Init(pSea, this, label);
			}
		}
		dwIdx++;
	}

	if (aFirePlaces.Size() == 0)
	{
		api->Trace("Ship %s doesn't have fire places", (const char *)sShipName);
	}

	ATTRIBUTES * pAS = AP()->FindAClass(AP(), "Ship"); Assert(pAS);
	pAS->SetAttributeUseDword("NumFirePlaces", aFirePlaces.Size());
}

bool NetShip::LoadShipParameters()
{
#define GetADword(x)	GetAShip()->GetAttributeAsDword(x)
#define GetAFloat(x)	GetAShip()->GetAttributeAsFloat(x)

	// standard values
	SP.fWaterLine = GetAFloat("WaterLine");

	State.vInertiaAccel.y = GetAFloat("InertiaAccelerationY");
	State.vInertiaBrake.y = GetAFloat("InertiaBrakingY");
	State.vInertiaAccel.x = GetAFloat("InertiaAccelerationX");
	State.vInertiaBrake.x = GetAFloat("InertiaBrakingX");
	State.vInertiaAccel.z = GetAFloat("InertiaAccelerationZ");
	State.vInertiaBrake.z = GetAFloat("InertiaBrakingZ");

	State.fWeight = GetAFloat("Weight");
	
	return true;
}

float NetShip::Trace(const CVECTOR &src, const CVECTOR &dst) 
{ 
	MODEL * pModel = GetModel(); Assert(pModel);
	return pModel->Trace(src,dst);
};

float NetShip::Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	Assert(IsServer());

	MODEL * pModel = GetModel(); Assert(pModel);
	
	if (iBallOwner == GetNetID()) return 2.0f;

	for (long i=0; i<iNumMasts; i++) if (!pMasts[i].bBroken)
	{
		mast_t * pM = &pMasts[i];
		float fRes = pM->pNode->Trace(vSrc, vDst);

		if (fRes <= 1.0f)
		{
			CVECTOR v1 = vSrc + fRes * (vDst - vSrc);
			VDATA * pV = api->Event("NetServer_OnShipMastDamage", "llffffll", NETSHIP_MAST_TOUCH_BALL, pM->iMastNum, v1.x, v1.y, v1.z, pM->fDamage, long(GetNetID()), iBallOwner);
			pM->fDamage = Clamp(pV->GetFloat());
			MastFall(pM);
		}
	}

	float fRes = pModel->Trace(vSrc, vDst);
	if (fRes <= 1.0f)
	{
		CVECTOR vDir = !(vDst - vSrc);
		CVECTOR vTemp = vSrc + fRes * (vDst - vSrc);
		// search nearest fire place
		float	fMinDistance = 1e8f;
		long	iBestIndex = -1;
		for (dword i=0; i<aFirePlaces.Size(); i++) if (!aFirePlaces[i].isActive())
		{
			float fDistance = aFirePlaces[i].GetDistance(vTemp);
			if (fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				iBestIndex = i;
			}
		}
		api->Event("NetServer_OnShipHullHit", "illffflf", GetID(), iBallOwner, long(GetNetID()), vTemp.x, vTemp.y, vTemp.z, iBestIndex, fMinDistance);
		//api->Send_Message(blots_id, "lffffff", MSG_BLOTS_HIT, vTemp.x, vTemp.y, vTemp.z, vDir.x, vDir.y, vDir.z);
	}
	return fRes; 
}

dword NetShip::AttributeChanged(ATTRIBUTES * pAttribute)
{
	CVECTOR vCamPos = 0.0f, vCamDir = 0.0f;

	ATTRIBUTES * pACamPos = AttributesPointer->FindAClass(AttributesPointer, "Camera.Pos");
	ATTRIBUTES * pACamDir = AttributesPointer->FindAClass(AttributesPointer, "Camera.Dir");

	if (pACamPos)
	{
		vCamPos.x = pACamPos->GetAttributeAsFloat("x");
		vCamPos.y = pACamPos->GetAttributeAsFloat("y");
		vCamPos.z = pACamPos->GetAttributeAsFloat("z");
	}

	if (pACamDir)
	{
		vCamDir.x = pACamDir->GetAttributeAsFloat("x");
		vCamDir.y = pACamDir->GetAttributeAsFloat("y");
		vCamDir.z = pACamDir->GetAttributeAsFloat("z");
	}

	if (*pAttribute == "IsCanFire" && pACamDir)
	{
		bool bCanFire = (pCannonController) ? pCannonController->isCanFire(vCamDir) : false;
		AttributesPointer->SetAttributeUseDword("IsCanFire", dword(bCanFire));
		dword dwCurrentTargetTeam = -1;
		if (bCanFire)
		{
			dword dwOurTeam = GetACharacter()->GetAttributeAsDword("Team");
			float fBestDistance = 1e8f;
			CVECTOR vOurPos = GetPos();
			CVECTOR vFirePos = pCannonController->GetFirePos(vCamDir);
			CVECTOR vFireDir = !(vFirePos - vOurPos);
			float fFireDist = sqrtf(~(vFirePos - vOurPos));
			vFirePos.y = 0.0f;

			ENTITY_ID eid;
			if (NetFindClass(true, &eid, "NetShip")) do
			{
				NetShip * pShip = (NetShip*)api->GetEntityPointer(&eid);
				if (!pShip || pShip->GetNetID() == GetNetID()) continue;
				CVECTOR vFakeShipPos = pShip->GetPos() + (!(pShip->GetPos() - vOurPos)) * pShip->GetBoxSize().z * 0.8f;
				float fDistance = (vFakeShipPos - vFirePos).GetLength2D();//pShip->GetDistance(vFirePos);
				if (fDistance <= 0.8f * pShip->GetBoxSize().z)
				{
					if (fDistance > fBestDistance) continue;
					fBestDistance = fDistance;

					dwCurrentTargetTeam = pShip->GetACharacter()->GetAttributeAsDword("Team", 0);
				}
			} while (NetFindClassNext(true, &eid));
			// check fort here
			NetFort * pFort = null;
			if (NetFindClass(true, &eid, "NetFort")) pFort = (NetFort*)api->GetEntityPointer(&eid);
			if (pFort) for (dword i=0; i<pFort->GetNumCannons(); i++)
			{
				NetCannon * pC = pFort->GetCannon(i);
				if (pC->isDamaged()) continue;
				float fDistance = pC->GetDistance(vOurPos);
				if (fDistance > fFireDist) continue;
				if (fDistance > fBestDistance) continue;
				float fDot = vFireDir | (!(pC->GetPos() - vOurPos));
				if (fDot < 0.0f) continue;

				CVECTOR vFire2CannonPos = pCannonController->GetFirePos(vCamDir, fDistance);
				float fRealDistance = ~(vFire2CannonPos - pC->GetPos());
				if (fRealDistance > SQR(8.0f)) continue;

				dwCurrentTargetTeam = 0;
				break; // we need only one cannon for check this
			}
		}

		AttributesPointer->SetAttributeUseDword("CurrentTargetTeam", dwCurrentTargetTeam);
		//AttributesPointer->SetAttributeUseDword("CurrentTargetRelation", dwCurrentTargetRelation);
		return 0;
	}

	if (*pAttribute == "DoCannonFireCameraOutside" && pCannonController)
	{
		ENTITY_ID			shipEID, fortEID;
		array<can_fire_t>	aShipFire(_FL_);

		dword dwOurTeam = GetACharacter()->GetAttributeAsDword("Team");
		dword dwFriendlyTeam = GetACharacter()->GetAttributeAsDword("FriendlyTeam");

		if (NetFindClass(true, &shipEID, "NetShip")) do
		{
			NetShip * pShip = (NetShip *)api->GetEntityPointer(&shipEID);
			if (pShip == this) continue;

			dword dwTeam = pShip->GetACharacter()->GetAttributeAsDword("Team");

			if (dwFriendlyTeam != -1 && dwFriendlyTeam == dwTeam) continue;

			can_fire_t & CF = aShipFire[aShipFire.Add()];
			CF.pShip = pShip;
			CF.pFortCannon = null;
			CF.fDistance = (pShip->GetPos() - GetPos()).GetLength2D();

		} while (NetFindClassNext(true, &shipEID));

		NetFort * pFort = null;
		if (NetFindClass(true, &fortEID, "NetFort")) pFort = (NetFort*)api->GetEntityPointer(&fortEID);
		if (pFort && pFort->GetACharacter()->GetAttributeAsDword("FriendlyTeam") != dwOurTeam) for (dword i=0; i<pFort->GetNumCannons(); i++)
		{
			NetCannon * pC = pFort->GetCannon(i);
			if (pC->isDamaged()) continue;

			can_fire_t & CF = aShipFire[aShipFire.Add()];
			CF.pShip = null;
			CF.pFortCannon = pC;
			CF.fDistance = pC->GetDistance(GetPos());
		}

		if (aShipFire.Size())
		{
			// sorting
			aShipFire.Sort();

			for (dword i=0; i<aShipFire.Size(); i++) 
			{
				can_fire_t * pFire = &aShipFire[i];

				if (pFire->pShip)		pCannonController->Fire(pFire->pShip->GetPos());
				if (pFire->pFortCannon) pCannonController->Fire(pFire->pFortCannon->GetPos());
			}
		}
	}

	if (*pAttribute == "DoCannonFire" && pCannonController && pACamPos && pACamDir)
	{
		pCannonController->Fire(vCamPos, vCamDir);
		return 0;
	}

	if (*pAttribute == "MastFall")
	{
		dword dwMastNum = pAttribute->GetAttributeAsDword();
		if (dwMastNum < dword(iNumMasts) && !pMasts[dwMastNum].bBroken)
		{
			pMasts[dwMastNum].fDamage = 1.0f;
			pMasts[dwMastNum].bBroken = true;
			aMastFalls.Add(dwMastNum);
			//MastFall(&pMasts[dwMastNum]);
		}
		return 0;
	}

	if (*pAttribute == "CannonsReload")
	{
		if (pCannonController) pCannonController->Reload();
	}

	if (*pAttribute == "Dead")
	{
		if (pAttribute->GetAttributeAsDword()) SetDead();
	}

	return 0;
}

CVECTOR		NetShip::GetBoxSize()				{ return State.vBoxSize; };
ENTITY_ID	NetShip::GetModelEID()				{ return model_id; }
MODEL *		NetShip::GetModel()					{ Assert(api->ValidateEntity(&GetModelEID())); return (MODEL*)api->GetEntityPointer(&GetModelEID()); }
CMatrix *	NetShip::GetMatrix()				{ return &GetModel()->mtx; }
void		NetShip::SetMatrix(CMatrix & mtx)	{ GetModel()->mtx = mtx; }
CVECTOR		NetShip::GetAng()					{ return State.vAng; }
CVECTOR		NetShip::GetPos()					{ return State.vPos; }
void		NetShip::SetPos(CVECTOR & vNewPos)	{ State.vPos = vNewPos; }

void		NetShip::SetSpeed(float fSpeed)		{ fSailState = fSpeed; }
float		NetShip::GetSpeed()					{ return fSailState; }

void		NetShip::SetRotate(float fRotSpd)	{ Strength[0].vRotate.y = fRotSpd; }
float		NetShip::GetRotate()				{ return Strength[0].vRotate.y; }

float		NetShip::GetCurrentSpeed()			{ return KNOTS2METERS(State.vSpeed.z); }

