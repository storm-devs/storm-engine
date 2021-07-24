#pragma once

#include "ShipCamera.h"
#include <vector>

class SEA_CAMERAS : public Entity
{
    std::vector<COMMON_CAMERA *> CamerasArray;
    bool bActive;

  public:
    SEA_CAMERAS();
    ~SEA_CAMERAS() override;

    void ProcessMessage(uint32_t iMsg, uint32_t wParam, uint32_t lParam);
    uint64_t ProcessMessage(MESSAGE &message) override;

    bool Init() override
    {
        return true;
    }

    void ProcessStage(Stage, uint32_t) override
    {
    }
};
