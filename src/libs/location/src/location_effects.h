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

#include "entity.h"
#include "matrix.h"
#include "v_module_api.h"

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
        int32_t start;
        int32_t num;
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
    ~LocationEffects() override;

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
    void DrawParticles(void *prts, int32_t num, int32_t size, int32_t texture, const char *tech, bool isEx = false,
                       int32_t numU = 0);

  private:
    VDX9RENDER *rs;

    // ---------------------------------------------------
    // Ѕplashes from the character
    // ---------------------------------------------------
    void CreateSplash(const CVECTOR &pos, float power);
    void ProcessedChrSplash(float dltTime);

    ChrSplash chrSplash[4];
    int32_t chrSplashRefCounter;
    int32_t splashesTxt;

    // ---------------------------------------------------
    // Flies near laterns
    // ---------------------------------------------------

    void AddLampFlys(CVECTOR &pos);
    void ProcessedFlys(float dltTime);

    std::vector<LampFlys> flys;
    int32_t numFlys;
    int32_t maxFlys;
    std::vector<ParticleFly> fly;
    int32_t numFly;
    int32_t flyTex;

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
    int32_t numSmoke;
    int32_t texSmoke;
    ParticleSG flinders[256];
    int32_t numFlinders;
    int32_t texFlinders;
    ParticleSG blood[256];
    int32_t numBlood;
    int32_t texBlood;
    int32_t texHor;

    Vertex buffer[256 * 6];
};
