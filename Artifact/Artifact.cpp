#include "..\common_h\rands.h"
#include "Artifact.h"

INTERFACE_FUNCTION
CREATE_CLASS(Artifact)

CMatrix cameraMatrix;
//------------------------------------------------------------------------------------
Artifact::Artifact()
	:particles(0)
	,myShip(0)
	,time(0)
	,rotationZ(0.f)
{
}

//------------------------------------------------------------------------------------
Artifact::~Artifact()
{
	renderService->LightEnable(1, false);
	delete particles;

	if (iBuffer)
		renderService->ReleaseIndexBuffer(iBuffer);
	if (vBuffer)
		renderService->ReleaseVertexBuffer(vBuffer);
}

//------------------------------------------------------------------------------------
bool Artifact::Init()
{
	GUARD(Artifact::Init())

	//main entities
	ENTITY_ID seaID;
	_CORE_API->FindClass(&seaID, "sea", 0);
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);
	if (!sea)
		return false;

	renderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!renderService)	_THROW("No service: dx8render");

	//particles
	INIFILE *psIni = _CORE_API->fio->OpenIniFile("resource\\ini\\particles\\artifact.ini");
	particles = NEW BSPS_PS();
	particles->Init(psIni, "artifact");
	delete psIni;

	//find my ship
	ENTITY_ID shipID;
	if (api->FindClass(&shipID, "ship", 0))
	{
		do
		{
			SHIP_BASE *ship = (SHIP_BASE *) api->GetEntityPointer(&shipID);
			if (ship && ship->GetACharacter()->GetAttribute("MainCharacter"))
			{
				myShip = ship;
				break;
			}
		}
		while (api->FindClassNext(&shipID));
	}
	texture = renderService->TextureCreate("plasma.tga");
	iBuffer = renderService->CreateIndexBuffer(6*sizeof(WORD), D3DUSAGE_WRITEONLY);
	vBuffer = renderService->CreateVertexBuffer(ARTIFACT_FVF,4*sizeof(ARTIFACT_VERTEX), D3DUSAGE_WRITEONLY);

	
	WORD *indexes = (WORD *) renderService->LockIndexBuffer(iBuffer); 
	*(indexes++) = 0;
	*(indexes++) = 1;
	*(indexes++) = 2;

	*(indexes++) = 0;
	*(indexes++) = 2;
	*(indexes++) = 3;
	renderService->UnLockIndexBuffer(iBuffer);

	return true;
	UNGUARD
}

//------------------------------------------------------------------------------------
void Artifact::Realize(dword _dTime)
{
	if (!sea)
		return;

	pos = myShip->GetModel()->mtx * CVECTOR(0.f, myShip->GetBoxSize().y / 1.5f, -myShip->GetBoxSize().z / 2.5f);
	renderService->GetTransform(D3DTS_VIEW,cameraMatrix);

	time += _dTime;
	//calc particles
	//CVECTOR pos = myShip->GetPos();
	//pos.y += emitterDelta.y;

	//light
	static float lightK1 = 0.5f;
	static float lightKOld = 0.5f;
	dword LIGHT_CHANGE = 300;
	static dword secTime = 1000;
	secTime += _dTime;
	if (secTime >= LIGHT_CHANGE)
	{
		lightKOld = lightK1;
		lightK1 = 1.f / (randUpper(3.f) + .1f);
		secTime = 0;
	}
	float lightK = lightK1 - (lightK1 - lightKOld)*(LIGHT_CHANGE - secTime) / LIGHT_CHANGE;
	//lightK += randCentered(0.02f);

	D3DLIGHT8 light;
	light.Type		= D3DLIGHT_POINT;
	light.Diffuse.r = 0.9f;
	light.Diffuse.g = 0.8f;
	light.Diffuse.b = 1.0f;
	light.Diffuse.a = 1.0f;
	light.Specular  = light.Diffuse;
	light.Ambient.r = 0.04f;
	light.Ambient.g = 0.04f;
	light.Ambient.b = 0.055f;
	light.Ambient.a = 0.0f;
	light.Position.x = pos.x;
	light.Position.y = pos.y;
	light.Position.z = pos.z;
	light.Direction.x = 0.f;
	light.Direction.y = 0.f;
	light.Direction.z = 0.f;
	light.Range		= 100.f;
	light.Falloff   = 1.f;
	light.Attenuation0 = lightK*1.8f;
	light.Attenuation1 = lightK*0.5f;
	light.Attenuation2 = 0.f;
	light.Theta = 0.f;
	light.Phi = 0.f;
	renderService->SetLight(1, &light);
	renderService->LightEnable(1, true);
	//set emitter
	particles->SetEmitter(pos, CVECTOR(0.f, 1.f, 0.f));
	pos = cameraMatrix * pos;

	//calc vBuffer, then draw it
	static float dU = 1.f / GRID_FRAMES;
	float u = ((time / GRID_FRAME_DELAY) % GRID_FRAMES)*dU;
	dword alpha = (dword) ((((float) (time % GRID_FRAME_DELAY)) / GRID_FRAME_DELAY) * 0xFF);
	dword vColor = (alpha << 24) | 0x00808080;
	/*
	if (api->Controls->GetDebugAsyncKeyState('Z') < 0)
	{
		vColor = (alpha << 24) | 0x00FFFFFF;
	}
	*/
	rotationZ += _dTime * 1e-4f;

	ARTIFACT_VERTEX *vertices = (ARTIFACT_VERTEX *) renderService->LockVertexBuffer(vBuffer); 
	vertices->pos.x =  - GRID_WIDTH / 2.f;
	vertices->pos.y =  + GRID_HEIGHT / 2.f;
	vertices->pos.z = 0.f;
	vertices->color = vColor;
	vertices->tu = u;
	vertices->tv = 0.f;
	vertices->tu2 = u + dU;
	vertices->tv2 = 0.f;
	++vertices;

	vertices->pos.x = - GRID_WIDTH / 2.f;
	vertices->pos.y = - GRID_HEIGHT / 2.f;
	vertices->pos.z = 0.f;
	vertices->color = vColor;
	vertices->tu = u;
	vertices->tv = 1.f;
	vertices->tu2 = u + dU;
	vertices->tv2 = 1.f;
	++vertices;

	vertices->pos.x = + GRID_WIDTH / 2.f;
	vertices->pos.y = - GRID_HEIGHT / 2.f;
	vertices->pos.z = 0.f;
	vertices->color = vColor;
	vertices->tu = u + dU;
	vertices->tv = 1.f;
	vertices->tu2 = u + dU*2;
	vertices->tv2 = 1.f;
	++vertices;

	vertices->pos.x = + GRID_WIDTH / 2.f;
	vertices->pos.y = + GRID_HEIGHT / 2.f;
	vertices->pos.z = 0.f;
	vertices->color = vColor;
	vertices->tu = u+dU;
	vertices->tv = 0.f;
	vertices->tu2 = u + dU*2;
	vertices->tv2 = 0.f;
	++vertices;
	renderService->UnLockVertexBuffer(vBuffer); 

	//CMatrix m;
	//renderService->SetTransform(D3DTS_WORLD, (D3DMATRIX *) m);
	renderService->TextureSet(0, texture);
	renderService->TextureSet(1, texture);

	CMatrix iMatrix;
	//CVECTOR CameraPos, CameraAng;
	//float Perspective;
	renderService->SetTransform(D3DTS_VIEW,iMatrix);
	iMatrix.BuildMatrix(CVECTOR(0.f, 0.f, rotationZ), pos);
	renderService->SetTransform(D3DTS_WORLD,iMatrix);

	renderService->DrawBuffer(vBuffer, 
						 sizeof(ARTIFACT_VERTEX), 
						 iBuffer, 
						 0, 
						 4, 
						 0, 
						 2, 
						 "Artifact");

	iMatrix.BuildMatrix(CVECTOR(0.f, 0.f, -0.1f * rotationZ), pos);
	renderService->SetTransform(D3DTS_WORLD,iMatrix);

	renderService->DrawBuffer(vBuffer, 
						 sizeof(ARTIFACT_VERTEX), 
						 iBuffer, 
						 0, 
						 4, 
						 0, 
						 2, 
						 "Artifact");

  renderService->SetTransform(D3DTS_VIEW,cameraMatrix);
	//draw particles
	particles->Realize(_dTime);

/*
	if ((GetKeyState('Z') & 0x8000) != 0)
	{
		delete particles;
		INIFILE *psIni = _CORE_API->fio->OpenIniFile("resource\\ini\\particles\\artifact.ini");
		particles = NEW BSPS_PS();
		particles->Init(psIni, "artifact");
		delete psIni;
	}
*/
}


//------------------------------------------------------------------------------------
dword _cdecl Artifact::ProcessMessage(MESSAGE &message)
{
	return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
