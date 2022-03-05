#include "script_func.h"
#include "entity.h"
#include "core.h"
#include "s_import_func.h"
#include "sail.h"
#include "v_s_stack.h"

extern float g_fSailHoleDepend;
// extern float GetSailSpeed(int holeQ,int holeMax,float maxSpeed,float fSailHoleDepend);

uint32_t _ShipSailState(VS_STACK *pS)
{
    auto *pChrIdx = (VDATA *)pS->Pop();
    if (!pChrIdx)
        return IFUNCRESULT_FAILED;
    const auto nChrIdx = pChrIdx->GetInt();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;

    // find sail class
    if (const auto eid = EntityManager::GetEntityId("SAIL"))
    {
        const int32_t n = static_cast<SAIL *>(EntityManager::GetEntityPointer(eid))->GetSailStateForCharacter(nChrIdx);
        pVR->Set(n);
    }
    else
        pVR->Set(0);

    return IFUNCRESULT_OK;
}

uint32_t _GetAssembledString(VS_STACK *pS)
{
    auto *pAttrPnt = (VDATA *)pS->Pop();
    if (!pAttrPnt)
        return IFUNCRESULT_FAILED;
    auto *pAttr = pAttrPnt->GetAClass();

    auto *pFormatStr = (VDATA *)pS->Pop();
    if (!pFormatStr)
        return IFUNCRESULT_FAILED;
    auto *const formatStr = pFormatStr->GetString();

    char retString[1024];
    retString[0] = 0;
    if (formatStr != nullptr && pAttr != nullptr)
    {
        bool bBuildAccessString = false;
        char accessString[sizeof(retString)];
        accessString[0] = 0;
        int accessStrSize = 0;
        for (int i = 0;; i++)
        {
            if (formatStr[i] == '#' || !formatStr[i])
            {
                accessString[accessStrSize] = 0;
                if (bBuildAccessString)
                {
                    if (accessStrSize > 1)
                    {
                        int nAttrNameStart = 1;
                        if (accessString[0] == 'f' && accessString[1] == '.')
                        {
                            nAttrNameStart = 3;
                        }
                        ATTRIBUTES *pA = pAttr->FindAClass(pAttr, &accessString[nAttrNameStart]);
                        char *writeStr = nullptr;
                        if (pA != nullptr)
                            writeStr = pA->GetThisAttr();
                        if (writeStr)
                            switch (accessString[0])
                            {
                            case 's':
                                strcat_s(retString, writeStr);
                                break;
                            case 'f': {
                                char tmpp[256];
                                float ftmp = 0.f;
                                sscanf(writeStr, "%f", &ftmp);
                                if (nAttrNameStart == 1)
                                    sprintf_s(tmpp, "%f", ftmp);
                                else
                                {
                                    char tmpFmtStr[5];
                                    tmpFmtStr[0] = '%';
                                    tmpFmtStr[1] = '.';
                                    tmpFmtStr[2] = accessString[2];
                                    tmpFmtStr[3] = 'f';
                                    tmpFmtStr[4] = 0;
                                    sprintf_s(tmpp, tmpFmtStr, ftmp);
                                }
                                strcat_s(retString, tmpp);
                            }
                            break;
                            case 'd': {
                                char tmpp[256];
                                int ntmp = 0;
                                sscanf(writeStr, "%d", &ntmp);
                                sprintf_s(tmpp, "%d", ntmp);
                                strcat_s(retString, tmpp);
                            }
                            break;
                            }
                    }
                }
                else
                    strcat_s(retString, accessString);
                bBuildAccessString = !bBuildAccessString;
                accessStrSize = 0;
            }
            else
            {
                accessString[accessStrSize++] = formatStr[i];
            }
            if (!formatStr[i])
                break;
        }
    }

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(retString);
    return IFUNCRESULT_OK;
}

uint32_t _funcGetSailSpeed(VS_STACK *pS)
{
    auto *pSailPow = (VDATA *)pS->Pop();
    const float fSailPow = pSailPow->GetFloat();

    auto *pHoleMax = (VDATA *)pS->Pop();
    const int32_t nHoleMax = pHoleMax->GetInt();

    auto *pHoleQ = (VDATA *)pS->Pop();
    const int32_t nHoleQ = pHoleQ->GetInt();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(fSailPow - GetSailSpeed(nHoleQ, nHoleMax, fSailPow));

    return IFUNCRESULT_OK;
}

uint32_t _RandomHole2Sail(VS_STACK *pS)
{
    VDATA *pData;

    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    int _addHoleQ = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const uint32_t _holeData = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    int _maxHole = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const int _groupNum = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const char *_reyName = pData->GetString();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const int _chrIdx = pData->GetInt();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;

    SAILONE_BASE *pSail = nullptr;
    if (const auto ei = EntityManager::GetEntityId("sail"))
    {
        pSail = static_cast<SAIL_BASE *>(EntityManager::GetEntityPointer(ei))
                    ->FindSailForCharacter(_chrIdx, _reyName, _groupNum);
    }

    int holeArraySize = 0;
    int holeIdx[20];
    uint32_t holeMask = _holeData;
    for (int i = 0; holeMask > 0; i++, holeMask >>= 1)
        if (!(holeMask & 1))
            holeIdx[holeArraySize++] = i;

    holeMask = _holeData;
    while (holeArraySize > 0 && _addHoleQ > 0)
    {
        const int i = rand() % holeArraySize;
        holeMask |= 1 << holeIdx[i];
        holeArraySize--;
        _addHoleQ--;
        holeIdx[i] = holeIdx[holeArraySize];
    }

    if (pSail != nullptr && holeMask != _holeData)
    {
        pSail->SetAllHole(holeMask);
        pSail->CalculateMirrorSailIndex();
    }

    pVR->Set(static_cast<int32_t>(holeMask));

    return IFUNCRESULT_OK;
}

uint32_t _DeleteOneSailHole(VS_STACK *pS)
{
    VDATA *pData;

    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    int _delHoleQ = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const uint32_t _holeData = pData->GetInt();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const char *_reyName = pData->GetString();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const char *_groupName = pData->GetString();
    if (!(pData = (VDATA *)pS->Pop()))
        return IFUNCRESULT_FAILED;
    const int _chrIdx = pData->GetInt();

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;

    int _groupNum;
    sscanf(_groupName, "%d", &_groupNum);

    SAILONE_BASE *pSail = nullptr;
    if (const auto ei = EntityManager::GetEntityId("sail"))
    {
        pSail = static_cast<SAIL_BASE *>(EntityManager::GetEntityPointer(ei))
                    ->FindSailForCharacter(_chrIdx, _reyName, _groupNum);
    }

    int holeArraySize = 0;
    int holeIdx[20];
    uint32_t holeMask = _holeData;
    for (int i = 0; holeMask > 0; i++, holeMask >>= 1)
        if (holeMask & 1)
            holeIdx[holeArraySize++] = i;

    holeMask = _holeData;
    while (holeArraySize > 0 && _delHoleQ > 0)
    {
        const int i = rand() % holeArraySize;
        holeMask &= ~(1 << holeIdx[i]);
        holeArraySize--;
        _delHoleQ--;
        holeIdx[i] = holeIdx[holeArraySize];
    }

    if (pSail != nullptr && holeMask != _holeData)
    {
        pSail->SetAllHole(holeMask);
        pSail->CalculateMirrorSailIndex();
    }

    pVR->Set(static_cast<int32_t>(holeMask));

    return IFUNCRESULT_OK;
}

bool SCRIPT_RIGGING_FILES::Init()
{
    IFUNCINFO sIFuncInfo;

    sIFuncInfo.nArguments = 3;
    sIFuncInfo.pFuncName = "funcGetSailSpeed";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _funcGetSailSpeed;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 6;
    sIFuncInfo.pFuncName = "RandomHole2Sail";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = _RandomHole2Sail;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 5;
    sIFuncInfo.pFuncName = "DeleteOneSailHole";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = _DeleteOneSailHole;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "GetAssembledString";
    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncAddress = _GetAssembledString;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ShipSailState";
    sIFuncInfo.pReturnValueName = "float";
    sIFuncInfo.pFuncAddress = _ShipSailState;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}
