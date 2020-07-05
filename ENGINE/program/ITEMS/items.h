#define GOLD					"gold"
#define GOLD_ITEM               "jewelry5"
#define STATUE1					"Statue1"

//TEMP
#define BLADE_SABER				"blade1"
#define BLADE_SHORT				"blade8"// boal 27.03.2004
#define BLADE_NORMAL			"blade10"// boal 27.03.2004
#define BLADE_LONG				"blade16"// boal 27.03.2004
#define GUN_COMMON				"pistol1"
#define CHEAP_SPYGLASS			"spyglass1"
#define COMMON_SPYGLASS			"spyglass2"
#define GOOD_SPYGLASS			"spyglass3"
#define SUPERIOR_SPYGLASS		"spyglass4"
#define POTION					"potion1"

// items group id
#define GUN_ITEM_TYPE			"gun"
#define BLADE_ITEM_TYPE			"blade"
#define SPYGLASS_ITEM_TYPE		"spyglass"
#define PATENT_ITEM_TYPE        "patent"
#define CIRASS_ITEM_TYPE		"cirass"
#define MAPS_ITEM_TYPE		    "maps"//Добавлена новая группа

// indexes for items` array
#define ITEMS_QUANTITY			  201  // не забываем менять номер
#define MAX_BUTTONS               64
#define RANDITEMS_QUANTITY    	  45
#define BUTTON_ACTIVATION_TIME	  1000
#define ITEMS_LAYER				  50
#define MAX_LOADED_RANDITEMS      10
#define MAX_HANDLED_BOXES 		  25
#define ITEM_PROBABILITY		  0.5
#define OBJECTS_IN_BOX			  9

object Items[ITEMS_QUANTITY];
object RandItems[RANDITEMS_QUANTITY];