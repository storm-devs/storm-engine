#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace storm::dialog {

using GetStringWidthFunction = std::function<int32_t (const std::string_view &text)>;

std::vector<int32_t> SplitIntoPages(const size_t line_count, const int32_t page_height, const std::vector<int32_t> &forced_breaks = {});

void AddToStringArrayLimitedByWidth(const std::string_view &text, int32_t nLimitWidth,
                                    std::vector<std::string> &asOutTextList, const GetStringWidthFunction &get_string_width);

} // namespace storm::dialog
