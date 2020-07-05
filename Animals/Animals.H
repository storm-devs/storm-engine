#ifndef _ANIMALS_H_
#define _ANIMALS_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"
#include "TSeagulls.h"
//#include "TSharks.h"
#include "TFishSchools.h"
#include "TButterflies.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class ANIMALS: public ENTITY
{
public:
	ANIMALS();
	virtual ~ANIMALS();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);
	dword AttributeChanged(ATTRIBUTES * pA);
	//virtual bool CreateState(ENTITY_STATE_GEN * state_gen);
	//virtual bool LoadState(ENTITY_STATE * state);

private:
	TSeagulls    *seagulls;
	//TSharks      *sharks;
	TFishSchools *fishSchools;
	TButterflies *butterflies;
};

//API_MODULE_START("Animals")
//	CREATE_CLASS(ANIMALS)
//API_MODULE_END

#endif