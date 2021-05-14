#pragma once

#include "s_import_func.h"
#include "s_vartab.h"
#include "strutils.h"
#include <vector>
#include <unordered_map>

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

    size_t FindFunc(const std::string &func_name) const; // get func index by name
    size_t AddFunc(const FuncInfo &fi); // add func to table, returns func index
    bool GetFunc(FuncInfo &fi, size_t func_index) const; // get func by index, returns true if func is registered and loaded
    bool GetFuncX(FuncInfo &fi, size_t func_index) const; // get func by index, returns true if func is registered
    void InvalidateBySegmentID(uint32_t segment_id); // invalidate all segment's functions
    
    bool SetFuncOffset(const std::string &func_name, uint32_t offset); // set func's compiler offset
    bool AddFuncVar(size_t func_index, const LocalVarInfo &lvi);       // add local var to func
    bool AddFuncArg(size_t func_index, const LocalVarInfo &lvi, bool is_extern = false); // add arg to func (must precede all regular local vars in order not to break compiler logic)
    size_t FindVar(size_t func_index, const std::string &var_name) const; // get func's local var or arg index by name
    bool GetVar(LocalVarInfo &lvi, size_t func_index, size_t var_index) const; // get local var or arg by index

    void AddTime(size_t func_index, uint64_t time);
    void AddCall(size_t func_index);

    void Release(); // clear table

  private:

    std::vector<FuncInfo> funcs_;
    CaseInsensitiveStringHasher hasher_;
    CaseInsensitiveStringComparator comparator_;
    std::unordered_map<std::string, size_t, CaseInsensitiveStringHasher, CaseInsensitiveStringComparator> hash_table_;
};
