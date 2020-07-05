#include <stdio.h>
#include <stdlib.h>
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
#include "..\SoundService\VSoundService.h"
#include "SINKEFFECT.h"

INTERFACE_FUNCTION
CREATE_CLASS(SINKEFFECT)

//--------------------------------------------------------------------
SINKEFFECT::SINKEFFECT()
	:sea(0)
	,renderer(0)
{
}

//--------------------------------------------------------------------
SINKEFFECT::~SINKEFFECT()
{
	GUARD(SINKEFFECT::~SINKEFFECT)

	UNGUARD
}

//--------------------------------------------------------------------
bool SINKEFFECT::Init()
{
	GUARD(SINKEFFECT::Init)

	ENTITY_ID seaID;
	_CORE_API->FindClass(&seaID, "sea", 0);
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);

	renderer = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	InitializeSinks();

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
dword _cdecl SINKEFFECT::ProcessMessage(MESSAGE & message)
{
	GUARD(SINKEFFECT::ProcessMessage)

	ENTITY_ID shipID;
	SHIP_BASE *shipBase;
	long code = message.Long();
	dword outValue = 0;

	switch (code)
	{
	case MSG_SHIP_DELETE:
		{
			ATTRIBUTES *attrs = message.AttributePointer();
			if (attrs)
			{
				if (_CORE_API->FindClass(&shipID, "ship", 0))
				{
					/*
					shipBase = (SHIP_BASE *) _CORE_API->GetEntityPointer(&shipID);
					if (shipBase->GetACharacter() == attrs)
					{
						TryToAddSink(shipBase->GetPos(), shipBase->GetBoxSize().z / 2.0f);
						return outValue;
					}*/

					do 
					{
						shipBase = (SHIP_BASE *) _CORE_API->GetEntityPointer(&shipID);
						if (shipBase->GetACharacter() == attrs)
						{
							TryToAddSink(shipBase->GetPos(), shipBase->GetBoxSize().z / 2.0f);
							return outValue;
						}
					} while (_CORE_API->FindClassNext(&shipID));
				}//if (FindClass)
			}//if (attrs)
		}//case
		break;
	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void SINKEFFECT::Realize(dword _dTime)
{
	GUARD(SINKEFFECT::Realize)

	for (int i = 0; i < MAX_SINKS; ++i)
		sinks[i].Realize(_dTime);

	UNGUARD
}

//--------------------------------------------------------------------
void SINKEFFECT::Execute(dword _dTime)
{
	GUARD(SINKEFFECT::Execute)
/*
	if (GetAsyncKeyState('X'))
	{
		if (renderer && sea)
		{
			static CVECTOR pos, ang, nose, head;
			static CMatrix view;
			//CVECTOR dir(randCentered(2.0f), -1.0f, randCentered(2.0f));
			CVECTOR dir(0.0f, -1.0f, 0.0f);

			renderer->GetTransform(D3DTS_VIEW, view);
			view.Transposition();
			nose = view.Vz();
			//head = view.Vy();
			pos = view.Pos();

			pos += 10.0f * !nose;
			pos.y = sea->WaveXZ(pos.x, pos.y);
			TSink *sinks= TryToAddSink(pos, 10.0f);
		}
	}
*/
	for (int i = 0; i < MAX_SINKS; ++i)
		sinks[i].Process(_dTime);

	UNGUARD
}

//--------------------------------------------------------------------
void SINKEFFECT::InitializeSinks()
{
	INIFILE *psIni = _CORE_API->fio->OpenIniFile("resource\\ini\\particles.ini");

	for (int i = 0; i < MAX_SINKS; ++i)
	{
		sinks[i].Release();
		sinks[i].Initialize(psIni, null, sea, renderer);
	}

	delete psIni;
}

//--------------------------------------------------------------------
TSink *SINKEFFECT::TryToAddSink(const CVECTOR &_pos, float _r)
{
	for (int i = 0; i < MAX_SINKS; ++i)
	{
		if (!sinks[i].Enabled())
		{
			sinks[i].Start(_pos, _r);
			return &sinks[i];
		}
	}

	return 0;
}

//--------------------------------------------------------------------
