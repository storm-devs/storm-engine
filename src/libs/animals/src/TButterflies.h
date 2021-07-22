#pragma once

#include "AnimalsDefines.h"
#include "Animation.h"
#include "IVBufferManager.h"
#include "Matrix.h"
#include "TButterfly.h"
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

    uint64_t ProcessMessage(long _code, MESSAGE &message);
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
    long butterfliesCount;

    float maxDistance;

    bool enabled;
    long yDefineTime;
    long texture;
};
