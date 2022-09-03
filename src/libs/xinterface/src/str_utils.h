#pragma once
#include <string>

namespace TOREMOVE
{

inline void trim(std::string &str)
{
    while (true)
    {
        const auto it = str.begin();
        if (it == str.end() || *it != '\t' && *it != ' ')
            break;
        str.erase(it);
    }
}

// trim right
inline void rtrim(std::string &str)
{
    while (true)
    {
        const auto it = str.rbegin();
        if (it == str.rend() || *it != '\t' && *it != ' ')
            break;
        str.erase(std::next(it).base());
    }
}

} // namespace TOREMOVE
