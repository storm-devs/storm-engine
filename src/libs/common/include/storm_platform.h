#ifndef _WIN32

#include "safe_str_lib.h"
// use custom sprintf_s instead of safeclib
#undef sprintf_s

template <size_t size> inline int sprintf_s(char (&buffer)[size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

inline int sprintf_s(char *buffer, size_t size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return result;
}

#endif
