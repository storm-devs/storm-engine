#ifndef _WIN32

#include "safe_str_lib.h"

// workaround for safeclib
//#undef sprintf_s(dest, dmax, ...)
//#define sprintf_s(dest, dmax, fmt, ...) _sprintf_s_chk(dest, dmax, BOS(dest), fmt, __VA_ARGS__)

template <size_t size> inline int sprintf_s(char (&buffer)[size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

/*
template <size_t size>
int sprintf_s(
   char (&buffer)[size],
   const char *format [,
   argument] ... 
);
*/

#endif
