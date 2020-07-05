#define SAILS_COLOR_QUANTITY				8
#define SHIP_TYPES_QUANTITY					24
#define SHIP_TYPES_QUANTITY_WITH_FORT		27	// must be (SHIP_TYPES_QUANTITY + 1)

#define SHIP_TARTANE			0

#define SHIP_WAR_TARTANE		1

//6
#define SHIP_LUGGER				2
#define SHIP_SLOOP				3

//5
#define SHIP_SCHOONER			4
#define SHIP_BARQUE				5
#define SHIP_CARAVEL			6

//4
#define SHIP_FLEUT				7
#define SHIP_BRIG				8
#define SHIP_GALEON_L			9

//3
#define SHIP_CORVETTE			10
#define SHIP_GALEON_H			11
#define SHIP_PINNACE			12

//2
#define SHIP_FRIGATE			13

//1
#define SHIP_LINESHIP			14
#define SHIP_WARSHIP			15
#define SHIP_BATTLESHIP			16

#define SHIP_MANOWAR			17

#define SHIP_SOLEYRU			18

#define SHIP_BRIGQEEN			19
#define SHIP_BRIGSW				20
#define SHIP_XebekVML			21
#define SHIP_CORVETTE_QUEST		22
#define SHIP_ARABELLA			23

//
#define SHIP_BOAT               25
#define SHIP_FORT               26

#define SHIP_NOTUSED			 1000
#define SHIP_NOTUSED_TYPE_NAME	"Not Used"
#define SHIP_NAME_NOTUSED		"Noname"

object  ShipsTypes[SHIP_TYPES_QUANTITY_WITH_FORT];  // RealShips
object	SailsColors[SAILS_COLOR_QUANTITY];


ref GetShipByType(int iType) { return &ShipsTypes[iType]; }  // пользовать GetRealShip