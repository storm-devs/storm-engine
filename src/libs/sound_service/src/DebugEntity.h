#pragma once

#include "Entity.h"

class SoundService;

class SoundVisualisationEntity : public Entity
{
    SoundService *pSound;

  public:
    SoundVisualisationEntity();
    ~SoundVisualisationEntity() override;

    bool Init() override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);

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
        }
    }

    void SetMasterSoundService(SoundService *pSoundService);
    void Wakeup() const;
};
