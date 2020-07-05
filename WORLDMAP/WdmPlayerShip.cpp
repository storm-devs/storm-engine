//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmPlayerShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmPlayerShip.h"
#include "WdmStorm.h"
#include "WdmEnemyShip.h"
#include "WdmIcon.h"
#include "WdmIslands.h"
#include "WorldMap.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmPlayerShip::WdmPlayerShip()
{
	wdmObjects->playerShip = this;
	actionRadius = 16.0f;
	stormEventTime = 0.0f;
	goForward = false;

	drawCircle = true;
}

WdmPlayerShip::~WdmPlayerShip()
{
	wdmObjects->playerShip = null;
}

void WdmPlayerShip::PushOutFromIsland()
{
	//Если не в острове то и не выталкиваемся
	if(!wdmObjects->islands->CollisionTest(mtx, modelL05, modelW05, false))
	{
		return;
	}
	//Крутим по спирали вокруг точки
	float ang = 0.0f, angStep = PI*0.1f;
	float areaRad = 0.1f*0.707f*sqrtf(wdmObjects->worldSizeX*wdmObjects->worldSizeX + wdmObjects->worldSizeZ*wdmObjects->worldSizeZ);
	float x = mtx.Pos().x;
	float z = mtx.Pos().z;
	for(float r = 0.0f; r < areaRad; r += modelRadius*0.2f, ang += angStep)
	{
		if(ang > 2.0f*PI) ang -= 2.0f*PI;
		float _x = x + r*sinf(ang);
		float _z = z + r*cosf(ang);
		CMatrix m(0.0f, ay, 0.0f, _x, 0.0f, _z);
		if(!wdmObjects->islands->CollisionTest(m, modelL05, modelW05, false))
		{
			Teleport(_x, _z, ay);
			return;
		}
	}
	//Неполучилось, попробуем случайно подвигать
	for(long i = 0; i < 256; i++)
	{
		float _x = x + areaRad*rand()*1.0f/RAND_MAX;
		float _z = z + areaRad*rand()*1.0f/RAND_MAX;
		CMatrix m(0.0f, ay, 0.0f, _x, 0.0f, _z);
		if(!wdmObjects->islands->CollisionTest(m, modelL05, modelW05, false))
		{
			Teleport(_x, _z, ay);
			return;
		}
	}
}

void WdmPlayerShip::SetActionRadius(float radius)
{
	if(radius < 0.0f) radius = 0.0f;
	actionRadius = radius;
}

//Расчёты
void WdmPlayerShip::Update(float dltTime)
{
	WdmShip::Update(dltTime);
	Move(dltTime);
	//Тестируем попадание в нашу область энкоунтеров
	if(stormEventTime > 0.0f) stormEventTime -= dltTime;
	if(wdmObjects->isPause) return;
	//Шторм
	long i = TestInStorm();
	if(i >= 0)
	{		
		if(stormEventTime <= 0.0f)
		{
			stormEventTime = 0.5f;
			_CORE_API->Event("WorldMap_PlayerInStorm", "fffl", mtx.Pos().x, mtx.Pos().z, ay, i);
		}
	}
	wdmObjects->playarInStorm = (i == -2);
	//Корабли
	wdmObjects->enableSkipEnemy = false;
	for(i = 0; i < wdmObjects->numShips; i++)
	{
		//Пропустим ненужных
		WdmEnemyShip * es = ((WdmEnemyShip *)wdmObjects->ships[i]);
		if((WdmShip *)es == this || !es->isLive || es->killMe)
		{
			if(wdmObjects->enemyShip == es)
			{
				es->isEntryPlayer = false;
				wdmObjects->enemyShip = null;
			}
			continue;
		}
		//Дистанция до кораблика
		float r = ~(es->mtx.Pos() - mtx.Pos());
		//Определим радиус тестирования
		if(es->isEnemy)
		{
			if(r < actionRadius*actionRadius*6.0f)
			{
				if(r < actionRadius*actionRadius)
				{
					//Догнали
					//((WdmEnemyShip *)wdmObjects->ships[i])->isLive = false;
					wdmObjects->ships[i]->isSelect = true;
					if(es->attack) es->attack->isSelect = true;
					_CORE_API->Event("WorldMap_ShipEncounter", "fffl", mtx.Pos().x, mtx.Pos().z, ay, i);
				}else{
					if(!es->isEntryPlayer || (wdmObjects->enemyShip && !wdmObjects->enemyShip->isEnemy))
					{
						wdmObjects->enemyShip = es;
						es->isEntryPlayer = true;
					}
					if(wdmObjects->enemyShip && r < actionRadius*actionRadius*4.0f)
					{
						wdmObjects->enableSkipEnemy = ((WdmEnemyShip *)wdmObjects->ships[i])->canSkip;
					}
				}
			}else{				
				if(wdmObjects->enemyShip == es)
				{
					es->isEntryPlayer = false;
					wdmObjects->enemyShip = null;
					wdmObjects->enableSkipEnemy = false;
				}
			}
		}else{
			if(!wdmObjects->enemyShip || !wdmObjects->enemyShip->isEnemy)
			{
				if(r < actionRadius*actionRadius)
				{
					//if(!es->isEntryPlayer)
					if(wdmObjects->enemyShip != es)
					{
						es->isEntryPlayer = true;
						wdmObjects->enemyShip = es;
					}
				}else{					
					if(wdmObjects->enemyShip == es)
					{
						es->isEntryPlayer = false;
						wdmObjects->enemyShip = null;
					}
				}
			}
		}
	}
	if(wdmObjects->wm && wdmObjects->wm->AttributesPointer)
	{
		wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipX", mtx.Pos().x);
		wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipZ", mtx.Pos().z);
		wdmObjects->wm->AttributesPointer->SetAttributeUseFloat("playerShipAY", ay);
	}

	long nOldIslandVal = wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_island",0);
	long nOldEncounterType = wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_type",0);
	// отметим попадание в остров
	if( wdmObjects->curIsland ) {
		wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_island",1);
	} else {
		wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_island",0);
	}
	// отметим попадание в енкаунтер
	if( wdmObjects->enemyShip ) {
		switch(wdmObjects->enemyShip->shipType)
		{
		case wdmest_unknow:	wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",0); break;
		case wdmest_merchant: wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",1); break;
		case wdmest_warring: wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",2); break;
		case wdmest_follow: wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",3); break;
		default: wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",-1);
		}
	} else {
		// отметим попадание в шторм
		if( wdmObjects->playarInStorm ) {
			wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",4);
		} else {
			wdmObjects->wm->AttributesPointer->SetAttributeUseDword("encounter_type",-1);
		}
	}
	if( nOldIslandVal != wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_island",0) ||
		nOldEncounterType != wdmObjects->wm->AttributesPointer->GetAttributeAsDword("encounter_type",0) )
		api->Event("WM_UpdateCurrentAction");
}

void WdmPlayerShip::LRender(VDX8RENDER * rs)
{
	WdmShip::LRender(rs);
	if(wdmObjects->isDebug)
	{
		CMatrix mtx(CVECTOR(0.0f), mtx.Pos());
		wdmObjects->DrawCircle(mtx, actionRadius, 0x4f0000ff);
	}
}

bool WdmPlayerShip::ExitFromMap()
{
	//Ищем селектированные
	for(long i = 0, finded = -1; i < wdmObjects->numShips; i++)
	{
		wdmObjects->ships[i]->isSelect = false;
		if(wdmObjects->ships[i] == this) continue;
		if(!wdmObjects->ships[i]->isLive) continue;
		if(wdmObjects->ships[i]->killMe) continue;
		if(wdmObjects->ships[i] == wdmObjects->enemyShip)
		{
			finded = i;
			wdmObjects->ships[i]->isSelect = true;
			//wdmObjects->enemyShip->isLive = false;
			wdmObjects->enemyShip->isSelect = true;
		}
	}
	if(finded < 0) return false;
	if(wdmObjects->enemyShip->attack) wdmObjects->enemyShip->attack->isSelect = true;
	_CORE_API->Event("WorldMap_ShipEncounter", "fffl", mtx.Pos().x, mtx.Pos().z, ay, finded);
	return true;
}



long WdmPlayerShip::TestInStorm()
{
	bool inStormZone = false;
	bool isTornado = false;
	for(long i = 0; i < wdmObjects->numStorms; i++)
	{
		if(wdmObjects->storms[i]->killMe) continue;
		if(wdmObjects->storms[i]->CheckIntersection(mtx.Pos().x, mtx.Pos().z, actionRadius))
		{
			wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "1");
			if(wdmObjects->storms[i]->isTornado)
			{
				wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "1");
			}else{
				wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
			}
			wdmObjects->wm->AttributesPointer->SetAttribute("stormId", wdmObjects->storms[i]->GetID());
			return i;
		}
		float x, z;
		wdmObjects->storms[i]->GetPosition(x, z);
		float d = (mtx.Pos().x - x)*(mtx.Pos().x - x) + (mtx.Pos().z - z)*(mtx.Pos().z - z);
		if(d < wdmObjects->stormZone*wdmObjects->stormZone)
		{
			if(wdmObjects->storms[i]->IsActive())
			{
				inStormZone = true;
				if(wdmObjects->storms[i]->isTornado)
				{
					isTornado = true;
				}
			}
		}
	}
	if(inStormZone)
	{
		wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "1");
		if(isTornado)
		{
			wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "1");
		}else{
			wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
		}
		wdmObjects->wm->AttributesPointer->SetAttribute("stormId", "");
		return -2;
	}
	wdmObjects->wm->AttributesPointer->SetAttribute("playerInStorm", "0");
	wdmObjects->wm->AttributesPointer->SetAttribute("stormWhithTornado", "0");
	return -1;
}

//Переместить кораблик
void WdmPlayerShip::Move(float dltTime)
{
	CONTROL_STATE cs;
	//Вперёд
	_CORE_API->Controls->GetControlState("WMapShipSailUp", cs);
	if(cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED) goForward = true;
	_CORE_API->Controls->GetControlState("WMapShipSailUp1", cs);
	if(cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED) goForward = true;
	if(goForward) speed += WDM_SHIP_INER_ST*WDM_SHIP_MAX_SPEED*dltTime*0.5f;
	//Назад
	bool isBack = false;
	_CORE_API->Controls->GetControlState("WMapShipSailDown", cs);
	if(cs.state == CST_ACTIVE) isBack = true;
	_CORE_API->Controls->GetControlState("WMapShipSailDown1", cs);
	if(cs.state == CST_ACTIVE) isBack = true;
	if(isBack)
	{
		goForward = false;
		speed -= WDM_SHIP_INER_ST*WDM_SHIP_MAX_SPEED*dltTime*0.5f;
		if(speed < 0.0f)
		{
			speed = 0.0f;
		}
	}
	_CORE_API->Controls->GetControlState("WMapShipSailDown", cs);
	//Повороты
	bool isTurn = false;
	_CORE_API->Controls->GetControlState("WMapShipTurnLeft", cs);
	if(cs.state == CST_ACTIVE) isTurn = true;
	_CORE_API->Controls->GetControlState("WMapShipTurnLeft1", cs);
	if(cs.state == CST_ACTIVE) isTurn = true;
	if(isTurn) turnspd -= WDM_SHIP_INER_ST*WDM_SHIP_TSPEED*dltTime;
	isTurn = false;
	_CORE_API->Controls->GetControlState("WMapShipTurnRight", cs);
	if(cs.state == CST_ACTIVE) isTurn = true;
	_CORE_API->Controls->GetControlState("WMapShipTurnRight1", cs);
	if(cs.state == CST_ACTIVE) isTurn = true;
	if(isTurn) turnspd += WDM_SHIP_INER_ST*WDM_SHIP_TSPEED*dltTime;
}

void WdmPlayerShip::Collide()
{
	goForward = false;
}
