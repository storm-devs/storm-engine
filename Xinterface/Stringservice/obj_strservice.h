#ifndef _OBJ_STRSERVICE_H_
#define _OBJ_STRSERVICE_H_

#include "..\\..\\common_h\\dx8render.h"
#include "..\..\common_h\stringService.h"

class OBJ_STRSERVICE : public ENTITY
{
	VSTRSERVICE * m_pStrService;
public:
	OBJ_STRSERVICE();
	~OBJ_STRSERVICE();
	bool	Init();
    dword _cdecl ProcessMessage(MESSAGE & message);
};

#endif
