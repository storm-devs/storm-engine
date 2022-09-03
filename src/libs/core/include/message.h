#pragma once

#include <cstdarg>
#include <cstring>
#include <variant>

#include "c_vector.h"
#include "entity.h"

class VDATA;

namespace storm
{
using MessageParam = std::variant<uint8_t, uint16_t, uint32_t, int32_t, float, double, ATTRIBUTES *, entid_t, VDATA *,
                                  CVECTOR, std::string>;

namespace detail {

template<typename T>
MessageParam convertMessageParam(T value)
{
    return MessageParam(value);
}

template<>
inline MessageParam convertMessageParam(const bool value)
{
    return {value ? 1 : 0};
}

// Convert uint32_t to int32_t to prevent conversion issues
template<>
inline MessageParam convertMessageParam(const uint32_t value)
{
    return MessageParam(static_cast<int32_t>(value));
}

} // namespace detail

} // namespace storm

class MESSAGE final
{
  public:
    void Move2Start();

    uint8_t Byte();
    uint16_t Word();
    int32_t Long();
    int32_t Dword();
    float Float();
    double Double();
    uintptr_t Pointer();
    ATTRIBUTES *AttributePointer();
    entid_t EntityID();
    VDATA *ScriptVariablePointer();
    CVECTOR CVector();
    const std::string &String();

    bool Set(int32_t value);
    bool Set(uintptr_t value);
    bool Set(float value);
    bool Set(std::string value);
    bool SetEntity(entid_t value);
    bool Set(VDATA *value);
    bool Set(ATTRIBUTES *value);

    void ValidateFormat(char c);
    void Reset(const std::string_view &format);

    template<typename... Args>
    void Reset(const std::string_view &format, Args... args)
    {
        Assert(format.size() == sizeof...(args));
        index = 0;
        format_ = format;
        params_ = {storm::detail::convertMessageParam(args)...};
    }

    void ResetVA(const std::string_view &format, va_list &args);
    char GetCurrentFormatType();;
    const char *StringPointer();

    [[nodiscard]] std::string_view GetFormat() const;
    [[nodiscard]] bool ParamValid() const;

private:
    static storm::MessageParam GetParamValue(const char c, va_list &args);

    std::string format_;
    std::vector<storm::MessageParam> params_;
    int32_t index{};
};
