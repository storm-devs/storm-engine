//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Grass
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Grass_H_
#define _Grass_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "grs.h"
#include "supervisor.h"

#define MSG_GRASS_LOAD_DATA			40666
#define MSG_GRASS_SET_TEXTURE		41666
#define MSG_GRASS_SET_PARAM			42666

class GEOS;

class Character;

class Grass : public ENTITY
{
#pragma pack(push, 1)

	struct Vertex
	{
		float x, y, z;
		dword data;
		dword offset;
		float wx, wz;		
		float alpha;
	};

	struct VSConstant
	{
		inline VSConstant()
		{
			x = y = z = 0.0f;
			w = 1.0f;
		};
		inline VSConstant(float _x, float _y, float _z, float _w)
		{
			x = _x; y = _y; z = _z; w = _w;
		};
		float x, y, z, w;
	};

#pragma pack(pop)

	struct AngleInfo
	{
		float dx, dz;			//Направление
		float cs;				//Косинус угла между направлением и источником
	};

	struct GRSMapElementEx
	{
		float x, y, z;			//Позиция травинки в мире
		union
		{
			struct
			{				
				byte frame;		//Кадр
				byte h;			//Высота
				byte w;			//Ширина
				byte ang;		//Угол поворота
			};
			dword data;
		};
	};

	enum RenderQuality
	{
		rq_full = 0,
		rq_middle = 1,
		rq_low = 2,
		rq_off = 3,
	};


public:
	struct CharacterPos
	{
		CVECTOR pos;			//Текущая позиция
		CVECTOR lastPos;		//Инерционная позиция
		Character * chr;		//Указатель на персонажа
		long useCounter;		//Счётчик влияний на траву
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Grass();
	virtual ~Grass();

	//Инициализация
	bool Init();
	//Работа
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//
	dword _cdecl ProcessMessage(MESSAGE &message);
	
	//Загрузить данные для травы
	bool LoadData(const char * patchName);
	//Установить текстуру
	void SetTexture(const char * texName);

	CharacterPos characters[MAX_CHARACTERS];
	long numCharacters;

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Рендер блока
	void RenderBlock(const CVECTOR & camPos, PLANE * plane, long numPlanes, long mx, long mz);
	//Проверка на видимость бокса
	bool VisibleTest(const PLANE * plane, long numPlanes, const CVECTOR & min, const CVECTOR & max);
	//Рендер блока
	void RenderBlock(GRSMiniMapElement & mme, float kLod);
	//Нарисовать содержимое буфера
	void DrawBuffer();
	//Получить цвет
	static long GetColor(CVECTOR color);

private:
	//Сервис рендера
	VDX8RENDER * rs;
	//Буфера
	long vb, ib;
	long numPoints;
	//Текстура
	long texture;

	//Миникарта
	GRSMiniMapElement * miniMap;
	//Размеры миникарты
	long miniX, miniZ;
	//Начальная позиция карты
	float startX, startZ;
	//Блоки с высотами
	GRSMapElementEx * block;
	long numElements;
	//Текущие параметры для поворотов травы
	AngleInfo angInfo[16];
	//Текущие фазы качания
	float phase[7];
	//Параметры текущего источника освещения
	CVECTOR lDir;		//Направление источника
	CVECTOR lColor;		//Цвет источника
	CVECTOR aColor;		//Цвет расеяного освещения

	long blockChrs[32];	//Индексы персонажей обрабатываемых блоком
	long numBlockChr;	//Количество персонажей обрабатываемых блоком

	float lodSelect;	//Коэфициент дальности выбора лода (kLod = kLod^lodSelect)
	float winForce;		//Коэфициент скорости ветра 0..1
	CVECTOR winDir;		//Нормализованное напрвавление ветра
	
	Vertex * vbuffer;	//Залоченый буфер	

	RenderQuality quality;	//Качество отрисовки

	float cosPh1, sinPh2, sinPh5, sinPh6, winPow, winF10, kAmpWF, kDirWF, kLitWF;
	float windAng;
	long initForce;

	VSConstant consts[42];

	char textureName[64];

	float m_fDataScale;
	float m_fMaxWidth;
	float m_fMaxHeight;
	float m_fMinVisibleDist;
	float m_fMaxVisibleDist;
	float m_fMinGrassLod;
	
	// boal параметры травы
	long isGrassLightsOn;
};

#endif
