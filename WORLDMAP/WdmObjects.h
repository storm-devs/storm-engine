//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmObjects
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmObjects_H_
#define _WdmObjects_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

#include "WdmObjectsWind.h"

class WdmIslands;
class WdmShip;
class WdmPlayerShip;
class WorldMap;
class WdmCamera;
class WdmLocations;
class WdmStorm;
class WdmEnemyShip;


//#define WDM_WORLD_SIZE_X	2000.0f
//#define WDM_WORLD_SIZE_Z	2000.0f

enum EnemyShipType
{
	wdmest_unknow,
	wdmest_merchant,
	wdmest_warring,
	wdmest_follow,
};

class GEOS;
class VGEOMETRY;

class WdmObjects  
{
	struct Model
	{
		GEOS * geo;
		string path;
		dword hash;
		long next;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmObjects();
	virtual ~WdmObjects();

	void SetWorldSize(float x, float z);
	void Clear();

	void AddShip(WdmShip * ship);
	void DelShip(WdmShip * ship);

	void AddStorm(WdmStorm * storm);
	void DelStorm(WdmStorm * storm);

	//Создать геометрию
	GEOS * CreateGeometry(const char * path);

	//Энжиновский орбъект, заведующий всем
	WorldMap * wm;
	//Сервис рендера
	VDX8RENDER * rs;
	//Сервис геометриии
	VGEOMETRY * gs;
	//Камера
	WdmCamera * camera;
	//Острова
	WdmIslands * islands;

	//Корабли
	//Корабль игрока
	WdmShip * playerShip;
	//Все существующие корабли
	WdmShip ** ships;
	long numShips;
	long maxShips;

	WdmEnemyShip * enemyShip;
	bool enableSkipEnemy;

	//Шторма	
	WdmStorm ** storms;
	long numStorms;	//Количество штормов
	long maxStorms;
	bool playarInStorm;

	const char * curIsland;

	bool isPause;
	bool isDebug;

	void DrawCircle(const CVECTOR & pos, float radius, dword color);
	void DrawCircle(CMatrix & mtx, float radius, dword color);
	void DrawVector(const CVECTOR & start, const CVECTOR & end, dword color);
	void DrawLine(const CVECTOR & start, const CVECTOR & end, dword color);
	void DrawBox2D(CMatrix & mtx, float l, float w, dword color);
	void GetVPSize(float & w, float & h);

	float shipSpeedOppositeWind;	//Относительная скорость корабля против ветра
	float shipSpeedOverWind;		//Относительная скорость корабля по ветру

	float enemyshipViewDistMin;		//Растояние на котором корабль начинает исчезать
	float enemyshipViewDistMax;		//Растояние на котором корабль исчезает полностью
	float enemyshipDistKill;		//Расстояние на котором убиваем корабль
	float enemyshipBrnDistMin;		//Минимальное растояние на котором рожается корабль
	float enemyshipBrnDistMax;		//Максимальное растояние на котором рожается корабль

	float stormViewDistMin;			//Растояние на котором шторм начинает исчезать
	float stormViewDistMax;			//Растояние на котором шторм исчезает полностью
	float stormDistKill;			//Расстояние на котором убиваем шторм
	float stormBrnDistMin;			//Минимальное растояние на котором рожается шторм
	float stormBrnDistMax;			//Максимальное растояние на котором рожается шторм
	float stormZone;				//Общий радиус действия шторма


	char attrSec[256];				//Секунды на текущем кадре
	char attrMin[256];				//Минуты на текущем кадре
	char attrHour[256];				//Часы на текущем кадре
	char attrDay[256];				//День на текущем кадре
	char attrMonth[256];			//Месяц на текущем кадре
	char attrYear[256];				//Год на текущем кадре
	bool isNextDayUpdate;			//Пора обновить данные следующего дня

	float worldSizeX;				//Размер мира по X
	float worldSizeZ;				//Размер мира по Z

	array<Model> models;			//Модельки
	long entryModels[1024];			//Таблица быстрого поиска геометрии
	string modelPath;

	//Найти хэщь-значение строки
	static dword CalcHash(const char * str);

	//Получить направление и силу ветра
	float GetWind(float x, float z, CVECTOR & dir);
	//Обновить состояние ветра
	void UpdateWind(float dltTime);
	//Получить строку сохранение
	const char * GetWindSaveString(string & windData);
	//Установить строку сохранение
	void SetWindSaveString(const char * str);
	//Добавить float в cтроку
	void AddDataToString(string & str, byte d);
	//Получить float из строки
	long GetDataFromString(const char * & cur);

	WindField windField;

private:
	struct Vertex
	{
		CVECTOR v;
		dword c;
	};

	static Vertex vertex[1024];
};

extern WdmObjects * wdmObjects;

//Найти хэщь-значение строки
inline dword WdmObjects::CalcHash(const char * str)
{
	if(!str) return 0;
	unsigned long hval = 0;
	while(*str != '\0')
	{
		char c = *str++;
		if(c >= 'A' && c <= 'Z') c += 'a' - 'A';
		hval = (hval<<4) + (unsigned long int)c;
		unsigned long g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}		
	}
	return hval;
}

#endif

