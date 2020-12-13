#pragma once

#include "collide.h"
#include "dx9render.h"
#include "model.h"
#include "sea_base.h"
#include "vmodule_api.h"
//#include "..\geom_lib\geos.h"
#include "TIVBufferManager.h"

#define MAX_RINGS 25
#define FADE_IN_TIME 200
#define FADE_OUT_TIME 1200

#define GRID_STEPS_COUNT 3
#define TRIANGLES_COUNT ((GRID_STEPS_COUNT - 1) * (GRID_STEPS_COUNT - 1) * 2)
#define Y_DELTA .01f

#define RING_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)
struct RING_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;
};

enum tRingState
{
    RING_WALK,
    RING_RUN,
    RING_SWIM
};
struct tRing
{
    bool active;
    long activeTime;
    long ivIndex;
    float x, z;
    tRingState state;
    bool firstUpdate;
    float cosA, sinA;
};

class WaterRings : public Entity
{
  public:
    WaterRings();
    ~WaterRings();
    bool Init();
    void Realize(uint32_t dTime);
    uint64_t ProcessMessage(MESSAGE &message);
    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        // case Stage::execute:
        //	Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }

  private:
    void UpdateGrid(int _ringI, uint16_t *iPointer, RING_VERTEX *vPointer, long vOffset);

    VDX9RENDER *renderService;
    SEA_BASE *sea;
    TIVBufferManager *ivManager;
    long ringTexture;
    tRing rings[MAX_RINGS];
};
