#pragma once

#include "Entity.h"
#include "stringService.h"

class OBJ_STRSERVICE : public Entity
{
    VSTRSERVICE *m_pStrService;

  public:
    OBJ_STRSERVICE();
    ~OBJ_STRSERVICE() override;
    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage, uint32_t) override
    {
    }
};
