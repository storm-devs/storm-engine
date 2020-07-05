#ifndef _INTERNAL_MODULES_H_
#define _INTERNAL_MODULES_H_

#include "..\..\common_h\vmodule_api.h"

struct INTERNAL_MODULE_DESC {
	char * name;
	long   code;
};

class INTERNAL_MODULES
{
public:
	 INTERNAL_MODULES();
	~INTERNAL_MODULES();

};

#endif