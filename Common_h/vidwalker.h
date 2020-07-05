#ifndef _VIDWALKER_H_
#define _VIDWALKER_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "d_types.h"

class VIDWALKER
{
public:
	virtual ~VIDWALKER(){};
	virtual ENTITY_ID * GetID()= 0;
	virtual ENTITY_ID * GetIDNext()= 0;
	virtual void Invalidate()= 0;
	virtual bool Validate()= 0;
	
};

#endif