//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef PARTICLES_MANAGER_IMPLEMENTATION
#define PARTICLES_MANAGER_IMPLEMENTATION

#include "particles/iparticlemanager.h"

#include "../System/ParticleProcessor/bb_processor.h"
#include "../System/ParticleProcessor/mdl_processor.h"
#include <string>
#include <vector>

class ParticleService;
class DataCache;
class GeomCache;
class ParticleSystem;
class DataSource;

//Менеджер партикловых систем
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

    //Процессор для партиклов моделей
    ModelProcessor *MDL_Processor;

    //Процессор для партиклов плашек
    BillBoardProcessor *BB_Processor;

    long pProjectTexture;
    long pProjectTextureNormalMap;
    DataCache *pDataCache;
    GeomCache *pGeomCache;

    std::string ShortProjectName;
    std::string TextureName;

    //Сервис который родил систему
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
    virtual ~ParticleManager();

    ParticleSystem *CreateParticleSystemFromDataSource(DataSource *pDataSource);

    uint32_t IteratorIndex;
    std::vector<std::string> EnumUsedGeom;

    bool FindInEnumUsedGeom(const char *GeomName);

  public:
    //Создание/удаление
    ParticleManager(ParticleService *service);
    bool Release() override;

    //Получить указатель на Render/FileService
    VDX9RENDER *Render() override;

    //Открыть проект
    bool OpenProject(const char *FileName) override;
    //Закрыть проект
    void CloseProject() override;

    //Удалить из списка ресурсов (системная)
    void RemoveResource(IParticleSystem *pResource) override;

    //Исполнить партиклы
    void Execute(float DeltaTime) override;

    //Узнать доступна система или нет
    bool IsSystemAvailable(const char *FileName) override;

    //Получить глобальную текстуру проекта
    long GetProjectTexture() override;
    //Установить текстуру проекта
    void SetProjectTexture(const char *FileName = nullptr) override;

    //Получить имя проекта
    const char *GetProjectFileName() override;

    //Создать партикловую систему из файла (файл должен быть в проекте!!!!!)
    IParticleSystem *CreateParticleSystemEx(const char *FileName, const char *File, int Line) override;

    //Создать пустую партикловую систему, для редактора...
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

#endif
