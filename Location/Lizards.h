//============================================================================================
//	Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lizards
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Lizards_h_
#define _Lizards_h_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"

#include "Lizard.h"


class Lizards : public ENTITY
{
public:
	Lizards();
	virtual ~Lizards();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	//Рисование
	void Realize(dword delta_time);


	Lizard lizard[8];
	long num;
};

#endif

