//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Location
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _Location_H_
#define _Location_H_

#include "Matrix.h"
#include "vmodule_api.h"
#include <cstdint>

#include "LocatorArray.h"
#include "ModelArray.h"
#include "PtcData.h"
#include "Supervisor.h"

class MODEL;
class Lights;

class Location : public Entity
{
    struct SphVertex
    {
        CVECTOR v;
        uint32_t c;
    };

    struct BarVertex
    {
        CVECTOR p;
        float rhw;
        uint32_t c;
        float u, v;
    };

    struct DmgMessage
    {
        CVECTOR p;
        float alpha;
        float hit, hp;
        uint32_t c;
    };

    struct EnemyBar
    {
        CVECTOR p;
        float hp;
        float energy;
        float alpha;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    Location();
    virtual ~Location();

    //Инициализация
    bool Init() override;
    //Исполнение
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //Сообщения
    uint64_t ProcessMessage(MESSAGE &message) override;

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
    // Location
    //--------------------------------------------------------------------------------------------
  public:
    //Найти группу локаторов
    LocatorArray *FindLocatorsGroup(const char *gName);

    //Получить патч для персонажа
    PtcData &GetPtcData();
    //Получить модельку патча для прыжков
    MODEL *JmpPatch();

    //Проверить видимость 2-х точек
    bool VisibleTest(const CVECTOR &p1, const CVECTOR &p2);

    //Протрейсит луч через локацию
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, long num));

    Lights *GetLights() const;

    VDX9RENDER *GetRS() const;
    void DrawLine(const CVECTOR &s, uint32_t cs, const CVECTOR &d, uint32_t cd, bool useZ = true) const;
    //Написать текст
    void Print(const CVECTOR &pos3D, float rad, long line, float alpha, uint32_t color, float scale, const char *format,
               ...) const;

    bool IsDebugView();
    bool IsExDebugView();

    bool IsPaused() const;

    bool IsSwimming() const;

    //Добавить сообщение о повреждении
    void AddDamageMessage(const CVECTOR &pos3D, float hit, float curhp, float maxhp);
    //Нарисовать на данном кадре полоски над врагом
    void DrawEnemyBars(const CVECTOR &pos, float hp, float energy, float alpha);

  public:
    //Объект управляющий расталкиванием персонажей
    Supervisor supervisor;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    void Update(uint32_t delta_time);
    long LoadStaticModel(const char *modelName, const char *tech, long level, bool useDynamicLights);
    bool LoadCharacterPatch(const char *ptcName);
    void LoadCaustic() const;
    bool LoadJumpPatch(const char *modelName);
    bool LoadGrass(const char *modelName, const char *texture);
    bool MessageEx(const char *name, MESSAGE &message);
    void UpdateLocators();
    void DrawLocators(LocatorArray *la);
    void CreateSphere();
    void TestLocatorsInPatch(MESSAGE &message);
    //Отрисовка полосок над персонажами
    void DrawEnemyBars();
    void DrawBar(const MTX_PRJ_VECTOR &vrt, uint32_t color, float hp, float energy);
    void CorrectBar(float v, float start, float end, BarVertex *vrt);

  private:
    PtcData ptc;
    long patchJump;

    long lastLoadStaticModel;

    //Все локаторы
    std::vector<LocatorArray *> locators;
    long numLocators;
    long maxLocators;

    bool isPause;
    bool isDebugView;

    VDX9RENDER *rs;

    //Все модельки
    ModelArray model;

    //Трава
    entid_t grass;
    //Орёл
    entid_t eagle;
    //Ящерецы
    entid_t lizards;
    //Крысы
    entid_t rats;
    //Крабы
    entid_t crabs;
    //Кровь
    entid_t blood;

    entid_t lightsid;
    Lights *lights; //Указатель для текущего кадра

    entid_t loceffectsid;

    SphVertex *sphereVertex;
    long sphereNumTrgs;

    float locationTimeUpdate;

    entid_t lighter;
    // entid_t cubeShotMaker;

    DmgMessage message[32];
    long curMessage;

    EnemyBar enemyBar[32];
    long enemyBarsCount;
    long enemyBarsTexture;
    bool bDrawBars;

    bool bSwimming;
};

//Получить патч для персонажа
inline PtcData &Location::GetPtcData()
{
    return ptc;
}

//Получить модельку патча для прыжков
inline MODEL *Location::JmpPatch()
{
    if (patchJump < 0)
        return nullptr;
    return model[patchJump];
}

inline VDX9RENDER *Location::GetRS() const
{
    return rs;
}

//Проверить видимость 2-х точек
inline bool Location::VisibleTest(const CVECTOR &p1, const CVECTOR &p2)
{
    return model.VisibleTest(p1, p2);
}

inline Lights *Location::GetLights() const
{
    return lights;
}

//Протрейсит луч через локацию
inline float Location::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    return model.Trace(src, dst);
}

inline bool Location::GetCollideTriangle(TRIANGLE &trg) const
{
    return model.GetCollideTriangle(trg);
}

inline void Location::Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, long num))
{
    model.Clip(p, numPlanes, cnt, rad, fnc);
}

inline bool Location::IsPaused() const
{
    return isPause;
}

#endif
