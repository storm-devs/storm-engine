#include "psystem.h"
#include "math3d/Matrix.h"
#include "particles.h"
#include "particles/iparticlesystem.h"

PARTICLE_SYSTEM::PARTICLE_SYSTEM(IParticleSystem *_pSystem)
{
    pManager = nullptr;
    pSystem = _pSystem;
}

PARTICLE_SYSTEM::~PARTICLE_SYSTEM()
{
    pManager->DeleteResource(this);

    if (pSystem)
        pSystem->Release();
    pSystem = nullptr;
}

void PARTICLE_SYSTEM::Stop()
{
}

void PARTICLE_SYSTEM::SetEmitter(CVECTOR p, CVECTOR a)
{
    // core.Trace("K2 Particles Wrapper: SetEmitter (%3.2f, %3.2f, %3.2f, %3.2f, %3.2f, %3.2f)", p.x, p.y, p.z, a.x,
    // a.y, a.z);
    Vector pos, angles;
    pos.x = p.x;
    pos.y = p.y;
    pos.z = p.z;
    angles.x = a.x;
    angles.y = a.y;
    angles.z = a.z;
    const Matrix mTransform(angles, pos);
    pSystem->SetTransform(mTransform);
}

void PARTICLE_SYSTEM::LinkToObject(entid_t id, CVECTOR _LinkPos)
{
}

void PARTICLE_SYSTEM::SetDelay(long _delay)
{
}

void PARTICLE_SYSTEM::SetLifeTime(uint32_t time)
{
}

void PARTICLE_SYSTEM::StopEmitter()
{
}

void PARTICLE_SYSTEM::Pause(bool _bPause)
{
    pSystem->PauseEmission(_bPause);
}

void PARTICLE_SYSTEM::SetManager(PARTICLES *_pManager)
{
    pManager = _pManager;
}

IParticleSystem *PARTICLE_SYSTEM::GetSystem() const
{
    return pSystem;
}
