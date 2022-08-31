#pragma once

#include "c_vector.h"
#include "dx9render.h"
#include "sea_base.h"

//#include "../sound_service/v_sound_service.h"

class INIFILE;
///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct GRID_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;
};

struct GRID_VERTEX2
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;
    float tu2, tv2;
};

class TSplash
{
  public:
    TSplash();
    virtual ~TSplash();

    void Initialize(INIFILE *_ini, IDirect3DDevice9 *_device, SEA_BASE *sea, VDX9RENDER *_renderer);
    void Release();
    void Start(const CVECTOR &_pos, const CVECTOR &_dir);
    bool Process(uint32_t dTime);
    bool Process2(uint32_t dTime);
    void PostProcess();
    void PostProcess2();
    void Realize(uint32_t dTime);
    void Realize2(uint32_t dTime);
    bool Enabled();

    static bool startRender;
    static uint64_t lockTicks, fillTicks, unlockTicks, realizeTicks;
    static uint32_t processCount, topIndex, topIndex2;

  private:
    float HeightF(uint32_t time, float _r, float _k);

    bool enabled;

    static VDX9RENDER *renderer;
    SEA_BASE *sea;
    IDirect3DDevice9 *device;

    static int32_t vBuffer, iBuffer, texture, texture2, vBuffer2, iBuffer2;
    static int buffersUsage;
    uint32_t time;

    float width2;
    float distortDivider;
    CVECTOR center, dir;
    float growK;
    float height, midY;
};
