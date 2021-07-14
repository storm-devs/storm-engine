#include "SteamApiScriptLib.hpp"

#include "SteamApi.hpp"
#include "core.h"
#include "defines.h"
#include "s_import_func.h"
#include "v_s_stack.h"

namespace steamapi
{
CREATE_SCRIPTLIBRIARY(SteamApiScriptLib)

namespace
{
uint32_t getSteamEnabled(VS_STACK *pS)
{
    auto *pV = pS->Push();
    if (!pV)
    {
        return IFUNCRESULT_FAILED;
    }
    pV->Set((long)SteamApi::getInstance().isSteamEnabled());
    return IFUNCRESULT_OK;
}

uint32_t getDlcEnabled(VS_STACK *pS)
{
    auto *pV = pS->Pop();
    long dlc{};
    if (!pV->Get(dlc))
    {
        return IFUNCRESULT_FAILED;
    }

    const auto dlc_active = SteamApi::getInstance().isDLCActive(dlc);
    pV = pS->Push();
    if (!pV)
    {
        return IFUNCRESULT_FAILED;
    }

    pV->Set((long)dlc_active);
    return IFUNCRESULT_OK;
}

uint32_t getDlcCount(VS_STACK *pS)
{
    auto *pV = pS->Push();
    if (!pV)
    {
        return IFUNCRESULT_FAILED;
    }

    pV->Set((long)SteamApi::getInstance().getDLCCount());
    return IFUNCRESULT_OK;
}

uint32_t getDlcData(VS_STACK *pS)
{
    auto *pV = pS->Pop();
    long dlc{};
    if (!pV->Get(dlc))
    {
        return IFUNCRESULT_FAILED;
    }

    const auto dlc_data = SteamApi::getInstance().getDLCDataByIndex(dlc);
    pV = pS->Push();
    if (!pV)
    {
        return IFUNCRESULT_FAILED;
    }

    pV->Set((long)dlc_data);
    return IFUNCRESULT_OK;
}

uint32_t activateGameOverlayDlc(VS_STACK *pS)
{
    auto *pV = pS->Pop();
    long dlc{};
    if (!pV->Get(dlc))
    {
        return IFUNCRESULT_FAILED;
    }

    const auto success = SteamApi::getInstance().activateGameOverlayDLC(dlc);
    pV = pS->Push();
    if (!pV)
    {
        return IFUNCRESULT_FAILED;
    }

    pV->Set((long)success);
    return IFUNCRESULT_OK;
}

uint32_t slSetAchievement(VS_STACK *pS)
{
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm && nm[0])
    {
        auto pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        ret = SteamApi::getInstance().SetAchievementState(nm);

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetAchievement(VS_STACK *pS)
{
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        auto pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        ret = SteamApi::getInstance().SetAchievementState(nm);

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slSetStat(VS_STACK *pS)
{
    auto pInt = (VDATA *)pS->Pop();
    long val = 0;
    long ret = 0;
    if (!pInt->Get(val))
        return IFUNCRESULT_FAILED;

    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        auto pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        ret = SteamApi::getInstance().SetStatValue(nm, val);

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slGetStat(VS_STACK *pS)
{
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;
    if (nm && nm[0])
    {
        auto pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        ret = SteamApi::getInstance().GetStatValue(nm);

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slStoreStats(VS_STACK *pS)
{
    long ret = 0;
    auto pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;

    ret = SteamApi::getInstance().StoreStats();

    pReturn->Set(ret);
    return IFUNCRESULT_OK;
}

uint32_t slClearAchievement(VS_STACK *pS)
{
    auto pStr = (VDATA *)pS->Pop();
    const char *nm = nullptr;
    long ret = 0;
    if (!pStr->Get(nm))
        return IFUNCRESULT_FAILED;

    if (nm && nm[0])
    {
        auto pReturn = (VDATA *)pS->Push();
        if (!pReturn)
            return IFUNCRESULT_FAILED;

        ret = SteamApi::getInstance().ClearAchievement(nm);

        pReturn->Set(ret);
        return IFUNCRESULT_OK;
    }
    return IFUNCRESULT_OK;
}

uint32_t slResetStats(VS_STACK *pS)
{
    auto pInt = (VDATA *)pS->Pop();
    long val = 0;
    long ret = 0;
    if (!pInt->Get(val))
        return IFUNCRESULT_FAILED;

    auto pReturn = (VDATA *)pS->Push();
    if (!pReturn)
        return IFUNCRESULT_FAILED;

    ret = SteamApi::getInstance().ResetStats(val);

    pReturn->Set(ret);

    return IFUNCRESULT_OK;
}
} // namespace

bool SteamApiScriptLib::Init()
{
    IFUNCINFO sIFuncInfo;

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "GetSteamEnabled";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = getSteamEnabled;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetDLCenabled";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = getDlcEnabled;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "GetDLCCount";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = getDlcCount;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetDLCData";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = getDlcData;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "DLCStartOverlay";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = activateGameOverlayDlc;
    core.SetScriptFunction(&sIFuncInfo);

    ///////

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
} // namespace steamapi
