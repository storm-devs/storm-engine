#ifndef _K2_PARTICLES_H_
#define _K2_PARTICLES_H_

#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\entity.h"
#include "..\..\common_h\messages.h"
#include "..\..\common_h\templates.h"

class IParticleService;
class IParticleManager;
class IParticleSystem;
class PARTICLE_SYSTEM;


class PARTICLES : public ENTITY
{
	bool CreationCapture;
	bool bSystemDelete;

	struct SystemInfo 
	{
		string FileName;
		PARTICLE_SYSTEM* pSystem;
		dword LifeTime;
		dword PassedTime;

		SystemInfo ()
		{
			PassedTime = 0;
			pSystem = NULL;
			LifeTime = 0xFFFFFF00;
		}
	};
	array<SystemInfo> CreatedSystems;

	IParticleService* pService;
	IParticleManager* pManager;


	PARTICLE_SYSTEM* CreateSystem (const char* pFileName, dword LifeTime);
	void DeleteSystem (long SystemID);
	void DeleteAll ();

	void PauseAllActive (bool bPaused);


	array<long> CaptureBuffer;

	void DeleteCaptured ();

public:
	void DeleteResource (PARTICLE_SYSTEM* pResource);


public:
 
  PARTICLES();
  ~PARTICLES();
  
  bool Init ();


	dword _cdecl ProcessMessage(MESSAGE & message);

	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);


 
};


#endif