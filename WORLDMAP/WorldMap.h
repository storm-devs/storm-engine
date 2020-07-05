//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WorldMap
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WorldMap_H_
#define _WorldMap_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\templates\string.h"


#define WDMAP_MAXOBJECTS	4096



class WdmRenderObject;
class WdmRenderModel;
class VDX8RENDER;
class WdmCamera;
class WdmEventWindow;
class WdmWaitMenu;

class WorldMap : public ENTITY
{
	struct RObject
	{
		WdmRenderObject * ro;
		long level;
		long next;
		long prev;
	};


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WorldMap();
	virtual ~WorldMap();

//--------------------------------------------------------------------------------------------
//Entity
//--------------------------------------------------------------------------------------------
public:
	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
	//Изменение атрибута
	dword AttributeChanged(ATTRIBUTES * apnt);

//--------------------------------------------------------------------------------------------
//Управление объектами
//--------------------------------------------------------------------------------------------
public:
	//Добавить объект
	WdmRenderObject * AddObject(WdmRenderObject * obj, long level = 0);
	//Добавить объект в список отрисовки до отражения
	void AddPObject(WdmRenderObject * obj, long level = 0);
	//Добавить объект в список отрисовки отражения
	void AddMObject(WdmRenderObject * obj, long level = 0);
	//Добавить объект в список отрисовки после отражения
	void AddLObject(WdmRenderObject * obj, long level = 0);
	//Удалить объект
	void DeleteObject(WdmRenderObject * obj);





	//Проинициализировать модельку и занести в нужные списки рендера
	WdmRenderObject * CreateModel(WdmRenderModel * rm, const char * modelName, bool pr = false, bool mr = true, bool lr = true, long objectLevel = 0, long drawLevel = 0);


//--------------------------------------------------------------------------------------------

	VDX8RENDER * GetRS();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Управление объектами
	//Включить запись об объекте в список с необходимым уровнем
	long GetObject(long & first, long level);
	//Исключить запись из списка
	void FreeObject(long & first, long i);

	//Утилитные
	//Создать шторм, если это возможно
	bool CreateStorm(bool isTornado, float time = -1.0f, ATTRIBUTES * save = null);
	//Создать кораблик купца
	bool CreateMerchantShip(const char * modelName, const char * locNameStart, const char * locNameEnd, float kSpeed, float time = -1.0f, ATTRIBUTES * save = null);
	// boal Создать кораблик купца в координатах
	bool CreateMerchantShipXZ(const char * modelName, float x1, float z1, float x2, float z2, float kSpeed, float time = -1.0f, ATTRIBUTES * save = null);
	//Создать кораблик преследующий нас
	bool CreateFollowShip(const char * modelName, float kSpeed, float time = -1.0f, ATTRIBUTES * save = null);
	//Создать пару воюющих кораблики
	bool CreateWarringShips(const char * modelName1, const char * modelName2, float time = -1.0f, ATTRIBUTES * save1 = null, ATTRIBUTES * save2 = null);
	//Удалить все энкоунтеры
	void ReleaseEncounters();
	//Создать атрибут для сохранения параметров энкоунтера
	ATTRIBUTES * GetEncSaveData(const char * type, const char * retName);


	//Найти координаты и радиус по месту назначения
	bool FindIslandPosition(const char * name, float & x, float & z, float & r);

	void ResetScriptInterfaces();

private:
	//Сервис рендера
	VDX8RENDER * rs;
	WdmCamera * camera;

	ATTRIBUTES * aStorm;
	ATTRIBUTES * aEncounter;
	ATTRIBUTES * aInfo;
	ATTRIBUTES * saveData;

	WdmEventWindow * eventWindow;
	WdmWaitMenu * waitMenu;

	//Событие
	float encTime;

	//Объекты
	long firstFreeObject;
	long firstObject;					//Все существующие объекты
	long firstPrObject;					//Объекты, рендерящиеся до зеркала
	long firstMrObject;					//Объекты рендерящиеся в зеркало
	long firstLrObject;					//Объекты рендерящиеся после зеркала
	
	RObject object[WDMAP_MAXOBJECTS];


	ATTRIBUTES * aDate;
	float timeScale;

	string bufForSave;

public:
	dword encCounter;

	float hour;
	long day;
	long mon;
	long year;

	static long month[];
};

inline VDX8RENDER * WorldMap::GetRS()
{
	return rs;
}

#endif
