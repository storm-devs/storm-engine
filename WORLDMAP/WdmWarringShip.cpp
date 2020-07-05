//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmWarringShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmWarringShip.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmWarringShip::WdmWarringShip()
{
	shipType = wdmest_warring;
	numRects = 0;
	brnTime = 5.0f;
	texture = -1;
	texture = wdmObjects->rs->TextureCreate("\\WorldMap\\csmoke.tga");
}

WdmWarringShip::~WdmWarringShip()
{
	if(texture >= 0) wdmObjects->rs->TextureRelease(texture);
}

//Расчёты
void WdmWarringShip::Update(float dltTime)
{
	WdmEnemyShip::Update(dltTime);
	//Удружим противнику :)
	if(attack)
	{
		if(!isLive) attack->isLive = false;
		if(killMe) attack->killMe = true;
	}
	//Дым
	//Рожаем если надо
	brnTime += dltTime;
	if(!numRects && rand()*(20.0f/RAND_MAX) < brnTime - 5.0f)
	{
		brnTime = 0.0f;
		//Половина для левого борта
		for(long i = 0; i < sizeof(move)/(2*sizeof(MoveInfo)); i++, numRects++)
		{
			rect[numRects].vPos = mtx.Pos() + CVECTOR(0.0f, 2.0f, 0.0f);
			rect[numRects].vPos += mtx.Vz()*2.5f*(1.0f - rand()*2.0f/RAND_MAX);
			rect[numRects].dwColor = 0;
			rect[numRects].dwSubTexture = 0;
			rect[numRects].fAngle = rand()*6.0f/RAND_MAX;
			rect[numRects].fSize = 0.0f;
			move[numRects].dAng = 0.3f*(1.0f - rand()*2.0f/RAND_MAX);
			move[numRects].time = 0.0f;
			move[numRects].kTime = 1.0f/(2.0f + rand()*2.0f/RAND_MAX);
			move[numRects].v = mtx.Vx()*30.0f*(0.6f + rand()*(0.4f/RAND_MAX));
		}
		//Половина для правого борта
		for(i = 0; i < sizeof(move)/(2*sizeof(MoveInfo)); i++, numRects++)
		{
			rect[numRects].vPos = mtx.Pos() + CVECTOR(0.0f, 2.0f, 0.0f);
			rect[numRects].vPos += mtx.Vz()*2.5f*(1.0f - rand()*2.0f/RAND_MAX);
			rect[numRects].dwColor = 0;
			rect[numRects].dwSubTexture = 0;
			rect[numRects].fAngle = rand()*6.0f/RAND_MAX;
			rect[numRects].fSize = 0.0f;
			move[numRects].dAng = 0.3f*(1.0f - rand()*2.0f/RAND_MAX);
			move[numRects].time = 0.0f;
			move[numRects].kTime = 1.0f/(2.0f + rand()*2.0f/RAND_MAX);
			move[numRects].v = mtx.Vx()*-30.0f*(0.6f + rand()*(0.4f/RAND_MAX));
		}
	}
	//Двигаем
	for(long i = 0; i < numRects; i++)
	{
		MoveInfo & mi = move[i];
		mi.time += mi.kTime*dltTime;
		if(mi.time >= 1.0f)
		{
			//Убиваем партикл
			numRects--;
			for(long j = i; j < numRects; j++)
			{
				rect[j] = rect[j + 1];
				move[j] = move[j + 1];
			}
			continue;
		}
		float k = 1.0f - 12.0f*mi.time;
		if(k < 0.0f) k = 0.0f;
		rect[i].vPos += mi.v*k*k*dltTime;
		rect[i].fAngle += mi.dAng*dltTime;
		rect[i].fSize = 3.0f*(0.5f + mi.time*mi.time*0.9f);
		float alpha = sqrtf(mi.time);
		alpha = 1.0f - 4.0f*(alpha - 0.5f)*(alpha - 0.5f);
		Assert(alpha >= 0.0f && alpha <= 1.0f);
		alpha *= 255.0f*0.3f*this->alpha;
		rect[i].dwColor = (long(alpha) << 24) | 0xffffff;
	}
}

//Отрисовка
void WdmWarringShip::LRender(VDX8RENDER * rs)
{
	WdmEnemyShip::LRender(rs);
	//Дым
	if(isWMRender && numRects > 0)
	{
		rs->TextureSet(0, texture);
		//Render
		rs->DrawRects(rect, numRects, "WdmWarringShipClouds");
	}
}