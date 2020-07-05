//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmShip.h"
#include "WdmIslands.h"

//============================================================================================

#define WDM_SHIP_MAXDTIME		0.1f		//Наибольший допустимый временной отрезок для расчётов
#define WDM_SHIP_CLAMPDTIME		0.5f		//Наибольший допустимый временной отрезок в Update

//============================================================================================

#define WDM_SHIP_WMSZ	(sizeof(lines)/sizeof(Line))

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmShip::WdmShip()
{
	modelL05 = 6.0f;
	modelW05 = 1.9f;
	modelRadius2 = modelL05*modelL05 + modelW05*modelW05;
	modelRadius = sqrtf(modelRadius2);
	dltAx = 60.0f*(0.01f + rand()*(0.005f/RAND_MAX));
	dltAz = 60.0f*(0.02f + rand()*(0.005f/RAND_MAX));
	ax = ay = az = 0.0f;
	speed = 0.0f;
	rspeed = 0.0f;
	turnspd = 0.0f;
	slope = 0.0f;
	for(long i = 1; i < sizeof(lines)/sizeof(Line); i++)
	{
		lines[i].x = 0.0f;
		lines[i].z = 0.0f;
		lines[i].ay = 0.0f;
		lines[i].size = 0.0f;
	}
	wmtexture = -1;
	wdmObjects->AddShip(this);
	kMaxSpeed = 1.0f;
	wmtexture = wdmObjects->rs->TextureCreate("WorldMap\\watermark.tga");
	isLive = true;
	isSelect = false;
	collisionCounter = 0;
}

WdmShip::~WdmShip()
{
	wdmObjects->DelShip(this);
	wdmObjects->rs->TextureRelease(wmtexture);
	if((WdmShip *)wdmObjects->enemyShip == this) wdmObjects->enemyShip = null;
}

bool WdmShip::Load(const char * modelName)
{
	bool res = WdmRenderModel::Load(modelName);
	if(geo)
	{
		GEOS::INFO ginfo;
		geo->GetInfo(ginfo);
		modelL05 = ginfo.boxsize.z*0.25f;
		modelW05 = ginfo.boxsize.x*0.25f;
	}
	return res;
}

void WdmShip::Teleport(float x, float z, float ay)
{
	mtx.BuildMatrix(0.0f, ay, 0.0f, x, 0.0f, z);
	this->ay = ay;
	for(long i = 0; i < WDM_SHIP_WMSZ; i++)
	{
		lines[i].x = x;
		lines[i].z = z;
		lines[i].ay = ay;
		lines[i].size = 0.0f;
	}
}

void WdmShip::GetPosition(float & x, float & z, float & ay)
{
	x = mtx.Pos().x;
	z = mtx.Pos().z;
	ay = this->ay;
}

void WdmShip::SetMaxSpeed(float k)
{	
	if(k > 5.0f) k = 5.0f;
	if(k < 0.0f) k = 0.0f;
	kMaxSpeed = k;
}

void WdmShip::Update(float dltTime)
{
	if(dltTime > WDM_SHIP_CLAMPDTIME) dltTime = WDM_SHIP_CLAMPDTIME;
	for(; dltTime >= WDM_SHIP_MAXDTIME; dltTime -= WDM_SHIP_MAXDTIME) ShipUpdate(WDM_SHIP_MAXDTIME);
	if(dltTime > 0.0f) ShipUpdate(dltTime);
}

//Расчёты
void WdmShip::ShipUpdate(float dltTime)
{
	const float pi2 = 3.14159265f*2.0f;
	//Сбросим флажёк для рендера
	isWMRender = false;
	//Сохраним предыдущую позицию
	float oay = ay;	
	CVECTOR opos = mtx.Pos();
	//Ограничем скорости
	if(speed > WDM_SHIP_MAX_SPEED*kMaxSpeed) speed = WDM_SHIP_MAX_SPEED*kMaxSpeed;
	if(speed < -0.4f*WDM_SHIP_MAX_SPEED*kMaxSpeed) speed = -0.4f*WDM_SHIP_MAX_SPEED*kMaxSpeed;
	if(turnspd > WDM_SHIP_MAX_TURNSPD) turnspd = WDM_SHIP_MAX_TURNSPD;
	if(turnspd < -WDM_SHIP_MAX_TURNSPD) turnspd = -WDM_SHIP_MAX_TURNSPD;
	//Покачаем
	ax += dltAx*dltTime;
	az += dltAz*dltTime;
	//Поворачиваем
	ay += turnspd*dltTime;
	//Ограничем углы
	if(ax > pi2) ax -= pi2; if(ax < -pi2) ax += pi2;
	if(ay > pi2) ay -= pi2; if(ay < -pi2) ay += pi2;
	if(az > pi2) az -= pi2; if(az < -pi2) az += pi2;
	//Коэфициент зависимости скорости от ветра
	CVECTOR windDir = 0.0f;
	float kWind = wdmObjects->GetWind(mtx.Pos().x, mtx.Pos().z, windDir);
	kWind *= (sinf(ay)*windDir.x + cosf(ay)*windDir.z)*0.5f + 0.5f;
	kWind = wdmObjects->shipSpeedOppositeWind + (wdmObjects->shipSpeedOverWind - wdmObjects->shipSpeedOppositeWind)*kWind;
	//Найдём новую позицию
	CVECTOR pos = mtx.Pos();
	float dltMove = speed*kWind*dltTime;
	pos += mtx.Vz()*dltMove + rspeed*dltTime;
	baseV -= dltMove*0.04f;
	if(baseV < -1.0f) baseV += 1.0f;
	pos.y = 0.0f;
	float k = WDM_SHIP_INER_BR*dltTime;
	if(k > 1.0f) k = 1.0f;
	turnspd -= turnspd*k;
	speed -= k*speed;
	rspeed -= k*rspeed;
	slope += (turnspd*speed*(0.5f/WDM_SHIP_MAX_SPEED) - slope)*k;
	//Пересчитаем матрицу
	mtx.BuildMatrix(0.1f*sinf(ax), ay, 0.1f*sinf(az) + slope, pos.x, 0.0f, pos.z);
	//Коллижен с землёй
	if(wdmObjects->islands)
	{
		if(wdmObjects->islands->CollisionTest(mtx, modelL05, modelW05))
		{
			collisionCounter++;
			static CVECTOR moveDir = pos - wdmObjects->islands->centPos;
			if(collisionCounter > 10)
			{
				if(collisionCounter == 11)
				{
					if(~moveDir > 1e-20f)
					{
						moveDir = !moveDir;
					}else{
						moveDir = 1.0f;
					}
					moveDir *= 5.0f;
				}
				opos += moveDir*dltTime;
			}
			ay = oay;
			//Посчитаем реакцию на столкновение
			pos = wdmObjects->islands->centPos - pos;
			k = ~pos;
			if(k)
			{
				pos *= 1.0f/sqrtf(k);
				k = speed;
				if(k >= 0.0f)
				{
					if(k < 0.8f) k = 0.8f;
				}else{
					k = -0.1f;
				}
				rspeed += pos*(-0.8f)*(k);
				k = ~rspeed;
				
				mtx.MulToInvNorm(CVECTOR(pos), pos);
				turnspd = -1.5f*2.0f*pos.x*pos.z*(pos.z > 0.0f ? 2.0f : 1.0f);
				
				//Ограничем скорость отдачи
				if(k > 25.0f) rspeed *= 5.0f/k;
			}
			//Вернём на предыдущую позицию кораблик
			mtx.BuildMatrix(0.1f*sinf(ax), oay, 0.1f*sinf(az) + slope, opos.x, 0.0f, opos.z);
			speed = 0.0f;
			Collide();
			
		}else{
			collisionCounter = 0;
		}
	}
	//Ограничем координаты
	if(mtx.Pos().x < -0.5f*wdmObjects->worldSizeX) mtx.Pos().x = -0.5f*wdmObjects->worldSizeX;
	if(mtx.Pos().x > 0.5f*wdmObjects->worldSizeX) mtx.Pos().x = 0.5f*wdmObjects->worldSizeX;
	if(mtx.Pos().z < -0.5f*wdmObjects->worldSizeZ) mtx.Pos().z = -0.5f*wdmObjects->worldSizeZ;
	if(mtx.Pos().z > 0.5f*wdmObjects->worldSizeZ) mtx.Pos().z = 0.5f*wdmObjects->worldSizeZ;
	//Обновим след
	UpdateWaterMark(dltTime);
}

void WdmShip::LRender(VDX8RENDER * rs)
{
	if(wmtexture >= 0 && isWMRender)
	{
		isWMRender = false;
		//Рисуем след
		float al = alpha*255.0f;
		if(al < 0.0f) al = 0.0f;
		if(al > 255.0f) al = 255.0f;
		//Вершины
		static Vertex vrt[WDM_SHIP_WMSZ*2 + 1];
		float y = 0.01f;
		dword color = (dword(al) << 16) | (dword(al) << 8) | (dword(al) << 0) | 0xff000000;
		vrt[0].x = mtx.Pos().x + (lines[0].x - mtx.Pos().x)*1.2f;
		vrt[0].y = y;
		vrt[0].z = mtx.Pos().z + (lines[0].z - mtx.Pos().z)*1.2f;
		vrt[0].color = color;
		vrt[0].tu = 0.5f;
		vrt[0].tv = 0.0f;
		for(long i = 0; i < WDM_SHIP_WMSZ; i++)
		{
			float k = (WDM_SHIP_SPEED/5.0f)*powf((i + 1)*1.0f/WDM_SHIP_WMSZ, 0.4f)*lines[i].size;
			float dx = -cosf(lines[i].ay)*k;
			float dz = sinf(lines[i].ay)*k;
			k = (1.0f - powf(i*1.0f/(WDM_SHIP_WMSZ - 1.0f), 0.3f));//*lines[i].size;
			k = al*k;
			if(k < 0.0f) k = 0.0f;
			if(k > 255.0f) k = 255.0f;
			color = dword(k); color |= 0xff000000 | (color << 16) | (color << 8) | color;
			vrt[i*2 + 1].x = lines[i].x - dx;
			vrt[i*2 + 1].y = y;
			vrt[i*2 + 1].z = lines[i].z - dz;
			vrt[i*2 + 1].color = color;
			vrt[i*2 + 1].tu = 0.0f;
			vrt[i*2 + 1].tv = baseV + (i + 1)*(1.0f/WDM_SHIP_WMSZ);
			vrt[i*2 + 2].x = lines[i].x + dx;
			vrt[i*2 + 2].y = y;
			vrt[i*2 + 2].z = lines[i].z + dz;
			vrt[i*2 + 2].color = color;
			vrt[i*2 + 2].tu = 1.0f;
			vrt[i*2 + 2].tv = baseV + (i + 1)*(1.0f/WDM_SHIP_WMSZ);
		}
		//Отрисовываем стрипом
		//Textures
		rs->TextureSet(0, wmtexture);
		//Transform
		static CMatrix identity;
		rs->SetTransform(D3DTS_WORLD, identity);
		//Render
		rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, WDM_SHIP_WMSZ*2 - 1, vrt, sizeof(Vertex), "WdmShipWaterTrack");
	}else{
		isWMRender = true;
		//Рисуем модельку
		WdmRenderModel::LRender(rs);
	}
	//wdmObjects->DrawVector(rs, mtx.Pos(), mtx.Pos() + rspeed, 0xff00ff00);
}


void WdmShip::UpdateWaterMark(float dltTime)
{
	const float inrtPos = 5.5f;
	const float inrtAng = 3.5f;
	const float inrtSize = 12.0f;
	const float pi = 3.14159265f;
	float kSpeed = speed*(1.5f/WDM_SHIP_MAX_SPEED);
	if(kSpeed < 0.0f) kSpeed = 0.0f;
	if(kSpeed > 0.8f) kSpeed = 0.8f;
	lines[0].x = mtx.Pos().x + sinf(ay)*modelL05*0.4f*kSpeed;
	lines[0].z = mtx.Pos().z + cosf(ay)*modelL05*0.4f*kSpeed;
	lines[0].ay = ay;
	lines[0].size = kSpeed*modelW05*0.36f;
	float kp = inrtPos*dltTime;
	if(kp > 1.0f) kp = 1.0f;
	float ka = inrtAng*dltTime;
	if(ka > 1.0f) ka = 1.0f;
	float ks = inrtSize*dltTime;
	if(ks > 10.0f) ks = 10.0f;
	ks *= modelW05*1.33f;
	for(long i = 1; i < WDM_SHIP_WMSZ; i++)
	{
		lines[i].x += (lines[i - 1].x - lines[i].x)*kp;
		lines[i].z += (lines[i - 1].z - lines[i].z)*kp;
		while(lines[i - 1].ay - lines[i].ay > pi)
			lines[i].ay += 2*pi;
		while(lines[i - 1].ay - lines[i].ay < -pi)
			lines[i].ay -= 2*pi;
		lines[i].ay += (lines[i - 1].ay - lines[i].ay)*ka;
		float kSize = 1.0f - i/float(WDM_SHIP_WMSZ);
		kSize = kSize*kSize*kSpeed*0.6f;
		lines[i].size += (lines[i - 1].size - lines[i].size + kSize)*ks;
	}
}


//true если свободно
bool WdmShip::CheckPosition(float x, float z, float objRadius)
{
	//Мир
	if(x - objRadius < -0.5f*wdmObjects->worldSizeX) return false;
	if(x + objRadius > 0.5f*wdmObjects->worldSizeX) return false;
	if(z - objRadius < -0.5f*wdmObjects->worldSizeZ) return false;
	if(z + objRadius > 0.5f*wdmObjects->worldSizeZ) return false;
	//Корабли
	for(long i = 0; i < wdmObjects->numShips; i++)
	{
		if(!wdmObjects->ships[i]->isLive) continue;
		if(wdmObjects->ships[i]->killMe) continue;
		float dx = wdmObjects->ships[i]->mtx.Pos().x - x;
		float dz = wdmObjects->ships[i]->mtx.Pos().z - z;
		if(dx*dx + dz*dz < wdmObjects->ships[i]->modelRadius2) return false;
	}
	//Острова
	if(i >= wdmObjects->numShips)
	{
		if(wdmObjects->islands)
		{
			if(!wdmObjects->islands->ObstacleTest(x, z, objRadius)) return true;
		}else return true;
	}
	return false;
}



