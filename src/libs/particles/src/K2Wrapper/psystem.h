#pragma once

#include "vparticle_system.h"

class IParticleSystem;
class PARTICLES;

class PARTICLE_SYSTEM : public VPARTICLE_SYSTEM
{
    IParticleSystem *pSystem;
    PARTICLES *pManager;

  public:
    PARTICLE_SYSTEM(IParticleSystem *_pSystem);
    ~PARTICLE_SYSTEM() override;
    void Stop() override;
    void SetEmitter(Vector p, Vector a) override;
    void LinkToObject(entid_t id, Vector _LinkPos) override;
    void SetDelay(long _delay) override;
    void SetLifeTime(uint32_t time) override;
    void StopEmitter() override;
    void Pause(bool _bPause) override;

  public:
    void SetManager(PARTICLES *_pManager);
    IParticleSystem *GetSystem() const;
};
