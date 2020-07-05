#ifndef _IEDITOR_MOUSE_POINTER_H_
#define _IEDITOR_MOUSE_POINTER_H_

#include "defines.h"

class EdMousePointer
{
public:
	EdMousePointer();
	~EdMousePointer();

protected:

public:
	EdPoint m_pntPos;

	long m_idTexture;
	EdPoint m_pntSize;
};

#endif
