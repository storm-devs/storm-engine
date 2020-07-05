
int LocationInitBoarding(int n)
{
	//// форт для абордажей, клон локаций городов
    //ID
	Locations[n].id = "BOARDING_FORT";
	locations[n].type = "fort_attack";
	Locations[n].image = "loading\Fort_512.tga";
	LAi_LocationFantomsGen(&locations[n], false);
	// тереть .reload
	Locations[n].boarding.locatorNum = 15;
	Locations[n].boarding = "false";
	Locations[n].boarding.nextdeck = "";
	n = n + 1;
	
	////////////////////////////////////////////////////////////
	/// SentJons Fort (Antigua)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "SentJons_fort";
	locations[n].id.label = "SentJons Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "SentJons";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "SentJons";
	locations[n].islandId = "Antigua";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "SentJons_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "SentJons_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "SentJons_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "SentJons";
    locations[n].parent_colony = "SentJons";
 	locations[n].islandId = "Antigua";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "SentJons_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Bridgetown Fort  (Barbados)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Bridgetown_fort";
	locations[n].id.label = "Bridgetown Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Bridgetown";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Bridgetown";
	locations[n].islandId = "Barbados";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Bridgetown_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Bridgetown_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Bridgetown_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Bridgetown";
	locations[n].parent_colony = "Bridgetown";
	locations[n].islandId = "Barbados";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Bridgetown_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Jamaica (PortRoyal)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "PortRoyal_fort";
	locations[n].id.label = "PortRoyal Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "PortRoyal";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "PortRoyal";
	locations[n].islandId = "Jamaica";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";		
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "PortRoyal_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "PortRoyal_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "PortRoyal_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "PortRoyal";
	locations[n].parent_colony = "PortRoyal";
	locations[n].islandId = "Jamaica";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "PortRoyal_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Villemstad Fort (Curacao)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Villemstad_fort";
	locations[n].id.label = "Villemstad Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Villemstad";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Villemstad";
	locations[n].islandId = "Curacao";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Villemstad_exitTown";
	locations[n].reload.l1.emerge = "reload3";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Villemstad_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Villemstad_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Villemstad";
	locations[n].parent_colony = "Villemstad";
	locations[n].islandId = "Curacao";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Villemstad_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// BasTer Fort    (Guadeloupe)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "BasTer_Fort";
	locations[n].id.label = "BasTer Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "BasTer";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "BasTer";
	locations[n].islandId = "Guadeloupe";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "BasTer_ExitTown";
	locations[n].reload.l1.emerge = "reload3";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "BasTer_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "BasTer_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "BasTer";
	locations[n].parent_colony = "BasTer";
	locations[n].islandId = "Guadeloupe";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "BasTer_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Santiago Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Santiago_fort";
	locations[n].id.label = "Santiago Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Santiago";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Santiago";
	locations[n].islandId = "Cuba";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Santiago_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Santiago_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Santiago_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Santiago";
	locations[n].parent_colony = "Santiago";
	locations[n].islandId = "Cuba";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Santiago_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// FortFrance Fort    (Martinique)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "FortFrance_fort";
	locations[n].id.label = "FortFrance Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "FortFrance";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "FortFrance";
	locations[n].islandId = "Martinique";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "FortFrance_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "FortFrance_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "FortFrance_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "FortFrance";
	locations[n].parent_colony = "FortFrance";
	locations[n].islandId = "Martinique";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "FortFrance_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Havana Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Havana_Fort";
	locations[n].id.label = "Havana Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Havana";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Havana";
	locations[n].islandId = "Cuba";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Havana_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Havana_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Havana_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Havana";
	locations[n].parent_colony = "Havana";
	locations[n].islandId = "Cuba";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Havana_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;
    
	////////////////////////////////////////////////////////////
	/// Charles Fort  (Nevis)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Charles_fort";
	locations[n].id.label = "Charles Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Charles";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Charles";
	locations[n].islandId = "Nevis";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Charles_Jungle_05";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Charles_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Charles_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Charles";
	locations[n].parent_colony = "Charles";
	locations[n].islandId = "Nevis";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Charles_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// SanJuan Fort  (PuertoRico)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "SanJuan_fort";
	locations[n].id.label = "SanJuan Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "SanJuan";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "SanJuan";
	locations[n].islandId = "PuertoRico";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";
		
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "SanJuan_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "SanJuan_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "SanJuan_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "SanJuan";
	locations[n].parent_colony = "SanJuan";
	locations[n].islandId = "PuertoRico";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "SanJuan_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// SantoDomingo Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "SantoDomingo_Fort";
	locations[n].id.label = "SantoDomingo Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "SantoDomingo";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "SantoDomingo";
	locations[n].islandId = "Hispaniola";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";
		
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "SantoDomingo_ExitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "SantoDomingo_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "SantoDomingo_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "SantoDomingo";
	locations[n].parent_colony = "SantoDomingo";
	locations[n].islandId = "Hispaniola";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "SantoDomingo_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Panama Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Panama_fort";
	locations[n].id.label = "Panama Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Panama";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Panama";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Panama_exitTown";
	locations[n].reload.l1.emerge = "reload3";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Panama_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Panama_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Panama";
	locations[n].parent_colony = "Panama";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Panama_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	locations[n].locators_radius.item.item1 = 0.8;
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// PortoBello Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "PortoBello_fort";
	locations[n].id.label = "PortoBello Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "PortoBello";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "PortoBello";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "PortoBello_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "PortoBello_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "PortoBello_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "PortoBello";
	locations[n].parent_colony = "PortoBello";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "PortoBello_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Cartahena Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Cartahena_fort";
	locations[n].id.label = "Cartahena Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Cartahena";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Cartahena";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Cartahena_exitTown";
	locations[n].reload.l1.emerge = "reload3";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Cartahena_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Cartahena_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Cartahena";
	locations[n].parent_colony = "Cartahena";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Cartahena_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// PortSpein Fort (Trinidad)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "PortSpein_Fort";
	locations[n].id.label = "PortSpein Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "PortSpein";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "PortSpein";
	locations[n].islandId = "Trinidad";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";
		
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "PortSpein_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "PortSpein_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "PortSpein_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "PortSpein";
	locations[n].parent_colony = "PortSpein";
	locations[n].islandId = "Trinidad";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "PortSpein_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Marigo Fort   (SentMartin)
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Marigo_fort";
	locations[n].id.label = "Marigo Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Marigo";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Marigo";
	locations[n].islandId = "SentMartin";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Marigo_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Marigo_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Marigo_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Marigo";
	locations[n].parent_colony = "Marigo";
	locations[n].islandId = "SentMartin";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Marigo_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Tortuga Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Tortuga_Fort";
	locations[n].id.label = "Tortuga Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Tortuga";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Tortuga";
	locations[n].islandId = "Tortuga";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Models
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortV";
	Locations[n].models.always.fortV = "fortV";	
	Locations[n].models.always.fortVseabed = "fortV_sb";
	Locations[n].models.always.locators = "fortV_locators";
		
	Locations[n].models.always.grassPatch = "fortV_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortV_patch";
	Locations[n].models.day.fortVFonarsDay = "fortV_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortV_patch";
	Locations[n].models.night.fortVBigNight= "fortV_fn";		
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Tortuga_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Tortuga_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Tortuga_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Tortuga";
	locations[n].parent_colony = "Tortuga";
	locations[n].islandId = "Tortuga";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Tortuga_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Maracaibo Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Maracaibo_Fort";
	locations[n].id.label = "Maracaibo Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Maracaibo";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Maracaibo";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Maracaibo_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Maracaibo_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Maracaibo_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Maracaibo";
	locations[n].parent_colony = "Maracaibo";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Maracaibo_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Caracas Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Caracas_Fort";
	locations[n].id.label = "Caracas Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Caracas";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Caracas";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Caracas_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Caracas_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Caracas_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Caracas";
	locations[n].parent_colony = "Caracas";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Caracas_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Cumana Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Cumana_Fort";
	locations[n].id.label = "Cumana Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Cumana";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Cumana";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Cumana_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Cumana_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Cumana_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Cumana";
	locations[n].parent_colony = "Cumana";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Cumana_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// SantaCatalina Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "SantaCatalina_Fort";
	locations[n].id.label = "SantaCatalina Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "SantaCatalina";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "SantaCatalina";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "SantaCatalina_exitTown";
	locations[n].reload.l1.emerge = "reload1";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "SantaCatalina_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "SantaCatalina_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "SantaCatalina";
	locations[n].parent_colony = "SantaCatalina";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "SantaCatalina_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Beliz Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "Beliz_Fort";
	locations[n].id.label = "Beliz Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "Beliz";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "Beliz";
	locations[n].islandId = "Mein";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "Beliz_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "Beliz_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "Beliz_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "Beliz";
	locations[n].parent_colony = "Beliz";
	locations[n].islandId = "Mein";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "Beliz_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	////////////////////////////////////////////////////////////
	/// Beliz Fort
	////////////////////////////////////////////////////////////
	//ID
	Locations[n].id = "PortPax_Fort";
	locations[n].id.label = "PortPax Fort";
	//Info
	Locations[n].image = "loading\outside\Fort.tga";
	locations[n].townsack = "PortPax";
	//Sound
	locations[n].type = "fort";
	locations[n].parent_colony = "PortPax";
	locations[n].islandId = "Hispaniola";
	locations[n].soldiers = true;
	LAi_LocationFantomsGen(&locations[n], true);
	//Always
	Locations[n].filespath.models = "locations\Fort_inside\FortVRight";
	Locations[n].models.always.fortV = "fortVRight";
	Locations[n].models.always.fortVseabed = "fortVRight_sb";
	Locations[n].models.always.locators = "fortVRight_locators";
		
	Locations[n].models.always.grassPatch = "fortVRight_grass";
	Locations[n].models.always.grassPatch.texture = "grass\grassshore.tga.tx";
	
	Locations[n].models.always.l1 = "plan1";
	Locations[n].models.always.l1.level = 9;
	Locations[n].models.always.l1.tech = "LocationModelBlend";	
	//Day
	locations[n].models.day.charactersPatch = "fortVRight_patch";
	Locations[n].models.day.fortVFonarsDay = "fortVRight_fd";
	//Night
	locations[n].models.night.charactersPatch = "fortVRight_patch";
	Locations[n].models.night.fortVBigNight= "fortVRight_fn";			
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	locations[n].environment.weather.rain = false;
    //Reload map
	locations[n].reload.l1.name = "reload1_back";
	locations[n].reload.l1.go = "PortPax_exitTown";
	locations[n].reload.l1.emerge = "reload2";
	locations[n].reload.l1.autoreload = "1";
	locations[n].locators_radius.reload.reload1_back = 2.0;

	locations[n].reload.l2.name = "reload2";
	locations[n].reload.l2.go = "PortPax_prison";
	locations[n].reload.l2.emerge = "reload1";
	locations[n].reload.l2.autoreload = "0";
	n = n + 1;
	
	////////////////////// prison ////////////////////////////
	Locations[n].id = "PortPax_prison";
	locations[n].id.label = "Prison";
	Locations[n].image = "loading\inside\prison.tga";
	locations[n].townsack = "PortPax";
	locations[n].parent_colony = "PortPax";
	locations[n].islandId = "Hispaniola";
	//Sound
	locations[n].type = "jail";
	//Models
	//Always
	locations[n].filespath.models = "locations\inside\prison1";
	locations[n].models.always.prison1 = "prison1";
	locations[n].models.always.lamps = "prison1_lamps";
	locations[n].models.always.locators = "prison1_locators";	
	//Day
	locations[n].models.day.charactersPatch = "prison1_patch";
	//Night
	locations[n].models.night.charactersPatch = "prison1_patch";
	//Environment
	locations[n].environment.weather = "true";
	Locations[n].lockWeather = "Inside";
	Locations[n].QuestlockWeather = "23 Hour";
	locations[n].environment.sea = "false";
	locations[n].environment.weather.rain = false;
    //Reload map
	Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "PortPax_fort";
	Locations[n].reload.l1.emerge = "reload2";
	Locations[n].reload.l1.autoreload = "0";
	n = n + 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// КАЮТА КАПИТАНА
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// каюты для абордажа, отличие от наших - сундуки генерятся
	// -------------------------------------------------
    Locations[n].id = "Cabin";   // каюта большая
    locations[n].id.label = "cabine";
	locations[n].filespath.models = "locations\inside\cabin01\";
	locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "boarding_cabine";
	//Models
	//Always
	locations[n].models.always.l1 = "cabin01";
	locations[n].models.always.locators = "cabin01_locators";

	locations[n].models.always.window = "cabin01_fonar";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65532;

	//Day
	locations[n].models.day.charactersPatch = "cabin01_patch";

	//Night
	locations[n].models.night.charactersPatch = "cabin01_patch";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].boarding.nextdeck = "";
	Locations[n].camshuttle = 1;
	Locations[n].boarding.locatorNum = 1;
	Locations[n].CabinType = true;
	locations[n].environment.weather.rain = false;
	Locations[n].boarding.Loc.Hero = "loc0";
    Locations[n].boarding.Loc.Capt = "aloc2";
	n = n + 1;
	
    ////////////////////////////////////
    Locations[n].id = "Cabin_Huge";   // совсем большая
    locations[n].id.label = "cabine2";
	locations[n].filespath.models = "locations\inside\cabin02\";
	locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "boarding_cabine";
	//Models
	//Always
	locations[n].models.always.l1 = "cabin02";
	locations[n].models.always.locators = "cabin02_locators";
	locations[n].models.always.window = "cabin02_fonar";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65532;

	//Day
	locations[n].models.day.charactersPatch = "cabin02_patch";

	//Night
	locations[n].models.night.charactersPatch = "cabin02_patch";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].boarding.nextdeck = "";
	Locations[n].camshuttle = 1;
	Locations[n].boarding.locatorNum = 1;
	Locations[n].CabinType = true;
	locations[n].environment.weather.rain = false;
	Locations[n].boarding.Loc.Hero = "loc0";
    Locations[n].boarding.Loc.Capt = "aloc1";
	n = n + 1;
	
	Locations[n].id = "Cabin_Small";
	Locations[n].id.label = "cabine";
	Locations[n].filespath.models = "locations\decks\capsm";
    Locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "boarding_cabine";
	//Models
	//Always
	Locations[n].models.always.locators = "capsm_l";
	Locations[n].models.always.l1 = "capsm";

	//Locations[n].models.always.window = "capsm_w";
	//Locations[n].models.always.window.tech = "LocationWindows";
	//Locations[n].models.always.window.level = 65531;

	//Day
	Locations[n].models.day.charactersPatch = "capsm_p";
	Locations[n].models.day.fonar = "capsm_fd";

	//Night
	Locations[n].models.night.charactersPatch = "capsm_p";
	Locations[n].models.night.fonar = "capsm_fn";

	//Environment
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather = "true";

	Locations[n].locators_radius.box.box2 = 1.1;

    Locations[n].locators_radius.randitem.randitem1 = 0.7;
    Locations[n].locators_radius.randitem.randitem2 = 0.7;

    Locations[n].locators_radius.rld.loc0 = 0.5;
    Locations[n].locators_radius.rld.loc1 = 0.5;
    Locations[n].locators_radius.rld.aloc0 = 0.5;
    Locations[n].locators_radius.rld.aloc1 = 0.5;

	Locations[n].boarding = "true";
	Locations[n].boarding.nextdeck = "";
	Locations[n].camshuttle = 1;
	Locations[n].boarding.locatorNum = 1;
    Locations[n].CabinType = true;
    locations[n].environment.weather.rain = false;
    Locations[n].boarding.Loc.Hero = "aloc0";
    Locations[n].boarding.Loc.Capt = "aloc1";
	n = n + 1;

	Locations[n].id = "Cabin_Medium";
	Locations[n].id.label = "cabine";
	Locations[n].filespath.models = "locations\decks\capmd";
    Locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "boarding_cabine";
	//Models
	//Always
	Locations[n].models.always.locators = "capmd_l";
	Locations[n].models.always.l1 = "capmd";

	Locations[n].models.always.window = "capmd_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65531;

	//Day
	Locations[n].models.day.charactersPatch = "capmd_pd";
	Locations[n].models.day.fonar = "capmd_fd";

	//Night
	Locations[n].models.night.charactersPatch = "capmd_pn";
	Locations[n].models.night.fonar = "capmd_fn";

	//Environment
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather = "true";

    Locations[n].locators_radius.randitem.randitem1 = 1;
    Locations[n].locators_radius.randitem.randitem2 = 1;

    Locations[n].locators_radius.rld.loc0 = 0.5;
    Locations[n].locators_radius.rld.loc1 = 0.5;
    Locations[n].locators_radius.rld.loc2 = 0.5;
    Locations[n].locators_radius.rld.aloc0 = 0.5;
    Locations[n].locators_radius.rld.aloc1 = 0.5;
    Locations[n].locators_radius.rld.aloc2 = 0.5;

	Locations[n].boarding = "true";
	Locations[n].boarding.nextdeck = "";
	Locations[n].camshuttle = 1;
	Locations[n].boarding.locatorNum = 1;
	Locations[n].CabinType = true;
	locations[n].environment.weather.rain = false;
	Locations[n].boarding.Loc.Hero = "loc2";
    Locations[n].boarding.Loc.Capt = "loc0";

	n = n + 1;
	/////////////////////////////////////////////
	//  каюты для ГГ
    // -------------------------------------------------
    Locations[n].id = "My_Cabin";   // наша каюта
    locations[n].id.label = "cabine";
	locations[n].filespath.models = "locations\inside\cabin01\";
	locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	locations[n].models.always.l1 = "cabin01";
	locations[n].models.always.locators = "cabin01_locators";

	locations[n].models.always.window = "cabin01_fonar";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65532;

	//Day
	locations[n].models.day.charactersPatch = "cabin01_patch";

	//Night
	locations[n].models.night.charactersPatch = "cabin01_patch";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;
	
	locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box3 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box4 = Items_MakeTime(0, 1, 1, 2003);

    LAi_LocationFightDisable(&Locations[n], true);
	Locations[n].locators_radius.randitem.randitem1 = 1;
    Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "My_Deck";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.label = "Deck.";
	Locations[n].items.randitem1 = "";
	locations[n].environment.weather.rain = false;
	n = n + 1;
	
	
	Locations[n].id = "My_Cabin_Huge";   // совсем большая
    locations[n].id.label = "cabine2";
	locations[n].filespath.models = "locations\inside\cabin02\";
	locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	locations[n].models.always.l1 = "cabin02";
	locations[n].models.always.locators = "cabin02_locators";
	locations[n].models.always.window = "cabin02_fonar";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65532;

	//Day
	locations[n].models.day.charactersPatch = "cabin02_patch";

	//Night
	locations[n].models.night.charactersPatch = "cabin02_patch";

	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box3 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box4 = Items_MakeTime(0, 1, 1, 2003);

    LAi_LocationFightDisable(&Locations[n], true);
	Locations[n].locators_radius.randitem.randitem1 = 1;
    Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "My_Deck";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.label = "Deck.";
	Locations[n].items.randitem1 = "";
	locations[n].environment.weather.rain = false;
	n = n + 1;

	Locations[n].id = "My_Cabin_Small";
	Locations[n].id.label = "cabine";
	Locations[n].filespath.models = "locations\decks\capsm";
    Locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	Locations[n].models.always.locators = "capsm_l";
	Locations[n].models.always.l1 = "capsm";

	//Locations[n].models.always.window = "capsm_w";
	//Locations[n].models.always.window.tech = "LocationWindows";
	//Locations[n].models.always.window.level = 65531;

	//Day
	Locations[n].models.day.charactersPatch = "capsm_p";
	Locations[n].models.day.fonar = "capsm_fd";

	//Night
	Locations[n].models.night.charactersPatch = "capsm_p";
	Locations[n].models.night.fonar = "capsm_fn";

	//Environment
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather = "true";

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);

	Locations[n].locators_radius.box.box2 = 1.1;

    Locations[n].locators_radius.randitem.randitem1 = 0.7;
    Locations[n].locators_radius.randitem.randitem2 = 0.7;

    Locations[n].locators_radius.rld.loc0 = 0.5;
    Locations[n].locators_radius.rld.loc1 = 0.5;
    Locations[n].locators_radius.rld.aloc0 = 0.5;
    Locations[n].locators_radius.rld.aloc1 = 0.5;

    LAi_LocationFightDisable(&Locations[n], true);

    Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "My_Deck";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.label = "Deck.";

	Locations[n].items.randitem2 = "";
	Locations[n].items.randitem1 = "mineral4";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;
	locations[n].environment.weather.rain = false;

	n = n + 1;

	Locations[n].id = "My_Cabin_Medium";
	Locations[n].id.label = "cabine";
	Locations[n].filespath.models = "locations\decks\capmd";
    Locations[n].image = "loading\capdeck.tga";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	Locations[n].models.always.locators = "capmd_l";
	Locations[n].models.always.l1 = "capmd";

	Locations[n].models.always.window = "capmd_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65531;

	//Day
	Locations[n].models.day.charactersPatch = "capmd_pd";
	Locations[n].models.day.fonar = "capmd_fd";

	//Night
	Locations[n].models.night.charactersPatch = "capmd_pn";
	Locations[n].models.night.fonar = "capmd_fn";

	//Environment
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather = "true";

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2013);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2013);

    Locations[n].locators_radius.randitem.randitem1 = 1;
    Locations[n].locators_radius.randitem.randitem2 = 1;

    Locations[n].locators_radius.rld.loc0 = 0.5;
    Locations[n].locators_radius.rld.loc1 = 0.5;
    Locations[n].locators_radius.rld.loc2 = 0.5;
    Locations[n].locators_radius.rld.aloc0 = 0.5;
    Locations[n].locators_radius.rld.aloc1 = 0.5;
    Locations[n].locators_radius.rld.aloc2 = 0.5;

    LAi_LocationFightDisable(&Locations[n], true);

    Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "My_Deck";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.label = "Deck.";

	Locations[n].items.randitem2 = "";
	Locations[n].items.randitem1 = "";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;
	locations[n].environment.weather.rain = false;

	n = n + 1;

	Locations[n].id = "My_Campus";
	Locations[n].id.label = "My_Campus";
	Locations[n].filespath.models = "locations\decks\camp_deck";
    Locations[n].image = "loading\campus.tga";
	//Sound
	locations[n].type = "residence";
	//Models
	//Always
	Locations[n].models.always.locators = "camp_deck_l";
	Locations[n].models.always.l1 = "camp_deck";

	Locations[n].models.always.window = "camp_deck_w";
	Locations[n].models.always.window.tech = "LocationWindows";
	Locations[n].models.always.window.level = 65531;

	//Day
	Locations[n].models.day.charactersPatch = "camp_deck_pd";
	Locations[n].models.day.fonar = "camp_deck_fd";

	//Night
	Locations[n].models.night.charactersPatch = "camp_deck_pn";
	Locations[n].models.night.fonar = "camp_deck_fn";

	//Environment
	Locations[n].environment.sea = "true";
	Locations[n].environment.weather = "true";

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2013);

    Locations[n].locators_radius.rld.loc0 = 0.5;
    Locations[n].locators_radius.rld.loc1 = 0.5;
    Locations[n].locators_radius.rld.loc2 = 0.5;
    Locations[n].locators_radius.rld.loc3 = 0.5;
    Locations[n].locators_radius.rld.aloc0 = 0.5;
    Locations[n].locators_radius.rld.aloc1 = 0.5;
    Locations[n].locators_radius.rld.aloc2 = 0.5;
    Locations[n].locators_radius.rld.aloc3 = 0.5;

    LAi_LocationFightDisable(&Locations[n], true);

    Locations[n].reload.l1.name = "reload2";
	Locations[n].reload.l1.go = "My_Deck_Medium";
	Locations[n].reload.l1.emerge = "reload_camp";
	Locations[n].reload.l1.label = "Deck.";

	Locations[n].items.randitem2 = "";
	Locations[n].items.randitem1 = "";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;
	locations[n].environment.weather.rain = false;

	n = n + 1;

	//ID
	Locations[n].id = "My_Deck_Medium";
	Locations[n].id.label = "My_Deck_Medium";
	//Info
	Locations[n].filespath.models = "locations\decks\deck2"; //7
	Locations[n].image = "loading\load_rat.tga";
	//Sound
	Locations[n].type = "residence";

	//Models
	//Always
	Locations[n].models.always.locators = "deck2_l";
	Locations[n].models.always.l1 = "deck2";
	//Day
	Locations[n].models.day.charactersPatch = "deck2_p";
	Locations[n].models.day.fonar = "deck2_fd";
	//Night
	Locations[n].models.night.charactersPatch = "deck2_p";
	Locations[n].models.night.fonar = "deck2_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "true";

	Locations[n].boarding = "true";

    locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);
    locations[n].box3 = Items_MakeTime(0, 1, 1, 2003);
    locations[n].box4 = Items_MakeTime(0, 1, 1, 2003);

    LAi_LocationFightDisable(&Locations[n], true);

    Locations[n].reload.l1.name = "reload_cabin";
	Locations[n].reload.l1.go = "My_Cabin";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "Cabin.";

	Locations[n].reload.l2.name = "reload_camp";
	Locations[n].reload.l2.go = "My_Campus";
	Locations[n].reload.l2.emerge = "reload2";
	Locations[n].reload.l2.autoreload = "0";
	Locations[n].reload.l2.label = "My_Campus.";

	Locations[n].reload.l3.name = "reload_hold";
	Locations[n].reload.l3.go = "My_Deck";
	Locations[n].reload.l3.emerge = "reload1";
	Locations[n].reload.l3.autoreload = "0";
	Locations[n].reload.l3.label = "My_Deck.";
	Locations[n].camshuttle = 1;
	locations[n].environment.weather.rain = false;
	n = n + 1;

    // boal 20.03.2004 -->
    //ID
	Locations[n].id = "My_Deck";    // Наш трюм
	Locations[n].id.label = "My_Deck";
	//Info
	Locations[n].filespath.models = "locations\decks\hold";
	Locations[n].image = "loading\load_rat.tga";
	//Sound
	Locations[n].type = "residence";

	//Models
	//Always
	Locations[n].models.always.locators = "hold_l";
	Locations[n].models.always.l1 = "hold";
	//Day
	Locations[n].models.day.charactersPatch = "hold_p";
	Locations[n].models.day.fonar = "hold_fd";
	//Night
	Locations[n].models.night.charactersPatch = "hold_p";
	Locations[n].models.night.fonar = "hold_fn";
	//Environment
	Locations[n].environment.weather = "true";
	Locations[n].environment.sea = "false";

	locations[n].box1 = Items_MakeTime(0, 1, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 1, 1, 2003);
    locations[n].box3 = Items_MakeTime(0, 1, 1, 2003);
    locations[n].box4 = Items_MakeTime(0, 1, 1, 2003);

    LAi_LocationFightDisable(&Locations[n], true);

    Locations[n].reload.l1.name = "reload1";
	Locations[n].reload.l1.go = "My_Cabin";
	Locations[n].reload.l1.emerge = "reload1";
	Locations[n].reload.l1.autoreload = "0";
	Locations[n].reload.l1.label = "Cabin.";

	Locations[n].boarding = "true";
	Locations[n].camshuttle = 1;
	locations[n].environment.weather.rain = false;

    // boal 20.03.2004 <--
	n = n + 1;

    ////////////////////////////////////////////////////////////
	/// Корабельная палуба  для море - море
	////////////////////////////////////////////////////////////
    //ID
	Locations[n].id = "Deck_Near_Ship_Medium";   // выслать туда шлюпку
	Locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].image = "loading\sea.tga";
	//Sound
	Locations[n].type = "residence";
	Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models	
	//Always
	Locations[n].filespath.models = "locations\decks\deckMedium";
	Locations[n].models.always.deckMedium = "deckMedium";
	Locations[n].models.always.locators = "deckMedium_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckMedium_patch";
	Locations[n].models.day.deckMediumFonarsDay = "deckMedium_fd";	
	//Night
	locations[n].models.night.charactersPatch = "deckMedium_patch";
	Locations[n].models.night.deckMediumFonarsNight= "deckMedium_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true"; 
	//Reload map
	LAi_LocationFightDisable(&Locations[n], true);
	n = n + 1;

    //ID
	Locations[n].id = "Deck_Near_Ship_Big";   // выслать туда шлюпку
	Locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].image = "loading\sea.tga";
	//Sound
	Locations[n].type = "residence";
	Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models	
	//Always
	Locations[n].filespath.models = "locations\decks\deckBig";
	Locations[n].models.always.deckBig = "deckBig";
	Locations[n].models.always.locators = "deckBig_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckBig_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckBig_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckBig_patch";
	Locations[n].models.night.deckMediumBigNight= "deckBig_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	//Reload map
	LAi_LocationFightDisable(&Locations[n], true);
	n = n + 1;

	Locations[n].id = "Deck_Near_Ship_Low";   // выслать туда шлюпку
	Locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].image = "loading\sea.tga";
	//Sound
	Locations[n].type = "residence";
	Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models	
	//Always
	Locations[n].filespath.models = "locations\decks\deckLow";
	Locations[n].models.always.deckLow = "deckLow";
	Locations[n].models.always.locators = "deckLow_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckLow_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckLow_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckLow_patch";
	Locations[n].models.night.deckMediumBigNight= "deckLow_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";
	//Reload map
	LAi_LocationFightDisable(&Locations[n], true);
	n = n + 1;
	
	////////////////////////////////////////////////////////////
	/// Корабельная палуба  для карта - море
	////////////////////////////////////////////////////////////
	Locations[n].id = "Ship_deck_Medium";
	Locations[n].image = "loading\sea.tga";
	locations[n].id.label = "Boarding deck";
	//Sound
	Locations[n].type = "deck";
	// нельзя, иначе нет моря Locations[n].boarding = "true";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;
	//Models
	//Always
	Locations[n].filespath.models = "locations\decks\deckMedium";
	Locations[n].models.always.deckMedium = "deckMedium";
	Locations[n].models.always.locators = "deckMedium_locators";
	//Day
	locations[n].models.day.charactersPatch = "deckMedium_patch";
	Locations[n].models.day.deckMediumFonarsDay = "deckMedium_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckMedium_patch";
	Locations[n].models.night.deckMediumFonarsNight= "deckMedium_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	locations[n].box1 = Items_MakeTime(0, 0, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box3 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box4 = Items_MakeTime(0, 0, 1, 2003);
	n = n + 1;
	///////////////////////////////
	Locations[n].id = "Ship_deck_Big";
	Locations[n].image = "loading\sea.tga";
	locations[n].id.label = "Boarding deck";
	//Sound
	Locations[n].type = "deck";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;
 	//Models
	//Always
	Locations[n].filespath.models = "locations\decks\deckBig";
	Locations[n].models.always.deckBig = "deckBig";
	Locations[n].models.always.locators = "deckBig_locators";
	//Day
	locations[n].models.day.charactersPatch = "deckBig_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckBig_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckBig_patch";
	Locations[n].models.night.deckMediumBigNight= "deckBig_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	locations[n].box1 = Items_MakeTime(0, 0, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box3 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box4 = Items_MakeTime(0, 0, 1, 2003);
	n = n + 1;
	///////////////////////
	Locations[n].id = "Ship_deck_Low";
	Locations[n].image = "loading\sea.tga";
	locations[n].id.label = "Boarding deck";
	//Sound
	Locations[n].type = "deck";
	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;
 	//Models
	//Always
 	Locations[n].filespath.models = "locations\decks\deckLow";
	Locations[n].models.always.deckLow = "deckLow";
	Locations[n].models.always.locators = "deckLow_locators";
	//Day
	locations[n].models.day.charactersPatch = "deckLow_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckLow_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckLow_patch";
	Locations[n].models.night.deckMediumBigNight= "deckLow_fn";
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	locations[n].box1 = Items_MakeTime(0, 0, 1, 2003);
	locations[n].box2 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box3 = Items_MakeTime(0, 0, 1, 2003);
    locations[n].box4 = Items_MakeTime(0, 0, 1, 2003);
	n = n + 1;
	
	// палубы для всех по типам boal 03.01.05 ------------------------------------------------------->
	//ID
	Locations[n].id = "BOARDING_SMALL_DECK";
	locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].type = "deck_fight";
	Locations[n].filespath.models = "locations\decks\deckLowVSMedium";
	Locations[n].image = "loading\Boarding.tga";

	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models
	//Always
	Locations[n].models.always.deckLowVSMedium = "deckLowVSMedium";
	Locations[n].models.always.locators = "deckLowVSMedium_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckLowVSMedium_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckLowVSMedium_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckLowVSMedium_patch";
	Locations[n].models.night.deckMediumBigNight= "deckLowVSMedium_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].boarding.locatorNum = 11;
	Locations[n].boarding.nextdeck = "";
	Locations[n].UpDeckType = true;
	
	n = n + 1;

	//ID
	Locations[n].id = "BOARDING_MEDIUM_DECK";
	locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].type = "deck_fight";
	Locations[n].filespath.models = "locations\decks\deckLowVSBig";
	Locations[n].image = "loading\Boarding.tga";

	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models
	//Always
	Locations[n].models.always.deckLowVSBig = "deckLowVSBig";
	Locations[n].models.always.locators = "deckLowVSBig_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckLowVSBig_patch";
	Locations[n].models.day.deckBigFonarsDay = "deckLowVSBig_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckLowVSBig_patch";
	Locations[n].models.night.deckMediumBigNigh = "deckLowVSBig_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].boarding.locatorNum = 12;
	Locations[n].boarding.nextdeck = "";
	Locations[n].UpDeckType = true;
	
	n = n + 1;

	//ID
	Locations[n].id = "BOARDING_BIG_DECK";
	locations[n].id.label = "Boarding deck";
	//Info
	Locations[n].type = "deck_fight";
	Locations[n].filespath.models = "locations\decks\deckMediumVSBig";
	Locations[n].image = "loading\Boarding.tga";

	Locations[n].lockCamAngle = 0.4;
	Locations[n].camshuttle = 1;	
	//Models	
	//Always
	Locations[n].models.always.deckMediumVSBig = "deckMediumVSBig";
	Locations[n].models.always.locators = "deckMediumVSBig_locators";	
	//Day
	locations[n].models.day.charactersPatch = "deckMediumVSBig_patch";
	Locations[n].models.day.deckFonarsDay = "deckMediumVSBig_fd";
	//Night
	locations[n].models.night.charactersPatch = "deckMediumVSBig_patch";
	Locations[n].models.night.deckFonarsNigh = "deckMediumVSBig_fn";	
	//Environment
	locations[n].environment.weather = "true";
	locations[n].environment.sea = "true";

	Locations[n].boarding = "true";
	Locations[n].boarding.locatorNum = 13;
	Locations[n].boarding.nextdeck = "";
	Locations[n].UpDeckType = true;
	
	n = n + 1;

	////////////////////
	return n;
}