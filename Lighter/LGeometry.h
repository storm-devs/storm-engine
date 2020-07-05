//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LGeometry
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LGeometry_H_
#define _LGeometry_H_

#include "LTypes.h"


class LGeometry  
{
	struct Object
	{
		Object()
		{
			name = null;
			nameReal = null;
			m = null;
			lBufSize = 0;
		};
		char * name;			//Путь то col файла
		char * nameReal;		//Имя модельки
		MODEL * m;				//Указатель на модельку
		ENTITY_ID model;		//Моделька
		long lBufSize;			//Размер буфера цветов для этой модельки
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	LGeometry();
	virtual ~LGeometry();

	//Установить путь до моделек
	void SetModelsPath(const char * mPath);
	//Установить путь для текущей погоды
	void SetLightPath(const char * lPath);
	//Добавить объект
	void AddObject(const char * name, ENTITY_ID & model);
	//Обработать данные
	bool Process(VDX8RENDER * rs, long numLights);
	//Нарисовать нормали
	void DrawNormals(VDX8RENDER * rs);
	//Обновить цвета в буферах
	void UpdateColors(VDX8RENDER * rs);
	//Протрейсить луч через все модели
	float Trace(CVECTOR & src, CVECTOR & dst);
	//Сохранить освещение
	bool Save();


	Object * object;
	long numObjects;
	long maxObjects;

	Vertex * vrt;
	long numVrt;
	long maxVrt;

	Triangle * trg;
	long numTrg;
	long maxTrg;

	VertexBuffer * vbuffer;
	long numVBuffers;
	long maxVBuffers;

	Shadow * shadows;
	
	CVECTOR min, max;
	float radius;

	bool useColor;

	CVECTOR * drawbuf;

	char modelsPath[512];
	char lightPath[512];
};

#endif

