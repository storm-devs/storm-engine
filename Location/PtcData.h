//============================================================================================
//	Spirenkov Maxim, 2002
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	PtcData
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _PtcData_H_
#define _PtcData_H_

#include "..\common_h\vmodule_api.h"
#include "ptc.h"


#define PTCDATA_MAXSTEPS	32

class VDX8RENDER;


class PtcData
{
public:
	struct Triangle
	{
		long index;
		CVECTOR v[3];
		CVECTOR n;
	};

	struct DbgVertex
	{
		float x, y, z;
		dword c;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	PtcData();
	virtual ~PtcData();

	//Загрузить патч
	bool Load(const char * path);
	//Определить текущию позицию
	long FindNode(const CVECTOR & pos, float & y);
	//Двигает pos в to, возвращает новый нод
	long Move(long curNode, const CVECTOR & to, CVECTOR & pos, long depth = 0);
	//Получить нормаль к ноду
	void GetNodeNormal(long curNode, CVECTOR & n);
	//Найти направление пути
	bool FindPathDir(long curNode, const CVECTOR & cur, long toNode, const CVECTOR & to, long & node, CVECTOR & toPos);
	//Найти пересечение с патчём
	float Trace(const CVECTOR & s, const CVECTOR & d);
	//Найти силу отталкивающую от краёв
	void FindForce(long curNode, CVECTOR & force);
	//Получить материал нода
	const char * GetMaterial(long curNode);

	//Получить треугольники пересекающии данный квадрат
	Triangle * GetTriangles(float x, float z, float sx, float sz, long & num);


	//Результаты последнего перемещения
	bool isSlide;		//Скользил
	CVECTOR slideDir;	//Направление скольжения
	bool isBearing;		//Упал на опору

	CVECTOR stepPos[PTCDATA_MAXSTEPS];
	long numSteps;

	CVECTOR middle;

	//Отладочная отрисовка
	void DebugDraw(VDX8RENDER * rs, float dltTime);


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Функция защиты
#ifndef _XBOX
	void __declspec(dllexport) __cdecl SFLB_PotectionLoad();
#else
	void SFLB_PotectionLoad();
#endif
	//Вычислить высоту точки на плоскосте треугольника
	float FindHeight(long trgID, float x, float z);
	//Найти направление пути
	bool FindPathDir(long step, long curNode, const CVECTOR & cur, long toNode, const CVECTOR & to, long & node, CVECTOR & pos);
	//Найти точку на ребре
	CVECTOR FindEdgePoint(const CVECTOR & vs, const CVECTOR & ve, const CVECTOR & cur, const CVECTOR & to);
	//Проверить пересечение треугольника с отрезком
	float Trace(PtcTriangle & trg, const CVECTOR & s, const CVECTOR & d);
	//Добавить треугольник в буфер
	void AddClTriangle(long i);

//private:
public:
	//Блок данных
	void * data;

	//Геометрия
	PtcTriangle * triangle;	//Треугольники геометрии
	long numTriangles;		//Количество треугольников геометрии
	
	PtcVertex * vertex;		//Вершины геометрии
	long numVerteces;		//Количество вершин геометрии
	
	PtcNormal * normal;		//Нормали треугольников геометрии
	long numNormals;		//Количество нормальей геометрии

	PtcMaterials * materials;//Материалы

	//Данные для коллижена
	CVECTOR min, max;		//Во что вписываеться
	
	PtcMap * map;			//Карта быстрого поиска
	long l, w;				//Размер карты быстрого поиска
	float ls, ws;			//Размер одного квадрата
	
	word * indeces;			//Индексы
	long numIndeces;		//Количество индексов

	//Данные для нахождения путей
	byte * table;			//Таблица направлений
	long lineSize;			//Размер строки

	//Треугольники после коллизии
	Triangle * ctriangle;
	long numClTriangles;
	long maxClTriangles;

	//Отладочная информация
	DbgVertex * dbgTriangles;
	DbgVertex * dbgEdges;
};

#endif

