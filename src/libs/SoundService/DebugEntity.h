#ifndef _SOUND_VISUALISATION_H_
#define _SOUND_VISUALISATION_H_

#include "dx9render.h"
#include "entity.h"

class SoundService;

class SoundVisualisationEntity : public Entity
{
    SoundService *pSound;

  public:
    SoundVisualisationEntity();
    virtual ~SoundVisualisationEntity();

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
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void SetMasterSoundService(SoundService *pSoundService);
    void Wakeup() const;
};

#endif
