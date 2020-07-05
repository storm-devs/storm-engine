//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	NPCaracter
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _NPCharacter_H_
#define _NPCharacter_H_

#include "AICharacter.h"

class NPCharacter : public AICharacter
{
protected:
	enum NPCTask
	{
		npct_unknow = 0,
		npct_none,				//Нет задачи, персонаж контролируется извне
		npct_stay,				//Стоять на месте
		npct_gotopoint,			//Идти в точку
		npct_runtopoint,		//Бежать в точку
		npct_followcharacter,	//Идти за перcонажем
		npct_fight,				//Сражаться с другим персонажем
		npct_escape,			//Уходить от персонажа
		npct_dead,				//Смерть персонажа
		npct_max
	};

	struct Task
	{
		NPCTask task;
		CVECTOR to;
		ENTITY_ID target;
		union
		{
			dword flags;
			struct
			{
				dword isRun : 1;
				dword isFight : 1;
				dword isFollowInit : 1;
			};
		};
	};

	struct EnemyState
	{
		NPCharacter * chr;	//Указатель на врага
		float look;			//Направление врага к нам (cos)
		float dir;			//Расположение врага относительно нас (cos)
		float state;		//Коэфициент состояния врага
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	NPCharacter();
	virtual ~NPCharacter();

	virtual bool PostInit();

	dword ChlProcessMessage(long messageID, MESSAGE & message);
	void Move(float dltTime);
	void Update(float dltTime);

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
public:
	//Получить атакуещего персонажа
	Character * GetAttackedCharacter();

//--------------------------------------------------------------------------------------------
//Задачи
//--------------------------------------------------------------------------------------------

	//Установить новую задачу
	bool SetNewTask(NPCTask tsk, MESSAGE & message);
	
	bool InitFollowChartacter(ENTITY_ID & eid);
	bool InitFightChartacter(ENTITY_ID & eid);


//--------------------------------------------------------------------------------------------
//Исполнение задач
//--------------------------------------------------------------------------------------------
protected:
	//Выполнение задачи следования за персонажем
	void UpdateFollowCharacter(float dltTime);
	//Выполнение задачи убегания
	void UpdateEscapeCharacter(float dltTime);
	//Выполнение задачи следования за персонажем
	void UpdateFightCharacter(float dltTime);

	//Бой
	//Поведение в бою
	void DoFightAction(float dltTime, NPCharacter * enemy);
	//Поведение в бою - idle
	void DoFightActionAnalysisNone(float dltTime, NPCharacter * enemy);
	//Поведение в бою - attack
	void DoFightAttack(Character * enemy, long enemyCounter, bool wishDefence);
	//Поведение в бою - block, parry
	void DoFightBlock(bool needParry = false);

	//Получить энергию
	float GetEnergy();
	//Получить энергию для действия
	float GetActEnergy(const char * act);

	//События

	//Невозможно дальнейшее выполнение команды
	virtual void FailureCommand();
	//Персонаж прибыл в точку
	virtual void EndGotoCommand();
	//Персонаж удалился от точки на необходимый радиус
	virtual void EndEscapeCommand();
	//С персонажем слишком часто коллизяться
	virtual void CollisionThreshold();
	
	virtual void HitChild(bool isInBlock);

	//Сохранить задачу в стеке
	bool PushTask();
	//Востоновить задачу из стека
	bool PopTask();


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Невозможно дальнейшее выполнение команды
	void FailureCommand(NPCTask task);
	//Принятие решений
	void FightTick();
	//Получить тип задачи по имени
	static NPCTask GetTaskID(const char * taskName);
	//Получить имя задачи по типу
	static const char * GetTaskName(NPCTask t);
	//Проверить событие
	static bool PrTest(float probability, float & testTime);
	static bool PrTest(float probability);

protected:
	Task task;			//Задача, которую надо исполнять
	NPCTask lastSetTask;//Последняя установленная задача
private:
	Task taskstack[16];	//Стек задач
	long stackPointer;	//Указатель стека

	//Объект групп
	ENTITY_ID charactersGroups;

	//Система боя
	float fightLevel;	//Уровень поведения в бою 0..1
	
	//Атаки
	float attackCur;			//Скорость нарастания вероятности атаки, в попугаях
	float attackPrbFast;		//Вероятность fgt_attack_fast
	float attackPrbForce;		//Вероятность fgt_attack_force
	float attackPrbRound;		//Вероятность fgt_attack_round
	float attackPrbBreak;		//Вероятность fgt_attack_break
	float attackPrbFeint;		//Вероятность fgt_attack_feint
	
	//Защита
	float defenceCur;			//Скорость нарастания вероятности блока, в попугаях
	float blockTime;			//Время блока
	float defencePrbBlock;		//Вероятность fgt_block
	float defencePrbParry;		//Вероятность fgt_parry
	bool isRecoilEnable;		//Разрешён ли отскок

	//Стрельба
	float fireCur;				//Скорость нарастания вероятности выстрела, в попугаях
	bool isFireEnable;			//Разрешён ли выстрел

	float fightTick;			//Время до следующего тика принятия решений
	bool wantToAttack;			//Желание атаковать
	bool wantToDefence;			//Желание защитится
	bool wantToFire;			//Желание выстрелить

	//Текущее состояние противника
	bool isFgtChanged;
	FightAction enemyFgtType;

	bool	bMusketer;
	float	fMusketerDistance;
	float	fMusketerTime, fMusketerFireTime, fMusketerCheckFireTime;
	bool	bMusketerNoMove;
	bool	bTryAnyTarget;

	void SetEscapeTask(Character * c);
};

//Получить атакуещего персонажа
inline Character * NPCharacter::GetAttackedCharacter()
{
	if(task.task != npct_fight) return null;
	return (Character *)api->GetEntityPointer(&task.target);
}

//Проверить событие
inline bool NPCharacter::PrTest(float probability, float & testTime)
{
	if(testTime < 1.0f/5.0f) return false;
	testTime = rand()*(0.02f/RAND_MAX);
	if(probability <= 0.0f) return false;
	float r = rand()*(1.0f/RAND_MAX);
	return r < probability;
}

//Проверить событие
inline bool NPCharacter::PrTest(float probability)
{
	float r = rand()*(1.0f/RAND_MAX);
	return r < probability;
}

#endif

