
#ifndef _LighterTypes_H_
#define _LighterTypes_H_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"
#include "..\common_h\model.h"


struct Light
{
	enum Type
	{
		t_none,
		t_amb,
		t_sun,
		t_sky,
		t_point,
		t_group,
	};

	CVECTOR color;		//Цвет источника
	CVECTOR p;			//Позиция точечного источника или направление направленного источника
	float att0;			//Затухание D^0
	float att1;			//Затухание D^1
	float att2;			//Затухание D^2
	float range;		//Радиус действия
	char * group;		//Группа к которой принадлежит источник
	//Параметры освещения
	float cosine;		//Коэфициент косинуса
	float shadow;		//Коэфициент тени
	float bright;		//Яркость тени
	float contr;		//Контрастность тени
	float gamma;		//Гамма тени
	float curgm;		//Текущая для источника гамма
	float curct;		//Текущий для источника контраст
	//	
	Type type;			//Тип источника
	bool isOn;			//Работает ли источник
	bool isMark;		//Для построения списка
};


struct Shadow
{
	double v;				//Расчитанное значение
	double nrm;				//Коэфициент нормализации
	double sm;				//Сглаженное значение
	float cs;				//Косинус угла нормали и направления на источник
	float att;				//Коэфициент затухания
	float dst;				//Дистанция до источника
	float csatt;			//Коэфициент затухания
	float shw;				//Коэфициент затухания тени
	CVECTOR c;				//Текущий цвет источника
};


struct Vertex
{
	enum Flag
	{
		f_zero = 0,			//Инициализация
		f_inv = 1,			//Инвертирована ли нормаль
		f_set = 2,			//Была ли внесена поправка нормали
		f_bug = 4,			//Была ошибка
	};

	CVECTOR p;				//Позиция
	CVECTOR n;				//Нормаль
	CVECTOR c;				//Цвет
	CVECTOR bc;				//Цвет для сглаживания
	CVECTOR mc;				//Цветовая маска
	dword alpha;			//Альфа вертекса
	Shadow * shadow;		//Значения затенения от каждого источника
	long flags;				//Флаги
	long vbid;				//Буфер вершин
	long addr;				//Относительный адрес цвета в буфере
	long obj;				//Объект которому принадлежим
	long cindex;			//Индекс в конечном буфере цветов объекта
};

struct Triangle
{
	CVECTOR n;				//Нормаль
	float sq;				//Площадь
	long i[3];				//Индексы вершин
};

struct VertexBuffer
{
	long vbID;				//Идентификатор вертекс буфера
	long start;				//Индекс буфера в глобальном массиве
};

struct OctFndVerts
{
	Vertex * v;				//Вершина
	float r2;				//Квадрат растояния
};

#endif
