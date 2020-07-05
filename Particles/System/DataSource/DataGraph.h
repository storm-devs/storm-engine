#ifndef _PARTICLE_DATA_GRAPH_H_
#define _PARTICLE_DATA_GRAPH_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"

#include "..\..\icommon\GraphVertex.h"


class DataGraph
{
  string Name;


//С какого времени последний раз забирали значение
	float MaxCachedTime;
	float MinCachedTime;
//Какой был индекс у этого времени
	DWORD MaxCachedIndex;
	DWORD MinCachedIndex;

	array<GraphVertex> MinGraph;
	array<GraphVertex> MaxGraph;


	void ResetCachedTime ();

	float GetMinAtTime (float Time, float LifeTime);
	float GetMaxAtTime (float Time, float LifeTime);


	bool bRelative;
	bool bNegative;
	

public:

//конструктор/деструктор
	DataGraph ();
	virtual ~DataGraph ();


	//Установить/получить могут быть отрицательные значения в графике или нет...
	void SetNegative (bool _bNegative);
	bool GetNegative ();

	//Установить/получить относительный график или нет...
	void SetRelative (bool _bRelative);
	bool GetRelative ();


//Получить значение (Текущее время, Коэфицент рандома[0..1])
	float GetValue (float Time, float LifeTime, float K_rand);
	float GetRandomValue (float Time, float LifeTime);


//Установить значения...
	void SetValues (const GraphVertex* MinValues, DWORD MinValuesSize, const GraphVertex* MaxValues, DWORD MaxValuesSize);

	//Устанавливает "значение по умолчанию"
	void SetDefaultValue (float MaxValue, float MinValue);


//Получить кол-во в графике минимума
	DWORD GetMinCount ();

//Получить кол-во в графике максимума
	DWORD GetMaxCount ();

//Получить значение по индексу из графика минимума
	const GraphVertex& GetMinVertex (DWORD Index);

//Получить значение по индексу из графика максимума
	const GraphVertex& GetMaxVertex (DWORD Index);


	void Load (MemFile* File);
	void Write (MemFile* File);
	
	void SetName (const char* szName);
	const char* GetName ();


	float GetMaxTime ();


	void ConvertRadToDeg ();
	void ConvertDegToRad ();
	void MultiplyBy (float Val);
	void Clamp (float MinValue, float MaxValue);
	void Reverse ();  //Graphs = 1.0f - Graph
	void NormalToPercent ();
	void PercentToNormal ();
	void NormalToAlpha ();
	void AlphaToNormal ();




};


#endif