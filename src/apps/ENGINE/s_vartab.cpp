#include "s_vartab.h"

VarInfo::VarInfo() : name(), type(TVOID), value(), elements(0), segment_id(INVALID_SEGMENT_ID)
{
}

VarTable::~VarTable()
{
    Release();
}

size_t VarTable::AddVar(const VarInfo &vi)
{
    if (vi.name.empty())
    {
        return INVALID_VAR_CODE;
    }

    auto result = hash_table_.try_emplace(vi.name, vars_.size()); // find or create index
    auto var_index = result.first->second;
    bool isNew = result.second;

    if (isNew) // newly created var, add to table
    {
        vars_.push_back(vi);
    }

    auto &var = vars_[var_index];

    if (!isNew)
    {
        if (var.segment_id != INVALID_SEGMENT_ID) // variable already loaded
        {
            return INVALID_VAR_CODE;
        }

        var = vi; // variable exists, but was unloaded, copy data
    }

    var.value = std::make_shared<DATA>();
    var.value->SetVCompiler(vc_);
    var.value->SetType(var.type, var.elements);
    var.value->SetGlobalVarTableIndex(var_index); // todo change to size_t

    return var_index;
}

bool VarTable::GetVar(VarInfo &vi, size_t var_index) const
{
    if (var_index >= vars_.size())
    {
        return false;
    }

    if (vars_[var_index].segment_id == INVALID_SEGMENT_ID)
    {
        return false;
    }

    vi = vars_[var_index]; // copy var info
    return true;
}

bool VarTable::GetVarX(VarInfo &vi, size_t var_index) const
{
    if (var_index >= vars_.size())
    {
        return false;
    }

    vi = vars_[var_index]; // copy var info
    return true;
}

void VarTable::InvalidateBySegmentID(uint32_t segment_id)
{
    for (auto &vi : vars_)
    {
        if (vi.segment_id == segment_id)
        {
            vi.segment_id = INVALID_SEGMENT_ID; // hash is not deleted from table
        }
    }
}

size_t VarTable::FindVar(const std::string &var_name) const
{
    auto result = hash_table_.find(var_name);

    if (result == hash_table_.end())
    {
        return INVALID_VAR_CODE;
    }

    return result->second;
}

void VarTable::SetElementsNum(size_t var_index, size_t elements_num)
{
    if (var_index >= vars_.size())
    {
        return;
    }

    if (!vars_[var_index].value->IsArray())
    {
        return;
    }

    vars_[var_index].elements = elements_num;
}

void VarTable::Release()
{
    vars_.clear();
    hash_table_.clear();
}
