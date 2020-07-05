//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	CameraFollow
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "CameraFollow.h"
#include "LocationCamera.h"
#include "..\common_h\model.h"
#include "Location.h"
#include "Character.h"




//============================================================================================

//#define LFC_DEBUG

#define LCF_RADIUS	0.25f

//Параметры поиска радиуса
float CameraFollow::fndRadius;
float CameraFollow::fndMaxRadius;
CVECTOR CameraFollow::fndCamDir;
CVECTOR CameraFollow::fndCamDirXZ;
CVECTOR CameraFollow::fndCamDirY;
CVECTOR CameraFollow::fndCamPos;
float CameraFollow::fndcsAx;
float CameraFollow::fndkAx;
float CameraFollow::fndcsAy;
float CameraFollow::fndkAy;

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

CameraFollow::CameraFollow()
{
	lc = null;
	isBrn = true;
	camay = 0.0f;
	kRadInert = 1.0f;
	kRadInertCur = 1.0f;
}

CameraFollow::~CameraFollow()
{

}

void CameraFollow::SetLocationCamera(LocationCamera * locCamera)
{
	lc = locCamera;
	camradius = lc->radius;
}

//============================================================================================

//Произвести расчёты
void CameraFollow::Update(float dltTime)
{
	CVECTOR oldPos(camPos);
	//Если дальше допустимого рождаемся заново
	if(~(lc->pos - oldPos) >= lc->teleport_dist*lc->teleport_dist) isBrn = true;
	//Расчитываем новую позицию
	if(!isBrn)
	{
		MoveCamera(dltTime);
	}else{
		BornCamera();
		isBrn = false;
		oldPos = camPos;
	}
	//Пересчитаем угол
	/*
	double vz = lookTo.z - camPos.z;
	double vx = lookTo.x - camPos.x;
	double lv = vx*vx + vz*vz;
	if(lv > 0.0000001)
	{
		//Ищем углы
		vz = acos(vz/sqrt(lv));
		if(vx < 0) vz = -vz;
		camay = float(vz);
	}*/
	lc->camPos = camPos;
	lc->lookTo = lookTo;
	//Ограничим по высоте
	if(lc->camPos.y < lc->pos.y + lc->lheight*0.5f) lc->camPos.y = lc->pos.y + lc->lheight*0.5f;




	/*
	//Проверим ситуацию застревания камеры
	float k1 = Trace(lookTo, camPos);
	if(k1 < 1.0f)
	{
		if(Trace(camPos, lookTo) <= 1.0f)
		{
			CVECTOR dir = camPos - lookTo;
			float l = ~dir;
			if(l > 0.0f)
			{
				k1 -= LCF_RADIUS/sqrtf(l);
				if(k1 < 0.0f) k1 = 0.0f;
			}
			camPos = lookTo + (camPos - lookTo)*k1;
		}
	}
	//
	CVECTOR dir = camPos - lookTo; dir.y = 0.0f;
	float r = sqrtf(~dir);
	lc->camPos = camPos;
	lc->lookTo = lookTo;
	if(r < lc->chradius*1.5f)
	{
		float k = 1.0f - r/(lc->chradius*1.5f);
		float clipH = lc->lookTo.y + k*(lc->height - lc->lheight + 0.2f)*1.2f;
		if(lc->camPos.y < clipH)
		{
			lc->lookTo.y += clipH - lc->camPos.y;
			lc->camPos.y = clipH;
		}
	}*/
	
}

//Найти текущую позицию камеры
void CameraFollow::MoveCamera(float dltTime)
{
	if(dltTime <= 0.0f) return;
	//Куда смотрим
	float likeCamay = lc->chay;
	if(lc->isSpecialMode)
	{
		likeCamay = camay + lc->dAy;
	}
	lookTo = lc->pos; lookTo.y += lc->lheight;
	//Решим, что делать
	const float pi = 3.14159265359f;
	float chay = likeCamay - long(likeCamay/pi)*2.0f*pi;
	float cay = camay - long(camay/pi)*2.0f*pi;
	if(cay - chay > pi) chay += 2*pi;
	if(cay - chay < -pi) chay -= 2*pi;
	float dAng = chay - cay;	
	//Расчитываем текущий радиус
	kRadInert = fabsf(dAng)/0.05f;
	if(kRadInert > 1.0f) kRadInert = 1.0f;
	kRadInert = powf(kRadInert, 5.0f);
	if(fabsf(dAng) > 0.001f)
	{
		//Камера вращается вокруг персонажа
		float k = dltTime*lc->rotInertia;
		if(k > 1.0f) k = 1.0f;
		camay = cay + dAng*k;
		lc->character->LockRotate(true);
	}	
	ChangeRadius(dltTime, FindRadius(camay));
	CalcPosition(camay, camradius, 0.0f, camPos);
}

//Переинициализировать позицию камеры
void CameraFollow::BornCamera()
{
	//Новые параметры камеры
	camay = lc->chay;
	camradius = lc->radius;
	//Куда смотрим
	lookTo = lc->pos; lookTo.y += lc->lheight;
	//Откуда смотрим
	CalcPosition(camay, camradius, 0.0f, camPos);
	//Проверяем на пересечение луч
	float kp = 2.0f;
	lc->Trace(lookTo, camPos);
	if(kp < 1.0f) camPos = lookTo + (camPos - lookTo)*(kp*0.9f);
}

//Вычислить позицию камеры для данного угла
void CameraFollow::CalcPosition(float ang, float radius, float dax, CVECTOR & pos)
{
	//Откуда смотрим
	float ax = -lc->ax + dax;
	pos.x = lc->pos.x - radius*cosf(ax)*sinf(ang);
	pos.y = lc->pos.y + lc->lheight - radius*sinf(ax);
	pos.z = lc->pos.z - radius*cosf(ax)*cosf(ang);
}

#ifdef LFC_DEBUG

CVECTOR fndv[4096*3];
long numTrg = 0;
bool isFndTrg = false;
CVECTOR fndvP1[4096];
long numP1 = 0;
CVECTOR fndvP2[4096];
long numP2 = 0;
CVECTOR fndvP3[4096*10];
long numP3 = 0;

CVECTOR fndvN[4096*10];
long numN = 0;
CVECTOR fndvE[4096*10];
long numE = 0;

CVECTOR fndvU[4096*10];
long numU = 0;

long numPoly1 = 0;

int camSteps = 0;

#endif

void CameraFollow::DrawDebug()
{
#ifdef LFC_DEBUG
	lc->location->GetRS()->SetTransform(D3DTS_WORLD, CMatrix());
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff00ff00);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ, numTrg, fndv, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffff00);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_POINTLIST, D3DFVF_XYZ, numP1, fndvP1, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_POINTLIST, D3DFVF_XYZ, numP2, fndvP2, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffff0000);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, numP3/2, fndvP3, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffff00ff);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, numN/2, fndvN, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff0000ff);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, numE/2, fndvE, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff00ffff);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, numU/2, fndvU, sizeof(CVECTOR), "DebugTrs");
	lc->location->GetRS()->Print(10, 90, "NumEdges: %i, NumU %i", numE/2, numU/2);
#endif
}

//
float CameraFollow::FindRadius(float curAng)
{
	static const float pi = 3.14159265359f;
	static const float day = pi*0.25f;		//Отклонение по вертикали
	static const float dax = pi*0.16f;		//Отклонение по вертикали

#ifdef LFC_DEBUG
	struct Vrt
	{
		CVECTOR p;
		float rhw;
	} vrt[64];

	for(long ii = 0; ii < 32; ii++)
	{
		float x = day*ii/31.0f;
		vrt[ii*2 + 0].p.x = 300.0f;
		vrt[ii*2 + 0].p.y = 400.0f;
		vrt[ii*2 + 0].p.z = 0.5f;
		vrt[ii*2 + 0].rhw = 2.0f;
		vrt[ii*2 + 1].p.x = vrt[ii*2 + 0].p.x + 200.0f*sinf(x);
		vrt[ii*2 + 1].p.y = vrt[ii*2 + 0].p.y - 200.0f*cosf(x);
		vrt[ii*2 + 1].p.z = 0.5f;
		vrt[ii*2 + 1].rhw = 2.0f;
	}
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffff0000);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZRHW, 32, vrt, sizeof(Vrt), "DebugTrs");

	for(ii = 0; ii < 32; ii++)
	{
		float x = day*ii/31.0f;
		float y = (cosf(x) - cosf(day))/(1.0f - cosf(day));

		y = 200.0f + (100.0f - 200.0f)*(1.0f - powf(1.0f - y, 1.5f));

		vrt[ii*2 + 0].p.x = 300.0f;
		vrt[ii*2 + 0].p.y = 400.0f;
		vrt[ii*2 + 0].p.z = 0.5f;
		vrt[ii*2 + 0].rhw = 2.0f;
		vrt[ii*2 + 1].p.x = vrt[ii*2 + 0].p.x + y*sinf(x);
		vrt[ii*2 + 1].p.y = vrt[ii*2 + 0].p.y - y*cosf(x);
		vrt[ii*2 + 1].p.z = 0.5f;
		vrt[ii*2 + 1].rhw = 2.0f;
	}
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff00ff00);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZRHW, 32, vrt, sizeof(Vrt), "DebugTrs");

	for(ii = 0; ii < 32; ii++)
	{
		float x = day*ii/31.0f;
		vrt[ii*2 + 0].p.x = 300.0f;
		vrt[ii*2 + 0].p.y = 400.0f;
		vrt[ii*2 + 0].p.z = 0.5f;
		vrt[ii*2 + 0].rhw = 2.0f;
		vrt[ii*2 + 1].p.x = vrt[ii*2 + 0].p.x + 100.0f*sinf(x);
		vrt[ii*2 + 1].p.y = vrt[ii*2 + 0].p.y - 100.0f*cosf(x);
		vrt[ii*2 + 1].p.z = 0.5f;
		vrt[ii*2 + 1].rhw = 2.0f;
	}
	lc->location->GetRS()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff0000ff);
	lc->location->GetRS()->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZRHW, 32, vrt, sizeof(Vrt), "DebugTrs");
	
#endif

	//Точка на которую смотрим
	CVECTOR pos(lc->pos.x, lc->pos.y + lc->lheight, lc->pos.z);
	float ax = -lc->ax;
	//Начальные значения поиска
	fndRadius = lc->radius + LCF_RADIUS;
	fndMaxRadius = lc->radius + LCF_RADIUS;
	fndCamDir = CVECTOR(-cosf(ax)*sinf(curAng), -sinf(ax), -cosf(ax)*cosf(curAng));
	fndCamDirXZ = fndCamDir; fndCamDirXZ.y = 0.0f;
	float l = sqrtf(~fndCamDirXZ);
	if(l > 0.0f) fndCamDirXZ *= 1.0f/l;
	fndCamDirY = fndCamDir; fndCamDirY.x = l; fndCamDirY.z = 0.0f;
	l = ~fndCamDirY;
	if(l > 0.0f) fndCamDirY *= 1.0f/sqrtf(l);
	fndCamPos = pos;
	fndcsAx = cosf(dax);
	fndkAx = 1.0f/(1.0f - fndcsAx);
	fndcsAy = cosf(day);
	fndkAy = 1.0f/(1.0f - fndcsAy);
	//Строим ограничивающий объём
	static PLANE p[5];
	//Левая плоскость
	p[0].Nx = -cosf(curAng + day);
	p[0].Ny = 0.0f;
	p[0].Nz = sinf(curAng + day);
	p[0].D = pos.x*p[0].Nx + pos.z*p[0].Nz;
	//Правая плоскость
	p[1].Nx = cosf(curAng - day);
	p[1].Ny = 0.0f;
	p[1].Nz = -sinf(curAng - day);
	p[1].D = pos.x*p[1].Nx + pos.z*p[1].Nz;
	//Нижняя плоскость
	p[2].Nx = sinf(ax + dax)*sinf(curAng);
	p[2].Ny = -cosf(ax + dax);
	p[2].Nz = sinf(ax + dax)*cosf(curAng);
	p[2].D = pos.x*p[2].Nx + pos.y*p[2].Ny + pos.z*p[2].Nz;
	//Верхняя плоскость
	p[3].Nx = -sinf(ax - dax)*sinf(curAng);
	p[3].Ny = cosf(ax - dax);
	p[3].Nz = -sinf(ax - dax)*cosf(curAng);
	p[3].D = pos.x*p[3].Nx + pos.y*p[3].Ny + pos.z*p[3].Nz;
	//Задняя стенка
	p[4].Nx = fndCamDir.x;
	p[4].Ny = fndCamDir.y;
	p[4].Nz = fndCamDir.z;
	p[4].D = fndCamDir | (pos + fndCamDir*lc->radius);

#ifdef LFC_DEBUG
	numPoly1 = 0;
	camSteps = 0;
	//!!!
	if(api->Controls->GetDebugAsyncKeyState('8') < 0)
	{
		isFndTrg = true;
		numTrg = 0;
		numP1 = 0;		
		numP2 = 0;
		numP3 = 0;
		numN = 0;
		numE = 0;
		numU = 0;
	}
#endif
	
	
	//Ишем радиус
	lc->Clip(p, 5, pos, lc->radius, ApplyPoly);

#ifdef LFC_DEBUG
	lc->location->GetRS()->Print(10, 10, "NumPoly: %i, steps: %i", numPoly1, camSteps);
	lc->location->GetRS()->Print(10, 30, "pos: %f, %f, %f", pos.x, pos.y, pos.z);
	lc->location->GetRS()->Print(10, 50, "curAng: %f, ax: %f rad = %f", curAng, ax, lc->radius);
	
#endif

	//isFndTrg = false;

#ifdef LFC_DEBUG
	/////////////////////
	static CVECTOR ps(0.0f);
	static CVECTOR vv[5];
	if(api->Controls->GetDebugAsyncKeyState('8') < 0)
	{
		ps = pos;
		CVECTOR vvv;
		CalcPosition(curAng, 2.0f, 0.0f, vvv);		
		
		vv[0] = ps + CVECTOR(p[0].Nx, p[0].Ny, p[0].Nz);
		vv[1] = ps + CVECTOR(p[1].Nx, p[1].Ny, p[1].Nz);
		vv[2] = ps + CVECTOR(p[2].Nx, p[2].Ny, p[2].Nz);
		vv[3] = ps + CVECTOR(p[3].Nx, p[3].Ny, p[3].Nz);
		vv[4] = ps + (vvv - pos);
	}	

	
	lc->location->DrawLine(ps, 0xffff0000, vv[0], 0xffffff00);
	lc->location->DrawLine(ps, 0xff00ff00, vv[1], 0xffffff00);
	lc->location->DrawLine(ps, 0xff0000ff, vv[2], 0xffffff00);
	lc->location->DrawLine(ps, 0xffff00ff, vv[3], 0xffffff00);
	lc->location->DrawLine(ps, 0xffff00ff, vv[4], 0xffffff00);
	

	Assert(p[0].Ny == 0.0f);
	Assert(p[1].Ny == 0.0f);
	/////////////////////
#endif

	fndRadius -= LCF_RADIUS;
	if(fndRadius < 0.1f) fndRadius = 0.1f;
	return fndRadius;
}

//Учесть полигон в поиске радиуса
bool CameraFollow::ApplyPoly(const CVECTOR * v, long n)
{
#ifdef LFC_DEBUG
	numPoly1++;
#endif
	if(n < 3) return true;
	//Нормаль плоскости
	CVECTOR norm = (v[0] - v[1])^(v[0] - v[2]);
	float dist = ~norm;
	if(dist <= 1e-10f) return true;
	norm *= 1.0f/sqrtf(dist);
	//Дистанция
	dist = v[0] | norm;
	//Дистанция до плоскости полигона
	float d = (fndCamPos | norm) - dist;
	//Если плоскость дальше чем текущий радиус, пропустим её
	if(fabs(d) >= fndRadius) return true;
	//Заполняем параметры плоскостей, проходящих через рёбра
	static EdgeInfo edge[32];
	if(n > 32) n = 32;
	for(long i = 0; i < n; i++)
	{
		//Ребро
		long j = i + 1;
		if(j >= n) j = 0;
		//Нормаль плоскости проходящей через ребро
		edge[i].n = (v[i] - v[j]) ^ norm;
		float d = ~edge[i].n;
		if(d == 0.0f) continue;
		edge[i].n *= 1.0f/sqrtf(d);
		//Дистанция
		edge[i].d = v[i] | edge[i].n;
		edge[i].s = v[i];
		edge[i].e = v[j];
	}
	//Ищем ближайшию точку к позиции куда смотрит камера
	CVECTOR pos1 = fndCamPos - norm*d;
	//Ищем ближайшию точку к точке пересечения луча направления на камеру
	float cs = norm | fndCamDir;
	if(cs != 0.0f)
	{
		d /= -cs;
		if(d >= 0.0f)
		{
			CVECTOR pos2 = fndCamPos + fndCamDir*d;
			CVECTOR dlt = pos2 - pos1;
			float step = sqrtf(~dlt)*3.0f;
			if(step > 0.0f)
			{
				if(step > 10.0f) step = 10.0f;
				step = float(int(step));
				if(step < 1.0f) step = 1.0f;
#ifdef LFC_DEBUG
				if(camSteps < int(step)) camSteps = int(step);
#endif
				step = 1.0f/step;
				for(float k = 0.0f; k <= 1.0f; k += step)
				{
					ApplyPolyFindNearest(edge, n, pos1 + dlt*k);
				}				
			}else ApplyPolyFindNearest(edge, n, pos1);
		}else ApplyPolyFindNearest(edge, n, pos1);
	}else ApplyPolyFindNearest(edge, n, pos1);
	return true;
}

void CameraFollow::ApplyPolyFindNearest(EdgeInfo * e, long ne, const CVECTOR & pos)
{
	//Тестируем с рёбрами, определяя близжайшее растояние
	CVECTOR pnt;
	float minDist = -1.0f;
	for(long i = 0; i < ne; i++)
	{
		//Дистанция до плоскости от точки
		float d = (pos | e[i].n) - e[i].d;
		if(d >= 0.0f) continue;
		//Точка на прямой
		CVECTOR p = pos - e[i].n*d;
		//Точка на ребре
		if(((e[i].s - e[i].e) | (e[i].s - p)) < 0.0f) p = e[i].s;
			else
				if(((e[i].s - e[i].e) | (p - e[i].e)) < 0.0f) p = e[i].e;
		//Дистанция
		d = ~(p - pos);
		if(minDist >= 0.0f)
		{
			if(minDist > d)
			{
				minDist = d;
				pnt = p;
			}
		}else{
			minDist = d;
			pnt = p;
		}
	}
	//Вектор направления на ближайшую точку
	if(minDist < 0.0f) pnt = pos - fndCamPos; else pnt = pnt - fndCamPos;
	//Длинна
	float dist = ~pnt;
	if(dist >= fndRadius*fndRadius) return;
	//Коэфициент ослабления
	//XZ
	float d = sqrtf(pnt.x*pnt.x + pnt.z*pnt.z);
	float kxz = 1.0f;
	if(d > 0.0f)
	{
		kxz = (pnt | fndCamDirXZ)/d;
		kxz = (kxz - fndcsAy)*fndkAy;
		if(kxz <= 0.0f) return;
	}
	//Y
	CVECTOR norm = pnt;
	pnt.x = d; pnt.z = 0.0f;
	d = sqrtf(pnt.x*pnt.x + pnt.y*pnt.y);
	float ky = 1.0f;
	if(d > 0.0f)
	{
		ky = (pnt | fndCamDirY)/d;
		ky = (ky - fndcsAx)*fndkAx;
		if(ky <= 0.0f) return;
	}
	//Дистанция с учётом коэфициентов поправки
	float oldDist = dist;
	dist = fndMaxRadius + (sqrtf(dist) - fndMaxRadius)*float(1.0 - powf(fabsf(1.0f - kxz*ky), 1.5f));
	Assert(dist >= 0.0f);
	if(dist < fndRadius) fndRadius = dist;
}


//Изменить радиус
void CameraFollow::ChangeRadius(float dltTime, float radius)
{
	if(radius < 0.0f) radius = 0.0f;
	if(radius > lc->radius) radius = lc->radius;
	float k = dltTime*10.0f;
	if(k > 0.99f) k = 0.99f;
	//kRadInertCur = kRadInert + (kRadInertCur - kRadInert)*k;
	//k = dltTime*lc->radInertia*(0.1f + kRadInertCur*10.0f);
	if(radius > camradius) k *= 0.7f;
	if(k < 0.0f) k = 0.0f;
	if(k > 1.0f) k = 1.0f;
	camradius += (radius - camradius)*k;
//	lc->location->GetRS()->Print(10, 70, "fndRadius = %f, camradius = %f", fndRadius, camradius);
}

//Найти ускорение остановки камеры при угловом столкновении с патчем, dir +-1
void CameraFollow::FindRotAccelCam()
{
	rotAcc = 0.0f;
	if(rotSpd > 0.0f) rotAcc = -3.0f;
	if(rotSpd < 0.0f) rotAcc = 3.0f;	
}

//Протянуть луч с учётом cull и размера камеры
float CameraFollow::Trace(const CVECTOR & src, const CVECTOR & dst)
{
	//Направление на камеру
	CVECTOR dir = dst - src;
	float dist = ~dir;
	if(dist <= 0.0f) return 2.0f;
	dist = sqrtf(dist); dir *= (dist + LCF_RADIUS)/dist;
	//Проверка центрального луча
	float k[5];
	k[0] = SubTrace(src, src + dir);
	//Строим базис
	CVECTOR left = dir ^ CVECTOR(0.0f, 1.0f, 0.0f);
	float l = ~left;
	if(l <= 0.0f) return k[0];	
	left *= 1.0f/sqrtf(l);
	CVECTOR up = dir ^ left;
	//Ищем ближайшию дистанцию
	CVECTOR s;
	s = src + left*LCF_RADIUS;
	k[1] = SubTrace(s, s + dir);
	s = src - left*LCF_RADIUS;
	k[2] = SubTrace(s, s + dir);
	s = src + up*LCF_RADIUS;
	k[3] = SubTrace(s, s + dir);
	s = src - up*LCF_RADIUS;
	k[4] = SubTrace(s, s + dir);
	float kRes = 2.0f;
	for(long i = 0; i < 5; i++) if(kRes > k[i]) kRes = k[i];
	return kRes;
	
}

//Протянуть луч с учётом cull
inline float CameraFollow::SubTrace(const CVECTOR & src, const CVECTOR & dst)
{
	TRIANGLE trg;
	float k = lc->Trace(src, dst);
	if(k > 1.0f || !lc->GetCollideTriangle(trg)) return 2.0f;
	//Определяем нормаль
	CVECTOR n = (trg.vrt[0] - trg.vrt[1]) ^ (trg.vrt[1] - trg.vrt[2]);
	float l = ~n;
	if(l > 0.0f)
	{
		//Оперделим с какой стороны проходим через треугольник
		n *= 1.0f/sqrtf(l);
		if((n | src) > (n | trg.vrt[0])) return k;
	}
	return 2.0f;
}


