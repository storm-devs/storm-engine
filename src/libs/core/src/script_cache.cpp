#include "script_cache.h"

storm::script_cache::ReaderException::ReaderException()
    : std::runtime_error("Unable to read binary data. Scripts mismatch?")
{
}

storm::script_cache::BufferReader::BufferReader(std::vector<char> &&buffer)
    : buffer_(std::move(buffer)), cur_pointer_(0)
{
}

std::string_view storm::script_cache::BufferReader::ReadArray()
{
    const auto buffer_size = Read<size_t>();
    if (buffer_size == 0)
    {
        return {};
    }
    if (cur_pointer_ + buffer_size > std::size(buffer_))
    {
        throw ReaderException();
    }

    const std::string_view sv{std::data(buffer_) + cur_pointer_, buffer_size};
    cur_pointer_ += buffer_size;

    return sv;
}

void storm::script_cache::BufferWriter::WriteArray(std::string_view data)
{
    buffer_.reserve(std::size(buffer_) + sizeof(decltype(std::size(data))) + std::size(data));
    WriteData(std::size(data));
    std::ranges::copy(data, std::back_inserter(buffer_));
}

std::string_view storm::script_cache::BufferWriter::GetDataBuffer() const noexcept
{
    return {std::data(buffer_), std::size(buffer_)};
}

void storm::script_cache::ReadScriptData(BufferReader &reader, S_TOKEN_TYPE type, DATA *data)
{
    switch (type)
    {
    case VAR_INTEGER: {
        const auto value = reader.Read<int32_t>();
        data->Set(value);
        break;
    }

    case VAR_FLOAT: {
        const auto value = reader.Read<float>();
        data->Set(value);
        break;
    }
    case VAR_STRING: {
        const auto value = reader.ReadArray();
        data->Set(std::string(value));
        break;
    }
    }
}

void storm::script_cache::WriteScriptData(BufferWriter &writer, S_TOKEN_TYPE type, DATA *data)
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
        writer.WriteArray(value);
        break;
    }
    }
}
