//============================================================================================
//	LocCrabs
//============================================================================================
			

#include "LocCrabs.h"

//============================================================================================

LocCrabs::LocCrabs()
{
	num = sizeof(crab)/sizeof(LocCrab);
}

LocCrabs::~LocCrabs()
{

}


//============================================================================================


//Инициализация
bool LocCrabs::Init()
{
	//Указатель на локацию
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	Location * location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return false;
	//Исполнение
	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetFlags("realize", LRFLAG_REALIZE);
	_CORE_API->LayerAdd("realize", GetID(), 100001);
	return true;
}

//Сообщения
dword _cdecl LocCrabs::ProcessMessage(MESSAGE & message)
{
	long num = message.Long();
	if(num < 1) num = 1;
	if(num > sizeof(crab)/sizeof(LocCrab)) num = sizeof(crab)/sizeof(LocCrab);
	//Указатель на локацию
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	Location * location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return 0;
	//Заводим крабов
	for(long i = 0; i < num; i++) crab[i].Init(location);	
	return 1;
}

//Исполнение
void LocCrabs::Execute(dword delta_time)
{
}

//Рисование
void LocCrabs::Realize(dword delta_time)
{
	float dltTime = delta_time*0.001f;
	for(long i = 0; i < num; i++) crab[i].Update(dltTime);
}





