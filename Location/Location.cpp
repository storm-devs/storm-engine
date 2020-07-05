//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Location
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Location.h"
#include "Grass.h"
#include "Lights.h"
#include "Character.h"

#include "..\common_h\model.h"
#include "..\common_h\cvector4.h"

float		fCausticScale, fCausticDelta, fFogDensity, fCausticDistance;
CVECTOR4	v4CausticColor;
bool		bCausticEnable = false;
float		fCausticFrame = 0.0f;
long		iCausticTex[32];
float		fCausticSpeed = 0.0f;

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Location::Location()
{
	numLocators = 0;
	maxLocators = 16;
	locators = NEW LocatorArray * [maxLocators];
	patchJump = -1;
	isDebugView = true;
	sphereVertex = null;
	sphereNumTrgs = 0;
	lastLoadStaticModel = -1;
	srand(GetTickCount() | 1);
	isPause = false;
	lights = null;
	curMessage = 0;
	for(long i = 0; i < sizeof(message)/sizeof(DmgMessage); i++) message[i].alpha = 0.0f;
	locationTimeUpdate = 0.0f;
	enemyBarsCount = 0;
	bSwimming = true;
	bCausticEnable = false;
}

Location::~Location()
{
	if(!AttributesPointer) return;
	ATTRIBUTES * atr = AttributesPointer->FindAClass(AttributesPointer, "locators");
	if(atr) AttributesPointer->DeleteAttributeClassX(atr);
#ifndef _XBOX
	_CORE_API->DeleteEntity(cubeShotMaker);
	_CORE_API->DeleteEntity(lighter);
#endif
	_CORE_API->DeleteEntity(lizards);
	_CORE_API->DeleteEntity(rats);
	_CORE_API->DeleteEntity(eagle);
	_CORE_API->DeleteEntity(grass);
	_CORE_API->DeleteEntity(lightsid);
	_CORE_API->DeleteEntity(loceffectsid);
	api->DeleteEntity(blood);

	for(long i = 0; i < numLocators; i++) delete locators[i];
	delete locators;
	if(sphereVertex) delete sphereVertex;
	sphereVertex = null;
}


//Инициализация
bool Location::Init()
{
	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");
	rs->SetRenderState(D3DRS_LIGHTING, FALSE);

	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("execute", GetID(), 10);

	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetFlags("realize", LRFLAG_REALIZE);
	_CORE_API->LayerAdd("realize", GetID(), 100000);

	_CORE_API->CreateEntity(&lightsid, "Lights");
	_CORE_API->CreateEntity(&loceffectsid, "LocationEffects");

	enemyBarsTexture = rs->TextureCreate("LocEfx\\state_bars.tga");

#ifndef _XBOX
	_CORE_API->CreateEntity(&lighter, "Lighter");
	_CORE_API->CreateEntity(&cubeShotMaker, "CubeShotMakerCam");
#endif
	return true;
}

//Исполнение
void Location::Execute(dword delta_time)
{
	bSwimming = AttributesPointer->GetAttributeAsDword("swimming", 1) != 0;

	//Обсчёт персонажей
	if(!isDebugView) Update(delta_time);
	//Обсчёт сообщений
	float dltTime = delta_time*0.001f;
	for(long i = 0; i < sizeof(message)/sizeof(DmgMessage); i++)
	{
		if(message[i].alpha <= 0.0f) continue;
		message[i].p.y += dltTime*0.2f;
		message[i].alpha -= dltTime*0.4f;
	}
	//Обновление данных для травы
	Grass * grs = (Grass *)api->GetEntityPointer(&grass);
	if(grs)
	{
		for(long i = 0; i < supervisor.numCharacters; i++)
		{
			supervisor.character[i].c->GetGrassPosition(grs->characters[i].pos, grs->characters[i].lastPos);
			grs->characters[i].chr = supervisor.character[i].c;
			grs->characters[i].useCounter = 0;
		}
		grs->numCharacters = supervisor.numCharacters;
	}
	//Обновление в локации
	locationTimeUpdate += dltTime;
	if(locationTimeUpdate > 1.0f)
	{
		api->Event("LocationTimeUpdate", "f", locationTimeUpdate);
		locationTimeUpdate = 0.0f;
	}
}

void Location::Realize(dword delta_time)
{
	float fDeltaTime = float(delta_time) * 0.001f;
	
	fCausticFrame += fDeltaTime * fCausticSpeed;
	
	while (fCausticFrame >= 32.0f) fCausticFrame -= 32.0f;

	//Отрисовка локаторов
	for(long i = 0; i < numLocators; i++)
		if(locators[i]->isVisible) DrawLocators(locators[i]);
	if(IsDebugView())
	{
		//Отрисовка патча
		ptc.DebugDraw(rs, delta_time*0.001f);
		//Информация о локации
		const char * c = null;
		if(AttributesPointer)
		{
			c = AttributesPointer->GetAttribute("id");
		}
		if(!c) c = "ID not found";
		rs->Print(10, 10, "Location ID: '%s'", c);
		if(IsExDebugView())
		{
			for(long i = 0; i < model.Models(); i++)
			{
				rs->Print(10, 40 + i*26, "%2i mdl: '%s.gm'", i, model.GetModelName(i));
			}
		}
	}
	//Обсчёт персонажей
	if(isDebugView) Update(delta_time);
	//Отрисовка сообщений
	i = curMessage;
	for(long c = 0; c < sizeof(message)/sizeof(DmgMessage); c++, i--)
	{
		if(i < 0) i = sizeof(message)/sizeof(DmgMessage) - 1;
		if(message[i].alpha <= 0.0f) continue;
		//Print(message[i].p, 10.0f, 0, message[i].alpha, message[i].c, 0.8f, "%.0f/%.0f", message[i].hit, message[i].hp);
		Print(message[i].p, 10.0f, 0, message[i].alpha, message[i].c, 0.8f, "%.0f", message[i].hit);
	}
	//Отрисовка полосок над персонажами
	DrawEnemyBars();
	enemyBarsCount = 0;
}

void Location::Update(dword delta_time)
{
	lights = (Lights *)_CORE_API->GetEntityPointer(&lightsid);

	const dword max_delta_time = 500;
	const float maxDltTime = 0.1f;
	if(delta_time > max_delta_time) delta_time = max_delta_time;
	float dltTime = delta_time*0.001f;
	//Эффекты моделей
	model.Update(dltTime);
	//Персонажи
	if(!isPause)
	{
		supervisor.PreUpdate(dltTime);
		for(; dltTime > maxDltTime; dltTime -= maxDltTime) supervisor.Update(maxDltTime);
		if(dltTime > 0.0f) supervisor.Update(dltTime);
		supervisor.PostUpdate(dltTime);
	}else supervisor.Update(0.0f);
}

//Сообщения
dword _cdecl Location::ProcessMessage(MESSAGE & message)
{
	long i;
	float u0, v0, u1, v1;
	long level;
	LocatorArray * la;
	char name[256];
	char tech[256];
	switch(message.Long())
	{
	case MSG_LOCATION_ADD_MODEL:
		{
			message.String(sizeof(name), name);
			name[sizeof(name) - 1] = 0;
			message.String(sizeof(tech), tech);
			tech[sizeof(tech) - 1] = 0;
			level = message.Long();
			long dynamicLightsOn = message.Long();
			lastLoadStaticModel = LoadStaticModel(name, tech, level, dynamicLightsOn==1);
			return lastLoadStaticModel >= 0;
		}
	case MSG_LOCATION_GET_MODEL:
		if(lastLoadStaticModel < 0) return 0;
		if(!model.IsValidateIndex(lastLoadStaticModel)) return 0;
		if(!_CORE_API->ValidateEntity(&model.ID(lastLoadStaticModel))) return 0;
		message.ScriptVariablePointer()->Set(model.ID(lastLoadStaticModel));
		return 1;
	case MSG_LOCATION_MODEL_SET_POS:
		if(lastLoadStaticModel < 0) return 0;
		if(!model.IsValidateIndex(lastLoadStaticModel)) return 0;
		if(!model[lastLoadStaticModel]) return 0;
		//Ищим группу
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		la = FindLocatorsGroup(name);
		if(!la) return 0;
		//Ищем локатор
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		i = la->FindByName(name);
		if(i < 0) return 0;
		la->GetLocatorPos(i, model[lastLoadStaticModel]->mtx);
		return 1;
	case MSG_LOCATION_MODEL_SET_ROT:
		if(lastLoadStaticModel < 0) return 0;
		u0 = message.Float();	//rx
		v0 = message.Float();	//ry
		u1 = message.Float();	//rz
		model.SetRotation(lastLoadStaticModel, u0, v0, u1);
		return 1;
	case MSG_LOCATION_MODEL_SET_UVS:
		if(lastLoadStaticModel < 0) return 0;
		u0 = message.Float();
		v0 = message.Float();
		u1 = message.Float();
		v1 = message.Float();
		model.SetUVSlide(lastLoadStaticModel, u0, v0, u1, v1);
		return 1;
	case MSG_LOCATION_MODEL_LAMPS:
		if(lastLoadStaticModel < 0) return 0;
		lights = (Lights *)_CORE_API->GetEntityPointer(&lightsid);
		if(!lights) return 0;		
		return lights->AddLampModel(model.ID(lastLoadStaticModel));
	case MSG_LOCATION_MODEL_REFLECTION:
		if(lastLoadStaticModel < 0) return 0;
		model.SetReflection(lastLoadStaticModel, message.Float());
		return 1;
	case MSG_LOCATION_SET_CHRS_PATCH:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		return LoadCharacterPatch(name);
	case MSG_LOCATION_SET_JMP_PATCH:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		return LoadJumpPatch(name);
	case MSG_LOCATION_SET_GRS_PATCH:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		message.String(sizeof(tech), tech);
		name[sizeof(tech) - 1] = 0;
		return LoadGrass(name, tech);
	case MSG_LOCATION_UPDATELOCATORS:
		UpdateLocators();
		return 1;
	case MSG_LOCATION_MODELSPATH:
		message.String(sizeof(model.modelspath), model.modelspath);
		model.modelspath[sizeof(model.modelspath) - 1] = 0;
		model.UpdateModelsPath();

#ifndef _XBOX
		_CORE_API->Send_Message(lighter, "ss", "ModelsPath", model.modelspath);
#endif
		
		return 1;
	case MSG_LOCATION_TEXTURESPATH:
		message.String(sizeof(model.texturespath), model.texturespath);
		model.texturespath[sizeof(model.texturespath) - 1] = 0;
		model.UpdateTexturesPath();
		return 1;
	case MSG_LOCATION_LIGHTPATH:
		message.String(sizeof(model.lightpath), model.lightpath);
		model.lightpath[sizeof(model.lightpath) - 1] = 0;
		model.UpdateLightPath();

#ifndef _XBOX
		_CORE_API->Send_Message(lighter, "ss", "LightPath", model.lightpath);
#endif

		return 1;
	case MSG_LOCATION_SHADOWPATH:
		message.String(sizeof(model.shadowpath), model.shadowpath);
		model.lightpath[sizeof(model.shadowpath) - 1] = 0;
		model.UpdateShadowPath();
		return 1;
	case MSG_LOCATION_PAUSE:
		isPause = message.Long() != 0;
		return 1;
	case MSG_LOCATION_VIEWLGROUP:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		la = FindLocatorsGroup(name);
		if(!la) return 0;
		la->isVisible = true;
		la->kViewRadius = message.Float();
		la->viewDist = message.Float();
		la->color = message.Long();
		isDebugView = true;
		return 1;
	case MSG_LOCATION_HIDELGROUP:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		la = FindLocatorsGroup(name);
		if(!la) return 0;
		la->isVisible = false;
		la->kViewRadius = 0.0f;
		isDebugView = false;
		for(i = 0; i < numLocators; i++)
		{
			if(locators[i]->isVisible)
			{
				isDebugView = true;
				break;
			}
		}
		return 1;
	case MSG_LOCATION_GRP_RADIUS:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		la = FindLocatorsGroup(name);
		if(!la) return 0;
		la->radius = message.Float();
		return 1;
	case MSG_LOCATION_LOC_RADIUS:
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		la = FindLocatorsGroup(name);
		if(!la) return 0;
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		i = la->FindByName(name);
		if(i < 0) return 0;
		la->SetLocatorRadius(i, message.Float());
		return 1;
	case MSG_LOCATION_CHECKENTRY:
		u0 = message.Float();	//x
		v0 = message.Float();	//y
		u1 = message.Float();	//z
		return supervisor.CheckPosition(u0, v0, u1, null);
	case MSG_LOCATION_SETCHRPOSITIONS:
		supervisor.SetSavePositions();
		break;
	case MSG_LOCATION_TLPCHRPOSITIONS:
		supervisor.DelSavePositions(true);
		break;
	case MSG_LOCATION_CLRCHRPOSITIONS:
		supervisor.DelSavePositions(false);
		break;
	case MSG_LOCATION_ADD_LIGHT:
		lights = (Lights *)_CORE_API->GetEntityPointer(&lightsid);
		if(!lights) return false;
		message.String(sizeof(name), name);
		name[sizeof(name) - 1] = 0;
		i = lights->FindLight(name);
		if(i < 0) return false;
		u0 = message.Float();
		v0 = message.Float();
		u1 = message.Float();
		lights->AddLight(i, CVECTOR(u0, v0, u1));
		return true;
	case MSG_LOCATION_EX_MSG:
		message.String(sizeof(name), name); name[sizeof(name) - 1] = 0;
		return MessageEx(name, message);
	}
	return 0;
}

//Найти группу локаторов
LocatorArray * Location::FindLocatorsGroup(const char * gName)
{
	if(!gName || !gName[0]) return null;
	long hash = LocatorArray::CalcHashString(gName);
	for(long i = 0; i < numLocators; i++)
	{
		if(locators[i]->CompareGroup(gName, hash)) return locators[i];
	}
	return null;
}

long Location::LoadStaticModel(const char * modelName, const char * tech, long level, bool useDynamicLights)
{
	lights = (Lights *)_CORE_API->GetEntityPointer(&lightsid);
	long im = model.CreateModel(modelName, tech, level, true, useDynamicLights?GetLights():0);
	if(im < 0) return -1;
	//Указатель на геометрию
	MODEL * mdl = model[im];
	if(!mdl)
	{
		model.DeleteModel(im);
		return -1;
	}
	NODE * node = mdl->GetNode(0);
	if(!node)
	{
		model.DeleteModel(im);
		return -1;
	}
	GEOS * g = node->geo;
	if(!g)
	{
		model.DeleteModel(im);
		return -1;
	}
	//Добавим модельку в специальные слои
	_CORE_API->LayerAdd("shadow", mdl->GetID(), 10);
	_CORE_API->LayerAdd("sun_trace", mdl->GetID(), 10);
	api->LayerAdd("blood", mdl->GetID(), 100);
	api->LayerAdd("rain_drops", mdl->GetID(), 100);
	//Зачитываем все локаторы
	GEOS::INFO ginfo;
	GEOS::LABEL label;
	g->GetInfo(ginfo);
	for(long i = 0; i < ginfo.nlabels; i++)
	{
		g->GetLabel(i, label);
		if(!label.group_name || !label.group_name[0]) continue;
		long hash = LocatorArray::CalcHashString(label.group_name);
		for(long j = 0; j < numLocators; j++)
		{
			if(locators[j]->CompareGroup(label.group_name, hash)) break;
		}
		if(j == numLocators)
		{
			if(numLocators == maxLocators)
			{
				maxLocators += 16;
				locators = (LocatorArray **)RESIZE(locators, maxLocators*sizeof(LocatorArray *));
			}
			numLocators++;
			locators[j] = NEW LocatorArray(label.group_name);
		}
		long locIndex = locators[j]->FindByName(label.name);
		if(locIndex < 0)
		{
			CMatrix & mtxx = *((CMatrix *)label.m);
#ifndef _XBOX
			for(long me = 0; me < 16; me++)
				if(_isnan(mtxx.matrix[me]))
				{
					api->Trace("Location: locator %s::%s in position have NaN value, reset it!", label.group_name, label.name);
					mtxx.SetIdentity();
					break;
				}
#endif
			locators[j]->AddLocator(mtxx, label.name);
		}else{
			locators[j]->SetNewMatrix(locIndex, *((CMatrix *)label.m));
		}
	}

#ifndef _XBOX
	_CORE_API->Send_Message(lighter, "ssi", "AddModel", modelName, mdl->GetID());
#endif

	return im;
}

bool Location::LoadCharacterPatch(const char * ptcName)
{
	//Формируем путь до файла
	char path[512];
	strcpy(path, "resource\\models\\");
	strcat(path, model.modelspath);
	strcat(path, ptcName);
	strcat(path, ".ptc");
	//Загружаем патч
	bool result = ptc.Load(path);
	if(!result) _CORE_API->Trace("Can't loaded patch data file %s.ptc for npc.", ptcName);
	return result;
}

bool __declspec(dllexport) __cdecl Location::LoadJumpPatch(const char * modelName)
{
	if(patchJump >= 0) model.DeleteModel(patchJump);
	patchJump = -1;
	if(!modelName || !modelName[0]) return false;
	patchJump = model.CreateModel(modelName, "", 0, false, 0);
	return patchJump >= 0;
}

bool __declspec(dllexport) __cdecl Location::LoadGrass(const char * modelName, const char * texture)
{
	_CORE_API->DeleteEntity(grass);
	if(!modelName || !modelName[0]) return true;
	_CORE_API->CreateEntity(&grass, "Grass");
	Grass * grs = (Grass *)_CORE_API->GetEntityPointer(&grass);
	if(!grs) return false;
	if(texture && texture[0]) grs->SetTexture(texture);
	char nm[512];
	strcpy(nm, "resource\\models\\");
	strcat(nm, model.modelspath);
	strcat(nm, modelName);
	strcat(nm, ".grs");
	long ll = strlen(nm);
	if(grs->LoadData(nm)) return true;
	_CORE_API->Trace("Can't load grass data file: %s", nm);
	_CORE_API->DeleteEntity(grass);
	return false;
}

bool Location::MessageEx(const char * name, MESSAGE & message)
{
	if(stricmp(name, "DelAllLights") == 0)
	{
		lights->DelAllLights();
	}else
	if(stricmp(name, "AddFlys") == 0)
	{
		ENTITY_ID effects;
		_CORE_API->FindClass(&effects, "LocationEffects", 0);
		float x = message.Float();
		float y = message.Float();
		float z = message.Float();
		api->Send_Message(effects, "sfff", "AddFly", x, y, z);
		return true;
	}else
	if(stricmp(name, "DelFlys") == 0)
	{
		ENTITY_ID effects;
		_CORE_API->FindClass(&effects, "LocationEffects", 0);
		api->Send_Message(effects, "s", "DelFlys");
		return true;
	}else
	if(stricmp(name, "GetPatchMiddlePos") == 0)
	{
		VDATA * vx = message.ScriptVariablePointer();
		if(!vx) return false;
		VDATA * vy = message.ScriptVariablePointer();
		if(!vy) return false;
		VDATA * vz = message.ScriptVariablePointer();
		if(!vz) return false;
		vx->Set(ptc.middle.x);
		vy->Set(ptc.middle.y);
		vz->Set(ptc.middle.z);
		return true;
	}else
	if(stricmp(name, "AddEagle") == 0)
	{
		_CORE_API->CreateEntity(&eagle, "LocEagle");
		return true;
	}else
	if(stricmp(name, "AddLizards") == 0)
	{
		_CORE_API->CreateEntity(&lizards, "Lizards");
		return true;
	}else
	if(stricmp(name, "AddRats") == 0)
	{
		_CORE_API->CreateEntity(&rats, "LocRats");
		if(!_CORE_API->Send_Message(rats, "l", message.Long()))
		{
			_CORE_API->DeleteEntity(rats);
			return false;
		}
		return true;
	}else
	if(stricmp(name, "AddBlood") == 0)
	{
		if( !api->ValidateEntity(&blood) ) {
			api->CreateEntity(&blood, "Blood");
			api->LayerAdd("execute", blood, 65540);
			api->LayerAdd("realize", blood, 65540);
		}
		CVECTOR vPos;
		vPos.x = message.Float();
		vPos.y = message.Float();
		vPos.z = message.Float();
		api->Send_Message(blood, "lfff", 2, vPos.x,vPos.y,vPos.z );
		return true;
	}else
	if(stricmp(name, "TestLocatorsGroup") == 0)
	{
		TestLocatorsInPatch(message);
		return true;
	}else
	if(stricmp(name, "DeleteLocationModel") == 0)
	{
		char modelname[MAX_PATH];
		message.String( sizeof(modelname), modelname );
		long n = model.FindModel( modelname );
		if( n>=0 ) model.DeleteModel( n );
		return true;
	}else
	if(stricmp(name, "HideLocationModel") == 0)
	{
		char modelname[MAX_PATH];
		message.String( sizeof(modelname), modelname );
		long n = model.FindModel( modelname );
		if( n>=0 )
			//api->LayerDel("realize", model.RealizerID(n));
			_CORE_API->Send_Message(model.RealizerID(n),"ll",2,0);
	}else
	if(stricmp(name, "ShowLocationModel") == 0)
	{
		char modelname[MAX_PATH];
		message.String( sizeof(modelname), modelname );
		long layer = message.Long();
		long n = model.FindModel( modelname );
		if( n>=0 )
			//api->LayerAdd("realize", model.RealizerID(n), layer);
			_CORE_API->Send_Message(model.RealizerID(n),"ll",2,1);
	} else
	if(stricmp(name, "SetGrassParams") == 0)
	{
		float fScale = message.Float();
		float fMaxWidth = message.Float();
		float fMaxHeight = message.Float();
		float fMinVisibleDist = message.Float();
		float fMaxVisibleDist = message.Float();
		float fMinGrassLod = message.Float();
		api->Send_Message( grass, "lffffff", MSG_GRASS_SET_PARAM,
			fScale, fMaxWidth,fMaxHeight, fMinVisibleDist,fMaxVisibleDist, fMinGrassLod );
	} else
	if(stricmp(name, "LoadCaustic") == 0)
	{
		LoadCaustic();
	}
	else
	if(stricmp(name, "EnableCaustic") == 0)
	{
		bCausticEnable = message.Long() != 0;
	}	return false;
}

void Location::UpdateLocators()
{
	CMatrix mtx;
	if(!AttributesPointer) return;
	ATTRIBUTES * atr = AttributesPointer->FindAClass(AttributesPointer, "locators");
	if(atr) AttributesPointer->DeleteAttributeClassX(atr);
	AttributesPointer->CreateSubAClass(AttributesPointer, "locators");
	atr = AttributesPointer->FindAClass(AttributesPointer, "locators");
	if(atr)
	{
		//Создаём новые атрибуты
		for(long i = 0; i < numLocators; i++)
		{
			char * groupName = locators[i]->GetGroupName();
			atr->CreateSubAClass(atr, groupName);
			ATTRIBUTES * at = atr->FindAClass(atr, groupName);
			if(at)
			{
				for(long j = 0; j < locators[i]->Num(); j++)
				{					
					at->CreateSubAClass(at, locators[i]->Name(j));
					ATTRIBUTES * a = at->FindAClass(at, locators[i]->Name(j));
					if(a)
					{
						//Radius
						a->CreateSubAClass(a, "radius");
						a->SetAttributeUseFloat("radius", locators[i]->GetLocatorRadius(j));
						//Matrix
						locators[i]->GetLocatorPos(j, mtx);
						//Pos
						a->SetAttributeUseFloat("x", mtx.Pos().x);
						a->SetAttributeUseFloat("y", mtx.Pos().y);
						a->SetAttributeUseFloat("z", mtx.Pos().z);
						//vz
						a->CreateSubAClass(a, "vz");
						ATTRIBUTES * v = a->FindAClass(a, "vz");
						if(v)
						{
							v->SetAttributeUseFloat("x", mtx.Vz().x);
							v->SetAttributeUseFloat("y", mtx.Vz().y);
							v->SetAttributeUseFloat("z", mtx.Vz().z);
						}else{
							_CORE_API->Trace("Location: Can't create attribute 'locators.%s.%s.vz'!", groupName, locators[i]->Name(j));
						}
						//vy
						a->CreateSubAClass(a, "vy");
						v = a->FindAClass(a, "vy");
						if(v)
						{
							v->SetAttributeUseFloat("x", mtx.Vy().x);
							v->SetAttributeUseFloat("y", mtx.Vy().y);
							v->SetAttributeUseFloat("z", mtx.Vy().z);
						}else{
							_CORE_API->Trace("Location: Can't create attribute 'locators.%s.%s.vy'!", groupName, locators[i]->Name(j));
						}
						//vx
						a->CreateSubAClass(a, "vx");
						v = a->FindAClass(a, "vx");
						if(v)
						{
							v->SetAttributeUseFloat("x", mtx.Vx().x);
							v->SetAttributeUseFloat("y", mtx.Vx().y);
							v->SetAttributeUseFloat("z", mtx.Vx().z);
						}else{
							_CORE_API->Trace("Location: Can't create attribute 'locators.%s.%s.vx'!", groupName, locators[i]->Name(j));
						}
					}else{
						_CORE_API->Trace("Location: Can't create attribute 'locators.%s.%s'!", groupName, locators[i]->Name(j));
					}
				}
			}else{
				_CORE_API->Trace("Location: Can't create attribute 'locators.%s'!", groupName);
			}
		}
	}else{
		_CORE_API->Trace("Location: Can't create attribute 'locators'!");
	}
}

void Location::DrawLocators(LocatorArray * la)
{
	if(la->kViewRadius > 0.0f)
	{
		//Стейты
		if(!sphereNumTrgs) CreateSphere();
		CMatrix mPos;
		//Уберём текстуры	
		rs->TextureSet(0, -1);
		rs->TextureSet(1, -1);
		//Стартуем технику
		bool isSet = rs->TechniqueExecuteStart("DbgDrawLocators");
		rs->SetRenderState(D3DRS_TEXTUREFACTOR, la->color);
		//Рисуем
		for(long i = 0; i < la->Num(); i++)
		{
			//Рисуем шарик
			la->GetLocatorPos(i, mPos);
			float rad = la->GetLocatorRadius(i)*la->kViewRadius;
			if(rad <= 0.0f) continue;
			mPos.m[0][0] *= rad;
			mPos.m[0][1] *= rad;
			mPos.m[0][2] *= rad;
			mPos.m[1][0] *= rad;
			mPos.m[1][1] *= rad;
			mPos.m[1][2] *= rad;
			mPos.m[2][0] *= rad;
			mPos.m[2][1] *= rad;
			mPos.m[2][2] *= rad;
			rs->SetTransform(D3DTS_WORLD, mPos);
			rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, sphereNumTrgs, sphereVertex, sizeof(SphVertex), null);
		}
		if(isSet) while(rs->TechniqueExecuteNext()){};
	}
	//Подписываем
	static CMatrix mtx, view, prj;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	//Получим текущие размеры vp
	static D3DVIEWPORT8 vp;
	rs->GetViewport(&vp);
	float w = vp.Width*0.5f;
	float h = vp.Height*0.5f;
	CVECTOR lvrt;
	MTX_PRJ_VECTOR vrt;
	long fh = rs->CharHeight(FONT_DEFAULT);
	long gw = rs->StringWidth(la->GetGroupName())/2;
	view.Transposition();
	float d = view.Vz() | view.Pos();
	float viewDst = la->viewDist*la->viewDist;
	//Рисуем
	for(long i = 0; i < la->Num(); i++)
	{	
		float lbh = la->GetLocatorRadius(i)*la->kViewRadius;
		if(lbh <= 0.0f) continue;
		if(lbh > 1.5f) lbh = 1.5f;
		//Пишем текст
		la->GetLocatorPos(i, lvrt.x, lvrt.y, lvrt.z);
		if((lvrt | view.Vz()) < d) continue;
		if(~(lvrt - view.Pos()) > viewDst) continue;
		lvrt.y += lbh;
		mtx.Projection(&lvrt, &vrt, 1, w, h, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
		rs->Print(long(vrt.x - gw), long(vrt.y - fh), la->GetGroupName());
		long lw = rs->StringWidth((char *)la->LocatorName(i))/2;
		rs->Print(long(vrt.x - lw), long(vrt.y), (char *)la->LocatorName(i));
	}
	rs->SetTransform(D3DTS_WORLD, CMatrix());
}

void Location::DrawLine(const CVECTOR & s, dword cs, const CVECTOR & d, dword cd, bool useZ)
{
	SphVertex lineVertex[2];
	lineVertex[0].v = s;
	lineVertex[0].c = cs;
	lineVertex[1].v = d;
	lineVertex[1].c = cd;
	rs->SetTransform(D3DTS_WORLD, CMatrix());
	//Уберём текстуры	
	rs->TextureSet(0, -1);
	rs->TextureSet(1, -1);
	//Установим Z
	dword oldZState = 1;
	rs->GetRenderState(D3DRS_ZENABLE, &oldZState);
	rs->SetRenderState(D3DRS_ZENABLE, useZ);
	//Рисуем
	rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, 1, lineVertex, sizeof(SphVertex), "DbgLocationDrawLine");
	//Востанавливаем
	rs->SetRenderState(D3DRS_ZENABLE, oldZState);
}

void Location::CreateSphere()
{
#define CalcKColor(ind) {kColor = light | !CVECTOR(sphereVertex[t*3 + ind].v.x, sphereVertex[t*3 + ind].v.y, sphereVertex[t*3 + ind].v.z); if(kColor < 0.0f) kColor = 0.0f; }
#define CLerp(c, min) (dword(c*(kColor*(1.0f - min) + min)))
#define Color ((CLerp(255.0f, 0.5f) << 24) | (CLerp(255.0f, 0.7f) << 16) | (CLerp(255.0f, 0.7f) << 8) | (CLerp(255.0f, 0.7f) << 0));
	
	if(sphereVertex) return;

	const float myPI = 3.1415926535897932f;
	const long a1 = 32;
	const long a2 = (a1/2);

	sphereNumTrgs = a1*a2*2;
	sphereVertex = NEW SphVertex[sphereNumTrgs*6];

	CVECTOR light = !CVECTOR(0.0f, 0.0f, 1.0f);
	float kColor;
	//Заполняем вершины
	for(long i = 0, t = 0; i < a2; i++)
	{
		float r1 = sinf(myPI*i/float(a2));
		float y1 = cosf(myPI*i/float(a2));
		float r2 = sinf(myPI*(i + 1)/float(a2));
		float y2 = cosf(myPI*(i + 1)/float(a2));
		for(long j = 0; j < a1; j++)
		{
			float x1 = sinf(2.0f*myPI*j/float(a1));
			float z1 = cosf(2.0f*myPI*j/float(a1));
			float x2 = sinf(2.0f*myPI*(j + 1)/float(a1));
			float z2 = cosf(2.0f*myPI*(j + 1)/float(a1));
			//0
			sphereVertex[t*3 + 0].v.x = r1*x1;
			sphereVertex[t*3 + 0].v.y = y1;
			sphereVertex[t*3 + 0].v.z = r1*z1;
			CalcKColor(0);
			sphereVertex[t*3 + 0].c = Color;
			//1
			sphereVertex[t*3 + 1].v.x = r2*x1;
			sphereVertex[t*3 + 1].v.y = y2;
			sphereVertex[t*3 + 1].v.z = r2*z1;
			CalcKColor(1);
			sphereVertex[t*3 + 1].c = Color;
			//2
			sphereVertex[t*3 + 2].v.x = r1*x2;
			sphereVertex[t*3 + 2].v.y = y1;
			sphereVertex[t*3 + 2].v.z = r1*z2;
			CalcKColor(2);
			sphereVertex[t*3 + 2].c = Color;
			//3 = 2
			sphereVertex[t*3 + 3] = sphereVertex[t*3 + 2];
			//4 = 1
			sphereVertex[t*3 + 4] = sphereVertex[t*3 + 1];
			//5
			sphereVertex[t*3 + 5].v.x = r2*x2;
			sphereVertex[t*3 + 5].v.y = y2;
			sphereVertex[t*3 + 5].v.z = r2*z2;
			CalcKColor(5);
			sphereVertex[t*3 + 5].c = Color;
			//Добавили 2 треугольника
			t += 2;
		}
	}
	Assert(sphereNumTrgs == t);
}

bool Location::IsExDebugView()
{
#ifndef _XBOX
	return api->Controls->GetDebugAsyncKeyState('O') < 0;
#else
	return false;
#endif
}

bool Location::IsDebugView()
{
#ifndef _XBOX
 	/*if(api->Controls->GetDebugAsyncKeyState('Y') < 0)
	{
		api->SetTimeScale(0.1f);
	}else{
		api->SetTimeScale(1.0f);
	}*/
	return api->Controls->GetDebugAsyncKeyState('G') < 0 || api->Controls->GetDebugAsyncKeyState('O') < 0;
#else
	return false;
#endif
}

#include <stdio.h>

//Написать текст
void _cdecl Location::Print(const CVECTOR & pos3D, float rad, long line, float alpha, dword color, float scale, const char * format, ...)
{
	static char buf[256];
	scale *= 2.0f;
	//Печатаем в буфер
	long len = _vsnprintf(buf, sizeof(buf) - 1, format, (char *)(&format + 1));
	buf[sizeof(buf) - 1] = 0;
	//Ищем позицию точки на экране
	static CMatrix mtx, view, prj;
	static D3DVIEWPORT8 vp;
	MTX_PRJ_VECTOR vrt;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	view.Transposition();
	float dist = ~(pos3D - view.Pos());
	if(dist >= rad*rad) return;
	float d = view.Vz() | view.Pos();
	if((pos3D | view.Vz()) < d) return;
	rs->GetViewport(&vp);
	mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width*0.5f, vp.Height*0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
	//Ищем позицию
	float fh = rs->CharHeight(FONT_DEFAULT)*0.8f;
	vrt.y -= (line + 0.5f)*fh*scale;
	//Прозрачность	
	const float kDist = 0.75f;
	if(alpha < 0.0f) alpha = 0.0f;
	if(alpha > 1.0f) alpha = 1.0f;
	if(dist > kDist*kDist*rad*rad)
	{
		dist = 1.0f - (sqrtf(dist) - kDist*rad)/(rad - kDist*rad);
		alpha *= dist;
	}
	if(alpha <= 0.0f) return;
	color = (dword(alpha*255.0f) << 24) | (color & 0xffffff);
	//Печатаем текст
	rs->ExtPrint(FONT_DEFAULT, color, 0x00000000, ALIGN_CENTER, 0, scale, 0, 0, long(vrt.x), long(vrt.y), buf);
}


//Добавить сообщение о повреждении
void Location::AddDamageMessage(const CVECTOR & pos3D, float hit, float curhp, float maxhp)
{
	curMessage++;
	if(curMessage >= sizeof(message)/sizeof(DmgMessage)) curMessage = 0;
	message[curMessage].p = pos3D;
	message[curMessage].alpha = 1.0f;
	message[curMessage].hit = hit;
	message[curMessage].hp = curhp;
	float k = 0.0f;
	if(maxhp > 0.0f) k = curhp/maxhp;
	if(k < 0.0f) k = 0.0f;
	if(k > 1.0f) k = 1.0f;
	float r1 = 0.2f, g1 = 1.0f, b1 = 0.2f;
	float r2 = 1.0f, g2 = 0.2f, b2 = 0.2f;
	float r = r2 + (r1 - r2)*k;
	float g = g2 + (g1 - g2)*k;
	float b = b2 + (b1 - b2)*k;
	message[curMessage].c = (long(r*255.0f) << 16) | (long(g*255.0f) << 8) | long(b*255.0f);
}

//Нарисовать на данном кадре полоски над врагом
void Location::DrawEnemyBars(const CVECTOR & pos, float hp, float energy, float alpha)
{
	if(enemyBarsCount >= sizeof(enemyBar)/sizeof(enemyBar[0]))
	{
		return;
	}
	if(alpha < 1.0f/255.0f)
	{
		return;
	}
	if(alpha > 1.0f) alpha = 1.0f;
	enemyBar[enemyBarsCount].p = pos;
	enemyBar[enemyBarsCount].alpha = alpha*255.0f;
	enemyBar[enemyBarsCount].hp = hp;
	enemyBar[enemyBarsCount].energy = energy;
	enemyBarsCount++;
}

void Location::TestLocatorsInPatch(MESSAGE & message)
{
	char buf[4096];
	message.String(sizeof(buf), buf);
	buf[sizeof(buf) - 1] = 0;
	LocatorArray * la = FindLocatorsGroup(buf);
	if(!la)
	{
		_snprintf(buf + 2048, sizeof(buf) - 2048, "Warning: Locators group '%s' not found.", buf);
		buf[sizeof(buf) - 1] = 0;
		api->Event("LocatorsEventTrace", "lsss", 0, buf + 2048, buf, "");
		return;
	}
	long num = la->Num();
	if(num <= 0)
	{
		_snprintf(buf, sizeof(buf), "Warning: Locators group '%s' not contain locators.", la->GetGroupName());
		buf[sizeof(buf) - 1] = 0;
		api->Event("LocatorsEventTrace", "lsss", 0, buf, la->GetGroupName(), "");
		return;
	}
	CVECTOR pos;
	for(long i = 0; i < num; i++)
	{
		la->GetLocatorPos(i, pos.x, pos.y, pos.z);
		float y = 0.0f;
		if(ptc.FindNode(pos, y) < 0)
		{
			_snprintf(buf, sizeof(buf), "Error: Locator '%s':'%s' not in patch.", la->GetGroupName(), la->LocatorName(i));
			buf[sizeof(buf) - 1] = 0;
			api->Event("LocatorsEventTrace", "lsss", 1, buf, la->GetGroupName(), la->LocatorName(i));
		}else{
			float ldist = pos.y - y;
			if(fabsf(ldist) > 0.2f)
			{
				_snprintf(buf, sizeof(buf), "Warning: Locator '%s':'%s' very far from patch: %fm", la->GetGroupName(), la->LocatorName(i), ldist);
				buf[sizeof(buf) - 1] = 0;
				api->Event("LocatorsEventTrace", "lsss", 0, buf, la->GetGroupName(), la->LocatorName(i));
			}
		}
	}
}

//Отрисовка полосок над персонажами
void Location::DrawEnemyBars()
{
	const float maxViewDist = 12.0f;
	const float alphaThresholdRelativeDist = 0.8f;
	//Текущии параметры сцены
	static CMatrix mtx, view, prj;
	static D3DVIEWPORT8 vp;
	struct SortElement
	{
		MTX_PRJ_VECTOR vrt;
		dword color;
		float hp;
		float energy;
	} sort[sizeof(enemyBar)/sizeof(enemyBar[0])];
	SortElement * selements[sizeof(enemyBar)/sizeof(enemyBar[0])];
	long sortCount = 0;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	view.Transposition();
	rs->GetViewport(&vp);
	//Перебираем все записи
	for(long i = 0; i < enemyBarsCount; i++)
	{
		//Ищем позицию точки на экране
		CVECTOR & pos3D = enemyBar[i].p;
		float dist = ~(pos3D - view.Pos());
		if(dist >= maxViewDist*maxViewDist) continue;
		float d = view.Vz() | view.Pos();
		if((pos3D | view.Vz()) < d) continue;
		MTX_PRJ_VECTOR vrt;
		mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width*0.5f, vp.Height*0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
		//Определяем прозрачность
		float k = sqrtf(dist)/maxViewDist;
		if(k > alphaThresholdRelativeDist)
		{
			k = 1.0f - (k - alphaThresholdRelativeDist)/(1.0f - alphaThresholdRelativeDist);
		}else{
			k = 1.0f;
		}
		dword color = long(k*enemyBar[i].alpha);
		if(!color) continue;
		color = (color << 24) | 0x007f7f7f;
		//Добавим в список отрисовка
		sort[sortCount].vrt = vrt;
		sort[sortCount].color = color;
		sort[sortCount].hp = enemyBar[i].hp;
		sort[sortCount].energy = enemyBar[i].energy;
		selements[sortCount++] = &sort[sortCount];
	}
	//Cортировка по дистанции
	for(long i = 0; i < sortCount - 1; i++)
	{
		for(long j = i + 1; j < sortCount; j++)
		{
			if(selements[i]->vrt.z < selements[j]->vrt.z)
			{
				  SortElement * tmp = selements[i];
				  selements[i] = selements[j];
				  selements[j] = tmp;
			}
		}
	}
	//Отрисовка
	for(long i = 0; i < sortCount; i++)
	{
		MTX_PRJ_VECTOR & vrt = selements[i]->vrt;
		dword & color = selements[i]->color;
		static BarVertex bar[18];
		float width = (256.0f*vrt.rhw)*0.5f;
		float height = (64.0f*vrt.rhw)*0.5f;
		if(width > vp.Width*0.1f)
		{
			float k = vp.Width*0.1f/width;
			width *= k;
			height *= k;
		}
		//Подложка
		bar[0].p.x = vrt.x - width; bar[0].p.y = vrt.y - height; bar[0].u = 0.0f; bar[0].v = 0.0f;
		bar[1].p.x = vrt.x + width; bar[1].p.y = vrt.y - height; bar[1].u = 1.0f; bar[1].v = 0.0f;
		bar[2].p.x = vrt.x - width; bar[2].p.y = vrt.y + height; bar[2].u = 0.0f; bar[2].v = 0.5f;
		bar[3] = bar[2]; bar[4] = bar[1];
		bar[5].p.x = vrt.x + width; bar[5].p.y = vrt.y + height; bar[5].u = 1.0f; bar[5].v = 0.5f;
		//Жизнь
		bar[6].p.x = vrt.x - width; bar[6].p.y = vrt.y - height; bar[6].u = 0.0f; bar[6].v = 0.5f;
		bar[7].p.x = vrt.x + width; bar[7].p.y = vrt.y - height; bar[7].u = 1.0f; bar[7].v = 0.5f;
		bar[8].p.x = vrt.x - width; bar[8].p.y = vrt.y; bar[8].u = 0.0f; bar[8].v = 0.75f;
		bar[9] = bar[8]; bar[10] = bar[7];
		bar[11].p.x = vrt.x + width; bar[11].p.y = vrt.y; bar[11].u = 1.0f; bar[11].v = 0.75f;
		//Энергия
		bar[12].p.x = vrt.x - width; bar[12].p.y = vrt.y; bar[12].u = 0.0f; bar[12].v = 0.75f;
		bar[13].p.x = vrt.x + width; bar[13].p.y = vrt.y; bar[13].u = 1.0f; bar[13].v = 0.75f;
		bar[14].p.x = vrt.x - width; bar[14].p.y = vrt.y + height; bar[14].u = 0.0f; bar[14].v = 1.0f;
		bar[15] = bar[14]; bar[16] = bar[13];
		bar[17].p.x = vrt.x + width; bar[17].p.y = vrt.y + height; bar[17].u = 1.0f; bar[17].v = 1.0f;
		//Общие поля
		for(long n = 0; n < sizeof(bar)/sizeof(bar[0]); n++)
		{
			bar[n].p.z = vrt.z;
			bar[n].rhw = vrt.rhw;
			bar[n].c = color;
		}
		//Корректируем полоски
		CorrectBar(selements[i]->hp, 0.03f, 0.97f, bar + 6);
		CorrectBar(selements[i]->energy, 0.03f, 0.97f, bar + 12);
		//Рисуем
		rs->TextureSet(0, enemyBarsTexture);
		rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 6, bar, sizeof(bar[0]), "LocationModelBlend");
	}
}

void Location::CorrectBar(float v, float start, float end, BarVertex * vrt)
{
	end = start + (end - start)*v;
	float dx = vrt[1].p.x - vrt[0].p.x;
	float du = vrt[1].u - vrt[0].u;
	float startX = vrt[0].p.x + dx*start;
	float startU = vrt[0].u + du*start;
	float endX = vrt[0].p.x + dx*end;	
	float endU = vrt[0].u + du*end;
	vrt[0].p.x = startX; vrt[0].u = startU;
	vrt[1].p.x = endX; vrt[1].u = endU;
	vrt[2].p.x = startX; vrt[2].u = startU;
	vrt[3].p.x = startX; vrt[3].u = startU;
	vrt[4].p.x = endX; vrt[4].u = endU;
	vrt[5].p.x = endX; vrt[5].u = endU;
}

bool Location::IsSwimming()
{
	return bSwimming;
}

void Location::LoadCaustic()
{
	bCausticEnable = false;

	ATTRIBUTES * pC = AttributesPointer->GetAttributeClass("Caustic");
	if (!pC) return;

	fCausticDelta = 0.0f;
	fCausticFrame = 0.0f;

	fCausticScale = pC->GetAttributeAsFloat("scale");
	fFogDensity = pC->GetAttributeAsFloat("fogdensity");
	fCausticDistance = pC->GetAttributeAsFloat("distance");
	v4CausticColor = COLOR2VECTOR4(pC->GetAttributeAsDword("color"));
	fCausticSpeed = pC->GetAttributeAsFloat("speed");

	char tex[256];
	for (long i=0; i<32; i++)
	{
		sprintf(tex, "weather\\caustic\\caustic%.2d.tga", i);
		iCausticTex[i] = Render().TextureCreate(tex);
	}
}
