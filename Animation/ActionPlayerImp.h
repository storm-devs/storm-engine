//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	ActionPlayerImp
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _AnimationImp_H_
#define _AnimationImp_H_

#include "..\COMMON_H\Animation.h"

class AnimationImp;
class ActionInfo;

#define ACTIONPLAYEREVENTFLAGS	((ANI_MAX_EVENTS + 31) >> 5)


class ActionPlayerImp : public ActionPlayer
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	ActionPlayerImp();
	virtual ~ActionPlayerImp();

	//Установить указатель на анимацию
	void SetAnimation(AnimationImp * animation, long index);

//--------------------------------------------------------------------------------------------
//ActionPlayer
//--------------------------------------------------------------------------------------------
public:
	//Установить текущие действие
	virtual bool SetAction(const char * actionName);
	virtual const char * GetAction() const;
	//Управление проигрыванием
	virtual bool Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsPlaying() const;
	virtual bool IsPause() const;
	//Автоостановка при завершении работы таймера
	virtual bool SetAutoStop(bool isStop);
	virtual bool IsAutoStop() const;
	//Текущая позиция проигрывания
	virtual float SetPosition(float position);
	virtual float GetPosition() const;
	//Тип проигрования
	virtual void SetType(AnimationType atype);
	virtual AnimationType GetType() const;
	//Коэфициент скорости проигрывания
	virtual float SetSpeed(float kSpeed = 1.0f);
	virtual float GetSpeed() const;
	virtual float GetDefSpeed() const;
	//Получить длительность действия в кадрах
	virtual long GetFrames() const;
	//Установить коэфициент блендинга 0..1
	virtual void SetBlend(float k);
	//Получить коэфициент блендинга 0..1
	virtual float GetBlend();
	//Получить пользовательские данные для этого действия
	virtual const char * GetData(const char * dataName) const;

//--------------------------------------------------------------------------------------------
//ActionPlayerImp
//--------------------------------------------------------------------------------------------
public:
	//Сделать шаг по времени
	void Execute(long dltTime);
	//Установить позицию в самое начало	
	void ResetPosition();
	//Получить текущее время
	float GetCurrentFrame();
	//Быстрый доступ до коэфициентов блендинга
	float & Blend();
	float & TimerBlend();
	//Копировать состояние другого плеера
	void CopyState(ActionPlayerImp & from);

public:
	//Текущий коэфициент блендинга
	float kBlendCurrent;

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Передвижение позиции
	void MoveNormal(float dlt);
	void MoveReverse(float dlt);
	void MovePingpong(float dlt);
	void MoveRPingpong(float dlt);
	//Проверить события и если надо инициировать
	void CheckEvents();
	//Сброс флажков произошедших событий
	void ResetEventsMask();
	//Системная остановка
	void SysStop();

private:
	//Анимация для этого действия
	AnimationImp * ani;
	//Индекс этого плеера
	long playerIndex;
	//Информация о текущем действие
	ActionInfo * action;
	//Тип проигрывания анимации
	AnimationType anitype;
	//Проигрывание
	bool isPlay, isPause;
	//Автостоп
	bool isAutostop;
	//Позиция проигрывания
	float position;
	//Направление движения true - нормальноеб иначе обратное
	bool dir;
	//Скорость проигрывания
	float speed;
	//Коэфициент скорости проигрывания
	float kspeed;
	//Проигрывать зацикленно
	bool isLoop;
	//Маска для свершившихся событий
	dword eventsMask[ACTIONPLAYEREVENTFLAGS];
	//Коэфициент блендинга
	float kBlend;
	//Скалирующий коэфициент блендинга для таймера
	float kBlendTimer;
};

//Быстрый доступ до коэфициента блендинга
inline float & ActionPlayerImp::Blend()
{
	return kBlend;
}

inline float & ActionPlayerImp::TimerBlend()
{
	return kBlendTimer;
}

#endif

