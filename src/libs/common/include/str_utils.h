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

inline bool starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

inline bool ends_with(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}


} // namespace TOREMOVE
