#pragma once

#include "../bi_defines.h"

class IBoardingStatus : public Entity
{
    VDX9RENDER *rs;

  public:
    IBoardingStatus();
    ~IBoardingStatus() override;
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
    void Create();
    void SetCharactersHP(float myHP, float enemyHP);

  protected:
    // show parameters
    //-----------------------
    long m_Width;
    long m_Height;
    FPOINT m_myPos;
    FPOINT m_enemyPos;
    uint32_t m_myColor;
    uint32_t m_enemyColor;

    BI_COLORONLY_VERTEX m_MyChar[4];
    BI_COLORONLY_VERTEX m_EnemyChar[4];
};
