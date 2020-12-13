#ifndef SEA_CAMERAS_HPP
#define SEA_CAMERAS_HPP

#include "ShipCamera.h"
#include <vector>

class SEA_CAMERAS : public Entity
{
    std::vector<COMMON_CAMERA *> CamerasArray;
    bool bActive;

  public:
    SEA_CAMERAS();
    virtual ~SEA_CAMERAS();

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

/*
API_MODULE_START("SEA_CAMERAS")
    CREATE_CLASS(0,SEA_CAMERAS)
    CREATE_CLASS(1,FREE_CAMERA)
    CREATE_CLASS(2,SHIP_CAMERA)
    CREATE_CLASS(3,DECK_CAMERA)
API_MODULE_END
*/

#endif
