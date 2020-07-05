//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//  Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	Animation
//--------------------------------------------------------------------------------------------
//	Интерфейсные классы для работы с анимацией
//============================================================================================

#ifndef _Animation_H_
#define _Animation_H_

//============================================================================================

#include "service.h"
#include "Matrix.h"

//============================================================================================

class AnimationPlayer;			//Проигрыватель действий
class AnimationTimer;			//Класс управляющий блендингом анимации
class Animation;				//Класс управляющий анимациями одной модели
class AnimationEventListener;	//Класс принимающий внутреннии события анимации
class AnimationService;			//Сервис анимации


//============================================================================================
//Константы
//============================================================================================

//Возможные способы проигрывания анимации действий
enum AnimationType
{
	at_static,					//Не изменяются кадры
	at_normal,					//От старта до конца
	at_reverse,					//От конца до старта
	at_pingpong,				//От старта до конца, обратно
	at_rpingpong,				//От конца до старта, обратно
};

//События устанавливаемые из кода, помогающие управлять анимацией
enum AnimationEvent
{
	//Внутренние события AnimationPlayer
	ae_setnewaction,			//Плееру установили новое действие
	ae_play,					//Включено проигрывание действия
	ae_pauseon,					//Приостановленно проигрывание действия
	ae_pauseoff,				//Продолжено проигрывание действия
	ae_stop,					//Остановить проигрывание действия
	ae_start,					//При начале действия
	ae_end,						//При окончнии действия
	ae_change,					//При изменении направления проигрывания (pingpong)
	//AnimationTimer
	ae_timerstop,				//Таймер закончил работу или был остановлен


	//Внутренние
	ae_numevents,				//Количество событий
};


#define ANI_MAX_ACTIONS	8		//Количество ActionPlayer и AnimationTimer для одной модели
#define ANI_MAX_EVENTS	8		//Количество событий на одно действие


//============================================================================================
//Класс проигрывающий действие
//============================================================================================
//Действие или анимация действия - фрагмент цельной анимации (хотьба, бег, прыжёк, ...)
//содержащий всю анимацию персонажа.
//
//Autostop по умолчанию включен и всегда включается при установке нового действия
//============================================================================================

class ActionPlayer
{
public:
	~ActionPlayer(){};
	//Установить текущие действие
	virtual bool SetAction(const char * actionName) = null;
	virtual const char * GetAction() const = null;
	//Управление проигрыванием
	virtual bool Play() = null;
	virtual void Pause() = null;
	virtual void Stop() = null;
	virtual bool IsPlaying() const = null;
	virtual bool IsPause() const = null;
	//Автоостановка при завершении работы таймера
	virtual bool SetAutoStop(bool isStop) = null;
	virtual bool IsAutoStop() const = null;
	//Текущая позиция проигрывания
	virtual float SetPosition(float position) = null;
	virtual float GetPosition() const = null;
	//Тип проигрования
	virtual void SetType(AnimationType atype) = null;
	virtual AnimationType GetType() const = null;
	//Коэфициент скорости проигрывания
	virtual float SetSpeed(float kSpeed = 1.0f) = null;
	virtual float GetSpeed() const = null;
	virtual float GetDefSpeed() const = null;
	//Получить длительность действия в кадрах
	virtual long GetFrames() const = null;
	//Установить коэфициент блендинга 0..1
	virtual void SetBlend(float k) = null;
	//Получить коэфициент блендинга 0..1
	virtual float GetBlend() = null;
	//Получить пользовательские данные для этого действия
	virtual const char * GetData(const char * dataName) const = null;
};

//============================================================================================
//Класс управляющий блендингом анимации
//============================================================================================
//Этот класс позволяет масштабировать коэфициент
//блендинга в ActionPlayer
//Нормальное значение изменяется от 0 до 1,
//инверсное от 1 до 0
//============================================================================================

class AnimationTimer
{
public:
	virtual ~AnimationTimer(){};
	//Запустить таймер (время в секундах)
	virtual void Start(float time, float startTime = 0.0f) = null;
	//Остановить таймер
	virtual void Stop() = null;
	//Сбросить состояние таймера
	virtual void ResetTimer() = null;
	//Узнать, работает ли таймер
	virtual bool IsWork() = null;
	//Получить текущее значение 0..1
	virtual float GetCurrent() const = null;
	//Получить заданное время работы таймера
	virtual float GetTime() const = null;
	//Назначить ActionPlayer для блендинга (isInverse == false -> kBlend = [0..1])
	virtual void SetPlayer(long playerIndex, bool isInverse = false) = null;
	virtual void ResetPlayer(long playerIndex) = null;
	//Узнать, используется ли ActionPlayer
	virtual bool IsUsedPlayer(long playerIndex, bool * isInverse = null) = null;
	//Получить велечину блендинга для плеера (если не используется то 1.0f)
	virtual float GetPlayerValue(long playerIndex) = null;
};

//============================================================================================
//Класс управляющий анимациями одной модели
//============================================================================================
//Этот класс содержит объекты ActionPlayer и AnimationTimer в количестве ANI_MAX_ACTIONS
//ActionPlayer и AnimationTimer можно использовать произвольно, по своему усмотрению
//============================================================================================

class Animation
{
public:
	virtual ~Animation(){};
	//Доступиться к проигрывателю действий
	virtual ActionPlayer & Player(long index) = null;
	//Доступиться к таймеру анимации
	virtual AnimationTimer & Timer(long index) = null;
	//События
	//Установить обработчик внутреннего события
	virtual long SetEvent(AnimationEvent event, long index, AnimationEventListener * ael) = null;
	//Удалить обработчик внутреннего события
	virtual void DelEvent(long eventID) = null;
	//Установить обработчик внешнего события
	virtual void SetEventListener(AnimationEventListener * ael = null) = null;
	//Доступ к костям
	//Получить количество костей в скелете
	virtual long GetNumBones() const = null;
	//Получить матрицу анимации для кости
	virtual CMatrix & GetAnimationMatrix(long iBone) const = null;
	//Разное
	//Получить пользовательские данные для анимации
	virtual const char * GetData(const char * dataName) const = null;
	//Копировать состояние одного плеера в другой
	virtual void CopyPlayerState(long indexSrc, long indexDst, bool copyTimerState = false) = null;
	//Получить скорость исполнения анимации
	virtual float GetFPS() = null;
	//Установить режимы блендинга
	//Автоматическая нормализация коэфициентов блендинга
	virtual bool SetAutoNormalize(bool isNormalize = true) = null;
	virtual bool GetAutoNormalize() = null;
	//Разрешить использование пользовательских коэфициентов блендинга в ActionPlayer
	virtual bool UserBlend(bool isBlend = true) = null;
	virtual bool IsUserBlend() = null;
};

//============================================================================================
//Класс принимающий внутреннии события анимации
//============================================================================================

class AnimationEventListener
{
public:
	virtual ~AnimationEventListener(){};
	//Принять событие
	virtual void Event(Animation * animation, long index, long eventID, AnimationEvent event){};
	//Принять событие
	virtual void Event(Animation * animation, long playerIndex, const char * eventName){};
};

//============================================================================================
//Сервис анимации
//============================================================================================

class AnimationService : public SERVICE
{
public:
	virtual ~AnimationService(){};
	//Создать анимацию для модели, удалять через delete
	virtual Animation * CreateAnimation(const char * animationName) = null;

};


//============================================================================================

#endif
