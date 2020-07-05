//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmIslands
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmIslands_H_
#define _WdmIslands_H_

#include "WdmRenderModel.h"
#include "..\common_h\geometry.h"
#include "PtcData.h"

class WdmIslandWaves : public WdmRenderModel
{
public:
	WdmIslandWaves();

	virtual void Update(float dltTime);
	virtual void LRender(VDX8RENDER * rs);
	void Render(VDX8RENDER * rs, float k);

private:
	float phase;
};


class WdmIslands : public WdmRenderObject
{
	struct Islands
	{
		WdmRenderModel * model;	//Моделька острова
		WdmRenderModel * area;	//Моделька области острова
		WdmRenderModel * palms;	//Моделька с пальмами
		WdmIslandWaves * waves;	//Моделька с пеной
		CMatrix toLocal;		//Преобразование в локальную систему острова
		string modelName;		//Имя модельки острова
		CVECTOR worldPosition;	//Позиция острова в мире
	};

	struct Label
	{
		string text;			//Текст метки
		CVECTOR pos;			//Позиция метки
		float l, t, r, b;		//Прямоугольник, описывающий метку в экранных координатах
		float dl, dt, dr, db;	//Смещения, чтобы получить прямоугольник, при известной точке на экране
		float textX, textY;		//Относительная позиция текста
		float iconX, iconY;		//Относительная позиция картинки
		float alpha;			//Текущее состояние прямоугольника
		float heightView;		//Высота начиная с которой метка гаснет
		long font;				//Индекс шкифта в массиве шрифтов
		long icon;				//Индекс картинки
		dword weight;			//Вес смещения
		string id;				//Идентификатор метки
		dword idHash;			//Хэшь значение идентификатора
		long next;				//Следующая в списке метка
		string locatorName;		//Имя локатора на котором размещаемся
	};

	struct Font
	{
		string name;			//Имя шрифта
		long id;				//Его идентификатор
	};

	struct Icons
	{
		float w, h;
		float u, v;
		dword num;
		dword frames;
		float fps;
		float frame;
		float f[2];
		long blend;
		long texture;
	};

	struct Quest
	{
		CVECTOR pos;
		string name;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmIslands();
	virtual ~WdmIslands();

	//Проверить на возможное столкновение
	bool CollisionTest(CMatrix & objMtx, float length, float width, bool heighTest = true);
	//Проверить наличие в данном месте треугольников
	bool ObstacleTest(float x, float z, float radius);

	//Зачитать данные об островах
	void SetIslandsData(ATTRIBUTES * apnt, bool isChange);

	//Найти направление для прибытия в заданную точку назначения из текущей
	void FindDirection(const CVECTOR & position, const CVECTOR & destination, CVECTOR & direction);
	//Найти силу отталкивания
	void FindReaction(const CVECTOR & position, CVECTOR & reaction);
	//Найти случайную точку для мерчанта
	bool GetRandomMerchantPoint(CVECTOR & p);
	//Получить координаты квестового локатора
	bool GetQuestLocator(const char * locName, CVECTOR & p);

	//Проверить попадание кораблика в зону острова
	bool CheckIslandArea(const char * islandName, float x, float z);
	//Получить ближайшую точку к зоне острова
	void GetNearPointToArea(const char * islandName, float & x, float & z);

	
	virtual void Update(float dltTime);
	virtual void LRender(VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	bool IsShipInArea(long islIndex, CVECTOR & pos);
	static bool _cdecl AddEdges(const GEOS::VERTEX * vrt, long numVrt);
	static bool _cdecl FindNearPoint(const GEOS::VERTEX * vrt, long numVrt);
	void LabelsReadIconParams(ATTRIBUTES * apnt);
	long LabelsFind(const char * id, dword hash);
	bool LabelsFindLocator(const char * name, CVECTOR & pos);
	long LabelsAddFont(const char * name);
	void LabelsRelease();
	static CVECTOR & Norm2D(CVECTOR & v);

private:
	//Модель, содержащая все локаторы
	WdmRenderModel * baseModel;
	//Патч для поиска пути
	PtcData * patch;
	//Модели островов
	array<Islands> islands;
	//Метки
	array<Label> labels;
	//Шрифты используемые метками
	array<Font> fonts;
	//Картинки
	Icons icons;
	//Зарегистрированные на отрисовку метки
	array<long> labelSort;
	//Точки места назначения мерчантов
	array<CVECTOR> merchants;
	//Точки места назначения квестовых энкоунтеров
	array<Quest> quests;

	//Входная таблица для поиска меток
	long labelsEntry[1024];

	static CMatrix curMatrix, locMatrix;
	static long numEdges;
	static CVECTOR curPos;
	static bool checkMode;
public:
	static CVECTOR centPos;
	

};

inline CVECTOR & WdmIslands::Norm2D(CVECTOR & v)
{
	v.y = 0.0f;
	double len = v.x*v.x + v.z*v.z;
	if(len >= 1e-30f)
	{
		len = 1.0f/sqrt(len);
		v.x = float(len*v.x);
		v.z = float(len*v.z);
	}else{
		v = 0.0f;
	}		
	return v;
}



#endif

