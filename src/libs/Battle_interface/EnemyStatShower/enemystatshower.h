#pragma once

#include "Entity.h"

class MODEL;
class NODE;

class EnemyStatShower : public Entity
{
  public:
    EnemyStatShower();
    ~EnemyStatShower();

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

    entid_t GetModelEIDFromCharacterEID(entid_t chrEID);

  protected: // data
    bool m_bVisible;
    entid_t m_eidChrEID;
    MODEL *m_pModel;
};
