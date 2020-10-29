#ifndef _BALLSPLASH_H_
#define _BALLSPLASH_H_

#include "BallSplashDefines.h"
#include "CVECTOR.h"
#include "TSplash.h"
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
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class BALLSPLASH : public ENTITY
{
  public:
    BALLSPLASH();
    virtual ~BALLSPLASH();

    virtual bool Init();
    virtual dword _cdecl ProcessMessage(MESSAGE &message);
    virtual void Realize(dword _dTime);
    virtual void Execute(dword _dTime);

  private:
    void InitializeSplashes();
    TSplash *TryToAddSplash(const CVECTOR &_pos, const CVECTOR &_dir);

    TSplash splashes[MAX_SPLASHES];
    VDX8RENDER *renderer;
    SEA_BASE *sea;
};

#endif