#ifndef _TSINK_H_
#define _TSINK_H_

#include "..\SoundService\VSoundService.h"
#include "CVECTOR.h"
#include "TFlotsam.h"
#include "TIVBufferManager.h"
#include "TSinkSplash.h"
#include "dx8render.h"
#include "exs.h"
#include "geos.h"
#include "matrix.h"
#include "messages.h"
#include "model.h"
#include "sea_base.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TSink
{
  public:
    TSink();
    virtual ~TSink();

    void Initialize(INIFILE *_ini, IDirect3DDevice9 *_device, SEA_BASE *sea, VDX8RENDER *_renderer);
    void Release();
    void Start(const CVECTOR &_pos, float _radius);
    void Process(dword _dTime);
    void Realize(dword _dTime);
    bool Enabled();

  private:
    bool enabled;

    VDX8RENDER *renderer;
    SEA_BASE *sea;
    long texture;
    TIVBufferManager *ivManager;
    dword time;

    TSinkSplash splashes[MAX_SPLASHES];
    long times[MAX_SPLASHES];
    long ivIndexes[MAX_SPLASHES];
    long flotsamTimes[MAX_FLOTSAMS];
    TFlotsam flotsams[MAX_FLOTSAMS];
    float distortDivider;
    float radius;
    CVECTOR center;
};

#endif