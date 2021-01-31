//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	WideScreen
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WideScreen_H_
#define _WideScreen_H_

#include "dx9render.h"
#include "vmodule_api.h"

class WideScreen : public Entity
{
    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WideScreen();
    virtual ~WideScreen();

    //Инициализация
    bool Init() override;
    //Сообщения
    uint64_t ProcessMessage(MESSAGE &message) override;
    //Работа
    void Realize(uint32_t delta_time);

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

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs; //
    float w, h;     //Размеры экрана
    float state;    //Состояние экрана
    float dlt;      //Направление изменения экрана
};

#endif
