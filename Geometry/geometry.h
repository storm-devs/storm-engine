#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "..\common_h\vmodule_api.h"
#include "..\Geom_lib\geos.h"

class VGEOMETRY  : public SERVICE
{
public:
	virtual ~VGEOMETRY(){};
	virtual GEOS * CreateGeometry(const char * file_name,const char * light_file_name, const char *anim, long flags)= 0;
	virtual float SetTime(float time)=0;
	virtual void DeleteGeometry(GEOS *)= 0;
};

#endif