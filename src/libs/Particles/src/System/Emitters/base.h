#pragma once

#include "../../ICommon/IEmitter.h"
#include "../../ICommon/Types.h"
#include "../DataSource/DataSource.h"
#include "../ParticleSystem/particlesystem.h"
#include "math3D.h"

class DataGraph;

class BaseEmitter : public IEmitter
{
    struct structParticleType
    {
        bool Visible;               // visible or not
        ParticleType Type;          // Particle type
        float Remain;               // How many not emitted from the last frame are left
        uint32_t ActiveCount;       // The number of active particles of this type
        uint32_t MaxParticlesCount; // Maximum number of particles of this type
        DataGraph *EmissionRate;    // Graph defining the rate of emission of particles

        FieldList *pFields;

        structParticleType()
        {
            ActiveCount = 0;
            Remain = 0.0f;
            EmissionRate = nullptr;
            pFields = nullptr;
            Type = UNKNOWN_PARTICLE;
        }
    };

    std::vector<structParticleType> ParticleTypes;

    DataSource::EmitterDesc *pEmitter;
    FieldList *pFields;
    bool IsAttachedFlag;

    Vector Position;
    EmitterType Type;
    float LifeTime;
    float ElapsedTime;
    DataGraph *EmissionDirX;
    DataGraph *EmissionDirY;
    DataGraph *EmissionDirZ;

    bool Looped;
    bool Stoped;
    bool Visible;

    uint32_t Unique_GUID;

    Matrix matWorldTransform;
    bool OldMatrixNotInitialized;
    Matrix matWorldTransformOld;
    Matrix matWorldTransformNew;

    void BlendMatrix(Matrix &result, const Matrix &mat1, const Matrix &mat2, float BlendK);

    void IncreaseTime(float DeltaTime);

  protected:
    std::string Name;
    ParticleSystem *pMaster;

  public:
    // Constructor / destructor
    BaseEmitter(ParticleSystem *pSystem);
    virtual ~BaseEmitter();

    // Get a position for the emission of new particles
    virtual Vector GetNewParticlePosition(float DeltaTime) = 0;

    // Emit new particles
    void BornParticles(float DeltaTime) override;
    // Execute
    void Execute(float DeltaTime) override;

    // Join data source
    virtual void AttachToDataSource(DataSource::EmitterDesc *pEmitter);

    virtual void CreateBillBoardParticle(FieldList &Fields);
    virtual void CreateModelParticle(FieldList &Fields);

    ParticleSystem *GetMaster() const;
    ParticleManager *GetManager() const;
    void GetEmissionDirection(Matrix &matWorld) const;

    virtual void SetGUID(uint32_t GUID)
    {
        Unique_GUID = GUID;
    }

    virtual uint32_t GetGUID()
    {
        return Unique_GUID;
    }

    void Restart() override;

    uint32_t GetParticleCount() override;
    bool IsStoped() override;

    void SetTransform(const Matrix &matWorld) override;
    void Teleport(const Matrix &matWorld) override;

    const char *GetName() override;

    // If the flag is set to true, the emitter will not emit particles by itself
    // because it is attached
    void SetAttachedFlag(bool Flag) override;
    bool IsAttached() override;

    float GetTime() override;
    void SetTime(float Time) override;

    uint32_t GetParticleTypesCount() override;
    FieldList *GetParticleTypeDataByIndex(uint32_t Index) override;
    ParticleType GetParticleTypeByIndex(uint32_t Index) override;

    FieldList *GetData() override;

    bool SetEnable(bool bVisible) override;
    bool GetEnable() override;

    // -1 if not found, otherwise index
    int GetParticleTypeIndex(FieldList *pFields) override;
    bool SetParticleTypeEnable(bool bVisible, uint32_t Index) override;
    bool GetParticleTypeEnable(uint32_t Index) override;

    void Editor_UpdateCachedData() override;

    void SetName(const char *Name) override;

    void Stop() override;
};
