//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmMerchantShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmMerchantShip.h"
#include "WdmIslands.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmMerchantShip::WdmMerchantShip()
{
	shipType = wdmest_merchant;
	gotoPos = 0.0f;
	gotoRad = wdmObjects->worldSizeX*wdmObjects->worldSizeX + wdmObjects->worldSizeZ*wdmObjects->worldSizeZ;
	isLookOnPlayer = true;
}

WdmMerchantShip::~WdmMerchantShip()
{
}

//Установка параметров
bool WdmMerchantShip::Load(const char * modelName)
{
	if(!WdmShip::Load(modelName)) return false;
	//Позиция
	float x, z;
	killMe = !GeneratePosition(1.2f*modelRadius, 2.0f, x, z);
	Teleport(x, z, 0.0f);
	isEnableSetDir = true;
	return true;
}

void WdmMerchantShip::Goto(float x, float z, float rad)
{
	if(x < -0.5f*wdmObjects->worldSizeX) x = -0.5f*wdmObjects->worldSizeX;
	if(x > 0.5f*wdmObjects->worldSizeX) x = 0.5f*wdmObjects->worldSizeX;
	if(z < -0.5f*wdmObjects->worldSizeZ) z = -0.5f*wdmObjects->worldSizeZ;
	if(z > 0.5f*wdmObjects->worldSizeZ) z = 0.5f*wdmObjects->worldSizeZ;
	if(rad < 1.0f) rad = 1.0f;
	gotoPos.x = x;
	gotoPos.y = 0.0f;
	gotoPos.z = z;
	gotoRad = rad;
	//Определим направление
	if(isEnableSetDir)
	{
		double sn = x - mtx.Pos().x;
		double cs = z - mtx.Pos().z;
		double l = sn*sn + cs*cs;
		if(l > 0.0) cs /= sqrt(l);
		if(cs > 1.0) cs = 1.0;
		if(cs < -1.0) cs = -1.0;
		cs = acos(cs);
		if(sn < 0.0) cs = -cs;
		ay = float(cs);
	}
}

//Найти силу притягивающую в нужном направлении
void WdmMerchantShip::FindMoveForce()
{
	isEnableSetDir = false;
	CVECTOR dir;
	if(wdmObjects->islands)
	{		
		wdmObjects->islands->FindDirection(mtx.Pos(), gotoPos, dir);
		//wdmObjects->rs->DrawVector(mtx.Pos(), gotoPos, 0xFFFFFFFF);
		//wdmObjects->rs->DrawVector(mtx.Pos(), mtx.Pos()+dir*100.f, 0xFFFF0000);
	}else{
		dir.x = gotoPos.x - mtx.Pos().x;
		dir.z = gotoPos.z - mtx.Pos().z;
		float dl = dir.x*dir.x + dz*dz;
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

//Проверка на завершение
bool WdmMerchantShip::KillTest()
{
	float dx = gotoPos.x - mtx.Pos().x;
	float dz = gotoPos.z - mtx.Pos().z;
	float dl = dx*dx + dz*dz;
	return dl <= gotoRad*gotoRad;
}

//Обновление сохраняемых данных
void WdmMerchantShip::UpdateSaveData()
{
	if(!saveAttribute) return;
	WdmEnemyShip::UpdateSaveData();
	saveAttribute->SetAttributeUseFloat("gotoX", gotoPos.x);
	saveAttribute->SetAttributeUseFloat("gotoZ", gotoPos.z);
	saveAttribute->SetAttributeUseFloat("gotoRad", gotoRad);
	saveAttribute->SetAttributeUseDword("isEnableSetDir", isEnableSetDir);
}

//Установка параметров
void WdmMerchantShip::SetSaveAttribute(ATTRIBUTES * save)
{
	if(save)
	{
		gotoPos.x = save->GetAttributeAsFloat("gotoX", gotoPos.x);
		gotoPos.z = save->GetAttributeAsFloat("gotoZ", gotoPos.z);
		gotoRad = save->GetAttributeAsFloat("gotoRad", gotoRad);
		isEnableSetDir = save->GetAttributeAsDword("isEnableSetDir", isEnableSetDir) != 0;
	}
	WdmEnemyShip::SetSaveAttribute(save);
}





