#pragma once

#include "collide.h"
#include "dx9render.h"
#include "model.h"
#include "sea_base.h"
#include "vma.hpp"
//#include "..\geom_lib\geos.h"
#include "iv_buffer_manager.h"

namespace waterrings
{
constexpr int MAX_RINGS = 25;
constexpr int FADE_IN_TIME = 200;
constexpr int FADE_OUT_TIME = 1200;

constexpr int GRID_STEPS_COUNT = 3;
constexpr int TRIANGLES_COUNT = ((GRID_STEPS_COUNT - 1) * (GRID_STEPS_COUNT - 1) * 2);
constexpr float Y_DELTA = .01f;

constexpr int RING_FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
} // namespace waterrings

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
    int32_t activeTime;
    int32_t ivIndex;
    float x, z;
    tRingState state;
    bool firstUpdate;
    float cosA, sinA;
};

class WaterRings : public Entity
{
  public:
    WaterRings();
    ~WaterRings() override;
    bool Init() override;
    void Realize(uint32_t dTime);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
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
    void UpdateGrid(int _ringI, uint16_t *iPointer, RING_VERTEX *vPointer, int32_t vOffset);

    VDX9RENDER *renderService;
    SEA_BASE *sea;
    IVBufferManager *ivManager;
    int32_t ringTexture;
    tRing rings[waterrings::MAX_RINGS];
};
