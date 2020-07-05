#ifndef _K2_PSYSTEM_H
#define _K2_PSYSTEM_H

#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\cvector.h"
#include "..\..\common_h\matrix.h"
#include "..\..\common_h\vparticle_system.h"


class IParticleSystem;
class PARTICLES;

class PARTICLE_SYSTEM : public VPARTICLE_SYSTEM
{
	IParticleSystem* pSystem;
	PARTICLES* pManager;



public:
	
	PARTICLE_SYSTEM(IParticleSystem* _pSystem);
	virtual ~PARTICLE_SYSTEM();
	virtual void	Stop();
	virtual void	SetEmitter(CVECTOR p, CVECTOR a);
	virtual void	LinkToObject(ENTITY_ID id, CVECTOR _LinkPos);
	virtual void	SetDelay(long _delay);
	virtual void	SetLifeTime(dword time);
	virtual void	StopEmitter();
	virtual	void	Pause(bool _bPause);

public:
	void SetManager (PARTICLES* _pManager);
	IParticleSystem* GetSystem ();


};



#endif