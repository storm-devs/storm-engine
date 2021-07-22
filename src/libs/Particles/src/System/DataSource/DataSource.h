#pragma once

#include "../../icommon/types.h"
#include "fieldlist.h"
#include <vector>

class DataObject;
class DataColor;
class DataBool;
class DataFloat;
class DataGraph;
class DataString;
class DataPosition;
class DataUV;

class MemFile;

class IParticleManager;

// Data source for particles ...
class DataSource
{
  public:
    // Particle description (used when creating a system)
    struct ParticleDesc
    {
        ParticleType Type;
        FieldList Fields;

        ParticleDesc()
        {
            Type = UNKNOWN_PARTICLE;
        }
    };

    // Description of the emitter (used when creating the system)
    struct EmitterDesc
    {
        EmitterType Type;
        FieldList Fields;
        std::vector<ParticleDesc> Particles;

        EmitterDesc()
        {
            Type = UNKNOWN_EMITTER;
        }
    };

  private:
    std::vector<EmitterDesc> Emitters;

    // Load point emitter
    void CreatePointEmitter(MemFile *pMemFile);

    // Download BillBoard Particle
    void CreateBillBoardParticle(std::vector<ParticleDesc> &Particles, MemFile *pMemFile);

    // Download Model Particle
    void CreateModelParticle(std::vector<ParticleDesc> &Particles, MemFile *pMemFile);

    int FindEmitter(const char *Name);

  protected:
    virtual ~DataSource();

  public:
    FieldList *CreateEmptyPointEmitter(const char *EmitterName);
    FieldList *CreateBillBoardParticle(const char *ParticleName, const char *EmitterName);
    FieldList *CreateModelParticle(const char *ParticleName, const char *EmitterName);

    void DeletePointEmitter(FieldList *pEmitter);
    void DeleteBillboard(FieldList *pEmitter, FieldList *pParticles);
    void DeleteModel(FieldList *pEmitter, FieldList *pParticles);

    void Destroy();

    // ---------- Create / Delete --------------------
    DataSource(IParticleManager *Master);
    bool Release() const;

    // ========================= Load & Save =======================================
    // Save / restore from file
    virtual void Write(MemFile *pMemFile);
    virtual void Load(MemFile *pMemFile);

    virtual int GetEmitterCount();
    EmitterDesc *GetEmitterDesc(int Index);
};
