#pragma once

#include "bi_defines.h"
#include <string>

class BIImageRender;
class IBIImage;

class BITimer : public Entity
{
  public:
    BITimer();
    ~BITimer() override;

    bool Init() override;
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

  protected:
    bool ReadAndCreate();
    void SetBeginData();
    void Release();

    void OpenTimer(float fTime, const char *pcEventName);
    void CloseTimer(bool bTimeOut);

  protected: // data
    float m_fCurTimerCounter;
    float m_fMaxTimerCounter;
    std::string m_sEventName;

    VDX9RENDER *m_pRender;
    BIImageRender *m_pImgRndr;
    IBIImage *m_pBackImage;
    IBIImage *m_pForeImage;
};
