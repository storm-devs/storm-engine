#ifndef _VPARTICLES_SYSTEM_H_
#define _VPARTICLES_SYSTEM_H_

//#include <windows.h>
#include "..\common_h\service.h"

class VPARTICLES_SYSTEM : public SERVICE
{
public:
	virtual void * CreateParticleSystem(char * ps_name, char * layer_name)= 0;
	virtual void * CreateParticleSystem(char * ps_name, char * layer_name,CVECTOR pos,CVECTOR dir)= 0;
	virtual void * CreateParticleSystem(char * ps_name, char * layer_name,CVECTOR pos,CVECTOR dir, ENTITY_ID eid)= 0;
};

#endif
