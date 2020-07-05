#ifndef __BATTLE_INTERFACE_MSG_CONTROL_
#define __BATTLE_INTERFACE_MSG_CONTROL_

// список активных действий
#define BI_ACTIVE_MOOR				0
#define BI_ACTIVE_SAIL_TO			1
#define BI_ACTIVE_BOARD				2
#define BI_ACTIVE_LAND_TROOPS		3
#define BI_ACTIVE_MAP				4
#define BI_ACTIVE_ATTACK			10
#define BI_ACTIVE_DEFEND			11
#define BI_ACTIVE_SAIL_AWAY			12
#define BI_ACTIVE_HEAVE_TO_DRIFT	13

// режим выбора
#define BI_COMMODE_MY_SHIP_SELECT		1
#define BI_COMMODE_ENEMY_SHIP_SELECT	2
#define BI_COMMODE_NEUTRAL_FORT_SELECT	4
#define BI_COMMODE_FRIEND_FORT_SELECT	8
#define BI_COMMODE_ENEMY_FORT_SELECT	16
#define BI_COMMODE_COMMAND_SELECT		32
#define BI_COMMODE_LAND_SELECT			64
#define BI_COMMODE_ALLLOCATOR_SELECT	128
#define BI_COMMODE_CANNON_CHARGE		256
#define BI_COMMODE_USER_ICONS			512
#define BI_COMMODE_ABILITY_ICONS		1024
#define BI_COMMODE_FRIEND_SHIP_SELECT	2048
#define BI_COMMODE_NEUTRAL_SHIP_SELECT	4096
#define BI_COMMODE_ENEMY_TOWN			8192
#define BI_COMMODE_DISEASED_TOWN		16384
#define BI_COMMODE_NOTDISEASED_TOWN		32768

#define BI_MSG_SET_ISLAND				46003 // "s" (modelName)
#define BI_MSG_NOT_ENOUGH_BALLS_FLAG	46004 // "l"
#define BI_IN_DEAD_SHIP					46009 // "l" (character index)
#define BI_IN_CREATE_SHIP				46011 // "laall" (character index,character attributes, base ship attributes, is my ship flag, relation)
#define BI_IN_DELETE_SHIP				46012 // "l" (character index)
#define BI_IN_SET_COMMAND_MODE			46013 // "lll" (command mode, command icon number, character index)
#define BI_SET_VISIBLE					46014 // "l" (visible flag)
#define BI_SET_ISLAND_LOCATORS			46002 // "la"
#define BI_MSG_REFRESH					46005 //
#define BI_MSG_SET_MSG_ICONS			46006 // "e" &aref[4]

#define BI_MSG_ADD_NEWTEXTURE			46007 // "sll" texName, horzSize, vertSize
#define BI_MSG_CARE_COMMANDLIST_UPDATE	46050 // "ls" texIdx, texName

#define BI_EVENT_SET_VISIBLE			"BI_Visible"
#define BI_EVENT_SET_SEA_STATE			"BI_SetSeaState"
#define BI_EVENT_GET_DATA				"BI_GetData"
#define BI_EVENT_CHECK_SHIPSTATE_SHOW	"BI_CheckSSShow"
#define BI_EVENT_GET_FORT_RELATION		"BI_GetFortRelation"

// типы отношений
#define BI_RELATION_ENEMY		0
#define BI_RELATION_FRIEND		1
#define BI_RELATION_NEUTRAL		2

// типы получаемых данных
#define BIDT_SHIPPICTURE		0 // "l" (characterIdx)
#define BIDT_GERALD_DATA		1 // "l" (characterIdx)

// команды "земного" интерфейса
#define MSG_BATTLE_LAND_START			46100 // ""
#define MSG_BATTLE_LAND_END				46101 // ""
#define MSG_BATTLE_LAND_SET_SHOW		46102 // "l" commandos show
#define MSG_BATTLE_LAND_SET_MSGICONS	46103 // "e" &aref[4]
#define MSG_BATTLE_LAND_SET_ICONTEX		46104 // "ls" texIdx, texName
#define MSG_BATTLE_LAND_MAKE_COMMAND	46105 // "s" command_name

// команды интерфейса подзорной трубы
#define MSG_ISG_VISIBLE			46201 // "l" (set visible interface)
#define MSG_ISG_UPDATE			46202 // "sslllfll" (ship name, ship type, hull, sail, crew, speed, cannons, charge)
#define MSG_ISG_SET_SHIPICON	46203 // "sffff" ( texture name, uv_rect{left,top,right,bottom} )
#define MSG_ISG_SET_CHARICON	46204 // "sffff" ( texture name, uv_rect{left,top,right,bottom} )

// команды интерфейса подзорной трубы
#define MSG_SP_CHANGESHIP		46301 // "l" (new character - ship owner)

// обращение к парусам
#define MSG_SAIL_SCRIPT_PROCESSING	40099	// "s..."

// обновить список активированных способностей
#define MSG_ACTIVE_PERK_LIST_REFRESH	47000

#endif
