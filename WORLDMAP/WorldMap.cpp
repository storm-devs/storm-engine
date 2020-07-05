//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WorldMap
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "..\common_h\messages.h"

#include "WorldMap.h"

#include "WdmCameraStdCtrl.h"

#include "WdmSea.h"

#include "WdmRenderModel.h"

#include "WdmIslands.h"

#include "WdmClouds.h"
#include "WdmStorm.h"

#include "WdmPlayerShip.h"
#include "WdmFollowShip.h"
#include "WdmMerchantShip.h"
#include "WdmWarringShip.h"

#include "WdmDateLabel.h"
#include "WdmWindUI.h"
#include "WdmWindRose.h"
#include "WdmCounter.h"
#include "WdmEventWindow.h"
//#include "WaitMenu.h"
#include "WdmIcon.h"

//============================================================================================

//#define EVENTS_OFF
//#define ENCS_OFF

#define WDM_MAX_STORMS		4

long WorldMap::month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WorldMap::WorldMap()
{
	Assert(!wdmObjects);
	new WdmObjects();
	firstFreeObject = 0;
	firstObject = -1;
	firstPrObject = -1;
	firstMrObject = -1;
	firstLrObject = -1;
	for(long i = 0; i < WDMAP_MAXOBJECTS; i++) object[i].next = i + 1;
	object[WDMAP_MAXOBJECTS - 1].next = -1;
	wdmObjects->wm = this;
	camera = null;	
	srand(GetTickCount());
	encTime = 0.0f;
	aStorm = null;
	aEncounter = null;
	aInfo = null;
	saveData = null;
	eventWindow = null;
	timeScale = 1.0f;
	hour = 11.0f;
	day = 14;
	mon = 6;
	year = 1655;
	encCounter = 0;
}

WorldMap::~WorldMap()
{
	if(AttributesPointer)
	{
		AttributesPointer->SetAttribute("WindData", wdmObjects->GetWindSaveString(bufForSave));
	}
	//Оставим параметры энкоунтеров невредимыми
	for(long i = 0; i < wdmObjects->numShips; i++)
	{
		if(wdmObjects->ships[i] == wdmObjects->playerShip) continue;
		((WdmEnemyShip *)wdmObjects->ships[i])->SetSaveAttribute(null);
	}
	for(i = 0; i < wdmObjects->numStorms; i++)
	{
		wdmObjects->storms[i]->SetSaveAttribute(null);
	}
	//Карабль игрока
	if(wdmObjects->playerShip)
	{
		float x, z, ay;
		wdmObjects->playerShip->GetPosition(x, z, ay);
		AttributesPointer->CreateSubAClass(AttributesPointer, "playerShipX");
		AttributesPointer->CreateSubAClass(AttributesPointer, "playerShipZ");
		AttributesPointer->CreateSubAClass(AttributesPointer, "playerShipAY");
		AttributesPointer->SetAttributeUseFloat("playerShipX", x);
		AttributesPointer->SetAttributeUseFloat("playerShipZ", z);
		AttributesPointer->SetAttributeUseFloat("playerShipAY", ay);
	}
	//Камера
	if(wdmObjects->camera)
	{
		AttributesPointer->CreateSubAClass(AttributesPointer, "wdmCameraY");
		AttributesPointer->CreateSubAClass(AttributesPointer, "wdmCameraAY");
		AttributesPointer->SetAttributeUseFloat("wdmCameraY", wdmObjects->camera->pos.y);
		AttributesPointer->SetAttributeUseFloat("wdmCameraAY", wdmObjects->camera->ang.y);
	}
	ResetScriptInterfaces();
	/*for(; firstObject >= 0; firstObject = object[firstObject].next)
	{
		delete object[firstObject].ro;
	}*/
	if(camera) delete camera;
	WdmRenderObject::DeleteAllObjects();
	wdmObjects->Clear();

	delete wdmObjects;
}

//============================================================================================
//Entity
//============================================================================================

//Инициализация
bool WorldMap::Init()
{
	GUARD(LocationCamera::Init())
	//Layers
	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetExecute("execute", true);	
	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetRealize("realize", true);
	_CORE_API->LayerAdd("execute", GetID(), 10000);
	_CORE_API->LayerAdd("realize", GetID(), 10000);

	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");
	rs->SetPerspective((1.57f + 1.0f)/2);
	wdmObjects->rs = rs;
	//GS
	wdmObjects->gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	//Создаём объекты карты
	WdmRenderObject * ro;
	//Создаём острова
	ro = AddObject(NEW WdmIslands());
	AddObject(ro, -100000);
	AddLObject(ro, 500);
	//
	rs->ProgressView();
	//Создаём море
	WdmSea * sea = NEW WdmSea();
	AddObject(sea);
	AddPObject(sea, 10);
	AddLObject(sea, -1);
	//Создаём облака
	AddLObject(AddObject(NEW WdmClouds()), 10000);
	rs->ProgressView();
	//Создаём камеру
	camera = NEW WdmCameraStdCtrl();
	float camAy = 0.0f;
	float camH = -1.0f;	
	bool camLock = false;
	if(AttributesPointer)
	{		
		camAy = AttributesPointer->GetAttributeAsFloat("wdmCameraAY", camAy);
		camH = AttributesPointer->GetAttributeAsFloat("wdmCameraY", camH);
		camLock = AttributesPointer->GetAttributeAsDword("wdmCameraRotLock", false) != 0;
		wdmObjects->SetWindSaveString(AttributesPointer->GetAttribute("WindData"));
		wdmObjects->shipSpeedOppositeWind = AttributesPointer->GetAttributeAsFloat("shipSpeedOppositeWind", wdmObjects->shipSpeedOppositeWind);
		wdmObjects->shipSpeedOverWind = AttributesPointer->GetAttributeAsFloat("shipSpeedOverWind", wdmObjects->shipSpeedOverWind);
		
	}else{
		wdmObjects->SetWindSaveString(null);
	}
	camera->Init(camAy, camH);
	camera->lock = camLock;
	//Создаём корабль игрока
	ro = CreateModel(NEW WdmPlayerShip(), "Ship"); 
	Assert(ro);
	AddLObject(ro, 100);
	float psX = 50.0f;
	float psZ = 0.0f;
	float psAy = 0.0f;
	float psRad = 16.0f;
	if(AttributesPointer)
	{
		psX = AttributesPointer->GetAttributeAsFloat("playerShipX", psX);
		psZ = AttributesPointer->GetAttributeAsFloat("playerShipZ", psZ);
		psAy = AttributesPointer->GetAttributeAsFloat("playerShipAY", psAy);
		psRad = AttributesPointer->GetAttributeAsFloat("playerShipActionRadius", psRad);
		wdmObjects->enemyshipViewDistMin = AttributesPointer->GetAttributeAsFloat("enemyshipViewDistMin", wdmObjects->enemyshipViewDistMin);
		wdmObjects->enemyshipViewDistMax = AttributesPointer->GetAttributeAsFloat("enemyshipViewDistMax", wdmObjects->enemyshipViewDistMax);
		wdmObjects->enemyshipDistKill = AttributesPointer->GetAttributeAsFloat("enemyshipDistKill", wdmObjects->enemyshipDistKill);
		wdmObjects->enemyshipBrnDistMin = AttributesPointer->GetAttributeAsFloat("enemyshipBrnDistMin", wdmObjects->enemyshipBrnDistMin);
		wdmObjects->enemyshipBrnDistMax = AttributesPointer->GetAttributeAsFloat("enemyshipBrnDistMax", wdmObjects->enemyshipBrnDistMax);
		wdmObjects->stormViewDistMin = AttributesPointer->GetAttributeAsFloat("stormViewDistMin", wdmObjects->stormViewDistMin);
		wdmObjects->stormViewDistMax = AttributesPointer->GetAttributeAsFloat("stormViewDistMax", wdmObjects->stormViewDistMax);
		wdmObjects->stormDistKill = AttributesPointer->GetAttributeAsFloat("stormDistKill", wdmObjects->stormDistKill);
		wdmObjects->stormBrnDistMin = AttributesPointer->GetAttributeAsFloat("stormBrnDistMin", wdmObjects->stormBrnDistMin);
		wdmObjects->stormBrnDistMax = AttributesPointer->GetAttributeAsFloat("stormBrnDistMax", wdmObjects->stormBrnDistMax);
		wdmObjects->stormZone = AttributesPointer->GetAttributeAsFloat("stormZone", wdmObjects->stormZone);
		char * s = AttributesPointer->GetAttribute("debug");
		wdmObjects->isDebug = s && (stricmp(s, "true") == 0);
		saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "encounters");
	}	
	((WdmShip *)ro)->Teleport(psX, psZ, psAy);
	((WdmPlayerShip *)ro)->SetActionRadius(psRad);
	rs->ProgressView();
	//Создаём описатель локаций
	wdmObjects->islands->SetIslandsData(AttributesPointer, false);
	//Атрибуты интерфейса со скриптом
	if(AttributesPointer)
	{
		//Storms interface
		AttributesPointer->CreateSubAClass(AttributesPointer, "storm.num");
		AttributesPointer->CreateSubAClass(AttributesPointer, "storm.cur");
		AttributesPointer->CreateSubAClass(AttributesPointer, "storm.x");
		AttributesPointer->CreateSubAClass(AttributesPointer, "storm.z");
		AttributesPointer->CreateSubAClass(AttributesPointer, "storm.time");
		aStorm = AttributesPointer->FindAClass(AttributesPointer, "storm");
		//Ship encounters inderface
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.num");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.cur");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.x");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.z");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.ay");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.time");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.type");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.attack");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.nation");
		AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.id");
		aEncounter = AttributesPointer->FindAClass(AttributesPointer, "encounter");
		//Info
		AttributesPointer->CreateSubAClass(AttributesPointer, "info.playerInStorm");
		AttributesPointer->CreateSubAClass(AttributesPointer, "info.updateinfo");
		aInfo = AttributesPointer->FindAClass(AttributesPointer, "info");
		//Дата	
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.sec");
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.min");
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.hour");
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.day");
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.month");
		AttributesPointer->CreateSubAClass(AttributesPointer, "date.year");
		aDate = AttributesPointer->FindAClass(AttributesPointer, "date");
	}
	if(aDate)
	{
		long sec = aDate->GetAttributeAsDword("sec", 1);
		long min = aDate->GetAttributeAsDword("min", 1);
		hour = float(aDate->GetAttributeAsDword("hour", long(hour)));
		hour += (min + sec/60.0f)/60.0f;
		day = aDate->GetAttributeAsDword("day", day);
		mon = aDate->GetAttributeAsDword("month", mon);
		year = aDate->GetAttributeAsDword("year", year);
		timeScale = aDate->GetAttributeAsFloat("hourPerSec", timeScale);
	}
	ResetScriptInterfaces();
	rs->ProgressView();
	//Создаём элементы интерфейса
	
	//Дата
	WdmWindUI * windUI = NEW WdmWindUI();
	windUI->SetAttributes(AttributesPointer);
	AddLObject(AddObject(windUI, 1001), 10100);

	//Компас
	//ro = CreateModel(NEW WdmWindRose(), "WindRose");
	//AddLObject(ro, 10099);
	//Календарь
	//WdmCounter * cnt = NEW WdmCounter();
	//if(!cnt->Init()) _CORE_API->Trace("Counter not created");
	
	//AddLObject(cnt, 10099);
	//Иконка
	AddLObject(AddObject(NEW WdmIcon(), 1000), 10099);
	
	eventWindow = NEW WdmEventWindow();
	AddLObject(AddObject(eventWindow, 1001), 10100);

	//Загружаем энкоунтеры, если таковы были
	if(saveData)
	{
		dword num = saveData->GetAttributesNum();
		for(dword i = 0; i < num; i++)
		{
			ATTRIBUTES * a = saveData->GetAttributeClass(i);
			if(!a) continue;
			const char * type = a->GetAttribute("type");
			const char * modelName = a->GetAttribute("modelName");
			if(!type || !type[0])
			{
				saveData->DeleteAttributeClassX(a);
				continue;
			}
			if(stricmp(type, "Merchant") == 0 && modelName && modelName[0])
			{
				if(!CreateMerchantShip(modelName, null, null, 1.0f, -1.0f, a))
				{
					api->Trace("WoldMap: not loaded merchant encounter.");
				}
				continue;
			}else
			if(stricmp(type, "Follow") == 0 && modelName && modelName[0])
			{
				if(!CreateFollowShip(modelName, 1.0f, -1.0f, a))
				{
					api->Trace("WoldMap: not loaded follow encounter.");
				}
				continue;
			}else
			if(stricmp(type, "Warring") == 0 && modelName && modelName[0])
			{
				char * attacked = a->GetAttribute("attacked");
				if(attacked)
				{
					ATTRIBUTES * a1 = saveData->FindAClass(saveData, attacked);
					if(a1)
					{
						char * modelName1 = a1->GetAttribute("modelName");
						if(modelName1 || modelName1[0])
						{
							if(!CreateWarringShips(modelName, modelName1, -1.0f, a, a1))
							{
								api->Trace("WoldMap: not loaded warring encounter.");
							}
						}else{
							api->Trace("WoldMap: not loaded warring encounter.");
							saveData->DeleteAttributeClassX(a);
							saveData->DeleteAttributeClassX(a1);
						}
					}
				}
				continue;
			}else
			if(stricmp(type, "Attacked") == 0)
			{
				continue;
			}else
			if(stricmp(type, "Storm") == 0)
			{
				bool isTornado = (a->GetAttributeAsDword("isTornado", 0) != 0);
				if(!CreateStorm(isTornado, -1.0f, a))
				{
					api->Trace("WoldMap: not loaded storm encounter.");
				}
				continue;
			}
			saveData->DeleteAttributeClassX(a);
		}
	}

	rs->ProgressView();


	//Корректируем корабль игрока
	WdmPlayerShip * playerShip = (WdmPlayerShip *)wdmObjects->playerShip;
	playerShip->PushOutFromIsland();
	ATTRIBUTES * atrData = AttributesPointer->FindAClass(AttributesPointer, "island");
	if(atrData)
	{
		float x, z, ay;
		playerShip->GetPosition(x, z, ay);
		if(!wdmObjects->islands->CheckIslandArea(atrData->GetThisAttr(), x, z))
		{
			wdmObjects->islands->GetNearPointToArea(atrData->GetThisAttr(), x, z);
			playerShip->Teleport(x, z, ay);
		}
	}
	return true;
	UNGUARD
}

//Исполнение
void WorldMap::Execute(dword delta_time)
{
}

void WorldMap::Realize(dword delta_time)
{
	if(AttributesPointer)
	{
		CVECTOR wind(0.0f);
		float x, z, ay;
		wdmObjects->playerShip->GetPosition(x, z, ay);
		float force = wdmObjects->GetWind(x, z, wind);
		AttributesPointer->SetAttributeUseFloat("WindX", wind.x);
		AttributesPointer->SetAttributeUseFloat("WindZ", wind.z);
		AttributesPointer->SetAttributeUseFloat("WindF", force);
	}
	if(!wdmObjects->isPause)
	{
		CONTROL_STATE cs;
		_CORE_API->Controls->GetControlState("WMapCancel",cs);
		if(cs.state == CST_ACTIVATED)
		{
			if(wdmObjects->playerShip)
			{
				if(!((WdmPlayerShip *)wdmObjects->playerShip)->ExitFromMap()) _CORE_API->Event("ExitFromWorldMap");
			}else _CORE_API->Event("ExitFromWorldMap");
		}
	}
	//---------------------------------------------------------
	float dltTime = 0.001f*delta_time;
	//Обновляем дату
	if(hour < 0.0f) hour = 0.0f;
	hour += dltTime*timeScale;
	long days = long(hour/24.0f);
	hour = (hour/24.0f - days)*24.0f;
	long dtHour = long(hour);
	long dtMin = long((hour - dtHour)*60.0f);
	long dtSec = long(((hour - dtHour)*60.0f - dtMin)*60.0f);
	aDate->SetAttributeUseDword("sec", dtSec);
	aDate->SetAttributeUseDword("min", dtMin);
	aDate->SetAttributeUseDword("hour", dtHour);
	if(days)
	{
		for(; days > 0; days--)
		{
			day++;
			if(day > month[mon])
			{
				day -= month[mon++];
				if(mon > 12)
				{
					mon = 1;
					year++;
					aDate->SetAttributeUseDword("year", year);
				}
				aDate->SetAttributeUseDword("month", mon);
			}
			aDate->SetAttributeUseDword("day", day);
			
#ifndef EVENTS_OFF
			_CORE_API->Event("WorldMap_UpdateDate", "f", hour);
			wdmObjects->isNextDayUpdate = true;
			_CORE_API->Event("NextDay");
#endif
		}
	}else{
#ifndef EVENTS_OFF
		_CORE_API->Event("WorldMap_UpdateDate", "f", hour);
#endif
	}
	//
	char * tmp = aDate->GetAttribute("sec"); if(tmp) strcpy(wdmObjects->attrSec, tmp);
	tmp = aDate->GetAttribute("min"); if(tmp) strcpy(wdmObjects->attrMin, tmp);
	tmp = aDate->GetAttribute("hour"); if(tmp) strcpy(wdmObjects->attrHour, tmp);
	tmp = aDate->GetAttribute("day"); if(tmp) strcpy(wdmObjects->attrDay, tmp);
	tmp = aDate->GetAttribute("month"); if(tmp) strcpy(wdmObjects->attrMonth, tmp);
	tmp = aDate->GetAttribute("year"); if(tmp) strcpy(wdmObjects->attrYear, tmp);
	//---------------------------------------------------------
	if(camera && !wdmObjects->isPause) camera->Move(dltTime, rs);
	bool isKill = false;
	//Исполним все объекты
	for(long i = firstObject; i >= 0; i = object[i].next)
	{
		if(!object[i].ro->killMe) object[i].ro->Update(object[i].ro->isEnablePause && wdmObjects->isPause ? 0.0f : dltTime);
		isKill |= object[i].ro->killMe;
	}
	//Удалим объекты если надо
	if(isKill)
	{
		for(i = firstObject; i >= 0; )
			if(object[i].ro->killMe)
			{			
				DeleteObject(object[i].ro);
				i = firstObject;
			}else i = object[i].next;
	}
	//Текущее количество событий
	if(aStorm) aStorm->SetAttributeUseDword("num", wdmObjects->numStorms);
	if(aEncounter) aEncounter->SetAttributeUseDword("num", wdmObjects->numShips - (wdmObjects->playerShip != 0));
	//События
	encTime += dltTime;
	if(encTime >= 1.0f && wdmObjects->playerShip && !wdmObjects->isPause)
	{
		float psx = 0.0f, psz = 0.0f, psay = 0.0f;
		wdmObjects->playerShip->GetPosition(psx, psz, psay);
#ifndef ENCS_OFF
		_CORE_API->Event("WorldMap_EncounterCreate", "ffff", encTime, psx, psz, psay);
#endif
		encTime = 0.0f;
	}
	rs->SetRenderState(D3DRS_FOGENABLE, FALSE);
	rs->SetRenderState(D3DRS_LIGHTING, FALSE);
	for(i = firstPrObject; i >= 0; i = object[i].next)
	{
		if(!object[i].ro->killMe) object[i].ro->PRender(rs);
	}
	for(i = firstMrObject; i >= 0; i = object[i].next)
	{
		if(!object[i].ro->killMe) object[i].ro->MRender(rs);
	}
	for(i = firstLrObject; i >= 0; i = object[i].next)
	{
		if(!object[i].ro->killMe) object[i].ro->LRender(rs);
	}
	//Обновим ветер
	wdmObjects->UpdateWind(dltTime);
	wdmObjects->isNextDayUpdate = false;
	//Проверим атрибут обновления энкоунтера
	if(AttributesPointer)
	{
		const char * upd = AttributesPointer->GetAttribute("addQuestEncounters");
		if(upd && upd[0] != 0)
		{
			api->Event("WorldMap_AddQuestEncounters", null);
		}
	}	
}

//Сообщения
dword _cdecl WorldMap::ProcessMessage(MESSAGE & message)
{
	char buf[256];
	char sName[256];
	char sName2[256];
	switch(message.Long())
	{
	case MSG_WORLDMAP_CREATESTORM:
		{
			bool isTornado = message.Long() != 0;
			CreateStorm(isTornado);
		}		
		break;
	case MSG_WORLDMAP_CREATEENC_MER:
		{
			message.String(sizeof(sName), sName);
			message.String(sizeof(buf), buf);
			message.String(sizeof(sName2), sName2);
			float kSpeed = message.Float();
			float timeOut = message.Float();  //boal
			return CreateMerchantShip(sName, buf, sName2, kSpeed, timeOut);  //boal
		}
		break;
	// boal 04/01/06 -->
	case MSG_WORLDMAP_CREATEENC_MER_XZ:
		{
			message.String(sizeof(sName), sName);
			float fx1 = message.Float();
			float fz1 = message.Float();
			float fx2 = message.Float();
			float fz2 = message.Float();
			float kSpeed = message.Float();
			float timeOut = message.Float();  
			return CreateMerchantShipXZ(sName, fx1, fz1, fx2, fz2, kSpeed, timeOut);  
		}
		break;
	// boal <--				
	case MSG_WORLDMAP_CREATEENC_FLW:
		{
			message.String(sizeof(sName), sName);
			float kSpeed = message.Float();
			float timeOut = message.Float();
			return CreateFollowShip(sName, kSpeed, timeOut);
		}
		break;
	case MSG_WORLDMAP_CREATEENC_WAR:
		{
			message.String(sizeof(sName), sName);
			message.String(sizeof(sName), sName2);
			float timeOut = message.Float();
			return CreateWarringShips(sName, sName2, timeOut);
		}
		break;
	case MSG_WORLDMAP_CREATEENC_RELEASE:
		ReleaseEncounters();
		break;
	case MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA:
		if(wdmObjects->playerShip)
		{
			if(!((WdmPlayerShip *)wdmObjects->playerShip)->ExitFromMap()) _CORE_API->Event("ExitFromWorldMap");
		}else _CORE_API->Event("ExitFromWorldMap");
		break;
	}
	return 0;
}

//Изменение атрибута
dword WorldMap::AttributeChanged(ATTRIBUTES * apnt)
{
	float x, z, ay;
	if(!apnt || !AttributesPointer) return 0;
	if(stricmp(apnt->GetThisName(), "deleteUpdate") == 0)
	{
		for(long i = 0; i < wdmObjects->numShips; i++)
		{
			if(wdmObjects->ships[i] == wdmObjects->playerShip) continue;			
			((WdmEnemyShip *)wdmObjects->ships[i])->DeleteUpdate();
		}
		for(long i = 0; i < wdmObjects->numStorms; i++)
		{
			wdmObjects->storms[i]->DeleteUpdate();
		}
	}else
	if(stricmp(apnt->GetThisName(), "playerShipUpdate") == 0)
	{
		if(wdmObjects->playerShip)
		{
			float x, z, ay;
			wdmObjects->playerShip->GetPosition(x, z, ay);
			AttributesPointer->SetAttributeUseFloat("playerShipX", x);
			AttributesPointer->SetAttributeUseFloat("playerShipZ", z);
			AttributesPointer->SetAttributeUseFloat("playerShipAY", ay);
		}
	}else
	if(stricmp(apnt->GetThisName(), "update") == 0)
	{
		ATTRIBUTES * pa = apnt->GetParent();
		if(pa && *pa == "eventWindow")
		{
			eventWindow->ActivateEvent(pa->GetAttribute("text"), pa->GetAttribute("yes"), pa->GetAttribute("no"));
			return 0;
		}
	}else
	if(stricmp(apnt->GetThisName(), "cur") == 0)
	{
		ATTRIBUTES * pa = apnt->GetParent();
		if(pa == aStorm)
		{
			long cur = long(pa->GetAttributeAsDword("cur"));
			if(cur >= 0 && cur < wdmObjects->numStorms)
			{
				Assert(wdmObjects->storms[cur]);
				wdmObjects->storms[cur]->GetPosition(x, z);
				pa->SetAttributeUseFloat("x", x);
				pa->SetAttributeUseFloat("z", z);
				pa->SetAttributeUseFloat("time", wdmObjects->storms[cur]->GetLiveTime());
			}else{
				pa->SetAttributeUseDword("cur", -1);
			}
		}else
		if(pa == aEncounter)
		{
			long cur = long(pa->GetAttributeAsDword("cur"));
			//Определим индекс энкоунтера
			for(long enc = 0, i = 0; i < wdmObjects->numShips; i++)
			{
				if(wdmObjects->ships[i] == wdmObjects->playerShip) continue;
				if(enc == cur) break;
				enc++;
			}			
			if(i >= 0 && i < wdmObjects->numShips)
			{
				Assert(wdmObjects->ships[i]);
				wdmObjects->ships[i]->GetPosition(x, z, ay);
				pa->SetAttributeUseFloat("x", x);
				pa->SetAttributeUseFloat("z", z);
				pa->SetAttributeUseFloat("ay", ay);
				WdmEnemyShip * es = (WdmEnemyShip *)wdmObjects->ships[i];
				pa->SetAttributeUseFloat("time", es->GetLiveTime());
				char buf[32];
				sprintf(buf, "%i", es->type);
				pa->SetAttribute("type", buf);
				pa->SetAttributeUseDword("select", es->isSelect);
				pa->SetAttribute("id", (char *)((WdmEnemyShip *)wdmObjects->ships[i])->GetAttributeName());
				//Если есть атакующий, определим его индекс				
				if(es->attack)
				{
					Assert(es->attack != es);
					for(long i = 0, j = 0; i < wdmObjects->numShips; i++)
					{
						if(wdmObjects->ships[i] == wdmObjects->playerShip) continue;
						if(wdmObjects->ships[i] == es->attack) break;
						j++;						
					}
					if(i >= wdmObjects->numShips) j = -1;
					pa->SetAttributeUseDword("attack", j);
				}else{
					pa->SetAttributeUseDword("attack", -1);
				}
			}else{
				pa->SetAttributeUseDword("cur", -1);
			}
		}
	}else
	if(stricmp(apnt->GetThisName(), "updateinfo") == 0)
	{
		ATTRIBUTES * pa = apnt->GetParent();
		if(pa == aInfo)
		{
			pa->SetAttributeUseDword("playerInStorm", long(wdmObjects->playarInStorm));
		}
	}else{

		for(ATTRIBUTES * pa = apnt; pa; pa = pa->GetParent())
		{
			if(stricmp(pa->GetThisName(), "labels") == 0)
			{
				wdmObjects->islands->SetIslandsData(AttributesPointer, true);
				return 0;
			}
		}
	}
	return 0;
}

//============================================================================================
//Управление объектами
//============================================================================================

//Добавить объект
WdmRenderObject * WorldMap::AddObject(WdmRenderObject * obj, long level)
{
	if(!obj) return null;
	long i = GetObject(firstObject, level);
	object[i].ro = obj;
	return obj;
}

//Добавить объект в список отрисовки до отражения
void WorldMap::AddPObject(WdmRenderObject * obj, long level)
{
	if(!obj) return;
	long i = GetObject(firstPrObject, level);
	object[i].ro = obj;
}

//Добавить объект в список отрисовки отражения
void WorldMap::AddMObject(WdmRenderObject * obj, long level)
{
	if(!obj) return;
	long i = GetObject(firstMrObject, level);
	object[i].ro = obj;
}

//Добавить объект в список отрисовки после отражения
void WorldMap::AddLObject(WdmRenderObject * obj, long level)
{
	if(!obj) return;
	long i = GetObject(firstLrObject, level);
	object[i].ro = obj;
}

//Удалить объект
void WorldMap::DeleteObject(WdmRenderObject * obj)
{
	if(!obj) return;
	//Проходимся по всем спискам, удаляя запись об объекте
	for(long i = firstObject, j; i >= 0;)
	{
		j = i; i = object[i].next;
		if(object[j].ro == obj) FreeObject(firstObject, j);
	}
	for(i = firstPrObject; i >= 0;)
	{		
		j = i; i = object[i].next;
		if(object[j].ro == obj) FreeObject(firstPrObject, j);
	}

	for(i = firstMrObject; i >= 0;)
	{		
		j = i; i = object[i].next;
		if(object[j].ro == obj) FreeObject(firstMrObject, j);
	}
	for(i = firstLrObject; i >= 0;)
	{		
		j = i; i = object[i].next;
		if(object[j].ro == obj) FreeObject(firstLrObject, j);
	}
	delete obj;
}


//============================================================================================
//Инкапсуляция
//============================================================================================

//Управление объектами

//Включить запись об объекте в список с необходимым уровнем
long WorldMap::GetObject(long & first, long level)
{
	Assert(firstFreeObject >= 0);
	long i = firstFreeObject;
	firstFreeObject = object[firstFreeObject].next;
	object[i].ro = null;
	object[i].level = level;
	object[i].prev = -1;
	object[i].next = -1;
	if(first >= 0)
	{
		if(level >= object[first].level)
		{
			for(long j = first; object[j].next >= 0 && level >= object[object[j].next].level; j = object[j].next);
			object[i].prev = j;
			object[i].next = object[j].next;
			object[j].next = i;
			if(object[i].next >= 0) object[object[i].next].prev = i;
		}else{
			object[i].next = first;
			first = i;
		}

	}else first = i;	
	return i;	
}

//Исключить запись из списка
void WorldMap::FreeObject(long & first, long i)
{
	Assert(i >= 0.0f && i < WDMAP_MAXOBJECTS);
	object[i].ro = null;
	object[i].level = 0;
	if(object[i].next >= 0) object[object[i].next].prev = object[i].prev;
	if(object[i].prev >= 0) object[object[i].prev].next = object[i].next;
		else first = object[i].next;	
	object[i].prev = -1;
	object[i].next = firstFreeObject;
	firstFreeObject = i;
}

//Утилитные

//Проинициализировать модельку и занести в нужные списки рендера
WdmRenderObject * WorldMap::CreateModel(WdmRenderModel * rm, const char * modelName, bool pr, bool mr, bool lr, long objectLevel, long drawLevel)
{
	if(!modelName || !modelName[0]) 
	{
		delete rm;  // boal fix нужно тереть
		return null;
	}
	if(!rm->Load(modelName))
	{
		delete rm;
		return null;
	}
	AddObject(rm, objectLevel);
	if(pr) AddPObject(rm, drawLevel);
	if(mr) AddMObject(rm, drawLevel);
	if(lr) AddLObject(rm, drawLevel);
	return rm;
}

//Создать шторм, если это возможно, и установить время жизни
bool WorldMap::CreateStorm(bool isTornado, float time, ATTRIBUTES * save)
{
	if(wdmObjects->numStorms >= WDM_MAX_STORMS) return false;
	WdmStorm * s = NEW WdmStorm();
	AddLObject(s, 800);
	if(!AddObject(s)) return false;
	if(time > 0.0f)
	{	
		if(time < 1.0f) time = 1.0f;
		s->SetLiveTime(time);
	}
	if(!save) save = GetEncSaveData("Storm", "EncounterID1");
	s->SetSaveAttribute(save);
	s->isTornado = isTornado;
	return true;
}

//Создать кораблик купца
bool WorldMap::CreateMerchantShip(const char * modelName, const char * locNameStart, const char * locNameEnd, float kSpeed, float time, ATTRIBUTES * save)
{
	if(kSpeed < 0.1f) kSpeed = 0.1f;
	WdmShip * ship = NEW WdmMerchantShip();
	if(ship->killMe) return false;
	if(!CreateModel(ship, modelName)) return false;
 	AddLObject(ship, 100);
	//Ищем место куда плыть
	if(!wdmObjects->islands)
	{
		api->Trace("World map: Islands not found");
	}
	CVECTOR gpos;
	if(!locNameEnd || !locNameEnd[0])
	{
		if(wdmObjects->islands)
		{
			if(!wdmObjects->islands->GetRandomMerchantPoint(gpos))
			{
				api->Trace("World map: Locators <Merchants:...> not found");
			}
		}
	}else{
		if(wdmObjects->islands)
		{
			if(!wdmObjects->islands->GetQuestLocator(locNameEnd, gpos))
			{
				api->Trace("World map: Quest locator <Quests:%s> for merchant not found", locNameEnd);
			}
		}
	}
	((WdmMerchantShip *)ship)->Goto(gpos.x, gpos.z, 2.0f);
	//Если надо, изменяем текущую позицию
	if(locNameStart && locNameStart[0])
	{
		if(wdmObjects->islands)
		{
			if(wdmObjects->islands->GetQuestLocator(locNameStart, gpos))
			{
				ship->Teleport(gpos.x, gpos.z, rand()*(PI*2.0f/RAND_MAX));
			}else{
				api->Trace("World map: Quest locator <Quests:%s> for merchant not found", locNameStart);  // boal fix
			}
		}
	}	
	//Скорость
	ship->SetMaxSpeed(kSpeed);
	//Время жизни
	if(time >= 0.0f)
	{
		if(time < 3.0f) time = 3.0f;
		((WdmEnemyShip *)ship)->SetLiveTime(time);
	}
	if(!save) save = GetEncSaveData("Merchant", "EncounterID1");
	if(save)
	{
		save->SetAttribute("modelName", (char *)modelName);
	}
	((WdmEnemyShip *)ship)->SetSaveAttribute(save);
	return true;
}

// boal Создать кораблик купца в координатах
bool WorldMap::CreateMerchantShipXZ(const char * modelName, float x1, float z1, float x2, float z2, float kSpeed, float time, ATTRIBUTES * save)
{
	if(kSpeed < 0.1f) kSpeed = 0.1f;
	WdmShip * ship = NEW WdmMerchantShip();
	if(ship->killMe) return false;
	if(!CreateModel(ship, modelName)) return false;
 	AddLObject(ship, 100);
	//Ищем место куда плыть
	if(!wdmObjects->islands)
	{
		api->Trace("World map: Islands not found");
	}
	
	((WdmMerchantShip *)ship)->Goto(x2, z2, 2.0f);   // куда
	//Если надо, изменяем текущую позицию
	ship->Teleport(x1, z1, rand()*(PI*2.0f/RAND_MAX)); // откуда
	//Скорость
	ship->SetMaxSpeed(kSpeed);
	//Время жизни
	if(time >= 0.0f)
	{
		if(time < 3.0f) time = 3.0f;
		((WdmEnemyShip *)ship)->SetLiveTime(time);
	}
	if(!save) save = GetEncSaveData("Merchant", "EncounterID1");
	if(save)
	{
		save->SetAttribute("modelName", (char *)modelName);
	}
	((WdmEnemyShip *)ship)->SetSaveAttribute(save);
	return true;
}

//Создать кораблик преследующий нас
bool WorldMap::CreateFollowShip(const char * modelName, float kSpeed, float time, ATTRIBUTES * save)
{
	if(kSpeed < 0.1f) kSpeed = 0.1f;
	WdmShip * ship = NEW WdmFollowShip();
	if(ship->killMe) return false;
	if(!CreateModel(ship, modelName)) return false;
	AddLObject(ship, 100);
	//Скорость
	ship->SetMaxSpeed(kSpeed);
	//Время жизни
	if(time >= 0.0f)
	{
		if(time < 1.0f) time = 1.0f;
		((WdmEnemyShip *)ship)->SetLiveTime(time);
	}
	VDATA * isSkipEnable = api->Event("WorldMap_IsSkipEnable");
	if(isSkipEnable)
	{
		long skipEnable = 0;
		if(isSkipEnable->Get(skipEnable))
		{
			((WdmEnemyShip *)ship)->canSkip = skipEnable != 0;
		}
	}
	if(!save) save = GetEncSaveData("Follow", "EncounterID1");
	if(save)
	{
		save->SetAttribute("modelName", (char *)modelName);
	}
	((WdmEnemyShip *)ship)->SetSaveAttribute(save);
	return true;
}

bool WorldMap::CreateWarringShips(const char * modelName1, const char * modelName2, float time, ATTRIBUTES * save1, ATTRIBUTES * save2)
{
	static const float pi = 3.14159265359f;
	//Создаём кораблики
	WdmWarringShip * ship1 = NEW WdmWarringShip();
	if(ship1->killMe) return false;	
	if(!CreateModel(ship1, modelName1)) return false;
	WdmWarringShip * ship2 = NEW WdmWarringShip();
	if(ship2->killMe) return false;	
	if(!CreateModel(ship2, modelName2)) return false;
	float moveRadius = (ship1->modelRadius + ship2->modelRadius)*(0.4f + (rand() & 3)*(0.1f/3.0f));
	float fullRadius = 0.6f*(moveRadius + 2.0f*max(ship1->modelRadius, ship2->modelRadius));
	//Общая позиция
	float x, z;
	if(!WdmEnemyShip::GeneratePosition(fullRadius, 1.5f, x, z)) return false;
	//Общий угол
	float angl = rand()*2.0f*pi/(RAND_MAX + 1);
	//Смещение относительно центра
	float dx = moveRadius*cosf(angl);
	float dz = -moveRadius*sinf(angl);
	AddLObject(ship1, 100);
	ship1->Teleport(x + dx, z + dz, angl + pi*(rand() & 1));
	AddLObject(ship2, 100);
	ship2->Teleport(x - dx, z - dz, angl + pi*(rand() & 1));
	ship2->SetLiveTime(ship1->GetLiveTime());
	//Устанавливаем характеристики
	ship1->attack = ship2;
	ship2->attack = ship1;
	//Время жизни
	if(time >= 0.0f)
	{
		if(time < 1.0f) time = 1.0f;
		ship1->SetLiveTime(time);
		ship2->SetLiveTime(time);
	}
	if(!save2) save2 = GetEncSaveData("Attacked", "EncounterID1");
	if(!save1) save1 = GetEncSaveData("Warring", "EncounterID2");	
	if(save1 && save2)
	{
		save1->SetAttribute("attacked", save2->GetThisName());
	}
	if(save1)
	{
		save1->SetAttribute("modelName", (char *)modelName1);
	}
	if(save2)
	{
		save2->SetAttribute("modelName", (char *)modelName2);
	}
	ship1->SetSaveAttribute(save1);
	ship2->SetSaveAttribute(save2);
	return true;
}

//Найти координаты и радиус по месту назначения
bool WorldMap::FindIslandPosition(const char * name, float & x, float & z, float & r)
{
	return false;
}

void WorldMap::ResetScriptInterfaces()
{
	if(aStorm)
	{
		aStorm->SetAttributeUseDword("num", 0);
		aStorm->SetAttributeUseDword("cur", 0);
		aStorm->SetAttributeUseDword("x", 0);
		aStorm->SetAttributeUseDword("z", 0);
		aStorm->SetAttributeUseDword("time", 0);
	}
	if(aEncounter)
	{
		aEncounter->SetAttributeUseDword("num", 0);
		aEncounter->SetAttributeUseDword("cur", 0);
		aEncounter->SetAttributeUseDword("x", 0);
		aEncounter->SetAttributeUseDword("z", 0);
		aEncounter->SetAttributeUseDword("ay", 0);
		aEncounter->SetAttributeUseDword("time", 0);
		aEncounter->SetAttribute("type", "-1");
		aEncounter->SetAttributeUseDword("attack", -1);
		aEncounter->SetAttributeUseDword("nation", -1);
		aEncounter->SetAttribute("id", "");
	}
	if(aInfo)
	{
		aInfo->SetAttributeUseDword("playerInStorm", 0);
	}
}

//Удалить все энкоунтеры
void WorldMap::ReleaseEncounters()
{
	//Оставим параметры энкоунтеров невредимыми
	for(long i = 0; i < wdmObjects->numShips; i++)
	{
		if(wdmObjects->ships[i] == wdmObjects->playerShip) continue;
		((WdmEnemyShip *)wdmObjects->ships[i])->SetSaveAttribute(null);
		wdmObjects->ships[i]->killMe = true;
	}
	for(i = 0; i < wdmObjects->numStorms; i++)
	{
		wdmObjects->storms[i]->SetSaveAttribute(null);
		wdmObjects->storms[i]->killMe = true;
	}
}

//Создать атрибут для сохранения параметров энкоунтера
ATTRIBUTES * WorldMap::GetEncSaveData(const char * type, const char * retName)
{
	if(!saveData) return null;
	//Генерим имя атрибута
	encCounter++;
	char atrName[64];
	for(long i = 0; i < 1000000; i++, encCounter++)
	{
		sprintf(atrName, "enc_%u", encCounter);
		ATTRIBUTES * a = saveData->FindAClass(saveData, atrName);
		if(!a) break;
		if(a->FindAClass(a, "needDelete"))
		{
			saveData->DeleteAttributeClassX(a);
			break;
		}
	}
	if(i == 1000000) return null;
	//Создаём ветку
	ATTRIBUTES * a = saveData->CreateSubAClass(saveData, atrName);
	if(!a) return false;
	//Устанавливаем тип
	a->SetAttribute("type", (char *)type);
	//Сохраняем имя
	if(AttributesPointer)
	{
		AttributesPointer->SetAttribute((char *)retName, atrName);
	}
	return a;
}