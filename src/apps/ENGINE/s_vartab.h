#pragma once

#include "data.h"
#include "storm/string_compare.hpp"
#include <unordered_map>
#include <vector>

// when segment_id is INVALID_SEGMENT_ID, variable segment is unloaded
// and variable value and type undefined

#define INVALID_VAR_CODE 0xffffffff
#define INVALID_SEGMENT_ID 0xffffffff

struct VarInfo
{
    VarInfo();
    VarInfo(const VarInfo &other);
    VarInfo(VarInfo &&other) = default;
    VarInfo &operator=(const VarInfo &other);

    bool IsArray() const
    {
        return elements > 1;
    }

    std::string name;
    S_TOKEN_TYPE type;
    std::unique_ptr<DATA> value;
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

    // add var to table, returns var index
    size_t AddVar(const VarInfo &vi);
    // get var by index, returns non-null ptr if var is registered and loaded
    const VarInfo *GetVar(size_t var_index) const;
    // get var by index, returns non-null ptr if var is registered
    const VarInfo *GetVarX(size_t var_index) const;
    // invalidate all segment's vars
    void InvalidateBySegmentID(uint32_t segment_id);
    // get var index by name
    size_t FindVar(const std::string &var_name) const;
    // set var's array size, returns true if successful
    bool SetElementsNum(size_t var_index, size_t elements_num);
    // clear table
    void Release();

    void SetVCompiler(VIRTUAL_COMPILER *vc)
    {
        vc_ = vc;
    }

  private:
    std::vector<VarInfo> vars_;
    std::unordered_map<std::string, size_t, storm::iStrHasher, storm::iStrComparator>
        hash_table_; // name to index mapping
    VIRTUAL_COMPILER *vc_;
};
