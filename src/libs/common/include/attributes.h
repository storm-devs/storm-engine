#pragma once

#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "storm_assert.h"
#include "storm/string_compare.hpp"

class VSTRING_CODEC
{
  public:
    VSTRING_CODEC() = default;

    virtual ~VSTRING_CODEC() noexcept = default;
    virtual uint32_t GetNum() = 0;
    virtual uint32_t Convert(const char *pString) = 0;
    virtual uint32_t Convert(const char *pString, int32_t iLen) = 0;
    virtual const char *Convert(uint32_t code) = 0;

    virtual void VariableChanged() = 0;
};

// here for now
constexpr size_t TSE_MAX_EVENT_LENGTH = 64;

#define INVALID_ATTRIBUTE_HANDLE 0xffffffff

//--------------------------------------------------------------------------------------------------------------
// Each attribute is a class with Name and Attribute string values. Class can contain list of ... later
//
// GetAttributeClass(attribute_name) - return class ATTRIBUTES pointer for specified attribute
// GetAttribute(attribute_name) - return string value of specified attribute
// SetAttribute(attribute_name,attribute) - create or modify attribute in attributes set
// GetAttributeAsDword(attribute_name, DWORD def = 0) - return attribute value, converted from string to uint32_t
// GetAttributeAsFloat(attribute_name, DWORD def = 0) - return attribute value, converted from string to float
// SetAttributeUseDword(attribute_name, DWORD val) - create or modify attribute, converted from uint32_t to string
// SetAttributeUseFloat(attribute_name, FLOAT val) - create or modify attribute, converted from float to string

class ATTRIBUTES final
{
    // TODO: remove with another iteration of rewriting this
    friend class COMPILER;

    VSTRING_CODEC &stringCodec_;
    uint32_t nameCode_{};
    std::optional<std::string> value_;
    std::vector<std::unique_ptr<ATTRIBUTES>> attributes_;
    ATTRIBUTES *parent_ = nullptr;
    bool break_ = false;

    ATTRIBUTES *CreateNewAttribute(uint32_t name_code)
    {
        std::unique_ptr<ATTRIBUTES> &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name_code));
        return attr.get();
    }

  public:
    [[deprecated("Pass StringCodec by reference instead")]] explicit ATTRIBUTES(VSTRING_CODEC *p) : ATTRIBUTES(*p)
    {
    }

    explicit ATTRIBUTES(VSTRING_CODEC &p);

    ATTRIBUTES(const ATTRIBUTES &) = delete;
    ATTRIBUTES &operator=(const ATTRIBUTES &) = delete;

    ATTRIBUTES(ATTRIBUTES &&other)
    noexcept
        : stringCodec_(other.stringCodec_), nameCode_(other.nameCode_), value_(std::move(other.value_)),
          attributes_(std::move(other.attributes_)), parent_(nullptr), break_(other.break_)
    {
    }

    ATTRIBUTES &operator=(ATTRIBUTES &&other) noexcept
    {
        stringCodec_ = other.stringCodec_;
        nameCode_ = other.nameCode_;
        value_ = std::move(other.value_);
        attributes_ = std::move(other.attributes_);
        // Do not update parent
        // parent_ = other.parent_;
        break_ = other.break_;
        return *this;
    }

    ~ATTRIBUTES() noexcept
    {
        Release();
    }

    void SetBreak(bool set_break)
    {
        this->break_ = set_break;
    }

    [[nodiscard]] auto GetParent() const
    {
        return parent_;
    }

    bool operator==(const char *str) const
    {
        if (!str || !str[0])
            return false;
        return storm::iEquals(stringCodec_.Convert(nameCode_), str);
    }

    [[nodiscard]] auto GetThisName() const
    {
        return stringCodec_.Convert(nameCode_);
    }

    [[nodiscard]] bool HasValue() const noexcept {
        return value_.has_value();
    }

    [[nodiscard]] const std::string &GetValue() const
    {
        return *value_;
    }

    [[nodiscard]] const char *GetThisAttr() const
    {
        return value_ ? value_->c_str() : nullptr;
    }

    void SetName(const std::string_view &new_name)
    {
        nameCode_ = stringCodec_.Convert(new_name.data());
    }

    [[deprecated("Pass attribute value by string_view instead")]]
    void SetValue(const char *new_value)
    {
        if (new_value == nullptr)
        {
            value_.reset();
        }
        else {
            value_ = new_value;
        }

        if (break_)
            stringCodec_.VariableChanged();
    }

    void SetValue(const std::string_view &new_value)
    {
        value_ = new_value;

        if (break_)
            stringCodec_.VariableChanged();
    }

    [[nodiscard]] auto GetAttributesNum() const
    {
        return attributes_.size();
    }

    [[nodiscard]] ATTRIBUTES *GetAttributeClass(const std::string_view &name)
    {
        for (const auto &attribute : attributes_)
            if (storm::iEquals(name, attribute->GetThisName()))
                return attribute.get();
        return nullptr;
    }

    [[nodiscard]] ATTRIBUTES *GetAttributeClass(uint32_t n)
    {
        return n >= attributes_.size() ? nullptr : attributes_[n].get();
    }

    [[nodiscard]] auto VerifyAttributeClass(const std::string_view &name)
    {
        auto *const pTemp = GetAttributeClass(name);
        return (pTemp) ? pTemp : CreateAttribute(name, "");
    }

    [[nodiscard]] const char *GetAttribute(size_t n)
    {
        if (n < attributes_.size() && attributes_[n]->value_) {
            return attributes_[n]->value_->c_str();
        }
        else {
            return nullptr;
        }
    }

    [[nodiscard]] const char *GetAttributeName(size_t n)
    {
        if (n < attributes_.size()) {
            return attributes_[n]->GetThisName();
        }
        else {
            return nullptr;
        }
    }

    [[nodiscard]] const char *GetAttribute(const std::string_view &name)
    {
        for (const auto &attribute : attributes_)
            if (storm::iEquals(name, attribute->GetThisName())) {
                if (attribute->value_) {
                    return attribute->value_->c_str();
                }
                else {
                    return nullptr;
                }
            }
        return nullptr;
    }

    [[nodiscard]] auto GetAttributeAsDword(const char *name = nullptr, uint32_t def = 0)
    {
        uint32_t vDword;
        const char *pAttribute;
        vDword = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                vDword = atol(pAttribute);
        }
        else
        {
            vDword = atol( value_->c_str());
        }
        return vDword;
    }

    [[nodiscard]] auto GetAttributeAsPointer(const char *name = nullptr, uintptr_t def = 0)
    {
        uintptr_t ptr;
        const char *pAttribute;
        ptr = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                ptr = atoll(pAttribute);
        }
        else
        {
            ptr = atoll(value_->c_str());
        }
        return ptr;
    }

    [[nodiscard]] auto GetAttributeAsFloat(const char *name = nullptr, float def = 0)
    {
        float vFloat;
        const char *pAttribute;
        vFloat = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                vFloat = static_cast<float>(atof(pAttribute));
        }
        else
        {
            vFloat = static_cast<float>(atof(value_->c_str()));
        }
        return vFloat;
    }

    auto SetAttributeUseDword(const char *name, uint32_t val)
    {
        std::string buffer;
        buffer = std::to_string(val);
        if (name)
            return SetAttribute(name, buffer.c_str()) != 0;
        SetValue(buffer.c_str());
        return true;
    }

    auto SetAttributeUseFloat(const char *name, float val)
    {
        char buffer[128];
        sprintf_s(buffer, "%g", val);
        if (name)
            return SetAttribute(name, buffer) != 0;
        SetValue(buffer);

        return true;
    }

    ATTRIBUTES &CreateAttribute(const std::string_view &name)
    {
        auto &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name));
        return *attr;
    }

    ATTRIBUTES *CreateAttribute(const std::string_view &name, const char *attribute)
    {
        auto &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name));

        if (attribute)
        {
            attr->value_ = attribute;
        }

        return attr.get();
    }

    [[deprecated("Pass attribute value by string_view instead")]]
    size_t SetAttribute(const std::string_view &name, const char *attribute)
    {
        return SetAttribute(stringCodec_.Convert(name.data()), attribute);
    }

    size_t SetAttribute(const std::string_view &name, const std::string_view &attribute)
    {
        return SetAttribute(stringCodec_.Convert(name.data()), attribute);
    }

    [[nodiscard]] ATTRIBUTES Copy() const;

    bool DeleteAttributeClassX(ATTRIBUTES *pA)
    {
        if (pA == nullptr)
            return false;
        if (pA == this)
        {
            attributes_.clear();
        }
        else
        {
            //            auto removed_it = std::remove_if(pAttributes.begin(), pAttributes.end(), [&](const auto&
            //            item){
            //                return item.get() == pA;
            //            });
            //            return removed_it != pAttributes.end();
            for (uint32_t n = 0; n < attributes_.size(); n++)
            {
                if (attributes_[n].get() == pA)
                {
                    for (auto i = n; i < attributes_.size() - 1; i++)
                        attributes_[i] = std::move(attributes_[i + 1]);

                    attributes_.pop_back();
                    return true;
                }
                if (attributes_[n]->DeleteAttributeClassX(pA))
                    return true;
            }
        }
        return false;
    }

    ATTRIBUTES *CreateSubAClass(ATTRIBUTES *pRoot, const char *access_string)
    {
        uint32_t dwNameCode;
        uint32_t n = 0;
        ATTRIBUTES *pResult = nullptr;
        ATTRIBUTES *pTemp;
        if (pRoot == nullptr)
            return nullptr;
        if (access_string == nullptr)
            return nullptr;

        while (true)
        {
            switch (access_string[n])
            {
            case '.':
                dwNameCode = stringCodec_.Convert(access_string, n);
                pTemp = pRoot->GetAttributeClassByCode(dwNameCode);
                if (!pTemp)
                    pTemp = pRoot->CreateNewAttribute(dwNameCode);
                pResult = CreateSubAClass(pTemp, &access_string[n + 1]);
                return pResult;

            case 0:
                dwNameCode = stringCodec_.Convert(access_string);
                pResult = pRoot->GetAttributeClassByCode(dwNameCode);
                return (pResult) ? pResult : pRoot->CreateNewAttribute(dwNameCode);
            }
            n++;
        }
        return nullptr;
    }

    [[nodiscard]] ATTRIBUTES *FindAClass(ATTRIBUTES *pRoot, const char *access_string)
    {
        uint32_t n = 0;
        ATTRIBUTES *pResult = nullptr;
        ATTRIBUTES *pTemp = nullptr;

        if (!pRoot || !access_string)
            return nullptr;
        if (!access_string[0])
            return pRoot;

        while (true)
        {
            switch (access_string[n])
            {
            case '.':
                pTemp = pRoot->GetAttributeClassByCode(stringCodec_.Convert(access_string, n));
                if (!pTemp)
                    return nullptr;
                pResult = FindAClass(pTemp, &access_string[n + 1]);
                return pResult;
            case 0:
                pResult = pRoot->GetAttributeClassByCode(stringCodec_.Convert(access_string));
                return pResult;
            }
            n++;
        }
        return nullptr;
    }

    [[nodiscard]] ATTRIBUTES *GetAttributeClassByCode(uint32_t name_code)
    {
        for (const auto &attribute : attributes_)
            if (name_code == attribute->nameCode_)
                return attribute.get();
        return nullptr;
    }

    [[nodiscard]] ATTRIBUTES *VerifyAttributeClassByCode(uint32_t name_code)
    {
        ATTRIBUTES *pTemp;
        pTemp = GetAttributeClassByCode(name_code);
        if (pTemp)
            return pTemp;
        return CreateAttribute(name_code, "");
    }

    ATTRIBUTES *CreateAttribute(uint32_t name_code, const char *attribute)
    {
        auto &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name_code));

        if (attribute)
        {
            attr->value_ = attribute;
        }

        return attr.get();
    }

    [[deprecated("Pass attribute value by string_view instead")]]
    size_t SetAttribute(uint32_t name_code, const char *attribute)
    {
        size_t n;
        for (n = 0; n < attributes_.size(); n++)
        {
            if (attributes_[n]->nameCode_ == name_code)
            {
                if (attribute)
                {
                    attributes_[n]->value_ = attribute;
                }
                else
                {
                    attributes_[n]->value_.reset();
                }
                return n;
            }
        }

        auto &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name_code));

        if (attribute)
        {
            attributes_[n]->value_ = attribute;
        }
        else {
            attributes_[n]->value_.reset();
        }

        return attributes_.size() - 1;
    }

    size_t SetAttribute(uint32_t name_code, const std::string_view &attribute)
    {
        size_t n;
        for (n = 0; n < attributes_.size(); n++)
        {
            if (attributes_[n]->nameCode_ == name_code)
            {
                attributes_[n]->value_ = attribute;
                return n;
            }
        }

        auto &attr = attributes_.emplace_back(new ATTRIBUTES(stringCodec_, this, name_code));

        attributes_[n]->value_ = attribute;

        return attributes_.size() - 1;
    }

    [[nodiscard]] uint32_t GetThisNameCode() const noexcept
    {
        return nameCode_;
    }

    void SetNameCode(uint32_t n) noexcept
    {
        nameCode_ = n;
    }

    [[nodiscard]] VSTRING_CODEC &GetStringCodec() const noexcept
    {
        return stringCodec_;
    }

  private:
    ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, const std::string_view &name);
    ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, uint32_t name_code);

    void Release() noexcept;
};
