#include "psystem.h"
#include "particles.h"
#include "..\..\common_h\particles.h"

PARTICLE_SYSTEM::PARTICLE_SYSTEM(IParticleSystem* _pSystem)
{
	pManager = NULL;
	pSystem = _pSystem;
}

PARTICLE_SYSTEM::~PARTICLE_SYSTEM()
{
	pManager->DeleteResource(this);

	if (pSystem) pSystem->Release();
	pSystem = NULL;
}

void	PARTICLE_SYSTEM::Stop()
{
}

void	PARTICLE_SYSTEM::SetEmitter(CVECTOR p, CVECTOR a)
{
	//api->Trace("K2 Particles Wrapper: SetEmitter (%3.2f, %3.2f, %3.2f, %3.2f, %3.2f, %3.2f)", p.x, p.y, p.z, a.x, a.y, a.z);
	Vector pos, angles;
	pos.x = p.x; pos.y = p.y; pos.z = p.z;
	angles.x = a.x; angles.y = a.y; angles.z = a.z;
	Matrix mTransform (angles, pos);
	pSystem->SetTransform(mTransform);
}

void	PARTICLE_SYSTEM::LinkToObject(ENTITY_ID id, CVECTOR _LinkPos)
{
}

void	PARTICLE_SYSTEM::SetDelay(long _delay)
{
}

void	PARTICLE_SYSTEM::SetLifeTime(dword time)
{
}

void	PARTICLE_SYSTEM::StopEmitter()
{
}

void	PARTICLE_SYSTEM::Pause(bool _bPause)
{
	pSystem->PauseEmission(_bPause);
}

void PARTICLE_SYSTEM::SetManager (PARTICLES* _pManager)
{
	pManager = _pManager;
}

IParticleSystem* PARTICLE_SYSTEM::GetSystem ()
{
	return pSystem;
}