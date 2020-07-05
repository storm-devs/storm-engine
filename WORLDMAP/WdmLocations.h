 //============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmLocations
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmLocations_H_
#define _WdmLocations_H_

#include "WdmRenderModel.h"

#define WDM_LOCATIONS_MAXLOC	8


class WdmLocations : public WdmRenderObject
{
	struct Location
	{
		//Init
		Location();
		~Location();
		bool SetModel(char * modelName);
		void SetVisible(bool isVis);
		//Work
		bool Update(float dltTime);
		
		//
		WdmRenderModel * model;		//Моделька локации
		float ay;
		bool isVisible;
		float alpha;
		bool isDebugMove;
	};

	struct Label
	{
		//Init
		Label();
		~Label();
		void SetText(const char * t);
		void SetIcon(long icon);
		void SetPivot(float px, float py);
		void SetVisible(bool isVis);
		bool IsHave();
		bool IsVisible();
		float Alpha();
		//Work
		bool Update(float dltTime, WdmLocations * loc);
		//Строка
		char * text;
		//Текущая прозрачность всей метки
		float alpha;
		//Pivot
		float x, y;
		//Размеры
		float w, h;
		//Позиция иконки
		float ix, iy;
		//Позиция строки
		float sx, sy;
		//Иконки
		struct Params
		{
			//Номер иконки
			long icon;
			//Прозрачность иконки
			float alpha;
		} t[2];
		//Видимость
		bool isVisible, isIsland;
		long font;
	};


	struct IslandInfo
	{
		//
		IslandInfo();
		~IslandInfo();
		void SetName(char * n);
		void Update(float dltTime, WdmLocations * loc);
		//
		float x, y, z;				//Позиция
		float rx, rz;				//Pivot
		float radius2;				//Размер
		float kRadius;				//Коэфициент радиуса отталкивания
		char * name;				//Имя атрибута для острова
		bool inside;				//Корабль игрока внутри или вне зоны острова
		//Локации на острове
		Location locations[WDM_LOCATIONS_MAXLOC];
		bool isNeedUpdate;
		//Текстовые метки этого острова
		Label label[WDM_LOCATIONS_MAXLOC + 1];
		//Описатели приоритетов для меток
		long labelPrt[WDM_LOCATIONS_MAXLOC + 1];
		long numLabels;
	};




//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmLocations();
	virtual ~WdmLocations();

	//Зачитать данные об островах и локациях
	void SetIslandsData(ATTRIBUTES * apnt);



	//Изменение значения атрибутов видимости
	void AttributeChanged(ATTRIBUTES * apnt);

	//Расчёты
	virtual void Update(float dltTime);
	//Отрисовка
	virtual void LRender(VDX8RENDER * rs);

	//Найти реакцию от островов для корабля
	void FindReaction(float x, float z, float & rx, float & rz);

	//Найти ближайший остров
	bool FindGoodIsland(float shipX, float shipZ, float & x, float & z, float & r, bool testInZone = false, bool retPivot = false);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void InitLabel(Label & label, ATTRIBUTES * apnt, bool isIsl, bool isSet = false);
	void UpdateLabelPriority(IslandInfo & ii);
	void Release();
	void DrawLabels(VDX8RENDER * rs);
	void DrawIcon(VDX8RENDER * rs, float x, float y, Label & label);

private:
	IslandInfo * islandsInfo;
	long numIslands;

	float curFrame;

	long curDebugIsland, curDebugLocation;

public:
	long labelFontIslands, labelFontLocations;
	float labelPivotX, labelPivotY;
	float heightViewLabel;
	long iconTexture;
	float iconWidth, iconHeight;
	long iconNum, iconFrames;
	float iconFps;
	char iconTextureName[128];
	//Прозрачность в зависимости от высоты
	static float heightAlpha;
};

#endif

