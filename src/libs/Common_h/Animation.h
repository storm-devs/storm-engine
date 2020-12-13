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

#include "Matrix.h"
#include "service.h"

//============================================================================================

class AnimationPlayer;        //Проигрыватель действий
class AnimationTimer;         //Класс управляющий блендингом анимации
class Animation;              //Класс управляющий анимациями одной модели
class AnimationEventListener; //Класс принимающий внутреннии события анимации
class AnimationService;       //Сервис анимации

//============================================================================================
//Константы
//============================================================================================

//Возможные способы проигрывания анимации действий
enum AnimationType
{
    at_static,
    //Не изменяются кадры
    at_normal,
    //От старта до конца
    at_reverse,
    //От конца до старта
    at_pingpong,
    //От старта до конца, обратно
    at_rpingpong,
    //От конца до старта, обратно
};

//События устанавливаемые из кода, помогающие управлять анимацией
enum AnimationEvent
{
    //Внутренние события AnimationPlayer
    ae_setnewaction,
    //Плееру установили новое действие
    ae_play,
    //Включено проигрывание действия
    ae_pauseon,
    //Приостановленно проигрывание действия
    ae_pauseoff,
    //Продолжено проигрывание действия
    ae_stop,
    //Остановить проигрывание действия
    ae_start,
    //При начале действия
    ae_end,
    //При окончнии действия
    ae_change,
    //При изменении направления проигрывания (pingpong)
    // AnimationTimer
    ae_timerstop,
    //Таймер закончил работу или был остановлен

    //Внутренние
    ae_numevents,
    //Количество событий
};

#define ANI_MAX_ACTIONS 8 //Количество ActionPlayer и AnimationTimer для одной модели
#define ANI_MAX_EVENTS 8  //Количество событий на одно действие

//============================================================================================
//Класс проигрывающий действие
//============================================================================================
//Действие или анимация действия - фрагмент цельной анимации (хотьба, бег, прыжёк, ...)
//содержащий всю анимацию персонажа.
//
// Autostop по умолчанию включен и всегда включается при установке нового действия
//============================================================================================

class ActionPlayer
{
  public:
    ~ActionPlayer()
    {
    }

    //Установить текущие действие
    virtual bool SetAction(const char *actionName) = 0;
    virtual const char *GetAction() const = 0;
    //Управление проигрыванием
    virtual bool Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual bool IsPlaying() const = 0;
    virtual bool IsPause() const = 0;
    //Автоостановка при завершении работы таймера
    virtual bool SetAutoStop(bool isStop) = 0;
    virtual bool IsAutoStop() const = 0;
    //Текущая позиция проигрывания
    virtual float SetPosition(float position) = 0;
    virtual float GetPosition() const = 0;
    //Тип проигрования
    virtual void SetType(AnimationType atype) = 0;
    virtual AnimationType GetType() const = 0;
    //Коэфициент скорости проигрывания
    virtual float SetSpeed(float kSpeed = 1.0f) = 0;
    virtual float GetSpeed() const = 0;
    virtual float GetDefSpeed() const = 0;
    //Получить длительность действия в кадрах
    virtual long GetFrames() const = 0;
    //Установить коэфициент блендинга 0..1
    virtual void SetBlend(float k) = 0;
    //Получить коэфициент блендинга 0..1
    virtual float GetBlend() = 0;
    //Получить пользовательские данные для этого действия
    virtual const char *GetData(const char *dataName) const = 0;
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
    virtual ~AnimationTimer()
    {
    }

    //Запустить таймер (время в секундах)
    virtual void Start(float time, float startTime = 0.0f) = 0;
    //Остановить таймер
    virtual void Stop() = 0;
    //Сбросить состояние таймера
    virtual void ResetTimer() = 0;
    //Узнать, работает ли таймер
    virtual bool IsWork() = 0;
    //Получить текущее значение 0..1
    virtual float GetCurrent() const = 0;
    //Получить заданное время работы таймера
    virtual float GetTime() const = 0;
    //Назначить ActionPlayer для блендинга (isInverse == false -> kBlend = [0..1])
    virtual void SetPlayer(long playerIndex, bool isInverse = false) = 0;
    virtual void ResetPlayer(long playerIndex) = 0;
    //Узнать, используется ли ActionPlayer
    virtual bool IsUsedPlayer(long playerIndex, bool *isInverse = nullptr) = 0;
    //Получить велечину блендинга для плеера (если не используется то 1.0f)
    virtual float GetPlayerValue(long playerIndex) = 0;
};

//============================================================================================
//Класс управляющий анимациями одной модели
//============================================================================================
//Этот класс содержит объекты ActionPlayer и AnimationTimer в количестве ANI_MAX_ACTIONS
// ActionPlayer и AnimationTimer можно использовать произвольно, по своему усмотрению
//============================================================================================

class Animation
{
  public:
    virtual ~Animation(){};
    //Доступиться к проигрывателю действий
    virtual ActionPlayer &Player(long index) = 0;
    //Доступиться к таймеру анимации
    virtual AnimationTimer &Timer(long index) = 0;
    //События
    //Установить обработчик внутреннего события
    virtual long SetEvent(AnimationEvent event, long index, AnimationEventListener *ael) = 0;
    //Удалить обработчик внутреннего события
    virtual void DelEvent(long eventID) = 0;
    //Установить обработчик внешнего события
    virtual void SetEventListener(AnimationEventListener *ael = nullptr) = 0;
    //Доступ к костям
    //Получить количество костей в скелете
    virtual long GetNumBones() const = 0;
    //Получить матрицу анимации для кости
    virtual CMatrix &GetAnimationMatrix(long iBone) const = 0;
    //Разное
    //Получить пользовательские данные для анимации
    virtual const char *GetData(const char *dataName) const = 0;
    //Копировать состояние одного плеера в другой
    virtual void CopyPlayerState(long indexSrc, long indexDst, bool copyTimerState = false) = 0;
    //Получить скорость исполнения анимации
    virtual float GetFPS() = 0;
    //Установить режимы блендинга
    //Автоматическая нормализация коэфициентов блендинга
    virtual bool SetAutoNormalize(bool isNormalize = true) = 0;
    virtual bool GetAutoNormalize() = 0;
    //Разрешить использование пользовательских коэфициентов блендинга в ActionPlayer
    virtual bool UserBlend(bool isBlend = true) = 0;
    virtual bool IsUserBlend() = 0;
};

//============================================================================================
//Класс принимающий внутреннии события анимации
//============================================================================================

class AnimationEventListener
{
  public:
    virtual ~AnimationEventListener(){};
    //Принять событие
    virtual void Event(Animation *animation, long index, long eventID, AnimationEvent event)
    {
    }

    //Принять событие
    virtual void Event(Animation *animation, long playerIndex, const char *eventName)
    {
    }
};

//============================================================================================
//Сервис анимации
//============================================================================================

class AnimationService : public SERVICE
{
  public:
    virtual ~AnimationService()
    {
    }

    //Создать анимацию для модели, удалять через delete
    virtual Animation *CreateAnimation(const char *animationName) = 0;
};

//============================================================================================

#endif
