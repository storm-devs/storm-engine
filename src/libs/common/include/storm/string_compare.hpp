#ifndef STORM_ENGINE_STRING_COMPARE_HPP
#define STORM_ENGINE_STRING_COMPARE_HPP
#pragma once

#include <algorithm>
#include <cctype>

namespace storm
{

namespace detail
{

struct is_iequal
{
    template <typename T1, typename T2 = T1>
    bool operator()(const T1 &first, const T2 &second) const
    {
        return std::toupper(first) == std::toupper(second);
    }
};

struct is_iless
{
    template <typename T1, typename T2 = T1>
    bool operator()(const T1 &Arg1, const T2 &Arg2) const
    {
        return std::toupper(Arg1) < std::toupper(Arg2);
    }
};

} // namespace detail

template <typename Range1T, typename Range2T = Range1T>
inline bool iEquals(const Range1T &first, const Range2T &second)
{
    detail::is_iequal comp;

    const auto end_input = end(first);
    const auto end_test = end(second);

    auto it_input = begin(first);
    auto it_test = begin(second);

    for (; it_input != end_input && it_test != end_test; ++it_input, ++it_test)
    {
        if (!comp(*it_input, *it_test))
        {
            return false;
        }
    }

    return it_input == end_input && it_test == end_test;
}

template <typename Range1T, typename Range2T = Range1T>
inline bool iLess(const Range1T &first, const Range2T &second)
{
    return std::lexicographical_compare(begin(first), end(first), begin(second), end(second), detail::is_iless{});
}

} // namespace storm

#endif // STORM_ENGINE_STRING_COMPARE_HPP
