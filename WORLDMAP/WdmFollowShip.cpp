//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmFollowShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmFollowShip.h"
#include "WdmIslands.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmFollowShip::WdmFollowShip()
{
	isEnemy = true;
	shipType = wdmest_follow;
	mtx.Pos().x = wdmObjects->worldSizeX;
	mtx.Pos().z = wdmObjects->worldSizeZ;
}

WdmFollowShip::~WdmFollowShip()
{

}

//Установка параметров
bool WdmFollowShip::Load(const char * modelName)
{
	if(!WdmShip::Load(modelName)) return false;
	//Ищем позицию
	float x, z, ay = 0.0f;
	killMe = !GeneratePosition(1.2f*modelRadius, 6.28f, x, z);
	if(wdmObjects->playerShip)
	{
		//Определяем угл
		double sn = wdmObjects->playerShip->mtx.Pos().x - mtx.Pos().x;
		double cs = wdmObjects->playerShip->mtx.Pos().z - mtx.Pos().z;
		double l = sn*sn + cs*cs;
		if(l > 0.0) cs /= sqrt(l);
		if(cs > 1.0) cs = 1.0;
		if(cs < -1.0) cs = -1.0;
		cs = acos(cs);
		if(sn < 0.0) cs = -cs;
		ay = float(cs);
	}
	//Ставим кораблик
	Teleport(x, z, ay);
	return true;
}

//Найти силу притягивающую в нужном направлении
void WdmFollowShip::FindMoveForce()
{
	CVECTOR dir;
	if(wdmObjects->islands)
	{		
		wdmObjects->islands->FindDirection(mtx.Pos(), ((WdmShip *)wdmObjects->playerShip)->mtx.Pos(), dir);
	}else{
		//Позиция игрока
		dir.x = ((WdmShip *)wdmObjects->playerShip)->mtx.Pos().x;
		dir.z = ((WdmShip *)wdmObjects->playerShip)->mtx.Pos().z;
		//Направление на игрока
		dir.x -= mtx.Pos().x;
		dir.z -= mtx.Pos().z;
		float dl = dir.x*dir.x + dir.z*dir.z;
		if(dl > 1.0f)
		{
			dl = 1.0f/sqrtf(dl);
			dir.x *= dl;
			dir.z *= dl;
		}
	}
	mx = dir.x;
	mz = dir.z;
}

