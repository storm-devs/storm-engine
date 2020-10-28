#ifndef _TSHARKS_H_
#define _TSHARKS_H_

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
class TShark : public TDynamicObject
{
  public:
    float depth;
    dword time;
};

class TShip : public TDynamicObject
{
  public:
    SHIP_BASE *ship;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TSharks : public TDynamicSystem
{
    friend class TShark;
    friend class TShip;

  public:
    TSharks();
    virtual ~TSharks();

    dword ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Realize(dword _dTime);
    void Execute(dword _dTime);

  private:
    void LoadSettings();

    VDX8RENDER *renderService;
    ENTITY_ID sharkModel;
    TShark *sharks[SHARK_COUNT];
    TShip *ships[SHARK_MAX_SHIPS];
    long shipsCount;
    long sharksCount;
    TDynamicObject cameraObject;

    float maxDistance;

    bool enabled;
    ENTITY_ID seaID;
    SEA_BASE *sea;
};

#endif // !defined
