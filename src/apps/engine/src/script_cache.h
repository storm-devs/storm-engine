#pragma once
#include "s_functab.h"

#include <istream>
#include <ostream>
#include <vector>
#include <zlib.h>

namespace storm
{
namespace script_cache
{
struct Define
{
    std::string name;
    uint32_t type;
    uintptr_t value;
};

struct FunctionLocalVariable
{
    LocalVarInfo info;
    bool is_extern;
};

struct Function
{
    FuncInfo info;
    std::vector<FunctionLocalVariable> arguments;
    std::vector<LocalVarInfo> local_variables;
};

struct EventHandler
{
    std::string event_name;
    std::string function_name;
};

class Reader final
{
public:
    explicit Reader(std::string_view data)
        : data_(data), offset_(0)
    {
    }

    template <typename T>
    T *ReadData()
    {
        const auto size = sizeof(T);
        if (offset_ + size > data_.size())
        {
            return nullptr;
        }

        auto address = data_.data() + offset_;
        offset_ += size;
        return address;
    }

    std::string_view ReadBytes()
    {
        auto size = ReadData<size_t>();
        if (!size)
        {
            return {};
        }

        if (offset_ + *size > data_.size())
        {
            return {};
        }

        const auto address = data_.data() + offset_;
        offset_ += *size;
        return {address, *size};
    }

private:
    std::string_view data_;
    size_t offset_;
};

class Writer
{
public:
    template <typename T>
    void WriteData(const T &data)
    {
        const auto size = data_.size();
        data_.resize(size + sizeof data);
        std::memcpy(data_.data() + size, &data, sizeof(T));
    }

    void WriteBytes(std::string_view data)
    {
        WriteData(data.size());
        const auto size = data_.size();
        data_.resize(size + data.size());
        std::memcpy(data_.data() + size, data.data(), data.size());
    }

private:
    std::vector<char> data_;
};
}

struct ScriptCache
{
    std::vector<script_cache::Define> defines;
    std::vector<std::string> files;
    std::vector<script_cache::Function> functions;
    std::vector<VarInfo> variables;
    std::vector<script_cache::EventHandler> event_handlers;
    std::vector<char> code;
};
}
