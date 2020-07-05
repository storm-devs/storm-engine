#include "PARTICLES.h"
#include "..\common_h\defines.h"

INTERFACE_FUNCTION
CREATE_CLASS(PARTICLES)

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


PARTICLES::PARTICLES()
{
	api = _CORE_API;
	//psini = 0;

	Root = 0;
	Top = 0;

	pInifiles = 0;

}

PARTICLES::~PARTICLES()
{
	PARTICLE_SYSTEM * ps;
	while(Root)
	{
		ps = Root;
		ps->Deattach(&Root,&Top);
		delete ps;
	}

	if(RenderService) api->FreeService("dx8render");
	
	//if(psini) delete psini;
	if(pInifiles)
	{
		for(DWORD n = 0;n<nFilesCounter;n++) delete pInifiles[n];
		delete (char *)pInifiles;
	}
}


bool PARTICLES::Init()
{
	GUARD(PARTICLES::Init())

	api->LayerAdd("realize",GetID(),0xfffff);
	api->LayerAdd("execute",GetID(),0);
	api->LayerSetExecute("execute",true);

	//api->SystemMessages(GetID(),true);

	//api->LayerAdd("sea_reflection",GetID(),0xfffff);

	RenderService = (VDX8RENDER *)api->CreateService("dx8render");
	if(!RenderService)	_THROW("No service: dx8render");
	
	//------------------------------------------------------------------
	CVECTOR p,a;
	p.x = p.y = p.z = 0; //p.y = 3.0; p.x = 2.0f; p.z = 0.f;
	a.x = a.y = a.z = 0; a.y = 0.0f; a.x = 1.0f;


	HANDLE fh;
	WIN32_FIND_DATA fd;
	nFilesCounter = 0;
	strcpy(PSConfigurationFileName,"resource\\ini\\particles\\*.ini");
	fh = fio->_FindFirstFile(PSConfigurationFileName,&fd);
	if(fh!=INVALID_HANDLE_VALUE)
	{
		do
		{
			pInifiles = (INIFILE **)RESIZE(pInifiles,(nFilesCounter + 1) * sizeof(INIFILE *));
			strcpy(PSConfigurationFileName,"resource\\ini\\particles\\");
			strcat(PSConfigurationFileName,fd.cFileName);
			pInifiles[nFilesCounter] = fio->OpenIniFile(PSConfigurationFileName);
			nFilesCounter++;
		} while(fio->_FindNextFile(fh,&fd));
		fio->_FindClose(fh);
	}

	//strcpy(PSConfigurationFileName,"resource\\ini\\particles.ini");
	//psini = fio->OpenIniFile(PSConfigurationFileName);

//	PARTICLE_SYSTEM * ps;
	ENTITY_ID id;
	api->FindClass(&id,"modelr",0);
	api->FindClassNext(&id);

/*	p.y = -0.5f;
	p.z = 12.5f; p.x = 0; 
	a.x = a.y = a.z = 0; a.y = 1.0f; a.z = 1.0;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"whitewater");
	ps->LinkToObject(id,p);

	p.z = 11.5f; p.x = -1.0f; 
	a.x = a.y = a.z = 0; a.y = 1.0f; a.z = 1.0; a.x = 1.0f;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"whitewater");
	ps->LinkToObject(id,p);

	p.z = 11.5f; p.x = 1.f;
	a.x = a.y = a.z = 0; a.y = 1.0f; a.z = 1.0; a.x = -1.0f;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"whitewater");
	ps->LinkToObject(id,p);

*/
/*	a.x = a.y = a.z = 0;
	a.y = 1.0f;
	p.y = 1.0f;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"fire");


	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"smoke_inv");

	p.x += 2;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"smoke");
*/	

//	ps->LinkToObject(id,p);

/*
	p.z = -2.0f;
	p.x = - 1;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"fire_medium");
	ps->LinkToObject(id,p);

	p.z = 2.0f;
	p.x =  1;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"fire_medium");
	ps->LinkToObject(id,p);

	p.z = 4.0f;
	p.x = - 1;
	ps = CreateParticleSystem();
	ps->SetEmitter(p,a);
	ps->Init(psini,"fire_medium");
	ps->LinkToObject(id,p);



	for(long n=0;n<1;n++)
	{
		p.y = 2.0; p.x = -2.0f; p.z = 0.f;
		a.x = a.y = a.z = 0;
		a.x = -1.0f;
		ps = CreateParticleSystem();
		ps->SetEmitter(p,a);
		ps->Init(psini,"cancloud");
		ps->LinkToObject(id,p);
	}

//*/	

	UNGUARD
	return true;
}

void PARTICLES::Realize(dword Delta_Time)
{
	PARTICLE_SYSTEM * ps, * psStart, * psStart1;
	ps = Root;

	CMatrix Matrix, IMatrix;
	RenderService->GetTransform(D3DTS_VIEW, Matrix);

	RenderService->SetTransform(D3DTS_VIEW, IMatrix);
	RenderService->SetTransform(D3DTS_WORLD, IMatrix);

	RenderService->SetVertexShader(PARTICLE_FVF);

	psStart = ps = Root;
	while (psStart) 
	{
		// search
		while (ps)
		{
			if (ps->TextureID[0] != psStart->TextureID[0]) break;
			if (ps->TexturesNum != psStart->TexturesNum) break;
			if (stricmp(ps->TechniqueName, psStart->TechniqueName) != 0) break;
			ps = ps->GetRightNode();
		}

		if (RenderService->TechniqueExecuteStart(psStart->TechniqueName))
		{
			RenderService->TextureSet(0, psStart->TextureID[0]);

			// execute
			psStart1 = psStart;
			while (psStart1 && psStart1 != ps)
			{
				psStart1->Matrix = Matrix;
				psStart1->Execute(Delta_Time);
				psStart1 = psStart1->GetRightNode();
			}

			// realize
			while (psStart && psStart != ps)
			{
				psStart->Realize(Delta_Time);
				psStart = psStart->GetRightNode();
			}

			while (RenderService->TechniqueExecuteNext()) {};
		}

		psStart = ps;
	}
	RenderService->SetTransform(D3DTS_VIEW,Matrix);
}


void PARTICLES::Execute(dword Delta_Time)
{
	PARTICLE_SYSTEM * ps;
	PARTICLE_SYSTEM * Tps;
	

	// sort
	ps = Root;
	while(ps)
	{
		ps->ProcessOrder(&Root,&Top);
		ps = ps->GetRightNode();
	}


	// delete completed
	ps = Root;
	while(ps)
	{
		Tps = ps->GetRightNode();
		if(ps->Complete())
		{
			ps->Deattach(&Root,&Top);
			delete ps;
		}
		ps = Tps;
	}

/*	ps = Root;
	while(ps)
	{
		ps->Execute(Delta_Time);
		ps = ps->GetRightNode();
	}
*/
}

bool PARTICLES::VerifyPointer(PARTICLE_SYSTEM * check_ps)
{
	PARTICLE_SYSTEM * ps;
	ps = Root;
	while(ps)
	{
		if(ps == check_ps) return true;
		ps = ps->GetRightNode();
	}
	return false;
}

void PARTICLES::ClearAll()
{
	PARTICLE_SYSTEM * ps;
	while(Root)
	{
		ps = Root;
		ps->Deattach(&Root,&Top);
		delete ps;
	}

}

void PARTICLES::DeletePS(long id)
{
	PARTICLE_SYSTEM * ps;
	ps = Root;
	while(ps)
	{
		if((long)ps == id) 
		{
			ps->Deattach(&Root,&Top);
			delete ps;
			return;
		}
		ps = ps->GetRightNode();
	}
	return;
}

void PARTICLES::PauseAll(bool bPause)
{
	PARTICLE_SYSTEM * pPS = Root;
	while (pPS)
	{
		pPS->Pause(bPause);
		pPS = pPS->GetRightNode();
	}
}

dword _cdecl PARTICLES::ProcessMessage(MESSAGE & message)
{
	long code = message.Long();
//	long make_double;
	long lifetime;
	
	
	PARTICLE_SYSTEM * ps;
	char ps_name[MAX_PATH];
	CVECTOR pos,dir;
	ENTITY_ID link_id;
	bool blink;
	blink = false;
	float ax,ay,az;
	
	switch (code)
	{
		case PS_PAUSEALL:
			PauseAll(message.Long() != 0);
		break;
		case PS_DELETE:
			DeletePS(message.Long());
		break;
		case PS_CLEARALL:
			ClearAll();
		break;
		case PS_CREATE_RIC:
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			
			ax = message.Float();
			ay = message.Float();
			az = message.Float();
			{CMatrix m(ax,ay,az);
			dir = m.Vz();}
			lifetime = message.Long();
			//dir.x = dir.y = dir.z = 0;

			/*dir.x = dir.y = dir.z = 0;
			dir.y = 1.0f;
			pos.y = 1.0f;*/

			ps = CreateParticleSystem();
			if(!ps) break;
			ps->SetEmitter(pos,dir);
			if(!ps->Init(pInifiles,nFilesCounter,ps_name))
			{
				DeletePS((long)ps);
				break;
			}

			ps->SetDelay(0);
			ps->SetLifeTime(lifetime);
		return (long)ps->GetIClass();
		case PS_CREATEX_RIC:
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			
			dir.x = message.Float();
			dir.y = message.Float();
			dir.z = message.Float();
			lifetime = message.Long();

			ps = CreateParticleSystem();
			if(!ps) break;
			ps->SetEmitter(pos,dir);
			if(!ps->Init(pInifiles,nFilesCounter,ps_name))
			{
				DeletePS((long)ps);
				break;
			}

			ps->SetDelay(0);
			ps->SetLifeTime(lifetime);
		return (long)ps->GetIClass();
		case PS_CREATE:
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			
			ax = message.Float();
			ay = message.Float();
			az = message.Float();
			{CMatrix m(ax,ay,az);
			dir = m.Vz();}
			lifetime = message.Long();
			//dir.x = dir.y = dir.z = 0;

			/*dir.x = dir.y = dir.z = 0;
			dir.y = 1.0f;
			pos.y = 1.0f;*/

			ps = CreateParticleSystem();
			if(!ps) break;
			ps->SetEmitter(pos,dir);
			if(!ps->Init(pInifiles,nFilesCounter,ps_name))
			{
				DeletePS((long)ps);
				break;
			}

			ps->SetDelay(0);
			ps->SetLifeTime(lifetime);
		return (long)ps;
		case PS_CREATEX:
			message.String(sizeof(ps_name),ps_name);
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			
			dir.x = message.Float();
			dir.y = message.Float();
			dir.z = message.Float();
			lifetime = message.Long();

			ps = CreateParticleSystem();
			if(!ps) break;
			ps->SetEmitter(pos,dir);
			if(!ps->Init(pInifiles,nFilesCounter,ps_name))
			{
				DeletePS((long)ps);
				break;
			}

			ps->SetDelay(0);
			ps->SetLifeTime(lifetime);
		return (long)ps;
		case PS_ADDTRACKPOINT:
			ps = (PARTICLE_SYSTEM *)message.Long();
			if(!VerifyPointer(ps))
			{
				_CORE_API->Trace("Invalid particle system pointer: %d",ps);
				break;
			}
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			ps->AddTrackPoint(pos);
		break;
		case PS_USESURFACE:
			ps = (PARTICLE_SYSTEM *)message.Long();
			if(!VerifyPointer(ps))
			{
				_CORE_API->Trace("Invalid particle system pointer: %d",ps);
				break;
			}
			link_id = message.EntityID();
			ps->UseSurface(link_id);
		break;


		/*case PS_CREATEXX:
			link_id = message.EntityID();
			blink  = true;
		case PS_CREATEX:
			message.String(sizeof(ps_name),ps_name);
			pos = message.CVector();
			dir = message.CVector();
			ps = CreateParticleSystem();
			ps->SetEmitter(pos,dir);
			ps->Init(pInifiles,nFilesCounter,ps_name);
			ps->SetDelay(0);
			if(blink) ps->LinkToObject(link_id,pos);

			make_double = psini->GetLong(ps_name,PSKEY_DOUBLE,-1);
			if(make_double >= 0)
			{
				ps = CreateParticleSystem();
				ps->SetEmitter(pos,dir);
				ps->Init(pInifiles,nFilesCounter,ps_name);
				ps->SetDelay(make_double);
				if(blink) ps->LinkToObject(link_id,pos);
			}
			
		break;*/
	}
	return 0;
}


PARTICLE_SYSTEM * PARTICLES::CreateParticleSystem()
{
	GUARD(PARTICLES::CreateParticleSystem())
	PARTICLE_SYSTEM * ps;
	
	ps = NEW PARTICLE_SYSTEM;
	if(ps == null) _THROW(mem error);
	ps->Attach(&Root,&Top);

	return ps;
	UNGUARD
}

void PARTICLES::ProcessMessage(dword iMsg,dword wParam,dword lParam)
{
}

dword PARTICLES::AttributeChanged(ATTRIBUTES * pA)
{
	CVECTOR wnddir;
	char aname[MAX_PATH];
	float wndpower;
	PARTICLE_SYSTEM * ps;


	if(pA == null) return 0;
	if(!pA->GetThisName()) return 0;
	strcpy(aname,pA->GetThisName());
	if(stricmp(aname,"windpower")==0)
	{
		wndpower = pA->GetAttributeAsFloat(0,0.0f);
		ps = Root;
		while(ps)
		{
			ps->fWindPower = wndpower;
			ps = ps->GetRightNode();
		}
	} else
	if(stricmp(aname,"x")==0 || stricmp(aname,"y")==0 || stricmp(aname,"z")==0)
	{
		pA = pA->GetParent();
		if(pA == null) return 0;
		//wnddir = !wnddir;
		wnddir.x = pA->GetAttributeAsFloat("x",0.0f);
		wnddir.y = pA->GetAttributeAsFloat("y",0.0f);
		wnddir.z = pA->GetAttributeAsFloat("z",0.0f);
		ps = Root;
		while(ps)
		{
			ps->vWindDirection = wnddir;
			ps = ps->GetRightNode();
		}
	}
	return 1;
}