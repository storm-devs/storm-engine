#ifndef _VBUFFER_H_
#define _VBUFFER_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "..\..\common_h\exs.h"

class VBUFFER
{
	dword dwSize;
public:
	char * Ptr;
	 VBUFFER() {dwSize = 0; Ptr = null;}
	~VBUFFER() {if(Ptr) delete Ptr;}
	void Size(dword _size)
	{
		if(!Ptr) Ptr = (char*)NEW char[_size];
		else Ptr = (char*)RESIZE(Ptr,_size);
		if(!Ptr) THROW; 
		dwSize = _size; };
};

#endif