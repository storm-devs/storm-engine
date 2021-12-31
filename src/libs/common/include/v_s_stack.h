#pragma once

#include "v_data.h"

class DATA;

class VS_STACK
{
  public:
    VS_STACK(){};

    virtual ~VS_STACK(){};
    virtual VDATA *Push(DATA *pdataclass = nullptr) = 0;
    virtual VDATA *Pop() = 0;
    virtual VDATA *Read(uint32_t offset, uint32_t index) = 0;
    virtual VDATA *Read() = 0;
    virtual uint32_t GetDataNum() = 0;
    virtual void InvalidateFrom(uint32_t index) = 0;
};
