
int LocationInitCommonLoc(int n)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ОБЩИЕ ЛОКАЦИИ НА ВСЕ ГОРОДА
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Дома К3
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonFlamHouse";
	locations[n].filespath.models = "locations\inside\FlamHouse";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\flamhouse.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	//locations[n].fastreload = "";
	//Models
	//Always
	locations[n].models.always.tavern = "FlamHouse";
	locations[n].models.always.tavern.level = 65538;
	locations[n].models.always.window = "FlamHouse_window";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;
	locations[n].models.day.locators = "FlamHouse_locators";
	locations[n].models.night.locators = "FlamHouse_Nlocators";

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "FlamHouse_patch";
	//Night
	locations[n].models.night.charactersPatch = "FlamHouse_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload3";
	locations[n].reload.l2.go = "CommonRoom_MH4";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";

	locations[n].reload.l3.name = "reload4";
	locations[n].reload.l3.go = "CommonRoom_MH3";
	locations[n].reload.l3.emerge = "reload1";
	locations[n].reload.l3.autoreload = "0";
	locations[n].reload.l3.label = "Room";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonCobHouse";
	locations[n].filespath.models = "locations\inside\CobHouse";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\CobHouse.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].models.always.tavern = "CobHouse";
	locations[n].models.always.tavern.level = 65538;
	locations[n].models.always.window = "CobHouse_window";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;
	locations[n].models.day.locators = "CobHouse_locators";
	locations[n].models.night.locators = "CobHouse_Nlocators";

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "CobHouse_patch";
	//Night
	locations[n].models.night.charactersPatch = "CobHouse_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonPirateresidence";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonStoneHouse";
	locations[n].filespath.models = "locations\inside\StoneHouse";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\stonehouse.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].models.always.tavern = "StoneHouse";
	locations[n].models.always.tavern.level = 65538;
	locations[n].models.always.window = "StoneHouse_window";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;
	locations[n].models.day.locators = "StoneHouse_locators";
	locations[n].models.night.locators = "StoneHouse_Nlocators";

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "StoneHouse_patch";
	//Night
	locations[n].models.night.charactersPatch = "StoneHouse_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonRoom_MH5";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonHut";
	locations[n].filespath.models = "locations\inside\Hut";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\hut.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].models.always.locators = "Hut_locators";
	locations[n].models.always.tavern = "Hut";
	locations[n].models.always.tavern.level = 65538;
	locations[n].models.always.window = "Hut_window";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "Hut_patch";
	//Night
	locations[n].models.night.charactersPatch = "Hut_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Medium Houses 0-10
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH";
	locations[n].id.label = "house";
	locations[n].image = "loading\inside\mediumhouse01.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse01";
	locations[n].models.always.mediumhouse01 = "mediumhouse01";
	locations[n].models.always.mediumhouse01.level = 65538;
	locations[n].models.day.locators = "mediumhouse01_locators";
	locations[n].models.night.locators = "mediumhouse01_Nlocators";

	Locations[n].models.always.mediumhouse01windows = "mediumhouse01_windows";
	Locations[n].models.always.mediumhouse01windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse01windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse01rand= "mediumhouse01_rand";
	locations[n].models.day.charactersPatch = "mediumhouse01_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse01_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
    //Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH2";
	locations[n].id.label = "house";
	locations[n].image = "loading\inside\mediumhouse02.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse02";
	locations[n].models.always.mediumhouse02 = "mediumhouse02";
	locations[n].models.always.mediumhouse02.level = 65538;
	locations[n].models.day.locators = "mediumhouse02_locators";
	locations[n].models.night.locators = "mediumhouse02_Nlocators";

	Locations[n].models.always.mediumhouse02windows = "mediumhouse02_windows";
	Locations[n].models.always.mediumhouse02windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse02windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse02rand= "mediumhouse02_rand";
	locations[n].models.day.charactersPatch = "mediumhouse02_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse02_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
    //Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH3";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse03";
	locations[n].models.always.mediumhouse03 = "mediumhouse03";
	locations[n].models.always.mediumhouse03.level = 65538;
	locations[n].models.day.locators = "mediumhouse03_locators";
	locations[n].models.night.locators = "mediumhouse03_Nlocators";

	Locations[n].models.always.mediumhouse03windows = "mediumhouse03_windows";
	Locations[n].models.always.mediumhouse03windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse03windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse03rand= "mediumhouse03_rand";
	locations[n].models.day.charactersPatch = "mediumhouse03_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse03_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH4";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse04";
	locations[n].models.always.mediumhouse04 = "mediumhouse04";
	locations[n].models.always.mediumhouse04.level = 65538;
	locations[n].models.day.locators = "mediumhouse04_locators";
	locations[n].models.night.locators = "mediumhouse04_Nlocators";

	Locations[n].models.always.mediumhouse04windows = "mediumhouse04_windows";
	Locations[n].models.always.mediumhouse04windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse04windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse04rand= "mediumhouse04_rand";
	locations[n].models.day.charactersPatch = "mediumhouse04_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse04_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "";
	locations[n].reload.l1.emerge = "";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH5";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse05";
	locations[n].models.always.mediumhouse05 = "mediumhouse05";
	locations[n].models.always.mediumhouse05.level = 65538;
	locations[n].models.day.locators = "mediumhouse05_locators";
	locations[n].models.night.locators = "mediumhouse05_Nlocators";

	Locations[n].models.always.mediumhouse05windows = "mediumhouse05_windows";
	Locations[n].models.always.mediumhouse05windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse05windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse05rand= "mediumhouse05_rand";
	locations[n].models.day.charactersPatch = "mediumhouse05_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse05_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH7";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse07";
	locations[n].models.always.house = "mediumhouse07";
	locations[n].models.always.house.level = 65538;
	locations[n].models.day.locators = "mediumhouse07_locators";
	locations[n].models.night.locators = "mediumhouse07_Nlocators";

	Locations[n].models.always.mediumhouse07windows = "mediumhouse07_windows";
	Locations[n].models.always.mediumhouse07windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse07windows.level = 65539;
	locations[n].models.always.tul = "mediumhouse07_tul";
	Locations[n].models.always.tul.tech = "LocationWaterFall";
	locations[n].models.always.tul.level = 180;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse07rand= "mediumhouse07_rand";
	locations[n].models.day.charactersPatch = "mediumhouse07_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse07_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH9";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse09";
	locations[n].models.always.house = "mediumhouse09";
	locations[n].models.always.house.level = 65538;
	locations[n].models.day.locators = "mediumhouse09_locators";
	locations[n].models.night.locators = "mediumhouse09_Nlocators";

	Locations[n].models.always.mediumhouse09windows = "mediumhouse09_windows";
	Locations[n].models.always.mediumhouse09windows.tech = "LocationWindows";
	locations[n].models.always.back.mediumhouse09windows = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse09rand= "mediumhouse09_rand";
	locations[n].models.day.charactersPatch = "mediumhouse09_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse09_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonRoom_MH10";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\mediumhouse10.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\mediumhouse10";
	locations[n].models.always.house = "mediumhouse10";
	locations[n].models.always.house.level = 65538;
	locations[n].models.day.locators = "mediumhouse10_locators";
	locations[n].models.night.locators = "mediumhouse10_Nlocators";

	Locations[n].models.always.mediumhouse10windows = "mediumhouse10_windows";
	Locations[n].models.always.mediumhouse10windows.tech = "LocationWindows";
	locations[n].models.always.mediumhouse10windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.mediumhouse10rand= "mediumhouse10_rand";
	locations[n].models.day.charactersPatch = "mediumhouse10_patch";
	//Night
	locations[n].models.night.charactersPatch = "mediumhouse10_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Doubleflour house and upstairs room
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonDoubleflourHouse_1"; //с комнатой из таверны на втором этаже
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\largehouse01.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\largehouse01";
	locations[n].models.always.largehouse01 = "largehouse01";
	locations[n].models.always.largehouse01.level = 65538;
	locations[n].models.day.locators = "largehouse01_locators";
	locations[n].models.night.locators = "largehouse01_Nlocators";

	Locations[n].models.always.largehouse01windows = "largehouse01_windows";
	Locations[n].models.always.largehouse01windows.tech = "LocationWindows";
	locations[n].models.always.largehouse01windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.largehouse01rand= "largehouse01_rand";
	locations[n].models.day.charactersPatch = "largehouse01_patch";
	//Night
	locations[n].models.night.charactersPatch = "largehouse01_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonDoubleflourRoom";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonDoubleflourHouse_2"; //с комнатой на втором этаже
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\largehouse01.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\largehouse01";
	locations[n].models.always.largehouse01 = "largehouse01";
	locations[n].models.always.largehouse01.level = 65538;
	locations[n].models.day.locators = "largehouse01_locators";
	locations[n].models.night.locators = "largehouse01_Nlocators";

	Locations[n].models.always.largehouse01windows = "largehouse01_windows";
	Locations[n].models.always.largehouse01windows.tech = "LocationWindows";
	locations[n].models.always.largehouse01windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.largehouse01rand= "largehouse01_rand";
	locations[n].models.day.charactersPatch = "largehouse01_patch";
	//Night
	locations[n].models.night.charactersPatch = "largehouse01_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonRoom_MH5";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";
	n = n + 1;
	
    //////////////////////////  бывшая комната наверху таверны из ПКМ.
	locations[n].id = "CommonDoubleflourRoom";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\largehouse02.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\largehouse02";
	locations[n].models.always.largehouse02 = "largehouse02";
	locations[n].models.always.largehouse02.level = 65538;
	locations[n].models.day.locators = "largehouse02_locators";
	locations[n].models.night.locators = "largehouse02_Nlocators";

	Locations[n].models.always.largehouse02windows = "largehouse02_windows";
	Locations[n].models.always.largehouse02windows.tech = "LocationWindows";
	locations[n].models.always.largehouse02windows.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.largehouse02rand= "largehouse02_rand";
	locations[n].models.day.charactersPatch = "largehouse02_patch";
	//Night
	locations[n].models.night.charactersPatch = "largehouse02_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "House";
	n = n + 1;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pirate residence  and  Pirate house
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonPirateResidence";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\smallresidence.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\SmallResidence";
	locations[n].models.always.locators = "SmallResidence_locators";
	locations[n].models.always.house = "SmallResidence";
	locations[n].models.always.house.level = 65538;
	locations[n].models.always.window = "SmallResidence_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "SmallResidence_patch";
	//Night
	locations[n].models.night.charactersPatch = "SmallResidence_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonPirateHouse";
	locations[n].id.label = "House";
	locations[n].image = "loading\inside\TwoFloorHouse.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\TwoFloorHouse";
	locations[n].models.always.house = "TwoFloorHouse";
	locations[n].models.always.house.level = 65538;
	locations[n].models.day.locators = "TwoFloorHouse_locators";
	locations[n].models.night.locators = "TwoFloorHouse_Nlocators";
	locations[n].models.always.window = "TwoFloorHouse_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	Locations[n].models.day.TwoFloorHouseRand= "TwoFloorHouse_rand";
	locations[n].models.day.charactersPatch = "TwoFloorHouse_patch";
	//Night
	locations[n].models.night.charactersPatch = "TwoFloorHouse_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	locations[n].locators_radius.item.item1 = 0.8;
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonPackhouse_1";
	locations[n].id.label = "packhouse";
	locations[n].image = "loading\inside\sklad.tga";
	locations[n].MustSetReloadBack = true;
	locations[n].packhouse = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\sklad";
	locations[n].models.always.locators = "sklad_locators";
	locations[n].models.always.modelSklad = "sklad";
	locations[n].models.always.modelSklad.level = 65538;
	locations[n].models.always.window = "sklad_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "sklad_patch";
	//Night
	locations[n].models.night.charactersPatch = "sklad_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonPackhouse_2";
	locations[n].id.label = "packhouse";
	locations[n].image = "loading\inside\sklad.tga";
	locations[n].MustSetReloadBack = true;
	locations[n].packhouse = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\sklad";
	locations[n].models.always.locators = "sklad_locators";
	locations[n].models.always.modelSklad = "sklad";
	locations[n].models.always.modelSklad.level = 65538;
	locations[n].models.always.window = "sklad_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "sklad_patch";
	//Night
	locations[n].models.night.charactersPatch = "sklad_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Residence 1-5 and Back Hall
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_1";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouse01.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\BigHouse01";
	locations[n].models.always.model = "BigHouse01";
	locations[n].models.always.model.level = 65538;
	locations[n].models.day.locators = "BigHouse01_locators";
	locations[n].models.night.locators = "BigHouse01_Nlocators";
	locations[n].models.always.window = "BigHouse01_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "BigHouse01_patch";
	//Night
	locations[n].models.night.charactersPatch = "BigHouse01_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	locations[n].locators_radius.reload.locator1 = 0.8;
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_2"; // c переходом в спальню
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouse02.tga";
	locations[n].MustSetReloadBack = true;
 	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\BigHouse02\";
	locations[n].models.always.models = "BigHouse02";
	locations[n].models.always.models.level = 65538;
	locations[n].models.always.boxes = "BigHouse02_boxes";
	locations[n].models.always.boxes.level = 65539;
	locations[n].models.always.window = "BigHouse02_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65540;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "BigHouse02_patch";
	locations[n].models.day.locators = "BigHouse02_locators";
	//Night
	locations[n].models.night.charactersPatch = "BigHouse02_patch";
	locations[n].models.night.locators = "BigHouse02_Nlocators";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Room";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonBedroom";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Bedroom";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_3"; // c переходом в спальню
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouse03.tga";
	locations[n].MustSetReloadBack = true;
 	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\BigHouse03\";
	locations[n].models.always.house = "BigHouse03";
	locations[n].models.always.house.level = 65538;
	locations[n].models.always.window = "BigHouse03_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "BigHouse03_patch";
	locations[n].models.day.locators = "BigHouse03_locators";
	//Night
	locations[n].models.night.charactersPatch = "BigHouse03_patch";
	locations[n].models.night.locators = "BigHouse03_Nlocators";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Room";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonBedroom";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Bedroom";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_4";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouse04.tga";
	locations[n].MustSetReloadBack = true;
	locations[n].PlayHall = true; //различаем типы локаций в резиденции, надо т.к. id.label затирается
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Always
	locations[n].filespath.models = "locations\Inside\BigHouse04";
	locations[n].models.always.house = "BigHouse04";
	locations[n].models.always.house.level = 65538;
	locations[n].models.always.window = "BigHouse04_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "BigHouse04_patch";
	locations[n].models.day.locators = "BigHouse04_locators";
	//Night
	locations[n].models.night.charactersPatch = "BigHouse04_patch";
	locations[n].models.night.locators = "BigHouse04_Nlocators";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_5";
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouse05.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Always
	locations[n].filespath.models = "locations\Inside\BigHouse05";
	locations[n].models.always.house = "BigHouse05";
	locations[n].models.always.window = "BigHouse05_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 50;

	locations[n].models.always.back = "..\inside_back";
	//Day
	locations[n].models.day.charactersPatch = "BigHouse05_patch";
	locations[n].models.day.locators = "BigHouse05_locators";
	//Night
	locations[n].models.night.charactersPatch = "BigHouse05_patch";
	locations[n].models.night.locators = "BigHouse05_Nlocators";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonResidence_BackHall";  //со спальней на втором этаже и пиратской резиденц. на первом
	locations[n].id.label = "Room";
	locations[n].image = "loading\inside\BigHouseBack.tga";
	locations[n].MustSetReloadBack = true;
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\BigHouseBack";
	locations[n].models.always.house = "BigHouseBack";
	locations[n].models.always.house.level = 65538;
	locations[n].models.always.window = "BigHouseBack_windows";
	locations[n].models.always.window.tech = "LocationWindows";
	locations[n].models.always.window.level = 65539;

	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.charactersPatch = "BigHouseBack_patch";
	locations[n].models.day.locators = "BigHouseBack_locators";
	//Night
	locations[n].models.night.charactersPatch = "BigHouseBack_patch";
	locations[n].models.night.locators = "BigHouseBack_Nlocators";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "";

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "CommonBedroom";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	locations[n].reload.l2.label = "Room";

	locations[n].reload.l3.name = "reload3";
	locations[n].reload.l3.go = "CommonPirateResidence";
	locations[n].reload.l3.emerge = "reload1";
	locations[n].reload.l3.autoreload = "0";
	locations[n].reload.l3.label = "Room";
	n = n + 1;

 	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bedroom
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "CommonBedroom";
	locations[n].id.label = "Bedroom";
	locations[n].image = "loading\inside\Bedroom.tga";
	locations[n].MustSetReloadBack = true;
	locations[n].Bedroom = true; //различаем типы локаций в резиденции, надо т.к. id.label затирается
	//Town sack
	locations[n].townsack = "";
	locations[n].lockWeather = "Inside";
	//Sound
	locations[n].type = "house";
	locations[n].fastreload = "";
	//Always
	locations[n].filespath.models = "locations\inside\bedRoom1";
	locations[n].models.always.bedRoom1 = "bedRoom1";
	locations[n].models.always.bedRoom1.level = 65538;
	Locations[n].models.always.bedRoom1windows = "mediumhouse01_windows";
	Locations[n].models.always.bedRoom1windows.tech = "LocationWindows";
	locations[n].models.always.bedRoom1windows.level = 65539;
	locations[n].models.always.back = "..\inside_back";
	locations[n].models.always.back.level = 65529;
	//Day
	locations[n].models.day.locators = "bedRoom1_locators";
	Locations[n].models.day.bedRoom1rand = "bedRoom1_rand";
	locations[n].models.day.charactersPatch = "bedRoom1_patch";
	//Night
	locations[n].models.night.locators = "bedRoom1_Nlocators";
	locations[n].models.night.charactersPatch = "bedRoom1_patch";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Room";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Джунгли
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	locations[n].id = "Common_jungle_01";
	locations[n].id.label = "Jungle";
	locations[n].image = "loading\outside\jungle.tga";
	locations[n].MustSetReloadBack = true;
	//Sound
	locations[n].type = "jungle";
	locations[n].islandId = "Mein";
	//Models
	//Always
	locations[n].filespath.models = "locations\Outside\Jungles\Jungle3";
	Locations[n].models.always.jungle = "jungle3";	
	Locations[n].models.always.locators = "jungle3_lcommon";		
	Locations[n].models.always.grassPatch = "jungle3_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";
	Locations[n].models.always.l2 = "plan2";
	Locations[n].models.always.l2.level = 8;
	Locations[n].models.always.l2.tech = "LocationModelBlend";
	Locations[n].models.always.l3 = "plan3";
	Locations[n].models.always.l3.level = 7;
	Locations[n].models.always.l3.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "jungle3_patch";
	//Night
	locations[n].models.night.charactersPatch = "jungle3_patch";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "false";
	//Reload map
	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "none";
	locations[n].reload.l1.emerge = "none";
	locations[n].reload.l1.autoreload = "1";
	locations[n].reload.l1.label = "Beach";
	locations[n].locators_radius.reload.reload1 = 2.0;
	n = n + 1;

	return n;
}
