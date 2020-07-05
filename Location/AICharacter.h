//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	AICharacter
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _AICharacter_H_
#define _AICharacter_H_

#include "Character.h"

class AICharacter : public Character  
{
protected:
	enum AICommand
	{
		aicmd_unknow = 0,
		aicmd_none,				//Нет команды, персонаж контролируется извне
		aicmd_stay,				//Стоять на месте
		aicmd_gotopoint,		//Идти в точку
		aicmd_escape,			//Уходить от точки
		aicmd_max
	};

	struct PathNode
	{
		CVECTOR pos;			//Точка прибытия
		CVECTOR nrm;			//Направление пути
		float dst;				//Дистанция до плоскости
		long node;				//Нод, которого должны достигнуть
	};

	struct Command
	{
		AICommand cmd;		//Команда
		CVECTOR pnt;		//Точка для команды
		long node;			//Нод, в который идти
		CVECTOR tpnt;		//Точка в которую идём в данный момент
		long tnode;			//Нод, на котором идём
		float radius;		//Радиус для завершения задачи
		float waitTime;		//Время ожидания (постоять)
		AICharacter * exch;	//С этим чарактером нерасталкиваемся
		//Флаги
		struct
		{
			dword cnt : 8;		//Счётчик повторений
			dword isWait : 1;	//Находимся в ожидании
			dword isBusy : 1;	//Проверять на занятость точку
		};
	};


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	AICharacter();
	virtual ~AICharacter();

//--------------------------------------------------------------------------------------------
//Character
//--------------------------------------------------------------------------------------------
public:
	//Перемещаем персонажа в желаемую позицию
	void Move(float dltTime);
	//Провести дополнительные расчёты
	void Calculate(float dltTime);
	//Обновить позицию персонажа
	void Update(float dltTime);
	//Отметить перемещение персонажа
	void CharacterTeleport();

//--------------------------------------------------------------------------------------------
//AICharacter
//--------------------------------------------------------------------------------------------
public:
	//Ничего не делать
	bool CmdNone();
	//Стоять
	bool CmdStay();
	//Идти в точку
	bool CmdGotoPoint(float x, float y, float z, float rad, long node = -1, bool isCheckBusyPos = true);
	//Уходить от точки
	bool CmdEscape(float x, float y, float z, float rad);
	//Установить персонажа с которым нерасталкиваемся
	void SetExCharacter(AICharacter * chr);

	//События

	//Невозможно дальнейшее выполнение команды
	virtual void FailureCommand(){};
	//Персонаж прибыл в точку
	virtual void EndGotoCommand(){};
	//Персонаж удалился от точки на необходимый радиус
	virtual void EndEscapeCommand(){};
	//С персонажем слишком часто коллизяться
	virtual void CollisionThreshold(){};

	static const char * GetCommandName(AICommand cmd);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
//private:
protected:
	//Идти в точку
	void CmdProcessGotoPoint(float dltTime);
	void CmdUpdateGotoPoint(float dltTime);
	//Уходить от точки
	void CmdProcessEscape(float dltTime);
	void CmdUpdateEscape(float dltTime);

	//Найти индекс нода для данной координаты
	long FindNodeIndex(const CVECTOR & pos, float * hy = null);
	//Найти направление куда идти (ориентация на местности)
	bool FindDirectional();
	//Найти расталкивающие силы
	void CalcRepulsionForces();
	//Вычислить точку опразуемую пересечением и лежащую на ребре
	static bool FindIntersection(const CVECTOR & s, const CVECTOR & e, const CVECTOR & cur, const CVECTOR & to, CVECTOR & res);
	//Вычислить угол из вектора направления
	static float Angle(double vx, double vz, float defAy);


protected:
	//Текущая команда
	Command command;
	PathNode path[2];
	//Предпочтительный коэфициент скорости
	float likeKSpd;
	float collisionValue;

	//Управляющие силы
	CVECTOR force;		//Сила расталкивания
	CVECTOR goForce;	//Сила определяющая направление перемещения
	CVECTOR separation;	//Сила оталкккивания
	CVECTOR alignment;	//Сила выравнивания направлений
	CVECTOR around;		//Сила обхода персонажей

	bool	bMusketer;
	bool	bMusketerNoMove;

};

#endif

