//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	AnimationTimerImp
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _AnimationTimerImp_H_
#define _AnimationTimerImp_H_

#include "..\common_h\Animation.h"


#define ANITM_PLAYERSMAX	((ANI_MAX_ACTIONS + 31) >> 5)


class AnimationImp;


class AnimationTimerImp : public AnimationTimer  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	AnimationTimerImp();
	virtual ~AnimationTimerImp();

	//Установить анимацию
	void SetAnimation(AnimationImp * animation);

//--------------------------------------------------------------------------------------------
//AnimationTimer
//--------------------------------------------------------------------------------------------	
public:
	//Запустить таймер (время в миллисекундах)
	virtual void Start(float time, float startTime = 0.0f);
	//Остановить таймер
	virtual void Stop();
	//Сбросить состояние таймера
	virtual void ResetTimer();
	//Узнать, работает ли таймер
	virtual bool IsWork();
	//Получить текущее значение
	virtual float GetCurrent() const;
	//Получить заданное время работы таймера
	virtual float GetTime() const;
	//Назначить ActionPlayer для блендинга (isInverse == false -> kBlend = [0..1])
	virtual void SetPlayer(long playerIndex, bool isInverse = false);
	virtual void ResetPlayer(long playerIndex);
	//Узнать, используется ли ActionPlayer
	virtual bool IsUsedPlayer(long playerIndex, bool * isInverse = null);
	//Получить велечину блендинга для плеера (если не используется то 1.0f)
	virtual float GetPlayerValue(long playerIndex);

//--------------------------------------------------------------------------------------------
//AnimationTimerImp
//--------------------------------------------------------------------------------------------	
public:
	//Исполнить
	void Execute(long dltTime);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Анимация
	AnimationImp * ani;
	//Параметры таймера
	float curTime;
	float kTime;
	float value;
	//Биты, показывающие кому чего устанавливать
	dword playersMask[ANITM_PLAYERSMAX];
	dword inverseMask[ANITM_PLAYERSMAX];
};

#endif

