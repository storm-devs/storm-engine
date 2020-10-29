#ifndef _ANIMALS_H_
#define _ANIMALS_H_

#include "TSeagulls.h"
#include "animation.h"
#include "dx8render.h"
#include "geometry.h"
#include "geos.h"
#include "matrix.h"
#include "messages.h"
#include "model.h"
#include "object.h"
//#include "TSharks.h"
#include "TButterflies.h"
#include "TFishSchools.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class ANIMALS : public ENTITY
{
  public:
    ANIMALS();
    virtual ~ANIMALS();

    virtual bool Init();
    virtual dword _cdecl ProcessMessage(MESSAGE &message);
    virtual void Realize(dword _dTime);
    virtual void Execute(dword _dTime);
    dword AttributeChanged(ATTRIBUTES *pA);
    // virtual bool CreateState(ENTITY_STATE_GEN * state_gen);
    // virtual bool LoadState(ENTITY_STATE * state);

  private:
    TSeagulls *seagulls;
    // TSharks      *sharks;
    TFishSchools *fishSchools;
    TButterflies *butterflies;
};

// API_MODULE_START("Animals")
//	CREATE_CLASS(ANIMALS)
// API_MODULE_END

#endif