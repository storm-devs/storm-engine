#pragma once

#include "Cvector.h"
#include "Matrix.h"
#include "dx9render.h"
#include "geometry.h"

#define MAX_MEASURE_POINTS 10
#define MAX_LEVELS 20
#define CARCASS_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

#define BOUND_UPPER(a, b)                                                                                              \
    if ((a) > (b))                                                                                                     \
        (a) = (b);
//--------------------------------------------------------------------
struct tCarcassVertex
{
    CVECTOR pos;
    uint32_t color;
    float u, v;
};

struct tMeasure
{
    float deltaPointX[MAX_MEASURE_POINTS];
    float deltaPointY[MAX_MEASURE_POINTS];
    int pointsCount;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TCarcass
{
  public:
    TCarcass(int _levelsCount, int _measurePointsCount, VDX9RENDER *_renderer, bool _normalsInverted = false);
    virtual ~TCarcass();

    void InitCircleMeasure(float _d, float _kx, float _ky);
    bool Initialize();
    void Uninitialize();
    void Execute(uint32_t dTime, CMatrix &_mtx, const CVECTOR *_starts);
    void Realize(const char *_technique);

    void SetSpeed(float _uSpeed, float _vSpeed, float _speedA);

  private:
    void RebuildLevels(tCarcassVertex *_vBuffer, bool _firstDraw, uint32_t dTime);
    void RebuildIndexes(uint16_t *_iBuffer);

    bool normalsInverted;
    tMeasure measure;
    CVECTOR levelStarts[MAX_LEVELS];
    int levelsCount;
    CMatrix sceneMatrix;
    long ivElementIndex;
    bool indexesCreated;
    uint32_t time;

    float uSpeed, vSpeed, speedA;
    VDX9RENDER *renderer;
    int iBuffer, vBuffer;
};
