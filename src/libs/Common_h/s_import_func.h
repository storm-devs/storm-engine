#pragma once

#include <cstdint>

enum FuncResult : uint32_t
{
    IFUNCRESULT_OK = 0U,
    IFUNCRESULT_FAILED = 0xffffffffU
};

#define INVALID_FUNCHANDLE 0xffffffffU

#include <cstdint>

class VS_STACK;
typedef uint32_t (*SIMPORTFUNC)(VS_STACK *);

struct IFUNCINFO
{
    IFUNCINFO() : nArguments(0)
    {
        pFuncName = nullptr;
        pReturnValueName = nullptr;
        pFuncAddress = nullptr;
        pDeclFileName = nullptr;
        nDeclLine = 0;
    };
    const char *pFuncName;
    const char *pReturnValueName;
    uint32_t nArguments;
    SIMPORTFUNC pFuncAddress;
    const char *pDeclFileName;
    uint32_t nDeclLine;
};
