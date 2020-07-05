//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocEagle
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "LocEagle.h"
#include "Location.h"
#include "..\Common_h\geometry.h"


LocEagle::LocEagle()
{
	time = 1.0f;
	kTime = 1.0f;
	y = 0.0f;
	dltY = 0.0f;
	kRad = 1.0f;
	dltRad = 0.0f;
	kAy = 1.0f;
	timeAy = 0.0f;
	ay = rand()*6.5f/RAND_MAX;
}

LocEagle::~LocEagle()
{
	api->DeleteEntity(mdl);
}


//Инициализация
bool LocEagle::Init()
{
	//Точка, вокруг которой летаем
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	Location * location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return false;
	cnt = location->GetPtcData().middle + CVECTOR(0.0f, 30.0f, 0.0f);
	//Путь для текстур
	VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	if(!gs)
	{
		_CORE_API->Trace("Can't create geometry service!");
		return false;
	}
	//Моделька
	if(!api->CreateEntity(&mdl, "modelr")) return false;
	_CORE_API->LayerAdd("realize", mdl, 20);
	gs->SetTexturePath("Animals\\");
	if(!api->Send_Message(mdl, "ls", MSG_MODEL_LOAD_GEO, "Animals\\eagle"))
	{
		gs->SetTexturePath("");
		return false;
	}
	gs->SetTexturePath("");
	//Анимация
	if(!api->Send_Message(mdl, "ls", MSG_MODEL_LOAD_ANI, "eagle")) return false;
	//Запускаем проигрывание анимации
	MODEL * m = (MODEL *)_CORE_API->GetEntityPointer(&mdl);
	if(!m) return false;
	Animation * ani = m->GetAnimation();
	if(!ani) return false;
	if(!ani->Player(0).SetAction("flight")) return false;
	if(!ani->Player(0).Play()) return false;
	//Включаем в список исполнения
	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("execute", GetID(), 10);
	return true;	
}

//Исполнение
void LocEagle::Execute(dword delta_time)
{
	//Моделька
	MODEL * m = (MODEL *)_CORE_API->GetEntityPointer(&mdl);
	if(!m) return;
	//Обновляем позицию
	float dltTime = delta_time*0.001f;
	time += kTime*dltTime;
	if(time >= 1.0f)
	{
		dltY = ((rand() & 15)*(2.0f/15.0f) - 1.0f)*	1.0f - y;
		time = 0.0f;
		kTime = 1.0f/(2.0f + rand()*(20.0f/RAND_MAX));
		dltRad = (0.5f + ((rand() & 7)*(1.0f/7.0f)*0.7f)) - kRad;
	}
	timeAy -= dltTime;
	if(timeAy <= 0.0f)
	{
		if(rand() & 3)
		{
			//Быстро
			timeAy = 10.0f + rand()*(10.0f/RAND_MAX);
			kAy = 1.0f;
		}else{
			//Медленно
			timeAy = 1.0f + rand()*(2.0f/RAND_MAX);
			kAy = 0.4f;
			time = 0.0f;
			kTime = 1.0f/(1.0f + rand()*(1.0f/RAND_MAX));
			dltY = -1.0f - y;
		}
	}
	ay += dltTime*kAy*0.1f/kRad;
	y += dltY*kTime*dltTime;
	//Устанавливаем позицию
	m->mtx.BuildMatrix(0.0f, ay + 1.57f, 0.0f);
	m->mtx.Pos().x = cnt.x + kRad*20.0f*sinf(ay);
	m->mtx.Pos().y = cnt.y + y;
	m->mtx.Pos().z = cnt.z + kRad*20.0f*cosf(ay);
}


