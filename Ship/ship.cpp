#include "ship.h" 
#include "ShipLights.h" 
#include "Track.h" 
#include "..\engine\program\sea_ai\Script_Defines.h"
#include "..\engine\program\sound.h"

INTERFACE_FUNCTION
CREATE_CLASS(SHIP)
CREATE_CLASS(ShipLights)
CREATE_CLASS(ShipTracks)

string	SHIP::sExecuteLayer;
string	SHIP::sRealizeLayer;

VDX8RENDER					* SHIP::pRS = null;
SEA_BASE					* SHIP::pSea = null;
ISLAND_BASE					* SHIP::pIsland = null;
COLLIDE						* SHIP::pCollide = null;
VGEOMETRY					* SHIP::pGS = null;

SHIP::SHIP() : aFirePlaces(_FL_, 16)
{
	pShipsLights = null;
	vSpeedAccel = 0.0f;
	uniIDX = 0;
	bUse = false;
	bDead = false;
	fSailState = 0.0f;
	bShip2Strand = false;
	bSetLightAndFog = false;
	iNumMasts = 0;

	bSetFixed = false;
	fFixedSpeed = 0.0f;

	bMassaShow = false;

	dtMastTrace.Setup(FRAND(0.5f), 0.5f);
	dtUpdateParameters.Setup(FRAND(1.0f), 1.0f);

	vCurDeadDir = 0.0f;

	State.vPos = 0.0f;
	fXOffset = fZOffset = 0.f;

	vSpeed = 0.0f;
	vSpeedsA = 0.0f;
	fMinusVolume = 0.0f;

	mRoot.SetIdentity();

	bVisible = true;
	bMounted = false;
	bKeelContour = false;
	bPerkTurnActive = false;

	fRockingY = 1.0f;
	fRockingAZ = 1.0f;

	pMasts = null;

	bModelUpperShip = false;
	pModelUpperShip = null;
}

SHIP::~SHIP()
{
	api->DeleteEntity(GetModelEID());
	api->Send_Message(sail_id, "li", MSG_SAIL_DEL_GROUP, GetID());
	api->Send_Message(rope_id, "li", MSG_ROPE_DEL_GROUP, GetModelEID());
	api->Send_Message(flag_id, "li", MSG_FLAG_DEL_GROUP, GetModelEID());
	api->Send_Message(vant_id, "li", MSG_VANT_DEL_GROUP, GetModelEID());
	api->DeleteEntity(blots_id);
	DELETE(pMasts);

	ENTITY_ID eidTmp;
	if (api->FindClass(&eidTmp, "ShipTracks", 0))
	{
		ShipTracks * pST = (ShipTracks*)api->GetEntityPointer(&eidTmp); 
		if (pST) pST->DelShip(this);
	}

	//aFirePlaces.DelAll();
}

//##################################################################
bool SHIP::Init()
{
	ZERO3(State, SP, vPos);
	ZERO3(vAng, ShipPoints, Strength);
	fXOffset = fZOffset = 0.f;

	Strength[STRENGTH_MAIN].bInertia = true;
	Strength[STRENGTH_MAIN].vSpeed = 0.0f;
	Strength[STRENGTH_MAIN].vRotate = 0.0f;

	srand(GetTickCount());

	LoadServices();

	iShipPriorityExecute = ((VDATA*)api->GetScriptVariable("iShipPriorityExecute"))->GetLong();
	iShipPriorityRealize = ((VDATA*)api->GetScriptVariable("iShipPriorityRealize"))->GetLong();

	return true;
}

void SHIP::LoadServices()
{
	ENTITY_ID ent;

	//api->LayerDel(sRealizeLayer,model_id);		// delete from realize

	pIsland = null;
	pSea = null;
	pRS = null;
	pGS = null;

	pGS = (VGEOMETRY *)api->CreateService("geometry");						Assert(pGS);
	pRS = (VDX8RENDER *)api->CreateService("dx8render");	Assert(pRS);
	pCollide = (COLLIDE *)api->CreateService("coll");						Assert(pCollide);

	api->FindClass(&touch_id, "touch", 0);

	if (api->FindClass(&ent, "island", 0)) pIsland = (ISLAND_BASE*)api->GetEntityPointer(&ent);
	if (api->FindClass(&sea_id, "sea", 0)) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);

	api->FindClass(&FirePlace::eidSound, "sound", 0);	
}

CVECTOR SHIP::ShipRocking(float fDeltaTime)
{
	CMatrix mat;
    mat = *GetMatrix();

	fDeltaTime = Min(fDeltaTime, 0.1f);
	float fDelta = (fDeltaTime / 0.025f);

	if (!pSea) return vAng;
	//api->Send_Message(model,"ll",2,(long)&mat);

	CVECTOR vAng2 = State.vAng;

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
			point.x = xx + State.vPos.x + fXOffset;
			point.y = mat.m[0][1]*x + mat.m[2][1]*z + State.vPos.y;
			point.z = zz + State.vPos.z + fZOffset;
			
			float wave_y = pSea->WaveXZ(point.x, point.z);
			float f = (wave_y - point.y);
			
			pspt->fY = wave_y;

			fFullY += pspt->fY;
		}
	}

	float fNewPos = fFullY / 36.0f;//(ShipPoints[2][2].fY + ShipPoints[3][2].fY + ShipPoints[2][3].fY + ShipPoints[3][3].fY) / 4.0f;
	State.vPos.y = State.vPos.y + Min(fDelta * fRockingY, 1.0f) * (fNewPos - State.vPos.y);

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

BOOL SHIP::CalculateNewSpeedVector(CVECTOR * Speed, CVECTOR * Rotate)
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

BOOL SHIP::ApplyStrength(float dtime, BOOL bCollision)
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
	/*if (State.vSpeed.x>5.0f) State.vSpeed.x = 5.0f;
	if (State.vSpeed.x<-5.0f) State.vSpeed.x = -5.0f;
	if (State.vSpeed.z>10.0f) State.vSpeed.z = 10.0f;
	if (State.vSpeed.z<-10.0f) State.vSpeed.z = -10.0f;
	if (State.vRotate.y>0.1f) State.vRotate.y = 0.1f;
	if (State.vRotate.y<-0.1f) State.vRotate.y = -0.1f; */

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

BOOL SHIP::TouchMove(DWORD DeltaTime, TOUCH_PARAMS *pTPOld, TOUCH_PARAMS *pTPNew)
{
	if (!pTPOld && !pTPNew) return false;
	SHIP_STATE old_state = State;

	if (pTPOld)
	{
		pTPOld->vSpeed	= State.vSpeed;		pTPOld->vRotate	= State.vRotate;
		pTPOld->vPos	= State.vPos;		pTPOld->vAng	= State.vAng;
		pTPOld->vPos.x += fXOffset;
		pTPOld->vPos.z += fZOffset;
	}

	Move(DeltaTime,true);
	
	if (pTPNew)
	{
		pTPNew->vSpeed	= State.vSpeed;		pTPNew->vRotate	= State.vRotate;
		pTPNew->vPos	= State.vPos;		pTPNew->vAng	= State.vAng;
		pTPNew->vPos.x += fXOffset;
		pTPNew->vPos.z += fZOffset;
	}
	
	State = old_state; 

	return true;
}

BOOL SHIP::Move(DWORD DeltaTime, BOOL bCollision)
{
	float dtime = DELTA_TIME(DeltaTime);
	ApplyStrength(dtime, bCollision);
	if (!bCollision && !isDead()) 
	{
		State.vAng = ShipRocking(dtime);
	}

	// Update position
	//if (!isDead())
	{
		float k = KNOTS2METERS(((bSetFixed) ? fFixedSpeed : State.vSpeed.z)) * dtime;
		State.vPos.z += k * cosf(State.vAng.y);
		State.vPos.x += k * sinf(State.vAng.y);

		k = KNOTS2METERS(State.vSpeed.x) * dtime;
		State.vPos.z += k * cosf(State.vAng.y+PId2);
		State.vPos.x += k * sinf(State.vAng.y+PId2);

		State.vAng.y += State.vRotate.y * dtime;
	}

	return true;
}

#define SHIP_TEST_Z_TIME		1.0f
#define SHIP_TEST_Y_TIME		1.0f

float SHIP::GetRotationAngle(float * pfTime)
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

float SHIP::GetBrakingDistance(float * pfTime)
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
void SHIP::CalculateImmersion()		
{
	ATTRIBUTES * pAShipImmersion = GetACharacter()->FindAClass(GetACharacter(),"Ship.Immersion");
	State.fShipImmersion = (pAShipImmersion) ? pAShipImmersion->GetAttributeAsFloat() : 0.0f;
	//return State.fShipImmersion;
}

void SHIP::CheckShip2Strand(float fDeltaTime)
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
		api->Event(SHIP_TO_STRAND, "ll", GetIndex(GetACharacter()), bShip2Strand);
	}
}

void SHIP::SetDead()
{
	if (!isDead())
	{
		State.vAng = vAng;

		vOldAng = State.vAng;
		vOldPos = State.vPos;

		ATTRIBUTES * pASinkSpeed = GetACharacter()->FindAClass(GetACharacter(), "ship.sink.speed"); 

		//aref aSink; makearef(aSink, rDead.Ship.Sink);
		if(!pASinkSpeed)
		{
			pASinkSpeed = GetACharacter()->CreateSubAClass(GetACharacter(), "ship.sink.speed");
			pASinkSpeed->SetAttributeUseFloat("y", 0.35f);	// speed of sink y
			pASinkSpeed->SetAttributeUseFloat("x", 0.017f * (FRAND(1.0f) * 2.0f - 1.0f));	// speed sink angle rotate around x
			pASinkSpeed->SetAttributeUseFloat("z", 0.017f * (FRAND(1.0f) * 2.0f - 1.0f));	// speed sink angle rotate around z
		}
		
		//Assert(pASinkSpeed);

		vDeadDir.x = pASinkSpeed->GetAttributeAsFloat("x");
		vDeadDir.y = pASinkSpeed->GetAttributeAsFloat("y");
		vDeadDir.z = pASinkSpeed->GetAttributeAsFloat("z");

		vCurDeadDir = 0.0f;

		bDead = true;
		api->LayerDel("sea_reflection2", GetID());

		if (pShipsLights)
			pShipsLights->SetDead(this);
	}
}

void SHIP::Execute(DWORD DeltaTime)
{
	ATTRIBUTES * pAPerks = GetACharacter()->FindAClass(GetACharacter(), "TmpPerks");

	ATTRIBUTES * pARocking = GetAShip()->GetAttributeClass("Rocking");
	
	if (pARocking)
	{
		fRockingY = pARocking->GetAttributeAsFloat("y", 1.0f);
		fRockingAZ = pARocking->GetAttributeAsFloat("az", 1.0f);
	}

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
	}

	// temp
	uniIDX = GetACharacter()->GetAttributeAsDword("index");
	if (uniIDX>=900) uniIDX = uniIDX - 900 + 2;

	float fDeltaTime = Min(0.1f, float(DeltaTime) * 0.001f);

	if (!bMounted) return;

	ATTRIBUTES	* pAShipStopped = GetACharacter()->FindAClass(GetACharacter(),"ship.stopped");
	bool		bMainCharacter = GetACharacter()->GetAttributeAsDword("MainCharacter", 0) != 0;

	if (dtUpdateParameters.Update(fDeltaTime))
	{
		if (!pAShipStopped || pAShipStopped->GetAttributeAsDword() == 0)
		{
			api->Event(SHIP_UPDATE_PARAMETERS, "lf", GetIndex(GetACharacter()), fSailState);
		}
	}
	// check impulse
	ATTRIBUTES * pAImpulse = GetACharacter()->FindAClass(GetACharacter(), "Ship.Impulse");
	if (pAImpulse && !isDead())
	{
		CVECTOR vRotate = 0.0f, vXSpeed = 0.0f;
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
			vXSpeed.x = pARotate->GetAttributeAsFloat("x", 0.0f);
			vXSpeed.y = pARotate->GetAttributeAsFloat("y", 0.0f);
			vXSpeed.z = pARotate->GetAttributeAsFloat("z", 0.0f);
		}

		GetACharacter()->DeleteAttributeClassX(pAImpulse);

		STRENGTH st;
		st.bInertia = false;
		st.bUse = true;
		st.vRotate = vRotate * fDeltaTime;
		st.vSpeed = vXSpeed * fDeltaTime;
		AddStrength(&st);
	}

	//if (DeltaTime==0) _asm int 3
	CalculateImmersion();		// 

    api->Send_Message(sail_id, "lipf", MSG_SAIL_GET_SPEED, GetID(), &Strength[STRENGTH_MAIN].vSpeed.z, fSailState);
	if (isDead()) Strength[STRENGTH_MAIN].vSpeed.z = 0.0f;

	vPos = State.vPos;
	vAng = State.vAng;

	Move(DeltaTime, false);

	if (!isDead()) 
	{
		float fRotate = State.vRotate.y;
		if (fRotate > 0.25f) fRotate = 0.25f;
		if (fRotate < -0.25f) fRotate = -0.25f;
		vAng.z += fRotate;
	}

	ATTRIBUTES * pASpeed = GetACharacter()->FindAClass(GetACharacter(), "ship.speed"); 
	if (!pASpeed) { pASpeed = GetACharacter()->CreateSubAClass(GetACharacter(), "ship.speed"); Assert(pASpeed); }
	pASpeed->SetAttributeUseFloat("x", State.vSpeed.x);	
	pASpeed->SetAttributeUseFloat("y", State.vRotate.y);	
	pASpeed->SetAttributeUseFloat("z", State.vSpeed.z);

	/*if (isDead())
	{
		CVECTOR vAng2 = ShipRocking(fDeltaTime);

		float fScale = Bring2Range(8.0f, 22.0f, -5.0f, 0.0f, vPos.y);
		fMinusVolume += fScale * fDeltaTime * 0.5f;
		State.vAng = vAng2;

		vCurDeadDir += vDeadDir * fDeltaTime;

		State.vSpeed.z -= State.vSpeed.z * 0.3f * fDeltaTime;

		if (isVisible() && vPos.y < -80.0f)
		{
			bVisible = false;

			// stop all fireplaces
			for (dword i=0; i<aFirePlaces.Size(); i++) aFirePlaces[i].Stop();

			// del vant,flags,sail and ropes
			api->Send_Message(sail_id, "li", MSG_SAIL_DEL_GROUP, GetID());
			api->Send_Message(rope_id, "li", MSG_ROPE_DEL_GROUP, GetModelEID());
			api->Send_Message(flag_id, "li", MSG_FLAG_DEL_GROUP, GetModelEID());
			api->Send_Message(vant_id, "li", MSG_VANT_DEL_GROUP, GetModelEID());

			api->LayerDel((char*)sRealizeLayer.GetBuffer(), GetID());
			api->LayerDel("ship_cannon_trace", GetID());
			api->Event(SHIP_DELETE, "li", GetIndex(GetACharacter()), GetID());
		}

		if (vPos.y < -100.0f) vPos.y = -1000.0f;
	}*/

	if (isDead())
	{
		float fScale = Bring2Range(0.0f, 1.0f, -15.0f, 0.0f, vPos.y);

		//State.vPos.y = 0.0f;
		//State.vAng.x = 0.0f;
		//State.vAng.z = 0.0f;

		State.vAng = vOldAng;
		State.vPos = vOldPos;

		State.vAng = ShipRocking(fDeltaTime);

		vOldAng = State.vAng;
		vOldPos = State.vPos;

		vCurDeadDir.x += vDeadDir.x * fDeltaTime;
		vCurDeadDir.y += vDeadDir.y * fDeltaTime;
		vCurDeadDir.z += vDeadDir.z * fDeltaTime;

		/*State.vPos.y *= fScale;		State.vPos.y -= vCurDeadDir.y;
		State.vAng.x *= fScale;		State.vAng.x += vCurDeadDir.x;	
		State.vAng.z *= fScale;		State.vAng.z += vCurDeadDir.z;*/

		State.vPos.y -= vCurDeadDir.y;
		//State.vAng.x += vCurDeadDir.x;	
		//State.vAng.z += vCurDeadDir.z;

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

			api->LayerDel((char*)sRealizeLayer.GetBuffer(), GetID());
			api->LayerDel("ship_cannon_trace", GetID());
			api->Event(SHIP_DELETE, "li", GetIndex(GetACharacter()), GetID());
		}
	}
	else
	{
	}

	// set attributes for script
	ATTRIBUTES * pAPos = GetACharacter()->FindAClass(GetACharacter(),"ship.pos");
	ATTRIBUTES * pAAng = GetACharacter()->FindAClass(GetACharacter(),"ship.ang");
	if (!pAPos) { pAPos = GetACharacter()->CreateSubAClass(GetACharacter(),"ship.pos"); Assert(pAPos); }
	if (!pAAng) { pAAng = GetACharacter()->CreateSubAClass(GetACharacter(),"ship.ang"); Assert(pAAng); }
	pAPos->SetAttributeUseFloat("x",State.vPos.x+fXOffset);	pAPos->SetAttributeUseFloat("y",State.vPos.y);	pAPos->SetAttributeUseFloat("z",State.vPos.z+fZOffset);
	pAAng->SetAttributeUseFloat("x",State.vAng.x);	pAAng->SetAttributeUseFloat("y",State.vAng.y);	pAAng->SetAttributeUseFloat("z",State.vAng.z);

	// check strand
	if (!bDead && bKeelContour) CheckShip2Strand(fDeltaTime);

	RecalculateWorldOffset();
	mRoot = CMatrix(vAng.x + vCurDeadDir.x, vAng.y, vAng.z + vCurDeadDir.z, vPos.x+fXOffset, vPos.y - State.fShipImmersion - SP.fWaterLine, vPos.z+fZOffset);
	SetMatrix(mRoot);

	CMatrix matrix = UpdateModelMatrix();

	// activate mast tracer
	if (dtMastTrace.Update(fDeltaTime))
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
	}

// key states
	Strength[STRENGTH_MAIN].vRotate.y = 0.0f;

	if (bMainCharacter && (!pAShipStopped || pAShipStopped->GetAttributeAsDword() == 0))
	{
		CONTROL_STATE cs, cs1;
		api->Controls->GetControlState("Ship_TurnLeft", cs);	api->Controls->GetControlState("Ship_TurnLeft1", cs1);
		if (cs.state == CST_ACTIVE || cs1.state == CST_ACTIVE) Strength[STRENGTH_MAIN].vRotate.y = -1.0f;
		api->Controls->GetControlState("Ship_TurnRight", cs);	api->Controls->GetControlState("Ship_TurnRight1", cs1);
		if (cs.state == CST_ACTIVE || cs1.state == CST_ACTIVE) Strength[STRENGTH_MAIN].vRotate.y = 1.0f;
		api->Controls->GetControlState("Ship_SailUp", cs);		api->Controls->GetControlState("Ship_SailUp1", cs1);
		if (cs.state == CST_ACTIVATED || cs1.state == CST_ACTIVATED) fSailState += 0.5f;
		api->Controls->GetControlState("Ship_SailDown", cs);	api->Controls->GetControlState("Ship_SailDown1", cs1);
		if (cs.state == CST_ACTIVATED || cs1.state == CST_ACTIVATED) fSailState -= 0.5f;
		
		if (fSailState < 0.0f) fSailState = 0.0f;
		if (fSailState > 1.0f) fSailState = 1.0f;
	}

	/*if (isDead())
	{
		fSailState = 0.0f;
	}*/

	// execute fire places
	for (dword i=0; i<aFirePlaces.Size(); i++) aFirePlaces[i].Execute(fDeltaTime);

	/*  // boal del_cheat
#ifndef _XBOX
	if (api->Controls->GetDebugAsyncKeyState('L') < 0)
		if (GetAsyncKeyState('K') < 0)
		{
			// for test only
			if (GetAsyncKeyState('0') < 0) bUse = (uniIDX == 0);
			if (GetAsyncKeyState('1') < 0) bUse = (uniIDX == 1);
			if (GetAsyncKeyState('2') < 0) bUse = (uniIDX == 2);
			if (GetAsyncKeyState('3') < 0) bUse = (uniIDX == 3);
			if (GetAsyncKeyState('4') < 0) bUse = (uniIDX == 4);
			if (GetAsyncKeyState('5') < 0) bUse = (uniIDX == 5);
			if (GetAsyncKeyState('6') < 0) bUse = (uniIDX == 6);
			if (GetAsyncKeyState('7') < 0) bUse = (uniIDX == 7);
			if (GetAsyncKeyState('8') < 0) bUse = (uniIDX == 8);
			if (GetAsyncKeyState('9') < 0) bUse = (uniIDX == 9);

			if (GetAsyncKeyState('J')<0 && bUse)
			{
				float fov;
				CVECTOR cpos,cang;
				pRS->GetCamera(cpos,cang,fov);
				State.vPos = cpos;
				fXOffset = fZOffset = 0.f;
				RecalculateWorldOffset();
				State.vAng = cang;
			}
		}
#endif
	 */
	// water sound: set position and volume
	ATTRIBUTES * pASounds = AttributesPointer->FindAClass(AttributesPointer, "Ship.Sounds");

	if (pASounds)
	{
		ATTRIBUTES * pAWaterID = pASounds->GetAttributeClass("WaterID");
		ATTRIBUTES * pATurnID = pASounds->GetAttributeClass("TurnID");
		ATTRIBUTES * pASailsID = pASounds->GetAttributeClass("SailsID");

		CMatrix mRotate; mRoot.Get3X3(mRotate);
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
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iWaterSound, SOUND_PARAM_POSITION, vPos.x+fXOffset, vPos.y, vPos.z+fZOffset);
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
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iTurnSound, SOUND_PARAM_POSITION, vPos.x+fXOffset, vPos.y, vPos.z+fZOffset);
		}

		long iSailSound = long(pASounds->GetAttributeAsDword("SailsID", -1));
		if (iSailSound > 0)
		{
			float x = pASailsID->GetAttributeAsFloat("x", 0.0f);
			float y = pASailsID->GetAttributeAsFloat("y", 0.0f);
			float z = pASailsID->GetAttributeAsFloat("z", 0.0f);
			CVECTOR vPos = State.vPos + (mRotate * CVECTOR(vBoxSize.x * x, vBoxSize.y * y, vBoxSize.z * z));
			api->Send_Message(FirePlace::eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iSailSound, SOUND_PARAM_POSITION, vPos.x+fXOffset, vPos.y, vPos.z+fZOffset);
		}
	}
}

void SHIP::MastFall(mast_t * pM)
{
	if (pM && pM->pNode && pM->fDamage >= 1.0f) 
	{
		ENTITY_ID ent;
		api->CreateEntity(&ent, "mast");
		api->Send_Message(ent, "lpii", MSG_MAST_SETGEOMETRY, pM->pNode, GetID(), GetModelEID());
		api->LayerAdd((char*)sExecuteLayer.GetBuffer(), ent, iShipPriorityExecute+1);
		api->LayerAdd((char*)sRealizeLayer.GetBuffer(), ent, iShipPriorityRealize+1);

		pShipsLights->KillMast(this, pM->pNode, false);

		char str[256];
		sprintf(str, "%s", pM->pNode->GetName());
		ATTRIBUTES * pAMasts = GetACharacter()->FindAClass(GetACharacter(),"Ship.Masts");
		if (!pAMasts)
			pAMasts = GetACharacter()->CreateSubAClass(GetACharacter(),"Ship.Masts");
		Assert(pAMasts);
		pAMasts->SetAttributeUseFloat(str, 1.0f);
		pM->bBroken = true;
	}
}

CMatrix SHIP::UpdateModelMatrix()
{
	MODEL * pModel = GetModel(); Assert(pModel);
	pModel->Update();
	return pModel->mtx;
}

void SHIP::RecalculateWorldOffset()
{
	// calculate X offset
	/*long nTmp = (long)((State.vPos.x + fXOffset) * 0.001f) * 1000;
	float fTmp = fXOffset - (float)nTmp;
	if( fTmp != 0.f ) {
		fXOffset = (float)nTmp;
		State.vPos.x += fTmp;
		vPos.x = State.vPos.x;
	}
	// calculate Z offset
	nTmp = (long)((State.vPos.z + fZOffset) * 0.001f) * 1000;
	fTmp = fZOffset - (float)nTmp;
	if( fTmp != 0.f ) {
		fZOffset = (float)nTmp;
		State.vPos.z += fTmp;
		vPos.z = State.vPos.z;
	}*/
}

//##################################################################

void SHIP::SetLightAndFog(bool bSetLight)
{
	if (isDead() && isVisible())
	{
		bSetLightAndFog = true;

		// ambient
		float fScale = Bring2Range(0.0f, 1.0f, -35.0f, 0.0f, State.vPos.y);
		pRS->GetRenderState(D3DRS_AMBIENT, &dwSaveAmbient);
		CVECTOR vAmbient = fScale * COLOR2VECTOR(dwSaveAmbient);
		pRS->SetRenderState(D3DRS_AMBIENT, ARGB((dwSaveAmbient >> 24L), vAmbient.x, vAmbient.y, vAmbient.z));

		// light
		D3DLIGHT8 newLight;
		pRS->GetLight(0, &saveLight);
		newLight = saveLight;
		newLight.Diffuse.r *= fScale;	newLight.Diffuse.g *= fScale;	newLight.Diffuse.b *= fScale;	newLight.Diffuse.a *= fScale; 
		newLight.Specular.r *= fScale;	newLight.Specular.g *= fScale;	newLight.Specular.b *= fScale;	newLight.Specular.a *= fScale; 
		newLight.Ambient.r *= fScale;	newLight.Ambient.g *= fScale;	newLight.Ambient.b *= fScale;	newLight.Ambient.a *= fScale; 
		pRS->SetLight(0, &newLight);

		// fog
		float fFogScale = Bring2Range(0.0f, 1.0f, -30.0f, 0.0f, State.vPos.y);
		pRS->GetRenderState(D3DRS_FOGCOLOR, &dwSaveFogColor);
		CVECTOR vFogColor = fFogScale * COLOR2VECTOR(dwSaveFogColor);
		pRS->SetRenderState(D3DRS_FOGCOLOR, ARGB((dwSaveFogColor >> 24L), vFogColor.x, vFogColor.y, vFogColor.z));
	}
}

void SHIP::RestoreLightAndFog()
{
	if (bSetLightAndFog)
	{
		bSetLightAndFog = false;
		pRS->SetRenderState(D3DRS_AMBIENT, dwSaveAmbient);
		pRS->SetRenderState(D3DRS_FOGCOLOR, dwSaveFogColor);
		pRS->SetLight(0, &saveLight);
	}
}

void SHIP::Realize(DWORD dtime)
{
	if (!bMounted) return;

	//if (dtime)
	//	ShipRocking2(float(dtime) * 0.001f);

	MODEL * pM = GetModel(); Assert(pM);

	bSetLightAndFog = false;

	SetLightAndFog(true);
	SetLights();

	pRS->SetRenderState(D3DRS_LIGHTING, true);
	pM->Realize(dtime);
	pRS->SetRenderState(D3DRS_LIGHTING, false);

	UnSetLights();
	RestoreLightAndFog();

	// draw model
	if (bModelUpperShip && pModelUpperShip)
	{
		CMatrix m1;
		m1.BuildMatrix(0.0f, fUpperShipAY, 0.0f, State.vPos.x+fXOffset, State.vPos.y + fUpperShipY, State.vPos.z+fZOffset);
		pModelUpperShip->mtx = m1; 
		pModelUpperShip->Realize(dtime);
	}

	if (bMassaShow)
	{
		Render().Print(0, 120, "Massa: %.2f", GetAShip()->GetAttributeAsFloat("Massa"));
		Render().Print(0, 140, "Volume: %.2f", GetAShip()->GetAttributeAsFloat("Volume"));
	}
}

void SHIP::SetLights()
{
	pShipsLights->SetLights(this);
}

void SHIP::UnSetLights()
{
	pShipsLights->UnSetLights(this);
}

void SHIP::Fire(const CVECTOR & vPos)
{
	pShipsLights->AddDynamicLights(this, vPos);
}

float SHIP::GetMaxSpeedZ()
{
	ATTRIBUTES * pAMaxSpeedZ = GetACharacter()->FindAClass(GetACharacter(), "Ship.MaxSpeedZ");
	return (pAMaxSpeedZ) ? pAMaxSpeedZ->GetAttributeAsFloat() : 0.0f;
}

float SHIP::GetMaxSpeedY()
{
	ATTRIBUTES * pAMaxSpeedY = GetACharacter()->FindAClass(GetACharacter(), "Ship.MaxSpeedY");
	return (pAMaxSpeedY) ? pAMaxSpeedY->GetAttributeAsFloat() : 0.0f;
}

bool SHIP::DelStrength(long iIdx)
{
	if (iIdx<RESERVED_STRENGTH || iIdx>=MAX_STRENGTH) return false;
	Strength[iIdx].bUse = false;
	return true;
}

long SHIP::AddStrength(STRENGTH *strength)
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

dword _cdecl SHIP::ProcessMessage(MESSAGE & message)
{
	ENTITY_ID	entity_id;
	CVECTOR		cpos, cang;
	float		fov;
	long		code = message.Long();
	char		str[256], str1[256], str2[256];

	switch (code)
	{
		case MSG_SEA_REFLECTION_DRAW:
			Realize(0);
		break;
		case AI_MESSAGE_SET_LAYERS:
			message.String(sizeof(str), str); sExecuteLayer = str;
			message.String(sizeof(str), str); sRealizeLayer = str;
		break;
		case MSG_SHIP_CREATE:
			SetACharacter(message.AttributePointer());
			Mount(message.AttributePointer());
			LoadPositionFromAttributes();
		break;
		case MSG_SHIP_SET_SAIL_STATE:
			SetSpeed(message.Float());
		break;
		case MSG_SHIP_GET_SAIL_STATE:
		{
			VDATA * pV = message.ScriptVariablePointer();
			pV->Set(GetSpeed());
		}
		break;
		case MSG_SHIP_SET_POS:
			pRS->GetCamera(cpos,cang,fov);
			State.vPos = cpos;
			State.vAng = cang;
			if (pSea)
				State.vPos.y = pSea->WaveXZ(State.vPos.x, State.vPos.z);
			fXOffset = fZOffset = 0.f;
			RecalculateWorldOffset();
		break;
		case MSG_SHIP_ACTIVATE_FIRE_PLACE:
		{
			dword dwFPIndex = dword(message.Long());
			message.String(sizeof(str), str);
			message.String(sizeof(str1), str1);
			message.String(sizeof(str2), str2);
			//long iSoundID = message.Long();
			float fRunTime = message.Float();
			long iBallCharacterIndex = message.Long();
			Assert(dwFPIndex != INVALID_ARRAY_INDEX && dwFPIndex < aFirePlaces.Size());
			aFirePlaces[dwFPIndex].Run(str, str1, iBallCharacterIndex, str2, fRunTime);
		}
		break;
		case MSG_SHIP_GET_CHARACTER_INDEX:
		{
			VDATA * pVData = message.ScriptVariablePointer();
			pVData->Set((long)GetIndex(GetACharacter()));
		}
		break;
		case MSG_SHIP_GET_NUM_FIRE_PLACES:
		{
			VDATA * pVData = message.ScriptVariablePointer();
			pVData->Set((long)aFirePlaces.Size());
		}
		break;
		case MSG_SHIP_RESET_TRACK:
		{
			ENTITY_ID eidTmp;
			if (api->FindClass(&eidTmp, "ShipTracks", 0))
			{
				ShipTracks * pST = (ShipTracks*)api->GetEntityPointer(&eidTmp); 
				pST->ResetTrack(this);
			}
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
		case MSG_SHIP_SETLIGHTSOFF:
		{
			float fTime = message.Float();
			bool bLigths = message.Long() != 0;
			bool bFlares = message.Long() != 0;
			bool bNow = message.Long() != 0;
			if (pShipsLights)
				pShipsLights->SetLightsOff(this, fTime, bLigths, bFlares, bNow);
		}
		break;
		case MSG_MAST_DELGEOMETRY:
		{
			NODE * pNode = (NODE *)message.Pointer();
			pShipsLights->KillMast(this, pNode, true);
		}
		break;
		case MSG_SHIP_SAFE_DELETE:
			// all system which have particles - must be deleted here
			aFirePlaces.DelAll();
		break;
		// boal 20.08.06 перерисовка флага -->
		case MSG_SHIP_FLAG_REFRESH:
            api->Send_Message(flag_id, "li", MSG_FLAG_DEL_GROUP, GetModelEID());
			if (api->FindClass(&flag_id,"flag",0))
				api->Send_Message(flag_id,"lil",MSG_FLAG_INIT,GetModelEID(),GetNation(GetACharacter()));
		break;
		// boal 20.08.06 перерисовка флага <--
	}
	return 0;
}

void SHIP::LoadPositionFromAttributes()
{
	ATTRIBUTES * pAPos = GetACharacter()->FindAClass(GetACharacter(),"ship.pos");
	ATTRIBUTES * pAAng = GetACharacter()->FindAClass(GetACharacter(),"ship.ang");
	Assert(pAPos && pAAng);
	State.vPos.x = pAPos->GetAttributeAsFloat("x");	
	State.vPos.z = pAPos->GetAttributeAsFloat("z");
	State.vAng.y = pAAng->GetAttributeAsFloat("y");
	fXOffset = fZOffset = 0.f;
	RecalculateWorldOffset();
}

bool SHIP::Mount(ATTRIBUTES * _pAShip)
{
	Assert(_pAShip);
	pAShip = _pAShip;

	long iIndex = GetIndex(pAShip);

	api->Event("Ship_StartLoad", "a", GetACharacter());
	api->Event(SEA_GET_LAYERS, "i", GetID());
	Assert(sRealizeLayer.Len() != 0 && sExecuteLayer.Len() != 0);

	api->LayerAdd("sea_reflection2", GetID(), 100);
	api->LayerAdd("rain_drops", GetID(), 100);

	api->LayerAdd((char *)sRealizeLayer.GetBuffer(), GetID(), iShipPriorityRealize);
	api->LayerAdd((char *)sExecuteLayer.GetBuffer(), GetID(), iShipPriorityExecute);

	api->LayerAdd("ship_cannon_trace", GetID(), iShipPriorityExecute);

	char * pName = GetAShip()->GetAttribute("Name");
	if (!pName)
	{
		api->Trace("SHIP::Mount : Can't find attribute name in ShipsTypes %d, char: %d, %s, %s, %s", GetAShip()->GetAttributeAsDword("index"), GetACharacter()->GetAttributeAsDword("index"), GetACharacter()->GetAttribute("name"), GetACharacter()->GetAttribute("lastname"), GetACharacter()->GetAttribute("id"));
		//GetACharacter()->Dump(GetACharacter(), 0);
		bMounted = false;
		return false;
	}

	strcpy(cShipIniName,pName);

	//api->Trace("Create ship with type = %s", cShipIniName);

	ATTRIBUTES * pASpeedZ = GetACharacter()->FindAClass(GetACharacter(),"Ship.Speed.z");
	float fNewSailState = (pASpeedZ) ? pASpeedZ->GetAttributeAsFloat() : 0.0f;
	ATTRIBUTES * pAShipStopped = GetACharacter()->FindAClass(GetACharacter(),"ship.stopped");
	if (pAShipStopped && pAShipStopped->GetAttributeAsDword()) fNewSailState = 0.0f;
	SetSpeed(fNewSailState);

	uniIDX = GetACharacter()->GetAttributeAsDword("index");
	if (uniIDX>=900) uniIDX = uniIDX - 900 + 2;

	State.vPos = 0.0f;
	State.vPos.x = 10.0f + uniIDX * 20.0f;
	fXOffset = fZOffset = 0.f;
	RecalculateWorldOffset();
	bUse = uniIDX==0;

	char temp_str[256];
	sprintf(temp_str,"ships\\%s\\%s",cShipIniName,cShipIniName);

	api->CreateEntity(&model_id,"MODELR");
	api->Send_Message(GetModelEID(),"ls",MSG_MODEL_LOAD_GEO,temp_str);
	api->LayerAdd("hull_trace",GetModelEID(),10);
	api->LayerAdd("sun_trace",GetModelEID(),10);
	api->LayerAdd("mast_ship_trace",GetID(),10);

	// sails
	if (api->FindClass(&sail_id,"sail",0))
		api->Send_Message(sail_id,"liil",MSG_SAIL_INIT,GetID(),GetModelEID(), GetSpeed() ? 1 : 0); 
    
	// ropes
	if (api->FindClass(&rope_id,"rope",0))
		api->Send_Message(rope_id,"lii",MSG_ROPE_INIT,GetID(),GetModelEID()); 

	// flags
	if (api->FindClass(&flag_id,"flag",0))
		api->Send_Message(flag_id,"lil",MSG_FLAG_INIT,GetModelEID(),GetNation(GetACharacter())); 

	// vants
	if (api->FindClass(&vant_id,"vant",0))
		api->Send_Message(vant_id,"lii",MSG_VANT_INIT,GetID(),GetModelEID()); 
	
	// blots
	if (api->CreateEntity(&blots_id,"blots"))
	{
		api->Send_Message(blots_id, "lia", MSG_BLOTS_SETMODEL, GetModelEID(), GetACharacter()); 
		api->LayerAdd(sRealizeLayer, blots_id, iShipPriorityRealize + 4);
		api->LayerAdd(sExecuteLayer, blots_id, iShipPriorityExecute + 4);
	}

	LoadShipParameters();

	ENTITY_ID temp_id = GetID();
	api->Send_Message(touch_id,"li",MSG_SHIP_CREATE,temp_id);
	api->Send_Message(sea_id,"lic",MSG_SHIP_CREATE,temp_id,CVECTOR(State.vPos.x+fXOffset,State.vPos.y,State.vPos.z+fZOffset));

	GEOS::INFO ginfo;
	MODEL * pModel = GetModel(); Assert(pModel);
	NODE * pNode = pModel->GetNode(0); Assert(pNode);
	pNode->geo->GetInfo(ginfo);

	CalcRealBoxSize();

	State.vBoxSize.x = ginfo.boxsize.x;
	State.vBoxSize.y = ginfo.boxsize.y;
	State.vBoxSize.z = ginfo.boxsize.z;

	SP.fLength = State.vBoxSize.z;
	SP.fWidth = State.vBoxSize.x;
	fGravity = 9.81f;

	float fCapacity = (float)SP.iCapacity; 
	float fLength = (float)30;
	
	float maxw = 10000.0f;
	float min_inertia = 0.28f; 
	float max_inertia = 0.003f;

	State.fLoad = 0.0f;

	State.fShipSpeedZ = SP.fSpeedRate;
	State.fShipSpeedY = SP.fTurnRate / 444.0f;
	State.fInertiaFactor = fCapacity/maxw;
	State.fMassInertia = 0.10f; //min_inertia - (min_inertia - max_inertia) * State.fInertiaFactor;
	
	State.fWeight = SP.fWeight;

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
	if (api->FindClass(&eidTmp, "shiplights", 0))
	{
		pShipsLights = (IShipLights*)api->GetEntityPointer(&eidTmp); 

		pShipsLights->AddLights(this, GetModel(), bLights, bFlares);
		pShipsLights->Execute(0);
	}
	Assert(pShipsLights);

	NODE * pFDay = pModel->FindNode("fonar_day"); 
	NODE * pFNight = pModel->FindNode("fonar_night"); 
	
	if (pFDay && pFNight)
		((bLights) ? pFDay : pFNight)->flags &= (~NODE::VISIBLE);

	// add fireplaces
	ScanShipForFirePlaces();
	// add masts
	BuildMasts();

	for (long i=0; i<iNumMasts; i++) if (pMasts[i].bBroken)
	{
		pShipsLights->KillMast(this, pMasts[i].pNode, true);
	}

	// create model upper ship if needed
	ENTITY_ID model_uppership_id;
	ATTRIBUTES * pAUpperShipModel = GetACharacter()->FindAClass(GetACharacter(), "ship.upper_model");
	if (pAUpperShipModel)
	{
		fUpperShipAY = pAUpperShipModel->GetAttributeAsFloat("ay", 0.0f);
		fUpperShipY = pAUpperShipModel->GetAttributeAsFloat("y", State.vBoxSize.y * 2.0f + 10.0f);

		strcpy(temp_str, pAUpperShipModel->GetThisAttr());

		bModelUpperShip = true;
		api->CreateEntity(&model_uppership_id, "MODELR");
		api->Send_Message(model_uppership_id, "ls", MSG_MODEL_LOAD_GEO, temp_str);
		pModelUpperShip = (MODEL*)api->GetEntityPointer(&model_uppership_id);
	}

	// event to script
	api->Event(SHIP_CREATE,"li",GetACharacter()->GetAttributeAsDword("index"),GetID());
	api->Event("Ship_EndLoad", "a", GetACharacter());

	// add to ship tracks
	if (api->FindClass(&eidTmp, "ShipTracks", 0))
	{
		ShipTracks * pST = (ShipTracks*)api->GetEntityPointer(&eidTmp); 
		if (pST) pST->AddShip(this);
	}

	if (pSea)
		State.vPos.y = pSea->WaveXZ(State.vPos.x+fXOffset, State.vPos.z+fZOffset);

	ATTRIBUTES * pABox = GetACharacter()->FindAClass(GetACharacter(),"ship.boxsize");
	if (!pABox) { pABox = GetACharacter()->CreateSubAClass(GetACharacter(),"ship.boxsize"); Assert(pABox); }
	pABox->SetAttributeUseFloat("x",State.vRealBoxSize.x);	pABox->SetAttributeUseFloat("y",State.vRealBoxSize.y);	pABox->SetAttributeUseFloat("z",State.vRealBoxSize.z);

	bMounted = true;
	return true;
}

void SHIP::CalcRealBoxSize()
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

void SHIP::ScanShipForFirePlaces()
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
		api->Trace("Ship %s doesn't have fire places",cShipIniName);
	}
}

BOOL SHIP::LoadShipParameters()
{
#define GetADword(x)	GetAShip()->GetAttributeAsDword(x)
#define GetAFloat(x)	GetAShip()->GetAttributeAsFloat(x)

	// standard values
	SP.iClass = GetADword("Class");
	SP.fSpeedRate = GetAFloat("SpeedRate");
	SP.fTurnRate = GetAFloat("TurnRate");
	SP.iCrewMin = GetADword("MinCrew");
	SP.iCrewMax = GetADword("MaxCrew");
	SP.iCapacity = GetADword("Capacity");
	SP.fWeight = GetAFloat("Weight");

	// depend values
	SP.fSpeedDependW = GetAFloat("SpeedDependWeight");
	SP.fSubSeaDependW = GetAFloat("SubSeaDependWeight");
	SP.fTurnDependW = GetAFloat("TurnDependWeight");
	SP.fTurnDependS = GetAFloat("TurnDependSpeed");

	SP.fWaterLine = GetAFloat("WaterLine");

	State.vInertiaAccel.y = GetAFloat("InertiaAccelerationY");
	State.vInertiaBrake.y = GetAFloat("InertiaBrakingY");
	State.vInertiaAccel.x = GetAFloat("InertiaAccelerationX");
	State.vInertiaBrake.x = GetAFloat("InertiaBrakingX");
	State.vInertiaAccel.z = GetAFloat("InertiaAccelerationZ");
	State.vInertiaBrake.z = GetAFloat("InertiaBrakingZ");
	
	return true;
}

float SHIP::Trace(const CVECTOR &src, const CVECTOR &dst) 
{ 
	MODEL * pModel = GetModel(); Assert(pModel);
	return pModel->Trace(src,dst);
};

float SHIP::Cannon_Trace(long iBallOwner, const CVECTOR &vSrc, const CVECTOR &vDst) 
{ 
	MODEL * pModel = GetModel(); Assert(pModel);
	
	long iOurIndex = GetIndex(GetACharacter());
	if (iBallOwner == iOurIndex) return 2.0f;

	for (long i=0; i<iNumMasts; i++) if (!pMasts[i].bBroken)
	{
		mast_t * pM = &pMasts[i];
		float fRes = pM->pNode->Trace(vSrc, vDst);

		if (fRes <= 1.0f)
		{
			CVECTOR v1 = vSrc + fRes * (vDst - vSrc);
			VDATA * pV = api->Event(SHIP_MAST_DAMAGE, "llffffaa", SHIP_MAST_TOUCH_BALL, pM->iMastNum, v1.x, v1.y, v1.z, pM->fDamage, GetACharacter(), iBallOwner);
			pM->fDamage = Clamp(pV->GetFloat());
			MastFall(pM);
		}
	}

	float fRes = pModel->Trace(vSrc,vDst);
	if (fRes<=1.0f)
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
		api->Event(SHIP_HULL_HIT, "illffflf", GetID(), iBallOwner, iOurIndex, vTemp.x, vTemp.y, vTemp.z, iBestIndex, fMinDistance);
		api->Send_Message(blots_id, "lffffff", MSG_BLOTS_HIT, vTemp.x, vTemp.y, vTemp.z, vDir.x, vDir.y, vDir.z);
	}
	return fRes; 
}

dword SHIP::AttributeChanged(ATTRIBUTES * pAttribute)
{
	return 0;
}

CVECTOR		SHIP::GetBoxSize()				{ return State.vBoxSize; };
ENTITY_ID	SHIP::GetModelEID()				{ return model_id; }
MODEL *		SHIP::GetModel()				{ Assert(api->ValidateEntity(&GetModelEID())); return (MODEL*)api->GetEntityPointer(&GetModelEID()); }
CMatrix *	SHIP::GetMatrix()				{ return &GetModel()->mtx; }
void		SHIP::SetMatrix(CMatrix & mtx)	{ GetModel()->mtx = mtx; }
CVECTOR		SHIP::GetAng()					{ return State.vAng; }
CVECTOR		SHIP::GetPos()					{ return CVECTOR(State.vPos.x+fXOffset,State.vPos.y,State.vPos.z+fZOffset); }
void		SHIP::SetPos(CVECTOR & vNewPos) 
{ 
	State.vPos = vNewPos;
	if (pSea)
		State.vPos.y = pSea->WaveXZ(State.vPos.x, State.vPos.z);
	fXOffset = fZOffset = 0.f;
	RecalculateWorldOffset();
}

void		SHIP::SetSpeed(float fSpeed)	{ fSailState = fSpeed; }
float		SHIP::GetSpeed()				{ return fSailState; }

void		SHIP::SetRotate(float fRotSpd)	{ Strength[0].vRotate.y = fRotSpd; }
float		SHIP::GetRotate()				{ return Strength[0].vRotate.y; }

float		SHIP::GetCurrentSpeed()			{ return KNOTS2METERS(State.vSpeed.z); }

void SHIP::SetACharacter(ATTRIBUTES * pAP) 
{ 
	VAI_OBJBASE::SetACharacter(pAP);

	if( pAP && pAP->GetAttributeAsDword("index",-1)>=0 )
	{
		VDATA* pVDat = (VDATA*)api->GetScriptVariable("Characters");
		if( pVDat )
			pVDat->Set(GetID(),pAP->GetAttributeAsDword("index",0));
	}

	if (bMounted)
	{
		api->DeleteEntity(blots_id);
		api->CreateEntity(&blots_id,"blots");
		api->Send_Message(blots_id, "lia", MSG_BLOTS_SETMODEL, GetModelEID(), GetACharacter()); 
		api->LayerAdd((char *)sRealizeLayer.GetBuffer(), blots_id, iShipPriorityRealize + 4);
		api->LayerAdd((char *)sExecuteLayer.GetBuffer(), blots_id, iShipPriorityExecute + 4);
	}
}

void SHIP::Save(CSaveLoad * pSL)
{
	dword i;

	pSL->SaveAPointer("character", GetACharacter());
	pSL->SaveAPointer("ship", pAShip);
	pSL->SaveString(sRealizeLayer);
	pSL->SaveString(sExecuteLayer);
	pSL->SaveString(string(cShipIniName));
	pSL->SaveLong(iShipPriorityExecute);
	pSL->SaveFloat(fGravity);
	pSL->SaveFloat(fSailState);

	pSL->SaveLong(uniIDX);
	pSL->SaveDword(bUse);
	pSL->SaveBuffer((const char *)&ShipPoints[0][0], sizeof(ShipPoints));
	pSL->SaveVector(vSpeedAccel);
	pSL->SaveBuffer((const char *)&SP, sizeof(SP));
	pSL->SaveVector(CVECTOR(vPos.x+fXOffset,vPos.y,vPos.z+fZOffset));
	pSL->SaveVector(vAng);
	pSL->SaveFloat(fWaterLine);
	pSL->SaveDword(bDead);
	pSL->SaveDword(bVisible);
	pSL->SaveVector(vDeadDir);
	pSL->SaveVector(vCurDeadDir);
	pSL->SaveBuffer((const char *)&vKeelContour[0], sizeof(vKeelContour));
	pSL->SaveDword(bShip2Strand);
	pSL->SaveDword(bMounted);
	pSL->SaveDword(bKeelContour);
	pSL->SaveDword(bPerkTurnActive);
	pSL->SaveFloat(fInitialPerkAngle);
	pSL->SaveFloat(fResultPerkAngle);
	pSL->SaveBuffer((const char *)&Strength[0], sizeof(Strength));
	pSL->SaveDword(bSetLightAndFog);
	pSL->SaveDword(dwSaveAmbient);
	pSL->SaveDword(dwSaveFogColor);
	pSL->SaveBuffer((const char *)&saveLight, sizeof(saveLight));
	pSL->SaveBuffer((const char *)&State, sizeof(State));

	pSL->SaveLong(iNumMasts);
	for (i=0; i<(dword)iNumMasts; i++) 
	{
		pSL->SaveVector(pMasts[i].vSrc);
		pSL->SaveVector(pMasts[i].vDst);
		pSL->SaveLong(pMasts[i].iMastNum);
		pSL->SaveDword(pMasts[i].bBroken);
		pSL->SaveFloat(pMasts[i].fDamage);
	}

	pSL->SaveDword(aFirePlaces.Size());
	for (i=0; i<aFirePlaces.Size(); i++) aFirePlaces[i].Save(pSL);	
}

void SHIP::Load(CSaveLoad * pSL)
{
	dword i;

	SetACharacter(pSL->LoadAPointer("character"));
	pAShip = pSL->LoadAPointer("ship");

	sRealizeLayer = pSL->LoadString();
	sExecuteLayer = pSL->LoadString();
	string sTmp = pSL->LoadString();
	strcpy(cShipIniName, sTmp.GetBuffer());
	pSL->LoadLong();
	fGravity = pSL->LoadFloat();
	fSailState = pSL->LoadFloat();

	Mount(pAShip);

	uniIDX = pSL->LoadLong();
	bUse = pSL->LoadDword() != 0;
	pSL->Load2Buffer((char *)&ShipPoints[0][0]);
	vSpeedAccel = pSL->LoadVector();
	pSL->Load2Buffer((char *)&SP);
	vPos = pSL->LoadVector();
	vAng = pSL->LoadVector();
	fWaterLine = pSL->LoadFloat();
	bDead = pSL->LoadDword() != 0;

	State.vPos = vPos;
	State.vAng = vAng;
	fXOffset = fZOffset = 0.f;
	RecalculateWorldOffset();
	if (bDead) { bDead = false; SetDead(); }

	bVisible = pSL->LoadDword() != 0;
	vDeadDir = pSL->LoadVector();
	vCurDeadDir = pSL->LoadVector();
	pSL->Load2Buffer((char *)&vKeelContour[0]);
	bShip2Strand = pSL->LoadDword() != 0;
	bMounted = pSL->LoadDword() != 0;
	bKeelContour = pSL->LoadDword() != 0;
	bPerkTurnActive = pSL->LoadDword() != 0;
	fInitialPerkAngle = pSL->LoadFloat();
	fResultPerkAngle = pSL->LoadFloat();
	pSL->Load2Buffer((char *)&Strength[0]);
	bSetLightAndFog = pSL->LoadDword() != 0;
	dwSaveAmbient = pSL->LoadDword();
	dwSaveFogColor = pSL->LoadDword();
	pSL->Load2Buffer((char *)&saveLight);
	pSL->Load2Buffer((char *)&State);

	iNumMasts = pSL->LoadLong();
	//pMasts = NEW mast_t[iNumMasts];
	for (i=0; i<(dword)iNumMasts; i++) 
	{
		pMasts[i].vSrc = pSL->LoadVector();
		pMasts[i].vDst = pSL->LoadVector();
		pMasts[i].iMastNum = pSL->LoadLong();
		pMasts[i].bBroken = pSL->LoadDword() != 0;
		pMasts[i].fDamage = pSL->LoadFloat();
	}

	dword dwNum = pSL->LoadDword();
	for (i=0; i<dwNum; i++) 
	{
		aFirePlaces[i].Load(pSL);	
		if (aFirePlaces[i].isActive()) api->Event(SHIP_LOAD_SHIPACTIVATEFIREPLACE, "lllf", GetIndex(GetACharacter()), aFirePlaces[i].GetBallCharacterIndex(), i, aFirePlaces[i].GetRunTime());
	}

	ZERO(ShipPoints);
}

void SHIP::SetFixedSpeed(bool _bSetFixed, float _fFixedSpeed)
{
	bSetFixed = _bSetFixed;
	fFixedSpeed = _fFixedSpeed;
}