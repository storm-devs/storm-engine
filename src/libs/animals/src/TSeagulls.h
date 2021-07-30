#pragma once

#include "AnimalsDefines.h"
#include "math3d.h"
#include "VSoundService.h"
#include "dx9render.h"
#include "message.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
struct tSeagull
{
    Vector center;
    float radius;
    float va;
    float height;
    float deltaA;
    float a;

    long circleTime;
    long circleTimePassed;
    long screamTime;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TSeagulls
{
  public:
    TSeagulls();
    virtual ~TSeagulls();

    uint64_t ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Add(float _x, float _y, float _z);
    void Realize(uint32_t dTime);
    void Execute(uint32_t dTime);

    void SetStartY(float _startY)
    {
        startY = _startY;
    }

  private:
    void LoadSettings();
    void Frighten();

    entid_t seagullModel;
    tSeagull seagulls[SEAGULL_COUNT];
    VDX9RENDER *renderService;
    VSoundService *soundService;
    bool enabled;
    long count;
    float maxDistance;
    float maxRadius;
    float maxAngleSpeed;
    float maxHeight;
    long countAdd;
    long maxCircleTime;
    long farChoiceChance;
    long relaxTime;
    bool frightened;
    long frightenTime;
    long screamTime;
    char screamFilename[256];

    Vector cameraPos, cameraAng;
    float startY;
};
