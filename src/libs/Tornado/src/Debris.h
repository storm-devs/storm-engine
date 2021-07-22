//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Debris
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "Pillar.h"
#include "VSoundService.h"
#include "model.h"

class Debris
{
    struct ModelInfo
    {
        float a, r;
        float y, ay;
        float alpha;
        float scale;
        float maxSpeed;
        CVECTOR ang;
        MODEL *mdl;
    };

    struct Model
    {
        MODEL *mdl;
        float prt;
        float maxSpeed;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Debris(Pillar &_pillar);
    virtual ~Debris();

    void Init();

    void Update(float dltTime);
    void Draw(VDX9RENDER *rs);

    void SetGlobalAlpha(float a);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void AddModel(const char *modelNamem, float prt, float spd);
    void NormalazedModels();
    MODEL *SelectModel(float &maxSpd);
    bool IsShip();

  private:
    VSoundService *soundService;
    float lastPlayTime;
    Pillar &pillar;
    float galpha;
    Model mdl[16];  // Uploaded models
    long numModels; // Number of models

    ModelInfo fly[64]; // Flying models
    long flyCounter;   // Number of flying models
};
