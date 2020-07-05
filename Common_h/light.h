#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "..\common_h\vmodule_api.h"

class LIGHTSRC
{
public:
	bool on;
	CVECTOR pos;
	float r,g,b;
	float intensity;
	float specular;
	float range;
};

class VLIGHT  : public SERVICE
{
public:
	virtual ~VLIGHT(){};
	virtual LIGHTSRC *CreateLight()=0;
	virtual void DeleteLight(LIGHTSRC *l)=0;
};

#endif