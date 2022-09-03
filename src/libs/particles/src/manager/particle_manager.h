//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#pragma once

#include "../i_particle_manager.h"

#include "../system/particle_processor/bb_processor.h"
#include "../system/particle_processor/mdl_processor.h"
#include <string>
#include <vector>

class ParticleService;
class DataCache;
class GeomCache;
class ParticleSystem;
class DataSource;

// Particle Systems Manager
class ParticleManager : public IParticleManager
{
    struct CacheReloadedInfo
    {
        std::string Name;
        bool AutoDeleted;
        Matrix matWorld;
    };

    bool ShowStat;
    bool GlobalDelete;

    struct SystemDesc
    {
        ParticleSystem *pSystem;
    };

    std::vector<SystemDesc> Systems;

    // Particle Model Processor
    ModelProcessor *MDL_Processor;

    // Billboard Particle Processor
    BillBoardProcessor *BB_Processor;

    int32_t pProjectTexture;
    int32_t pProjectTextureNormalMap;
    DataCache *pDataCache;
    GeomCache *pGeomCache;

    std::string ShortProjectName;
    std::string TextureName;

    // The service that created the system
    ParticleService *pService;

    VDX9RENDER *pRS;

    void DeleteAllSystems();

    float TimeFromLastStatUpdate;
    uint64_t nowTickTime;
    uint64_t nowUpdateTime;
    uint32_t ActiveSystems;
    uint32_t ActiveEmitters;
    uint32_t ActiveBillboardParticles;
    uint32_t ActiveModelParticles;

    std::vector<ParticleSystem *> DeleteQuery;

  protected:
    ~ParticleManager() override;

    ParticleSystem *CreateParticleSystemFromDataSource(DataSource *pDataSource);

    uint32_t IteratorIndex;
    std::vector<std::string> EnumUsedGeom;

    bool FindInEnumUsedGeom(const char *GeomName);

  public:
    // Create / delete
    ParticleManager(ParticleService *service);
    bool Release() override;

    // Get a pointer to Render / FileService
    VDX9RENDER *Render() override;

    // Open project
    bool OpenProject(const char *FileName) override;
    // Close project
    void CloseProject() override;

    // Remove from resource list (system)
    void RemoveResource(IParticleSystem *pResource) override;

    // Execute Particles
    void Execute(float DeltaTime) override;

    // Find out whether the system is available or not
    bool IsSystemAvailable(const char *FileName) override;

    // Get the global texture of the project
    int32_t GetProjectTexture() override;
    // Set project texture
    void SetProjectTexture(const char *FileName = nullptr) override;

    // Get project name
    const char *GetProjectFileName() override;

    // Create a particle system from a file (the file must be in the project)
    IParticleSystem *CreateParticleSystemEx(const char *FileName, const char *File, int Line) override;

    // Create an empty particle system, for the editor
    IParticleSystem *CreateEmptyParticleSystemEx(const char *FileName, int Line) override;

    BillBoardProcessor *GetBBProcessor() const;
    ModelProcessor *GetMDLProcessor() const;

    bool ValidateSystem(IParticleSystem *pSystem) override;

    GEOS *GetModel(const char *FileName) const;

    uint32_t GetCreatedSystemCount() const;
    ParticleSystem *GetCreatedSystemByIndex(uint32_t Index);

    bool ReadyForUse() override;

    void DefferedDelete(ParticleSystem *pSys);

    const char *GetProjectTextureName() override;

    void Editor_UpdateCachedData() override;

    virtual uint32_t GetProjectSystemCount();
    virtual const char *GetProjectSystemName(uint32_t Index);

    const char *GetFirstGeomName(const char *FileName) override;
    const char *GetNextGeomName() override;

    virtual void CreateGeomCache();

    void WriteSystemCache(const char *FileName) override;
    virtual void WriteSystemCache(const char *FileName, MemFile *pMemFile);
    virtual void WriteSystemCacheAs(const char *FileName, const char *NewName);

    virtual void LoadSystemCache(const char *FileName, MemFile *pMemFile);
    virtual void CacheReloaded();

    virtual FieldList *Editor_CreatePointEmitter(const char *SystemName, const char *EmitterName);
    virtual FieldList *Editor_CreateBillBoardParticle(const char *SystemName, const char *EmitterName,
                                                      const char *ParticleName);
    virtual FieldList *Editor_CreateModelParticle(const char *SystemName, const char *EmitterName,
                                                  const char *ParticleName);

    virtual void DeletePointEmitter(const char *SystemName, IEmitter *pEmitter);
    virtual void DeleteBillboard(const char *SystemName, IEmitter *pEmitter, FieldList *pParticles);
    virtual void DeleteModel(const char *SystemName, IEmitter *pEmitter, FieldList *pParticles);

    void OpenDefaultProject() override;
};
