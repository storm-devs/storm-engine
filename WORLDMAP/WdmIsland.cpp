//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmIsland
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmIsland.h"
#include "..\common_h\geometry.h"

//============================================================================================
//Данные для коллижена
//============================================================================================

CVECTOR WdmIsland::centPos;
long WdmIsland::numEdges;
CVECTOR WdmIsland::curPos;
bool WdmIsland::checkMode;


bool _cdecl WdmIsland_AddEdges(const GEOS::VERTEX * vrt, long numVrt);
bool _cdecl WdmIsland_OTest(const GEOS::VERTEX * vrt, long numVrt);

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmIsland::WdmIsland()
{
	wdmObjects.islands = this;
}

WdmIsland::~WdmIsland()
{
	wdmObjects.islands = null;
}

//Проверить на возможное столкновение
bool WdmIsland::CollisionTest(CMatrix & mtx, float length, float width, bool heighTest)
{
	if(!geo) return false;
	GEOS::PLANE p[5];
	//Вектор направления
	p[0].nrm.x = mtx.Vz().x;
	p[0].nrm.y = 0.0f;
	p[0].nrm.z = mtx.Vz().z;
	float radius = p[0].nrm.x*p[0].nrm.x + p[0].nrm.z*p[0].nrm.z;
	//Проскочим кривую модель
	if(radius < 0.000001f) return false;
	//Нормаль направления
	radius = 1.0f/sqrtf(radius);
	p[0].nrm.x *= radius;
	p[0].nrm.z *= radius;
	//Остальные нормали
	p[1].nrm.x = -p[0].nrm.x; p[1].nrm.y = 0.0f; p[1].nrm.z = -p[0].nrm.z;
	p[2].nrm.x = -p[0].nrm.z; p[2].nrm.y = 0.0f; p[2].nrm.z = p[0].nrm.x;
	p[3].nrm.x = p[0].nrm.z; p[3].nrm.y = 0.0f; p[3].nrm.z = -p[0].nrm.x;
	p[4].nrm.x = 0.0f; p[4].nrm.y = 1.0f; p[4].nrm.z = 0.0f;
	//Дистанции до плоскостей
	p[0].d = (p[0].nrm.x*length + mtx.Pos().x)*p[0].nrm.x + (p[0].nrm.z*length + mtx.Pos().z)*p[0].nrm.z;
	p[1].d = (p[1].nrm.x*length + mtx.Pos().x)*p[1].nrm.x + (p[1].nrm.z*length + mtx.Pos().z)*p[1].nrm.z;
	p[2].d = (p[2].nrm.x*width + mtx.Pos().x)*p[2].nrm.x + (p[2].nrm.z*width + mtx.Pos().z)*p[2].nrm.z;
	p[3].d = (p[3].nrm.x*width + mtx.Pos().x)*p[3].nrm.x + (p[3].nrm.z*width + mtx.Pos().z)*p[3].nrm.z;
	p[4].d = 0.2f;
	//Позиция сферы
	GEOS::VERTEX v; v.x = mtx.Pos().x; v.y = mtx.Pos().y; v.z = mtx.Pos().z;
	curPos = mtx.Pos();
	//Радиус
	radius = sqrtf(length*length + width*width)*(heighTest ? 2.0f : 1000000.0f);
	if(radius < 0.000001f) return false;
	//Ищем попадание полигонов
	numEdges = 0;
	centPos = 0.0f;
	checkMode = !heighTest;
	geo->Clip(p, heighTest ? 5 : 4, v, radius, WdmIsland_AddEdges);
	if(numEdges) centPos *= 1.0f/numEdges;
	//Исключим 
	return numEdges != 0;
}

bool _cdecl WdmIsland_AddEdges(const GEOS::VERTEX * vrt, long numVrt)
{
	//Пропустим кривые данные
	if(numVrt < 3) return true;
	if(WdmIsland::checkMode)
	{
		WdmIsland::centPos = CVECTOR(vrt[0].x, 0.0f, vrt[0].z);
		WdmIsland::numEdges = 1;
		return false;
	}
	//Попытаемся определить место положение точки относительно плоскости
	CVECTOR v1(vrt[1].x - vrt[0].x, vrt[1].y - vrt[0].y, vrt[1].z - vrt[0].z);
	CVECTOR v2(vrt[2].x - vrt[1].x, vrt[2].y - vrt[1].y, vrt[2].z - vrt[1].z);
	CVECTOR v = v1 ^ v2;
	float d = (v | WdmIsland::curPos) - (v | CVECTOR(vrt[0].x, vrt[0].y, vrt[0].z));
	if(d < 0) return true;
	//Добавляем рёбра
	WdmIsland::numEdges += numVrt;
	for(long i = 0; i < numVrt; i++)
	{
		WdmIsland::centPos += CVECTOR(vrt[i].x, 0.0f, vrt[i].z);
	}	
	//Продолжим
	return true;
}

//Проверить наличие в данном месте треугольников
bool WdmIsland::ObstacleTest(float x, float z, float radius)
{
	if(!geo || radius <= 0.0f) return false;
	GEOS::PLANE p[4];
	p[0].nrm.x = 0.0f; p[0].nrm.y = 0.0f; p[0].nrm.z = 1.0f; p[0].d = z + radius;
	p[1].nrm.x = 0.0f; p[1].nrm.y = 0.0f; p[1].nrm.z = -1.0f; p[1].d = -(z - radius);
	p[2].nrm.x = 1.0f; p[2].nrm.y = 0.0f; p[2].nrm.z = 0.0f; p[2].d = x + radius;
	p[3].nrm.x = -1.0f; p[3].nrm.y = 0.0f; p[3].nrm.z = 0.0f; p[3].d = -(x - radius);
	GEOS::VERTEX vrt;
	vrt.x = x;	
	vrt.y = 0.0f;
	vrt.z = z;	
	return geo->Clip(p, 4, vrt, 50.0f, 0);
}


