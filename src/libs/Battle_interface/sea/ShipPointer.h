#pragma once

#include "Sd2_h/VAI_ObjBase.h"

class SHIPPOINTER : public Entity
{
    VDX9RENDER *rs;

  public:
    SHIPPOINTER();
    ~SHIPPOINTER();
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time) const;
    uint64_t ProcessMessage(MESSAGE &message) override;

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

  protected:
    void UpdateShipPointer() const;
    VAI_OBJBASE *FindShipByChrIndex(long chrIdx) const;

    bool m_bVisible;
    bool m_bFriend;

    long m_idFriendTex;
    long m_idEnemyTex;

    long m_idVBuf;

    float m_fShiftVal;
    float m_fShiftSpeed;
    float m_fShiftAmp;
    float m_fShiftWSize;
    float m_fShiftHSize;
    float m_fShiftTop;

    VAI_OBJBASE *m_pShip;
};
