#ifndef EMITTER_INTERFACE
#define EMITTER_INTERFACE


#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../common_h/exs.h"
#include "../../common_h/templates.h"
#include "../../common_h/math3d.h"
#include "../system/DataSource/FieldList.h"



class IParticleSystem;

class IEmitter
{

public:
 
	// Конструктор / деструктор
  IEmitter() {};
  virtual ~IEmitter() {};
  
  
	//Родить партиклы, используеться при движении привязанного эмиттера
	virtual void BornParticles (float DeltaTime) = 0;

	//Исполнить
	virtual void Execute (float DeltaTime) = 0;

	virtual void Restart () = 0;

	virtual DWORD GetParticleCount () = 0;

	virtual bool IsStoped () = 0;

	virtual void SetTransform (const Matrix &matWorld) = 0;
	virtual void Teleport (const Matrix &matWorld) = 0;


	virtual const char* GetName () = 0;

	//Если флаг в true емиттер не будет самостоятельно испускать партиклы
	//так, как он привязан
	virtual void SetAttachedFlag (bool Flag) = 0;
	virtual bool IsAttached () = 0;

	//Запомнить/восстановить тек.время эмиттера
	virtual float GetTime () = 0;
	virtual void SetTime (float Time) = 0;



	virtual DWORD GetParticleTypesCount () = 0;
	virtual FieldList* GetParticleTypeDataByIndex (DWORD Index) = 0;
	virtual ParticleType GetParticleTypeByIndex  (DWORD Index) = 0;

	virtual FieldList* GetData () = 0;


	virtual bool SetEnable (bool bVisible) = 0;
	virtual bool GetEnable () = 0;

	//-1 если не нашли, иначе индекс
	virtual int GetParticleTypeIndex (FieldList* pFields) = 0;
	virtual bool SetParticleTypeEnable (bool bVisible, DWORD Index) = 0;
	virtual bool GetParticleTypeEnable (DWORD Index) = 0;


	virtual void Editor_UpdateCachedData () = 0;


	virtual void SetName (const char* Name) = 0;


	virtual void Stop () = 0;





};

#endif