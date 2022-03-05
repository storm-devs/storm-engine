#pragma once

#include "animals_defines.h"
#include "matrix.h"
#include "v_sound_service.h"
#include "dx9render.h"
#include "message.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
struct tSeagull
{
    CVECTOR center;
    float radius;
    float va;
    float height;
    float deltaA;
    float a;

    int32_t circleTime;
    int32_t circleTimePassed;
    int32_t screamTime;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TSeagulls
{
  public:
    TSeagulls();
    virtual ~TSeagulls();

    uint64_t ProcessMessage(int32_t _code, MESSAGE &message);
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
    int32_t count;
    float maxDistance;
    float maxRadius;
    float maxAngleSpeed;
    float maxHeight;
    int32_t countAdd;
    int32_t maxCircleTime;
    int32_t farChoiceChance;
    int32_t relaxTime;
    bool frightened;
    int32_t frightenTime;
    int32_t screamTime;
    char screamFilename[256];

    CVECTOR cameraPos, cameraAng;
    float startY;
};
