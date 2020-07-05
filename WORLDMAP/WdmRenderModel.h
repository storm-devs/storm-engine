//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmRenderModel
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmRenderModel_H_
#define _WdmRenderModel_H_


#include "WdmRenderObject.h"
#include "WdmObjects.h"

#include "..\common_h\matrix.h"

class GEOS;
class VGEOMETRY;


class WdmRenderModel : public WdmRenderObject  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmRenderModel();
	virtual ~WdmRenderModel();

	virtual bool Load(const char * modelName);

	void PRender(VDX8RENDER * rs);
	void MRender(VDX8RENDER * rs);
	void LRender(VDX8RENDER * rs);

	void SetTech(const char * t, const char * ta);
	long GetTexture(long stage);
	void SetTexture(long stage, long id);
	

	CMatrix mtx;
	float alpha;

public:
	void Render(VDX8RENDER * rs);

	GEOS *geo;
	const char * tech;
	const char * techa;

	//Позиционированный круг
	CVECTOR center;
	float radius;

	bool drawCircle;
	
};

#endif

