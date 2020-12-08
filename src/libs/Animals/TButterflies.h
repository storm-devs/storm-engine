#ifndef _TBUTTERFLIES_H_
#define _TBUTTERFLIES_H_

#include "AnimalsDefines.h"
#include "TButterfly.h"
#include "TIVBufferManager.h"
#include "animation.h"
#include "collide.h"
#include "dx8render.h"
#include "geometry.h"
#include "geos.h"
#include "matrix.h"
#include "messages.h"
#include "model.h"
#include "object.h"

#define Y_REDEFINE_TIME 250

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TButterflies
{
  public:
    TButterflies();
    virtual ~TButterflies();

    dword ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Realize(dword _dTime);
    void Execute(dword _dTime);

  private:
    void LoadSettings();

    VDX8RENDER *renderService;
    COLLIDE *collide;
    VIDWALKER *walker;
    TIVBufferManager *ivManager;
    ENTITY_ID butterflyModel;
    TButterfly butterflies[BUTTERFLY_COUNT];
    long butterfliesCount;

    float maxDistance;

    bool enabled;
    long yDefineTime;
    long texture;
};

#endif // !defined
