//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmCamera
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmCamera.h"
#include "WdmPlayerShip.h"
#include "WdmObjects.h"


//============================================================================================

#define WDM_CAMERA_HEIGHT_MIN	50.0f
#define WDM_CAMERA_HEIGHT_MAX	300.0f
#define WdmCamera_myPI 3.1415926535897932384626433832795f

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmCamera::WdmCamera()
{
	pos = CVECTOR(0.0f, 120.0f, 0.0f);
	ang = CVECTOR(1.0f, 0.0f, 0.0f);
	wdmObjects->camera = this;
	isInit = false;
	realHeight = 0.0f;
	elasticAy = 0.0f;
	lastAng = ang.y;
	isFreeMode = false;
	realHeight = pos.y;
	hgh = pos.y;
	kBlendPos = 1.0f;
	lock = false;
}

WdmCamera::~WdmCamera()
{
	wdmObjects->camera = null;
}

void WdmCamera::Init(float defAy, float defHeight)
{
	const float pi2 = 2.0f*3.14159265f;
	float k = defAy/pi2;
	ang.y = (k - long(k))*pi2;	
	if(defHeight < WDM_CAMERA_HEIGHT_MIN) defHeight = WDM_CAMERA_HEIGHT_MIN;
	if(defHeight > WDM_CAMERA_HEIGHT_MAX) defHeight = WDM_CAMERA_HEIGHT_MAX;
	pos.y = defHeight;
	realHeight = pos.y;
}

void WdmCamera::Move(float dltTime, VDX8RENDER * rs)
{
	const float pi = 3.14159265f;
	if(dltTime > 0.1f) dltTime = 0.1f;
	CtrlProcess(dltTime);
	CVECTOR ang = this->ang;
	float k = dltTime*2.0f;
	if(k > 1.0f) k = 1.0f;
	float shipAy = ((WdmPlayerShip *)wdmObjects->playerShip)->GetAy();
	float dltAng = shipAy - elasticAy;
	if(dltAng > pi) elasticAy += 2.0f*pi;
	if(dltAng < -pi) elasticAy -= 2.0f*pi;
	elasticAy += (shipAy - elasticAy)*k;

	if(!isInit) elasticAy = shipAy;
	
	bool freeMode = CurrentFreeMode();
	if(isFreeMode != freeMode)
	{
		if(freeMode)
		{
			lastAng = this->ang.y;
			this->ang.y = ang.y + elasticAy;
		}else{
			this->ang.y = lastAng;
			kBlendPos = 0.0f;
			bndPos = pos;
		}
		isFreeMode = freeMode;
	}
	if(!freeMode) ang.y += elasticAy;


	if(lock) ang.y = 0.0f;


	float cs = cosf(ang.y);
	float sn = sinf(ang.y);
	float csX = cosf(ang.x);
	float snX = sinf(ang.x);
	float movelrspd = 50.0f*MoveLeftRight(dltTime);
	float moveudspd = 50.0f*MoveUpDown(dltTime);
	float rotspd = 1.0f*RotLeftRight(dltTime);
	float zoomspd = 30.0f*ZoomInOut(dltTime);
	//Точка на высоте 0 в которую смотрим относительно текущий позиции камеры
	float cx = sn*pos.y*csX/snX;
	float cz = cs*pos.y*csX/snX;
	float r = sqrtf(cx*cx + cz*cz);
	//Перемещение свободной камеры
	if(CurrentFreeMode() && isInit)
	{
		pos.x += moveudspd*sn - movelrspd*cs;
		pos.z += moveudspd*cs + movelrspd*sn;
		if(pos.x < -0.5f*wdmObjects->worldSizeX - cx) pos.x = -0.5f*wdmObjects->worldSizeX - cx;
		if(pos.x > 0.5f*wdmObjects->worldSizeX + cx) pos.x = 0.5f*wdmObjects->worldSizeX + cx;
		if(pos.z < -0.5f*wdmObjects->worldSizeZ - cz) pos.z = -0.5f*wdmObjects->worldSizeZ - cz;
		if(pos.z > 0.5f*wdmObjects->worldSizeZ + cz) pos.z = 0.5f*wdmObjects->worldSizeZ + cz;
	}else{
		pos.x = wdmObjects->playerShip->mtx.Pos().x - cx;
		pos.z = wdmObjects->playerShip->mtx.Pos().z - cz;
		isInit = true;
	}
	//Вращение камеры
	this->ang.y += rotspd;
	pos.x = pos.x + cx - r*sinf(ang.y);
	pos.z = pos.z + cz - r*cosf(ang.y);
	//Масштабирование
	pos.x += zoomspd*sn*csX;
	pos.y -= zoomspd*snX;
	pos.z += zoomspd*cs*csX;
	if(pos.y < WDM_CAMERA_HEIGHT_MIN)
	{
		r = (WDM_CAMERA_HEIGHT_MIN - pos.y)*csX/snX;
		pos.y = WDM_CAMERA_HEIGHT_MIN;
		pos.x -= r*sn;
		pos.z -= r*cs;
	}else
	if(pos.y > WDM_CAMERA_HEIGHT_MAX)
	{
		r = (WDM_CAMERA_HEIGHT_MAX - pos.y)*csX/snX;
		pos.y = WDM_CAMERA_HEIGHT_MAX;
		pos.x -= r*sn;
		pos.z -= r*cs;
	}
	//Ограничем перемещение камеры
	CheckRange();
	//Устанавливаем новою позицию камеры
	float height;
	k = dltTime*2000.0f;
	if(!GetHightHeight(height))
	{
		height = pos.y;
	}
	if(realHeight <= height)
	{
		realHeight += k;
		if(realHeight > height) realHeight = height;
	}else{
		realHeight -= k*2.0f;
		if(realHeight < height) realHeight = height;
	}	
	r = (pos.y - realHeight)*csX/snX;
	CVECTOR ps = pos;
	kBlendPos += dltTime*8.0f;
	if(kBlendPos < 1.0f)
	{
		ps.x = bndPos.x + (pos.x - bndPos.x)*kBlendPos;
		ps.z = bndPos.z + (pos.z - bndPos.z)*kBlendPos;
	}else kBlendPos = 1.0f;
	rs->SetCamera(&CVECTOR(ps.x + r*sn, realHeight, ps.z + r*cs), &ang);
}

void WdmCamera::CheckRange()
{
	float cs = cosf(ang.y);
	float sn = sinf(ang.y);
	float csX = cosf(ang.x);
	float snX = sinf(ang.x);
	//Точка, в которую смотрим
	float cx = pos.x + sn*pos.y*csX/snX;
	float cz = pos.z + cs*pos.y*csX/snX;
	float ccx = cx, ccz = cz;
	//Ограничиваем координату
	//if(cx < -0.5f*WDM_WORLD_SIZE_X) cx = -0.5f*WDM_WORLD_SIZE_X;
	//if(cx > 0.5f*WDM_WORLD_SIZE_X) cx = 0.5f*WDM_WORLD_SIZE_X;
	//if(cz < -0.5f*WDM_WORLD_SIZE_Z) cz = -0.5f*WDM_WORLD_SIZE_Z;
	//if(cz > 0.5f*WDM_WORLD_SIZE_Z) cz = 0.5f*WDM_WORLD_SIZE_Z;
	//Просчитываем смещение
	pos.x += cx - ccx;
	pos.z += cz - ccz;
}

