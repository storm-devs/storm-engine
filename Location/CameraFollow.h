//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	CameraFollow
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _CameraFollow_H_
#define _CameraFollow_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"
#include "..\common_h\model.h"

class LocationCamera;


class CameraFollow  
{
	struct EdgeInfo
	{
		CVECTOR n;	//Нормаль к плоскости
		float d;	//Дистанция до плоскости
		CVECTOR s;	//Начало ребра
		CVECTOR e;	//Конец ребра
	};


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	CameraFollow();
	virtual ~CameraFollow();
	void SetLocationCamera(LocationCamera * locCamera);

//--------------------------------------------------------------------------------------------	
public:
	//Произвести расчёты
	void Update(float dltTime);

	//Найти текущую позицию камеры
	void MoveCamera(float dltTime);
	//Переинициализировать позицию камеры
	void BornCamera();
	//Вычислить позицию камеры для данного угла
	void CalcPosition(float ang, float radius, float dax, CVECTOR & pos);
	//Оценить радиус в искомом направлении на 1сек
	float FindRadius(float curAng);
	//Учесть полигон в поиске радиуса
	static bool ApplyPoly(const CVECTOR * v, long n);
	static void ApplyPolyFindNearest(EdgeInfo * e, long ne, const CVECTOR & pos);
	//Изменить радиус
	void ChangeRadius(float dltTime, float radius);
	//Найти ускорение остановки камеры при угловом столкновении с патчем
	void FindRotAccelCam();

	//Протянуть луч с учётом cull и размера камеры
	float Trace(const CVECTOR & src, const CVECTOR & dst);
	//Протянуть луч с учётом cull
	float SubTrace(const CVECTOR & src, const CVECTOR & dst);

	void DrawDebug();

//private:
public:
	LocationCamera * lc;

	CVECTOR camPos, lookTo;
	//Камера следующая за персонажем
	float camay;				//Текущее направление камеры
	float camradius;			//Текущий радиус
	bool isBrn;					//Если установленно то надо заново родить камеру
	float rotSpd;				//Скорость движения камеры на момент остановки
	float rotAcc;				//Ускорение остановки	
	float kRadInert;			//Зависимость инерции изменения радиуса от скорости поворота
	float kRadInertCur;			//Текущая зависимость инерции
	
	
	static float fndRadius;		//Текущий радиус при поиске
	static float fndMaxRadius;	//Максимальный радиус при поиске
	static CVECTOR fndCamDir;	//Нормализованное напровление на камеру
	static CVECTOR fndCamDirXZ;	//Нормализованное напровление на камеру в XZ
	static CVECTOR fndCamDirY;	//Нормализованное напровление на камеру в Y
	static CVECTOR fndCamPos;	//Точка наблюдения
	static float fndcsAx;		//Косинус ограничивающего угла по x
	static float fndkAx;		//1/(1 - cos(ax))
	static float fndcsAy;		//Косинус ограничивающего угла по y
	static float fndkAy;		//1/(1 - cos(ay))	
	static PLANE fndPlanes[32];	//Временно используемые плоскости граней
};

#endif

