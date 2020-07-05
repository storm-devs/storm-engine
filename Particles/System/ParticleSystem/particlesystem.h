#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "..\..\..\common_h\templates.h"
#include "..\..\..\common_h\vmodule_api.h"
#include "..\..\..\common_h\particles\iparticlesystem.h"
#include "..\..\\icommon\types.h"
#include "..\..\manager\particlemanager.h"
#include "..\datasource\datasource.h"



class IEmitter;

class ParticleSystem : public IParticleSystem
{
	struct EmitterDesc
	{
		EmitterType Type;
		IEmitter* pEmitter;
	};

	array<EmitterDesc> Emitters;

	//Автоудаляемая система или обычная
	bool AutoDeleted;  
	
	// Не рождать новые партиклы !!!
	bool EmissionPause;		

	bool DeleteIfNeed ();

	string SystemName;


	

protected:

	virtual ~ParticleSystem();


	IEmitter* CreatePointEmitter (DataSource::EmitterDesc* pEmitter);

	void DeleteAllEmitters ();

	ParticleManager* pMaster;

	Matrix matWorld;

public:

//Создание/удаление 
	ParticleSystem(ParticleManager* serv);
	virtual bool Release ();

//Отработать всем партиклам
  virtual DWORD Execute (float DeltaTime);


//Перезапустить партикловую систему  
  virtual void Restart (DWORD RandomSeed);
  
//Запаузить испускание партиклов  
  virtual void PauseEmission (bool bPause);

	virtual void Stop ();
//Узнать на паузе эмиссия или нет  
  virtual bool IsEmissionPaused ();


//Установить автоудаляемая система или обычная...  
  virtual void AutoDelete (bool Enabled);
//Узнаять автоудаляемая система или нет  
	virtual bool IsAutoDeleted ();
//Установить матрицу трансформации для системы 
	virtual void SetTransform (const Matrix& transform);
	virtual void GetTransform (Matrix& _matWorld);


	void CreateFromDataSource (DataSource* pDataSource);


	ParticleManager* GetMaster ();

	IEmitter* FindEmitter (const char* name);
	IEmitter* FindEmitterByData (FieldList* Data);
	

	void Teleport (const Matrix &transform);

	bool IsAlive ();

	const char* GetName ();
	void SetName (const char* Name);


public:
	DWORD GetEmittersCount ();
	IEmitter* GetEmitterByIndex (DWORD Index);
	EmitterType GetEmitterTypeByIndex  (DWORD Index);

	void Editor_UpdateCachedData ();



};

#endif