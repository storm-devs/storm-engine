#include <stdio.h>
#include "..\common_h\messages.h"
//#include "..\common_h\defines.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
#include "..\SoundService\VSoundService.h"

#include "ANIMALS.h"

INTERFACE_FUNCTION
CREATE_CLASS(ANIMALS)
//--------------------------------------------------------------------
ANIMALS::ANIMALS()
	:seagulls(0)
	//,sharks(0)
	,fishSchools(0)
	,butterflies(0)
{	
	seagulls = NEW TSeagulls();
	//sharks = NEW TSharks();
	fishSchools = NEW TFishSchools();
	butterflies = NEW TButterflies();
}

//--------------------------------------------------------------------
ANIMALS::~ANIMALS()
{
	GUARD(ANIMALS::~ANIMALS)

	if (seagulls)
		delete seagulls;
	//if (sharks)
	//	delete sharks;
	if (fishSchools)
		delete fishSchools;
	if (butterflies)
		delete butterflies;

	UNGUARD
}

//--------------------------------------------------------------------
bool ANIMALS::Init()
{
	GUARD(ANIMALS::Init)

	_CORE_API->LayerAdd("realize",GetID(),77);
	_CORE_API->LayerAdd("execute",GetID(),77);

	seagulls->Init();
	//sharks->Init();
	fishSchools->Init();
	butterflies->Init();

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
dword _cdecl ANIMALS::ProcessMessage(MESSAGE & message)
{
	GUARD(ANIMALS::ProcessMessage)

	long code = message.Long();
	dword outValue = 0;

	switch (code)
	{
	case MSG_SOUND_SET_MASTER_VOLUME:
		break;
	default:
		outValue = seagulls->ProcessMessage(code, message);
		if (outValue)
			return outValue;
		outValue = fishSchools->ProcessMessage(code, message);
		if (outValue)
			return outValue;
		//outValue = sharks->ProcessMessage(code, message);
		//if (outValue)
		//	return outValue;
		outValue = butterflies->ProcessMessage(code, message);
		if (outValue)
			return outValue;

		break;
	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void ANIMALS::Realize(dword _dTime)
{
	GUARD(ANIMALS::Realize)

	seagulls->Realize(_dTime);
	//sharks->Realize(_dTime);
	fishSchools->Realize(_dTime);
	butterflies->Realize(_dTime);

	UNGUARD
}

//--------------------------------------------------------------------
void ANIMALS::Execute(dword _dTime)
{
	GUARD(ANIMALS::Execute)

	seagulls->Execute(_dTime);
	//sharks->Execute(_dTime);
	fishSchools->Execute(_dTime);
	butterflies->Execute(_dTime);
	
	UNGUARD
}

//--------------------------------------------------------------------
dword ANIMALS::AttributeChanged(ATTRIBUTES * _pA)
{
	if (*_pA == "midY")
	{
		seagulls->SetStartY(this->AttributesPointer->GetAttributeAsFloat("midY"));
	}

	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
