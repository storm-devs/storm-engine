#include "KeyBuffer.h"

ControlKeyBuffer::ControlKeyBuffer() : m_pcBuffer{}
{
    m_nBufLen = 0;
}

ControlKeyBuffer::~ControlKeyBuffer()
{
}

void ControlKeyBuffer::Reset()
{
    m_nBufLen = 0;
}

void ControlKeyBuffer::AddKey(char *u8_str, int u8_size, bool bSystem)
{
    m_pcBuffer[m_nBufLen].ucVKey.c = 0;
    for (int i = 0; i < u8_size; i++)
    {
        m_pcBuffer[m_nBufLen].ucVKey.b[i] = u8_str[i];
    }
    m_pcBuffer[m_nBufLen].ucVKey.l = u8_size;
    m_pcBuffer[m_nBufLen].bSystem = bSystem;
    m_nBufLen++;
}
