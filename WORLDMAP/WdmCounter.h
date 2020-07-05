//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmCounter
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmCounter_H_
#define _WdmCounter_H_

#include "WdmRenderModel.h"


#define WMD_NUM_SKYS	8


class WdmCounter : public WdmRenderModel  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmCounter();
	virtual ~WdmCounter();

	bool Init();

	//Расчёты
	void Update(float dltTime);

	void PRender(VDX8RENDER * rs){};
	void MRender(VDX8RENDER * rs){};
	void LRender(VDX8RENDER * rs);


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	bool LoadModel(WdmRenderModel * & pnt, const char * name, const char * tech);
	void DrawNum(VDX8RENDER * rs, WdmRenderModel * m, float u, float v);


private:
	WdmRenderModel * sky;
	WdmRenderModel * d[2];
	WdmRenderModel * m[2];
	WdmRenderModel * y[4];

	long skytx[WMD_NUM_SKYS];
	long lastSkys[2];
	long skyseq[8];
	long dayCounter;
	long skyCounter;

	static const char * skytex[WMD_NUM_SKYS];
};

#endif

