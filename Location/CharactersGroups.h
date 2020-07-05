//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	CharactersGroups
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _CharactersGroups_H_
#define _CharactersGroups_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"

#include "LocatorArray.h"
#include "Location.h"
#include "Character.h"

class Location;
class Character;

class CharactersGroups : public ENTITY
{
public:
	struct String
	{
		String();
		String(const char * str);
		~String();
		char * name;
		long len;
		long max;
		long hash;

		void operator = (const char * str);
		inline operator const char * (){ return name; };
		bool Cmp(const char * str, long l, long h);
		static long GetHash(const char * str);
		static long GetLen(const char * str);
	};

	enum RelState
	{
		rs_beginvalue,
		rs_friend,
		rs_neitral,
		rs_enemy,
		rs_endvalue,
	};

	struct Relation
	{
		float alarm;		//Текущее состояние тревоги
		float alarmdown;	//Скорость убывания тревоги
		float alarmmin;		//Порог деактивации
		float alarmmax;		//Порог активации
		bool isActive;		//Активна ли тревога
		RelState curState;	//Текущие отношение между группами
		RelState actState;	//Отношение между группами, которое будет установлено в случае активации тревоги
		RelState relState;	//Отношение между группами, которое будет установлено в случае деактивации тревоги
	};

	struct Group
	{
		long index;						//Индекс группы
		String name;					//Имя группы
		float look;						//Радиус видимости противников
		float hear;						//Радиус на котором персонаж абсолютно определяет противника
		float say;						//Радиус на котором персонаж может сообщить соседним об опасности
		long priority;					//Преоритет защиты
		Relation * relations;			//Список отношений - размер соответствует индексу группы в списке
		ENTITY_ID c[MAX_CHARACTERS];	//Список персонажей находящихся в группе
		long numChr;					//Количество персонажей в группе
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	CharactersGroups();
	virtual ~CharactersGroups();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
	//Изменение атрибута
	dword AttributeChanged(ATTRIBUTES * apnt);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Проверка обнаружения персонажем остальных
	void CharacterVisibleCheck(Character * chr);
	//Проверить найденных персонажей на врагов
	void FindEnemyFromFindList(Character * chr, Group * grp, long num, bool visCheck);
	//Добавить или обновить врага
	bool AddEnemyTarget(Character * chr, Character * enemy, float maxtime = -1.0);
	//Удалить все неактивные или неправильные цели
	void RemoveAllInvalidTargets();
	//Удалить неактивные или неправильные цели
	bool RemoveInvalidTargets(Character * chr, Character * check = null);

private:
	//Проверить на действительность цель
	bool MsgIsValidateTarget(MESSAGE & message);
	//Найти оптимальную цель
	bool MsgGetOptimalTarget(MESSAGE & message);
	//Враг ли данный персонаж
	bool MsgIsEnemy(MESSAGE & message);
	//Реакция групп на атаку
	void MsgAttack(MESSAGE & message);
	//Добавить цель
	void MsgAddTarget(MESSAGE & message);
	//Обновить цели у данного персонажа
	void MsgUpdChrTrg(MESSAGE & message);

	//Зарегистрировать группу
	void MsgRegistryGroup(MESSAGE & message);
	//Удалить группу
	void MsgReleaseGroup(MESSAGE & message);
	//Зарегистрировать группу
	long RegistryGroup(const char * groupName);
	//Удалить группу
	void ReleaseGroup(const char * groupName);

	//Установить для группы радиус видимости
	bool MsgSetGroupLook(MESSAGE & message);
	//Установить для группы радиус слышимости
	bool MsgSetGroupHear(MESSAGE & message);
	//Установить для группы радиус сообщения
	bool MsgSetGroupSay(MESSAGE & message);
	//Установить для группы приоритет
	bool MsgSetGroupPriority(MESSAGE & message);
	//Установить скорость уровень тревоги
	bool MsgSetAlarm(MESSAGE & message);
	//Установить скорость убывания тревоги
	bool MsgSetAlarmDown(MESSAGE & message);
	//Добавить в группу персонажа
	bool MoveCharacterToGroup(MESSAGE & message);
	//Установить отношения между группами
	void MsgSetRelation(MESSAGE & message);
	//Установить реакцию на тревогу для пары групп
	void MsgSetAlarmReaction(MESSAGE & message);



	//Выгрузка персонажа
	void UnloadCharacter(MESSAGE & message);

	//Исключить персонажа из всех групп
	void RemoveCharacterFromAllGroups(ENTITY_ID * chr);
public:
	//Получить группу из сообщения
	Group * GetGroup(MESSAGE & message, bool isRegistry = true);
	//Найти группу по имени
	Group * FindGroup(const char * name);
	//Найти группу по имени
	long FindGroupIndex(const char * name);
	//Найти отношение групп
	Relation & FindRelation(MESSAGE & message, bool * selfgroup = null);
	//Найти отношение групп
	Relation & FindRelation(const char * name1, const char * name2, bool * selfgroup = null);
	//Найти отношение групп
	Relation & FindRelation(long g1, long g2, bool * selfgroup = null);
	//Получить индекс группы персонажа
	long GetCharacterGroup(Character * c);

	//Удалить все цели
	void ClearAllTargets();
	//Сохранить даные в объект
	void SaveData();
	//Прочитать даные отношений из объекта
	void LoadDataRelations();
	//Установить отношения для активных групп
	void RestoreStates();

	//Вывести информацию об отношениях
	void DumpRelations();
	//Получить состояние в виде строки
	const char * GetTextState(RelState state);

private:
	Group ** groups;		//Группы
	long numGroups;			//Количество групп
	long maxGroups;			//Количество групп
	Location * location;	//Текущая локация
	long curExecuteChr;		//Индекс текущего исполняемого персонажа
	float waveTime;			//Время с прошлого запуска волны
	
	//Массив для поиска персонажей
	Supervisor::FindCharacter fnd[MAX_CHARACTERS];
};

#endif


