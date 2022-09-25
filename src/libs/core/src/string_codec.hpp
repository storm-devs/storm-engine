#pragma once

#include "attributes.h"
#include <platform/platform.hpp>
#include <string_compare.hpp>

struct HTSUBELEMENT
{
    std::string string{};
    uint32_t hashCode{};
};

struct HTELEMENT
{
    std::vector<HTSUBELEMENT> elements;
};

class STRING_CODEC final : public VSTRING_CODEC
{
  public:
    STRING_CODEC();
    ~STRING_CODEC() noexcept override;

    [[nodiscard]] uint32_t GetNum() const override;
    uint32_t Convert(const std::string_view &pString) override;
    [[nodiscard]] const char *Convert(uint32_t code) const override;

    void VariableChanged() override;

    // For iteration
    const char *Get();
    const char *GetNext();

  private:
    std::vector<HTELEMENT> hashTable_{};

    uint32_t totalStringCount_ = 0;

    // For iteration
    uint32_t tableIndex_ = 0;
    uint32_t tableElementIndex_ = 0;
};
