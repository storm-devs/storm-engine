#pragma once

#include "vmodule_api.h"

#include "../../../libs/animals/src/Animals.h"

#include "../../../libs/animation/src/AnimationServiceImp.h"

#include "../../../libs/ball_splash/src/BallSplash.h"

#include "../../../libs/battle_interface/src/ActivePerkShower.h"
#include "../../../libs/battle_interface/src/ItemEntity/itementity.h"
#include "../../../libs/battle_interface/src/LogAndAction.h"
#include "../../../libs/battle_interface/src/WorldMapInterface/interface.h"
#include "../../../libs/battle_interface/src/interfacemanager/InterfaceManager.h"
#include "../../../libs/battle_interface/src/land/IBoardingStatus.h"
#include "../../../libs/battle_interface/src/land/battle_land.h"
#include "../../../libs/battle_interface/src/sea/ShipPointer.h"
#include "../../../libs/battle_interface/src/sea/ibattle.h"
#include "../../../libs/battle_interface/src/spyglass/spyglass.h"
#include "../../../libs/battle_interface/src/timer/Timer.h"

#include "../../../libs/blade/src/blade.h"

#include "../../../libs/blot/src/Blots.h"

#include "../../../libs/collide/src/vcollide.h"

#include "../../../libs/dialog/src/dialog.h"

#include "../../../libs/geometry/src/geometry_R.h"

#include "../../../libs/island/src/Foam.h"
#include "../../../libs/island/src/ISLAND.h"

#include "../../../libs/lighter/src/Lighter.h"

#include "../../../libs/location/src/Blood.h"
#include "../../../libs/location/src/CharacterAnimationKipper.h"
#include "../../../libs/location/src/CharactersGroups.h"
#include "../../../libs/location/src/Fader.h"
#include "../../../libs/location/src/Grass.h"
#include "../../../libs/location/src/Lights.h"
#include "../../../libs/location/src/Lizards.h"
#include "../../../libs/location/src/LocCrabs.h"
#include "../../../libs/location/src/LocEagle.h"
#include "../../../libs/location/src/LocRats.h"
#include "../../../libs/location/src/Location.h"
#include "../../../libs/location/src/LocationCamera.h"
#include "../../../libs/location/src/LocationEffects.h"
#include "../../../libs/location/src/LocationScriptLib.h"
#include "../../../libs/location/src/ModelRealizer.h"
#include "../../../libs/location/src/NPCharacter.h"
#include "../../../libs/location/src/Player.h"
#include "../../../libs/location/src/WideScreen.h"

#include "../../../libs/locator/src/blast.h"
#include "../../../libs/locator/src/locator.h"

#include "../../../libs/mast/src/mast.h"

#include "../../../libs/model/src/modelr.h"

#include "../../../libs/particles/src/K2Wrapper/particles.h"
#include "../../../libs/particles/src/Service/particleservice.h"

#include "../../../libs/pcs_controls/include/pcs_controls.h"

#include "../../../libs/renderer/src/sdevice.h"

#include "../../../libs/rigging/src/Flag.h"
#include "../../../libs/rigging/src/Rope.h"
#include "../../../libs/rigging/src/Vant.h"
#include "../../../libs/rigging/src/sail.h"
#include "../../../libs/rigging/src/script_func.h"

#include "../../../libs/sailors/src/Sailors.h"
#include "../../../libs/sailors/src/SailorsEditor.h"

#include "../../../libs/script_library/src/script_libriary_test.h"

#include "../../../libs/sea/src/SEA.h"

#include "../../../libs/sea_creatures/src/Sharks.h"

#include "../../../libs/sea_foam/src/Seafoam.h"

#include "../../../libs/sea_ai/src/AIBalls.h"
#include "../../../libs/sea_ai/src/AIFort.h"
#include "../../../libs/sea_ai/src/AISeaGoods.h"
#include "../../../libs/sea_ai/src/LocatorShow.h"
#include "../../../libs/sea_ai/src/SEA_AI.h"

#include "../../../libs/sea_cameras/src/DeckCamera.h"
#include "../../../libs/sea_cameras/src/FreeCamera.h"
#include "../../../libs/sea_cameras/src/SEA_CAMERAS.h"
#include "../../../libs/sea_cameras/src/ShipCamera.h"

#include "../../../libs/sea_operator/src/SEA_OPERATOR.h"

#include "../../../libs/shadow/src/shadow.h"

#include "../../../libs/ship/src/ShipLights.h"
#include "../../../libs/ship/src/Track.h"
#include "../../../libs/ship/src/ship.h"

#include "../../../libs/sink_effect/src/SinkEffect.h"

#include "../../../libs/sound/src/Sound.h"

#include "../../../libs/sound_service/src/DebugEntity.h"
#include "../../../libs/sound_service/src/SoundService.h"

#include "../../../libs/teleport/src/teleport.h"

#include "../../../libs/tornado/src/Tornado.h"

#include "../../../libs/touch/src/touch.h"

#include "../../../libs/water_rings/src/WaterRings.h"

#include "../../../libs/weather/src/Astronomy.h"
#include "../../../libs/weather/src/Lightning.h"
#include "../../../libs/weather/src/Rain.h"
#include "../../../libs/weather/src/SunGlow.h"
#include "../../../libs/weather/src/WaterFlare.h"
#include "../../../libs/weather/src/Weather.h"

#include "../../../libs/worldmap/src/WorldMap.h"

#include "../../../libs/xinterface/src/BackScene/backscene.h"
#include "../../../libs/xinterface/src/HelpChooser/HelpChooser.h"
#include "../../../libs/xinterface/src/InfoHandler.h"
#include "../../../libs/xinterface/src/Scrshoter/scrshoter.h"
#include "../../../libs/xinterface/src/Stringservice/obj_strservice.h"
#include "../../../libs/xinterface/src/Stringservice/strservice.h"
#include "../../../libs/xinterface/src/TextureSequence/TextureSequence.h"
#include "../../../libs/xinterface/src/aviplayer/aviplayer.h"
#include "../../../libs/xinterface/src/xinterface.h"

#include "SteamApiScriptLib.hpp"

// animals
CREATE_CLASS(ANIMALS)

// animation
CREATE_SERVICE(AnimationServiceImp)

// ballsplash
CREATE_CLASS(BALLSPLASH)

// battle_interface
CREATE_CLASS(BATTLE_INTERFACE)
CREATE_CLASS(ILogAndActions)
CREATE_CLASS(IBoardingStatus)
CREATE_CLASS(BATTLE_LAND_INTERFACE)
CREATE_CLASS(ISPYGLASS)
CREATE_CLASS(SHIPPOINTER)
CREATE_CLASS(ActivePerkShower)
CREATE_CLASS(BITimer)
CREATE_CLASS(ItemEntity)
CREATE_CLASS(WM_INTERFACE)
CREATE_CLASS(BI_InterfaceManager)

// blade
CREATE_CLASS(BLADE)

// blot
CREATE_CLASS(Blots)

// collide
CREATE_SERVICE(COLL)

// dialog
CREATE_CLASS(DIALOG)

// geometry
CREATE_SERVICE(GEOMETRY)

// island
CREATE_CLASS(ISLAND)
CREATE_CLASS(CoastFoam)

// lighter
CREATE_CLASS(Lighter)

// location
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
CREATE_SCRIPTLIBRIARY(ScriptLocationLibrary)

// locator
CREATE_CLASS(LOCATOR)
CREATE_CLASS(BLAST)

// mast
CREATE_CLASS(MAST)
CREATE_CLASS(HULL)

// model
CREATE_CLASS(MODELR)

// particles
CREATE_SERVICE(ParticleService)
CREATE_CLASS(PARTICLES)

// pcs_controls
CREATE_SERVICE(PCS_CONTROLS)

// renderer
CREATE_SERVICE(DX9RENDER)
CREATE_SCRIPTLIBRIARY(DX9RENDER_SCRIPT_LIBRIARY)

// rigging
CREATE_CLASS(SAIL)
CREATE_CLASS(FLAG)
CREATE_CLASS(ROPE)
CREATE_CLASS(VANT)
CREATE_CLASS(VANTL)
CREATE_CLASS(VANTZ)
CREATE_SCRIPTLIBRIARY(SCRIPT_RIGGING_FILES)

// sailors
CREATE_CLASS(Sailors)
CREATE_CLASS(SailorsEditor)

// script_library
CREATE_SCRIPTLIBRIARY(SCRIPT_LIBRIARY_TEST)

// sea
CREATE_CLASS(SEA)

// seacreatures
CREATE_CLASS(Sharks)

// seafoam
CREATE_CLASS(SEAFOAM)

// sea_ai
CREATE_CLASS(SEA_AI)
CREATE_CLASS(AIFort)
CREATE_CLASS(AIBalls)
CREATE_CLASS(AISeaGoods)
CREATE_CLASS(SeaLocatorShow)

// sea_cameras
CREATE_CLASS(SEA_CAMERAS)
CREATE_CLASS(FREE_CAMERA)
CREATE_CLASS(SHIP_CAMERA)
CREATE_CLASS(DECK_CAMERA)

// sea_operator
CREATE_CLASS(SEA_OPERATOR)

// shadow
CREATE_CLASS(Shadow)

// ship
CREATE_CLASS(SHIP)
CREATE_CLASS(ShipLights)
CREATE_CLASS(ShipTracks)

// sink_effect
CREATE_CLASS(SINKEFFECT)

// sound
CREATE_CLASS(SOUND)

// soundservice
CREATE_SERVICE(SoundService)
CREATE_CLASS(SoundVisualisationEntity)

// teleport
CREATE_CLASS(TMPTELEPORT)
CREATE_CLASS(FINDFILESINTODIRECTORY)
CREATE_CLASS(FINDDIALOGNODES)

// tornado
CREATE_CLASS(Tornado)

// touch
CREATE_CLASS(TOUCH)

// waterrings
CREATE_CLASS(WaterRings)

// weather
CREATE_CLASS(WEATHER)
CREATE_CLASS(RAIN)
CREATE_CLASS(SUNGLOW)
CREATE_CLASS(LIGHTNING)
CREATE_CLASS(SKY)
CREATE_CLASS(WATERFLARE)
CREATE_CLASS(Astronomy)

// worldmap
CREATE_CLASS(WorldMap)

// xinterface
CREATE_SERVICE(STRSERVICE)
CREATE_SCRIPTLIBRIARY(SCRIPT_INTERFACE_FUNCTIONS)
CREATE_CLASS(OBJ_STRSERVICE)
CREATE_CLASS(TextureSequence)
CREATE_CLASS(XINTERFACE)
CREATE_CLASS(SCRSHOTER)
CREATE_CLASS(HELPCHOOSER)
CREATE_CLASS(InfoHandler)
CREATE_CLASS(CONTROLS_CONTAINER)
CREATE_CLASS(InterfaceBackScene)
CREATE_CLASS(CAviPlayer)

// steam
using steamapi::SteamApiScriptLib;
CREATE_SCRIPTLIBRIARY(SteamApiScriptLib)