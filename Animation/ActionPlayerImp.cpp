//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	ActionPlayerImp
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "AnimationImp.h"
#include "AnimationInfo.h"
#include "AnimationServiceImp.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

ActionPlayerImp::ActionPlayerImp()
{
	ani = null;
	playerIndex = -1;
	action = null;
	isPlay = false;
	isPause = false;
	isAutostop = true;
	anitype = at_normal;
	speed = 1.0f;
	kspeed = 1.0f;
	isLoop = false;
	kBlend = 1.0f;
	kBlendTimer = 1.0f;
	ResetEventsMask();
}

ActionPlayerImp::~ActionPlayerImp()
{

}

//Установить указатель на менеджер анимации
void ActionPlayerImp::SetAnimation(AnimationImp * animation, long index)
{
	Assert(!ani);
	ani = animation;
	playerIndex = index;
}

//--------------------------------------------------------------------------------------------
//ActionPlayer
//--------------------------------------------------------------------------------------------

//Установить текущие действие
bool ActionPlayerImp::SetAction(const char * actionName)
{
	if(actionName && action && stricmp(action->GetName(), actionName) == null) return true;
	action = null;
	isPlay = isPause = false;
	isAutostop = true;
	kBlendTimer = 1.0f;
	ResetPosition();
	if(!actionName || actionName[0] == 0)
	{
		ani->ApeSetnewaction(playerIndex);
		return false;
	}
	action = ani->GetActionInfo(actionName);
	if(action == null)
	{
		ani->ApeSetnewaction(playerIndex);
		return false;
	}
	//Зачитываем параметры
	if(action->startFrame != action->endFrame) speed = 0.001f*ani->GetFPS()/(action->endFrame - action->startFrame);
		else speed = 0.0f;
	kspeed = action->kRate;
	anitype = action->type;
	isLoop = action->isLoop;
	//Выставляем позицию
	ResetPosition();
	//Сообщаем об изменении
	ani->ApeSetnewaction(playerIndex);
	return true;
}

const char * ActionPlayerImp::GetAction() const
{
	if(action) return action->GetName();
	return null;
}

//Управление проигрыванием
bool ActionPlayerImp::Play()
{
	if(!action) return false;
	if(isPlay && !isPause) return true;
	isPlay = true;
	bool p = isPause;	
	if(!isPause)
	{
		ani->ApePlay(playerIndex);
		ani->ApeStart(playerIndex);
	}else ani->ApePauseOff(playerIndex);
	isPause = false;
	return true;
}
									
void ActionPlayerImp::Pause()
{
	isPause = true;
	ani->ApePauseOn(playerIndex);
}

void ActionPlayerImp::Stop()
{
	ResetPosition();
	isPlay = false;
	isPause = false;
	ani->ApeStop(playerIndex);
}

void ActionPlayerImp::SysStop()
{
	if(isPause) return;
	isPlay = false;
	isPause = false;
	ani->ApeStop(playerIndex);
	if(position < 0.0f) position = 0.0f;
	if(position > 1.0f) position = 1.0f;	
	ResetEventsMask();
}


bool ActionPlayerImp::IsPlaying() const
{
	return isPlay;
}

bool ActionPlayerImp::IsPause() const
{
	return isPause;
}

//Автоостановка при завершении работы таймера
bool ActionPlayerImp::SetAutoStop(bool isStop)
{
	bool old = isAutostop;
	isAutostop = isStop;
	return old;
}

bool ActionPlayerImp::IsAutoStop() const
{
	return isAutostop;
}

//Текущая позиция проигрывания
float ActionPlayerImp::SetPosition(float position)
{
	float pos = this->position;
	if(position < 0.0f) position = 0.0f;
	if(position > 1.0f) position = 1.0f;
	this->position = position;
	return pos;
}

float ActionPlayerImp::GetPosition() const
{
	return position;
}

//Тип проигрования
void ActionPlayerImp::SetType(AnimationType atype)
{
	anitype = atype;
}

AnimationType ActionPlayerImp::GetType() const
{
	return anitype;
}

//Коэфициент скорости проигрывания
float ActionPlayerImp::SetSpeed(float kSpeed)
{
	if(kSpeed < 0.0f) kSpeed = 0.0f;
	if(kSpeed > 10.0f) kSpeed = 10.0f;
	float ks = kspeed;
	kspeed = kSpeed;
	return ks;
}

float ActionPlayerImp::GetSpeed() const
{
	return kspeed;
}

float ActionPlayerImp::GetDefSpeed() const
{
	if(action) return action->kRate;
	return 0.0f;
}

//Получить длительность действия в миллисекундах
long ActionPlayerImp::GetFrames() const
{
	if(action) return action->GetFrames();
	return 0;
}

//Установить коэфициент блендинга 0..1
void ActionPlayerImp::SetBlend(float k)
{
	if(k < 0.0f) k = 0.0f;
	if(k > 1.0f) k = 1.0f;
	kBlend = k;
}

//Получить коэфициент блендинга 0..1
float ActionPlayerImp::GetBlend()
{
	return kBlend;
}

//Получить пользовательские данные для этого действия
const char * ActionPlayerImp::GetData(const char * dataName) const
{
	if(!action) return null;
	return action->GetUserData().GetData(dataName);
}

//--------------------------------------------------------------------------------------------
//AnimationImp
//--------------------------------------------------------------------------------------------

//Сделать шаг по времени
void ActionPlayerImp::Execute(long dltTime)
{
	kBlendTimer = 1.0f;
	if(!action || !isPlay || isPause || anitype == at_static) return;
	float dlt = dltTime*speed*kspeed;
	switch(anitype)
	{
	case at_static: break;
	case at_normal: MoveNormal(dlt); break;
	case at_reverse: MoveReverse(dlt); break;
	case at_pingpong: MovePingpong(dlt); break;
	case at_rpingpong: MoveRPingpong(dlt); break;
	default:
		throw "ActionPlayerImp::Execute -> anknow animation type";
	}
	if(isPlay) CheckEvents(); else ResetPosition();
}

//Установить позицию в самое начало	
void ActionPlayerImp::ResetPosition()
{
	position = 0.0f;
	dir = true;
	if(action && (anitype == at_reverse || anitype == at_rpingpong))
	{
		position = 1.0f;
		dir = false;
	}
	ResetEventsMask();
}

//Получить текущее время
float ActionPlayerImp::GetCurrentFrame()
{
	if(!action) return 0;
	return action->startFrame + position*(action->endFrame - action->startFrame);
}

//Копировать состояние другого плеера
void ActionPlayerImp::CopyState(ActionPlayerImp & from)
{
	//Информация о текущем действие
	action = from.action;
	//Тип проигрывания анимации
	anitype = from.anitype;
	//Проигрывание
	isPlay = from.isPlay;
	isPause = from.isPause;
	//Автостоп
	isAutostop = from.isAutostop;
	//Позиция проигрывания
	position = from.position;
	//Направление движения true - нормальноеб иначе обратное
	dir = from.dir;
	//Скорость проигрывания
	speed = from.speed;
	//Коэфициент скорости проигрывания
	kspeed = from.kspeed;
	//Проигрывать зацикленно
	isLoop = from.isLoop;
	//Маска для свершившихся событий
	for(long i = 0; i < ACTIONPLAYEREVENTFLAGS; i++)
						eventsMask[i] = from.eventsMask[i];
	//Коэфициент блендинга
	kBlend = from.kBlend;
	//Скалирующий коэфициент блендинга для таймера
	kBlendTimer = from.kBlendTimer;
}



//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------

//Передвижение позиции
void ActionPlayerImp::MoveNormal(float dlt)
{
	position += dlt;
	if(position >= 1.0f)
	{
		ani->ApeEnd(playerIndex);
		if(isLoop)
		{
			ResetEventsMask();
			position -= float(long(position));
			ani->ApeStart(playerIndex);
		}else SysStop();
	}
}

void ActionPlayerImp::MoveReverse(float dlt)
{
	position -= dlt;
	if(position <= 0.0f)
	{		
		ani->ApeEnd(playerIndex);
		if(isLoop)
		{
			ResetEventsMask();
			position -= long(position) - 1.0f;
			ani->ApeStart(playerIndex);
		}else SysStop();
	}
}

void ActionPlayerImp::MovePingpong(float dlt)
{
	if(dir)
	{
		//Прямое движение
		position += dlt;
		if(position >= 1.0f)
		{
			//Сменим направление
			position = 1.0f - (position - float(long(position)));
			dir = false;
			ani->ApeChange(playerIndex);
		}
	}else{
		//Обратное движение
		position -= dlt;
		if(position < 0.0f)
		{			
			ani->ApeEnd(playerIndex);
			if(isLoop)
			{
				//Сменим направление
				ResetEventsMask();
				position = -(position - long(position));
				dir = true;
				ani->ApeStart(playerIndex);
			}else SysStop();
		}
	}
}

void ActionPlayerImp::MoveRPingpong(float dlt)
{
	if(!dir)
	{
		//Обратное движение
		position -= dlt;
		if(position < 0.0f)
		{
			ResetEventsMask();
			//Сменим направление
			position = -(position - long(position));
			dir = true;
			ani->ApeChange(playerIndex);
		}
	}else{
		//Прямое движение
		position += dlt;
		if(position >= 1.0f)
		{
			ani->ApeEnd(playerIndex);
			if(isLoop)
			{
				ResetEventsMask();
				//Сменим направление
				position = 1.0f - (position - float(long(position)));
				dir = false;
				ani->ApeStart(playerIndex);
			}else SysStop();
		}
	}
}

//Проверить события и если надо инициировать
void ActionPlayerImp::CheckEvents()
{
	if(!action) return;
	long num = action->GetNumEvents();
	for(long i = 0; i < num; i++)
	{
		long mask = 1 << (i & 31);
		if(eventsMask[i >> 5] & mask) continue;
		if(action->CheckEvent(i, position, dir))
		{
			eventsMask[i >> 5] |= mask;
			//Шлём пользовательское событие
			ani->AteExtern(playerIndex, action->EventName(i));
		}
	}
}

//Сброс флажков произошедших событий
void ActionPlayerImp::ResetEventsMask()
{
	for(long i = 0; i < ACTIONPLAYEREVENTFLAGS; i++) eventsMask[i] = 0;
}

