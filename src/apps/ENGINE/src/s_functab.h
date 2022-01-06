#pragma once

#include "s_import_func.h"
#include "s_vartab.h"
#include "storm/string_compare.hpp"
#include <unordered_map>
#include <vector>

#define INVALID_FUNC_CODE 0xffffffff
#define INVALID_FUNC_OFFSET 0xffffffff
#define INTERNAL_SEGMENT_ID 0xffffffff
#define IMPORTED_SEGMENT_ID 0xfffffffe

// when offset value of function is INVALID_FUNC_OFFSET, function segment
// isnt currently loaded and function call is impossible
// when segment_id is INTERNAL_SEGMENT_ID, this function is internal and offset
// means internal function number

struct LocalVarInfo
{
    LocalVarInfo() = default;

    bool IsArray()
    {
        return elements > 1;
    }

    std::string name;
    size_t hash; // for faster comparison
    S_TOKEN_TYPE type;
    size_t elements;
};

struct FuncInfo
{
    FuncInfo();

    std::string name;

    std::vector<LocalVarInfo> local_vars;

    // compiler info
    uint32_t segment_id;
    uint32_t offset;
    uint32_t stack_offset;
    uint32_t arguments;
    S_TOKEN_TYPE return_type;

    // debug info
    std::string decl_file_name;
    uint32_t decl_line;

    // profile info
    uint64_t usage_time;
    uint32_t number_of_calls;

    // imported func info
    SIMPORTFUNC imported_func;
    uint32_t extern_arguments;
};

class FuncTable
{
  public:
    FuncTable() = default;
    ~FuncTable();

    // get func num
    size_t GetFuncNum() const;
    // get func index by name
    size_t FindFunc(const std::string &func_name) const;
    // add func to table, returns func index
    size_t AddFunc(const FuncInfo &fi);
    // get func by index, returns true if func is registered and loaded
    bool GetFunc(FuncInfo &fi, size_t func_index) const;
    // get func by index, returns true if func is registered
    bool GetFuncX(FuncInfo &fi, size_t func_index) const;
    // invalidate all segment's functions
    void InvalidateBySegmentID(uint32_t segment_id);

    // set func's compiler offset
    bool SetFuncOffset(const std::string &func_name, uint32_t offset);
    // add local var to func
    bool AddFuncVar(size_t func_index, const LocalVarInfo &lvi);
    // add arg to func (must precede all regular local vars in order not to break compiler logic)
    bool AddFuncArg(size_t func_index, const LocalVarInfo &lvi, bool is_extern = false);
    // get func's local var or arg index by name
    size_t FindVar(size_t func_index, const std::string &var_name) const;
    // get local var or arg by index
    bool GetVar(LocalVarInfo &lvi, size_t func_index, size_t var_index) const;

    bool AddTime(size_t func_index, uint64_t time);
    bool AddCall(size_t func_index);

    void Release(); // clear table

  private:
    std::vector<FuncInfo> funcs_;
    storm::iStrHasher hasher_;
    std::unordered_map<std::string, size_t, storm::iStrHasher, storm::iStrComparator> hash_table_;
};
