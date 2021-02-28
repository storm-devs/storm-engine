#include "LocationScriptLib.h"
#include "Entity.h"
#include "Fader.h"
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

    long Cmp(const LocationFindCacheElement &v) const
    {
        if (v.size != size)
            return false;
        if (_stricmp(v.name, name) == 0)
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
    long size;
    long max;
    long index;
    long use;
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
    res = _stricmp(attr, id) == 0;
    return true;
}

void slAddToCache(LocationFindCacheElement *element, long size, const char *name, long index)
{
    Assert(name);
    Assert(name[0]);
    // looking for a cell for recording
    long j = 0;
    for (long i = 0, min = element[i].use; i < size; i++)
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

uint32_t slNativeFastFind(VS_STACK *pS, LocationFindCacheElement *cache, long cacheSize)
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
        pReturn->Set(-1L);
        return IFUNCRESULT_OK;
    }
    // Lowering the cache usage values
    for (long i = 0; i < cacheSize; i++)
    {
        cache[i].use--;
        if (cache[i].use < 0)
            cache[i].use = 0;
    }
    // look in the cache
    bool res;
    for (long i = 0; i < cacheSize; i++)
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
    const long num = pArray->GetElementsNum();
    for (long i = 0; i < num; i++)
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
    pReturn->Set(-1L);
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
    VDATA *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    // Looking for a location
    const auto loc = EntityManager::GetEntityId("location");
    if (!loc)
    {
        pReturn->Set(-1L);
        return IFUNCRESULT_OK;
    }
    Entity *l = EntityManager::GetEntityPointer(loc);
    if (!l || !l->AttributesPointer)
    {
        pReturn->Set(-1L);
        return IFUNCRESULT_OK;
    }
    const long index = l->AttributesPointer->GetAttributeAsDword("index", -1L);
    pReturn->Set(index);
    return IFUNCRESULT_OK;
}

uint32_t slNativeSetReloadBackImage(VS_STACK *pS)
{
    // Get strings
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    // Setting the picture
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (rs)
    {
        rs->SetProgressImage(nm);
    }
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressStart(VS_STACK *pS)
{
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (rs)
        rs->StartProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressUpdate(VS_STACK *pS)
{
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (rs)
        rs->ProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressEnd(VS_STACK *pS)
{
    auto *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (rs)
        rs->EndProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeSleep(VS_STACK *pS)
{
    // Get strings
    auto *pInt = (VDATA *)pS->Pop();
    long delay = 1;
    if (!pInt || !pInt->Get(delay))
        return IFUNCRESULT_FAILED;
    if (delay < 1)
        delay = 1;
    Sleep(delay);
    return IFUNCRESULT_OK;
}

uint32_t slNativeExecuteTechnique(VS_STACK *pS)
{
    // Get string
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    // Execute technique
    if (nm && nm[0])
    {
        auto *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
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

uint32_t slSetAchievement(VS_STACK *pS)
{
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm && nm[0])
    {
        VDATA *pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        if (core.isSteamEnabled())
            ret = core.SetAchievementState(nm);

        else
            ret = 0;

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetAchievement(VS_STACK *pS)
{
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        VDATA *pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        if (core.isSteamEnabled())
            ret = core.GetAchievementState(nm);

        else
            ret = 0;

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slSetStat(VS_STACK *pS)
{
    VDATA *pInt = (VDATA *)pS->Pop();
    long val = 0;
    long ret = 0;
    if (!pInt->Get(val))
        return IFUNCRESULT_FAILED;

    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        VDATA *pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        if (core.isSteamEnabled())
            ret = core.SetStatValue(nm, val);

        else
            ret = 0;

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetStat(VS_STACK *pS)
{
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm && nm[0])
    {
        VDATA *pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        if (core.isSteamEnabled())
            ret = core.GetStatValue(nm);

        else
            ret = 0;

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slStoreStats(VS_STACK *pS)
{
    long ret = 0;
    VDATA *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;

    if (core.isSteamEnabled())
        ret = core.StoreStats();

    else
        ret = 0;

    pReturn->Set(ret);
    return IFUNCRESULT_OK;
}

uint32_t slClearAchievement(VS_STACK *pS)
{
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        VDATA *pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        if (core.isSteamEnabled())
            ret = core.ClearAchievement(nm);

        else
            ret = 0;

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slResetStats(VS_STACK *pS)
{
    VDATA *pInt = (VDATA *)pS->Pop();
    long val = 0;
    long ret = 0;
    if (!pInt->Get(val))
        return IFUNCRESULT_FAILED;

    VDATA *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;

    if (core.isSteamEnabled())
        ret = core.ResetStats(val);

    else
        ret = 0;

    pReturn->Set(ret);

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

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SetAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slSetAchievement;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slGetAchievement;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "SetStat";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slSetStat;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetStat";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slGetStat;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "StoreStats";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slStoreStats;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ClearAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slClearAchievement;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ResetStats";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slResetStats;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}
