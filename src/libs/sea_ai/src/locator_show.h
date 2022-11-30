#pragma once

#include "ai_helper.h"

class SeaLocatorShow : public Entity
{
    ATTRIBUTES *pALocators;
    bool bShow;
    float fScale;

    float fWidth, fHeight;
    CMatrix view, mtx;

    struct SphVertex
    {
        CVECTOR v;
        uint32_t c;
    };

    uint32_t sphereNumTrgs;
    SphVertex *sphereVertex;

    void CreateSphere();

    bool isLocator(ATTRIBUTES *pA);
    CVECTOR GetLocatorPos(ATTRIBUTES *pA);
    float GetLocatorAng(ATTRIBUTES *pA);
    float GetLocatorRadius(ATTRIBUTES *pA);
    const char *GetRealLocatorName(ATTRIBUTES *pA);
    const char *GetLocatorName(ATTRIBUTES *pA);
    const char *GetLocatorGroupName(ATTRIBUTES *pA);

    void PrintLocator(ATTRIBUTES *pA);
    void ProcessLocators(ATTRIBUTES *pA);

  public:
    SeaLocatorShow();
    ~SeaLocatorShow() override;

    bool Init() override;
    void SetDevice();

    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time) const;

    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);

    void ProcessMessage(uint32_t iMsg, uint32_t wParam, uint32_t lParam);
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

    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;
};
