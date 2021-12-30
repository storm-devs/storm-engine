#pragma once

#include <Windows.h>
#include <cstdint>

class ENTITY_STATE_GEN
{
  public:
    virtual ~ENTITY_STATE_GEN() = default;

    virtual void _cdecl SetState(const char *Format, ...) = 0;
};

class ENTITY_STATE
{
  public:
    ENTITY_STATE(){};

    virtual ~ENTITY_STATE(){};
    virtual uint8_t Byte() = 0;
    virtual uint16_t Word() = 0;
    virtual long Long() = 0;
    virtual uint32_t Dword() = 0;
    virtual float Float() = 0;
    virtual double Double() = 0;
    virtual char *Pointer() = 0;

    virtual void String(uint32_t buffer_size, char *buffer) = 0;

    virtual void MemoryBlock(uint32_t memsize, char *buffer) = 0;

    virtual void Struct(uint32_t sizeofstruct, char *s) = 0;
};
