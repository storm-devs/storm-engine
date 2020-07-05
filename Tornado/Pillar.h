//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Pillar
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Pillar_H_
#define _Pillar_H_

#include "..\common_h\vmodule_api.h"

#define TRND_PI				3.141592654f

class Pillar  
{
	//Секция столба торнадо
	struct Section
	{
		float radius;	//Радиус секции
		float alpha;	//Прозрачность секции
		float x, z;		//Смещение относительно позиции торнадо
		float y;		//Позиция то высоте
	};

	//Вектор направления сегмента
	struct Segment
	{
		float x, z;
	};

public:
	struct Vertex
	{
		float x, y, z;
		dword color;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Pillar();
	virtual ~Pillar();

	void Update(float dltTime);
	void FillVertexBuffer(Vertex * buffer);
	void FillIndexBuffer(word * buffer);

	long GetNumVerteces();
	long GetNumTriangles();

	float GetHeight();
	float GetKHeight(float y);
	float GetRaduis(float y);
	float GetX(float y);
	float GetZ(float y);
	float X();
	float Z();

	float RandomPos(CVECTOR & pos);

	void SetGlobalAlpha(float a);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	Section section[24];
	Segment segment[16];
	float x, z;
	float dx, dz;
	float phaseX, phaseZ;
	float phaseX1, phaseZ1;
	float phaseX2, phaseZ2;
	float phaseX3, phaseZ3;
	float galpha;
};

inline float Pillar::X()
{
	return x;
}

inline float Pillar::Z()
{
	return z;
}

#endif

