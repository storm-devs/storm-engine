//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef PARTICLES_SERVICE_INTERFACE
#define PARTICLES_SERVICE_INTERFACE

#include "..\service.h"
#include "..\vmodule_api.h"

class IParticleManager;


#define CreateManager(FileName) CreateManagerEx(FileName, __FILE__, __LINE__)


//Сервис партиклов, создает менеджеры партиклов
class IParticleService : public SERVICE
{


public:
 
//Конструктор / деструктор
 IParticleService () {};
 virtual ~IParticleService () {};


 //В качестве имени файла, нужно передать имя проекта
 //в котором мы хотим работать в данный момент
 //
 //Если имя файла = NULL, не будет доступных систем для загрузки
 virtual IParticleManager* CreateManagerEx (const char* ProjectName, const char* File, int Line) = 0;


 virtual DWORD GetManagersCount () = 0;
 virtual IParticleManager* GetManagerByIndex (DWORD Index) = 0;


 virtual IParticleManager* DefManager () = 0;

   
};


#endif