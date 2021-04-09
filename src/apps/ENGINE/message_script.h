#pragma once

#include "core.h"

#include "Cvector.h"
#include "Entity.h"
#include "message.h"
#include "vdata.h"
#include "vmodule_api.h"

// Class for implementing function SendMessage from script in same way as
// api function Send_Message (with variable number of arguments)

class MESSAGE_SCRIPT : public MESSAGE
{
  protected:
    char *format;
    char *pData;
    long Data_size;
    char *ReadPointer;

    // long index;
  public:
    MESSAGE_SCRIPT()
    {
        format = nullptr;
        index = 0;
        pData = nullptr;
        Data_size = 0;
        ReadPointer = nullptr;
    };
    ~MESSAGE_SCRIPT()
    {
        delete format;
        free(pData);
    };
    entid_t Sender_ID;
    void Move2Start()
    {
        ResetIndex();
    };
    // va_list args;
    bool Set(char *data)
    {
        long arg_size;
        switch (GetCurrentFormatType())
        {
        case 'l':
            arg_size = sizeof(long);
            break;
        case 'p':
            arg_size = sizeof(uintptr_t);
            break;
        case 'f':
            arg_size = sizeof(float);
            break;
        case 's':
            if (data == nullptr)
                arg_size = 1;
            else
                arg_size = strlen(data) + 1;
            break;
        case 'i':
            arg_size = sizeof(entid_t);
            break;
        case 'e':
            arg_size = sizeof(DATA *);
            break;
        case 'a':
            arg_size = sizeof(ATTRIBUTES *);
            break;
        default:
            throw "Invalid MESSAGE_SCRIPT data type";
        }
        pData = (char *)realloc(pData, Data_size + arg_size);
        if (GetCurrentFormatType() == 's' && data == nullptr)
        {
            char bf = 0;
            memcpy((char *)(pData + Data_size), &bf, 1);
        }
        else
            memcpy((char *)(pData + Data_size), data, arg_size);
        Data_size += arg_size;
        index++;
        return true;
    };
    uint8_t Byte()
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    uint16_t Word()
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    long Dword()
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    double Double()
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    // char * Pointer()    { throw "Invalid MESSAGE_SCRIPT data type"; }
    CVECTOR CVector()
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    void MemoryBlock(uint32_t memsize, char *buffer)
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    void Struct(uint32_t sizeofstruct, char *s)
    {
        throw "Invalid MESSAGE_SCRIPT data type";
    }
    ATTRIBUTES *AttributePointer()
    {
        ATTRIBUTES *tAP;
        ValidateFormat('a');
        memcpy(&tAP, ReadPointer, sizeof(ATTRIBUTES *));
        ReadPointer += sizeof(ATTRIBUTES *);
        return tAP;
    }
    long Long()
    {
        long tLong;
        ValidateFormat('l');
        memcpy(&tLong, ReadPointer, sizeof(long));
        ReadPointer += sizeof(long);
        return tLong;
    }
    char *Pointer()
    {
        char *tPtr;
        ValidateFormat('p');
        memcpy(&tPtr, ReadPointer, sizeof(tPtr));
        ReadPointer += sizeof(tPtr);
        return tPtr;
    }
    float Float()
    {
        float tFloat;
        ValidateFormat('f');
        memcpy(&tFloat, ReadPointer, sizeof(float));
        ReadPointer += sizeof(float);
        return tFloat;
    }
    entid_t EntityID()
    {
        entid_t id;
        ValidateFormat('i');
        memcpy(&id, ReadPointer, sizeof(entid_t));
        ReadPointer += sizeof(entid_t);
        return id;
    }
    VDATA *ScriptVariablePointer()
    {
        VDATA *ptr;
        ValidateFormat('e');
        memcpy(&ptr, ReadPointer, sizeof(VDATA *));
        ReadPointer += sizeof(VDATA *);
        return ptr;
    }
    void String(uint32_t dest_buffer_size, char *buffer)
    {
        ValidateFormat('s');
        uint32_t size = strlen(ReadPointer) + 1;
        if (size >= dest_buffer_size)
        {
            memcpy(buffer, ReadPointer, dest_buffer_size);
            if (dest_buffer_size > 0)
                buffer[dest_buffer_size - 1] = 0;
            core.Trace("MESSAGE_SCRIPT::String() data clamped to %s ", buffer);
            return;
            // throw "insufficient string buffer";
        }
        memcpy(buffer, ReadPointer, size);
        ReadPointer += size;

        /*char * mem_PTR;
        uint32_t size;
        if(!buffer) throw "zero string buffer";
        ValidateFormat('s');
        mem_PTR = va_arg(args,char*);
        size = strlen(mem_PTR) + 1;
        if(size >= dest_buffer_size) throw "insufficient string buffer";
        memcpy(buffer,mem_PTR,size);*/
    }
    virtual char *StringPointer()
    {
        ValidateFormat('s');
        char *pStr = ReadPointer;
        ReadPointer += strlen(ReadPointer) + 1;
        return pStr;
    }
    void ValidateFormat(char c)
    {
        if (!format)
            throw "Read from empty MESSAGE_SCRIPT";
        if (format[index] != c)
            throw "Incorrect MESSAGE_SCRIPT data";
        index++;
    }
    void ResetIndex()
    {
        index = 0;
        ReadPointer = pData;
    };
    void Reset(const char *_format)
    {
        if (!_format)
            throw "MESSAGE_SCRIPT: cant reset with empty format string";
        if (format)
            delete format;

        const auto len = strlen(_format) + 1;
        format = new char[len];
        index = 0;
        strcpy_s(format, len, _format);
        // format =  _format;
        free(pData);
        pData = nullptr;
        Data_size = 0;
        ReadPointer = nullptr;
        index = 0;
    }
    char GetCurrentFormatType()
    {
        return format[index];
    };
};
