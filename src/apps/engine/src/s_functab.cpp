#include "s_functab.h"

FuncInfo::FuncInfo()
    : name(), local_vars(), segment_id(INVALID_SEGMENT_ID), offset(INVALID_FUNC_OFFSET), stack_offset(), arguments(),
      return_type(TVOID), decl_file_name(), decl_line(), usage_time(), number_of_calls(), imported_func(),
      extern_arguments()
{
}

FuncTable::~FuncTable()
{
    Release();
}

size_t FuncTable::GetFuncNum() const
{
    return funcs_.size();
}

size_t FuncTable::FindFunc(const std::string &func_name) const
{
    auto result = hash_table_.find(func_name);

    if (result == hash_table_.end())
    {
        return INVALID_FUNC_CODE;
    }

    return result->second;
}

size_t FuncTable::AddFunc(const FuncInfo &fi)
{
    if (fi.name.empty())
    {
        return INVALID_FUNC_CODE;
    }

    auto result = hash_table_.try_emplace(fi.name, funcs_.size()); // find or create index
    auto func_index = result.first->second;
    bool is_new = result.second;

    if (is_new) // newly created function, add to table
    {
        funcs_.push_back(fi);
    }

    auto &func = funcs_[func_index];

    if (!is_new)
    {
        if (func.offset != INVALID_FUNC_OFFSET) // function already loaded
        {
            return INVALID_FUNC_CODE;
        }

        func = fi; // function exists, but was unloaded, copy data
    }

    return func_index;
}

bool FuncTable::GetFunc(FuncInfo &fi, size_t func_index) const
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    if (funcs_[func_index].segment_id == IMPORTED_SEGMENT_ID)
    {
        if (funcs_[func_index].imported_func == nullptr)
        {
            return false;
        }

        fi = funcs_[func_index]; // copy func info
        return true;
    }

    if (funcs_[func_index].offset == INVALID_FUNC_OFFSET)
    {
        return false;
    }

    fi = funcs_[func_index]; // copy func info
    return true;
}

bool FuncTable::GetFuncX(FuncInfo &fi, size_t func_index) const
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    fi = funcs_[func_index]; // copy func info
    return true;
}

void FuncTable::InvalidateBySegmentID(uint32_t segment_id)
{
    for (auto &fi : funcs_)
    {
        if (fi.segment_id == segment_id)
        {
            fi.segment_id = INVALID_SEGMENT_ID; // hash is not deleted from table
            fi.offset = INVALID_FUNC_OFFSET;
            fi.local_vars.clear(); // delete local vars
            fi.decl_file_name.clear();
        }
    }
}

bool FuncTable::SetFuncOffset(const std::string &func_name, uint32_t offset)
{
    const auto func_index = FindFunc(func_name);

    if (func_index == INVALID_FUNC_CODE)
    {
        return false;
    }

    funcs_[func_index].offset = offset;
    return true;
}

bool FuncTable::AddFuncVar(size_t func_index, const LocalVarInfo &lvi)
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    if (lvi.name.empty())
    {
        return false;
    }

    if (FindVar(func_index, lvi.name) != INVALID_VAR_CODE) // var already exists
    {
        return false;
    }

    auto &var = funcs_[func_index].local_vars.emplace_back(lvi);
    var.hash = hasher_(var.name);

    return true;
}

bool FuncTable::AddFuncArg(size_t func_index, const LocalVarInfo &lvi, bool is_extern)
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    if (is_extern)
    {
        ++funcs_[func_index].extern_arguments;
        return true;
    }

    ++funcs_[func_index].arguments;
    return AddFuncVar(func_index, lvi);
}

size_t FuncTable::FindVar(size_t func_index, const std::string &var_name) const
{
    if (func_index >= funcs_.size())
    {
        return INVALID_VAR_CODE;
    }

    if (var_name.empty())
    {
        return INVALID_VAR_CODE;
    }

    const auto &vars = funcs_[func_index].local_vars;
    size_t hash = hasher_(var_name);
    const auto result = std::find_if(vars.begin(), vars.end(), [hash, &var_name](const LocalVarInfo &var) {
        return var.hash == hash && storm::iEquals(var.name, var_name); // fast comparison
    });

    if (result == vars.end())
    {
        return INVALID_VAR_CODE;
    }

    return result - vars.begin();
}

bool FuncTable::GetVar(LocalVarInfo &lvi, size_t func_index, size_t var_index) const
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    const auto &vars = funcs_[func_index].local_vars;

    if (var_index >= vars.size())
    {
        return false;
    }

    lvi = vars[var_index];

    return true;
}

bool FuncTable::AddTime(size_t func_index, uint64_t time)
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    funcs_[func_index].usage_time += time;
    return true;
}

bool FuncTable::AddCall(size_t func_index)
{
    if (func_index >= funcs_.size())
    {
        return false;
    }

    ++funcs_[func_index].number_of_calls;
    return true;
}

void FuncTable::Release()
{
    funcs_.clear();
    hash_table_.clear();
}
