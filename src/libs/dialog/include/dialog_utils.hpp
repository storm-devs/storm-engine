#pragma once

#include <cstdint>
#include <vector>

namespace storm::dialog {

std::vector<int32_t> SplitIntoPages(const size_t line_count, const int32_t page_height, const std::vector<int32_t> &forced_breaks);

} // namespace storm::dialog
