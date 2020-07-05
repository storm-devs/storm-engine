//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	Supervisor
//--------------------------------------------------------------------------------------------
//	Объект контролирующий перемещение персонажей
//============================================================================================

#ifndef _Supervisor_H_
#define _Supervisor_H_

class Character;
class LocatorArray;
struct CVECTOR;

#define MAX_CHARACTERS	64

class Supervisor
{
	friend Character;
public:
	struct CharacterInfo
	{
		Character * c;		//Персонаж с которым взаимодействуем
		float d;			//Расстояние до персонажа
		float maxD;			//Наибольшее анализируемое растояние
	};

	struct FindCharacter
	{
		Character * c;		//Персонаж, которого искали
		float dx, dy, dz;	//Вектор от парсонажа до нас
		float d2;			//Квадрат растояния до персонажа в xz
		float l, r, n;		//Дистанция до плоскостей отсечения (опционально)
		float sectDist;		//Растояние до сектора
	};

	struct CharacterEx
	{
		Character * c;
		float lastTime;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Supervisor();
	virtual ~Supervisor();

	//Проверить на свободность позицию
	bool CheckPosition(float x, float y, float z, Character * c);
	//Найти по радиусу персонажей
	bool FindCharacters(FindCharacter fndCharacter[MAX_CHARACTERS], long & numFndCharacters, Character * chr, float radius, float angTest = 0.0f, float nearPlane = 0.4f, float ax = 0.0f, bool isSort = false, bool lookCenter = false);

	void Update(float dltTime);
	void PreUpdate(float dltTime);
	void PostUpdate(float dltTime);
	

	//Установить позиции для загрузки
	void SetSavePositions();
	//Удалить позиции для загрузки
	void DelSavePositions(bool isTeleport);

	//Найти оптимальный локатор для продолжения прогулки персонажа
	long FindForvardLocator(LocatorArray * la, const CVECTOR & pos, const CVECTOR & norm, bool lookChr = false);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Добавить персонажа в локацию
	void AddCharacter(Character * ch);
	//Удалить персонажа из локации
	void DelCharacter(Character * ch);

	float time, waveTime;
	long curUpdate;

public:
	CharacterEx character[MAX_CHARACTERS];
	long numCharacters;
	CharacterInfo colchr[MAX_CHARACTERS*MAX_CHARACTERS];
	long numColChr;
	bool isDelete;
	Character * player;
};

#endif

