#include <stdio.h>
#include "waterflare.h"

#define WATERFLARE_DIR		"WEATHER\\SEA\\FLARE\\"

WATERFLARE::WATERFLARE()
{
	GUARD(WATERFLARE::WATERFLARE())
	iFlareTex = -1;
	iFlaresNum = 0;
	RS = null;
	pWeather = null;
	pfAlpha = null;
	pRSRect = null;
	UNGUARD
}

WATERFLARE::~WATERFLARE()
{
	GUARD(WATERFLARE::~WATERFLARE())
	DELETE(pRSRect);
	DELETE(pfAlpha);
	UNGUARD
}

bool WATERFLARE::Init()
{
	GUARD(bool WATERFLARE::Init())

	_CORE_API->LayerAdd("realize",GetID(),-1);
	_CORE_API->LayerAdd("execute",GetID(),-1);
	
	SetDevice();

	UNGUARD
	return true;
}

void WATERFLARE::SetDevice()
{
	GUARD(void WATERFLARE::SetDevice())

	RS = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if (!RS) _THROW("No service: dx8render");

	ENTITY_ID	ent;
	if (!_CORE_API->FindClass(&ent,"Weather",0)) _THROW("No found WEATHER entity!");
	pWeather = (WEATHER_BASE*)_CORE_API->GetEntityPointer(&ent);

	UNGUARD
}

bool WATERFLARE::CreateState(ENTITY_STATE_GEN * state_gen)
{
	GUARD(bool WATERFLARE::CreateState(ENTITY_STATE_GEN * state_gen))
	UNGUARD
	return true;
}

bool WATERFLARE::LoadState(ENTITY_STATE * state)
{
	GUARD(bool WATERFLARE::LoadState(ENTITY_STATE * state))
	UNGUARD
	return true;
}

void WATERFLARE::Execute(dword Delta_Time)
{
	GUARD(void WATERFLARE::Execute(dword Delta_Time))

	/*if (pWeather->GetLong(whi_weather_update)) 
	{
		char str[256];
		// create texture
		sprintf(str,"%swaterflare.tga",WATERFLARE_DIR);
		if (iFlareTex>0) RS->TextureRelease(iFlareTex);
		iFlareTex = RS->TextureCreate(str);
		GenerateFlares();
	}*/

	UNGUARD
}

void WATERFLARE::GenerateFlares()
{
	iFlaresNum = 1024 + (rand()%64);
	pRSRect = (RS_RECT*)NEW RS_RECT[iFlaresNum];
	pfAlpha = (float*)NEW float[iFlaresNum];
	for (long i=0;i<iFlaresNum;i++)
	{
		pfAlpha[i] = FRAND(-40.0f);
		pRSRect[i].vPos = CVECTOR(FRAND(1000.0f),0.0f,FRAND(1000.0f));
		pRSRect[i].fAngle = 0.0f;
		pRSRect[i].dwSubTexture = 0;
		pRSRect[i].fSize = 0.5f;
	}
}

void WATERFLARE::Realize(dword Delta_Time)
{
	GUARD(void WATERFLARE::Realize(dword Delta_Time))

	for (long i=0;i<iFlaresNum;i++)
	{
		float fDeltaTime = float(Delta_Time) * 0.001f;
		pfAlpha[i] += fDeltaTime;
		if (pfAlpha[i]>2.0f)
		{
			pfAlpha[i] = 0.0f;
			pRSRect[i].vPos = CVECTOR(FRAND(200.0f),0.0f,FRAND(200.0f));
		}
		dword dwAlpha = dword(255.0f * ((pfAlpha[i]>1.0f) ? 2.0f - pfAlpha[i] : pfAlpha[i]));
		pRSRect[i].dwColor = RGB(dwAlpha,dwAlpha,dwAlpha);
	}

	RS->TextureSet(0,iFlareTex);
	RS->DrawRects(pRSRect,iFlaresNum,"waterflare");

	UNGUARD
}

void WATERFLARE::ProcessMessage(dword iMsg,dword wParam,dword lParam)
{
	GUARD(void WATERFLARE::ProcessMessage(dword iMsg,dword wParam,dword lParam))
	UNGUARD
}
