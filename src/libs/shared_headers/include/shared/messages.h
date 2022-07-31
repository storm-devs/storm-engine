#ifndef COMMON_MESSAGES_HPP
#define COMMON_MESSAGES_HPP

//============================================================================================
// Model messages
//============================================================================================

#define MSG_MODEL_LOAD_GEO 20500 // "ls", string
#define MSG_MODEL_LOAD_ANI 20501
#define MSG_MODEL_SET_PARENT 20502 // "li", entid_t
#define MSG_MODEL_SET_LIGHT_PATH 20503
#define MSG_MODEL_RELEASE 20504
#define MSG_MODEL_RESTORE 20505
#define MSG_MODEL_SET_POSITION 20506
//    SendMessage(ref _model, "lffffffffffff", MSG_MODEL_SET_POSITION, float _x, float _y, float _z,
//        float _vxx, float _vxy, float _vxz, float _vyx, float _vyy, float _vyz, float _vzx, float _vzy, float _vzz);

#define MSG_MODEL_BLEND 20507
#define MSG_MODEL_SET_LIGHT_LMPATH 20508
#define MSG_MODEL_SET_DIRPATH 20509   // "s" = "texture_path\\"
#define MSG_MODEL_SET_TECHNIQUE 20510 // "s" = technique
#define MSG_MODEL_SET_FOG 20511
#define MSG_MODEL_SET_MAX_VIEW_DIST 20512

#define MSG_MODEL_SUBSTITUTE_GEOMETRY_NODE 20600 // "ss", geometry, new model name
#define MSG_MODEL_PLAY_ACTION 20601 // "lls", player, action name

//============================================================================================
// blade messages
//============================================================================================
#define MSG_BLADE_SET 21000           //"lisfll, entity, modelName, lifeTime, color[2]
#define MSG_BLADE_BELT 21001          //"l", default
#define MSG_BLADE_HAND 21002          //"l"
#define MSG_BLADE_TRACE_ON 21003      //"l"
#define MSG_BLADE_TRACE_OFF 21004     //"l"
#define MSG_BLADE_BLOOD 21005         //"l"
#define MSG_BLADE_LIGHT 21006         //"l"
#define MSG_BLADE_GUNSET 21007        //"lis, entity, modelName
#define MSG_BLADE_GUNBELT 21008       // l
#define MSG_BLADE_GUNHAND 21009       // l
#define MSG_BLADE_GUNFIRE 21010       // l
#define MSG_CHARACTER_BLADEHAND 21011 //"ll", boolBladeInHand
#define MSG_BLADE_ALPHA 21012         //"ll", alpha|0xFFFFFF

//============================================================================================
// Island Messages
//============================================================================================
#define MSG_ISLAND_LOAD_GEO 50100 // "ls", island name
#define MSG_ISLAND_SET_GEO 50101  // "lis", model_id,island name
#define MSG_ISLAND_START 50102    // "lis", model_id,island name
#define MSG_ISLAND_ADD_FORT 50103 // "li", model_id

//============================================================================================
// Sea Reflection Messages
//============================================================================================
#define MSG_SEA_REFLECTION_DRAW 50200 // "ls", island name
#define MSG_SEA_SUNROAD_DRAW 50201    // "ls", island name

//============================================================================================
// Ship Messages
//============================================================================================
#define MSG_SHIP_CREATE 50300
#define MSG_SHIP_DELETE 50301
#define MSG_SHIP_SEA_INFO 50302
#define MSG_SHIP_ACTIVATE_FIRE_PLACE 50304
#define MSG_SHIP_SET_SAIL_STATE 50305
#define MSG_SHIP_GET_NUM_FIRE_PLACES 50306
#define MSG_SHIP_GET_CHARACTER_INDEX 50307
#define MSG_SHIP_ADD_MOVE_IMPULSE 50308
#define MSG_SHIP_RESET_TRACK 50309
#define MSG_SHIP_SETLIGHTSOFF 50310
#define MSG_SHIP_FLAG_REFRESH 50312 // boal 20.08.06
#define MSG_SHIP_SET_CUSTOM_FLAG 50313 // "li", flag entity - Specify a custom flag entity to use for this ship
#define MSG_SHIP_SAFE_DELETE 50315
#define MSG_SHIP_GET_SAIL_STATE 50325
#define MSG_SHIP_CURVES 50400
#define MSG_SHIP_LIGHTSRESET 50402 // ugeen 25.12.19
#define MSG_SHIP_DO_FAKE_FIRE 50403

//============================================================================================
// Weather Messages
//============================================================================================
#define MSG_WHR_LIGHTNING_ADD 54000

//============================================================================================
// Location messages
//============================================================================================

// Location
// Add a model to a location
// "lssl" load static model, read its locators, set rendering technique and drawing level
#define MSG_LOCATION_ADD_MODEL 30100
#define MSG_LOCATION_GET_MODEL 30101 // "le" get model identifier
// Set the last added model properties
#define MSG_LOCATION_MODEL_SET_POS 30110 // "lss" put the last loaded model into the locator
#define MSG_LOCATION_MODEL_SET_ROT 30111 // "lfff" set the rotation speed of the last loaded model
// set the texture sliding effect to the last loaded model (ffff - velocities u0, v0, u1, v1)
#define MSG_LOCATION_MODEL_SET_UVS 30112
#define MSG_LOCATION_MODEL_LAMPS 30113 // "l" add the current model to the ignore list for tracing rays from lamps
// "lf" set the effect of generating the matrix for reflection, f is the scaling factor
#define MSG_LOCATION_MODEL_REFLECTION 30114
// Install patches
#define MSG_LOCATION_SET_CHRS_PATCH 30120 // "ls" load the character patch
#define MSG_LOCATION_SET_CAM_PATCH 30121  // "ls" load camera patch
#define MSG_LOCATION_SET_JMP_PATCH 30122  // "ls" load the jump patch
#define MSG_LOCATION_SET_GRS_PATCH 30123  // "ls" load grass
// Radius of locators
#define MSG_LOCATION_GRP_RADIUS 30125 // "lsf" set the radius to the locator group
#define MSG_LOCATION_LOC_RADIUS 30126 // "lssf" set the radius to the locator
#define MSG_LOCATION_LOC_EXISTS 30127 // "lssf" set the radius to the locator
// Add light source
#define MSG_LOCATION_ADD_LIGHT 30128 // "lsfff" source name, source position
// Register locators
#define MSG_LOCATION_UPDATELOCATORS 30130 // "l" copy all locators to location attributes
// File paths
#define MSG_LOCATION_MODELSPATH 30140   // "ls" set relative path to location models
#define MSG_LOCATION_TEXTURESPATH 30141 // "ls" set relative path to location textures
#define MSG_LOCATION_LIGHTPATH 30142    // "ls" set relative path to location lighting
#define MSG_LOCATION_SHADOWPATH 30143   // "ls" set relative path to the texture on the floor
// Utilities
// "lss" check the entrance to the location for the possibility of coming to it (is it free)
#define MSG_LOCATION_CHECKENTRY 30150
#define MSG_LOCATION_PAUSE 30151           // "ll" pause the location
#define MSG_LOCATION_SETCHRPOSITIONS 30152 // "l" save positions to restore
#define MSG_LOCATION_TLPCHRPOSITIONS 30153 // "l" move and delete positions to restore
#define MSG_LOCATION_CLRCHRPOSITIONS 30154 // "l" delete positions to restore
#define MSG_LOCATION_EX_MSG 30155          // "ls" extended s command, ...
#define MSG_LOCATION_VIEWSTATEBARS 30156   // "ll" display of enemy state (HP, energy) <-- ugeen 05.24.12
// Debug
// "lsffl" show a group of locators in the form of spheres with a scaling factor of the radius,
// distance of visibility of marks, with colors
#define MSG_LOCATION_VIEWLGROUP 30180
#define MSG_LOCATION_HIDELGROUP 30181 // "ls" hide locator group

// Characters
// Character model
#define MSG_CHARACTER_SETMODEL 30200 // "lss" load model with animation
#define MSG_CHARACTER_GETMODEL 30201 // "le" get character model
#define MSG_CHARACTER_GETPOS 30202   // "leee" get character position
#define MSG_CHARACTER_GETAY 30203    // "le" get character position
#define MSG_CHARACTER_DIST2D 30204   // "lie" get the distance to the character
#define MSG_CHARACTER_DIST3D 30205   // "lie" get the distance to the character
// Teleportation
#define MSG_CHARACTER_TELEPORT 30210    // "lfff" move character to point x, y, z
#define MSG_CHARACTER_TELEPORT_AY 30211 // "lffff" move character to point x, y, z and point along ay
// "lss" move character to locator specified as group name, locator name
#define MSG_CHARACTER_TELEPORT_TO_LOCATOR 30212
// Locator analysis
#define MSG_CHARACTER_ADD_DETECTOR 30220 // "ls" add locator group for analysis
#define MSG_CHARACTER_DEL_DETECTOR 30221 // "ls" remove locator group for analysis
// Dynamic character movement
#define MSG_CHARACTER_ENTRY_TO_LOCATION 30330  // "lss" dynamically place the character in the location
#define MSG_CHARACTER_EXIT_FROM_LOCATION 30331 // "l" remove character from location
// Character rotation
#define MSG_CHARACTER_TURNBYLOC 30340   // "lss" point towards locator
#define MSG_CHARACTER_TURNBYCHR 30341   // "le" point to another character
#define MSG_CHARACTER_TURNBYPOINT 30342 // "lfff" point towards point
#define MSG_CHARACTER_TURNAY 30343      // "lf" point by an angle
#define MSG_CHARACTER_TURNPUSH 30344    // "l" save the rotation of the character on the stack
#define MSG_CHARACTER_TURNPOP 30345     // "l" save the rotation of the character on the stack
// Animation playback
#define MSG_CHARACTER_ACTIONPLAY 30350 // "ls" play action. if "" - switch to idle animation mode
// Set a saber to a character
#define MSG_CHARACTER_SETBLADE 30360    // "lsfll" install saber
#define MSG_CHARACTER_SETGUN 30361      // "ls" install saber
#define MSG_CHARACTER_SETFTGLEVEL 30362 // "lf" set the fighting level

// Search for characters
// "leeffffll" install saber
// l - MSG_CHARACTER_FINDNEAR
// e - a reference to an array of objects that will be filled:
// index - character index
// dist - distance between characters
// dx, dy, dz - deltas in the direction of the character
// if necessary, the array size will be increased
// e - reference to the int variable - the number of characters found
// Search terms:
// f - search radius
// f - test angle in height -> sin(ax) * dist 0 throughout the entire distance to the character
// f - analysis angle in the horizontal plane, 0 do not analyze
// f - distance of the near clipping plane, if the previous test works
// l - 1 test visibility, 0 no
// l - 1 sort by range, 0 no
#define MSG_CHARACTER_FINDNEAR 30370

// Checking his visibility by us
#define MSG_CHARACTER_VISIBLE 30371 // "i"
// "lfff" output about the damage inflicted on the character f - inflicted, f - current hp, f - total hp
#define MSG_CHARACTER_VIEWDAMAGE 30372
#define MSG_CHARACTER_EX_MSG 30373

// Procedural head look
#define MSG_CHARACTER_HEADLOOK_DEFAULT 30380    // "l" default (ani)
#define MSG_CHARACTER_HEADLOOK_CAMERA 30381     // "l" follow the camera
#define MSG_CHARACTER_HEADLOOK_CHARACTER 30382  // "li" follow another character
#define MSG_CHARACTER_HEADLOOK_POINT 30383      // "lfff" follow a point

// Setting tasks for the character
#define MSG_NPCHARACTER_SETTASK 30400
#define MSG_NPCHARACTER_PUSHTASK 30401
#define MSG_NPCHARACTER_POPTASK 30402
#define MSG_NPCHARACTER_GETTASK 30403

// Camera
#define MSG_CAMERA_SETTARGET 30500 // "li" set the model to follow
#define MSG_CAMERA_FOLLOW 30510    // "l" set camera mode - chase character
#define MSG_CAMERA_LOOK 30511      // "l" set camera mode - eye view
#define MSG_CAMERA_TOPOS 30512     // "lfffl" set the camera mode - be at the point and observe the character
// "lffff" set the camera mode - move to a point with a given speed and follow the character
#define MSG_CAMERA_MOVE 30513
#define MSG_CAMERA_FREE 30514 // "l" free flight of the camera

#define MSG_CAMERA_SLEEP 30520 // "ll" stop the camera (1) or resume (0)
#define MSG_CAMERA_SET_RADIUS 30521 // "lf" set radius

#define MSG_CAMERA_SET_PERSPECTIVE 30530 // "lf" set perspective

// Blots on the ship
#define MSG_BLOTS_SETMODEL 30600 // "li" set the model, model_id
#define MSG_BLOTS_HIT 30601      // "lffffff" set point x, y, z, nx, ny, nz

//============================================================================================
// World map messages
//============================================================================================
// Storm
#define MSG_WORLDMAP_CREATESTORM 31000      // "l" Create storm
#define MSG_WORLDMAP_CREATESTORM_TIME 31001 // "lf" Create storm and specify lifetime in seconds
// Encounter ships
// Speed coefficient kSpeed determines the speed of the encounter relative to the player, 1.0 are the same, 1.2 is 20%
// higher, 1.0 / 1.2 20% lower Merchant - event ship with merchant behavior (following to the island)
#define MSG_WORLDMAP_CREATEENC_MER 31100 // "lsssff" type, nation, kSpeed (1.0f - player speed), island name  time
// boal 04/10/06 new method
#define MSG_WORLDMAP_CREATEENC_MER_XZ 31102 // "lsffffff"  sName, fx1, fz1, fx2, fz2, kSpeed, timeOut
// бутафория, не юзается // "lllsff" type, nation, kSpeed (1.0f - player speed), island name, live time in sec
#define MSG_WORLDMAP_CREATEENC_MER_TIME 31101
// Follow - event ship following us
#define MSG_WORLDMAP_CREATEENC_FLW 31110      // "lllf" type, nation, kSpeed
#define MSG_WORLDMAP_CREATEENC_FLW_TIME 31111 // "lllff" type, nation, kSpeed, live time in sec
// Warring - a pair of warring ships
#define MSG_WORLDMAP_CREATEENC_WAR 31120      // "lllll" type1, nation1, type2, nation2
#define MSG_WORLDMAP_CREATEENC_WAR_TIME 31121 // "lllllf" type1, nation1, type2, nation2, live time in sec
// Delete all encounters
#define MSG_WORLDMAP_CREATEENC_RELEASE 31200 // ""
// Launch going out to sea from script
#define MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA 31130 // ""
// Setting a flag on the map
#define MSG_WORLDMAP_SET_NATION_FLAG 31150 // "ll" nation
// Print line with coordinates
#define MSG_WORLDMAP_SET_COORDINATES 31151 // "ls" coordinates string

// ============================================================================================
// Effects
//============================================================================================

// Fader
#define FADER_OUT 33000        //"lfl", fade time in sec, true - autodelte
#define FADER_IN 33001         //"lfl", fade time in sec, true - autodelte
#define FADER_STARTFRAME 33002 //"l", if send, then make shot from next frame & show it
#define FADER_PICTURE 33003    //"ls", texture name
#define FADER_PICTURE0 33004   //"ls", texture name

//============================================================================================
// Particle system
//============================================================================================

#define PS_CREATE 10000
#define PS_SETVECTORS 10001
#define PS_CREATEX 10002
#define PS_CREATEXX 10003
#define PS_ADDTRACKPOINT 10004
#define PS_USESURFACE 10005
#define PS_CLEARALL 10006
#define PS_DELETE 10007
#define PS_CREATE_RIC 10008
#define PS_CREATEX_RIC 10009
#define PS_PAUSEALL 10010
#define PS_CREATIONCAPTURE_BEG 10011
#define PS_CREATIONCAPTURE_END 10012
#define PS_CLEAR_CAPTURED 10013
#define PS_VALIDATE_PARTICLE 10014

#define LM_SET_GEOMETRY 1500
#define LM_LOCATE 1501
#define LM_LOCATE_FIRST 1502
#define LM_LOCATE_NEXT 1503
#define LM_LOCATE_I 1504

#define LM_SETPOINT 1600
#define LM_SETPOINTANDANGLES 1601
#define LM_SETPOINTANDVECTOR 1602

//============================================================================================
// Sound Service messages
//============================================================================================
// Control functions
#define MSG_SOUND_SET_ENABLED 77016         //"l"          enabled
#define MSG_SOUND_SET_CAMERA_POSITION 77001 //"fff"       vector_x, vector_ym, vector_z
//"ffffff"    nose_vector_x, nose_vector_ym, nose_vector_z, head_vector_x, head_vector_y, head_vector_z
#define MSG_SOUND_SET_CAMERA_ORIENTATION 77002
#define MSG_SOUND_SET_MASTER_VOLUME 77021 //"fff"       FX, music, speech volumes
#define MSG_SOUND_GET_MASTER_VOLUME 77022 //"eee"       FX, music, speech volumes
#define MSG_SOUND_SET_PITCH 77031 // "lf" set pitch
#define MSG_SOUND_GET_PITCH 77032 // "le" get pitch

// Sound management routines
//"sl[llllfff]" file_name, type, simple_cache?, looped?, cached?, fade_in_time, start_x, start_y, start_y
#define MSG_SOUND_PLAY 77003
#define MSG_SOUND_STOP 77004         //"ll"        id(0=all), fade_out_time
#define MSG_SOUND_RELEASE 77005      //"l"         id(0=all)
#define MSG_SOUND_DUPLICATE 77006    //"l"         id
#define MSG_SOUND_SET_3D_PARAM 77007 //"llp"       id(0=all), param_type, pointer_to_operand
#define MSG_SOUND_SET_VOLUME 77008   //"lf"        id(0=all), new_volume
#define MSG_SOUND_IS_PLAYING 77009   //"l"         id
#define MSG_SOUND_GET_POSITION 77010 //"l"         id
#define MSG_SOUND_RESTART 77011      //"l"         id(0=all)
#define MSG_SOUND_RESUME 77012       //"ll"         id(0=all), fade_in_time
// Sound schemes
#define MSG_SOUND_SCHEME_RESET 77013 //""
#define MSG_SOUND_SCHEME_SET 77014   //"s"         scheme_name
#define MSG_SOUND_SCHEME_ADD 77015   //"s"          scheme_name
// Aliases
#define MSG_SOUND_ALIAS_ADD 77017 //"s"          alias_name
//============================================================================================

//============================================================================================
// Animals messages
//============================================================================================
#define MSG_ANIMALS_SEAGULLS_SHOW 77100     //""
#define MSG_ANIMALS_SEAGULLS_HIDE 77101     //""
#define MSG_ANIMALS_SEAGULLS_FRIGHTEN 77102 //""
#define MSG_ANIMALS_SEAGULLS_ADD 77103      //"fff"         x,y,z
#define MSG_ANIMALS_SHARKS_SHOW 77104       //""
#define MSG_ANIMALS_SHARKS_HIDE 77105       //""
#define MSG_ANIMALS_FISHSCHOOLS_SHOW 77106  //""
#define MSG_ANIMALS_FISHSCHOOLS_HIDE 77107  //""
#define MSG_ANIMALS_BUTTERFLIES_SHOW 77108  //""
#define MSG_ANIMALS_BUTTERFLIES_HIDE 77109  //""
#define MSG_ANIMALS_BUTTERFLIES_XYZ 77110   //""
//============================================================================================
//============================================================================================
// Ball splash messages
//============================================================================================
#define MSG_BALLSPLASH_ADD 77200 //"ffffff"    position, ball_vector
//============================================================================================

//============================================================================================
// Telescope messages
//============================================================================================
#define MSG_TELESCOPE_INIT_ARRAYS 77300 //"ee"
#define MSG_TELESCOPE_REQUEST 77301
#define MSG_TELESCOPE_SET_TYPE 77302 //"l", telescope_type
//============================================================================================

//============================================================================================
// Sea operator messages
//============================================================================================
#define MSG_SEA_OPERATOR_FIRE 77400        //"isfff"        shipID, bortName, x,y,z
#define MSG_SEA_OPERATOR_BALL_UPDATE 77401 //...fff...
#define MSG_SEA_OPERATOR_SHIP_HIT 77402    // fffi        ballX,ballY,ballZ, hitShipCharacter
//============================================================================================

//============================================================================================
// Peoples on ship
//============================================================================================
#define MSG_PEOPLES_ON_SHIP_MASTFALL 77500 //"ll"        characterIndex, mastNumber
#define MSG_PEOPLES_ON_SHIP_HULLHIT 77600  //"lafff"
//============================================================================================

//============================================================================================
// Camera on ship
//============================================================================================
#define MSG_DECK_CAMERA_SET_VIEWPOINT 41800 //"fff"        point.x, point.y, point.z
//============================================================================================

#endif
