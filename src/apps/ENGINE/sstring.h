#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstring>

class SSTRING
{
    char *pDataPointer;

  public:
    SSTRING()
    {
        pDataPointer = nullptr;
    };
    SSTRING(const char *p)
    {
        if (!p)
            return;
        const auto len = strlen(p) + 1;
        pDataPointer = new char[len];
        strcpy_s(pDataPointer, len, p);
    }
    SSTRING(const SSTRING &ss) : pDataPointer(nullptr)
    {
        const auto len = strlen(ss.pDataPointer) + 1;
        pDataPointer = new char[len];
        strcpy_s(pDataPointer, len, ss.pDataPointer);
    }
    ~SSTRING()
    {
        if (pDataPointer)
            delete pDataPointer;
        pDataPointer = nullptr;
    };
    SSTRING &operator=(const SSTRING &ss)
    {
        if (this == &ss)
            return *this;
        if (pDataPointer)
            delete pDataPointer;
        const auto len = strlen(ss.pDataPointer) + 1;
        pDataPointer = new char[len];
        strcpy_s(pDataPointer, len, ss.pDataPointer);
        return *this;
    }
    SSTRING &operator=(const char sym)
    {
        if (pDataPointer)
            delete pDataPointer;
        pDataPointer = new char[2];
        pDataPointer[0] = sym;
        pDataPointer[1] = 0;
        return *this;
    }
    operator const char *()
    {
        return pDataPointer;
    };
    bool operator==(const char *p)
    {
        if (pDataPointer == p)
            return true;
        if (pDataPointer == nullptr || p == nullptr)
            return false;
        if (strcmp(pDataPointer, p) == 0)
            return true;
        return false;
    };
    bool operator!=(const char *p)
    {
        if (pDataPointer == p)
            return false;
        if (pDataPointer == nullptr || p == nullptr)
            return true;
        if (strcmp(pDataPointer, p) == 0)
            return false;
        return true;
    };
    void operator+=(const char *p)
    {
        if (p == nullptr)
            return;
        if (pDataPointer == nullptr)
        {
            const auto len = strlen(p) + 1;
            pDataPointer = new char[len];
            strcpy_s(pDataPointer, len, p);
            return;
        }
        const auto len = strlen(pDataPointer) + strlen(p) + 1;
        char *pp = new char[len];
        strcpy_s(pp, len, pDataPointer);
        strcat_s(pp, len, p);
        delete pDataPointer;
        pDataPointer = pp;
    };
    void operator+=(const char sym)
    {
        if (pDataPointer == nullptr)
        {
            pDataPointer = new char[2];
            pDataPointer[0] = sym;
            pDataPointer[1] = sym;
            return;
        }
        uint32_t nLen = strlen(pDataPointer);
        char *pp = new char[nLen + 2];
        strcpy_s(pp, nLen + 2, pDataPointer);
        pp[nLen] = sym;
        pp[nLen + 1] = 0;
        delete[] pDataPointer;
        pDataPointer = pp;
    };
    SSTRING &format(const char *format_string, ...)
    {
        char buffer[256];
        va_list args;
        va_start(args, format_string);
        _vsnprintf_s(buffer, sizeof(buffer), format_string, args);
        va_end(args);
        if (pDataPointer)
            delete pDataPointer;
        const auto len = strlen(buffer) + 1;
        pDataPointer = new char[len];
        strcpy_s(pDataPointer, len, buffer);
        return *this;
    }
};
