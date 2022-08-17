#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "storm_assert.h"

class VSTRING_CODEC
{
  public:
    VSTRING_CODEC() = default;
    virtual ~VSTRING_CODEC() = default;

    virtual uint32_t GetNum() = 0;
    virtual uint32_t Convert(const char *pString) = 0;
    virtual uint32_t Convert(const char *pString, int32_t iLen) = 0;
    virtual const char *Convert(uint32_t code) = 0;

    virtual void VariableChanged() = 0;
};

class ATTRIBUTES final
{
    // TODO: remove with another iteration of rewriting this
    friend class COMPILER;

    class LegacyProxy;

  public:
    [[deprecated("Pass StringCodec by reference instead")]] explicit ATTRIBUTES(VSTRING_CODEC *p);

    explicit ATTRIBUTES(VSTRING_CODEC &p);
    ATTRIBUTES(const ATTRIBUTES &) = delete;
    ATTRIBUTES(ATTRIBUTES &&other) noexcept;
    ATTRIBUTES &operator=(const ATTRIBUTES &) = delete;
    ATTRIBUTES &operator=(ATTRIBUTES &&other) noexcept;
    ~ATTRIBUTES();

    void SetBreak(bool set_break);
    [[nodiscard]] ATTRIBUTES* GetParent() const;
    bool operator==(const char *str) const;
    [[nodiscard]] const char* GetThisName() const;
    [[nodiscard]] bool HasValue() const noexcept;
    [[nodiscard]] const std::string &GetValue() const;
    [[nodiscard]] LegacyProxy GetThisAttr() const;
    void SetName(const std::string_view &new_name);
    [[deprecated("Pass attribute value by string_view instead")]]
    void SetValue(const char *new_value);
    void SetValue(const std::string_view &new_value);
    [[nodiscard]] size_t GetAttributesNum() const;
    [[nodiscard]] ATTRIBUTES *GetAttributeClass(const std::string_view &name) const;
    [[nodiscard]] ATTRIBUTES *GetAttributeClass(uint32_t n) const;
    [[nodiscard]] ATTRIBUTES *VerifyAttributeClass(const std::string_view &name);
    [[nodiscard]] LegacyProxy GetAttribute(size_t n) const;
    [[nodiscard]] LegacyProxy GetAttribute(const std::string_view &name) const;
    [[nodiscard]] const char *GetAttributeName(size_t n) const;
    [[nodiscard]] uint32_t GetAttributeAsDword(const char *name = nullptr, uint32_t def = 0) const;
    [[nodiscard]] uintptr_t GetAttributeAsPointer(const char *name = nullptr, uintptr_t def = 0) const;
    [[nodiscard]] float GetAttributeAsFloat(const char *name = nullptr, float def = 0) const;
    bool SetAttributeUseDword(const char *name, uint32_t val);
    bool SetAttributeUseFloat(const char *name, float val);
    ATTRIBUTES &CreateAttribute(const std::string_view &name);
    ATTRIBUTES *CreateAttribute(const std::string_view &name, const char *attribute);
    [[deprecated("Pass attribute value by string_view instead")]]
    size_t SetAttribute(const std::string_view &name, const char *attribute);
    size_t SetAttribute(const std::string_view &name, const std::string_view &attribute);
    [[nodiscard]] ATTRIBUTES Copy() const;
    bool DeleteAttributeClassX(ATTRIBUTES *pA);
    ATTRIBUTES *CreateSubAClass(ATTRIBUTES *pRoot, const char *access_string);
    [[nodiscard]] ATTRIBUTES *FindAClass(ATTRIBUTES *pRoot, const char *access_string);
    [[nodiscard]] ATTRIBUTES *GetAttributeClassByCode(uint32_t name_code) const;
    [[nodiscard]] ATTRIBUTES *VerifyAttributeClassByCode(uint32_t name_code);
    ATTRIBUTES *CreateAttribute(uint32_t name_code, const char *attribute);
    [[deprecated("Pass attribute value by string_view instead")]]
    size_t SetAttribute(uint32_t name_code, const char *attribute);
    size_t SetAttribute(uint32_t name_code, const std::string_view &attribute);
    [[nodiscard]] uint32_t GetThisNameCode() const noexcept;
    void SetNameCode(uint32_t n) noexcept;
    [[nodiscard]] VSTRING_CODEC &GetStringCodec() const noexcept;

private:
    ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, const std::string_view &name);
    ATTRIBUTES(VSTRING_CODEC &string_codec, ATTRIBUTES *parent, uint32_t name_code);

    void Release() const noexcept;
    ATTRIBUTES *CreateNewAttribute(uint32_t name_code);

    VSTRING_CODEC &stringCodec_;
    uint32_t nameCode_{};
    std::optional<std::string> value_;
    std::vector<std::unique_ptr<ATTRIBUTES>> attributes_;
    ATTRIBUTES *parent_{nullptr};
    bool break_{false};
    
    class LegacyProxy
    {
        using value_t = decltype(value_);
        using proxy_value_t = std::add_lvalue_reference_t<std::add_const_t<value_t>>;

      public:
        LegacyProxy(const LegacyProxy &other) = delete;
        LegacyProxy(LegacyProxy &&other) = delete;
        LegacyProxy & operator=(const LegacyProxy &other) = delete;
        LegacyProxy & operator=(LegacyProxy &&other) = delete;

        LegacyProxy() : proxy_value_(null_value)
        {            
        }

        LegacyProxy(proxy_value_t value) : proxy_value_(value)
        {
        }

        // use to_string directly when the implicit cast is ambiguous
        operator std::string() const &&
        {
            return proxy_value_ ? *proxy_value_ : value_t::value_type{};
        }

        operator const char *() const &&
        {
            return proxy_value_ ? proxy_value_->c_str() : nullptr;
        }

        friend std::string to_string(LegacyProxy &&proxy)
        {
            return std::move(proxy).operator std::string();
        }

      private:
        static inline value_t null_value;

        proxy_value_t proxy_value_;
    };
};

