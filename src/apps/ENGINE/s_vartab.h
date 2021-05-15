#pragma once

#include "data.h"
#include "strutils.h"
#include <unordered_map>
#include <vector>

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define INVALID_VAR_CODE 0xffffffff
#define INVALID_SEGMENT_ID 0xffffffff

struct VarInfo
{
    VarInfo();

    bool IsArray()
    {
        return elements > 1;
    }

    std::string name;
    S_TOKEN_TYPE type;
    std::shared_ptr<DATA> value;
    size_t elements;
    uint32_t segment_id;
};

class VarTable
{
  public:
    VarTable() = default;
    ~VarTable();

    size_t GetVarNum() const // get var table size
    {
        return vars_.size();
    };

    size_t AddVar(const VarInfo &vi);                  // add var to table, returns var index
    bool GetVar(VarInfo &vi, size_t var_index) const;  // get var by index, returns true if var registered and loaded
    bool GetVarX(VarInfo &vi, size_t var_index) const; // get var by index, return true if var registered
    void InvalidateBySegmentID(uint32_t segment_id);   // invalidate all segment's vars
    size_t FindVar(const std::string &var_name) const; // get var index by name
    void SetElementsNum(size_t var_index, size_t elements_num); // set var's array size
    void Release();                                             // clear table

    void SetVCompiler(VIRTUAL_COMPILER *vc)
    {
        vc_ = vc;
    }

  private:
    std::vector<VarInfo> vars_;
    std::unordered_map<std::string, size_t, CaseInsensitiveStringHasher, CaseInsensitiveStringComparator>
        hash_table_; // name to index mapping
    VIRTUAL_COMPILER *vc_;
};
