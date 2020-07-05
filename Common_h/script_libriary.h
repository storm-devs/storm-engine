#ifndef _SCRIPT_LIBRIARY_H_
#define _SCRIPT_LIBRIARY_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "d_types.h"
#include "entity_state.h"

class SCRIPT_LIBRIARY
{
public:
	SCRIPT_LIBRIARY(){};
	virtual ~SCRIPT_LIBRIARY(){};
	virtual bool Init(){return false;};
};

#endif