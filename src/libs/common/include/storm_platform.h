#ifndef _WIN32

#include "safe_str_lib.h"

// use custom strcpy_s instead of safeclib: #define strcpy_s(dest, dmax, src)
#undef strcpy_s

inline int strcpy_s(char *dest, size_t size, const char *src)
{
    if (!dest)
        return EINVAL;

    if (0 == size)
    {
        dest[0] = '\0';
        return ERANGE;
    }

    if (!src)
    {
        dest[0] = '\0';
        return EINVAL;
    }

    size_t i;
    for (i = 0; i < size; i++)
    {
        if ((dest[i] = src[i]) == '\0')
            return 0;
    }
    dest[0] = '\0';
    return ERANGE;
}

template <std::size_t size> inline int strcpy_s(char (&dest)[size], const char *src)
{
    return strcpy_s(dest, size, src);
}

// use custom sprintf_s instead of safeclib: #define sprintf_s(dest, dmax, ...)
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
