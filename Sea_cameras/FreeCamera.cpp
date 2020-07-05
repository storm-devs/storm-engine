#include "FreeCamera.h"
#include "..\common_h\collide.h"
#include "..\model\modelr.h"

#define SENSITIVITY		0.0015f
#define FOV				1.285f

ENTITY_ID	sphere;
COLLIDE		*pCollide;

FREE_CAMERA::FREE_CAMERA()
{
	SetOn(false);
	SetActive(false);

	pIslandBase = null;
	pRS = null;
	ZERO2(vPos,vAng);
	vPos.z = 250.0f;
	vPos.y = 3.0f;
	fFov = FOV;

	iLockX = 0;
	iLockY = 0;

	fCameraOnEarthHeight = 3.0f;
	bCameraOnEarth = false;
}

FREE_CAMERA::~FREE_CAMERA()
{
}

bool FREE_CAMERA::Init()
{
	GUARD(FREE_CAMERA::FREE_CAMERA())
	_CORE_API->LayerCreate("realize",true,false);
	_CORE_API->LayerAdd("system_messages",GetID(),1);
	SetDevice();
	UNGUARD
	return true;
}

void FREE_CAMERA::SetDevice()
{
	pRS = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	Assert(pRS);
	pCollide		= (COLLIDE*)_CORE_API->CreateService("COLL");
	Assert(pCollide);

	/*_CORE_API->CreateEntity(&sphere,"modelr");
	_CORE_API->Send_Message(sphere,"ls",MSG_MODEL_LOAD_GEO,"mirror");
	_CORE_API->LayerAdd("realize",sphere,10000);*/
}

bool FREE_CAMERA::CreateState(ENTITY_STATE_GEN * state_gen)
{
	state_gen->SetState("vv",sizeof(vPos),vPos,sizeof(vAng),vAng);
	return true;
}

bool FREE_CAMERA::LoadState(ENTITY_STATE * state)
{
	SetDevice();
	state->Struct(sizeof(vPos),(char *)&vPos);
	state->Struct(sizeof(vAng),(char *)&vAng);
	return true;
}

void FREE_CAMERA::Execute(dword Delta_Time)
{
	if (!isOn()) return;

	SetPerspective(AttributesPointer->GetAttributeAsFloat("Perspective"));

	ENTITY_ID ent;
	if (!pIslandBase && _CORE_API->FindClass(&ent,"island",0))
		pIslandBase = (ISLAND_BASE*)_CORE_API->GetEntityPointer(&ent);

	Move(_CORE_API->GetDeltaTime());
}

void FREE_CAMERA::Move(DWORD DeltaTime)
{
	if (!isActive()) return;
	if(LOWORD(GetKeyState(VK_NUMLOCK)) != 0) return;

	//POINT pnt;
	//GetCursorPos(&pnt);
	//if(pnt.x != iLockX || pnt.y != iLockY)
	CONTROL_STATE cs;

	{
		_CORE_API->Controls->GetControlState("FreeCamera_Turn_H",cs);
		vAng.y += SENSITIVITY*(float)(cs.fValue);
		_CORE_API->Controls->GetControlState("FreeCamera_Turn_V",cs);
		vAng.x += SENSITIVITY*(float)(cs.fValue);
		//SetCursorPos(iLockX,iLockY);
	}
	if (bCameraOnEarth && pIslandBase) 
	{
		pIslandBase->GetDepth(vPos.x,vPos.z,&vPos.y);
		vPos.y += fCameraOnEarthHeight;
	}
	float c0 = cosf(vAng.y);	float s0 = sinf(vAng.y);
	float c1 = cosf(vAng.x);	float s1 = sinf(vAng.x);
	float c2 = cosf(vAng.z);	float s2 = sinf(vAng.z);
	float speed = 5.0f * 0.001f * float(DeltaTime);
#ifndef _XBOX
	if (GetAsyncKeyState(VK_SHIFT))		speed *= 4.0f;
	if (GetAsyncKeyState(VK_CONTROL))	speed *= 8.0f;
#endif
	_CORE_API->Controls->GetControlState("FreeCamera_Forward",cs);
	if(cs.state == CST_ACTIVE) vPos += speed*CVECTOR(s0*c1, -s1, c0*c1);
	_CORE_API->Controls->GetControlState("FreeCamera_Backward",cs);
	if(cs.state == CST_ACTIVE) vPos -= speed*CVECTOR(s0*c1, -s1, c0*c1);

	/*if (GetAsyncKeyState(VK_LBUTTON))	vPos += speed*CVECTOR(s0*c1, -s1, c0*c1);
	if (GetAsyncKeyState(VK_RBUTTON))	vPos -= speed*CVECTOR(s0*c1, -s1, c0*c1);
	if(GetAsyncKeyState('I'))	vPos += speed*CVECTOR(0.0f, 0.1f , 0.0f);
	if(GetAsyncKeyState('K'))	vPos += speed*CVECTOR(0.0f, -0.1f, 0.0f);*/
	
	//vPos = CVECTOR(0.0f, 20.0f, 0.0f);

	pRS->SetCamera(&vPos,&vAng,GetPerspective());

	return;

	CVECTOR vRes;
	CVECTOR vDst = vPos + 2000.0f*CVECTOR(s0*c1, -s1, c0*c1);

	VIDWALKER *pVW = _CORE_API->LayerGetWalker("sun_trace");
	float fRes = pCollide->Trace(*pVW,vPos,vDst,0,0);
	if (fRes > 1.0f) vRes = vDst;
	else 
	{
		vRes = vPos + fRes * (vDst - vPos);
		ENTITY_ID ent = pCollide->GetObjectID();
		MODELR *pEntity = (MODELR*)_CORE_API->GetEntityPointer(&ent);
	}


	MODEL* pModel = (MODEL*)_CORE_API->GetEntityPointer(&sphere);
	pModel->mtx.BuildPosition(vRes.x,vRes.y,vRes.z);
	delete pVW;
}

void FREE_CAMERA::Save(CSaveLoad * pSL)
{
	pSL->SaveVector(vPos);
	pSL->SaveVector(vAng);
	pSL->SaveFloat(fFov);
	pSL->SaveLong(iLockX);
	pSL->SaveLong(iLockY);

	pSL->SaveDword(bCameraOnEarth);
	pSL->SaveFloat(fCameraOnEarthHeight);
}

void FREE_CAMERA::Load(CSaveLoad * pSL)
{
	vPos = pSL->LoadVector();
	vAng = pSL->LoadVector();
	fFov = pSL->LoadFloat();
	iLockX = pSL->LoadLong();
	iLockY = pSL->LoadLong();

	bCameraOnEarth = pSL->LoadDword() != 0;
	fCameraOnEarthHeight = pSL->LoadFloat();
}
