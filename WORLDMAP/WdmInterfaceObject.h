//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmInterfaceObject
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmInterfaceObject_H_
#define _WdmInterfaceObject_H_

#include "WdmRenderObject.h"

class WdmInterfaceObject : public WdmRenderObject  
{
protected:
	struct Vertex
	{
		float x, y, z, rhw;
		dword color;
		float tu, tv;
		float tu1, tv1;
	};



//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmInterfaceObject();
	virtual ~WdmInterfaceObject();

//--------------------------------------------------------------------------------------------
//Интерфейс для наследующихся классов
//--------------------------------------------------------------------------------------------
protected:
	//Заполнить 6 вершин координатами прямоугольника
	static void FillRectCoord(Vertex * vrt, float x, float y, float w, float h, float ang = 0.0f);
	//Заполнить 6 вершин текстурными координатами прямоугольника
	static void FillRectUV(Vertex * vrt, float tu, float tv, float tw, float th, float ang = 0.0f);
	//Заполнить 6 вершин текстурными координатами прямоугольника
	static void FillRectUV1(Vertex * vrt, float tu, float tv, float tw, float th, float ang = 0.0f);
	//Заполнить 6 вершин цветом
	static void FillRectColor(Vertex * vrt, dword color = 0xffffffff);
	//Нарисовать буффер прямоугольников
	static void DrawRects(Vertex * vrt, long numRects, char * techName = null);
	
	//Заполнить 18 вершин скалирующегося прямоугольника
	static void FillSRectCoord(Vertex * vrt, float x, float y, float w, float h, float ws);
	//Заполнить 18 вершин текстурными координатами скалирующегося прямоугольника
	static void FillSRectUV(Vertex * vrt, float tu, float tv, float tw, float th, float ts);
	//Заполнить 18 вершин цветом скалирующегося прямоугольника
	static void FillSRectColor(Vertex * vrt, dword color = 0xffffffff);

	//Вывести текст в заданную полоску по горизонтали и в заданной высоте
	static void Print(long font, long color, float xleft, float xright, float y, const char * format, ...);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
protected:
	static char stringBuffer[1024];
private:
	
};

#endif

