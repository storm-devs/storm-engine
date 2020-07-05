//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Blots
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Blots_H_
#define _Blots_H_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"



#define BLOTS_NTRGS		32		//Треугольников в 1 пятне
#define BLOTS_MAX		256		//Всего пятен
#define BLOTS_TIME		120.0f	//Время жизни пятна
#define BLOTS_DIST		150.0f	//Дистанция видимости в метрах


class Blots : public ENTITY  
{

#pragma pack(push, 1)

	struct Blot
	{
		byte isUsed;		//Используеться ли это
		byte lastAlpha;		//Последние значение альфы
		word numTrgs;		//Количество треугольников
		float liveTime;		//Время жизни
		long startIndex;	//Начальный индекс в массиве
		long rnd;
		CVECTOR pos, dir;
	};

	struct Vertex
	{
		CVECTOR pos;
		long c;
		float u, v;
	};

#pragma pack(pop)

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Blots();
	virtual ~Blots();

	//Инициализация
	bool Init();
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
	//Работа
	void Realize(dword delta_time);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Отметить поподиние
	void Hit(MESSAGE & message);
	//Добавить пятно
	void AddBlot(long i, long rnd, const CVECTOR & pos, const CVECTOR & dir, float time);
	//
	void SetNodesCollision(NODE * n, bool isSet);
	//Сохранить параметры пятна
	void SaveBlot(long i);
	//Загрузить параметры пятна
	void LoadBlot(long i);

private:
	
	VDX8RENDER * rs;	
	ENTITY_ID model;
	long textureID;
	ATTRIBUTES * blotsInfo;
	ATTRIBUTES * pCharAttributeRoot;

	Blot blot[BLOTS_MAX];
	Vertex vrt[3*BLOTS_NTRGS*BLOTS_MAX];
	long useVrt;

	long updateBlot;

	static bool AddPolygon(const CVECTOR * v, long nv);
	static CVECTOR clipTriangles[3*BLOTS_NTRGS];
	static long numClipTriangles;
	static CVECTOR dir, normal;
};

#endif

