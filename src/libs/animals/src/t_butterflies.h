#pragma once

#include "animals_defines.h"
#include "animation.h"
#include "iv_buffer_manager.h"
#include "matrix.h"
#include "t_butterfly.h"
#include "collide.h"
#include "dx9render.h"
#include "geometry.h"

#define Y_REDEFINE_TIME 250

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TButterflies
{
  public:
    TButterflies();
    virtual ~TButterflies();

    uint64_t ProcessMessage(int32_t _code, MESSAGE &message);
    void Init();
    void Realize(uint32_t dTime);
    void Execute(uint32_t dTime);

  private:
    void LoadSettings();

    VDX9RENDER *renderService;
    COLLIDE *collide;
    IVBufferManager *ivManager;
    entid_t butterflyModel;
    TButterfly butterflies[BUTTERFLY_COUNT];
    int32_t butterfliesCount;

    float maxDistance;

    bool enabled;
    int32_t yDefineTime;
    int32_t texture;
};
