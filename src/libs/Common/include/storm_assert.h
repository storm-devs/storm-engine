#pragma once
#include <spdlog/spdlog.h>

#define EX_OFF

inline void __Storm_Assert__(bool expression, const char *file, long line, const char *str)
{
    if (!expression)
    {
        if (str)
        {
            spdlog::critical("Assert failed in {} line {}, expression string {}", file, line, str);
        }
        else
        {
            spdlog::critical("Assert failed in {} line {}", file, line);
        }
#ifdef EX_OFF
        __debugbreak();
#else
        throw std::runtime_error("assert");
#endif
    }
}

#ifdef _DEBUG
#define Assert(exp) __Storm_Assert__(exp, __FILE__, __LINE__, #exp)
#else
#define Assert(exp)
#endif
