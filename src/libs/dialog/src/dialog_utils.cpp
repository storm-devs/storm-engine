#include "dialog_utils.hpp"

namespace storm::dialog {

std::vector<int32_t> SplitIntoPages(const size_t line_count, const int32_t page_height, const std::vector<int32_t> &forced_breaks)
{
    std::vector<int32_t> page_ends;
    int32_t current_page_break = 0;
    for (int32_t forced_break : forced_breaks)
    {
        if (forced_break > line_count) {
            break;
        }

        while (forced_break > current_page_break + page_height)
        {
            current_page_break += page_height;
            page_ends.push_back(current_page_break);
        }

        current_page_break = forced_break;
        page_ends.push_back(current_page_break);
    }
    while (current_page_break + page_height < line_count) {
        current_page_break += page_height;
        page_ends.push_back(current_page_break);
    }
    return page_ends;
}

} // namespace storm::dialog