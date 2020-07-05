//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef PARTICLES_SERVICE_IMPLEMENTATION
#define PARTICLES_SERVICE_IMPLEMENTATION

#include "..\..\common_h\Particles\iparticlesservice.h"
#include "..\..\common_h\templates.h"

class ParticleManager;

class ParticleService : public IParticleService
{
	IParticleManager* pDefaultManager;

	struct CreatedManager
	{
		ParticleManager* pManager;
		string FileName;
		int Line;
	};


	bool sysDelete;
	array<CreatedManager> CreatedManagers;

public:

//Конструктор / деструктор
	ParticleService ();
	virtual ~ParticleService ();


	virtual bool Init ();

//Создать менеджер партиклов
	virtual IParticleManager* CreateManagerEx (const char* ProjectName, const char* File, int Line);

	virtual void RemoveManagerFromList (IParticleManager* pManager);

	virtual DWORD GetManagersCount ();
	virtual IParticleManager* GetManagerByIndex (DWORD Index);

	virtual IParticleManager* DefManager ();

};


#endif