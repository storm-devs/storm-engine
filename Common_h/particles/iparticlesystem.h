#ifndef PARTICLE_SYSTEM_INTERFACE
#define PARTICLE_SYSTEM_INTERFACE

#include "..\math3d.h"
#include "iparticlemanager.h"

class IGMXEntity;


class IParticleSystem
{

protected:

	virtual ~IParticleSystem() {};

public:

//Создание/удаление 
	IParticleSystem() {};
	virtual bool Release () = 0;

//Отработать всем партиклам
  virtual DWORD Execute (float DeltaTime) = 0;


//Перезапустить партикловую систему  
  virtual void Restart (DWORD RandomSeed) = 0;
  
//Запаузить испускание партиклов  
  virtual void PauseEmission (bool bPause) = 0;
  
//Узнать на паузе эмиссия или нет  
  virtual bool IsEmissionPaused () = 0;
//Установить автоудаляемая система или обычная...  
  virtual void AutoDelete (bool Enabled) = 0;
//Узнаять автоудаляемая система или нет  
	virtual bool IsAutoDeleted () = 0;
//Установить матрицу трансформации для системы 
	virtual void SetTransform (const Matrix& transform) = 0;
	virtual void GetTransform (Matrix& _matWorld) = 0;

	virtual void Teleport (const Matrix &transform) = 0;

	virtual bool IsAlive () = 0;

	virtual const char* GetName () =0;

	virtual void Stop () = 0;


};

#endif