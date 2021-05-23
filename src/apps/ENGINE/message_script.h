#pragma once

#include "core.h"

#include "Cvector.h"
#include "entity.h"
#include "message.h"
#include "vdata.h"
#include "vmodule_api.h"

// Class for implementing function SendMessage from script in same way as
// api function Send_Message (with variable number of arguments)

class MESSAGE_SCRIPT : public MESSAGE
{
  public:
    entid_t Sender_ID;
    void Move2Start()
    {
        ResetIndex();
    };

    bool Set(long value)
    {
        ValidateFormat('l');
        params_[index - 1] = value;
        return true;
    }

    bool Set(uintptr_t value)
    {
        ValidateFormat('p');
        params_[index - 1] = value;
        return true;
    }

    bool Set(float value)
    {
        ValidateFormat('f');
        params_[index - 1] = value;
        return true;
    }

    bool Set(std::string value)
    {
        ValidateFormat('s');
        params_[index - 1] = std::move(value);
        return true;
    }

    bool SetEntity(entid_t value)
    {
        ValidateFormat('i');
        params_[index - 1] = value;
        return true;
    }

    bool Set(VDATA *value)
    {
        ValidateFormat('e');
        params_[index - 1] = value;
        return true;
    }

    bool Set(ATTRIBUTES *value)
    {
        ValidateFormat('a');
        params_[index - 1] = value;
        return true;
    }

    void String(uint32_t dest_buffer_size, char *buffer)
    {
        std::string result = MESSAGE::String();
        if (result.size() >= dest_buffer_size)
        {
            memcpy(buffer, result.c_str(), dest_buffer_size);
            if (dest_buffer_size > 0)
                buffer[dest_buffer_size - 1] = 0;
            core.Trace("MESSAGE_SCRIPT::String() data clamped to %s ", buffer);
            return;
        }
        memcpy(buffer, result.c_str(), result.size());
    }

    void ValidateFormat(char c)
    {
        if (format_.empty())
            throw std::runtime_error("Read from empty MESSAGE_SCRIPT");
        if (format_[index] != c)
            throw std::runtime_error("Incorrect MESSAGE_SCRIPT data");
        index++;
    }
    void ResetIndex()
    {
        index = 0;
    };
    void Reset(const char *_format)
    {
        format_ = _format;
        params_.resize(format_.size());
        index = 0;
    }
    char GetCurrentFormatType()
    {
        return format_[index];
    };
};
