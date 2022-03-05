#include "location_script_lib.h"

#include <thread>

#include "entity.h"
#include "fader.h"
#include "steam_api_impl.hpp"
#include "core.h"
#include "s_import_func.h"
#include "v_s_stack.h"

//============================================================================================

struct LocationFindCacheElement
{
    LocationFindCacheElement()
    {
        name = nullptr;
        size = 0;
        max = 0;
        index = -1;
        use = 0;
    };

    ~LocationFindCacheElement()
    {
        delete name;
    };

    int32_t Cmp(const LocationFindCacheElement &v) const
    {
        if (v.size != size)
            return false;
        if (storm::iEquals(v.name, name))
            return true;
        return false;
    };

    void Set(const char *str)
    {
        size = strlen(str) + 1;
        if (size > max)
        {
            max = (size + 15) & ~15;
            delete name;
            name = new char[max];
        }
        memcpy(name, str, size);
    };

    char *name;
    int32_t size;
    int32_t max;
    int32_t index;
    int32_t use;
};

LocationFindCacheElement charactersFindCache[16];
LocationFindCacheElement locationsFindCache[8];
LocationFindCacheElement charactersFindBuf;

inline bool CheckID(VDATA *vd, const char *id, bool &res)
{
    res = false;
    if (!vd || !id || !id[0])
        return false;
    auto *a = vd->GetAClass();
    if (!a)
        return false;
    a = a->GetAttributeClass("id");
    if (!a)
        return true;
    auto *const attr = a->GetThisAttr();
    if (!attr)
        return true;
    res = storm::iEquals(attr, id);
    return true;
}

void slAddToCache(LocationFindCacheElement *element, int32_t size, const char *name, int32_t index)
{
    Assert(name);
    Assert(name[0]);
    // looking for a cell for recording
    int32_t j = 0;
    for (int32_t i = 0, min = element[i].use; i < size; i++)
    {
        if (element[i].index < 0)
        {
            j = i;
            break;
        }
        if (element[i].use < min)
        {
            j = i;
            min = element[i].use;
        }
    }
    element[j].index = index;
    element[j].use = 16;
    element[j].Set(name);
}

uint32_t slNativeFastFind(VS_STACK *pS, LocationFindCacheElement *cache, int32_t cacheSize)
{
    // Get strings
    auto *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm)
        charactersFindBuf.Set(nm);
    else
        charactersFindBuf.Set("");
    // Array of characters
    auto *pArray = (VDATA *)pS->Pop();
    if (!pArray)
        return IFUNCRESULT_FAILED;
    pArray = (VDATA *)pArray->GetReference();
    if (!pArray)
        return IFUNCRESULT_FAILED;
    // Return value
    auto *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    if (!charactersFindBuf.name[0])
    {
        pReturn->Set(-1);
        return IFUNCRESULT_OK;
    }
    // Lowering the cache usage values
    for (int32_t i = 0; i < cacheSize; i++)
    {
        cache[i].use--;
        if (cache[i].use < 0)
            cache[i].use = 0;
    }
    // look in the cache
    bool res;
    for (int32_t i = 0; i < cacheSize; i++)
    {
        if (cache[i].index < 0)
            continue;
        if (!cache[i].Cmp(charactersFindBuf))
            continue;
        // Checking for the correctness of the cache value
        if (static_cast<uint32_t>(cache[i].index) >= pArray->GetElementsNum())
        {
            cache[i].index = -1;
            continue;
        }
        auto *vd = (VDATA *)pArray->GetArrayElement(cache[i].index);
        if (!CheckID(vd, charactersFindBuf.name, res))
        {
            cache[i].index = -1;
            continue;
        }
        if (res)
        {
            // Found in cache, return
            cache[i].use++;
            pReturn->Set(cache[i].index);
            return IFUNCRESULT_OK;
        }
    }
    // Have to search through the array
    const int32_t num = pArray->GetElementsNum();
    for (int32_t i = 0; i < num; i++)
    {
        auto *const vd = (VDATA *)pArray->GetArrayElement(i);
        if (CheckID(vd, charactersFindBuf.name, res))
        {
            if (res)
            {
                slAddToCache(cache, cacheSize, charactersFindBuf.name, i);
                pReturn->Set(i);
                return IFUNCRESULT_OK;
            }
        }
    }
    //
    pReturn->Set(-1);
    return IFUNCRESULT_OK;
}

uint32_t slNativeFindCharacter(VS_STACK *pS)
{
    return slNativeFastFind(pS, charactersFindCache, sizeof(charactersFindCache) / sizeof(LocationFindCacheElement));
}

uint32_t slNativeFindLocation(VS_STACK *pS)
{
    return slNativeFastFind(pS, locationsFindCache, sizeof(locationsFindCache) / sizeof(LocationFindCacheElement));
}

uint32_t slNativeFindLaodLocation(VS_STACK *pS)
{
    // Return value
    auto pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    // Looking for a location
    const auto loc = EntityManager::GetEntityId("location");
    if (!loc)
    {
        pReturn->Set(-1);
        return IFUNCRESULT_OK;
    }
    Entity *l = EntityManager::GetEntityPointer(loc);
    if (!l || !l->AttributesPointer)
    {
        pReturn->Set(-1);
        return IFUNCRESULT_OK;
    }
    const int32_t index = l->AttributesPointer->GetAttributeAsDword("index", -1L);
    pReturn->Set(index);
    return IFUNCRESULT_OK;
}

uint32_t slNativeSetReloadBackImage(VS_STACK *pS)
{
    // Get strings
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    // Setting the picture
    auto rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (rs)
    {
        rs->SetProgressImage(nm);
    }
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressStart(VS_STACK *pS)
{
    auto rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (rs)
        rs->StartProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressUpdate(VS_STACK *pS)
{
    auto rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (rs)
        rs->ProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressEnd(VS_STACK *pS)
{
    auto *rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (rs)
        rs->EndProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeSleep(VS_STACK *pS)
{
    // Get strings
    auto *pInt = (VDATA *)pS->Pop();
    int32_t delay = 1;
    if (!pInt || !pInt->Get(delay))
        return IFUNCRESULT_FAILED;
    if (delay < 1)
        delay = 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    return IFUNCRESULT_OK;
}

uint32_t slNativeExecuteTechnique(VS_STACK *pS)
{
    // Get string
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    // Execute technique
    if (nm && nm[0])
    {
        auto *rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
        rs->TechniqueExecuteStart(nm);
        while (rs->TechniqueExecuteNext())
            ;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetNextLineString(VS_STACK *pS)
{
    // Return value
    auto *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    pReturn->Set("\r\n");
    return IFUNCRESULT_OK;
}

uint32_t slNativeSetReloadNextTipsImage(VS_STACK *pS)
{
    if (Fader::numberOfTips <= 0)
    {
        Fader::currentTips = -1;
    }
    else
    {
        Fader::currentTips = rand() % Fader::numberOfTips;
    }
    return IFUNCRESULT_OK;
}

//============================================================================================

bool ScriptLocationLibrary::Init()
{
    IFUNCINFO sIFuncInfo;
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "NativeFindCharacter";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slNativeFindCharacter;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "NativeFindLocation";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slNativeFindLocation;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "NativeFindLoadCharacter";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slNativeFindLaodLocation;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SetReloadProgressBackImage";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSetReloadBackImage;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "SetReloadNextTipsImage";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSetReloadNextTipsImage;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressStart";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressStart;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressUpdate";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressUpdate;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressEnd";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressEnd;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SystemDelay";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSleep;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ExecuteTechnique";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeExecuteTechnique;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "GetNextLineString";
    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncAddress = slGetNextLineString;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}
