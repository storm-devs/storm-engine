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

#include "Entity.h"
#include "vmodule_api.h"
#include <string>

#define WDMAP_MAXOBJECTS 4096

class ATTRIBUTES;
class MESSAGE;
class WdmRenderObject;
class WdmRenderModel;
class VDX9RENDER;
class WdmCamera;
class WdmEventWindow;
class WdmWaitMenu;

class WorldMap : public Entity
{
    struct RObject
    {
        WdmRenderObject *ro;
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
    // Entity
    //--------------------------------------------------------------------------------------------
  public:
    //Инициализация
    bool Init() override;
    //Исполнение
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //Сообщения
    uint64_t ProcessMessage(MESSAGE &message) override;
    //Изменение атрибута
    uint32_t AttributeChanged(ATTRIBUTES *apnt) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    //--------------------------------------------------------------------------------------------
    //Управление объектами
    //--------------------------------------------------------------------------------------------
  public:
    //Добавить объект
    WdmRenderObject *AddObject(WdmRenderObject *obj, long level = 0);
    //Добавить объект в список отрисовки до отражения
    void AddPObject(WdmRenderObject *obj, long level = 0);
    //Добавить объект в список отрисовки отражения
    void AddMObject(WdmRenderObject *obj, long level = 0);
    //Добавить объект в список отрисовки после отражения
    void AddLObject(WdmRenderObject *obj, long level = 0);
    //Удалить объект
    void DeleteObject(WdmRenderObject *obj);

    //Проинициализировать модельку и занести в нужные списки рендера
    WdmRenderObject *CreateModel(WdmRenderModel *rm, const char *modelName, bool pr = false, bool mr = true,
                                 bool lr = true, long objectLevel = 0, long drawLevel = 0);

    //--------------------------------------------------------------------------------------------

    VDX9RENDER *GetRS() const;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    //Управление объектами
    //Включить запись об объекте в список с необходимым уровнем
    long GetObject(long &first, long level);
    //Исключить запись из списка
    void FreeObject(long &first, long i);

    //Утилитные
    //Создать шторм, если это возможно
    bool CreateStorm(bool isTornado, float time = -1.0f, ATTRIBUTES *save = nullptr);
    //Создать кораблик купца
    bool CreateMerchantShip(const char *modelName, const char *locNameStart, const char *locNameEnd, float kSpeed,
                            float time = -1.0f, ATTRIBUTES *save = nullptr);
    // boal Создать кораблик купца в координатах
    bool CreateMerchantShipXZ(const char *modelName, float x1, float z1, float x2, float z2, float kSpeed,
                              float time = -1.0f, ATTRIBUTES *save = nullptr);
    //Создать кораблик преследующий нас
    bool CreateFollowShip(const char *modelName, float kSpeed, float time = -1.0f, ATTRIBUTES *save = nullptr);
    //Создать пару воюющих кораблики
    bool CreateWarringShips(const char *modelName1, const char *modelName2, float time = -1.0f,
                            ATTRIBUTES *save1 = nullptr, ATTRIBUTES *save2 = nullptr);
    //Удалить все энкоунтеры
    void ReleaseEncounters();
    //Создать атрибут для сохранения параметров энкоунтера
    ATTRIBUTES *GetEncSaveData(const char *type, const char *retName);

    //Найти координаты и радиус по месту назначения
    bool FindIslandPosition(const char *name, float &x, float &z, float &r);

    void ResetScriptInterfaces() const;

  private:
    //Сервис рендера
    VDX9RENDER *rs;
    WdmCamera *camera;

    ATTRIBUTES *aStorm;
    ATTRIBUTES *aEncounter;
    ATTRIBUTES *aInfo;
    ATTRIBUTES *saveData;

    WdmWaitMenu *waitMenu;

    //Событие
    float encTime;

    //Объекты
    long firstFreeObject;
    long firstObject;   //Все существующие объекты
    long firstPrObject; //Объекты, рендерящиеся до зеркала
    long firstMrObject; //Объекты рендерящиеся в зеркало
    long firstLrObject; //Объекты рендерящиеся после зеркала

    RObject object[WDMAP_MAXOBJECTS];

    ATTRIBUTES *aDate;
    float timeScale;

    std::string bufForSave;

  public:
    uint32_t encCounter;

    float hour;
    long day;
    long mon;
    long year;

    static long month[];
};

inline VDX9RENDER *WorldMap::GetRS() const
{
    return rs;
}

#endif
