//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	AnimationInfo
//--------------------------------------------------------------------------------------------
//	Здесь хранится информация об анимации, загружаемая 1 раз
//============================================================================================

#ifndef _AnimationInfo_H_
#define _AnimationInfo_H_

#include "ActionInfo.h"
#include "Bone.h"
#include "UserData.h"


class AnimationInfo  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	AnimationInfo(const char * animationName);
	virtual ~AnimationInfo();
	//Установить количество кадров в анимации
	void SetNumFrames(long _numFrames);
	//Создать кости
	void CreateBones(long numbones);
	//Создать действие
	ActionInfo * AddAction(const char * anctionName, long startframe, long endframe);
	//Установить скорость исполнения
	void SetFPS(float _fps);

//--------------------------------------------------------------------------------------------
//Работа с анимацией
//--------------------------------------------------------------------------------------------
public:
	//Количество костей
	long NumBones();
	//Доступиться до кости
	Bone & GetBone(long iBone);
	//Сравнить с текущим именем
	bool operator == (const char * animationName);
	//Повысить счётчик ссылок
	void AddRef();
	//Понизить счётчик ссылок
	void RelRef();
	//Добавить время простоя
	void AddDowntime(long dltTime);
	//Получить время простоя
	long GetDowntime();
	//Получить имя анимации
	const char * GetName();
	//Найти действие по имени
	ActionInfo * FindAction(const char * actionName);
	//Получить время анимации
	long GetAniNumFrames();
	//Доступ к пользовательским данным
	UserData & GetUserData();
	//Получить скорость исполнения анимации
	float GetFPS();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	char name[64];			//Название анимации
	
	long numFrames;			//Количество кадров во всей анимации
	float fps;				//Количество кадров в секунду

	Bone * bone;			//Кости с ключами анимации
	long numBones;			//Количество костей в скелете

	ActionInfo ** action;	//Действия
	long numActions;		//Количество действий

	long refCounter;		//Счётчик ссылок на эту анимацию
	long downtime;			//Время простоя

	UserData userData;		//Пользовательские данные
};

//============================================================================================
//inline
//============================================================================================

//Установить количество кадров в анимации
inline void AnimationInfo::SetNumFrames(long _numFrames)
{
	if(_numFrames < 0) _numFrames = 0;
	numFrames = _numFrames;
}

//Установить скорость исполнения
inline void AnimationInfo::SetFPS(float _fps)
{
	if(fps < 0.0f) fps = 0.0f;
	if(fps > 1000000.0f) fps = 1000000.0f;
	fps = _fps;
}

//Количество костей
inline long AnimationInfo::NumBones()
{
	return numBones;
}

//Доступиться до кости
inline Bone & AnimationInfo::GetBone(long iBone)
{
	Assert(iBone >= 0 && iBone < numBones);
	return bone[iBone];
}

//Повысить счётчик ссылок
inline void AnimationInfo::AddRef()
{
	refCounter++;
}

//Понизить счётчик ссылок
inline void AnimationInfo::RelRef()
{
	refCounter--;
	Assert(refCounter >= 0);
}

//Добавить время простоя
inline void AnimationInfo::AddDowntime(long dltTime)
{
	if(refCounter == 0)
	{
		if(dltTime <= 10) dltTime = 10;
		downtime += dltTime;
	}else downtime = 0;
	if(downtime < 0) downtime = 0;
}

//Получить время простоя
inline long AnimationInfo::GetDowntime()
{
	return downtime;
}

//Получить имя анимации
inline const char * AnimationInfo::GetName()
{
	return name;
}

//Получить время анимации
inline long AnimationInfo::GetAniNumFrames()
{
	return numFrames;
}

//Доступ к пользовательским данным
inline UserData & AnimationInfo::GetUserData()
{
	return userData;
}

//Получить скорость исполнения анимации
inline float AnimationInfo::GetFPS()
{
	return fps;
}

#endif

