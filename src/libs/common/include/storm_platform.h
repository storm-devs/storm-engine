#pragma once
#ifdef _WIN32
const char *convert_path_sep(const char *cPath);
#else

#include <limits.h>

#include "storm_d3dx9.h"
#include "wine/winuser.rh"
#include "safe_types.h"
#undef EXTERN // fix for token.h:72:5: error: expected identifier EXTERN,

#define MAX_PATH PATH_MAX
#define _MAX_FNAME NAME_MAX
#define MAKELONG(low, high) ((int32_t)(((uint16_t)(low)) | (((uint32_t)((uint16_t)(high))) << 16)))

char *convert_path_sep(const char *cPath);

errno_t strcat_s(char *restrict dest, rsize_t size, const char *restrict src);
template <rsize_t size> errno_t strcat_s(char (&dest)[size], const char *src)
{
    return strcat_s(dest, size, src);
}

errno_t strcpy_s(char *restrict dest, rsize_t size, const char *restrict src);
template <rsize_t size> errno_t strcpy_s(char (&dest)[size], const char *src)
{
    return strcpy_s(dest, size, src);
}

errno_t strncpy_s(char *restrict dest, rsize_t size, const char *restrict src, rsize_t count);
template <rsize_t size> errno_t strncpy_s(char (&dest)[size], const char *src, rsize_t count)
{
    return strncpy_s(dest, size, src, count);
}

template <rsize_t size> int sprintf_s(char (&buffer)[size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}
int sprintf_s(char *buffer, rsize_t size, const char *format, ...);

#endif
