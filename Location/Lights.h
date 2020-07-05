//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lights
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Lights_H_
#define _Lights_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "LocationEffects.h"
#include "..\common_h\templates\array.h"

class Lights : public ENTITY  
{
	//Описание источника освещения
	struct LightType
	{
		char * name;
		D3DLIGHT8 dxLight;
		D3DCOLORVALUE color;
		float flicker;
		float flickerSlow;
		float freq;
		float freqSlow;
		float p;
		float pSlow;
		float coronaRange;
		float coronaRange2;
		float invCoronaRange;
		float coronaSize;
		long corona;
	};

	//Источник
	struct Light
	{
		D3DCOLORVALUE color;	//Текущий цвет источника
		D3DVECTOR pos;			//Позиция источника
		float time;				//Время с момента последнего изменения мерцающей интенсивности
		float timeSlow;			//Время с момента последнего изменения интерполируемой интенсивности
		float itens;			//Мерцающая интенсивность
		float itensSlow;		//Необхадимая интерполируемая интенсивность
		float itensDlt;			//Разность интерполируемой интенсивности
		float i;				//Результирующая интенсивность
		long type;				//Индекс типа источника
		float corona;			//Прозрачность короны
	};

	//Управляемый(перемещающийся) источник
	struct MovingLight
	{
		long id;
		long light;
	};

	struct Vertex
	{
		CVECTOR pos;
		dword color;
		float u, v;
	};


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Lights();
	virtual ~Lights();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	//Рисование корон
	void Realize(dword delta_time);

	//Найти индекс источника
	long FindLight(const char * name);
	//Добавить источник в локацию
	void AddLight(long index, const CVECTOR & pos);
	//Добавить модельку фонарей
	bool AddLampModel(const ENTITY_ID & lampModel);
	//
	void DelAllLights();

	//Добавить переносной источник
	long AddMovingLight(const char* type, const CVECTOR& pos);
	//Поставить переносной источник в новую позицию
	void UpdateMovingLight(long id, const CVECTOR& pos);
	//Удалить переносной источник
	void DelMovingLight(long id);

	//Установить для персонажа источники освещения
	void SetCharacterLights(const CVECTOR & pos);
	//Запретить установленные для персонажа источники освещения
	void DelCharacterLights();
	//Установить те же источники что и для последнего расчета
	void SetCharacterLights();

	//Обновить типы источников
	void UpdateLightTypes(long i);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	VDX8RENDER * rs;
	COLLIDE * collide;
	//Установленные источники освещения
	struct{bool set; long light;} lt[8];
	//Виды источников освещения
	LightType * types;
	long numTypes;
	long maxTypes;
	//Существующие источники освещения
	Light * lights;
	long numLights;
	long maxLights;
	long lighter_code;

	//переносные источники света
	array<MovingLight> aMovingLight;

	//Модельки фонарей
	ENTITY_ID lampModels[16];
	long numLampModels;

	Vertex buf[6*1];
};

#endif

