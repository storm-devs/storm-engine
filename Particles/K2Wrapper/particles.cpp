#include "particles.h"
#include "psystem.h"
#include "..\..\common_h\particles.h"




PARTICLES::PARTICLES() : CreatedSystems(_FL_),
                         CaptureBuffer(_FL_)
{
	bSystemDelete = false;
	pService = NULL;
	pManager = NULL;
	CreationCapture = false;
}

PARTICLES::~PARTICLES()
{
	bSystemDelete = true;
	DeleteAll ();
}

bool PARTICLES::Init ()
{
	api->LayerAdd("realize",GetID(),0xfffff);
	api->LayerAdd("execute",GetID(),0);
	api->LayerSetExecute("execute",true);

	pService = (IParticleService*)api->CreateService("ParticleService");
	Assert (pService);
	pManager = pService->DefManager();
	Assert (pManager);
	return true;
}


dword _cdecl PARTICLES::ProcessMessage(MESSAGE & message)
{
	long code = message.Long();

	static char ps_name[MAX_PATH];
	CVECTOR pos, angles;
	long lifetime;

	switch (code)
	{
	//ѕоставить все партиклы на паузу 
	// новые не пауженные рождаютс€...
	case PS_PAUSEALL:
		{
			PauseAllActive (message.Long() != 0);
			break;
		}
		
	//”далить конкретную систему
	case PS_DELETE:
		{
			DeleteSystem (message.Long());
			break;
		}



	case PS_CREATIONCAPTURE_BEG:
		{
			CreationCapture = true;
			break;
		}
	case PS_CREATIONCAPTURE_END:
		{
			CreationCapture = false;
			break;
		}
	case PS_CLEAR_CAPTURED:
		{
			DeleteCaptured ();
			break;
		}

		

	//”далить все
	case PS_CLEARALL:
		{
			DeleteAll ();
			break;
		}
	//создать систему (string им€, float x,y,z позици€, float rx, ry, rz вращение, float life_time врем€ жизни)
	case PS_CREATE_RIC:
		{
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();

			angles.x = message.Float();
			angles.y = message.Float();
			angles.z = message.Float();
			lifetime = message.Long();

			PARTICLE_SYSTEM* pSystem = CreateSystem (ps_name, lifetime);
			if (!pSystem) return 0;

			pSystem->SetEmitter(pos,angles);
			pSystem->SetDelay(0);
			pSystem->SetLifeTime(lifetime);
			return (long)pSystem;

			break;
		}
	case PS_CREATEX_RIC:
		{
			THROW ("Unsupported particle manager command !!!");
			break;
		}

	//создать систему
	case PS_CREATE:
		{
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();

			angles.x = message.Float();
			angles.y = message.Float();
			angles.z = message.Float();
			lifetime = message.Long();

			PARTICLE_SYSTEM* pSystem = CreateSystem (ps_name, lifetime);
			if (!pSystem) return 0;


			pSystem->SetEmitter(pos,angles);
			pSystem->SetDelay(0);
			pSystem->SetLifeTime(lifetime);
			return (long)pSystem;

			break;
		}
	//создать систему
	case PS_CREATEX:
		{
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();

			Vector normal;
			normal.x = message.Float();
			normal.y = message.Float();
			normal.z = message.Float();
			double fLen = (double)normal.Normalize();

			if (fLen)
			{
				angles.y = normal.GetAY();
				double fDiv = -(normal.y / fLen);
				fDiv = Min (Max (fDiv, -1.0), 1.0);
				angles.x = (float)asin (fDiv);
			} else
				{
					angles.x = 0.0f;
					angles.y = 0.0f;
				}
			
			angles.z = 0.0f;

			lifetime = message.Long();

			PARTICLE_SYSTEM* pSystem = CreateSystem (ps_name, lifetime);
			if (!pSystem) return 0;

			pSystem->SetEmitter(pos, angles);
			pSystem->SetDelay(0);
			pSystem->SetLifeTime(lifetime);
			return (long)pSystem;

			break;
		}
	case PS_ADDTRACKPOINT:
		{
			THROW ("Unsupported particle manager command !!!");
			break;
		}
	case PS_USESURFACE:
		{
			THROW ("Unsupported particle manager command !!!");
			break;
		}
	case PS_VALIDATE_PARTICLE:
		{
			long SystemID = message.Long();
			for (dword n = 0; n < CreatedSystems.Size(); n++)
				if (CreatedSystems[n].pSystem == (PARTICLE_SYSTEM*)SystemID)
					return 1;
			return 0;
			break;
		}
	}
	return 0;


}


PARTICLE_SYSTEM* PARTICLES::CreateSystem (const char* pFileName, dword LifeTime)
{
	string pFullFileName;
	pFullFileName = "resource\\particles\\";
	pFullFileName += pFileName;
	pFullFileName.AddExtention(".xps");

	//api->Trace("K2 Particles Wrapper: Create system '%s'", pFileName);
	IParticleSystem* pSys = pManager->CreateParticleSystemEx(pFullFileName.GetBuffer(), __FILE__, __LINE__);
	if (!pSys)
	{
		//api->Trace("Can't create particles system '%s'", pFileName);
		return NULL;
	}

	pSys->AutoDelete(false);
	
	PARTICLE_SYSTEM* pNewPS = NEW PARTICLE_SYSTEM(pSys);
	pNewPS->SetManager(this);
	


	//api->Trace("PSYS Created ok");

	SystemInfo Info;
	Info.pSystem = pNewPS;
	Info.LifeTime = LifeTime;
	Info.FileName = pFullFileName;
	CreatedSystems.Add(Info);

	if (CreationCapture)
	{
		CaptureBuffer.Add((long)pNewPS);
	}


	return pNewPS;
}

void PARTICLES::DeleteSystem (long SystemID)
{
	bSystemDelete = true;
	for (dword n = 0; n < CreatedSystems.Size(); n++)
	{
		if (CreatedSystems[n].pSystem == (PARTICLE_SYSTEM*)SystemID)
		{
			//api->Trace("Delete particles system with name '%s'", CreatedSystems[n].FileName.GetBuffer());
			delete CreatedSystems[n].pSystem;
			CreatedSystems.ExtractNoShift(n);
			bSystemDelete = false;
			return;
		}
	}

	//api->Trace("Can't delete particle system with GUID %d", SystemID);

	bSystemDelete = false;
}

void PARTICLES::DeleteAll ()
{
	bSystemDelete = true;
	for (dword n = 0; n < CreatedSystems.Size(); n++)
	{
		delete CreatedSystems[n].pSystem;
	}

	CreatedSystems.DelAll();
	bSystemDelete = false;
}

void PARTICLES::DeleteResource (PARTICLE_SYSTEM* pResource)
{
	if (bSystemDelete) return;

	for (dword n = 0; n < CreatedSystems.Size(); n++)
	{
		if (CreatedSystems[n].pSystem == pResource)
		{
			CreatedSystems.ExtractNoShift(n);
			return;
		}
	}
}

void PARTICLES::Realize(dword Delta_Time)
{
	bSystemDelete = true;
	float fDeltaTime = (float)Delta_Time * 0.001f;
	pManager->Execute(fDeltaTime);


	//≈сли врем€, ставим эмитирование на паузу
	// когда все партиклы умрут система удалитьс€ сама...
	for (dword n = 0; n < CreatedSystems.Size(); n++)
	{
		CreatedSystems[n].PassedTime +=	Delta_Time;

		if (CreatedSystems[n].LifeTime == 0) continue;

		if (CreatedSystems[n].PassedTime > CreatedSystems[n].LifeTime)
		{
			//CreatedSystems[n].pSystem->Pause(true);
			CreatedSystems[n].pSystem->Stop();
		}
	}

	//”дал€ем умершие системы...
	for (n = 0; n < CreatedSystems.Size(); n++)
	{
		if (!CreatedSystems[n].pSystem->GetSystem()->IsAlive())
		{
			delete CreatedSystems[n].pSystem;
			CreatedSystems.ExtractNoShift(n);
			n--;
		}
	}


	bSystemDelete = false;
}

void PARTICLES::Execute(dword Delta_Time)
{
}

void PARTICLES::PauseAllActive (bool bPaused)
{
	
	for (dword n = 0; n < CreatedSystems.Size(); n++)
	{
		CreatedSystems[n].pSystem->GetSystem()->Restart(0);
		CreatedSystems[n].pSystem->Pause(bPaused);
	}

}


void PARTICLES::DeleteCaptured ()
{
	for (dword n = 0; n < CaptureBuffer.Size(); n++)
	{
		DeleteSystem(CaptureBuffer[n]);
	}

	CaptureBuffer.DelAll();
}
