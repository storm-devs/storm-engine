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

    uint64_t ProcessMessage(const long _code, MESSAGE &message);
    void Init();
    void Realize(const uint32_t dTime);
    void Execute(const uint32_t dTime);

  private:
    void LoadSettings();

    VDX9RENDER *renderService;
    COLLIDE *collide;
    IVBufferManager *ivManager;
    entid_t butterflyModel;
    std::vector<std::unique_ptr<TButterfly>> butterflies;
    long butterfliesCount;

    float maxDistance;

    bool enabled;
    long yDefineTime;
    long texture;
};
