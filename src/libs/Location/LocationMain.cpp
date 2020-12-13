//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Sea dogs II
//--------------------------------------------------------------------------------------------
//	main
//--------------------------------------------------------------------------------------------
//	≈кспорт классов дл€ Storm engine v2.0
//============================================================================================

#include "Blood.h"
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
#include "Player.h"
#include "WideScreen.h"

#define LOCATION_SCRIPT_VERSION 54128

class LocationP : public Entity
{
  public:
    LocationP();
    ~LocationP();

    bool Init() override;
    void Execute(uint32_t dwDeltaTime);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
            // case Stage::realize:
            //	Realize(delta); break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }
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

void LocationP::Execute(uint32_t dwDeltaTime)
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

CREATE_CLASS(LocationP)

//============================================================================================

CREATE_SCRIPTLIBRIARY(ScriptLocationLibrary)
