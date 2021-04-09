#pragma once

#include "data.h"
#include <vector>

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define VAR_BUFFER_BLOCK_SIZE 1024
#define INVALID_VAR_CODE 0xffffffff
#define INVALID_SEGMENT_ID 0xffffffff

struct VARINFO
{
    VARINFO()
    {
        pDClass = nullptr;
        name = nullptr;
        type = UNKNOWN;
        bArray = false;
        elements = 0;
        segment_id = INVALID_SEGMENT_ID;
        hash = 0;
    };
    uint32_t segment_id;
    uint32_t hash;
    DATA *pDClass;
    char *name;
    uint32_t elements;
    S_TOKEN_TYPE type;
    bool bArray;
};

class S_DEBUG;

#define VTHASHT_SIZE 256

struct VTHASHLINE
{
    VTHASHLINE()
    {
        nNumElements = 0;
    };
    uint32_t nNumElements;
    std::vector<uint32_t> pElements;
};

class S_VARTAB
{
    friend S_DEBUG;

    uint32_t Buffer_size;
    uint32_t Var_num;
    std::vector<VARINFO> pTable;
    // bool bKeepName;
    uint32_t Global_var_num;
    VIRTUAL_COMPILER *pVCompiler;
    VTHASHLINE HashLine[VTHASHT_SIZE];

  public:
    S_VARTAB();
    ~S_VARTAB();
    uint32_t GetVarNum()
    {
        return Var_num;
    };
    uint32_t AddVar(VARINFO &vi);
    bool GetVar(VARINFO &vi, uint32_t var_code);  // return true if var registred and loaded
    bool GetVarX(VARINFO &vi, uint32_t var_code); // return true if var registred
    uint32_t MakeHashValue(const char *string);
    //    void  KeepNameMode(bool on){bKeepName = on;};
    void Release();
    void InvalidateBySegmentID(uint32_t segment_id);
    uint32_t FindVar(const char *var_name);
    void SetElementsNum(uint32_t var_code, uint32_t elements_num){};
    void SetType(uint32_t var_code, uint32_t type){};
    void SetVCompiler(VIRTUAL_COMPILER *pvc)
    {
        pVCompiler = pvc;
    }
    bool ArraySizeChanged(uint32_t nIndex, uint32_t nNewSize);
    void UpdateHashTable(uint32_t code, uint32_t hash, bool in);
};
