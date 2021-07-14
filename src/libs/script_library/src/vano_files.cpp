#include "vano_files.h"

#include "core.h"

#include "defines.h"
#include "s_import_func.h"
#include "v_s_stack.h"
#include "vmodule_api.h"

uint32_t _RDTSC_B(VS_STACK *pS)
{
    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    uint64_t dwRDTSC;
    RDTSC_B(dwRDTSC);
    pVR->Set(static_cast<long>(dwRDTSC));
    return IFUNCRESULT_OK;
}

uint32_t _RDTSC_E(VS_STACK *pS)
{
    auto *pValue = (VDATA *)pS->Pop();
    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    uint64_t dwRDTSC = static_cast<uint32_t>(pValue->GetLong());
    RDTSC_E(dwRDTSC);
    pVR->Set(static_cast<long>(dwRDTSC));
    return IFUNCRESULT_OK;
}

uint32_t _Clampf(VS_STACK *pS)
{
    auto *pValue = (VDATA *)pS->Pop();
    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(Clamp(pValue->GetFloat()));

    return IFUNCRESULT_OK;
}

uint32_t _Degree2Radian(VS_STACK *pS)
{
    auto *pValue = (VDATA *)pS->Pop();
    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;

    pVR->Set(PIm2 * pValue->GetFloat() / 360.0f);

    return IFUNCRESULT_OK;
}

uint32_t _Bring2Range(VS_STACK *pS)
{
    auto *pValue = (VDATA *)pS->Pop();
    auto *pMax2 = (VDATA *)pS->Pop();
    auto *pMin2 = (VDATA *)pS->Pop();
    auto *pMax1 = (VDATA *)pS->Pop();
    auto pMin1 = (VDATA *)pS->Pop();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(
        Bring2Range(pMin1->GetFloat(), pMax1->GetFloat(), pMin2->GetFloat(), pMax2->GetFloat(), pValue->GetFloat()));

    return IFUNCRESULT_OK;
}

uint32_t _Bring2RangeNoCheck(VS_STACK *pS)
{
    auto *pValue = (VDATA *)pS->Pop();
    auto *pMax2 = (VDATA *)pS->Pop();
    auto pMin2 = (VDATA *)pS->Pop();
    auto *pMax1 = (VDATA *)pS->Pop();
    auto *pMin1 = (VDATA *)pS->Pop();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(Bring2RangeNoCheck(pMin1->GetFloat(), pMax1->GetFloat(), pMin2->GetFloat(), pMax2->GetFloat(),
                                pValue->GetFloat()));

    return IFUNCRESULT_OK;
}

void Vano_Init()
{
    IFUNCINFO sIFuncInfo;

    sIFuncInfo.nArguments = 5;
    sIFuncInfo.pFuncName = "Bring2Range";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _Bring2Range;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 5;
    sIFuncInfo.pFuncName = "Bring2RangeNoCheck";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _Bring2RangeNoCheck;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "Degree2Radian";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _Degree2Radian;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "Clampf";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _Clampf;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncName = "rdtsc_b";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = _RDTSC_B;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "rdtsc_e";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = _RDTSC_E;
    core.SetScriptFunction(&sIFuncInfo);
}
