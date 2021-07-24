#pragma once

#include "Cvector.h"
#include "EntityManager.h"
#include "IVBufferManager.h"
#include "collide.h"
#include "model.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
#define MAX_ACTIVITY_TIME 20000
#define MAX_WAIT_TIME 20000
#define MAX_EFFECT_RADIUS 1.5f
#define VELOCITY 1.1f // [m/sec]
#define MIN_Y_DELTA 0.05f
#define MIN_ACTIVITY 0.4f
#define RISE_IMPROBABILITY 6 // improbability of back effect
#define FALL_IMPROBABILITY 10
#define MAX_DISPLACE_TIME 300
#define DISPLACE_SPEED 10.0f // [m/(sec^2)]
#define MAX_REMOTE_DISTANCE 10.0f
#define MAX_HEIGHT 4.0f
#define MODEL_SIDE 0.05f
#define WINGS_TIME_K 30.0f
#define Y_SPEED 1.0f
#define SINGLE_SIZE 0.25f

#define BUTTERFLY_VERTEX_TYPE (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

#pragma pack(push, 1)
struct tButterflyVertex
{
    CVECTOR pos;
    float tu, tv;
};
#pragma pack(pop)
//--------------------------------------------------------------------
class TButterfly
{
  public:
    TButterfly();
    virtual ~TButterfly();

    void Initialize(const CVECTOR &_center, float _radius, long _bufferIndex, int _tI, int _tJ);
    void Calculate(long _dTime, COLLIDE *_collide, EntityManager::LayerIterators its);
    void Effect(const CVECTOR &_position);

    static void SetCenter(const CVECTOR &_center)
    {
        center = _center;
    };

    const CVECTOR GetDirection()
    {
        return centerVelocity;
    }

    const CVECTOR GetPosition()
    {
        return centerPosition;
    }

    void SetMinY(float _minY)
    {
        minY = _minY;
        maxY = minY + MAX_HEIGHT;
    }

    void Draw(HDC _dc);
    void Draw(IVBufferManager *_ivManager);
    void Draw(VDX9RENDER *_renderer, MODEL *_model);

  private:
    CVECTOR centerPosition, centerVelocity, oldPos;
    CVECTOR displaceVector;
    long timeToNextDisplace;
    long fullActiveTime, activeTime, waitTime;
    long bufferIndex;
    bool active;

    static CVECTOR center;
    float minY, maxY;
    bool firstDraw;
    float time;

    float tI, tJ;
};

//--------------------------------------------------------------------
