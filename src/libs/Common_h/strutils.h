#pragma once
#include <algorithm>
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

class CaseInsensitiveStringHasher
{
  public:
    size_t operator()(const std::string &key) const
    {
        std::string lower_copy = key;
        std::transform(lower_copy.begin(), lower_copy.end(), lower_copy.begin(), ::tolower);
        return inner_hasher_(lower_copy);
    }

  private:
    std::hash<std::string> inner_hasher_;
};

class CaseInsensitiveStringComparator
{
  public:
    bool operator()(const std::string &left, const std::string &right) const
    {
        return std::equal(left.begin(), left.end(), right.begin(), right.end(), CompareChars);
    }

  private:
    static bool CompareChars(char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    }
};
