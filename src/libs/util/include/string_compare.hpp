#pragma once

#include "istring.hpp"

#include <algorithm>
#include <cctype>
#include <string>

namespace storm
{
namespace detail
{
struct is_iequal
{
    template <typename T1, typename T2 = T1> bool operator()(const T1 &first, const T2 &second) const
    {
        return std::toupper(first) == std::toupper(second);
    }
};

} // namespace detail

template <typename Range1T, typename Range2T = Range1T>
bool iEquals(const Range1T &first, const Range2T &second, const size_t count)
{
    detail::is_iequal comp;

    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    const auto first_begin = std::begin(first_normalized);
    const auto second_begin = std::begin(second_normalized);

    const auto first_end = std::end(first_normalized);
    const auto second_end = std::end(second_normalized);

    const auto first_length = std::distance(first_begin, first_end);
    const auto second_length = std::distance(second_begin, second_end);
    if (first_length < count || second_length < count)
    {
        if (first_length != second_length)
        {
            return false;
        }
        else
        {
            return std::equal(first_begin, first_begin + first_length, second_begin, second_begin + second_length,
                              comp);
        }
    }
    else
    {
        return std::equal(first_begin, first_begin + count, second_begin, second_begin + count, comp);
    }
}

template <typename Range1T, typename Range2T = Range1T> bool iEquals(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return traits_cast<ichar_traits<char>>(first_normalized) == traits_cast<ichar_traits<char>>(second_normalized);
}

template <typename Range1T, typename Range2T = Range1T> bool iLess(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return traits_cast<ichar_traits<char>>(first_normalized) < traits_cast<ichar_traits<char>>(second_normalized);
}

template <typename Range1T, typename Range2T = Range1T> bool iLessOrEqual(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return traits_cast<ichar_traits<char>>(first_normalized) <= traits_cast<ichar_traits<char>>(second_normalized);
}

template <typename Range1T, typename Range2T = Range1T> bool iGreater(const Range1T &first, const Range2T &second)
{
    return !iLessOrEqual(first, second);
}

template <typename Range1T, typename Range2T = Range1T> bool iGreaterOrEqual(const Range1T &first, const Range2T &second)
{
    return !iLess(first, second);
}


// The wildcmp function was taken from http://www.codeproject.com/KB/string/wildcmp.aspx; the
// wildicmp (case insensitive wildcard comparison) was based on it.

inline int wildcmp(const char *wild, const char *string)
{
    // Written by Jack Handy - jakkhandy@hotmail.com

    const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            if (cp != nullptr)
            {
                string = cp++;
            }
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

inline int wildicmp(const char *wild, const char *string)
{
    const char *cp = nullptr, *mp = nullptr;

    while ((*string) && (*wild != '*'))
    {
        if ((tolower(*wild) != tolower(*string)) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ((tolower(*wild) == tolower(*string)) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            if (cp != nullptr)
            {
                string = cp++;
            }
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

inline int wildicmp(const char *wild, const char8_t *string)
{
    // TODO: implement for UTF8!
    return wildicmp(wild, reinterpret_cast<const char *>(string));
}

class iStrHasher
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

struct iStrComparator
{
    bool operator()(const std::string &left, const std::string &right) const
    {
        return iEquals(left, right);
    }
};
} // namespace storm
