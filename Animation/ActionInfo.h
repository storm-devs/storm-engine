//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	ActionInfo
//--------------------------------------------------------------------------------------------
//	Информация об одном действие
//============================================================================================

#ifndef _ActionInfo_H_
#define _ActionInfo_H_

#include "Bone.h"
#include "UserData.h"
#include "..\common_h\Animation.h"


enum ExtAnimationEventType
{
	eae_always,	//Всегда
	eae_normal,	//При прямом проигрывании
	eae_reverse	//При обратном проигрывании
};

class ActionPlayerImp;

class ActionInfo  
{
	friend ActionPlayerImp;

	struct Event
	{
		char name[64];					//Название события
		float time;						//Относительное время события
		ExtAnimationEventType event;	//Тип события
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	ActionInfo(const char * aname, long startframe, long endframe);
	virtual ~ActionInfo();
	//Установить коэфициент скорости воспроизведения
	void SetRate(float rate);
	//Установить тип анимации
	void SetAnimationType(AnimationType t);
	//Зацикленность воспроизведения анимации
	void SetLoop(bool isloop);
	//Добавить событие
	bool AddEvent(const char * ename, float frame, ExtAnimationEventType event);

//--------------------------------------------------------------------------------------------
//Работа с действием
//--------------------------------------------------------------------------------------------
public:
	//Сравнить с текущим именем
	bool operator == (const char * actionName);
	//Получить имя действия
	const char * GetName();
	//Получить количество событий
	long GetNumEvents();
	//Проверим на выполнимость условия генерации события, 
	//direction == true для нормального направления
	bool CheckEvent(long index, float time, bool direction);
	//Получить имя сообщения
	const char * EventName(long index);
	//Получить длительность в кадрах
	long GetFrames();
	//Доступ к пользовательским данным
	UserData & GetUserData();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	char name[64];		//Название действия
	
	long startFrame;	//Начальная позиция в общей анимации
	long endFrame;		//Конечная позиция в общей анимации

	float kRate;		//Коэфициент скорости воспроизведения
	AnimationType type;	//Тип анимации
	bool isLoop;		//Зацикленность воспроизведения анимации

	dword bonesMask[8];	//Маска используемых костей в анимации

	Event event[ANI_MAX_EVENTS];	//События
	long numEvents;					//Количество событий

	UserData userData;	//Пользовательские данные
};

//============================================================================================
//inline
//============================================================================================

//Установить коэфициент скорости воспроизведения
inline void ActionInfo::SetRate(float rate)
{
	if(rate < 0.0f) rate = 0.0f;
	if(rate > 10.0f) rate = 10.0f;
	kRate = rate;
}

//Установить тип анимации
inline void ActionInfo::SetAnimationType(AnimationType t)
{
	type = t;
}

//Зацикленность воспроизведения анимации
inline void ActionInfo::SetLoop(bool isloop)
{
	isLoop = isloop;
}

//--------------------------------------------------------------------------------------------
//Работа с действием
//--------------------------------------------------------------------------------------------

//Получить имя действия
inline const char * ActionInfo::GetName()
{
	return name;
}

//Получить количество событий
inline long ActionInfo::GetNumEvents()
{
	return numEvents;
}

//Получить длительность в кадрах
inline long ActionInfo::GetFrames()
{
	return endFrame - startFrame;
}

//Доступ к пользовательским данным
inline UserData & ActionInfo::GetUserData()
{
	return userData;
}


#endif

