#include "key_buffer.h"

ControlKeyBuffer::ControlKeyBuffer()
{
}

ControlKeyBuffer::~ControlKeyBuffer()
{
}

void ControlKeyBuffer::Reset()
{
    pcBuffer_.clear();
}

void ControlKeyBuffer::AddKey(char *u8_str, int u8_size, bool bSystem)
{
    KeyDescr key;
    key.ucVKey.c = 0;
    for (int i = 0; i < u8_size; i++)
        key.ucVKey.b[i] = u8_str[i];

    key.ucVKey.l = u8_size;
    key.bSystem = bSystem;

    pcBuffer_.push_back(key);
}

void ControlKeyBuffer::AddKey(const KeyDescr &key)
{
    pcBuffer_.push_back(key);
}
