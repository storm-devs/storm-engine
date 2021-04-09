#pragma once

#include "s_import_func.h"
#include "s_vartab.h"
#include <vector>

#define FUNC_BUFFER_BLOCK_SIZE 1024
#define INVALID_FUNC_CODE 0xffffffff
#define INVALID_FUNC_OFFSET 0xffffffff
#define INTERNAL_SEGMENT_ID 0xffffffff
#define IMPORTED_SEGMENT_ID 0xfffffffe

// when offset value of function is INVALID_FUNC_OFFSET, function segment
// isnt currently loaded and function call is impossible
// when segment_id is INTERNAL_SEGMENT_ID, this function is internal and offset
// means internal function number

struct LVARINFO
{
    S_TOKEN_TYPE type;
    uint32_t hash;
    const char *name;
    uint32_t elements;
    bool bArray;
};

struct FUNCINFO
{
    uint32_t segment_id;
    uint32_t offset;
    uint32_t hash;
    const char *name;
    uint32_t arguments;
    uint32_t var_num;
    uint32_t stack_offset;
    std::vector<LVARINFO> pLocal;
    S_TOKEN_TYPE return_type;
    const char *decl_file_name;
    uint32_t decl_line;
    uint32_t code;
    double fTimeUsage;
    uint32_t nNumberOfCalls;
    SIMPORTFUNC pImportedFunc;
    uint32_t ext_args;
};

#define HASHT_SIZE 256

struct HASHLINE
{
    HASHLINE()
    {
        nNumElements = 0;
    }
    uint32_t nNumElements;
    std::vector<uint32_t> pElements;
};

class S_FUNCTAB
{
    uint32_t Buffer_size;
    uint32_t Func_num;
    std::vector<FUNCINFO> pTable;
    // bool bKeepName;

    HASHLINE HashLine[HASHT_SIZE];

  public:
    S_FUNCTAB();
    ~S_FUNCTAB();
    uint32_t GetFuncNum()
    {
        return Func_num;
    };
    uint32_t AddFunc(const FUNCINFO &fi);
    bool GetFunc(FUNCINFO &fi, uint32_t func_code);  // return true if func registred and loaded
    bool GetFuncX(FUNCINFO &fi, uint32_t func_code); // return true if func registred
    uint32_t MakeHashValue(const char *string);
    // void  KeepNameMode(bool on){bKeepName = on;};
    void Release();
    void InvalidateBySegmentID(uint32_t segment_id);
    uint32_t FindFunc(const char *func_name);
    bool SetFuncOffset(const char *func_name, uint32_t offset);
    bool AddFuncVar(uint32_t func_code, LVARINFO &lvi);
    bool AddFuncArg(uint32_t func_code, LVARINFO &lvi, bool bExt = false);
    uint32_t FindVar(uint32_t func_code, const char *var_name);
    void AddTime(uint32_t func_code, uint64_t time);
    void SetTimeUsage(uint32_t func_code, double f);
    void AddCall(uint32_t func_code);
    void InvalidateFunction(uint32_t nFuncHandle);
    void UpdateHashTable(uint32_t code, uint32_t hash, bool in);
    bool GetVar(LVARINFO &lvi, uint32_t func_code, uint32_t var_code);
};
