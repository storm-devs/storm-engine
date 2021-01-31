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

#include "Matrix.h"
#include "dx9render.h"

#include "WdmObjectsWind.h"

#include <string>
#include <vector>

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
        GEOS *geo;
        std::string path;
        uint32_t hash;
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

    void AddShip(WdmShip *ship);
    void DelShip(WdmShip *ship);

    void AddStorm(WdmStorm *storm);
    void DelStorm(WdmStorm *storm);

    //Создать геометрию
    GEOS *CreateGeometry(const char *path);

    //Энжиновский орбъект, заведующий всем
    WorldMap *wm;
    //Сервис рендера
    VDX9RENDER *rs;
    //Сервис геометриии
    VGEOMETRY *gs;
    //Камера
    WdmCamera *camera;
    //Острова
    WdmIslands *islands;

    //Корабли
    //Корабль игрока
    WdmShip *playerShip;
    //Все существующие корабли
    std::vector<WdmShip *> ships;

    WdmEnemyShip *enemyShip;
    bool enableSkipEnemy;

    //Шторма
    std::vector<WdmStorm *> storms;
    bool playarInStorm;

    const char *curIsland;

    bool isPause;
    bool isDebug;

    void DrawCircle(const CVECTOR &pos, float radius, uint32_t color) const;
    void DrawCircle(CMatrix &mtx, float radius, uint32_t color) const;
    void DrawVector(const CVECTOR &start, const CVECTOR &end, uint32_t color) const;
    void DrawLine(const CVECTOR &start, const CVECTOR &end, uint32_t color) const;
    void DrawBox2D(CMatrix &mtx, float l, float w, uint32_t color) const;
    void GetVPSize(float &w, float &h) const;

    float shipSpeedOppositeWind; //Относительная скорость корабля против ветра
    float shipSpeedOverWind;     //Относительная скорость корабля по ветру

    float enemyshipViewDistMin; //Растояние на котором корабль начинает исчезать
    float enemyshipViewDistMax; //Растояние на котором корабль исчезает полностью
    float enemyshipDistKill;    //Расстояние на котором убиваем корабль
    float enemyshipBrnDistMin; //Минимальное растояние на котором рожается корабль
    float enemyshipBrnDistMax; //Максимальное растояние на котором рожается корабль

    float stormViewDistMin; //Растояние на котором шторм начинает исчезать
    float stormViewDistMax; //Растояние на котором шторм исчезает полностью
    float stormDistKill;    //Расстояние на котором убиваем шторм
    float stormBrnDistMin;  //Минимальное растояние на котором рожается шторм
    float stormBrnDistMax; //Максимальное растояние на котором рожается шторм
    float stormZone;       //Общий радиус действия шторма

    char attrSec[256];    //Секунды на текущем кадре
    char attrMin[256];    //Минуты на текущем кадре
    char attrHour[256];   //Часы на текущем кадре
    char attrDay[256];    //День на текущем кадре
    char attrMonth[256];  //Месяц на текущем кадре
    char attrYear[256];   //Год на текущем кадре
    bool isNextDayUpdate; //Пора обновить данные следующего дня

    long nationFlagIndex; //индекс национального флага

    char coordinate[128]; // для вывода строки координат
    char stCoordinate[128];

    float resizeRatio; // для ресайза интерфейсов
    float worldSizeX;  //Размер мира по X
    float worldSizeZ;  //Размер мира по Z

    std::vector<Model> models; //Модельки
    long entryModels[1024];    //Таблица быстрого поиска геометрии
    std::string modelPath;

    //Получить направление и силу ветра
    float GetWind(float x, float z, CVECTOR &dir);
    //Обновить состояние ветра
    void UpdateWind(float dltTime);
    //Получить строку сохранение
    const char *GetWindSaveString(std::string &windData);
    //Установить строку сохранение
    void SetWindSaveString(const char *str);
    //Добавить float в cтроку
    void AddDataToString(std::string &str, uint8_t d);
    //Получить float из строки
    long GetDataFromString(const char *&cur);

    WindField windField;

  private:
    struct Vertex
    {
        CVECTOR v;
        uint32_t c;
    };

    static Vertex vertex[1024];
};

extern WdmObjects *wdmObjects;

#endif
