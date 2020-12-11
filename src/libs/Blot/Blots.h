//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Blots
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _Blots_H_
#define _Blots_H_

#include "dx9render.h"
#include "model.h"
#include "vmodule_api.h"

#define BLOTS_NTRGS 32    //Треугольников в 1 пятне
#define BLOTS_MAX 256     //Всего пятен
#define BLOTS_TIME 120.0f //Время жизни пятна
#define BLOTS_DIST 150.0f //Дистанция видимости в метрах

class Blots : public Entity
{

#pragma pack(push, 1)

    struct Blot
    {
        uint8_t isUsed;    //Используеться ли это
        uint8_t lastAlpha; //Последние значение альфы
        uint16_t numTrgs;  //Количество треугольников
        float liveTime;    //Время жизни
        long startIndex;   //Начальный индекс в массиве
        long rnd;
        CVECTOR pos, dir;
    };

    struct Vertex
    {
        CVECTOR pos;
        long c;
        float u, v;
    };

#pragma pack(pop)

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    Blots();
    virtual ~Blots();

    //Инициализация
    bool Init();
    //Сообщения
    uint64_t ProcessMessage(MESSAGE &message);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        // case Stage::execute:
        //	Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }

    //Работа
    void Realize(uint32_t delta_time);

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    //Отметить поподиние
    void Hit(MESSAGE &message);
    //Добавить пятно
    void AddBlot(long i, long rnd, const CVECTOR &pos, const CVECTOR &dir, float time);
    //
    void SetNodesCollision(NODE *n, bool isSet);
    //Сохранить параметры пятна
    void SaveBlot(long i);
    //Загрузить параметры пятна
    void LoadBlot(long i);

  private:
    VDX9RENDER *rs;
    entid_t model;
    long textureID;
    ATTRIBUTES *blotsInfo;
    ATTRIBUTES *pCharAttributeRoot;

    Blot blot[BLOTS_MAX];
    Vertex vrt[3 * BLOTS_NTRGS * BLOTS_MAX];
    long useVrt;

    long updateBlot;

    static bool AddPolygon(const CVECTOR *v, long nv);
    static CVECTOR clipTriangles[3 * BLOTS_NTRGS];
    static long numClipTriangles;
    static CVECTOR dir, normal;
};

#endif
