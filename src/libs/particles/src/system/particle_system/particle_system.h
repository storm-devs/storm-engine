#pragma once

#include "../../i_common/types.h"
#include "../../manager/particle_manager.h"
#include "../data_source/data_source.h"
#include "particles/i_particle_system.h"
#include "v_module_api.h"

class IEmitter;

class ParticleSystem : public IParticleSystem
{
    struct EmitterDesc
    {
        EmitterType Type;
        IEmitter *pEmitter;
    };

    std::vector<EmitterDesc> Emitters;

    // Auto-delete system or regular
    bool AutoDeleted;

    // Do not emit new particles !!!
    bool EmissionPause;

    bool DeleteIfNeed();

    std::string SystemName;

  protected:
    ~ParticleSystem() override;

    IEmitter *CreatePointEmitter(DataSource::EmitterDesc *pEmitter);

    void DeleteAllEmitters();

    ParticleManager *pMaster;

    Matrix matWorld;

  public:
    // Create / delete
    ParticleSystem(ParticleManager *serv);
    bool Release() override;

    // Execute all particles
    uint32_t Execute(float DeltaTime) override;

    // Restart particle system
    void Restart(uint32_t RandomSeed) override;

    // Pause particle emission
    void PauseEmission(bool bPause) override;

    void Stop() override;
    // Find out whether emission is paused or not
    bool IsEmissionPaused() override;

    // Set whether auto-delete system or regular ...
    void AutoDelete(bool Enabled) override;
    // Find out if auto-delete system or not
    bool IsAutoDeleted() override;
    // Set transformation matrix for system
    void SetTransform(const Matrix &transform) override;
    void GetTransform(Matrix &_matWorld) override;

    void CreateFromDataSource(DataSource *pDataSource);

    ParticleManager *GetMaster() const;

    IEmitter *FindEmitter(const char *name);
    IEmitter *FindEmitterByData(FieldList *Data);

    void Teleport(const Matrix &transform) override;

    bool IsAlive() override;

    const char *GetName() override;
    void SetName(const char *Name);

  public:
    uint32_t GetEmittersCount() const;
    IEmitter *GetEmitterByIndex(uint32_t Index);
    EmitterType GetEmitterTypeByIndex(uint32_t Index);

    void Editor_UpdateCachedData();
};
