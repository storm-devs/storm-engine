//============================================================================================
//	LocCrabs
//============================================================================================

#ifndef _LocCrabs_h_
#define _LocCrabs_h_

#include "..\common_h\collide.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"
#include "..\common_h\vmodule_api.h"

#include "LocCrab.h"

class LocCrabs : public ENTITY
{
  public:
    LocCrabs();
    virtual ~LocCrabs();

    //--------------------------------------------------------------------------------------------
  public:
    //Инициализация
    bool Init();
    //Исполнение
    void Execute(dword delta_time);
    //Рисование
    void Realize(dword delta_time);

    //Сообщения
    dword _cdecl ProcessMessage(MESSAGE &message);

    //--------------------------------------------------------------------------------------------
  private:
    LocCrab crab[32];
    long num;
};

#endif
