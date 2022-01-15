#include "particle_manager.h"

#include "core.h"

#include "../data_cache/data_cache.h"
#include "../geom_cache/geom_cache.h"
#include "../service/particle_service.h"
#include "../system/particle_system/particle_system.h"

#include "../i_common/i_emitter.h"
#include "../i_common/names.h"
#include "../system/data_source/data_string.h"
#include "defines.h"

#include <filesystem>

uint32_t GraphRead = 0;

ParticleManager::ParticleManager(ParticleService *service) : IParticleManager(service)
{
    pService = service;
    ShowStat = false;
    MDL_Processor = new ModelProcessor(this);
    BB_Processor = new BillBoardProcessor;
    GlobalDelete = false;
    TimeFromLastStatUpdate = 100.0f;
    pRS = static_cast<VDX9RENDER *>(core.GetService("DX9Render"));
    Assert(pRS != NULL);

    pDataCache = new DataCache(this);
    pGeomCache = new GeomCache;

    pProjectTexture = -1;
    pProjectTextureNormalMap = -1;
}

ParticleManager::~ParticleManager()
{
    DeleteAllSystems();
    if (pProjectTexture >= 0)
    {
        pRS->TextureRelease(pProjectTexture);
    }
    pProjectTexture = -1;

    if (pProjectTextureNormalMap)
    {
        pRS->TextureRelease(pProjectTextureNormalMap);
    }
    pProjectTextureNormalMap = -1;

    delete pDataCache;
    delete pGeomCache;
    delete BB_Processor;
    delete MDL_Processor;
    pService->RemoveManagerFromList(this);
}

bool ParticleManager::Release()
{
    delete this;
    return true;
}

VDX9RENDER *ParticleManager::Render()
{
    return pRS;
}

// Set project texture
void ParticleManager::SetProjectTexture(const char *FileName)
{
    if (pProjectTexture)
    {
        pRS->TextureRelease(pProjectTexture);
        pProjectTexture = -1;
    }

    if (pProjectTextureNormalMap)
    {
        pRS->TextureRelease(pProjectTextureNormalMap);
        pProjectTextureNormalMap = -1;
    }

    pProjectTexture = pRS->TextureCreate(FileName);

    const std::filesystem::path path = FileName;
    pProjectTextureNormalMap = pRS->TextureCreate((path.stem().string() + "nm").c_str());

    TextureName = FileName;
}

const char *ParticleManager::GetProjectTextureName()
{
    return TextureName.c_str();
}

// Open project
bool ParticleManager::OpenProject(const char *FileName)
{
    CloseProject();
    ShortProjectName = FileName;

    // std::string LongFileName = "resource\\particles\\";
    auto path = std::filesystem::path() / "resource" / "particles" / FileName;
    auto pathStr = path.extension().string();
    if (!storm::iEquals(pathStr, ".prj"))
        path += ".prj";
    pathStr = path.string();
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~
    // LongFileName += FileName;
    // LongFileName.AddExtention(".prj");

    auto IniFile = fio->OpenIniFile(pathStr.c_str());
    if (!IniFile)
    {
        core.Trace("Can't find project '%s'", pathStr.c_str());
        return false;
    }

    static char IniStringBuffer[8192];

    // Setting the texture of the project
    IniFile->ReadString("Textures", "MainTexture", IniStringBuffer, 8192, "none");
    // core.Trace("Manager use texture: %s", IniStringBuffer);
    SetProjectTexture(IniStringBuffer);

    /*
      // Load the models into the cache
      for (int n = 0; n < 9999; n++)
      {
        string Section;
        Section.Format("Model_%04d", n);
        bool ReadSuccess = IniFile->ReadString("ModelsCache", (char*)Section.c_str(), IniStringBuffer, 8192, "none");
        if (!ReadSuccess) break;

        //core.Trace("Cache geom: %s", IniStringBuffer);
        pGeomCache->CacheModel(IniStringBuffer);

      }
    */

    // Loading data
    for (auto n = 0; n < 9999; n++)
    {
        char buf[64];
        snprintf(buf, std::size(buf), "System_%04d", n);
        // Section.Format("System_%04d", n);
        const auto ReadSuccess = IniFile->ReadString("Manager", buf, IniStringBuffer, 8192, "none");
        if (!ReadSuccess)
            break;
        pDataCache->CacheSystem(IniStringBuffer);
    }

    // FIX ME !
    // If there will be asynchronous loading it is wrong
    CreateGeomCache();

    return true;
}

// Get project name
const char *ParticleManager::GetProjectFileName()
{
    return ShortProjectName.c_str();
}

// Close project
void ParticleManager::CloseProject()
{
    BB_Processor->Clear();
    MDL_Processor->Clear();
    DeleteAllSystems();
    if (pProjectTexture)
    {
        pRS->TextureRelease(pProjectTexture);
    }

    if (pProjectTextureNormalMap)
    {
        pRS->TextureRelease(pProjectTextureNormalMap);
    }
    pProjectTextureNormalMap = -1;

    pProjectTexture = -1;
    pDataCache->ResetCache();
    pGeomCache->ResetCache();
}

// Remove from resource list (system)
void ParticleManager::RemoveResource(IParticleSystem *pResource)
{
    if (GlobalDelete)
        return;
    for (uint32_t n = 0; n < Systems.size(); n++)
    {
        if (Systems[n].pSystem == pResource)
        {
            // Systems.ExtractNoShift(n);
            Systems[n] = Systems.back();
            Systems.pop_back();
            return;
        }
    }
}

// Execute Particles
void ParticleManager::Execute(float DeltaTime)
{
    GraphRead = 0;
    ActiveSystems = 0;
    ActiveEmitters = 0;
    ActiveBillboardParticles = 0;
    ActiveModelParticles = 0;

    // pRS->Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET |D3DCLEAR_ZBUFFER, 0xFF404080, 1.0f, 0);

    uint64_t TicksTime, ProcessTime;
    RDTSC_B(TicksTime);
    ProcessTime = TicksTime;

    ActiveSystems = Systems.size();
    for (uint32_t n = 0; n < Systems.size(); n++)
    {
        ActiveEmitters += Systems[n].pSystem->Execute(DeltaTime);
    }
    BB_Processor->Process(DeltaTime);
    MDL_Processor->Process(DeltaTime);
    RDTSC_E(ProcessTime);

    ActiveBillboardParticles = BB_Processor->GetCount();
    ActiveModelParticles = MDL_Processor->GetCount();
    pRS->TextureSet(0, pProjectTexture);
    pRS->TextureSet(1, pProjectTexture);
    pRS->TextureSet(3, pProjectTextureNormalMap);
    //    pRS->SetTexture(0, pProjectTexture);
    //    pRS->SetTexture(1, pProjectTexture);
    BB_Processor->Draw();
    MDL_Processor->Draw();

    RDTSC_E(TicksTime);

    TimeFromLastStatUpdate += DeltaTime;
    if (TimeFromLastStatUpdate > 1.0f)
    {
        TimeFromLastStatUpdate = 0.0f;
        nowTickTime = TicksTime;
        nowUpdateTime = ProcessTime;
    }

    if (ShowStat)
    {
        /*
        IFont* pSysFont = pRS->GetSystemFont ();
        float Width = pSysFont->GetLength("Graph read count - %d", GraphRead);
        pSysFont->Release();
        */

        D3DVIEWPORT9 ViewPort;
        pRS->GetViewport(&ViewPort);
        RS_SPRITE spr[4];
        const auto x1 = -1.0f;
        const float x2 = ((220.0f / static_cast<float>(ViewPort.Width)) * 2) - 1.0f;
        const float y1 = 1.0f - ((16.0f / static_cast<float>(ViewPort.Height)) * 2.0f);
        const float y2 = 1.0f - ((150.0f / static_cast<float>(ViewPort.Height)) * 2.0f);
        spr[0].vPos = CVECTOR(x1, y1, 0.2f);
        spr[1].vPos = CVECTOR(x2, y1, 0.2f);
        spr[2].vPos = CVECTOR(x2, y2, 0.2f);
        spr[3].vPos = CVECTOR(x1, y2, 0.2f);
        pRS->DrawSprites(spr, 1, "dbgInfoSprite");

        /*
            pRS->Print(0, 16, 0xFFFFFFFF, "Systems - %d", ActiveSystems);
            pRS->Print(0, 32, 0xFFFFFFFF, "Emitters - %d", ActiveEmitters);
            pRS->Print(0, 48, 0xFFFFFFFF, "Billboards - %d", ActiveBillboardParticles);
            pRS->Print(0, 64, 0xFFFFFFFF, "Models - %d", ActiveModelParticles);
            pRS->Print(0, 80, 0xFFFFFFFF, "Total time - %d", nowTickTime);
            pRS->Print(0, 96, 0xFFFFFFFF, "Update time - %d", nowUpdateTime);
            pRS->Print(0, 112, 0xFFFFFFFF, "Graph read count - %d", GraphRead);
        */

        if (GraphRead != 0)
        {
            float AverageReadTime = static_cast<float>(nowUpdateTime) / static_cast<float>(GraphRead);
            //            pRS->Print(0, 128, 0xFFFFFFFF, "Average read time - %3.2f", AverageReadTime);
        }
    }

    if (core.Controls->GetDebugAsyncKeyState(VK_F3) < 0 && core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0)
    {
        ShowStat = !ShowStat;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (uint32_t n = 0; n < DeleteQuery.size(); n++)
    {
        DeleteQuery[n]->Release();
    }

    DeleteQuery.clear();
}

// Find out whether the system is available or not
bool ParticleManager::IsSystemAvailable(const char *FileName)
{
    if (pDataCache->GetParticleSystemDataSource(FileName))
        return true;
    return false;
}

// Get the global texture of the project
int32_t ParticleManager::GetProjectTexture()
{
    return pProjectTexture;
}

// Create a particle system from a file (the file must be in the project)
IParticleSystem *ParticleManager::CreateParticleSystemEx(const char *FileName, const char *File, int Line)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
    {
        core.Trace("Particle system '%s' can't loading. Reason: Not found in cache", FileName);
        return nullptr;
    }

    //    core.Trace("Create system '%s'", FileName);
    ParticleSystem *pSys = CreateParticleSystemFromDataSource(pDataSource);
    pSys->SetName(FileName);
    return pSys;
}

void ParticleManager::DeleteAllSystems()
{
    GlobalDelete = true;
    for (uint32_t n = 0; n < Systems.size(); n++)
    {
        Systems[n].pSystem->Release();
    }

    Systems.clear();
    GlobalDelete = false;
}

ParticleSystem *ParticleManager::CreateParticleSystemFromDataSource(DataSource *pDataSource)
{
    auto *pSys = new ParticleSystem(this);
    pSys->CreateFromDataSource(pDataSource);

    SystemDesc SysDesc;
    SysDesc.pSystem = pSys;
    Systems.push_back(SysDesc);

    return pSys;
}

BillBoardProcessor *ParticleManager::GetBBProcessor() const
{
    return BB_Processor;
}

ModelProcessor *ParticleManager::GetMDLProcessor() const
{
    return MDL_Processor;
}

// Create an empty particle system, for the editor
IParticleSystem *ParticleManager::CreateEmptyParticleSystemEx(const char *FileName, int Line)
{
    // NEED WRITE !!!
    return nullptr;
}

bool ParticleManager::ValidateSystem(IParticleSystem *pSystem)
{
    for (uint32_t n = 0; n < Systems.size(); n++)
    {
        if (Systems[n].pSystem == pSystem)
            return true;
    }
    return false;
}

GEOS *ParticleManager::GetModel(const char *FileName) const
{
    return pGeomCache->GetModel(FileName);
}

uint32_t ParticleManager::GetCreatedSystemCount() const
{
    return Systems.size();
}

ParticleSystem *ParticleManager::GetCreatedSystemByIndex(uint32_t Index)
{
    return Systems[Index].pSystem;
}

bool ParticleManager::ReadyForUse()
{
    return true;
}

void ParticleManager::DefferedDelete(ParticleSystem *pSys)
{
    DeleteQuery.push_back(pSys);
}

void ParticleManager::Editor_UpdateCachedData()
{
    for (uint32_t n = 0; n < Systems.size(); n++)
    {
        Systems[n].pSystem->Editor_UpdateCachedData();
    }
}

uint32_t ParticleManager::GetProjectSystemCount()
{
    return pDataCache->GetCachedCount();
}

const char *ParticleManager::GetProjectSystemName(uint32_t Index)
{
    return pDataCache->GetCachedNameByIndex(Index);
}

const char *ParticleManager::GetFirstGeomName(const char *FileName)
{
    IteratorIndex = 0;
    GeomNameParser Parser;
    EnumUsedGeom.clear();
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
        return nullptr;

    const uint32_t count = pDataSource->GetEmitterCount();

    for (uint32_t n = 0; n < count; n++)
    {
        DataSource::EmitterDesc *pDesc = pDataSource->GetEmitterDesc(n);
        for (uint32_t i = 0; i < pDesc->Particles.size(); i++)
        {
            DataString *pModelNames = pDesc->Particles[i].Fields.FindString(PARTICLE_GEOM_NAMES);
            if (!pModelNames)
                continue;

            Parser.Tokenize(pModelNames->GetValue());

            const uint32_t TCount = Parser.GetTokensCount();
            if (TCount > 0)
            {
                for (uint32_t j = 0; j < TCount; j++)
                {
                    const char *GeomName = Parser.GetTokenByIndex(j);
                    if (!FindInEnumUsedGeom(GeomName))
                        EnumUsedGeom.push_back(GeomName);
                }
            }
        }
    }

    if (EnumUsedGeom.size() == 0)
        return nullptr;
    const char *FirstName = EnumUsedGeom[IteratorIndex].c_str();
    IteratorIndex++;
    return FirstName;
}

const char *ParticleManager::GetNextGeomName()
{
    if (IteratorIndex >= EnumUsedGeom.size())
        return nullptr;
    const char *FirstName = EnumUsedGeom[IteratorIndex].c_str();
    IteratorIndex++;
    return FirstName;
}

bool ParticleManager::FindInEnumUsedGeom(const char *GeomName)
{
    for (uint32_t n = 0; n < EnumUsedGeom.size(); n++)
    {
        const char *StoredGeomName = EnumUsedGeom[n].c_str();
        if (storm::iEquals(StoredGeomName, GeomName))
            return true;
    }
    return false;
}

void ParticleManager::CreateGeomCache()
{
    pGeomCache->ResetCache();
    const uint32_t SystemCount = pDataCache->GetCachedCount();
    for (uint32_t n = 0; n < SystemCount; n++)
    {
        const char *pSystemName = pDataCache->GetCachedNameByIndex(n);
        const char *GeomName = nullptr;
        GeomName = GetFirstGeomName(pSystemName);

        while (GeomName)
        {
            // core.Trace("Cache geom %s", GeomName);
            pGeomCache->CacheModel(GeomName);
            GeomName = GetNextGeomName();
        }
    }
}

void ParticleManager::WriteSystemCache(const char *FileName)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
    {
        core.Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
        return;
    }

    MemFile pMemSave;
    pMemSave.OpenWrite(1048576);
    pDataSource->Write(&pMemSave);

    // std::string LongFileName = "resource\\particles\\";
    // LongFileName+=FileName;
    // LongFileName.AddExtention(".xps");
    /*
      IWrite* pFile = pFS->Write(LongFileName.c_str(), iw_create_always, _FL_);
      pFile->Write(pMemSave.c_str(), pMemSave.GetLength());
      pFile->Release();
    */

    pMemSave.Close();
    core.Trace("Particle system '%s' saved.", FileName);
}

void ParticleManager::WriteSystemCacheAs(const char *FileName, const char *NewName)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
    {
        core.Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
        return;
    }

    MemFile pMemSave;
    pMemSave.OpenWrite(1048576);
    pDataSource->Write(&pMemSave);
    /*
      IWrite* pFile = pFS->Write(NewName, iw_create_always, _FL_);
      pFile->Write(pMemSave.c_str(), pMemSave.GetLength());
      pFile->Release();
    */

    pMemSave.Close();
    core.Trace("Particle system '%s' saved.", NewName);
}

void ParticleManager::WriteSystemCache(const char *FileName, MemFile *pMemFile)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
    {
        core.Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
        return;
    }

    pDataSource->Write(pMemFile);
}

void ParticleManager::LoadSystemCache(const char *FileName, MemFile *pMemFile)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(FileName);
    if (!pDataSource)
    {
        core.Trace("Particle system '%s' can't load. Reason: Not found in cache", FileName);
        return;
    }

    pDataSource->Destroy();
    pDataSource->Load(pMemFile);

    CacheReloaded();
}

void ParticleManager::CacheReloaded()
{
    uint32_t n = 0;
    std::vector<CacheReloadedInfo> UsedSystems;
    for (n = 0; n < Systems.size(); n++)
    {
        CacheReloadedInfo Info;
        Info.Name = Systems[n].pSystem->GetName();
        Info.AutoDeleted = Systems[n].pSystem->IsAutoDeleted();
        Systems[n].pSystem->GetTransform(Info.matWorld);

        UsedSystems.push_back(Info);
    }

    DeleteAllSystems();

    BB_Processor->Clear();
    MDL_Processor->Clear();

    for (n = 0; n < UsedSystems.size(); n++)
    {
        IParticleSystem *pSystem = CreateParticleSystemEx(UsedSystems[n].Name.c_str(), __FILE__, __LINE__);
        pSystem->AutoDelete(UsedSystems[n].AutoDeleted);
        pSystem->SetTransform(UsedSystems[n].matWorld);
    }
}

FieldList *ParticleManager::Editor_CreatePointEmitter(const char *SystemName, const char *EmitterName)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return nullptr;

    FieldList *pData = pDataSource->CreateEmptyPointEmitter(EmitterName);

    CacheReloaded();

    return pData;
}

FieldList *ParticleManager::Editor_CreateBillBoardParticle(const char *SystemName, const char *EmitterName,
                                                           const char *ParticleName)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return nullptr;

    FieldList *pData = pDataSource->CreateBillBoardParticle(ParticleName, EmitterName);

    CacheReloaded();

    return pData;
}

FieldList *ParticleManager::Editor_CreateModelParticle(const char *SystemName, const char *EmitterName,
                                                       const char *ParticleName)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return nullptr;

    FieldList *pData = pDataSource->CreateModelParticle(ParticleName, EmitterName);

    CacheReloaded();

    return pData;
}

void ParticleManager::DeletePointEmitter(const char *SystemName, IEmitter *pEmitter)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return;

    FieldList *pEmitterData = pEmitter->GetData();

    pDataSource->DeletePointEmitter(pEmitterData);

    CacheReloaded();
}

void ParticleManager::DeleteBillboard(const char *SystemName, IEmitter *pEmitter, FieldList *pParticles)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return;

    FieldList *pEmitterData = pEmitter->GetData();

    pDataSource->DeleteBillboard(pEmitterData, pParticles);

    CacheReloaded();
}

void ParticleManager::DeleteModel(const char *SystemName, IEmitter *pEmitter, FieldList *pParticles)
{
    DataSource *pDataSource = pDataCache->GetParticleSystemDataSource(SystemName);
    if (!pDataSource)
        return;

    FieldList *pEmitterData = pEmitter->GetData();

    pDataSource->DeleteModel(pEmitterData, pParticles);

    CacheReloaded();
}

void ParticleManager::OpenDefaultProject()
{
    CloseProject();
    ShortProjectName = "default";

    SetProjectTexture("particles_list.tga");

    const auto vFilenames = fio->_GetPathsOrFilenamesByMask("resource\\particles", "*.xps", false);
    for (std::string curName : vFilenames)
    {
        pDataCache->CacheSystem(curName.c_str());
    }

    CreateGeomCache();
}
