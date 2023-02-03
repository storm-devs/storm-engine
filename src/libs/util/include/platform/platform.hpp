#pragma once
#ifdef _WIN32

#define PATH_SEP '\\'
#define WRONG_PATH_SEP '/'

inline const char *convert_path_sep(const char *cPath)
{
    return cPath;
}

#else // NOT _WIN32

#include <cerrno>
#include <climits>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include "d3dx9.hpp"
#include "winuser.rh"

#undef EXTERN // fix for token.h:72:5: error: expected identifier EXTERN,

#define MAX_PATH PATH_MAX
#define _MAX_FNAME NAME_MAX
#define MAKELONG(low, high) ((int32_t)(((uint16_t)(low)) | (((uint32_t)((uint16_t)(high))) << 16)))

#define PATH_SEP '/'
#define WRONG_PATH_SEP '\\'

inline char *convert_path_sep(const char *cPath)
{
    const auto len = strlen(cPath) + 1;
    auto newPath = new char[len];
    strcpy(newPath, cPath);

    while (char *sep = strchr(newPath, '\\'))
        *sep = '/';

    return newPath;
}

inline int strcat_s(char * dest, size_t num, const char * source)
{
    if(!dest)
        return EINVAL;

    if(!source)
    {
        dest[0] = '\0';
        return EINVAL;
    }

    size_t i, j;
    for(i = 0; i < num; i++)
    {
        if(dest[i] == '\0')
        {
            for(j = 0; (j + i) < num; j++)
            {
                if((dest[j + i] = source[j]) == '\0')
                    return 0;
            }
        }
    }

    dest[0] = '\0';
    return ERANGE;
}

template <size_t size> inline int strcat_s(char (&dest)[size], const char *src)
{
    return strcat_s(dest, size, src);
}

inline int strcpy_s(char *dest, size_t num, const char *source)
{
    if(!dest)
        return EINVAL;

    if(0 == num)
    {
        dest[0] = '\0';
        return ERANGE;
    }

    if(!source)
    {
        dest[0] = '\0';
        return EINVAL;
    }

    size_t i;
    for(i = 0; i < num; i++)
    {
        if((dest[i] = source[i]) == '\0')
            return 0;
    }
    dest[0] = '\0';
    return ERANGE;
}

template <size_t size> inline int strcpy_s(char (&dest)[size], const char *src)
{
    return strcpy_s(dest, size, src);
}

inline int strncpy_s(char * dest, size_t dst_size, const char * source, size_t num)
{
    if (!dest || (0 == dst_size))
        return EINVAL;

    if(0 == num)
    {
        dest[0] = '\0';
        return 0;
    }

    if (!source)
    {
        dest[0] = '\0';
        return EINVAL;
    }

    size_t i, end;
    if(num < dst_size)
        end = num;
    else
        end = dst_size - 1;

    for(i = 0; i < end && source[i]; i++)
        dest[i] = source[i];

    if(!source[i] || end == num)
    {
        dest[i] = '\0';
        return 0;
    }

    dest[0] = '\0';

    return EINVAL;
}

template <size_t size> inline int strncpy_s(char (&dest)[size], const char *src, size_t count)
{
    return strncpy_s(dest, size, src, count);
}

inline int sprintf_s(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

template <size_t size> inline int sprintf_s(char (&buffer)[size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

#endif
