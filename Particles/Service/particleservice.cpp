#include "particleservice.h"
#include "..\manager\particlemanager.h"
#include "..\k2Wrapper\particles.h"


INTERFACE_FUNCTION
CREATE_SERVICE(ParticleService)
CREATE_CLASS(PARTICLES)

ParticleService::ParticleService () : CreatedManagers(_FL_)
{
	pDefaultManager = NULL;
	sysDelete = false;
}

ParticleService::~ParticleService ()
{
	if (pDefaultManager) pDefaultManager->Release();
	sysDelete = true;

	if (CreatedManagers.Size () > 0) 
	{
		api->Trace("Unreleased particles managers found !\n");
	}
	for (int n = 0; n < CreatedManagers; n++)
	{
		api->Trace("Manager created in %s, Line %d\n", CreatedManagers[n].FileName, CreatedManagers[n].Line);
		CreatedManagers[n].pManager->Release();
	}

}

IParticleManager* ParticleService::CreateManagerEx (const char* ProjectName, const char* File, int Line)
{
	ParticleManager* pManager = NEW ParticleManager (this);

	CreatedManager manager;
	manager.pManager = pManager;
	manager.Line = Line;
	manager.FileName = File;
	CreatedManagers.Add(manager);

	if (ProjectName != NULL)
	{
		pManager->OpenProject(ProjectName);
	}
	return pManager;
}

void ParticleService::RemoveManagerFromList (IParticleManager* pManager)
{
	if (sysDelete) return;
	for (int n = 0; n < CreatedManagers; n++)
	{
		if (CreatedManagers[n].pManager == pManager)
		{
			CreatedManagers.ExtractNoShift(n);
			return;
		}
	}
}

DWORD ParticleService::GetManagersCount ()
{
	return CreatedManagers.Size();
}

IParticleManager* ParticleService::GetManagerByIndex (DWORD Index)
{
	return CreatedManagers[Index].pManager;
}

bool ParticleService::Init ()
{
	pDefaultManager = CreateManagerEx (NULL, __FILE__, __LINE__);
	Assert (pDefaultManager);
	pDefaultManager->OpenDefaultProject();
	return true;
}

IParticleManager* ParticleService::DefManager ()
{
	return pDefaultManager;
}