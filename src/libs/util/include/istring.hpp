#pragma once

#include <string>

namespace storm
{

template <typename T> struct ichar_traits : public std::char_traits<T>
{
    static bool eq(T c1, T c2)
    {
        return std::toupper(c1) == std::toupper(c2);
    }

    static bool lt(T c1, T c2)
    {
        return std::toupper(c1) < std::toupper(c2);
    }

    static int compare(const T *s1, const T *s2, size_t n)
    {
        while (n-- != 0)
        {
            if (std::toupper(*s1) < std::toupper(*s2))
                return -1;
            if (std::toupper(*s1) > std::toupper(*s2))
                return 1;
            ++s1;
            ++s2;
        }
        return 0;
    }

    static const T *find(const T *source, int n, const T &target)
    {
        const auto uppercase_target = std::toupper(target);
        while (n-- > 0 && std::toupper(*source) != uppercase_target)
        {
            ++source;
        }
        return source;
    }
};

using istring = std::basic_string<char, ichar_traits<char>>;
using istring_view = std::basic_string_view<char, ichar_traits<char>>;

template <class DstTraits, class CharT, class SrcTraits>
constexpr std::basic_string_view<CharT, DstTraits> traits_cast(
    const std::basic_string_view<CharT, SrcTraits> &src) noexcept
{
    return {src.data(), src.size()};
}

template <class DstTraits, class CharT, class SrcTraits>
constexpr std::basic_string<CharT, DstTraits> traits_cast(
    const std::basic_string<CharT, SrcTraits> &src) noexcept
{
    return {src.data(), src.size()};
}

} // namespace storm
