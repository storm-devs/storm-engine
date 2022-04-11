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
    result.SetValue(value_);

    for (const auto &attribute : attributes_)
    {
        const auto i = result.SetAttribute(attribute->GetThisName(), attribute->value_);
        result.attributes_[i] = std::make_unique<ATTRIBUTES>(attribute->Copy());
    }

    return result;
}

void ATTRIBUTES::Release() noexcept
{
    if (break_)
        stringCodec_.VariableChanged();
    delete[] value_;
    value_ = nullptr;
}

void ATTRIBUTES::ReleaseLeafs()
{
    attributes_.clear();
}
