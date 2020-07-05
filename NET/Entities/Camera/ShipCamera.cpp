#include "ShipCamera.h"

#define SCMR_BOXSCALE_X		1.6f
#define SCMR_BOXSCALE_Y		1.3f
#define SCMR_BOXSCALE_Z		1.4f

NetShipCamera::NetShipCamera()
{
	SetOn(false);
	SetActive(false);

	ZERO(vAng);
	pSea = null;
	pIsland = null;

	fDistanceDlt = 0.0f;
	fAngleXDlt = fAngleYDlt = 0.0f;
	fModelAy = 0.0f;

	fDistanceInertia = 15.0f;
	fAngleXInertia = 12.0f;
	fAngleYInertia = 10.0f;
}

NetShipCamera::~NetShipCamera()
{
}

bool NetShipCamera::Init()
{
	api->SystemMessages(GetID(),true);

	iLockX = 0;
	iLockY = 0;
	
	SetDevices();

	return true;
}

void NetShipCamera::SetDevices()
{
	ENTITY_ID sea_id;
	if (NetFindClass(IsServer(), &sea_id, "NetSea")) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);
}

void NetShipCamera::Execute(DWORD dwDeltaTime)
{
	SetNetID((word)AttributesPointer->GetAttributeAsDword("NetID"));

	if (!isOn()) return;
	if (!FindShip()) return;

	SetPerspective(AttributesPointer->GetAttributeAsFloat("Perspective"));

	float fDeltaTime = 0.001f * float(api->GetDeltaTime());

	MODEL *pModel = GetModelPointer();
	Assert(pModel);
	CMatrix *mtx = &pModel->mtx;
	vCenter = mtx->Pos();
	
	fModelAy = float(atan2(mtx->Vz().x, mtx->Vz().z));

	Move(fDeltaTime);
}

void NetShipCamera::Move(float fDeltaTime)
{
	if (!pSea) return;
	if (!isActive()) return;

	NetShip * pShip = GetShip();

	float fSpeed = fDeltaTime;
	float fTempHeight = 0.0f;

	CONTROL_STATE cs;
	
	//Distance
	float fSensivityDistanceDlt = 0.0f;
	api->Controls->GetControlState("ShipCamera_Forward",cs);
	if(cs.state == CST_ACTIVE) 
		fSensivityDistanceDlt -= fSensivityDistance;
	api->Controls->GetControlState("ShipCamera_Backward",cs);
	if(cs.state == CST_ACTIVE) 
		fSensivityDistanceDlt += fSensivityDistance;
	
	float fKInert = fDistanceInertia * fSpeed;
	if(fKInert < 0.0f) fKInert = 0.0f;
	if(fKInert > 1.0f) fKInert = 1.0f;
	fSensivityDistance = 1.0f;
	fDistanceDlt += (fSensivityDistanceDlt - fDistanceDlt) * fKInert;
	fDistance += fSpeed * fDistanceDlt;
	if(fDistance > 1.0f) fDistance = 1.0f;
	if(fDistance < 0.0f) fDistance = 0.0f;

	//Rotate
	api->Controls->GetControlState("ShipCamera_Turn_H",cs);

	float fValue = fInvertMouseX * 2.0f * (cs.fValue) * fSensivityAzimuthAngle;
	fKInert = fAngleYInertia * fSpeed;
	if(fKInert < 0.0f) fKInert = 0.0f;
	if(fKInert > 1.0f) fKInert = 1.0f;
	fAngleYDlt += (fValue - fAngleYDlt) * fKInert;
	vAng.y += fSpeed * fAngleYDlt;

	api->Controls->GetControlState("ShipCamera_Turn_V",cs);
	
	fValue = fInvertMouseY * 3.0f * (cs.fValue) * fSensivityHeightAngle;
	fKInert = fAngleXInertia * fSpeed;
	if(fKInert < 0.0f) fKInert = 0.0f;
	if(fKInert > 1.0f) fKInert = 1.0f;
	fAngleXDlt += (fValue - fAngleXDlt) * fKInert;
	vAng.x += fSpeed * fAngleXDlt;

	if (vAng.x < -2.0f*PI) vAng.x += 2.0f*PI;
	if (vAng.x > 2.0f*PI) vAng.x -= 2.0f*PI;
	if (vAng.x < fMinAngleX) vAng.x = fMinAngleX;
	if (vAng.x > fMaxAngleX) vAng.x = fMaxAngleX;

	//Current distance
	CVECTOR boxSize = pShip->GetBoxSize()*CVECTOR(SCMR_BOXSCALE_X*0.5f, SCMR_BOXSCALE_Y*0.5f, SCMR_BOXSCALE_Z*0.5f);
	boxSize.x += boxSize.y;
	boxSize.z += boxSize.y;
	float maxRad = boxSize.z*2.0f;
	//Полуоси эллипсоида по которому движеться камера
	float a = boxSize.x*1.2f + fDistance*(maxRad - boxSize.x*1.2f);			//x
	float b = boxSize.y*1.5f + fDistance*(70.0f - boxSize.y*1.5f);			//y
	float c = boxSize.z*1.2f + fDistance*(maxRad - boxSize.z*1.2f);			//z
	//Найдём позицию камеры на эллипсоиде
	vCenter.y += 0.5f*boxSize.y;
	CVECTOR vPos;
	if(vAng.x <= 0.0f)
	{
		//Выше 0 ездием по эллипсоиду
		vPos.x = a*cosf(-vAng.x)*sinf(vAng.y);
		vPos.y = b*sinf(-vAng.x);
		vPos.z = c*cosf(-vAng.x)*cosf(vAng.y);
	}else{
		//Ниже 0 ездием по эллиптическому цилиндру
		vPos.x = a*sinf(vAng.y);
		vPos.y = 0.0f;//b*sinf(-vAng.x);
		vPos.z = c*cosf(vAng.y);
	}
	vPos = CMatrix(CVECTOR(0.0f, fModelAy, 0.0f), vCenter)*vPos;
	if(vAng.x > 0.0f) vCenter.y += boxSize.z*vAng.x*6.0f;
	//Ограничим высоту с низу
	float fWaveY = pSea->WaveXZ(vPos.x,vPos.z);
	if(vPos.y - fWaveY < fMinHeightOnSea) vPos.y = fWaveY + fMinHeightOnSea;
	float oldPosY = vPos.y;
	//Ships collision
	ShipsCollision(vPos);
	//Island collision
	if(IslandCollision(vPos))
	{
		ShipsCollision(vPos);
		IslandCollision(vPos);
	}
	if(vPos.y > oldPosY) vCenter.y += vPos.y - oldPosY;
	//Set new camera
	vCenter.y += fDistance * 2.0f * boxSize.y;

	Render().SetCamera(vPos, vCenter, CVECTOR(0.0f,1.0f,0.0f));
	Render().SetPerspective(GetPerspective());
}

void NetShipCamera::Realize(dword dwDeltaTime)
{
}

dword NetShipCamera::AttributeChanged(ATTRIBUTES * pAttr)
{
	if (*pAttr == "SensivityDistance")			fSensivityDistance = pAttr->GetAttributeAsFloat();
	if (*pAttr == "SensivityAzimuthAngle")		fSensivityAzimuthAngle = pAttr->GetAttributeAsFloat();
	if (*pAttr == "SensivityHeightAngle")		fSensivityHeightAngle = pAttr->GetAttributeAsFloat();
	if (*pAttr == "SensivityHeightAngleOnShip") fSensivityHeightAngleOnShip = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MaxAngleX")					fMaxAngleX = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MinAngleX")					fMinAngleX = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MaxHeightOnShip")			fMaxHeightOnShip = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MinHeightOnSea")				fMinHeightOnSea = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MaxDistance")				fMaxDistance = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MinDistance")				fMinDistance = pAttr->GetAttributeAsFloat();
	if (*pAttr == "Distance")					fDistance = pAttr->GetAttributeAsFloat();
	if (*pAttr == "InvertMouseX")				fInvertMouseX = pAttr->GetAttributeAsFloat();
	if (*pAttr == "InvertMouseY")				fInvertMouseY = pAttr->GetAttributeAsFloat();

	if (*pAttr == "On")							SetOn(pAttr->GetAttributeAsDword() != 0);
	if (*pAttr == "Active")						SetActive(pAttr->GetAttributeAsDword() != 0);

	return 0;
}

void NetShipCamera::ShipsCollision(CVECTOR & pos)
{
	ENTITY_ID id;

	NetShip * pOurShip = GetShip();

	bool res = NetFindClass(IsServer(), &id, "NetShip");
	if(!res) return;
	CVECTOR p;
	for(; res; res = NetFindClassNext(IsServer(), &id))
	{
		//Указатель на объект
		NetShip * ship = (NetShip*)_CORE_API->GetEntityPointer(&id);
		if(!ship) break;
		if(ship == pOurShip) continue;
		//Позиция камеры в системе корабля
		Assert(ship->GetMatrix());
		ship->GetMatrix()->MulToInv(pos, p);
		//Проверим попадание в бокс
		CVECTOR s = ship->GetBoxSize()*CVECTOR(SCMR_BOXSCALE_X*0.5f, SCMR_BOXSCALE_Y*0.5f, SCMR_BOXSCALE_Z*0.5f);
		if(s.x <= 0.0f || s.y <= 0.0f || s.z <= 0.0f) continue;
		//Строим эллипсоид
		float a = s.z + s.y;						//z
		float b = s.x + s.y;						//x
		float k1 = s.z/a;
		float k2 = s.x/b;
		float c = s.y/sqrtf(1.0f - k1*k1 - k2*k2);	//y
		//Ишем высоту
		k1 = p.z/a;
		k2 = p.x/b;
		float h = (1.0f - k1*k1 - k2*k2);
		if(h <= 0.0f) continue;
		h = b*b*h;									//^2
		h = sqrtf(h);
		if(h > s.y) h = s.y*(1.0f + 0.1f*(h - s.y)/(c - s.y));
		if(p.y < h) p.y = h;
		s = ship->GetMatrix()[0]*p;
		if(pos.y < s.y) pos.y = s.y;
	}
}

bool NetShipCamera::IslandCollision(CVECTOR & pos)
{
	const float camRadius = 0.4f;
	//Island
	if (pIsland == null)
	{
		ENTITY_ID island_id;
		if (NetFindClass(IsServer(), &island_id, "NetIsland")) pIsland = (ISLAND_BASE*)api->GetEntityPointer(&island_id);
		if(pIsland == null) return false;
	}
	//Model
	MODEL * mdl = (MODEL*)api->GetEntityPointer(&pIsland->GetModelEID());
	if(mdl == null) return false;
	//Find direction, distance
	CVECTOR dir = pos - vCenter;
	float dist = ~dir;
	if(dist <= 0.0f) return false;
	dist = sqrtf(dist); dir *= 1.0f/dist;
	CVECTOR dr = dir*(dist + camRadius);
	//First check
	float k[5];
	k[0] = mdl->Trace(vCenter,vCenter + dr);
	//Basis
	CVECTOR left = dir ^ CVECTOR(0.0f, 1.0f, 0.0f);
	float l = ~left;
	if(l <= 0.0f)
	{
		if(k[0] < 1.0f) pos = vCenter + (pos - vCenter)*k[0] - dir*camRadius;
		return k[0] < 1.0f;
	}
	left *= 1.0f/sqrtf(l);
	CVECTOR up = dir ^ left;
	//Find nearest distanse	
	CVECTOR src;
	src = vCenter + left*camRadius;
	k[1] = mdl->Trace(src, src + dr);
	src = vCenter - left*camRadius;
	k[2] = mdl->Trace(src, src + dr);
	src = vCenter + up*camRadius;
	k[3] = mdl->Trace(src, src + dr);
	src = vCenter - up*camRadius;
	k[4] = mdl->Trace(src, src + dr);
	float kRes = 2.0f;
	for(long i = 0; i < 5; i++) if(kRes > k[i]) kRes = k[i];
	if(kRes < 1.0f) pos = vCenter + (pos - vCenter)*kRes - dir*camRadius;
	return kRes < 1.0f;
}

