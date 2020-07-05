#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\entity.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\messages.h"

#include "particles_ms.h"
#include "particles_system.h"

class PARTICLES : public ENTITY
{
	VDX8RENDER * RenderService;

	char PSConfigurationFileName[MAX_PATH];
	//INIFILE * psini;
	INIFILE * * pInifiles;
	DWORD nFilesCounter;

	VAPI * api;

	PARTICLE_SYSTEM * Root;
	PARTICLE_SYSTEM * Top;

public:
	 PARTICLES();
	~PARTICLES();
	bool Init();
	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE & message);
	void ProcessMessage(dword iMsg,dword wParam,dword lParam);
	dword AttributeChanged(ATTRIBUTES *);

	void DeletePS(long id);
	void ClearAll();
	void PauseAll(bool bPause);
	PARTICLE_SYSTEM * CreateParticleSystem();
	bool VerifyPointer(PARTICLE_SYSTEM * check_ps);

};

//API_MODULE_START("Particles system")
//	CREATE_CLASS(0,PARTICLES)
//API_MODULE_END

#endif
