#pragma once
#include "s_functab.h"

#include <vector>
#include "data.h"

#include <zlib.h>

namespace storm
{
namespace script_cache
{
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

struct Define
{
    std::string name;
    uint32_t type;
    uintptr_t value;
};

class Reader final
{
public:
    explicit Reader(std::string_view data)
        : data_(data), offset_(0)
    {
    }

    template <typename T>
    const T *ReadData()
    {
        const auto size = sizeof(T);
        if (offset_ + size > data_.size())
        {
            return nullptr;
        }

        auto address = data_.data() + offset_;
        offset_ += size;
        return reinterpret_cast<const T *>(address);
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
        auto data_bytes = std::string_view(reinterpret_cast<const char *>(&data), sizeof(data));
        std::ranges::copy(data_bytes, std::back_inserter(data_));
    }

    void WriteBytes(std::string_view data)
    {
        WriteData(data.size());
        std::ranges::copy(data, std::back_inserter(data_));
    }

    auto &GetData() const noexcept
    {
        return data_;
    }

private:
    std::vector<char> data_;
};

inline void ReadScriptData(Reader &reader, S_TOKEN_TYPE type, DATA *data)
{
    switch (type)
    {
    case VAR_INTEGER: {
        const auto value = reader.ReadData<int32_t>();
        data->Set(*value);
        break;
    }

    case VAR_FLOAT: {
        const auto value = reader.ReadData<float>();
        data->Set(*value);
        break;
    }
    case VAR_STRING: {
        const auto value = reader.ReadBytes();
        data->Set(std::string(value));
        break;
    }
    }
}

inline void WriteScriptData(Writer &writer, S_TOKEN_TYPE type, DATA *data)
{
    switch (type)
    {
    case VAR_INTEGER: {
        const auto value = data->GetInt();
        writer.WriteData(value);
        break;
    }

    case VAR_FLOAT: {
        const auto value = data->GetFloat();
        writer.WriteData(value);
        break;
    }
    case VAR_STRING: {
        const auto value = data->GetString();
        writer.WriteBytes(value);
        break;
    }
    }
}

inline uint32_t ComputeCRC(uint32_t crc, std::string_view data)
{
    return crc32(crc, reinterpret_cast<const Bytef *>(data.data()), data.size());
}
}

struct ScriptCache
{
    std::vector<script_cache::Define> defines;
    std::vector<std::string> script_libs;
    std::vector<std::string> files;
    std::vector<script_cache::Function> functions;
    std::vector<VarInfo> variables;
    std::vector<script_cache::EventHandler> event_handlers;
    uint32_t crc;
};
}
