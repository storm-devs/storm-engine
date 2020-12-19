//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Lights
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "collide.h"
#include "dx9render.h"
#include "vmodule_api.h"
#include <vector>

class Lights : public Entity
{
    //Описание источника освещения
    struct LightType
    {
        char *name;
        D3DLIGHT9 dxLight;
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
        D3DCOLORVALUE color; //Текущий цвет источника
        D3DVECTOR pos;       //Позиция источника
        float time; //Время с момента последнего изменения мерцающей интенсивности
        float timeSlow; //Время с момента последнего изменения интерполируемой интенсивности
        float itens;    //Мерцающая интенсивность
        float itensSlow; //Необхадимая интерполируемая интенсивность
        float itensDlt;  //Разность интерполируемой интенсивности
        float i;         //Результирующая интенсивность
        long type;       //Индекс типа источника
        float corona;    //Прозрачность короны
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
        uint32_t color;
        float u, v;
    };

    //Для сортировки для по дистанции
    struct lt_elem
    {
        long idx;
        float dst;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    Lights();
    virtual ~Lights();

    //Инициализация
    bool Init() override;
    //Исполнение
    void Execute(uint32_t delta_time);
    //Рисование корон
    void Realize(uint32_t delta_time);

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

    //Найти индекс источника
    long FindLight(const char *name);
    //Добавить источник в локацию
    void AddLight(long index, const CVECTOR &pos);
    //Добавить модельку фонарей
    bool AddLampModel(entid_t lampModel);
    //
    void DelAllLights();

    //Добавить переносной источник
    long AddMovingLight(const char *type, const CVECTOR &pos);
    //Поставить переносной источник в новую позицию
    void UpdateMovingLight(long id, const CVECTOR &pos);
    //Удалить переносной источник
    void DelMovingLight(long id);

    //Установить для персонажа источники освещения
    void SetCharacterLights(const CVECTOR *pos = nullptr);

    //Запретить установленные для персонажа источники освещения
    void DelCharacterLights();

    //Обновить типы источников
    void UpdateLightTypes(long i);

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;
    COLLIDE *collide;

    //Установленные источники освещения
    struct
    {
        bool set;
        long light;
    } lt[8];

    //Виды источников освещения
    std::vector<LightType> types;
    long numTypes;
    long maxTypes;
    //Существующие источники освещения
    std::vector<Light> lights;
    long numLights;
    long maxLights;
    long lighter_code;

    //переносные источники света
    std::vector<MovingLight> aMovingLight;

    //Модельки фонарей
    entid_t lampModels[16];
    long numLampModels;

    Vertex buf[6 * 1];

    //Отсортированный массив источников для последнего расчета
    std::vector<lt_elem> aLightsDstSort;
};
