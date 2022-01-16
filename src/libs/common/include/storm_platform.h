#ifndef _WIN32

#include "safe_str_lib.h"

// use inline wrapper for strcat_s from safeclib instead of #define strcat_s
#undef strcat_s
inline errno_t strcat_s(char *restrict dest, rsize_t size, const char *restrict src)
{
    return _strcat_s_chk(dest, size, src, BOS(dest));
}

template <rsize_t size> inline errno_t strcat_s(char (&dest)[size], const char *src)
{
    return strcat_s(dest, size, src);
}

// use inline wrapper for strcpy_s from safeclib instead of #define strcpy_s
#undef strcpy_s
inline errno_t strcpy_s(char *restrict dest, rsize_t size, const char *restrict src)
{
    return _strcpy_s_chk(dest, size, src, BOS(dest));
}

template <rsize_t size> inline errno_t strcpy_s(char (&dest)[size], const char *src)
{
    return strcpy_s(dest, size, src);
}

// use inline wrapper for strncpy_s from safeclib instead of #define strncpy_s
#undef strncpy_s
inline errno_t strncpy_s(char *restrict dest, rsize_t size, const char *restrict src, rsize_t count)
{
    return _strncpy_s_chk(dest, size, src, count, BOS(dest), BOS(src));
}

template <rsize_t size> inline errno_t strncpy_s(char (&dest)[size], const char *src, rsize_t count)
{
    return strncpy_s(dest, size, src, count);
}

// use custom sprintf_s instead of safeclib: #define sprintf_s(dest, dmax, ...)
#undef sprintf_s
template <rsize_t size> inline int sprintf_s(char (&buffer)[size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

inline int sprintf_s(char *buffer, rsize_t size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return result;
}

#endif