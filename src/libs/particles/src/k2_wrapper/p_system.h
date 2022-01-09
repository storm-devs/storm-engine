#pragma once

#include "c_vector.h"
#include "matrix.h"
#include "v_module_api.h"
#include "v_particle_system.h"

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
    void SetEmitter(CVECTOR p, CVECTOR a) override;
    void LinkToObject(entid_t id, CVECTOR _LinkPos) override;
    void SetDelay(int32_t _delay) override;
    void SetLifeTime(uint32_t time) override;
    void StopEmitter() override;
    void Pause(bool _bPause) override;

  public:
    void SetManager(PARTICLES *_pManager);
    IParticleSystem *GetSystem() const;
};
