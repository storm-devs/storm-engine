//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	WdmEnemyShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmEnemyShip_H_
#define _WdmEnemyShip_H_

#include "WdmShip.h"
#include "attributes.h"

class WdmEnemyShip : public WdmShip
{
    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WdmEnemyShip();
    virtual ~WdmEnemyShip();

    //Расчёты
    void Update(float dltTime) override;

    //Отрисовка дебажной информации
    void LRender(VDX9RENDER *rs) override;

    //Найти позицию для корабля относительно игрока
    static bool GeneratePosition(float objRadius, float brnDltAng, float &x, float &z);

    //Установить время жизни
    void SetLiveTime(float time);
    //Получить время жизни
    float GetLiveTime() const;

    bool isEnableKill;
    bool isEnemy; //Если установлен, то атакует нас
    bool isEntryPlayer;

    uint32_t type;
    WdmEnemyShip *attack;

    EnemyShipType shipType;

    bool canSkip;

    //Получить имя атрибута
    const char *GetAttributeName() const;

  protected:
    //Найти силу притягивающую в нужном направлении
    virtual void FindMoveForce();
    //Найти силу отталкивающую от островов
    virtual void FindIslandForce();
    //Найти силу отталкивающую от кораблей
    virtual void FindShipsForce();
    //Все расчёты то перемещению вместе
    virtual void Move(float dltTime);
    //Проверка на завершение
    virtual bool KillTest();
    //Обновление сохраняемых данных
    virtual void UpdateSaveData();

  public:
    //Установка параметров
    virtual void SetSaveAttribute(ATTRIBUTES *save);
    void DeleteUpdate();

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------

  protected:
    float mx, mz; //Вектор направления куда перемещаться
    float ix, iz; //Вектор отталкивания от островов
    float sx, sz; //Вектор отталкивания от кораблей
    float dx, dz; //Вектор результирующего направления

    //Обращать ли внимание при расталкивание на плеера
    bool isLookOnPlayer;

    ATTRIBUTES *saveAttribute;

  private:
    float brnAlpha;
    float deleteAlpha;
    float liveTime;
    float slowingAlfa;
};

//Получить время жизни
inline float WdmEnemyShip::GetLiveTime() const
{
    if (!isEnableKill)
        return -1.0f;
    return liveTime >= 0.0f ? liveTime : 0.0f;
}

#endif
