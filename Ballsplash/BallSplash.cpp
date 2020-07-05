#include <stdio.h>
#include <stdlib.h>
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
//#include "..\SoundService\VSoundService.h"
#include "BALLSPLASH.h"

INTERFACE_FUNCTION
CREATE_CLASS(BALLSPLASH)	

//--------------------------------------------------------------------
BALLSPLASH::BALLSPLASH()
	:sea(0)
	,renderer(0)
{
}

//--------------------------------------------------------------------
BALLSPLASH::~BALLSPLASH()
{
	GUARD(BALLSPLASH::~BALLSPLASH)

	UNGUARD
}

//--------------------------------------------------------------------
bool BALLSPLASH::Init()
{
	GUARD(BALLSPLASH::Init)

	ENTITY_ID seaID;
	_CORE_API->FindClass(&seaID, "sea", 0);
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);

	renderer = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	//_CORE_API->CreateEntity(&arrowModel,"MODELR");
	//_CORE_API->Send_Message(arrowModel,"ls",MSG_MODEL_LOAD_GEO, "fish01");
	InitializeSplashes();

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
dword _cdecl BALLSPLASH::ProcessMessage(MESSAGE & message)
{
	GUARD(BALLSPLASH::ProcessMessage)

	long code = message.Long();
	dword outValue = 0;

	switch (code)
	{
	case MSG_BALLSPLASH_ADD:
		{
			CVECTOR pos,dir;
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			/*
			dir.x = message.Float();
			dir.y = message.Float();
			dir.z = message.Float();
			*/
			
			dir.x = 0.0f;
			dir.y = -1.0f;
			dir.z = 0.0f;
			
			TryToAddSplash(pos, dir);
		}
		break;
	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void BALLSPLASH::Realize(dword _dTime)
{
	GUARD(BALLSPLASH::Realize)

	dword ticks;
	RDTSC_B(ticks);

	TSplash::lockTicks    = 0;
	TSplash::fillTicks    = 0; 
	TSplash::unlockTicks  = 0;
	TSplash::realizeTicks = 0;
	TSplash::processCount = 0;

	// draw bottom part
	TSplash::startRender = true;	
	TSplash::topIndex    = 0;
	int lastProcessed    = -1;
	for (int i = 0; i < MAX_SPLASHES; ++i)
		if (splashes[i].Process(_dTime))
			lastProcessed = i;
	if (lastProcessed != -1)
		splashes[lastProcessed].PostProcess();
	splashes[lastProcessed].Realize(_dTime);

	// draw top part
	bool techniqueStarted = renderer->TechniqueExecuteStart("splash2");
	TSplash::startRender = true;	
	TSplash::topIndex    = 0;
	lastProcessed		 = -1;
	for (i = 0; i < MAX_SPLASHES; ++i)
	{
		if (splashes[i].Process2(_dTime))
			lastProcessed = i;
	}
	splashes[lastProcessed].PostProcess2();
	splashes[lastProcessed].Realize2(_dTime);
	if(techniqueStarted) 
		while (renderer->TechniqueExecuteNext())
			;

	RDTSC_E(ticks);
	/*
	if ((GetKeyState('Z') & 0x8000) != 0)
	{
		renderer->Print(0, 150, "splash: all = %d, count = %d", ticks/1000, TSplash::processCount);
		if (TSplash::processCount)
		{
			renderer->Print(0, 175, "splash: lock = %d()", TSplash::lockTicks / 1000);
			renderer->Print(0, 200, "splash: fill = %d(%d)", TSplash::fillTicks / 1000, TSplash::fillTicks / (TSplash::processCount*1000));
			renderer->Print(0, 225, "splash: ulock = %d()", TSplash::unlockTicks / 1000);
			renderer->Print(0, 250, "splash: realize = %d(%d)", TSplash::realizeTicks / 1000, TSplash::realizeTicks / (2*1000));
		}
	}
	*/
	UNGUARD
}

//--------------------------------------------------------------------
void BALLSPLASH::Execute(dword _dTime)
{
	GUARD(BALLSPLASH::Execute)

	UNGUARD
}

//--------------------------------------------------------------------
void BALLSPLASH::InitializeSplashes()
{
	INIFILE *psIni = _CORE_API->fio->OpenIniFile("resource\\ini\\particles.ini");

	for (int i = 0; i < MAX_SPLASHES; ++i)
	{
		splashes[i].Release();
		splashes[i].Initialize(psIni, null, sea, renderer);
	}

	delete psIni;
}

//--------------------------------------------------------------------
TSplash *BALLSPLASH::TryToAddSplash(const CVECTOR &_pos, const CVECTOR &_dir)
{
	CVECTOR backDir = !_dir;
	backDir.y = -backDir.y;

	for (int i = 0; i < MAX_SPLASHES; ++i)
	{
		if (!splashes[i].Enabled())
		{
			splashes[i].Start(_pos, backDir);
			return &splashes[i];
		}
	}

	return 0;
}

//--------------------------------------------------------------------
