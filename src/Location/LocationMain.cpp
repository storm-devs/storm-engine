//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Sea dogs II
//--------------------------------------------------------------------------------------------
//	main
//--------------------------------------------------------------------------------------------
//	Експорт классов для Storm engine v2.0
//============================================================================================

#include "Blood.h"
#include "Character.h"
#include "CharacterAnimationKipper.h"
#include "CharactersGroups.h"
#include "Fader.h"
#include "Grass.h"
#include "Lights.h"
#include "Lizards.h"
#include "LocCrabs.h"
#include "LocEagle.h"
#include "LocRats.h"
#include "Location.h"
#include "LocationCamera.h"
#include "LocationEffects.h"
#include "LocationScriptLib.h"
#include "ModelRealizer.h"
#include "NPCharacter.h"
#include "Player.h"
#include "WideScreen.h"

#define LOCATION_SCRIPT_VERSION 57853
//#define	LOCATION_SCRIPT_VERSION		54128

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
    if (dwDeltaTime != LOCATION_SCRIPT_VERSION)
        throw;
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
CREATE_CLASS(LocCrabs)
CREATE_CLASS(LocModelRealizer)
CREATE_CLASS(Blood)

class LocationPvmacd : public VMA
{
  public:
    LocationPvmacd(VMA *&_pR)
    {
        pNext = _pR;
        _pR = this;
    };

    char *GetName()
    {
        return "LocationP";
    }

    void *CreateClass()
    {
        nReference++;
        return new ("LocationMain.cpp", 83) LocationP;
    }
};

LocationPvmacd LocationPvmaci(_pModuleClassRoot);

//============================================================================================

CREATE_SCRIPTLIBRIARY(ScriptLocationLibrary)