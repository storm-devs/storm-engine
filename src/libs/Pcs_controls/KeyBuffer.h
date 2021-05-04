#pragma once

#include "../Animation/ActionInfo.h"
#include "controls.h"
#include "vmodule_api.h"

#include <vector>

struct ControlKeyBuffer
{
  public:
    ControlKeyBuffer();
    ~ControlKeyBuffer();

    void Reset();
    void AddKey(char *u8_str, int u8_size, bool bSystem);
    void AddKey(const KeyDescr &key);

    long GetBufferLength()
    {
        return m_pcBuffer.size();
    }

    // FIXME: Not good
    const KeyDescr *c_str()
    {
        return m_pcBuffer.data();
    }

    std::vector<KeyDescr> m_pcBuffer;
};
