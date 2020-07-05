//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lighter
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Lighter.h"
#include "..\common_h\messages.h"


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Lighter::Lighter()
{
	rs = null;
	initCounter = 10;
	isInited = false;
	waitChange = 0.0f;
}

Lighter::~Lighter()
{
}

//Инициализация
bool Lighter::Init()
{
	//Проверяем, будем ли работать
	INIFILE * ini = api->fio->OpenIniFile("resource\\ini\\loclighter.ini");
	if(!ini) return false;
	long isLoading = ini->GetLong(null, "loading", 0);	
	autoTrace = ini->GetLong(null, "autotrace", 0) != 0;
	autoSmooth = ini->GetLong(null, "autosmooth", 0) != 0;
	window.isSmallSlider = ini->GetLong(null, "smallslider", 0) != 0;
	geometry.useColor = ini->GetLong(null, "usecolor", 0) != 0;
	delete ini;
	if(!isLoading) return false;
	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");
	//
	_CORE_API->LayerCreate("lighter_execute", true, false);
	_CORE_API->LayerSetFlags("lighter_execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("lighter_execute", GetID(), 1000);
	_CORE_API->LayerCreate("lighter_realize", true, false);
	_CORE_API->LayerSetFlags("lighter_realize", LRFLAG_REALIZE);
	_CORE_API->LayerAdd("lighter_realize", GetID(), 1000);
	//
	lightProcessor.SetParams(&geometry, &window, &lights, &octTree, rs);
	//оконная система
	if(!window.Init(rs)) return false;

	return true;
}

//Исполнение
void Lighter::Execute(dword delta_time)
{
	float dltTime = delta_time*0.001f;
	if(window.isSaveLight)
	{
		window.isSaveLight = false;
		if(geometry.Save())
		{
			window.isSuccessful = 1.0f;
		}else{
			window.isFailed = 10.0f;
		}
	}
	lightProcessor.Process();
	if(window.isNeedInit)
	{
		window.isNeedInit = false;
		window.Reset(true);
		PreparingData();
	}
	if(waitChange <= 0.0f)
	{
		if(GetAsyncKeyState(VK_NUMPAD0) < 0)
		{
			waitChange = 0.5f;
			if(isInited)
			{
				window.Reset(!window.isVisible);
			}else{
				window.isNeedInit = true;
				isInited = true;
			}
		}
	}else waitChange -= dltTime;

}

void Lighter::PreparingData()
{
	//Освещение
	//Рассеяное
	dword amb = 0xff404040;
	rs->GetRenderState(D3DRS_AMBIENT, &amb);
	CVECTOR clr;
	clr.x = ((amb >> 16) & 0xff)/255.0f;
	clr.y = ((amb >> 8) & 0xff)/255.0f;
	clr.z = ((amb >> 0) & 0xff)/255.0f;
	float mx = clr.x > clr.y ? clr.x : clr.y;
	mx = mx > clr.z ? mx : clr.z;
	if(mx > 0.0f) clr *= 1.0f/mx; else clr = 1.0f;
	lights.AddAmbient(clr);
	//Солнце
	if(rs)
	{
		BOOL isLight = FALSE;
		rs->GetLightEnable(0, &isLight);
		D3DLIGHT8 lit;
		if(isLight && rs->GetLight(0, &lit))
		{
			CVECTOR clr, dir = !CVECTOR(1.0f, 1.0f, 1.0f);
			clr.x = lit.Diffuse.r;
			clr.y = lit.Diffuse.g;
			clr.z = lit.Diffuse.b;
			if(lit.Type == D3DLIGHT_DIRECTIONAL)
			{
				dir.x = -lit.Direction.x;
				dir.y = -lit.Direction.y;
				dir.z = -lit.Direction.z;
			}
			float mx = dir.x > dir.y ? dir.x : dir.y;
			mx = mx > dir.z ? mx : dir.z;
			if(mx > 0.0f) dir *= 1.0f/mx; else dir = 1.0f;
			lights.AddWeaterLights(clr, dir);
		}
	}
	lights.PostInit();
	//Геометрия
	if(!geometry.Process(rs, lights.Num()))
	{
		window.isFailedInit = true;
		return;
	}
	octTree.Init(&geometry);
	//Освещалка
	lightProcessor.UpdateLightsParam();
	//Интерфейс
	window.InitList(lights);
	window.isTraceShadows = autoTrace;
	window.isSmoothShadows = autoSmooth;
}

void Lighter::Realize(dword delta_time)
{
	if(GetAsyncKeyState(VK_DECIMAL) < 0)
	{
		window.isNoPrepared = !isInited;
		geometry.DrawNormals(rs);
	}else window.isNoPrepared = false;
	window.Draw(delta_time*0.001f);
}

//Сообщения
dword _cdecl Lighter::ProcessMessage(MESSAGE & message)
{
	char command[32];
	message.String(31, command);
	command[31] = 0;
	if(stricmp(command, "AddModel") == 0)
	{
		//Добавляем модельку
		MsgAddModel(message);
		return true;
	}else
	if(stricmp(command, "ModelsPath") == 0)
	{
		//Добавляем модельку
		MsgModelsPath(message);
		return true;
	}else
	if(stricmp(command, "LightPath") == 0)
	{
		//Добавляем модельку
		MsgLightPath(message);
		return true;
	}else
	if(stricmp(command, "AddLight") == 0)
	{
		//Добавляем модельку
		MsgAddLight(message);
		return true;
	}
	return false;
}

void Lighter::MsgAddModel(MESSAGE & message)
{
	char name[512];
	message.String(511, name);
	name[511] = 0;
	if(!name[0])
	{
		api->Trace("Location lighter: no model name, skip it!");
		return;
	}
	ENTITY_ID model = message.EntityID();
	geometry.AddObject(name, model);
}

void Lighter::MsgModelsPath(MESSAGE & message)
{
	char name[512];
	message.String(511, name);
	name[511] = 0;	
	geometry.SetModelsPath(name);
}

void Lighter::MsgLightPath(MESSAGE & message)
{
	char name[512];
	message.String(511, name);
	name[511] = 0;	
	geometry.SetLightPath(name);
}

void Lighter::MsgAddLight(MESSAGE & message)
{
	CVECTOR pos, clr;
	//Позиция
	pos.x = message.Float();
	pos.y = message.Float();
	pos.z = message.Float();
	//Цвет
	clr.x = message.Float();
	clr.y = message.Float();
	clr.z = message.Float();
	//Затухание
	float att0 = message.Float();
	float att1 = message.Float();
	float att2 = message.Float();
	//Дистанция
	float range = message.Float();
	//Имя группы
	char group[512];
	message.String(511, group);
	group[511] = 0;
	//Добавляем источник
	lights.AddPointLight(clr, pos, att0, att1, att2, range, group);
}




