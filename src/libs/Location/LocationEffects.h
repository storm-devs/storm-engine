//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocationEffects
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Entity.h"
#include "Matrix.h"
#include "vmodule_api.h"

class MESSAGE;
class VDX9RENDER;

#define LFX_SPLASHES_SECT 16

class LocationEffects : public Entity
{
#pragma pack(push, 1)

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

#pragma pack(pop)

    struct Particle
    {
        CVECTOR pos;
        float angle;
        float size;
        float alpha;
    };

    struct ParticleEx : public Particle
    {
        uint32_t color;
        float frame;
    };

    struct ParticleSplash : public Particle
    {
        CVECTOR dir;
        float dAng;
    };

    struct ChrSplash
    {
        float time;
        float kTime;
        CVECTOR pos;
        ParticleSplash prt[64];
    };

    struct ParticleFly : public ParticleEx
    {
        float ax, ay;
        float kx, ky;
        float a, k;
    };

    struct LampFlys
    {
        CVECTOR pos;
        float radius;
        long start;
        long num;
    };

    struct ParticleSG : public Particle
    {
        CVECTOR spd;
        float dang;
        float time;
        float ktime;
    };

    // --------------------------------------------------------------------------------------------
    // construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LocationEffects();
    virtual ~LocationEffects();

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void DrawParticles(void *prts, long num, long size, long texture, const char *tech, bool isEx = false,
                       long numU = 0);

  private:
    VDX9RENDER *rs;

    // ---------------------------------------------------
    // Ѕplashes from the character
    // ---------------------------------------------------
    void CreateSplash(const CVECTOR &pos, float power);
    void ProcessedChrSplash(float dltTime);

    ChrSplash chrSplash[4];
    long chrSplashRefCounter;
    long splashesTxt;

    // ---------------------------------------------------
    // Flies near laterns
    // ---------------------------------------------------

    void AddLampFlys(CVECTOR &pos);
    void ProcessedFlys(float dltTime);

    std::vector<LampFlys> flys;
    long numFlys;
    long maxFlys;
    std::vector<ParticleFly> fly;
    long numFly;
    long flyTex;

    // ---------------------------------------------------
    // Shotgun particles
    // ---------------------------------------------------
    void SGInited();
    void SGRelease();
    void SGEnvPrt(const CVECTOR &pos, const CVECTOR &ndir);
    void SGBldPrt(const CVECTOR &pos, const CVECTOR &ndir);
    void SGFirePrt(const CVECTOR &pos, const CVECTOR &ndir);
    void ProcessedShotgun(float dltTime);

    ParticleSG smoke[64];
    bool isShgInited;
    long numSmoke;
    long texSmoke;
    ParticleSG flinders[256];
    long numFlinders;
    long texFlinders;
    ParticleSG blood[256];
    long numBlood;
    long texBlood;
    long texHor;

    Vertex buffer[256 * 6];
};
