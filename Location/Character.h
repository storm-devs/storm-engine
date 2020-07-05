//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Character
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Character_H_
#define _Character_H_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"
#include "..\common_h\model.h"
#include "..\common_h\templates.h"
#include "PtcData.h"

class Location;
class MODEL;
class Animation;
class LocatorArray;
class Supervisor;
class VSoundService;


#define CHARACTER_ATTACK_DIST		2.4f	//Максимальная дистанция срабатывания атаки (м)
#define CHARACTER_ATTACK_ANG		60.0f	//Максимальный угл срабатывания атаки (градусы)
#define CHARACTER_BLOCK_ANG			50.0f	//Максимальный угл срабатывания блока (градусы)
#define CHARACTER_MAX_JMP_STEPS		50

class Character : public ENTITY  
{
	float hScale;
	float wScale;

	friend Supervisor;

	struct ActionCharacter
	{
		ActionCharacter();
		~ActionCharacter();
		void SetName(const char * _name);
		void ChangeName(const char * _name);

		char * name;	//Название действия
		float tblend;	//Время блендинга начала действия
	};

	//Действие перемещения
	struct ActionMove : public ActionCharacter
	{
		ActionMove();
		float speed;	//Скорость перемещения для данного действия м/сек
		float turnspd;	//Скорость поворота рад/сек		
	};

	//Действие бездействия
	struct ActionIdle : public ActionCharacter
	{
		ActionIdle();
		float p;		//Общая вероятность выбора этого действия
	};

	//Действия боя
	
	//Атака
	struct ActionFightAttack : ActionCharacter
	{
		float hitTime;	//Время срабатывания удара
	};

	//Действие смерти
	struct ActionDead : public ActionIdle
	{
		ActionDead();
		float ang;			//Направление падения
	};

	struct Blender
	{
		Blender();
		void Blend(float _old, float _new, float _time);
		bool Update(float dltTime);
		float Get();

		float old;
		float dlt;
		float kTime;
		float time;
	};

	struct Detector
	{
		Detector(LocatorArray * _la);
		void Check(float dltTime, Character * ch);
		void Exit(Character * ch);

		LocatorArray * la;
		long lastLocator;
		float timeInLocator;
		float lastEventTime;
	};

	friend Detector;

	class RTuner : public MODEL::RenderTuner
	{
		virtual void Set(MODEL * model, VDX8RENDER * rs);
		virtual void Restore(MODEL * model, VDX8RENDER * rs);
	public:
		RTuner();
		float alpha;
		float chrAlpha;
		float camAlpha;
		float selected;
		Character * character;
		bool isVisible;

		float GetAlpha();
	};


	class EventListener : public AnimationEventListener
	{
	public:
		Character * character;
		//Принять событие
		virtual void Event(Animation * animation, long index, long eventID, AnimationEvent event);
		//Принять событие
		virtual void Event(Animation * animation, long playerIndex, const char * eventName);
	};

	friend RTuner;
	friend EventListener;

	struct ObstacleZone
	{
		float x,z;
		float dw;
		bool use;
	};

protected:
	enum FightAction
	{
		fgt_none = 0,		//Нет никакого боевого действия
		fgt_attack_fast,	//Быстрый удар
		fgt_attack_force,	//Сильный удар
		fgt_attack_round,	//Круговой удар
		fgt_attack_break,	//Пробивающий удар
		fgt_attack_feint,	//Финт - спрециальный обманный удар
		fgt_attack_feintc,	//Продолжение финта в случае контратаки
		fgt_fire,			//Выстрел из пистолета
		fgt_hit_attack,		//Реакция попадания удара по персонажу вводящая его в stall
		fgt_hit_feint,		//Реакция от финта вводящая его в stall
		fgt_hit_parry,		//Реакция от парирования вводящая его в stall
		fgt_hit_round,		//Реакция отталкивание круговым ударом
		fgt_hit_fire,		//Реакция от выстрела вводящая его в stall
		fgt_block,			//Защита саблей
		fgt_blockhit,		//Защита саблей
		fgt_blockbreak,		//Пробивка блока
		fgt_parry,			//Парирование, защитное движение вводящее противника в stall
		fgt_recoil,			//Отскок назад
		fgt_strafe_l,		//Отскок влево
		fgt_strafe_r,		//Отскок право
		fgt_max,
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Character();
	virtual ~Character();

	//Инициализация
	bool Init();
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
	//Изменение атрибута
	dword AttributeChanged(ATTRIBUTES * apnt);
	void SetSignModel();
	void SetSignTechnique();
	void ReadFightActions(ATTRIBUTES * at, ActionCharacter actions[4], long & counter);

	virtual bool PostInit(){ return true; };
	virtual dword ChlProcessMessage(long messageID, MESSAGE & message){ return 0; };

	void AlreadyDelete();

//--------------------------------------------------------------------------------------------
//Character model
//--------------------------------------------------------------------------------------------
public:
	//Получить модельку персонажа
	MODEL * Model();
	
	//Переместить модельку в точку x, y, z
	bool Teleport(float x, float y, float z);
	//Переместить модельку в точку x, y, z и направить по ay
	bool Teleport(float x, float y, float z, float ay);
	//Переместить модельку в локатор
	bool Teleport(const char * group, const char * locator);

	//Установить позиции для загрузки
	void SetSavePosition();
	//Удалить позиции для загрузки
	void DelSavePosition(bool isTeleport);
	//Сохранить параметры
	virtual void SetSaveData(ATTRIBUTES * sdata){};
	//Востанавить параметры
	virtual void GetSaveData(ATTRIBUTES * sdata){};
	void StopFightAnimation(); // boal

protected:
	virtual void CharacterTeleport(){};
	virtual void HitChild(bool isInBlock){};

//--------------------------------------------------------------------------------------------
//Character animation
//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
//Character commands
//--------------------------------------------------------------------------------------------
public:
	//Пойти
	void StartMove(bool isBack = false);
	//Остановиться
	void StopMove();
	//Направить персонажа по вектору
	void Turn(float dx, float dz);
	//Направить персонажа по углу
	void Turn(float _ay);
	//Узнать направление
	float GetAY();
	//Установить режим бега
	void SetRunMode(bool _isRun = true);
	//Двигаемся ли
	bool IsMove();
	//Узнать режим бега
	bool IsRun();
	//Плывём ли
	bool IsSwim();
	//Установить режим боя
	bool SetFightMode(bool _isFight, bool isPlayAni = true);
	//Проверить, можно ли переходить в режим боя
	bool IsFightEnable();
	//Узнать режим боя
	bool IsFight();
	//В данный момент идёт выбор цели для выстрела (игроком)
	bool IsFireFindTarget();
	//В диаалоге
	bool IsDialog();
	//Получить высоту персонажа
	float GetHeight();
	//Получить позицию персонажа
	void GetPosition(CVECTOR & pos);
	//Получить позицию персонажа
	void GetGrassPosition(CVECTOR & pos, CVECTOR & gpos);
	//Установить флажёк хождения по траве
	void SetGrassSound();
	//Получить максимальный радиус персонажа
	float GetRadius();
	//Атаковать
	void Attack(Character * enemy, FightAction type);
	//Блок
	void Block();
	//Парирование
	void Parry();
	//Отскок
	void Recoil();
	//Пыжёк влево
	void StrafeLeft();
	//Пыжёк вправо
	void StrafeRight();
	//Попадание
	void Hit(FightAction type);
	//Выстрел
	void Fire();
	//Проверить, заряжен ли пистолет
	bool IsGunLoad();
	//Смерть
	void Dead();
	//Заход в локацию
	void EntryToLocation();
	//Выход из локации
	void ExitFromLocation();
	//Может уже умер
	bool IsDead();
	//прыжок
	void StartJump();

	//Есть ли оружие
	bool IsSetBlade();

	//Установить прозрачность персонажу
	void SetCameraAlpha(float alpha);
	//Заблокировать выбор idle анимации
	void LockIdle(bool isLock);
	//Выделить персонажа
	void Select();

	//Заблокировать перемещения персонажа
	void LockMove(bool isLock);	
	//Заблокировать вращение персонажа за камерой
	void LockRotate(bool isLock);
	//Отросительная скорость поворота камеры
	float CameraTurnSpeed();
	//Просмотр из глаз
	void LookFromEyes(bool isLook);


	


//--------------------------------------------------------------------------------------------
//Character
//--------------------------------------------------------------------------------------------
public:
	//Сбросить состояние
	virtual void Reset();
	//Перемещаем персонажа в желаемую позицию
	virtual void Move(float dltTime);
	//Провести дополнительные расчёты
	virtual void Calculate(float dltTime){};
	//Обновить позицию персонажа
	virtual void Update(float dltTime);
	
	//Событие окончания проигрывания анимации действия
	virtual void ActionEvent(const char * actionName, Animation * animation, long index, long eventID, AnimationEvent event);
	virtual void ActionEvent(Animation * animation, long playerIndex, const char * eventName);

	//Персонаж атакует, проанализировать кому наносить демедж
	//virtual void CharacterNowIsAttacked(){};

	long PlaySound(const char * soundName, bool isLoop = false, bool isCached = false);
	void PlayStep();
	void SetSoundPosition(long id);
	void ReleaseSound(long id);

	virtual bool IsPlayer() {return false;}

protected:
	//Найти персонажа для диалога
	Character * FindDialogCharacter();

	long startColCharacter;
	long numColCharacter;
	bool isSlide;
	CVECTOR slideDir;

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	bool zLoadModel(MESSAGE & message);
	bool zTeleport(MESSAGE & message, bool isAy);
	bool zTeleportL(MESSAGE & message);
	bool zAddDetector(MESSAGE & message);
	bool zDelDetector(MESSAGE & message);
	bool zActionPlay(MESSAGE & message);
	bool zEntry(MESSAGE & message);
	bool zSetBlade(MESSAGE & message);
	bool zSetGun(MESSAGE & message);
	bool zTurnByLoc(MESSAGE & message);	
	bool zTurnByChr(MESSAGE & message);	
	bool zTurnByPoint(MESSAGE & message);
	bool zDistByCharacter(MESSAGE & message, bool is2D);
	dword zExMessage(MESSAGE & message);
	bool zPlaySound(MESSAGE & message);
	bool TestJump(CVECTOR pos);
	bool BuildJump(CVECTOR pos,float fAng);
	bool TraceWithObstacle(const CVECTOR& src,const CVECTOR& dst);
	void UpdateActionsData();
	void UpdateActionMoveData(ActionMove & am, Animation * a);
	void UpdateActionCharacterData(ActionCharacter & ac, Animation * a);
	void UpdateActionIdleData(ActionIdle & ai, Animation * a);
	void UpdateActionDeadData(ActionDead & ai, Animation * a);
	bool SetAction(const char * actionName, float tstart = 0.0f, float movespeed = 0.0f, float trnspeed = 0.0f, bool forceStart = false);
	void UpdateAnimation();
	const char * FindIdleAnimation(float & tstart);
	const char * FindFightIdleAnimation(float & tstart);
	float GetCurrentPlayerPos();
	float GetAniPlayTime();
	bool SetPriorityAction(const char * action);
	//Проверить другого персонажа на поражение нами
	void CheckAttackHit();
	//Обновить состояние оружия
	void UpdateWeapons();
	//Получить направление на противника для подскока при ударе
	CVECTOR GetEnemyDirForImpulse();

	bool PriorityActionIsJump();

protected:
	//Найти персонажа в которого попали из пистолета kDist = 1..0
	Character * FindGunTarget(float & kDist, bool bOnlyEnemyTest = false);
	//Найти персонажей "eeffffll"
	void FindNearCharacters(MESSAGE & message);
	//Проверить видимость
	bool CharactersVisibleTest(MESSAGE & message);
public:
	//Проверка видимости двух персонажей
	bool VisibleTest(Character * chr);
	//Получить постфикс
	static const char * GetValueByPrefix(const char * str, const char * pref);

protected:
	//Локация в которой находится персонаж
	Location * location;
	//
	VSoundService * soundService;
	//Размеры персонажа
	float radius, radiusNrm, radiusFgt;
	//Текущая и предыдущая позиции
	CVECTOR curPos, oldPos, colMove, grsPos, impulse;
	float strafeMove;
	float seaY;

	long currentNode;	//Текущий нод

	float speed;		//Текущая скорость перемещения
	float turnspd;		//Скорость поворота
	Blender bspeed;		//Блендер линейной скорости
	Blender bturn;		//Блендер скорости поворота
	float ay;			//Направление персонажа	
	float nay;			//Желаемое направление
	float turnDir;		//Направление поворота +1..0..-1
	float vy;			//Скорость падения	
	float height;		//Высота персонажа

	float turnSpeed;	//Скорость поворота в радианах

	//Параметры персонажа
	float chclass;		//Класс персонажа 0..1, определяющий поведение
	float fatigue;		//Усталость 0..1
	float kSpd;			//Желаемый коэфициент скорости
	float kSpdCur;		//Текущий коэфициент скорости
	float kSpdDlt;		//Разброс по скорости у разных персонажей
	bool isMove;		//Стоим или движемся
	bool isBack;		//Идём назад
	bool isRun;			//Установлен ли режим бега
	bool isRunDisable;	//Запрещен ли режим бега
	bool isFight;		//Режим боя
	bool lockFightMode;	//Заблокировать режим боя
	bool isSwim;		//Плывём
	bool isJump;		//Спрыгиваем
	bool isJumpSnd;		//Надо проиграть звук прыжка
	bool isEnableJump;	//Разрешино ли спрыгивание
	bool isUp;			//Направление градиента перемещения
	bool isCollision;	//Был коллижен с другим персонажем
	bool isTurnLock;	//Блокирование управления поворотом
	bool lockMove;		//Запретить перемещения
	bool lockRotate;	//Запретить вращения
	bool isDialog;		//Персонаж в диалоге
	bool isFightWOWps;	//Разрешить переходить в режим боя без оружия
	bool isActiveState;	//Если true то персонаж активен
	bool recoilLook;	//Для срабатывания дразнилки после отскока
	float swimChange;	//Время отклика на изменение режима
	float isTurn;		//Поворачиваемся	
	bool isNFHit;		//Попали вне режима боя
	float movecs;		//Косинус угла наклона плоскости
	const char * deadName;//Имя действия смерти
	long jumpSound;		//Звук развивающейся одежды при прыжке
	long recoilSound;	//Звук шарканья

	float noBlendTime;	

	bool isLookFromEyes;

	ActionMove * curMove;		//Текущее действие перемещения

	ActionMove walk;			//Ходьба вперёд
	ActionMove backwalk;		//Ходьба назад
	ActionMove run;				//Бег вперёд
	ActionMove backrun;			//Бег назад
	ActionMove stsUp;			//Идти вверх по лестнице
	ActionMove stsDown;			//Идти вниз по лестнице
	ActionMove stsUpBack;		//Идти вверх по лестнице назад
	ActionMove stsDownBack;		//Идти вниз по лестнице назад
	ActionMove stsUpRun;		//Бежать вверх по лестнице
	ActionMove stsDownRun;		//Бежать вниз по лестнице
	ActionMove stsUpRunBack;	//Бежать вверх по лестнице назад
	ActionMove stsDownRunBack;	//Бежать вниз по лестнице назад
	ActionMove swim;			//Плыть
	ActionMove fall;			//Спрыгивание
	ActionMove fall_land;		//Спрыгивание, вставание на земле
	ActionMove fall_water;		//Спрыгивание, всплытие в воде
	ActionMove jump;			//Прыгание
	
	
	
	ActionIdle nfhit;			//Реакция вне режима боя


	//Idle действия
	long curIdleIndex;
	ActionIdle actionIdle[32];
	long numActionIdles;
	ActionIdle actionFightIdle[32];
	long numFightActionIdles;

	//Смерть
	ActionDead actionDead[8];
	long numActionDead;
	ActionDead actionFightDead[8];
	long numActionFightDead;

	ActionCharacter actionTurnL;//Действие поворота влево
	ActionCharacter actionTurnR;//Действие поворота вправо
	ActionCharacter userIdle;	//Действие установленное из вне

	//Бой
	//Обычные перемещения
	ActionMove fightwalk;		//Ходьба вперёд в режиме боя
	ActionMove fightbackwalk;	//Ходьба назад в режиме боя
	ActionMove fightrun;		//Бег вперёд в режиме боя
	ActionMove fightbackrun;	//Бег назад в режиме боя
	//Атаки
	ActionCharacter attackFast[4];	//Быстрые атаки
	long numAttackFast;				//Количество быстрых атак
	ActionCharacter attackForce[4];	//Силовые атаки
	long numAttackForce;			//Количество силовых атак
	ActionCharacter attackRound[4];	//Круговая атака
	long numAttackRound;			//Количество круговых атак
	ActionCharacter attackBreak[4];	//Провивающие атаки
	long numAttackBreak;			//Количество пробивающих атак
	ActionCharacter attackFeint[4];	//Финт
	ActionCharacter attackFeintC[4];//Атакующие продолжение финта
	long numAttackFeint;			//Количество финтов
	ActionCharacter parry[4];	    //Парирование  eddy
	long numParry;					//Количество парирований
	//Дистанционные атаки
	ActionCharacter shot;			//Выстрел из пистолета
	//Реакции
	ActionCharacter hit[4];			//Попадание по персонажу
	long numHits;					//Количество вариантов попаданий
	ActionCharacter hitFeint;		//Реакция на финт
	ActionCharacter hitParry;		//Реакция на парирование
	ActionCharacter hitRound;		//Реакция отталкивание круговым ударом
	ActionCharacter hitFire;		//Реакция на выстрел
	//Защита
	ActionCharacter block;			//Блок
	ActionCharacter blockaxe;		//Блок топором
	ActionCharacter blockhit;		//Попадание по персонажу в блоке
	ActionCharacter blockaxehit;	//Попадание по персонажу в блоке с топором
	ActionCharacter blockbreak;		//Пробивание блока
	ActionCharacter recoil;			//Отскок назад
	ActionCharacter strafe_l;		//Отскок влево
	ActionCharacter strafe_r;		//Отскок вправо
	//Логическое состояние
	FightAction fgtCurType;			//Тип текущего действия	
	long fgtCurIndex;				//Индекс текущего действия
	FightAction fgtSetType;			//Тип установленного действия
	long fgtSetIndex;				//Индекс установленного действия
	bool isParryState;				//Состояние парироваиня
	bool isFeintState;				//Состояние парироваиня
	bool isFired;					//Произведён ли выстрел
	bool isLockIdleForCamera;		//
	float recoilWait;				//Задержка после прыжка
	float strafeWait;				//Задержка после стрейфа
	float camRotWait;				//Текущий коэфициент задержки поворота
	float camRotMax;				//Максимальное время поворота
	float strafeAngle;				//Угол стрейфа
	float strafeVel;				//Стрейф на месте
	ENTITY_ID enemyAttack;			//На кого ориентируемся во время атаки

	//Таблица возможности смены одного действия другим
	static byte fightTbl[fgt_max][fgt_max];
	static char * fightNamesTbl[fgt_max];

	//Действие, которое необходимо проигрывать
	ActionCharacter priorityAction;
	float priorityActionMoveSpd;
	float priorityActionRotSpd;
	bool isResetAutoAction;
	bool isSetPriorityAction;

	//Детекторы
	Detector * detector[64];
	long numDetectors;


	//Моделька персонажа
	ENTITY_ID mdl;
	//Тень
	ENTITY_ID shadow;
	//Сабля
	ENTITY_ID blade;
	bool isBladeSet;
	bool isGunSet;
	//Море
	ENTITY_ID sea;

	ENTITY_ID effects;
	//Моделька привязанного знака
	ENTITY_ID sign;
	string signName;
	string signTechniqueName;


	//Круги на воде
	ENTITY_ID waterrings;
	float stepsRate;

	//Если установлен, то не удаляться из supervisor
	bool isDeleted;

	//Пол персонажа
	bool isMale;
	//Звук шага
	bool soundStep;
	//Звук шуршания в траве
	bool soundGrass;

	//Динамическое появление и удаление
	RTuner tuner;
	float liveValue;	//>0 появляеться, <0 изчезает, 0 неизменяеться

	//События анимации
	EventListener eventListener;

	float ayStack[16];
	long ayStackPointer;

	CVECTOR jumpTrack[50];
	CVECTOR osculationPoint;
	float jumpFallTime;
	float curJumpFallTime;
	long jumpPoints;

	char * characterID;	//ID персонажа

	//
	long m_nHandLightID;
	char* m_pcHandLightLocator;
	CVECTOR GetHandLightPos();

	bool CheckObstacle(float fx,float fz, float fzlen);
	long GetRandomIndexByObstacle(ObstacleZone* pZone, long num);

public:	

	bool isPlayerEnemy;
	float enemyBarsAlpha;

	//Информация для групп
	struct GrpTarget
	{
		ENTITY_ID chr;		//Идентификатор цели
		float time;			//Время потери цели из виду
		float timemax;		//Максимальное время удержания цели
	};

	GrpTarget grpTargets[32];	//Список целей
	long numTargets;			//Количество целей
	long groupID;				//Индекс группы для ускорения поиска
	char group[128];			//Имя текущей группы
};

inline void Character::AlreadyDelete()
{
	isDeleted = true;
}

//Узнать направление
inline float Character::GetAY()
{
	return ay;
}

//Узнать режим бега
inline bool Character::IsRun()
{
	return isRun && !isRunDisable;
}

//Плывём ли
inline bool Character::IsSwim()
{
	return isSwim;
}

//Узнать режим боя
inline bool Character::IsFight()
{
	return isFight;
}

//В диаалоге
inline bool Character::IsDialog()
{
	return isDialog;
}

//Получить высоту персонажа
inline float Character::GetHeight()
{
	return height;
}

//Получить позицию персонажа
inline void Character::GetPosition(CVECTOR & pos)
{
	pos = curPos;
	if(isSwim) pos.y = seaY;
}

//Получить позицию персонажа
inline void Character::GetGrassPosition(CVECTOR & pos, CVECTOR & gpos)
{
	pos = curPos;
	gpos = grsPos;
}

//Установить флажёк хождения по траве
inline void Character::SetGrassSound()
{
	soundGrass = true;
}

//Получить максимальный радиус персонажа
inline float Character::GetRadius()
{
	return radius;
}

//Установить прозрачность персонажу
inline void Character::SetCameraAlpha(float alpha)
{
	if(alpha < 0.0f) alpha = 0.0f;
	if(alpha > 1.0f) alpha = 1.0f;
	tuner.camAlpha = alpha;
}

//Двигаемся ли
inline bool Character::IsMove()
{
	return isMove;
}

//Заблокировать выбор idle анимации
inline void Character::LockIdle(bool isLock)
{
	isLockIdleForCamera = isLock;
}

//Выделить персонажа
inline void Character::Select()
{
	tuner.selected = 1.0f;
}

//Заблокировать перемещения персонажа
inline void Character::LockMove(bool isLock)
{
	lockMove = isLock;
}

//Заблокировать вращение персонажа за камерой
inline void Character::LockRotate(bool isLock)
{
	lockRotate = isLock;
}

//Отросительная скорость поворота камеры
inline float Character::CameraTurnSpeed()
{
	if(camRotWait <= 0.0f || camRotMax <= 0.0f) return 1.0f;
	float k = camRotWait/camRotMax;
	if(k > 1.0f) k = 1.0f;
	return powf(1.0f - k, 1.8f);
}

//Просмотр из глаз
inline void Character::LookFromEyes(bool isLook)
{
	isLookFromEyes = isLook;
}

//Может уже умер
inline bool Character::IsDead()
{
	return (liveValue < 0 || deadName);
}

//Есть ли оружие
inline bool Character::IsSetBlade()
{
	return isBladeSet | isFightWOWps;
}

inline bool Character::PriorityActionIsJump()
{
	return (priorityAction.name && (stricmp(priorityAction.name,jump.name)==0 || stricmp(priorityAction.name,fall.name)==0));
}

#endif
