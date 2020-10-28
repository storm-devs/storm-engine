#ifndef _TFISHSCHOOLS_H_
#define _TFISHSCHOOLS_H_

#include "AnimalsDefines.h"
#include "TDynamicObject.h"
#include "TDynamicSystem.h"
#include "animation.h"
#include "dx8render.h"
#include "geometry.h"
#include "geos.h"
#include "matrix.h"
#include "messages.h"
#include "model.h"
#include "object.h"
#include "sea_base.h"
#include "ship_base.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
class TFishSchool : public TDynamicObject
{
  public:
    float depth;
    dword time;
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

    dword ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Realize(dword _dTime);
    void Execute(dword _dTime);

  private:
    void LoadSettings();

    VDX8RENDER *renderService;
    ENTITY_ID fishSchoolModel;
    TFishSchool *fishSchools[FISHSCHOOL_COUNT];
    long shipsCount;
    long fishSchoolsCount;
    TDynamicObject cameraObject;

    float maxDistance;

    bool enabled;
    ENTITY_ID seaID;
    SEA_BASE *sea;
};

#endif // !defined
