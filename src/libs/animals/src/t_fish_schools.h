#pragma once

#include "animals_defines.h"
#include "matrix.h"
#include "t_dynamic_object.h"
#include "t_dynamic_system.h"
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

    uint64_t ProcessMessage(int32_t _code, MESSAGE &message);
    void Init();
    void Realize(uint32_t dTime);
    void Execute(uint32_t dTime);

  private:
    void LoadSettings();

    VDX9RENDER *renderService;
    entid_t fishSchoolModel;
    TFishSchool *fishSchools[FISHSCHOOL_COUNT];
    int32_t shipsCount;
    int32_t fishSchoolsCount;
    TDynamicObject cameraObject;

    float maxDistance;

    bool enabled;
    SEA_BASE *sea;
};
