#include "DataCache.h"

#include "core.h"

#include "filesystem.h"
#include "vfile_service.h"
#include "vmodule_api.h"

bool ReadingAlreadyComplete;

// Constructor / destructor
DataCache::DataCache(IParticleManager *pManager)
{
    Master = pManager;
}

DataCache::~DataCache()
{
    ResetCache();
}

// Put data for the system in the cache
void DataCache::CacheSystem(const char *FileName)
{
    // NameWithExt.AddExtention(".xps");
    // NameWithExt.Lower();

    // std::string LongFileName = "resource\\particles\\";
    // LongFileName+=FileName;
    // LongFileName.AddExtention(".xps");
    auto path = fs::path() / "resource" / "particles" / FileName;
    auto pathStr = path.extension().string();
    if (_stricmp(pathStr.c_str(), ".xps") != 0)
        path += ".xps";
    pathStr = path.string();
    std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), tolower);
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~

    auto *const pSysFile = fio->_CreateFile(pathStr.c_str());

    if (pSysFile == INVALID_HANDLE_VALUE)
    {
        core.Trace("Particles: '%s' File not found !!!", pathStr.c_str());
        return;
    }

    const auto FileSize = fio->_GetFileSize(pSysFile, nullptr);

    auto *pMemBuffer = new uint8_t[FileSize];
    fio->_ReadFile(pSysFile, pMemBuffer, FileSize, nullptr);

    // Create data from file ...
    CreateDataSource(pMemBuffer, FileSize, pathStr.c_str());

    delete[] pMemBuffer;

    fio->_CloseHandle(pSysFile);
}

// Reset cache
void DataCache::ResetCache()
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].pData)
            Cache[n].pData->Release();
    }

    Cache.clear();
}

// Get a pointer to data for a particle system
DataSource *DataCache::GetParticleSystemDataSource(const char *FileName)
{
    // std::string NameWithExt = FileName;
    // NameWithExt.AddExtention(".xps");
    // NameWithExt.Lower();
    fs::path path = FileName;
    auto pathStr = path.extension().string();
    if (_stricmp(pathStr.c_str(), ".xps") != 0)
        path += ".xps";
    pathStr = path.string();
    std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), tolower);

    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].FileName == pathStr)
            return Cache[n].pData;
    }

    return nullptr;
}

// Check pointer for validity
bool DataCache::ValidatePointer(DataSource *pData)
{
    for (auto n = 0; n < Cache.size(); n++)
        if (Cache[n].pData == pData) // fix
            return true;

    return false;
}

void DataCache::CreateDataSource(void *pBuffer, uint32_t BufferSize, const char *SourceFileName)
{
    LoadedDataSource NewDataSource;
    NewDataSource.FileName = SourceFileName;
    NewDataSource.pData = new DataSource(Master);
    Cache.push_back(NewDataSource);

    // core.Trace("\nCreate data source for file %s", SourceFileName);

    auto *ReadFile = new MemFile;
    ReadFile->OpenRead(pBuffer, BufferSize);
    NewDataSource.pData->Load(ReadFile);
    ReadFile->Close();
    delete ReadFile;
}

uint32_t DataCache::GetCachedCount() const
{
    return Cache.size();
}

const char *DataCache::GetCachedNameByIndex(uint32_t Index)
{
    return Cache[Index].FileName.c_str();
}
