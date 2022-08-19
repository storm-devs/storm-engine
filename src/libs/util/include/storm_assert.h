#pragma once

#ifdef EX_OFF
#include "debug-trap.h"
#else
#include <stdexcept>
#include <fmt/format.h>
#endif

inline void __Storm_Assert__(bool expression, const char *file, int32_t line, const char *str)
{
    if (!expression)
    {
#ifdef EX_OFF
        psnip_trap();
#else
        throw std::runtime_error(fmt::format("Assert failed in {} line {}, expression string {}", file, line, str ? str : ""));
#endif
    }
}

#ifdef _DEBUG
#define Assert(exp) __Storm_Assert__(exp, __FILE__, __LINE__, #exp)
#else
#define Assert(exp)
#endif
