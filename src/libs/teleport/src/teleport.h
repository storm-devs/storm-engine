#pragma once

#include "dx9render.h"
#include "vmodule_api.h"

class TMPTELEPORT : public Entity
{
    VDX9RENDER *rs;

    struct TELEPORT_DESCR
    {
        int num;
        char *name;
    } * m_descrArray;

  public:
    TMPTELEPORT();
    ~TMPTELEPORT() override;
    bool Init() override;
    void Execute(uint32_t Delta_Time);
    void Realize(uint32_t Delta_Time);
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
    void ReleaseAll();
    void SetShowData(ATTRIBUTES *pA);
    void SortShowData();
    void XChange(TELEPORT_DESCR &d1, TELEPORT_DESCR &d2);

  protected:
    long m_leftPos;
    long m_topPos;
    long m_deltaPos;
    long m_showStrQuantity;

    long m_nStrQuantity;
    long m_nCurStr;
    long m_nCurShowPos;

    long m_nShowType;
};

class FINDFILESINTODIRECTORY : public Entity
{
  public:
    FINDFILESINTODIRECTORY()
    {
    }

    ~FINDFILESINTODIRECTORY() override
    {
    }

    bool Init() override;

    void ProcessStage(Stage, uint32_t) override
    {
    }
};

class FINDDIALOGNODES : public Entity
{
  public:
    FINDDIALOGNODES()
    {
    }

    ~FINDDIALOGNODES() override
    {
    }

    bool Init() override;

    void ProcessStage(Stage, uint32_t) override
    {
    }
};
