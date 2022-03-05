#pragma once

#include "defines.h"
#include "sd2_h/cannon_trace.h"

class ISLAND_BASE : public CANNON_TRACE_BASE
{
  public:
    virtual entid_t GetModelEID() = 0;

    virtual bool Check2DBoxDepth(CVECTOR vPos, CVECTOR vSize, float fAngY, float fMinDepth) = 0;
    virtual bool GetDepth(float x, float z, float *fRes = nullptr) = 0;
    virtual bool GetDepthFast(float x, float z, float *fRes = nullptr) = 0;
    virtual bool GetDepth(FRECT *pRect, float *fMinH, float *fMaxH) = 0;

    virtual bool GetShadow(float x, float z, float *fRes = nullptr) = 0;

    virtual bool GetMovePoint(CVECTOR &vSrc, CVECTOR &vDst, CVECTOR &vRes) = 0;

    virtual entid_t GetSeabedEID() = 0;

    virtual float GetCurrentImmersion() = 0;
};
