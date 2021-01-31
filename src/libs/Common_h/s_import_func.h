#ifndef _S_IMPORT_FUNC_H_
#define _S_IMPORT_FUNC_H_

#define IFUNCRESULT_OK 0
#define IFUNCRESULT_FAILED 0xffffffff

#define INVALID_FUNCHANDLE 0xffffffff

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

#endif
