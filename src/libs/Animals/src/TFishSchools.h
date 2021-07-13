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
    virtual ~TFishSchools();

    uint64_t ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Realize(uint32_t dTime);
    void Execute(uint32_t dTime);

  private:
    void LoadSettings();

    VDX9RENDER *renderService;
    entid_t fishSchoolModel;
    TFishSchool *fishSchools[FISHSCHOOL_COUNT];
    long shipsCount;
    long fishSchoolsCount;
    TDynamicObject cameraObject;

    float maxDistance;

    bool enabled;
    SEA_BASE *sea;
};
