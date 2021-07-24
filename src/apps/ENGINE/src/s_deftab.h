#pragma once

#include "data.h"
#include <vector>

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define DEF_BUFFER_BLOCK_SIZE 1024
#define INVALID_DEF_CODE 0xffffffff
#define INVALID_SEGMENT_ID 0xffffffff

struct DEFINFO
{
    char *name;
    uint32_t segment_id;
    uint32_t hash;
    uintptr_t data4b;
    uint32_t deftype;
};

#define DTHASHT_SIZE 256

struct DTHASHLINE
{
    DTHASHLINE()
    {
        nNumElements = 0;
    };
    uint32_t nNumElements;
    std::vector<uint32_t> pElements;
};

class S_DEFTAB
{
    uint32_t Buffer_size;
    uint32_t Def_num;
    std::vector<DEFINFO> pTable;
    // bool bKeepName;
    DTHASHLINE HashLine[DTHASHT_SIZE];

  public:
    S_DEFTAB();
    ~S_DEFTAB();

    uint32_t GetDefNum()
    {
        return Def_num;
    };
    uint32_t AddDef(DEFINFO &di);
    bool GetDef(DEFINFO &di, uint32_t def_code);
    bool GetDefX(DEFINFO &vi, uint32_t def_code);
    uint32_t MakeHashValue(const char *string);
    //    void  KeepNameMode(bool on){bKeepName = on;};
    void Release();
    void InvalidateBySegmentID(uint32_t segment_id);
    uint32_t FindDef(const char *def_name);
    void UpdateHashTable(uint32_t code, uint32_t hash, bool in);
};
