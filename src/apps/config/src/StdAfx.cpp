// stdafx.cpp : source file that includes just the standard includes
//    Setup.pch will be the pre-compiled header
//    stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

char *GetTokenWhile(char *src, char *dst, const char *skip)
{
    if (!src || !dst || !skip)
        return 0;
    dst[0] = 0;
    char *dst1 = dst;
    long lensrc = strlen(src);
    long dstlen = 0;
    while (lensrc && src[0] == ' ')
    {
        src++;
        lensrc--;
    };
    while ((lensrc--) && (*src != *skip))
    {
        *dst1++ = *src++;
        dstlen++;
    }
    *dst1++ = 0;
    char *pRet = ((lensrc > 0) ? ++src : src);
    if (lensrc)
        lensrc--;
    while (lensrc && pRet[0] == 32)
    {
        pRet++;
        lensrc--;
    };
    while (dstlen && dst[dstlen - 1] == ' ')
    {
        dstlen--;
        dst[dstlen] = 0;
    }
    return pRet;
}
