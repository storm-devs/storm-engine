#include "particle_system.h"

#include "string_compare.hpp"
#include "../emitters/point.h"

// Global GUID to assign to emitters
uint32_t EmitterID = 0;

// Create / delete
ParticleSystem::ParticleSystem(ParticleManager *serv)
{
    AutoDeleted = false;
    pMaster = serv;
    EmissionPause = false;
}

ParticleSystem::~ParticleSystem()
{
    pMaster->RemoveResource(this);
    DeleteAllEmitters();
}

bool ParticleSystem::Release()
{
    delete this;
    return false;
}

// Execute all particles
uint32_t ParticleSystem::Execute(float DeltaTime)
{
    if (AutoDeleted)
    {
        if (DeleteIfNeed())
        {
            // core.Trace("AUTO DELETE !!!!\n");
            return 0;
        }
    }

    if (EmissionPause)
        return Emitters.size();

    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Execute(DeltaTime);
    }

    return Emitters.size();
}

void ParticleSystem::CreateFromDataSource(DataSource *pDataSource)
{
    const auto EmitterCount = pDataSource->GetEmitterCount();
    if (EmitterCount <= 0)
        return;

    for (auto n = 0; n < EmitterCount; n++)
    {
        auto *const pEmitterDecription = pDataSource->GetEmitterDesc(n);
        IEmitter *pEmitter = nullptr;
        switch (pEmitterDecription->Type)
        {
        case POINT_EMITTER:
            pEmitter = CreatePointEmitter(pEmitterDecription);
            break;
        default:
            throw std::runtime_error("Particles: Unknown emitter type !!!!");
        }

        // Assert (pEmitter);

        // CreateParticles (pEmitter, pEmitterDecription->Particles);
    }
}

IEmitter *ParticleSystem::CreatePointEmitter(DataSource::EmitterDesc *pEmitter)
{
    //    core.Trace("Create 'Point' emitter\n");
    auto *pNewEmitter = new PointEmitter(this);
    pNewEmitter->AttachToDataSource(pEmitter);
    pNewEmitter->SetGUID(EmitterID);

    EmitterDesc NewEmitter;
    NewEmitter.Type = POINT_EMITTER;
    NewEmitter.pEmitter = pNewEmitter;
    Emitters.push_back(NewEmitter);

    EmitterID += GUIDSTEP;

    return pNewEmitter;
}

void ParticleSystem::DeleteAllEmitters()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        delete Emitters[n].pEmitter;
    }

    Emitters.clear();
}

ParticleManager *ParticleSystem::GetMaster() const
{
    return pMaster;
}

// Restart particle system
void ParticleSystem::Restart(uint32_t RandomSeed)
{
    srand(RandomSeed);
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Restart();
    }

    PauseEmission(false);
}

// Pause particle emission
void ParticleSystem::PauseEmission(bool bPause)
{
    EmissionPause = bPause;
}

// Find out whether emission is paused or not
bool ParticleSystem::IsEmissionPaused()
{
    return EmissionPause;
}

bool ParticleSystem::DeleteIfNeed()
{
    if (!IsAlive())
    {
        pMaster->DefferedDelete(this);
        return true;
    }
    return false;
}

// Set whether auto-delete system or regular ...
void ParticleSystem::AutoDelete(bool Enabled)
{
    AutoDeleted = Enabled;
}

// Find out if auto-delete system or not
bool ParticleSystem::IsAutoDeleted()
{
    return AutoDeleted;
}

// Set transformation matrix for system
void ParticleSystem::SetTransform(const Matrix &transform)
{
    matWorld = transform;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->SetTransform(transform);
    }
}

void ParticleSystem::Teleport(const Matrix &transform)
{
    matWorld = transform;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Teleport(transform);
    }
}

void ParticleSystem::GetTransform(Matrix &_matWorld)
{
    _matWorld = matWorld;
}

IEmitter *ParticleSystem::FindEmitterByData(FieldList *Data)
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        if (Emitters[n].pEmitter->GetData() == Data)
            return Emitters[n].pEmitter;
    }

    return nullptr;
}

IEmitter *ParticleSystem::FindEmitter(const char *name)
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        if (storm::iEquals(Emitters[n].pEmitter->GetName(), name))
            return Emitters[n].pEmitter;
    }

    return nullptr;
}

uint32_t ParticleSystem::GetEmittersCount() const
{
    return Emitters.size();
}

IEmitter *ParticleSystem::GetEmitterByIndex(uint32_t Index)
{
    return Emitters[Index].pEmitter;
}

EmitterType ParticleSystem::GetEmitterTypeByIndex(uint32_t Index)
{
    return Emitters[Index].Type;
}

bool ParticleSystem::IsAlive()
{
    uint32_t pCount = 0;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        pCount += Emitters[n].pEmitter->GetParticleCount();
        if (!Emitters[n].pEmitter->IsStoped())
            return true;
    }

    if (pCount > 0)
        return true;

    return false;
}

const char *ParticleSystem::GetName()
{
    return SystemName.c_str();
}

void ParticleSystem::SetName(const char *Name)
{
    SystemName = Name;
}

void ParticleSystem::Editor_UpdateCachedData()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Editor_UpdateCachedData();
    }
}

void ParticleSystem::Stop()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Stop();
    }
}
