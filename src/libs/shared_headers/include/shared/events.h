#ifndef COMMON_EVENT_HPP_SCRIPT
#define COMMON_EVENT_HPP_SCRIPT

#define FORT_DESTROYED "FortDestroyed"
#define FORT_CAPTURED "FortCaptured"
#define SHIP_CAPTURED "ShipCaptured"
#define ABORDAGE_START_EVENT "AbordageStartEvent"

// cannons
#define GET_CANNON_BY_TYPE_EVENT "GetCannonByTypeEvent"

// nations
#define GET_RELATION_EVENT "GetRelationEvent"

// game
#define NEW_GAME_EVENT "NewGameEvent"
#define GAME_OVER_EVENT "GameOverEvent"

// crosshair
#define TELESCOPE_ACTIVE "TelescopeActive"

// Worldmap
#define WORLDMAP_ENCOUNTER_TYPE_UNKNOWN 0
#define WORLDMAP_ENCOUNTER_TYPE_MERCHANT 1
#define WORLDMAP_ENCOUNTER_TYPE_WARRING 2
#define WORLDMAP_ENCOUNTER_TYPE_FOLLOW 3
#define WORLDMAP_ENCOUNTER_TYPE_STORM 4
#define WORLDMAP_ENCOUNTER_TYPE_INVALID 2147483647 // INT32_MAX = 0x7fffffff

#endif
