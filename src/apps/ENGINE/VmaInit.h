#pragma once

#include "vmodule_api.h"

#include "../../libs/animals/Animals.h"

#include "../../libs/animation/AnimationServiceImp.h"

#include "../../libs/ballsplash/BallSplash.H"

#include "../../libs/battle_interface/ActivePerkShower.h"
#include "../../libs/battle_interface/ItemEntity/itementity.h"
#include "../../libs/battle_interface/LogAndAction.h"
#include "../../libs/battle_interface/WorldMapInterface/interface.h"
#include "../../libs/battle_interface/interfacemanager/InterfaceManager.h"
#include "../../libs/battle_interface/land/IBoardingStatus.h"
#include "../../libs/battle_interface/land/battle_land.h"
#include "../../libs/battle_interface/sea/ShipPointer.h"
#include "../../libs/battle_interface/sea/ibattle.h"
#include "../../libs/battle_interface/spyglass/spyglass.h"
#include "../../libs/battle_interface/timer/Timer.h"

#include "../../libs/blade/blade.h"

#include "../../libs/blot/Blots.h"

#include "../../libs/collide/vcollide.h"

#include "../../libs/dialog/dialog.h"

#include "../../libs/geometry/geometry_R.h"

#include "../../libs/island/Foam.h"
#include "../../libs/island/ISLAND.h"

#include "../../libs/lighter/Lighter.h"

#include "../../libs/location/Blood.h"
#include "../../libs/location/CharacterAnimationKipper.h"
#include "../../libs/location/CharactersGroups.h"
#include "../../libs/location/Fader.h"
#include "../../libs/location/Grass.h"
#include "../../libs/location/Lights.h"
#include "../../libs/location/Lizards.h"
#include "../../libs/location/LocCrabs.h"
#include "../../libs/location/LocEagle.h"
#include "../../libs/location/LocRats.h"
#include "../../libs/location/Location.h"
#include "../../libs/location/LocationCamera.h"
#include "../../libs/location/LocationEffects.h"
#include "../../libs/location/LocationScriptLib.h"
#include "../../libs/location/ModelRealizer.h"
#include "../../libs/location/NPCharacter.h"
#include "../../libs/location/Player.h"
#include "../../libs/location/WideScreen.h"

#include "../../libs/locator/blast.h"
#include "../../libs/locator/locator.h"

#include "../../libs/mast/mast.h"

#include "../../libs/model/modelr.h"

#include "../../libs/particles/K2Wrapper/particles.h"
#include "../../libs/particles/Service/particleservice.h"

#include "../../libs/pcs_controls/pcs_controls.h"

#include "../../libs/renderer/sdevice.h"

#include "../../libs/rigging/Flag.h"
#include "../../libs/rigging/Rope.h"
#include "../../libs/rigging/Vant.h"
#include "../../libs/rigging/sail.h"
#include "../../libs/rigging/script_func.h"

#include "../../libs/sailors/Sailors.h"
#include "../../libs/sailors/SailorsEditor.h"

#include "../../libs/script_library/script_libriary_test.h"

#include "../../libs/sea/sea.h"

#include "../../libs/seacreatures/Sharks.h"

#include "../../libs/seafoam/Seafoam.h"

#include "../../libs/sea_ai/AIBalls.h"
#include "../../libs/sea_ai/AIFort.h"
#include "../../libs/sea_ai/AISeaGoods.h"
#include "../../libs/sea_ai/LocatorShow.h"
#include "../../libs/sea_ai/SEA_AI.h"

#include "../../libs/sea_cameras/DeckCamera.h"
#include "../../libs/sea_cameras/FreeCamera.h"
#include "../../libs/sea_cameras/SEA_CAMERAS.h"
#include "../../libs/sea_cameras/ShipCamera.h"

#include "../../libs/sea_operator/SEA_OPERATOR.h"

#include "../../libs/shadow/shadow.h"

#include "../../libs/ship/ShipLights.h"
#include "../../libs/ship/Track.h"
#include "../../libs/ship/ship.h"

#include "../../libs/sink_effect/SinkEffect.H"

#include "../../libs/sound/Sound.h"

#include "../../libs/soundservice/DebugEntity.h"
#include "../../libs/soundservice/SoundService.h"

#include "../../libs/teleport/teleport.h"

#include "../../libs/tornado/Tornado.h"

#include "../../libs/touch/touch.h"

#include "../../libs/waterrings/WaterRings.h"

#include "../../libs/weather/Astronomy.h"
#include "../../libs/weather/Lightning.h"
#include "../../libs/weather/Rain.h"
#include "../../libs/weather/SunGlow.h"
#include "../../libs/weather/WaterFlare.h"
#include "../../libs/weather/Weather.h"

#include "../../libs/worldmap/WorldMap.h"

#include "../../libs/xinterface/BackScene/backscene.h"
#include "../../libs/xinterface/HelpChooser/HelpChooser.h"
#include "../../libs/xinterface/InfoHandler.h"
#include "../../libs/xinterface/SCRSHOTER/scrshoter.h"
#include "../../libs/xinterface/Stringservice/obj_strservice.h"
#include "../../libs/xinterface/Stringservice/strservice.h"
#include "../../libs/xinterface/TextureSequence/TextureSequence.h"
#include "../../libs/xinterface/aviplayer/aviplayer.h"
#include "../../libs/xinterface/xinterface.h"

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