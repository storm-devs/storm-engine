//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmObjects
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmObjects.h"
#include "..\common_h\vmodule_api.h"
#include "..\common_h\geometry.h"

//============================================================================================

WdmObjects * wdmObjects = null;

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmObjects::WdmObjects() : models(_FL_)
{
	Assert(!wdmObjects);
	srand(GetTickCount());
	wdmObjects = this;
	wm = null;
	rs = null;
	gs = null;
	camera = null;
	islands = null;
	playerShip = null;
	ships = null;
	numShips = 0;
	maxShips = 0;
	storms = null;
	numStorms = 0;
	maxStorms = 0;
	isDebug = false;
	Clear();
	enemyShip = null;
	enableSkipEnemy = false;
	playarInStorm = false;
	worldSizeX = 2000.0f;
	worldSizeZ = 2000.0f;
	for(long i = 0; i < sizeof(entryModels)/sizeof(entryModels[0]); i++)
	{
		entryModels[i] = -1;
	}
}

WdmObjects::~WdmObjects()
{
	if(ships) delete ships;
	if(storms) delete storms;
	wdmObjects = null;
	for(long i = 0; i < models; i++)
	{
		delete models[i].geo;
	}
}

void WdmObjects::SetWorldSize(float x, float z)
{
	worldSizeX = x;
	worldSizeZ = z;
	x *= 1.2f;
	z *= 1.2f;
	windField.Init(-0.5f*x, -0.5f*z, 0.5f*x, 0.5f*z);
}

void WdmObjects::Clear()
{
	if(ships) delete ships;
	if(storms) delete storms;
	wm = null;
	rs = null;
	gs = null;
	islands = null;
	ships = null;
	numShips = 0;
	maxShips = 0;
	storms = null;
	numStorms = 0;
	maxStorms = 0;
	isDebug = false;
	isPause = false;
	shipSpeedOppositeWind = 0.8f;
	shipSpeedOverWind = 1.2f;
	enemyshipViewDistMin = 20.0f;		//Растояние на котором корабль начинает исчезать
	enemyshipViewDistMax = 50.0f;		//Растояние на котором исчезает полностью корабль
	enemyshipDistKill = 100.0f;			//Расстояние на котором убиваем корабль
	enemyshipBrnDistMin = 40.0f;		//Минимальное растояние при котором рожается корабль
	enemyshipBrnDistMax = 70.0f;		//Максимальное растояние при котором рожается корабль
	stormViewDistMin = 120.0f;			//Растояние на котором шторм начинает исчезать
	stormViewDistMax = 160.0f;			//Растояние на котором шторм исчезает полностью
	stormDistKill = 200.0f;				//Расстояние на котором убиваем шторм
	stormBrnDistMin = 70.0f;			//Минимальное растояние на котором рожается шторм
	stormBrnDistMax = 160.0f;			//Максимальное растояние на котором рожается шторм
	stormZone = 50.0f;					//Общий радиус действия шторма
	enableSkipEnemy = false;
	playarInStorm = false;
	isNextDayUpdate = true;
	SetWorldSize(2000.0f, 2000.0f);
}

void WdmObjects::AddShip(WdmShip * ship)
{
	Assert(ship);
	if(numShips >= maxShips)
	{
		maxShips += 16;
		ships = (WdmShip **)RESIZE(ships, maxShips*4);
	}
	ships[numShips++] = ship;
}

void WdmObjects::DelShip(WdmShip * ship)
{
	Assert(ship);
	for(long i = 0; i < numShips; i++)
		if(ships[i] == ship)
		{
			ships[i] = ships[--numShips];
			return;
		}
	_asm int 3;
}

void WdmObjects::AddStorm(WdmStorm * storm)
{
	Assert(storm);
	if(numStorms >= maxStorms)
	{
		maxStorms += 16;
		storms = (WdmStorm **)RESIZE(storms, maxStorms*4);
	}
	storms[numStorms++] = storm;
}

void WdmObjects::DelStorm(WdmStorm * storm)
{
	Assert(storm);
	for(long i = 0; i < numStorms; i++)
		if(storms[i] == storm)
		{
			storms[i] = storms[--numStorms];
			return;
		}
	_asm int 3;
}

//Создать геометрию
GEOS * WdmObjects::CreateGeometry(const char * path)
{
	if(!path || !path[0] || !gs) return null;
	//Ищим среди добавленных
	dword hash = CalcHash(path);
	long i = hash & (sizeof(entryModels)/sizeof(entryModels[0]) - 1);
	for(i = entryModels[i]; i >= 0; i = models[i].next)
	{
		if(models[i].hash == hash)
		{
			if(stricmp(models[i].path, path) == 0)
			{
				return models[i].geo;
			}
		}
	}
	//Загружаем геометрию
	modelPath = "WorldMap\\";
	modelPath += path;
	gs->SetTexturePath("WorldMap\\Geometry\\");
	GEOS * geo = gs->CreateGeometry(modelPath, "", 0);
	gs->SetTexturePath("");
	//Добавляем в таблицу
	Model & m = models[models.Add()];
	m.path = path;
	m.hash = hash;
	m.next = -1;
	m.geo = geo;
	long index = hash & (sizeof(entryModels)/sizeof(entryModels[0]) - 1);
	if(entryModels[index] < 0)
	{
		entryModels[index] = i;
	}else{
		for(index = entryModels[index]; models[index].next >= 0; index = models[index].next);
		models[index].next = i;
	}
	return geo;
}

//============================================================================================
//Debug functions
//============================================================================================

#define WdmObjects_myPI 3.1415926535897932384626433832795f

WdmObjects::Vertex WdmObjects::vertex[1024];



void WdmObjects::DrawCircle(const CVECTOR & pos, float radius, dword color)
{
	static CMatrix mtx;
	mtx.SetPosition(pos.x, pos.y, pos.z);
	DrawCircle(mtx, radius, color);
}

void WdmObjects::DrawCircle(CMatrix & mtx, float radius, dword color)
{
	for(long i = 0; i < 64; i++)
	{
		vertex[i].v.x = radius*sinf(2.0f*WdmObjects_myPI*i/63);
		vertex[i].v.y = 0.0f;
		vertex[i].v.z = radius*cosf(2.0f*WdmObjects_myPI*i/63);
		vertex[i].c = color;
	}
	rs->TextureSet(0, -1);
	rs->TextureSet(1, -1);
	rs->SetTransform(D3DTS_WORLD, mtx);
	rs->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, D3DFVF_XYZ | D3DFVF_DIFFUSE, 62, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawVector(const CVECTOR & start, const CVECTOR & end, dword color)
{
	CVECTOR dir = end - start;
	float len = ~dir;
	if(len == 0.0f) return;
	len = sqrtf(len);
	dir *= 1.0f/len;
	float p = 0.9f*len;
	float r = 0.03f*len;

	for(long i = 0, imax = 8, t = 0; i < imax; i++)
	{
		float y1 = r*sinf(2.0f*WdmObjects_myPI*i/float(imax));
		float x1 = r*cosf(2.0f*WdmObjects_myPI*i/float(imax));
		float y2 = r*sinf(2.0f*WdmObjects_myPI*(i + 1)/float(imax));
		float x2 = r*cosf(2.0f*WdmObjects_myPI*(i + 1)/float(imax));
		//Добавляем треугольники 1 линии
		//0,0,0
		vertex[t*3 + 0].v.x = 0.0f;
		vertex[t*3 + 0].v.y = 0.0f;
		vertex[t*3 + 0].v.z = len;
		vertex[t*3 + 0].c = color;
		//0,1,1
		vertex[t*3 + 1].v.x = x1;
		vertex[t*3 + 1].v.y = y1;
		vertex[t*3 + 1].v.z = p;
		vertex[t*3 + 1].c = color;
		//0,2,2
		vertex[t*3 + 2].v.x = x2;
		vertex[t*3 + 2].v.y = y2;
		vertex[t*3 + 2].v.z = p;
		vertex[t*3 + 2].c = color;
		//1,0,3
		vertex[t*3 + 3] = vertex[t*3 + 2];
		//1,1,4
		vertex[t*3 + 4] = vertex[t*3 + 1];
		//1,2,5
		vertex[t*3 + 5].v.x = x1*0.3f;
		vertex[t*3 + 5].v.y = y1*0.3f;
		vertex[t*3 + 5].v.z = p;
		vertex[t*3 + 5].c = color;
		//2,0,6
		vertex[t*3 + 6] = vertex[t*3 + 2];
		//2,1,7
		vertex[t*3 + 7] = vertex[t*3 + 5];
		//2,2,8
		vertex[t*3 + 8].v.x = x2*0.3f;
		vertex[t*3 + 8].v.y = y2*0.3f;
		vertex[t*3 + 8].v.z = p;		
		vertex[t*3 + 8].c = color;
		//3,0,9
		vertex[t*3 + 10] = vertex[t*3 + 5];
		//3,1,10
		vertex[t*3 + 9] = vertex[t*3 + 8];
		//3,2,11
		vertex[t*3 + 11].v.x = x1*0.5f;
		vertex[t*3 + 11].v.y = y1*0.5f;
		vertex[t*3 + 11].v.z = 0.0f;
		vertex[t*3 + 11].c = color;
		//4,0,12
		vertex[t*3 + 12] = vertex[t*3 + 8];
		//4,1,13
		vertex[t*3 + 13] = vertex[t*3 + 11];
		//4,2,14
		vertex[t*3 + 14].v.x = x2*0.5f;
		vertex[t*3 + 14].v.y = y2*0.5f;
		vertex[t*3 + 14].v.z = 0.0f;
		vertex[t*3 + 14].c = color;
		//5,0,15
		vertex[t*3 + 15] = vertex[t*3 + 14];
		//5,1,16
		vertex[t*3 + 16] = vertex[t*3 + 11];
		//5,2,17
		vertex[t*3 + 17].v.x = 0.0f;
		vertex[t*3 + 17].v.y = 0.0f;
		vertex[t*3 + 17].v.z = 0.0f;
		vertex[t*3 + 17].c = color;

		t += 6;
	}
	Assert(t*3*6 < sizeof(vertex)/sizeof(Vertex));
	rs->TextureSet(0, -1);
	rs->TextureSet(1, -1);
	CVECTOR yBs = dir ^ CVECTOR(0.0f, 1.0f, 0.0f);
	if(~yBs == 0.0f)
	{
		yBs = dir ^ CVECTOR(1.0f, 0.0f, 0.0f);
		if(~yBs == 0.0f) yBs = dir ^ CVECTOR(0.0f, 0.0f, 1.0f);
		if(~yBs == 0.0f) return;
	}
	CMatrix mtx;
	mtx.Vx() = yBs ^ dir;
	mtx.Vy() = yBs;
	mtx.Vz() = dir;
	mtx.SetPosition(start.x, start.y, start.z);
	rs->SetTransform(D3DTS_WORLD, mtx);
	rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, t, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawLine(const CVECTOR & start, const CVECTOR & end, dword color)
{
	vertex[0].v = start;
	vertex[0].c = color;
	vertex[1].v = end;
	vertex[1].c = color;
	CMatrix mtx;
	rs->SetTransform(D3DTS_WORLD, mtx);
	rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, 1, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

void WdmObjects::DrawBox2D(CMatrix & mtx, float l, float w, dword color)
{
	vertex[0].v = CVECTOR(-w, 0.2f, l);
	vertex[0].c = color;
	vertex[1].v = CVECTOR(w, 0.2f, l);
	vertex[1].c = color;
	vertex[2].v = CVECTOR(w, 0.2f, l);
	vertex[2].c = color;
	vertex[3].v = CVECTOR(w, 0.2f, -l);
	vertex[3].c = color;
	vertex[4].v = CVECTOR(w, 0.2f, -l);
	vertex[4].c = color;
	vertex[5].v = CVECTOR(-w, 0.2f, -l);
	vertex[5].c = color;
	vertex[6].v = CVECTOR(-w, 0.2f, -l);
	vertex[6].c = color;
	vertex[7].v = CVECTOR(-w, 0.2f, l);
	vertex[7].c = color;
	rs->SetTransform(D3DTS_WORLD, mtx);
	rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, 4, vertex, sizeof(vertex[0]), "WdmDebugDraw");
}

//Получить размеры vp
void WdmObjects::GetVPSize(float & w, float & h)
{
	//Получим текущие размеры vp
	D3DVIEWPORT8 vp;
	rs->GetViewport(&vp);
	w = float(vp.Width);
	h = float(vp.Height);
}

//Получить направление и силу ветра
float WdmObjects::GetWind(float x, float z, CVECTOR & dir)
{
	windField.GetWind(x, z, dir.x, dir.z);
	dir.y = 0.0f;
	float len = sqrtf(dir.x*dir.x + dir.z*dir.z);
	if(len > 1e-20f)
	{
		dir.x /= len;
		dir.z /= len;
	}else{
		dir.x = 0.0f;
		dir.z = 1.0f;
	}
	if(len < 0.0f) len = 0.0f;
	if(len > 1.0f) len = 1.0f;
	return len;
}

//Обновить состояние ветра
void WdmObjects::UpdateWind(float dltTime)
{
	dltTime = dltTime / 2.0;
	windField.Step(dltTime);

	/*
	const long size = 64;
	for(long z = 0; z < size; z++)
	{
		for(long x = 0; x < size; x++)
		{
			float px = worldSizeX*(x/(float)size - 0.5f);
			float pz = worldSizeZ*(z/(float)size - 0.5f);
			float wx, wz;
			windField.GetWind(px, pz, wx, wz);
			DrawLine(CVECTOR(px, 1.0f, pz), CVECTOR(px, 1.0f, pz) + CVECTOR(wx, 0.0f, wz)*30.0f, 0xff00ff00);
		}
	}
	*/

}

//Получить строку сохранение
const char * WdmObjects::GetWindSaveString(string & windData)
{
	windData = "v02_";
	long size = sizeof(windField);
	AddDataToString(windData, byte(size >> 0));
	AddDataToString(windData, byte(size >> 8));
	AddDataToString(windData, byte(size >> 16));
	AddDataToString(windData, byte(size >> 24));
	const byte * buf = (const byte *)&windField;
	for(long i = 0; i < size; i++)
	{
		AddDataToString(windData, buf[i]);
	}
	return windData;
}

//Установить строку сохранение
void WdmObjects::SetWindSaveString(const char * str)
{
	if(!str)
	{
		windField.Reinit();
		return;
	}
	if(str[0] != 'v' || str[1] != '0' || str[2] != '2' || str[3] != '_')
	{
		windField.Reinit();
		return;
	}
	str += 4;
	long size = sizeof(windField);
	long testSize = 0;
	testSize |= (dword)GetDataFromString(str) << 0;
	testSize |= (dword)GetDataFromString(str) << 8;
	testSize |= (dword)GetDataFromString(str) << 16;
	testSize |= (dword)GetDataFromString(str) << 24;
	if(size != testSize)
	{
		windField.Reinit();
		return;
	}
	byte * buf = (byte *)&windField;
	for(long i = 0; i < size; i++)
	{
		long data = GetDataFromString(str);
		if(data < 0)
		{
			windField.Reinit();
			return;
		}
		buf[i] = (byte)data;
	}
}

//Добавить float в cтроку
void WdmObjects::AddDataToString(string & str, byte d)
{
	char hex[] = "0123456789ABCDEF";
	str += hex[(d >> 4) & 0xf];
	str += hex[(d >> 0) & 0xf];
}

//Получить float из строки
long WdmObjects::GetDataFromString(const char * & cur)
{
	dword tmp = 0;
	for(long cnt = 0; cnt < 2; cnt++)
	{
		if(!*cur) return -1;
		dword v;
		switch(*cur++)
		{
		case '0': v = 0x0; break;
		case '1': v = 0x1; break;
		case '2': v = 0x2; break;
		case '3': v = 0x3; break;
		case '4': v = 0x4; break;
		case '5': v = 0x5; break;
		case '6': v = 0x6; break;
		case '7': v = 0x7; break;
		case '8': v = 0x8; break;
		case '9': v = 0x9; break;
		case 'a':
		case 'A': v = 0xA; break;
		case 'b':
		case 'B': v = 0xB; break;
		case 'c':
		case 'C': v = 0xC; break;
		case 'd':
		case 'D': v = 0xD; break;
		case 'e':
		case 'E': v = 0xE; break;
		case 'f':
		case 'F': v = 0xF; break;
		default:
			return -1;
		}
		tmp = (tmp << 4) | v;
	}
	return tmp;
}


