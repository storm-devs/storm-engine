#include "core.h"
#include "defines.h"
#include "s_import_func.h"
#include "v_s_stack.h"
#include "SteamApi.hpp"
#include "script_libriary.h"

namespace steamapi
{

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

class SteamApiScriptLib : public SCRIPT_LIBRIARY
{
  public:
    bool Init() override;
};

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

    return true;
}
} // namespace
} // namespace steamapi
