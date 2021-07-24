#pragma once

#include "Common_Camera.h"
#include "Island_Base.h"
#include "dx9render.h"

class FREE_CAMERA : public COMMON_CAMERA
{
  private:
    VDX9RENDER *pRS;
    CVECTOR vPos, vAng;
    float fFov;
    long iLockX, iLockY;

    ISLAND_BASE *pIslandBase;
    BOOL bCameraOnEarth;
    float fCameraOnEarthHeight;

  public:
    FREE_CAMERA();
    ~FREE_CAMERA() override;

    void SetDevice();
    bool Init() override;
    void Move(uint32_t DeltaTime);
    void Execute(uint32_t Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen) const;
    bool LoadState(ENTITY_STATE *state);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
            // case Stage::realize:
            //    Realize(delta); break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void Save(CSaveLoad *pSL) override;
    void Load(CSaveLoad *pSL) override;
};
