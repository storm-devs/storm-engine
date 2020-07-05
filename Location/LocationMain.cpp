//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Sea dogs II
//--------------------------------------------------------------------------------------------
//	main
//--------------------------------------------------------------------------------------------
//	≈кспорт классов дл€ Storm engine v2.0
//============================================================================================

#include "Location.h"
#include "Character.h"
#include "Player.h"
#include "LocationCamera.h"
#include "NPCharacter.h"
#include "Fader.h"
#include "Grass.h"
#include "Lights.h"
#include "WideScreen.h"
#include "CharacterAnimationKipper.h"
#include "LocationEffects.h"
#include "CharactersGroups.h"
#include "LocEagle.h"
#include "Lizards.h"
#include "LocationScriptLib.h"
#include "LocRats.h"
#include "ModelRealizer.h"
#include "Blood.h"

#define	LOCATION_SCRIPT_VERSION		54128

class LocationP : public ENTITY
{
public:
	LocationP();
	~LocationP();

	bool Init();
	void Execute(dword dwDeltaTime);
};

LocationP::LocationP()
{
}

LocationP::~LocationP()
{
}

bool LocationP::Init()
{
	return true;
}

void LocationP::Execute(dword dwDeltaTime)
{
	if (dwDeltaTime != LOCATION_SCRIPT_VERSION) throw;
}

//============================================================================================

INTERFACE_FUNCTION
CREATE_CLASS(Location)
CREATE_CLASS(NPCharacter)
CREATE_CLASS(Player)
CREATE_CLASS(LocationCamera)
CREATE_CLASS(Fader)
CREATE_CLASS(Grass)
CREATE_CLASS(Lights)
CREATE_CLASS(WideScreen)
CREATE_CLASS(CharacterAnimationKipper)
CREATE_CLASS(LocationEffects)
CREATE_CLASS(CharactersGroups)
CREATE_CLASS(LocEagle)
CREATE_CLASS(Lizards)
CREATE_CLASS(LocRats)
CREATE_CLASS(LocModelRealizer)
CREATE_CLASS(Blood)
CREATE_CLASS(LocationP)

//============================================================================================

CREATE_SCRIPTLIBRIARY(ScriptLocationLibrary)