#ifndef _VEVENT_H_
#define _VEVENT_H_

#include "..\common_h\vmodule_api.h"

class VEVENT : public SERVICE
{
public:
	VEVENT(){};
	virtual ~VEVENT(){};
	virtual dword _cdecl Event(char * name, char * format,...)= 0;
};

#endif
