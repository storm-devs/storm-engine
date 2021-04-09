#pragma once

#include <algorithm>
#include <cctype>

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

struct is_iless
{
    template <typename T1, typename T2 = T1> bool operator()(const T1 &first, const T2 &second) const
    {
        return std::toupper(first) < std::toupper(second);
    }
};

} // namespace detail

template <typename Range1T, typename Range2T = Range1T> inline bool iEquals(const Range1T &first, const Range2T &second)
{
    detail::is_iequal comp;

    const auto end_input = std::end(first);
    const auto end_test = std::end(second);

    auto it_input = std::begin(first);
    auto it_test = std::begin(second);

    for (; it_input != end_input && it_test != end_test; ++it_input, ++it_test)
    {
        if (!comp(*it_input, *it_test))
        {
            return false;
        }
    }

    return it_input == end_input && it_test == end_test;
}

template <typename Range1T, typename Range2T = Range1T> inline bool iLess(const Range1T &first, const Range2T &second)
{
    return std::lexicographical_compare(std::begin(first), std::end(first), std::begin(second), std::end(second),
                                        detail::is_iless{});
}

// The wildcmp function was taken from http://www.codeproject.com/KB/string/wildcmp.aspx; the
// wildicmp (case insensitive wildcard comparison) was based on it.

int wildcmp(const char *wild, const char *string)
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
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

int wildicmp(const char *wild, const char *string)
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
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

} // namespace storm
