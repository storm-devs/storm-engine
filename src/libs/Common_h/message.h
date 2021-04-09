#pragma once

#include <cstdarg>
#include <cstring>

#include "Cvector.h"
#include "Entity.h"

class VDATA;

class MESSAGE
{
  protected:
    const char *format;
    long index;
    va_list args_start;

  public:
    entid_t Sender_ID;
    va_list args;

    virtual void Move2Start()
    {
        args = args_start;
        index = 0;
    };

    virtual uint8_t Byte()
    {
        ValidateFormat('b');
        return va_arg(args, uint8_t);
    }

    virtual uint16_t Word()
    {
        ValidateFormat('w');
        return va_arg(args, uint16_t);
    }

    virtual long Long()
    {
        ValidateFormat('l');
        return va_arg(args, long);
    }

    virtual long Dword()
    {
        ValidateFormat('u');
        return va_arg(args, uint32_t);
    }

    virtual float Float()
    {
        ValidateFormat('f');
        return static_cast<float>(va_arg(args, double));
    }

    virtual double Double()
    {
        ValidateFormat('d');
        return va_arg(args, double);
    }

    virtual char *Pointer()
    {
        ValidateFormat('p');
        return va_arg(args, char *);
    }

    virtual ATTRIBUTES *AttributePointer()
    {
        ValidateFormat('a');
        return va_arg(args, ATTRIBUTES *);
    }

    virtual entid_t EntityID()
    {
        ValidateFormat('i');
        return va_arg(args, entid_t);
    }

    virtual VDATA *ScriptVariablePointer()
    {
        ValidateFormat('e');
        return va_arg(args, VDATA *);
    }

    virtual CVECTOR CVector()
    {
        ValidateFormat('c');
        return va_arg(args, CVECTOR);
    }

    virtual void String(uint32_t dest_buffer_size, char *buffer)
    {
        char *mem_PTR;
        size_t size;
        if (!buffer)
            throw "zero string buffer";
        ValidateFormat('s');
        mem_PTR = va_arg(args, char *);
        if (!mem_PTR)
            throw "invalid string : MESSAGE.String()";
        size = strlen(mem_PTR) + 1;
        if (size >= dest_buffer_size)
            throw "insufficient string buffer";
        memcpy(buffer, mem_PTR, size);
    }

    virtual void MemoryBlock(uint32_t memsize, char *buffer)
    {
        char *mem_PTR;
        size_t size;
        if (!buffer)
            throw "zero mem buffer";
        ValidateFormat('m');
        size = va_arg(args, uint32_t);
        if (memsize != size)
            throw "invalid memory block size";
        mem_PTR = va_arg(args, char *);
        memcpy(buffer, mem_PTR, memsize);
    }

    virtual void Struct(uint32_t sizeofstruct, char *s)
    {
        uint32_t a;
        size_t size;
        if (!s)
            throw "Invalid s buffer";
        ValidateFormat('v');
        size = va_arg(args, uint32_t);
        if (sizeofstruct != size)
            throw "Invalid message structure size";
        a = ((sizeofstruct + sizeof(int) - 1) & ~(sizeof(int) - 1));
        memcpy(s, static_cast<char *>((args += a) - a), sizeofstruct);
    }

    virtual void ValidateFormat(char c)
    {
        if (!format)
            throw "Read from empty message";
        if (format[index] != c)
            throw "Incorrect message data";
        index++;
    }

    virtual void Reset(const char *_format)
    {
        index = 0;
        format = _format;
        args_start = args;
    }

    virtual char GetCurrentFormatType()
    {
        return format[index];
    };

    virtual char *StringPointer()
    {
        char *mem_PTR;
        ValidateFormat('s');
        mem_PTR = va_arg(args, char *);
        return mem_PTR;
    }
};
