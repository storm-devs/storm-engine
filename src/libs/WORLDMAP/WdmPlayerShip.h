//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmPlayerShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmPlayerShip_H_
#define _WdmPlayerShip_H_

#include "WdmShip.h"

class WdmPlayerShip : public WdmShip
{
    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WdmPlayerShip();
    virtual ~WdmPlayerShip();

    void PushOutFromIsland();

    void SetActionRadius(float radius);

    //Расчёты
    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    //Проверка при выходе из карты
    bool ExitFromMap();
    //Проверка на нахождении кораблика в шторме
    long TestInStorm() const;

    float GetAy() const;

    bool canSkip;

  protected:
    //Переместить кораблик
    virtual void Move(float dltTime);

    void Collide() override;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    bool goForward;
    float actionRadius;
    float stormEventTime;
};

inline float WdmPlayerShip::GetAy() const
{
    return ay;
}

#endif
