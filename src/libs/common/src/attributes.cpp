#include "attributes.h"

ATTRIBUTES::ATTRIBUTES(VSTRING_CODEC &p) : ATTRIBUTES(p, nullptr, "root")
{
}

ATTRIBUTES::ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, const std::string_view &name)
    : ATTRIBUTES(string_codec, parent, string_codec.Convert(name.data()))
{
}

ATTRIBUTES::ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, const uint32_t name_code)
    : stringCodec_(string_codec), nameCode_(name_code), parent_(parent)
{
}

ATTRIBUTES ATTRIBUTES::Copy() const
{
    ATTRIBUTES result(stringCodec_, nullptr, nameCode_);
    result.value_ = value_;

    for (const auto &attribute : attributes_)
    {
        auto &new_child = result.CreateAttribute(attribute->GetThisName());
        new_child = attribute->Copy();
    }

    return result;
}

void ATTRIBUTES::Release() noexcept
{
    if (break_)
        stringCodec_.VariableChanged();
}
