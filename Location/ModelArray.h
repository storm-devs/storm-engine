//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea dogs II
//--------------------------------------------------------------------------------------------
//	ModelArray
//--------------------------------------------------------------------------------------------
//	Хранение информации о моделях
//============================================================================================

#ifndef _ModelArray_H_
#define _ModelArray_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\model.h"

class Animation;

class PathTracer;

#define MA_MAX_NAME_LENGTH	256

class ModelArray  
{
	class UVSlider : public MODEL::RenderTuner
	{
	public:
		virtual void Set(MODEL * model, VDX8RENDER * rs);
		virtual void Restore(MODEL * model, VDX8RENDER * rs);

		float u0, v0;
		float us0, vs0;
		float u1, v1;		
		float us1, vs1;
	};

	class Relection : public MODEL::RenderTuner
	{
	public:
		virtual void Set(MODEL * model, VDX8RENDER * rs);
		virtual void Restore(MODEL * model, VDX8RENDER * rs);
		dword tfactor;
	};

	struct Rotator
	{
		float rx, ry, rz;
	};

	struct LocationModel
	{
		ENTITY_ID modelrealizer;		//Отрисовщик модели
		ENTITY_ID id;					//Модель
		dword hash;						//Хэшь значение для быстрого поиска
		union
		{
			dword flags;
			struct
			{
				dword isVisible : 1;

			};
		};
		UVSlider * slider;
		Rotator * rotator;
		Relection * reflection;
		char name[MA_MAX_NAME_LENGTH];	//Имя модели
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	ModelArray();
	virtual ~ModelArray();
	
	//Создать модель
	long CreateModel(const char * modelName, const char * technique, long level, bool isVisible = true, void* pLights=0);
	//Удалить модель
	void DeleteModel(long modelIndex);
	//Установить модели анимацию
	bool SetAnimation(long modelIndex, const char * modelAni);
	//Найти индекс модели по имени
	long FindModel(const char * modelName);

	//Проверить на правильность индекс
	bool IsValidateIndex(long index);
	//Получить название модели
	const char * GetModelName(long index);

	//Количество моделий
	long Models();
	//Получение ID модели по индексу
	ENTITY_ID & ID(long modelIndex);
	//Получение модели по индексу
	MODEL * operator [](long modelIndex);
	//Получение анимации по индексу
	Animation * GetAnimation(long modelIndex);
	//Получение ID отрисовщика по индексу
	ENTITY_ID & RealizerID(long modelIndex);

	//Установить модельке анимацию скольжения uv
	void SetUVSlide(long modelIndex, float u0, float v0, float u1, float v1);
	//Установить модельке анимацию вращения
	void SetRotation(long modelIndex, float rx, float ry, float rz);
	//Установить модельке режим генерации матрицы для отражения
	void SetReflection(long modelIndex, float scale);

	//Анимировать
	void Update(float dltTime);

	void UpdateModelsPath();
	void UpdateTexturesPath();
	void UpdateLightPath();
	void UpdateShadowPath();

	//Проверить видимость 2-х точек
	bool VisibleTest(const CVECTOR & p1, const CVECTOR & p2);
	//Протрейсит луч через локацию
	float Trace(const CVECTOR & src, const CVECTOR & dst);
	bool GetCollideTriangle(TRIANGLE & trg);
	void Clip(PLANE * p, long numPlanes, CVECTOR & cnt, float rad, bool (* fnc)(const CVECTOR * vtx, long num));

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	dword CalcHashString(const char * str);
	static void UpdatePath(char * path);
private:
	//Модели локации
	LocationModel * model;
	long numModels;
	long maxModels;
	TRIANGLE ctrg;
	bool isHavecTrg;

public:
	char modelspath[256];
	char texturespath[256];
	char lightpath[256];
	char shadowpath[256];
	char resPath[512];
};

//Проверить на правильность индекс
inline bool ModelArray::IsValidateIndex(long index)
{
	return index >= 0 && index < numModels;
}

//Получить название модели
inline const char * ModelArray::GetModelName(long index)
{
	if(index >= 0 && index < numModels) return model[index].name;
	return null;
}

#endif

