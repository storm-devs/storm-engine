#pragma once
#include "s_functab.h"

#include "data.h"
#include <stdexcept>
#include <vector>

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

class ReaderException : public std::runtime_error
{
  public:
    ReaderException();
};

class BufferReader final
{
  public:
    explicit BufferReader(std::vector<char> &&buffer);

    template <typename To> std::enable_if_t<!std::is_enum_v<To>, To> Read()
    {
        static_assert(std::is_trivially_copyable_v<To>);

        To to;
        if (cur_pointer_ + sizeof(to) > std::size(buffer_))
        {
            throw ReaderException();
        }

        const auto from = std::data(buffer_) + cur_pointer_;
        std::copy_n(from, sizeof(to), reinterpret_cast<char *>(&to));
        cur_pointer_ += sizeof(to);

        return to;
    }

    template <typename To> std::enable_if_t<std::is_enum_v<To>, To> Read()
    {
        return static_cast<To>(Read<std::underlying_type_t<To>>());
    }

    [[nodiscard]] std::string_view ReadArray();

  private:
    std::vector<char> buffer_;
    size_t cur_pointer_;
};

class BufferWriter
{
  public:
    template <typename T> void WriteData(const T &buffer)
    {
        buffer_.reserve(sizeof(buffer_) + sizeof(buffer));
        std::copy_n(reinterpret_cast<const char *>(&buffer), sizeof(buffer), std::back_inserter(buffer_));
    }

    void WriteArray(std::string_view data);

    [[nodiscard]] std::string_view GetDataBuffer() const noexcept;

  private:
    std::vector<char> buffer_;
};

void ReadScriptData(BufferReader &reader, S_TOKEN_TYPE type, DATA *data);
void WriteScriptData(BufferWriter &writer, S_TOKEN_TYPE type, DATA *data);
} // namespace script_cache

struct ScriptCache
{
    std::vector<script_cache::Define> defines;
    std::vector<std::string> script_libs;
    std::vector<std::string> files;
    std::vector<script_cache::Function> functions;
    std::vector<VarInfo> variables;
    std::vector<script_cache::EventHandler> event_handlers;
};
} // namespace storm
