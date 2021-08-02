#pragma once

#include "AnimalsDefines.h"
#include "Matrix.h"
#include "TDynamicObject.h"
#include "TDynamicSystem.h"
#include "dx9render.h"
#include "geometry.h"
#include "sea_base.h"
#include "ship_base.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
class TFishSchool : public TDynamicObject
{
  public:
    float depth;
    uint32_t time;
    float timeDivider;
    float amplitude;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TFishSchools : public TDynamicSystem
{
    friend class TFishSchool;

  public:
    TFishSchools();
    ~TFishSchools() override;

    uint64_t ProcessMessage(const long _code, MESSAGE &message);
    void Init();
    void Realize(const uint32_t dTime);
    void Execute(const uint32_t dTime);

  private:
    void LoadSettings();

    VDX9RENDER *renderService;
    entid_t fishSchoolModel;
    std::vector<std::unique_ptr<TFishSchool>> fishSchools;
    long shipsCount;
    long fishSchoolsCount;
    std::unique_ptr<TDynamicObject> cameraObject = std::make_unique<TDynamicObject>();

    float maxDistance;

    bool enabled;
    SEA_BASE *sea;
};
