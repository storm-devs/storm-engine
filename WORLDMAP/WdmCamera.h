//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmCamera
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmCamera_H_
#define _WdmCamera_H_

#include "..\common_h\dx8render.h"

class WdmCamera  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmCamera();
	virtual ~WdmCamera();

//--------------------------------------------------------------------------------------------
//Перемещение камеры
//--------------------------------------------------------------------------------------------
public:
	void Init(float defAy, float defHeight = -1.0f);
	void Move(float dltTime, VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//Функции для управления
//--------------------------------------------------------------------------------------------
protected:
	virtual void CtrlProcess(float dltTime) = null;
	virtual float MoveLeftRight(float dltTime) = null;
	virtual float MoveUpDown(float dltTime) = null;
	virtual float RotLeftRight(float dltTime) = null;
	virtual float ZoomInOut(float dltTime) = null;
	virtual bool CurrentFreeMode() = null;
	virtual bool GetHightHeight(float & height) = null;

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
public:
	CVECTOR pos;
	CVECTOR ang;
	float realHeight;
	float hgh;

	bool lock;

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void CheckRange();

private:
	float scrollSpeed;
	float rotateSpeed;
	float elasticAy;

	bool isFreeMode;
	float lastAng;
	CVECTOR bndPos;
	float kBlendPos;

	bool isInit;
};

#endif

