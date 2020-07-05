#include "KeyBuffer.h"
#include "..\\common_h\\defines.h"

ControlKeyBuffer::ControlKeyBuffer()
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

void ControlKeyBuffer::AddKey( long nKeyCode, UINT uScanCode, bool bSystem )
{
	m_pcBuffer[m_nBufLen].bSystem = bSystem;
	m_pcBuffer[m_nBufLen].ucVKey = (UCHAR)((DWORD)nKeyCode & 0xFF);
	m_nBufLen++;
}
