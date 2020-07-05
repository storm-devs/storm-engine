//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	AnimationImp
//--------------------------------------------------------------------------------------------
//	Реализация интерфейса Animation
//============================================================================================

#ifndef _AnimationManagerImp_H_
#define _AnimationManagerImp_H_

#include "..\COMMON_H\Animation.h"
#include "ActionPlayerImp.h"
#include "AnimationTimerImp.h"
#include "AnimationInfo.h"


#define ANIIMP_MAXLISTENERS	8

class AnimationServiceImp;

class AnimationImp : public Animation
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	AnimationImp(long id, AnimationInfo * animationInfo);
	virtual ~AnimationImp();

	//Установить указатель на сервис анимации
	static void SetAnimationService(AnimationServiceImp * animationService);

//--------------------------------------------------------------------------------------------
//Animation
//--------------------------------------------------------------------------------------------
public:
	//Доступиться к проигрывателю действий
	virtual ActionPlayer & Player(long index);
	//Доступиться к таймеру анимации
	virtual AnimationTimer & Timer(long index);
	//События
	//Установить внутренние событие
	virtual long SetEvent(AnimationEvent event, long index, AnimationEventListener * ael);
	//Удалить внутренние событие
	virtual void DelEvent(long eventID);
	//Установить обработчик внешнего события
	virtual void SetEventListener(AnimationEventListener * ael);
	//Доступ к костям
	//Получить количество костей в скелете
	virtual long GetNumBones() const;
	//Получить матрицу анимации для кости
	virtual CMatrix & GetAnimationMatrix(long iBone) const;
	//Разное
	//Получить пользовательские данные для анимации
	virtual const char * GetData(const char * dataName) const;
	//Копировать состояние одного плеера в другой
	virtual void CopyPlayerState(long indexSrc, long indexDst, bool copyTimerState = false);
	//Получить скорость исполнения анимации
	virtual float GetFPS();
	//Установить режимы блендинга
	//Автоматическая нормализация коэфициентов блендинга
	virtual bool SetAutoNormalize(bool isNormalize = true);
	virtual bool GetAutoNormalize();
	//Разрешить использование пользовательских коэфициентов блендинга в ActionPlayer
	virtual bool UserBlend(bool isBlend = true);
	virtual bool IsUserBlend();

//--------------------------------------------------------------------------------------------
//AnimationImp
//--------------------------------------------------------------------------------------------
public:
	//Получить thisID
	long GetThisID();
	//Получить указатель на AnimationInfo
	AnimationInfo * GetAnimationInfo();
	//Найти действие по имени
	ActionInfo * GetActionInfo(const char * actionName);
	//Сделать шаг по времени
	void Execute(long dltTime);
	//Расчитать матрицы анимации
	void BuildAnimationMatrices();
	//Получить указатель на срвис анимации
	static AnimationServiceImp * GetAniService();
	//События AnimationPlayer
	//Плееру установили новое действие
	void ApeSetnewaction(long index);
	//Включено проигрывание действия
	void ApePlay(long index);
	//Приостановленно проигрывание действия
	void ApePauseOn(long index);
	//Продолжено проигрывание действия
	void ApePauseOff(long index);
	//Остановить проигрывание действия
	void ApeStop(long index);
	//Cобытие генерить на каждом кадре
	void ApeAlways(long index);
	//При начале действия
	void ApeStart(long index);
	//При окончнии действия
	void ApeEnd(long index);
	//При изменении направления проигрывания (pingpong)
	void ApeChange(long index);
	//Таймер закончил работу или был остановлен
	void AteTimerstop(long index);

	//Внешние событие
	void AteExtern(long plIndex, const char * evt);

private:
	//Разослать события
	void SendEvent(AnimationEvent event, long index);


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Собственный идентификатор
	long thisID;
	//Указатель на анимацию
	AnimationInfo * aniInfo;
	//Анимации
	ActionPlayerImp action[ANI_MAX_ACTIONS];
	//Таймеры
	AnimationTimerImp timer[ANI_MAX_ACTIONS];
	//Автонормализация
	bool isAutoNormalize;
	//Пользовательский блендинг
	bool isUserBlend;
	//Матрицы скелета
	CMatrix * matrix;
	//Подписчики на внутреннии события
	AnimationEventListener * ae_listeners[ae_numevents][ANIIMP_MAXLISTENERS];
	//Подписчики на внешние события
	AnimationEventListener * ae_listenersExt;
	//Указатель на сервис анимации
	static AnimationServiceImp * aniService;
};

//============================================================================================
//inline
//============================================================================================

//Установить указатель на сервис анимации
inline void AnimationImp::SetAnimationService(AnimationServiceImp * animationService)
{
	Assert(aniService == null);
	aniService = animationService;
}

//--------------------------------------------------------------------------------------------
//AnimationImp
//--------------------------------------------------------------------------------------------

//Получить thisID
inline long AnimationImp::GetThisID()
{
	return thisID;
}

//Получить указатель на AnimationInfo
inline AnimationInfo * AnimationImp::GetAnimationInfo()
{
	return aniInfo;
}

//Найти действие по имени
inline ActionInfo * AnimationImp::GetActionInfo(const char * actionName)
{
	return aniInfo->FindAction(actionName);
}

//Получить указатель на срвис анимации
inline AnimationServiceImp * AnimationImp::GetAniService()
{
	return aniService;
}

//События AnimationPlayer
//Плееру установили новое действие
inline void AnimationImp::ApeSetnewaction(long index)
{
	SendEvent(ae_setnewaction, index);
}

//Включено проигрывание действия
inline void AnimationImp::ApePlay(long index)
{
	SendEvent(ae_play, index);
}

//Приостановленно проигрывание действия
inline void AnimationImp::ApePauseOn(long index)
{
	SendEvent(ae_pauseon, index);
}

//Продолжено проигрывание действия
inline void AnimationImp::ApePauseOff(long index)
{
	SendEvent(ae_pauseoff, index);
}

//Остановить проигрывание действия
inline void AnimationImp::ApeStop(long index)
{
	SendEvent(ae_stop, index);
}

//При начале действия
inline void AnimationImp::ApeStart(long index)
{
	SendEvent(ae_start, index);
}

//При окончнии действия
inline void AnimationImp::ApeEnd(long index)
{
	SendEvent(ae_end, index);
}

//При изменении направления проигрывания (pingpong)
inline void AnimationImp::ApeChange(long index)
{
	SendEvent(ae_change, index);
}

//Таймер закончил работу или был остановлен
inline void AnimationImp::AteTimerstop(long index)
{
	SendEvent(ae_timerstop, index);
}

//Внешние событие
inline void AnimationImp::AteExtern(long plIndex, const char * evt)
{
	if(ae_listenersExt) ae_listenersExt->Event(this, plIndex, evt);
}

#endif
