#pragma once
#include <vector>
#include <memory>

#include "AnimalsDefines.h"
#include "Matrix.h"
#include "VSoundService.h"
#include "dx9render.h"
#include "message.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
struct TSeagull
{
    CVECTOR center;
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

    uint64_t ProcessMessage(const long _code, MESSAGE &message);
    void Init();
    void Add(const float _x, const float _y, const float _z);
    void Realize(const uint32_t dTime);
    void Execute(const uint32_t dTime);

    void SetStartY(const float _startY)
    {
        startY = _startY;
    }

  private:
    void LoadSettings();
    void Frighten();

    entid_t seagullModel;
    std::vector<TSeagull> seagulls;
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

    CVECTOR cameraPos, cameraAng;
    float startY;
};
