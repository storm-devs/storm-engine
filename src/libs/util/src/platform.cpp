#ifdef _WIN32
const char *convert_path_sep(const char *cPath)
{
    return cPath;
}
#else
#include "platform/platform.hpp"

#include <cstring>

#include <safe_str_lib.h>

char *convert_path_sep(const char *cPath)
{
    const auto len = strlen(cPath) + 1;
    auto newPath = new char[len];
    strcpy(newPath, cPath);

    while (char *sep = strchr(newPath, '\\'))
        *sep = '/';

    return newPath;
}

// use wrapper for strcat_s from safeclib instead of #define strcat_s
#undef strcat_s
errno_t strcat_s(char *restrict dest, rsize_t size, const char *restrict src)
{
    return _strcat_s_chk(dest, size, src, BOS(dest));
}

// use wrapper for strcpy_s from safeclib instead of #define strcpy_s
#undef strcpy_s
errno_t strcpy_s(char *restrict dest, rsize_t size, const char *restrict src)
{
    return _strcpy_s_chk(dest, size, src, BOS(dest));
}

// use wrapper for strncpy_s from safeclib instead of #define strncpy_s
#undef strncpy_s
errno_t strncpy_s(char *restrict dest, rsize_t size, const char *restrict src, rsize_t count)
{
    return _strncpy_s_chk(dest, size, src, count, BOS(dest), BOS(src));
}

// use custom sprintf_s instead of safeclib: #define sprintf_s(dest, dmax, ...)
#undef sprintf_s
int sprintf_s(char *buffer, rsize_t size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return result;
}

#endif
