#pragma once

#include "Cvector.h"
#include "VSoundService.h"
#include "dx9render.h"
#include "sea_base.h"
#include "seps.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct SINK_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;
};

class TSinkSplash
{
  public:
    TSinkSplash();
    virtual ~TSinkSplash();

    void Initialize(INIFILE *_ini, SEA_BASE *sea);
    void Release();
    void Start(const CVECTOR &_pos, uint16_t *_indexes, SINK_VERTEX *_vertexes, long vOffset);
    void Process(uint32_t dTime, uint16_t *_indexes, SINK_VERTEX *_vertexes);
    bool Enabled();
    void Reset(uint16_t *_indexes, SINK_VERTEX *_vertexes);
    void AdditionalRealize(uint32_t dTime);

  private:
    float HeightF(uint32_t time, float _r, float _k);

    SEPS_PS *ps;
    bool enabled;
    SEA_BASE *sea;
    uint32_t time;

    float distortDivider;
    CVECTOR center, dir;
    float growK;
};
