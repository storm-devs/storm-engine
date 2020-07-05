//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmClouds
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmClouds.h"
#include "WdmCloud.h"
#include "WorldMap.h"
#include "WdmCamera.h"

//============================================================================================

#define WdmCloudsCloudRad		80.0f
#define WdmCloudsCloudHeight	80.0f
#define WdmCloudsSizeMin		30.0f
#define WdmCloudsSizeMax		50.0f

WdmClouds::Cloud::Cloud()
{
	count = 0;
}

WdmClouds::Cloud::~Cloud()
{
}

__forceinline bool WdmClouds::Cloud::Reset(bool isFirstTime)
{
	if(count > 0) return false;
	//Определяем количество
	count = long((rand()*(0.5f/RAND_MAX) + 0.5f)*WDMCLOUDSMAX);
	if(count < 1) count = 1;
	//Базовая позиция
	float baseX = (rand()*(1.0f/RAND_MAX) - 0.5f)*wdmObjects->worldSizeX;
	float baseZ = (rand()*(1.0f/RAND_MAX) - 0.5f)*wdmObjects->worldSizeZ;
	//Скалирующий фактор
	float scaleX = (rand()*(0.8f/RAND_MAX) + 0.2f);
	float scaleZ = (rand()*(0.8f/RAND_MAX) + 0.2f);
	float nrm = 1.0f/sqrtf(scaleX*scaleX + scaleZ*scaleZ);
	scaleX *= nrm; scaleZ *= nrm;
	//Определяем стартовые позиции
	for(long i = 0; i < count; i++)
	{
		float ang = rand()*(2.0f*PI/RAND_MAX);
		float rad = rand()*(WdmCloudsCloudRad/RAND_MAX);
		Cld & cld = cloud[i];
		cld.pos.x = baseX + rad*scaleX*sinf(ang);
		cld.pos.y = WdmCloudsCloudHeight;
		cld.pos.z = baseZ + rad*scaleZ*cosf(ang);
		cld.angle = rand()*(2.0f*PI/RAND_MAX);
		byte size = rand() & 0xff;
		cld.size = WdmCloudsSizeMin + size*((WdmCloudsSizeMax - WdmCloudsSizeMin)/255.0f);
		cld.alpha = 1.0f;
		dword dclr = rand() & 0xf;
		cld.color = 0x80f00000 | (dclr << 16) | size | ((rand() & 0x7f) << 24);
		cld.pict = rand() & 3;
		cld.index = 0;
		cld.aspd = (rand()*(1.0f/RAND_MAX) - 0.5f)*0.3f;
		cld.dir = 0.0f;
		ang = rand()*(2.0f*PI/RAND_MAX);
		rad = rand()*(0.3f/RAND_MAX) + 0.01f;
		cld.rdr.x = rad*sinf(ang);
		cld.rdr.y = 0.0f;
		cld.rdr.z = rad*cosf(ang);
	}
	if(!isFirstTime)
	{
		alpha = 0.0f;
	}else{
		alpha = 1.0f;
	}	
	lifeTime = 80.0f + rand()*(80.0f/RAND_MAX);
	return true;
}

//Расчёты
__forceinline void WdmClouds::Cloud::Update(float dltTime)
{
	if(count <= 0) return;
	const float minDist = WdmCloudsCloudRad*0.5f;
	const float midDist = WdmCloudsCloudRad*1.0f;
	const float maxDist = WdmCloudsCloudRad*1.5f;
	const float minWorldX = -0.5f*wdmObjects->worldSizeX;
	const float maxWorldX = 0.5f*wdmObjects->worldSizeX;
	const float minWorldZ = -0.5f*wdmObjects->worldSizeZ;
	const float maxWorldZ = 0.5f*wdmObjects->worldSizeZ;
	//Двигаем облака
	long outsideCount = 0;
	for(long i = 0; i < count; i++)
	{
		//Партикл
		Cld & cld = cloud[i];
		//Модифицируем направление
		if(cld.index >= count) cld.index = 0;
		CVECTOR dir = cloud[cld.index].pos - cld.pos;	//Направление	
		float dist = sqrtf(dir.x*dir.x + dir.z*dir.z);	//Получаем дистанцию
		if(dist > minDist)
		{
			//Модифицируем альфаканал
			if(dist < midDist)
			{
				cld.alpha += dltTime*0.06f;
				if(cld.alpha < 0.0f) cld.alpha = 0.0f;
			}else
			if(dist > maxDist)
			{
				cld.alpha -= dltTime*0.03f;
				if(cld.alpha < 0.0f) cld.alpha = 0.0f;
			}
			//Притягиваемся
			if(dist > midDist)
			{			
				float nrm = 1.0f/dist;
				dist = (dist - midDist)/(maxDist - midDist);
				if(dist > 1.0f) dist = 1.0f;
				dir *= nrm*dist;
			}else{
				dir = 0.0f;
			}
		}else{
			//Отталкиваемся
			dist = (dist - minDist)/minDist;
			if(dist < -1.0f) dist = -1.0f;
			dir *= dist;
			cld.alpha -= dltTime*0.05f;
			if(cld.alpha < 0.0f) cld.alpha = 0.0f;
		}
		cld.dir += (dir - cld.dir + cld.rdr)*(0.5f/count);	//Подбленжеваем независимо от времени
		cld.index++;										//На следующем кадре следующий будет действовать
		//Направление ветра
		wdmObjects->GetWind(cld.pos.x, cld.pos.z, dir);
		//Вращение
		float arot = cld.dir.z*dir.x - cld.dir.x*dir.z;
		if(arot < -1.0f) arot = -1.0f;
		if(arot > 1.0f) arot = 1.0f;
		cld.aspd += arot*dltTime*0.3f;
		if(cld.aspd < -1.0f) cld.aspd = -1.0f;
		if(cld.aspd > 1.0f) cld.aspd = 1.0f;
		cld.angle += cld.aspd*dltTime*0.2f;
		//Позиция		
		dir += cld.dir*0.5f;
		cld.pos += dir*(dltTime*10.0f);
		if(cld.pos.x < minWorldX || cld.pos.x > maxWorldX || cld.pos.z < minWorldZ || cld.pos.z > maxWorldZ)
		{
			outsideCount++;
		}
		//Учитываем партикл в боундсфере
		float cldr = cld.size*1.5f;
		if(i)
		{
			dir = center - cld.pos;
			float r = sqrtf(dir.x*dir.x + dir.z*dir.z);
			if(r + cldr < radius) continue;
			if(r > 1e-8f)
			{
				center = (center + cld.pos + dir*((radius - cldr)/r))*0.5f;
				radius = (r + radius + cldr)*0.5f;
			}else{
				if(radius < cldr) radius = cldr;
			}

		}else{
			center = cld.pos;
			radius = cldr;
		}
	}
	//Время жизни
	if(outsideCount >= count) lifeTime = 0.0f;
	lifeTime -= dltTime;
	if(lifeTime > 0.0f)
	{
		alpha += dltTime*0.1f;
		if(alpha > 1.0f) alpha = 1.0f;
	}else{
		alpha -= dltTime*0.1f;
		if(alpha <= 0.0f)
		{
			alpha = 0.0f;
			count = 0;			
		}
	}
}

__forceinline long WdmClouds::Cloud::FillRects(RS_RECT * rects, long cnt, float galpha)
{
	for(long i = 0; i < count; i++)
	{
		Cld & c = cloud[i];
		//Вычисляем альфу
		float a = float(byte(c.color >> 24));
		a *= alpha;
		a *= 0.1f + c.alpha*0.9f;
		long la = long(a*galpha);
		if(la > 0xff) la = 0xff;
		if(la <= 0) continue;
		//Заполняем
		RS_RECT & r = rects[cnt++];
		r.vPos = c.pos;
		r.fAngle = c.angle;
		r.fSize = c.size;
		r.dwColor = (c.color & 0x00ffffff) | (la << 24);
		r.dwSubTexture = c.pict;
	}
	return cnt;
}

//Получить центр сферы и радиус
__forceinline float WdmClouds::Cloud::GetBound(CVECTOR & _center)
{
	if(!count)
	{
		_center = 0.0f;
		return 0.0f;
	}
	_center = center;
	return radius;
}

//Запустить механизм удаления облака если есть пересечение
__forceinline void WdmClouds::Cloud::Kill(const Cloud & cld)
{
	if(count && cld.count)
	{
		float dx = center.x - cld.center.x;
		float dz = center.z - cld.center.z;
		float d = dx*dx + dz*dz;
		float minDist = (radius + cld.radius)*0.2f;
		if(d < minDist*minDist)
		{
			lifeTime = 0.0f;
		}
	}
}

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmClouds::WdmClouds()
{
	//Выставим позиции и направления группам
	for(long i = 0; i < sizeof(clouds)/sizeof(Cloud); i++)
	{
		clouds[i].Reset(true);
	}
	texture = wdmObjects->rs->TextureCreate("\\WorldMap\\clouds.tga");
	light = wdmObjects->rs->TextureCreate("\\WorldMap\\cloudslight.tga");
}

WdmClouds::~WdmClouds()
{
	if(texture >= 0) wdmObjects->rs->TextureRelease(texture);
	if(light >= 0) wdmObjects->rs->TextureRelease(light);
}

//Расчёты
void WdmClouds::Update(float dltTime)
{
	//Перезаводим убитые
	for(long i = 0; i < sizeof(clouds)/sizeof(Cloud); i++)
	{
		if(clouds[i].Reset()) break;
	}
	//Обновляем позиции
	for(long i = 0; i < sizeof(clouds)/sizeof(Cloud); i++)
	{
		clouds[i].Update(dltTime);
	}
	//Удаляем сильно пересекающиеся
	for(long i = 0; i < sizeof(clouds)/sizeof(Cloud) - 1; i++)
	{
		for(long j = i + 1; j < sizeof(clouds)/sizeof(Cloud); j++)
		{
			clouds[j].Kill(clouds[i]);
		}		
	}
}

//Рисование
void WdmClouds::LRender(VDX8RENDER * rs)
{
	long cnt = 0;
	//Получаем фрустум камеры
	PLANE * plane = rs->GetPlanes();
	//Определяем глобальную альфу в зависимости от дистанции до камеры
	CMatrix view;
	rs->GetTransform(D3DTS_VIEW, view);
	CVECTOR camPos;
	view.MulToInv(CVECTOR(0.0f), camPos);
	float alpha = (camPos.y - WdmCloudsCloudHeight - 10.0f)*1.0f/80.0f;
	if(alpha <= 0.0f) return;
	if(alpha > 1.0f) alpha = 1.0f;
	alpha *= alpha;
	//Рисуем видимые
	for(long i = 0, count = 0; i < sizeof(clouds)/sizeof(Cloud); i++)
	{
		//Получаем сферу
		CVECTOR c;
		float r = clouds[i].GetBound(c);		
		//Тестируем на видимость
		for(long j = 0; j < 4; j++)
		{
			PLANE & p = plane[j];
			float dist = c.x*p.Nx + c.y*p.Ny + c.z*p.Nz - p.D;
			if(dist < -r) break;
		}
		if(j < 4) continue;
		//Добавляем в буфер
		count = clouds[i].FillRects(rects, count, alpha);

		cnt++;
	}
	if(count > 0) 
	{
		rs->TextureSet(0, texture);
		rs->TextureSet(1, light);

		CMatrix prj;
		rs->GetTransform(D3DTS_PROJECTION, prj);
		view.Transposition();
		rs->SetVertexShaderConstant(0, prj, 4);
		prj.matrix[0] = view.matrix[1];
		prj.matrix[1] = view.matrix[5];
		prj.matrix[2] = view.matrix[9];
		prj.matrix[3] = view.matrix[13] - WdmCloudsCloudHeight;
		prj.matrix[4] = WdmCloudsSizeMin*1.4f;
		prj.matrix[5] = 0.0f;
		prj.matrix[6] = (WdmCloudsSizeMax - WdmCloudsSizeMin)*1.4f;
		prj.matrix[7] = 0.5f;
		rs->SetVertexShaderConstant(4, prj, 2);
		rs->DrawRects(rects, count, "WdmClouds", 2, 2);
	}
	//rs->Print(20, 200, "Visible clouds = %i, Visible particles = %i", cnt, count);
}

