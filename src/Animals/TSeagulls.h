#ifndef _TSEAGULLS_H_
#define _TSEAGULLS_H_

#include "..\SoundService\VSoundService.h"
#include "AnimalsDefines.h"
#include "animation.h"
#include "dx8render.h"
#include "geometry.h"
#include "geos.h"
#include "matrix.h"
#include "messages.h"
#include "model.h"
#include "object.h"

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

    dword ProcessMessage(long _code, MESSAGE &message);
    void Init();
    void Add(float _x, float _y, float _z);
    void Realize(dword _dTime);
    void Execute(dword _dTime);
    void SetStartY(float _startY)
    {
        startY = _startY;
    }

  private:
    void LoadSettings();
    void Frighten();

    ENTITY_ID seagullModel;
    tSeagull seagulls[SEAGULL_COUNT];
    VDX8RENDER *renderService;
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

#endif // !defined
