#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include <Windows.h>

namespace utf8
{
struct u8_char
{
    u8_char() : l(0), c(0)
    {
    }

    u8_char(char ch) : l(sizeof(ch)), c(ch)
    {
    }

    int l;
    union {
        uint32_t c;
        char b[4];
    };
};

inline std::string ConvertWideToUtf8(const std::wstring &wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

inline std::wstring ConvertUtf8ToWide(const std::string &str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}

/* is c the start of a utf8 sequence? */
#define isutf(c) (((c)&0xC0) != 0x80)

// taken from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
inline int CodepointToUtf8(char *out, uint32_t codepoint)
{
    if (codepoint <= 0x7F)
    {
        // Plain ASCII
        out[0] = (char)codepoint;
        out[1] = 0;
        return 1;
    }
    else if (codepoint <= 0x07FF)
    {
        // 2-byte unicode
        out[0] = (char)(((codepoint >> 6) & 0x1F) | 0xC0);
        out[1] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        // 3-byte unicode
        out[0] = (char)(((codepoint >> 12) & 0x0F) | 0xE0);
        out[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        out[2] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        // 4-byte unicode
        out[0] = (char)(((codepoint >> 18) & 0x07) | 0xF0);
        out[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        out[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        out[3] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else
    {
        // error - use replacement character
        out[0] = (char)0xEF;
        out[1] = (char)0xBF;
        out[2] = (char)0xBD;
        out[3] = 0;
        return 0;
    }
}

// taken from http://www.zedwood.com/article/cpp-utf8-char-to-codepoint
inline int Utf8ToCodepoint(const char *utf8)
{
    int l = strlen(utf8);

    if (l < 1)
        return -1;
    unsigned char u0 = utf8[0];
    if (u0 >= 0 && u0 <= 0x7F)
        return u0;

    if (l < 2)
        return -1;
    unsigned char u1 = utf8[1];
    if (u0 >= 0xC0 && u0 <= 0xDF)
        return (u0 - 192) * 64 + (u1 - 128);

    if (utf8[0] == 0xed && (utf8[1] & 0xa0) == 0xa0)
        return -1; // code points, 0xd800 to 0xdfff

    if (l < 3)
        return -1;
    unsigned char u2 = utf8[2];
    if (u0 >= 224 && u0 <= 239)
        return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);

    if (l < 4)
        return -1;
    unsigned char u3 = utf8[3];
    if (u0 >= 240 && u0 <= 247)
        return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
    return -1;
}

inline int Utf8StringLength(const char *s)
{
    long s_num = 0;
    while (*s)
        s_num += (*s++ & 0xC0) != 0x80;

    return s_num;
}

// taken from https://www.cprogramming.com/tutorial/utf8.c
inline int u8_inc(const char *s)
{
    int i = 1;
    while (s[i] && (s[i] & 0xC0) == 0x80)
    {
        i++;
    }
    return i;

    // int i = 0;
    // uint8_t c0 = s[i++];
    // if (c0 >)
    // uint8_t c1 = s[i++];
    //
    // return i;
}

inline int u8_dec(const char *s)
{
    int i = 1;
    while (s[-i] && (s[-i] & 0xC0) == 0x80)
    {
        i++;
    }
    return i;
}

/* charnum => byte offset */
inline int u8_offset(const char *str, int charnum)
{
    int offs = 0;

    while (charnum > 0 && str[offs])
    {
        (void)(isutf(str[++offs]) || isutf(str[++offs]) || isutf(str[++offs]) || ++offs);
        charnum--;
    }
    return offs;
}
} // namespace utf8