//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Debris
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Debris.h"
#include "..\common_h\geometry.h"
#include "..\common_h\ship_base.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Debris::Debris(Pillar & _pillar) : pillar(_pillar)
{
	numModels = 0;
	flyCounter = 0;
	shipcode = _CORE_API->Class_Name2Code("ship");
	SetGlobalAlpha(1.0f);
	soundService = null;
	lastPlayTime = 0.0f;
}

Debris::~Debris()
{
	for(long i = 0; i < numModels; i++) _CORE_API->DeleteEntity(mdl[i].mdl->GetID());
}

void Debris::Init()
{
	AddModel("Tornado\\palka01", 500, 1.5f);
	AddModel("Tornado\\palka02", 400, 1.7f);
	AddModel("Tornado\\palka03", 100, 1.0f);
	AddModel("Tornado\\palka04", 500, 1.1f);
	AddModel("Tornado\\palka05", 25, 0.9f);
	AddModel("Tornado\\Flotsam1", 5, 0.6f);
	AddModel("Tornado\\Flotsam2", 1, 0.4f);
	AddModel("Tornado\\Flotsam3", 20, 0.8f);
	AddModel("Tornado\\Flotsam4", 15, 0.5f);
	AddModel("Tornado\\Flotsam5", 10, 0.85f);
	AddModel("Tornado\\Flotsam6", 5, 1.1f);
	AddModel("Tornado\\Flotsam7", 5, 1.2f);
	NormalazedModels();
	soundService = (VSoundService *)api->CreateService("SoundService");
}

void Debris::Update(float dltTime)
{
	if(numModels == 0) return;
	if(lastPlayTime > 0.0f) lastPlayTime -= dltTime;
	//Позиция на воде
	//Заведение
	if(flyCounter < sizeof(fly)/sizeof(ModelInfo))
	{
		if((rand() & 1) == 1)
		{
			//Проверим ближние корабли
			if(IsShip())
			{
				//Надо добавить новую модельку
				fly[flyCounter].mdl = SelectModel(fly[flyCounter].maxSpeed);
				fly[flyCounter].r = rand()*10.0f/RAND_MAX;
				fly[flyCounter].y = 0.0f + (rand() & 7);
				fly[flyCounter].a = rand()*2.0f*TRND_PI/RAND_MAX;
				fly[flyCounter].ay = rand()*(3.0f/RAND_MAX);
				fly[flyCounter].ang = 0.0f;
				fly[flyCounter].scale = 1.0f + (rand() & 3)/4.0f;
				flyCounter++;
				if(soundService)
				{
					if(lastPlayTime <= 0.0f)
					{
						soundService->SoundPlay("TornadoCrackSound", PCM_3D, VOLUME_FX, false, false, true, 0, &CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f)));
						lastPlayTime = 0.2f + rand()*(0.2f/RAND_MAX);
					}
				}
			}
		}
	}
	//Полёт
	float h = pillar.GetHeight();
	for(long i = 0; i < flyCounter; i++)
	{
		//Обновляем позицию по высоте
		fly[i].ay += dltTime*fly[i].maxSpeed;
		if(fly[i].ay > 20.0f*fly[i].maxSpeed) fly[i].ay = 20.0f*fly[i].maxSpeed;
		fly[i].y += dltTime*fly[i].ay;
		if(fly[i].y > h*0.75f)
		{
			fly[i] = fly[--flyCounter];
			continue;
		}
		//Обновляем радиус
		float k = dltTime*1.0f;
		if(k > 1.0f) k = 1.0f;
		fly[i].r += (pillar.GetRaduis(fly[i].y) - fly[i].r)*k;
		//Обновляем угл
		k = pillar.GetKHeight(fly[i].y) - 0.5f;
		k = 1.0f - k*k*4.0f;
		k = dltTime*(3.0f + 5.0f*k);
		fly[i].a += k;
		if(fly[i].a > 2.0f*TRND_PI) fly[i].a -= 2.0f*TRND_PI;
		fly[i].ang.y += k*1.01f;
		if(fly[i].ang.y > 2.0f*TRND_PI) fly[i].ang.y -= 2.0f*TRND_PI;
		fly[i].ang.x += dltTime*2.21f;
		if(fly[i].ang.x > 2.0f*TRND_PI) fly[i].ang.x -= 2.0f*TRND_PI;
		fly[i].ang.z += dltTime*3.37f;
		if(fly[i].ang.z > 2.0f*TRND_PI) fly[i].ang.z -= 2.0f*TRND_PI;
		//Прозрачность
		fly[i].alpha = 1.0f;
		if(fly[i].y < 3.0f) fly[i].alpha *= fly[i].y/3.0f;
		if(fly[i].y > h*0.5f) fly[i].alpha *= (fly[i].y - h*0.5f)/(h*0.75f - h*0.5f);
		if(fly[i].alpha < 0.0f) fly[i].alpha = 0.0f;
	}
}

void Debris::Draw(VDX8RENDER * rs)
{
	for(long i = 0; i < flyCounter; i++)
	{
		//Позиция модельки
		CVECTOR pos;
		pos.x = pillar.GetX(fly[i].y) + fly[i].r*sinf(fly[i].a);		
		pos.y = fly[i].y;
		pos.z = pillar.GetZ(fly[i].y) + fly[i].r*cosf(fly[i].a);
		fly[i].mdl->mtx.BuildMatrix(fly[i].ang, pos);
		for(long a = 0; a < 3; a++)
			for(long b = 0; b < 3; b++) fly[i].mdl->mtx.m[a][b] *= fly[i].scale;
		rs->SetRenderState(D3DRS_TEXTUREFACTOR, (long(fly[i].alpha*galpha) << 24) | 0xffffff);
		fly[i].mdl->Realize(10);
	}
}

void Debris::AddModel(const char * modelName, float prt, float spd)
{
	if(numModels >= sizeof(mdl)/sizeof(MODEL *)) return;
	//Создаём модельку
	ENTITY_ID id;
	if(!_CORE_API->CreateEntity(&id, "modelr")) return;
	MODEL * m = (MODEL *)_CORE_API->GetEntityPointer(&id);
	if(!m) return;
	//Путь для текстур
	VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	if(!gs) return;
	gs->SetTexturePath("Tornado\\");
	//Загружаем
	try
	{
		_CORE_API->Send_Message(id, 
								"ls", 
								MSG_MODEL_LOAD_GEO, 
								modelName);
	}catch(...){	
		gs->SetTexturePath("");
		_CORE_API->DeleteEntity(id);
		return;
	}	
	gs->SetTexturePath("");
	//Настраиваем
	NODE * node = m->GetNode(0);
	if(node) node->SetTechnique("TornadoDebris");
	//Сохраняем
	mdl[numModels].mdl = m;
	mdl[numModels].prt = prt;
	mdl[numModels++].maxSpeed = spd;
}

void Debris::NormalazedModels()
{
	float sum = 0.0f;
	for(long i = 0; i < numModels; i++) sum += mdl[i].prt;
	for(i = 0; i < numModels; i++) mdl[i].prt /= sum;
}

MODEL * Debris::SelectModel(float & maxSpd)
{
	float rnd = rand()*(1.0f/RAND_MAX);
	float sum = 0.0f;
	for(long i = 0; i < numModels - 1; i++)
	{
		sum += mdl[i].prt;
		if(rnd < sum)
		{
			maxSpd = mdl[i].maxSpeed*(0.5f + rand()*(1.4f/RAND_MAX));
			return mdl[i].mdl;
		}
	}
	maxSpd = mdl[i].maxSpeed;
	return mdl[i].mdl;
}

bool Debris::IsShip()
{
	ENTITY_ID id;	
	bool res = _CORE_API->FindClass(&id, null, shipcode);
	if(!res) return false;
	CVECTOR p(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
	CVECTOR pos;
	for(; res; res = _CORE_API->FindClassNext(&id))
	{
		//Указатель на объект
		VAI_OBJBASE * ship = (VAI_OBJBASE *)_CORE_API->GetEntityPointer(&id);
		if(!ship) break;
		//Позиция торнадо в системе корабля
		Assert(ship->GetMatrix());
		ship->GetMatrix()->MulToInv(p, pos);
		//Проверим попадание в бокс
		CVECTOR s = ship->GetBoxSize();
		if(pos.x < -s.x - 6.0f || pos.x > s.x + 6.0f) continue;
		if(pos.z < -s.z - 6.0f || pos.z > s.z + 6.0f) continue;
		return true;
	}
	return false;
}

void Debris::SetGlobalAlpha(float a)
{
	if(a < 0.0f) a = 0.0f;
	if(a > 1.0f) a = 1.0f;
	galpha = a*255.0f;
}

