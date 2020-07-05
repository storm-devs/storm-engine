//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocRats
//--------------------------------------------------------------------------------------------
//	
//============================================================================================
			

#include "LocRats.h"

//============================================================================================

LocRats::LocRats()
{
	num = sizeof(rat)/sizeof(LocRat);
}

LocRats::~LocRats()
{

}


//============================================================================================


//Инициализация
bool LocRats::Init()
{
	//Указатель на локацию
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	Location * location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return false;
	//Исполнение
	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetFlags("realize", LRFLAG_REALIZE);
	_CORE_API->LayerAdd("realize", GetID(), 100000);
	return true;
}

//Сообщения
dword _cdecl LocRats::ProcessMessage(MESSAGE & message)
{
	long num = message.Long();
	if(num < 1) num = 1;
	if(num > sizeof(rat)/sizeof(LocRat)) num = sizeof(rat)/sizeof(LocRat);
	//Указатель на локацию
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	Location * location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return 0;
	//Заводим крыс
	for(long i = 0; i < num; i++) rat[i].Init(location);
	return 1;
}

//Исполнение
void LocRats::Execute(dword delta_time)
{
}

//Рисование
void LocRats::Realize(dword delta_time)
{
	float dltTime = delta_time*0.001f;
	for(long i = 0; i < num; i++) rat[i].Update(dltTime);

}





