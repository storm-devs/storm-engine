#ifndef _ModelWraper_H_
#define _ModelWraper_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\geometry.h"

class Lights;

class LocModelRealizer : public ENTITY
{
	ENTITY_ID eid_model;
	Lights* lights;
	VDX8RENDER* rs;
	VGEOMETRY* gs;
	bool bShow;

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	LocModelRealizer();
	~LocModelRealizer();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
};

#endif
