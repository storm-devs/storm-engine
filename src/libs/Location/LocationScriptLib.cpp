#include "LocationScriptLib.h"
#include "EntityManager.h"
#include "Fader.h"
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
    //Ищем ячейку для записи
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
    //Получить строки
    auto *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm)
        charactersFindBuf.Set(nm);
    else
        charactersFindBuf.Set("");
    //Массив персонажей
    auto *pArray = (VDATA *)pS->Pop();
    if (!pArray)
        return IFUNCRESULT_FAILED;
    pArray = (VDATA *)pArray->GetReference();
    if (!pArray)
        return IFUNCRESULT_FAILED;
    //Возвращаемое значение
    auto *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    if (!charactersFindBuf.name[0])
    {
        pReturn->Set(-1L);
        return IFUNCRESULT_OK;
    }
    //Снижаем значения использования в кеше
    for (long i = 0; i < cacheSize; i++)
    {
        cache[i].use--;
        if (cache[i].use < 0)
            cache[i].use = 0;
    }
    //Смотрим в кеше
    bool res;
    for (long i = 0; i < cacheSize; i++)
    {
        if (cache[i].index < 0)
            continue;
        if (!cache[i].Cmp(charactersFindBuf))
            continue;
        //Проверяем на правильность кешь-значения
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
            //Нашли в кеше, возвращаем
            cache[i].use++;
            pReturn->Set(cache[i].index);
            return IFUNCRESULT_OK;
        }
    }
    //Придётся искать по массиву
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
    //Возвращаемое значение
    VDATA *pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;
    //Ищим локацию
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
    //Получить строки
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    //Устанавливаем картинку
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (rs)
    {
        rs->SetProgressImage(nm);
    }
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressStart(VS_STACK *pS)
{
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (rs)
        rs->StartProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressUpdate(VS_STACK *pS)
{
    VDX9RENDER *rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (rs)
        rs->ProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeReloadProgressEnd(VS_STACK *pS)
{
    auto *rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (rs)
        rs->EndProgressView();
    return IFUNCRESULT_OK;
}

uint32_t slNativeSleep(VS_STACK *pS)
{
    //Получить строки
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
    //Получить строку
    VDATA *pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    //Исполить технику
    if (nm && nm[0])
    {
        auto *rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
        rs->TechniqueExecuteStart(nm);
        while (rs->TechniqueExecuteNext())
            ;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetNextLineString(VS_STACK *pS)
{
    //Возвращаемое значение
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

        if (api->isSteamEnabled())
            ret = api->SetAchievementState(nm);

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

        if (api->isSteamEnabled())
            ret = api->GetAchievementState(nm);

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

        if (api->isSteamEnabled())
            ret = api->SetStatValue(nm, val);

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

        if (api->isSteamEnabled())
            ret = api->GetStatValue(nm);

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

    if (api->isSteamEnabled())
        ret = api->StoreStats();

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

        if (api->isSteamEnabled())
            ret = api->ClearAchievement(nm);

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

    if (api->isSteamEnabled())
        ret = api->ResetStats(val);

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
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "NativeFindLocation";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slNativeFindLocation;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "NativeFindLoadCharacter";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slNativeFindLaodLocation;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SetReloadProgressBackImage";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSetReloadBackImage;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "SetReloadNextTipsImage";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSetReloadNextTipsImage;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressStart";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressStart;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressUpdate";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressUpdate;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "ReloadProgressEnd";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeReloadProgressEnd;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SystemDelay";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeSleep;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ExecuteTechnique";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slNativeExecuteTechnique;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "GetNextLineString";
    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncAddress = slGetNextLineString;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "SetAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slSetAchievement;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slGetAchievement;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "SetStat";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slSetStat;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetStat";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slGetStat;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "StoreStats";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slStoreStats;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ClearAchievement";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slClearAchievement;
    api->SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ResetStats";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slResetStats;
    api->SetScriptFunction(&sIFuncInfo);

    return true;
}
