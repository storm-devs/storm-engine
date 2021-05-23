#pragma once

#include <cstdarg>
#include <cstring>
#include <variant>

#include "Cvector.h"
#include "entity.h"

class VDATA;

namespace storm {

using MessageParam = std::variant<
    uint8_t,
    uint16_t,
    uint32_t,
    long,
    float,
    double,
    char *,
    ATTRIBUTES *,
    entid_t,
    VDATA *,
    CVECTOR,
    std::string
    >;

} // namespace storm

class MESSAGE
{
  protected:
    long index;

  public:
    entid_t Sender_ID;

    virtual void Move2Start()
    {
        index = 0;
    };

    virtual uint8_t Byte()
    {
        ValidateFormat('b');
        return get<uint8_t>(params_[index - 1]);
    }

    virtual uint16_t Word()
    {
        ValidateFormat('w');
        return get<uint16_t>(params_[index - 1]);
    }

    virtual long Long()
    {
        ValidateFormat('l');
        return get<long>(params_[index - 1]);
    }

    virtual long Dword()
    {
        ValidateFormat('u');
        return static_cast<long>(get<uint32_t>(params_[index - 1]));
    }

    virtual float Float()
    {
        ValidateFormat('f');
        return get<float>(params_[index - 1]);
    }

    virtual double Double()
    {
        ValidateFormat('d');
        return get<double>(params_[index - 1]);
    }

    virtual char *Pointer()
    {
        ValidateFormat('p');
        return get<char *>(params_[index - 1]);
    }

    virtual ATTRIBUTES *AttributePointer()
    {
        ValidateFormat('a');
        return get<ATTRIBUTES *>(params_[index - 1]);
    }

    virtual entid_t EntityID()
    {
        ValidateFormat('i');
        return get<entid_t>(params_[index - 1]);
    }

    virtual VDATA *ScriptVariablePointer()
    {
        ValidateFormat('e');
        return get<VDATA *>(params_[index - 1]);
    }

    virtual CVECTOR CVector()
    {
        ValidateFormat('c');
        return get<CVECTOR>(params_[index - 1]);
    }

    virtual const std::string& String()
    {
        ValidateFormat('s');
        return get<std::string>(params_[index - 1]);
    }

    virtual void ValidateFormat(char c)
    {
        if (format_.empty())
            throw std::runtime_error("Read from empty message");
        if (format_[index] != c)
            throw std::runtime_error("Incorrect message data");
        index++;
    }

    virtual void Reset(const char *_format, va_list args)
    {
        index = 0;
        format_ = _format;
        params_.resize(format_.size());
        std::transform(format_.begin(), format_.end(), params_.begin(), [&] (const char c) {
            return GetParamValue(c, args);
        });
    }

    virtual char GetCurrentFormatType()
    {
        return format_[index];
    };

    virtual const char *StringPointer()
    {
        return String().c_str();
    }

  protected:
    static storm::MessageParam GetParamValue(const char c, va_list& args) {
        switch (c)
        {
        case 'b':
            return va_arg(args, uint8_t);
        case 'w':
            return va_arg(args, uint16_t);
        case 'l':
            return va_arg(args, long);
        case 'u':
            return va_arg(args, uint32_t);
        case 'f':
            return static_cast<float>(va_arg(args, double));
        case 'd':
            return va_arg(args, double);
        case 'p':
            return va_arg(args, char *);
        case 'a':
            return va_arg(args, ATTRIBUTES *);
        case 'i':
            return va_arg(args, entid_t);
        case 'e':
            return va_arg(args, VDATA *);
        case 'c':
            return va_arg(args, CVECTOR);
        case 's': {
            char* ptr = va_arg(args, char *);
            return std::string(ptr);
        }
        default:
            throw std::runtime_error(fmt::format("Unknown message format: '{}'", c));
        }
    }

    std::string format_;
    std::vector<storm::MessageParam> params_;
};
