//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Sharks
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _Sharks_H_
#define _Sharks_H_

#include "Animation.h"
#include "Island_Base.h"
#include "Matrix.h"
#include "dx9render.h"
#include "sea_base.h"
#include "ship_base.h"

class Sharks : public Entity
{
    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    class Shark : public AnimationEventListener
    {
        static uint16_t indeces[];

      public:
        Shark();
        ~Shark();

        bool Init(float vp_x, float vp_z, bool isLoadModel = true);

        void Reset(float cam_x, float cam_z);
        void Repulsion(Shark &shr);
        void ShipApply(float x, float z, float r2);
        void Coordination(float cam_x, float cam_z, float dltTime, SEA_BASE *sb, ISLAND_BASE *ib);
        void IslandCollision(ISLAND_BASE *ib, long numPnt, float rad, float frc);
        void Event(Animation *animation, long index, long eventID, AnimationEvent event) override;
        long GenerateTrack(uint16_t *inds, Vertex *vrt, uint16_t base, SEA_BASE *sb);

        //Точка следования
        CVECTOR pos;    //Позиция точки следования
        CVECTOR vel;    //Натравление перемещение
        CVECTOR force;  //Сила направления движения
        float yDir;     //Тенденция к всплытию или погружению
        float dirTime;  //Время до следующей смены
        float accs;     //Ускорение акулы
        CVECTOR rForce; //Случайная сторонния сила
        float rTime;
        //Акула
        CVECTOR fforce; //Сила направления движения
        CVECTOR spos;   //Позиция модельки
        CVECTOR angs;   //Углы модельки
        float turn;     //Скорость изменения угла
        float imspd;    //Скорость погружения
        float speed;    //Скорость перемещения
        float shipY;    //Ограничение кораблём
        float vBase;    //Смещение трека акулы
        entid_t model;  //Моделька акулы
        //Анимация
        float aniTime;  //Время проигрывания текущей анимации
        float jumpTime; //Время до следующего выпрыгивания
        bool speedUp;
    };

    friend Shark;

    struct Periscope
    {
        CVECTOR pos;
        float ay;
        float time;
        entid_t model;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    Sharks();
    virtual ~Sharks();

    //Инициализация
    bool Init();
    //Исполнение
    void Execute(uint32_t delta_time);
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

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    bool LoadPeriscopeModel();

  private:
    VDX9RENDER *rs;
    Shark shark[6];
    long numShakes;
    Periscope periscope;
    float waitPTime;
    CVECTOR camPos;
    entid_t sea;
    entid_t island;
    long trackTx;
    uint16_t indeces[7 * 10 * 3];
    Vertex vrt[7 * 10];
};

#endif
