#pragma once

#include "../Animation/ActionInfo.h"
#include "controls.h"
#include "vmodule_api.h"

struct ControlKeyBuffer
{
  public:
    ControlKeyBuffer();
    ~ControlKeyBuffer();

    void Reset();
    void AddKey(char *u8_str, int u8_size, bool bSystem);

    long GetBufferLength()
    {
        return m_nBufLen;
    }
    const KeyDescr *c_str()
    {
        return m_pcBuffer;
    }

    long m_nBufLen;
    KeyDescr m_pcBuffer[512];
};
