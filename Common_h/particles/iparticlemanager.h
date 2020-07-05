//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef PARTICLES_MANAGER_INTERFACE
#define PARTICLES_MANAGER_INTERFACE

#include <d3d8.h>
#include "..\dx8render.h"

class ParticleService;
class IParticleSystem;


#ifndef NULL
#define NULL 0L
#endif


#define CreateParticleSystem(name) CreateParticleSystemEx(name, __FILE__, __LINE__)

//Менеджер партикловых систем
class IParticleManager
{


protected:

	virtual ~IParticleManager () {};

public:

//Создание/удаление 
	IParticleManager (ParticleService* service) {};
	virtual bool Release () = 0;

//Получить указатель на Render/FileService 
	virtual VDX8RENDER* Render () = 0;

//Открыть проект 
	virtual bool OpenProject (const char* FileName) = 0;
//Закрыть проект 
	virtual void CloseProject () = 0;
 
//Удалить из списка ресурсов (системная)
	virtual void RemoveResource (IParticleSystem* pResource) = 0;

//Исполнить партиклы 
	virtual void Execute (float DeltaTime) = 0;

//Узнать доступна система или нет 
	virtual bool IsSystemAvailable (const char* FileName) = 0;

//Получить глобальную текстуру проекта 
	virtual long GetProjectTexture () = 0;
	
	//Установить текстуру проекта
	virtual void SetProjectTexture (const char* FileName = NULL) = 0;


 


//Получить имя проекта 
 virtual const char* GetProjectFileName () = 0;
 

 //Создать пустую партикловую систему, для редактора...
 virtual IParticleSystem* CreateEmptyParticleSystemEx  (const char* FileName, int Line) = 0;


//Создать партикловую систему из файла (файл должен быть в проекте!!!!!)
 virtual IParticleSystem* CreateParticleSystemEx (const char* FileName, const char* File, int Line) = 0;


 //Проверить "валиден" ли указатель на систему партиклов, вдруг она уже удалилась
 virtual bool ValidateSystem (IParticleSystem* pSystem) = 0;


 //Проверить ли загрузились асинхронно все партиклы и готов ли менеджер
 //к созданию сисстем
 virtual bool ReadyForUse () = 0;


 virtual const char* GetProjectTextureName () = 0;

 virtual void Editor_UpdateCachedData () = 0;

 virtual const char* GetFirstGeomName (const char* FileName) = 0;
 virtual const char* GetNextGeomName () = 0;


 virtual void WriteSystemCache (const char* FileName) = 0;



 virtual void OpenDefaultProject () = 0;


 
	
};


#endif