
void wdmInitWorldMap()
{
	// Open language file for lables
	int idLngFile = LanguageOpenFile("LocLables.txt");

	//Init world map object
	//Debug info
	worldMap.debug = "false";
	worldMap.evwin = "false";
	worldMap.noenc = "false";
	
	//Start date
	worldMap.date.hour = 11;
	worldMap.date.min = 14;
	worldMap.date.sec = 42;	
	worldMap.date.day   = STARTGAME_DAY;
	worldMap.date.month = STARTGAME_MONTH;
	worldMap.date.year  = STARTGAME_YEAR;
	worldMap.date.hourPerSec = 1.5;


	//==========================================================================
	//Labels
	//=========================================================================
	//Icons description
	// 0----------->
	// 1-----------> frames
	// 2----------->
	// ...
	// icons
	//
	worldMap.icon.width = 32;
	worldMap.icon.height = 32;
	worldMap.icon.num = 8;
	worldMap.icon.frames = 8;
	worldMap.icon.fps = 45;
	worldMap.icon.texture = "icons.tga";
	//Label types description
	worldMap.labeltype.Island.icon = -1;				//Default icon
	worldMap.labeltype.Island.font = "ISLANDS_MAPS";			//Default font
	worldMap.labeltype.Island.pivotX = -0.5;			//Default label shifting left-upper point by X (1 -> full width)
	worldMap.labeltype.Island.pivotY = -0.5;			//Default label shifting left-upper point by Y (1 -> full height)
	worldMap.labeltype.Island.heightView = 1000.0;		//Default camera height when hide this labels
	worldMap.labeltype.Island.weight = 100;				//Weight is use when shake intersection labels
	worldMap.labeltype.Town.icon = -1;
	worldMap.labeltype.Town.font = "LABELS_MAPS";	
	worldMap.labeltype.Town.pivotX = -0.5;
	worldMap.labeltype.Town.pivotY = -0.5;
	worldMap.labeltype.Town.heightView = 250.0;
	worldMap.labeltype.Town.weight = 50;
	worldMap.labeltype.Shore.icon = -1;
	worldMap.labeltype.Shore.font = "LABELS_MAPS";	
	worldMap.labeltype.Shore.pivotX = -0.5;
	worldMap.labeltype.Shore.pivotY = -0.5;
	worldMap.labeltype.Shore.heightView = 200.0;
	worldMap.labeltype.Shore.weight = 1;


	//=========================================================================
	//Island BugFix
	//=========================================================================
	worldMap.labels.BugFix.type = "Island";       // без этой хрени первые метки идут левым шрифтом или не видны
	worldMap.labels.BugFix.id = "Island BugFix";
	worldMap.labels.BugFix.locator = "Barbados";//"BugFix";
	worldMap.labels.BugFix.text = "";//CheckingTranslate(idLngFile,"Antigua");

	//worldMap.labels.Antigua.icon = 0;
	//-----------------------------------------------------------
	worldMap.labels.BugFixTown.type = "Town";
	worldMap.labels.BugFixTown.id = "BugFix_town";
	worldMap.labels.BugFixTown.locator = "Barbados";//"BugFix_town";
	worldMap.labels.BugFixTown.text = "";//CheckingTranslate(idLngFile,"Antigua Port");
	//worldMap.labels.BugFixTown.icon = SPAIN;
	
	worldMap.labels.BugFixShore.type = "Shore";
	worldMap.labels.BugFixShore.id = "BugFix_Shore";
	worldMap.labels.BugFixShore.locator = "Barbados";
	worldMap.labels.BugFixShore.text = "";

	//=========================================================================
	//Island Antigua
	//=========================================================================
	worldMap.labels.Barbados.type = "Island";
	worldMap.labels.Barbados.id = "Barbados";
	worldMap.labels.Barbados.locator = "Barbados";
	worldMap.labels.Barbados.text = CheckingTranslate(idLngFile,"Barbados");
	
	worldMap.labels.Antigua.type = "Island";
	worldMap.labels.Antigua.id = "Antigua";
	worldMap.labels.Antigua.locator = "Antigua";
	worldMap.labels.Antigua.text = CheckingTranslate(idLngFile,"Antigua");
	
	worldMap.labels.Bermudes.type = "Island";
	worldMap.labels.Bermudes.id = "Bermudes";
	worldMap.labels.Bermudes.locator = "Bermudes";
	worldMap.labels.Bermudes.text = CheckingTranslate(idLngFile,"Bermudes");
	
	worldMap.labels.Beliz.type = "Island";
	worldMap.labels.Beliz.id = "Beliz";
	worldMap.labels.Beliz.locator = "Beliz";
	worldMap.labels.Beliz.text = CheckingTranslate(idLngFile,"Beliz");
	
	worldMap.labels.Cuba1.type = "Island";
	worldMap.labels.Cuba1.id = "Cuba1";
	worldMap.labels.Cuba1.locator = "Cuba1";
	worldMap.labels.Cuba1.text = CheckingTranslate(idLngFile,"Cuba1");
	
	worldMap.labels.Cuba2.type = "Island";
	worldMap.labels.Cuba2.id = "Cuba2";
	worldMap.labels.Cuba2.locator = "Cuba2";
	worldMap.labels.Cuba2.text = CheckingTranslate(idLngFile,"Cuba2");
	
	worldMap.labels.Caiman.type = "Island";
	worldMap.labels.Caiman.id = "Caiman";
	worldMap.labels.Caiman.locator = "Caiman";
	worldMap.labels.Caiman.text = CheckingTranslate(idLngFile,"Caiman");
	
	worldMap.labels.Cumana.type = "Island";
	worldMap.labels.Cumana.id = "Cumana";
	worldMap.labels.Cumana.locator = "Cumana";
	worldMap.labels.Cumana.text = CheckingTranslate(idLngFile,"Cumana");
	
	worldMap.labels.Caracas.type = "Island";
	worldMap.labels.Caracas.id = "Caracas";
	worldMap.labels.Caracas.locator = "Caracas";
	worldMap.labels.Caracas.text = CheckingTranslate(idLngFile,"Caracas");
	
	worldMap.labels.Curacao.type = "Island";
	worldMap.labels.Curacao.id = "Curacao";
	worldMap.labels.Curacao.locator = "Curacao";
	worldMap.labels.Curacao.text = CheckingTranslate(idLngFile,"Curacao");
	
	worldMap.labels.Cartahena.type = "Island";
	worldMap.labels.Cartahena.id = "Cartahena";
	worldMap.labels.Cartahena.locator = "Cartahena";
	worldMap.labels.Cartahena.text = CheckingTranslate(idLngFile,"Cartahena");
	
	worldMap.labels.Dominica.type = "Island";
	worldMap.labels.Dominica.id = "Dominica";
	worldMap.labels.Dominica.locator = "Dominica";
	worldMap.labels.Dominica.text = CheckingTranslate(idLngFile,"Dominica");
	
	worldMap.labels.Guadeloupe.type = "Island";
	worldMap.labels.Guadeloupe.id = "Guadeloupe";
	worldMap.labels.Guadeloupe.locator = "Guadeloupe";
	worldMap.labels.Guadeloupe.text = CheckingTranslate(idLngFile,"Guadeloupe");
	
	worldMap.labels.Hispaniola1.type = "Island";
	worldMap.labels.Hispaniola1.id = "Hispaniola1";
	worldMap.labels.Hispaniola1.locator = "Hispaniola1";
	worldMap.labels.Hispaniola1.text = CheckingTranslate(idLngFile,"Hispaniola1");

	worldMap.labels.Hispaniola2.type = "Island";
	worldMap.labels.Hispaniola2.id = "Hispaniola2";
	worldMap.labels.Hispaniola2.locator = "Hispaniola2";
	worldMap.labels.Hispaniola2.text = CheckingTranslate(idLngFile,"Hispaniola2");

	worldMap.labels.Jamaica.type = "Island";
	worldMap.labels.Jamaica.id = "Jamaica";
	worldMap.labels.Jamaica.locator = "Jamaica";
	worldMap.labels.Jamaica.text = CheckingTranslate(idLngFile,"Jamaica");
	
	worldMap.labels.Maracaibo.type = "Island";
	worldMap.labels.Maracaibo.id = "Maracaibo";
	worldMap.labels.Maracaibo.locator = "Maracaibo";
	worldMap.labels.Maracaibo.text = CheckingTranslate(idLngFile,"Maracaibo");
	
	worldMap.labels.Martinique.type = "Island";
	worldMap.labels.Martinique.id = "Martinique";
	worldMap.labels.Martinique.locator = "Martinique";
	worldMap.labels.Martinique.text = CheckingTranslate(idLngFile,"Martinique");
	
	worldMap.labels.SentMartin.type = "Island";
	worldMap.labels.SentMartin.id = "SentMartin";
	worldMap.labels.SentMartin.locator = "SentMartin";
	worldMap.labels.SentMartin.text = CheckingTranslate(idLngFile,"SentMartin");
	
	worldMap.labels.Nevis.type = "Island";
	worldMap.labels.Nevis.id = "Nevis";
	worldMap.labels.Nevis.locator = "Nevis";
	worldMap.labels.Nevis.text = CheckingTranslate(idLngFile,"Nevis");
	
	worldMap.labels.PuertoRico.type = "Island";
	worldMap.labels.PuertoRico.id = "PuertoRico";
	worldMap.labels.PuertoRico.locator = "PuertoRico";
	worldMap.labels.PuertoRico.text = CheckingTranslate(idLngFile,"PuertoRico");
	
	worldMap.labels.PortoBello.type = "Island";
	worldMap.labels.PortoBello.id = "PortoBello";
	worldMap.labels.PortoBello.locator = "PortoBello";
	worldMap.labels.PortoBello.text = CheckingTranslate(idLngFile,"PortoBello");
	
	worldMap.labels.Pearl.type = "Island";
	worldMap.labels.Pearl.id = "Pearl";
	worldMap.labels.Pearl.locator = "Pearl";
	worldMap.labels.Pearl.text = CheckingTranslate(idLngFile,"Pearl");
	
	worldMap.labels.Panama.type = "Island";
	worldMap.labels.Panama.id = "Panama";
	worldMap.labels.Panama.locator = "Panama";
	worldMap.labels.Panama.text = CheckingTranslate(idLngFile,"Panama");
	
	worldMap.labels.SantaCatalina.type = "Island";
	worldMap.labels.SantaCatalina.id = "SantaCatalina";
	worldMap.labels.SantaCatalina.locator = "SantaCatalina";
	worldMap.labels.SantaCatalina.text = CheckingTranslate(idLngFile,"SantaCatalina");
	
	worldMap.labels.Terks.type = "Island";
	worldMap.labels.Terks.id = "Terks";
	worldMap.labels.Terks.locator = "Terks";
	worldMap.labels.Terks.text = CheckingTranslate(idLngFile,"Terks");
	
	worldMap.labels.Tortuga.type = "Island";
	worldMap.labels.Tortuga.id = "Tortuga";
	worldMap.labels.Tortuga.locator = "Tortuga";
	worldMap.labels.Tortuga.text = CheckingTranslate(idLngFile,"Tortuga");
	
	worldMap.labels.Trinidad.type = "Island";
	worldMap.labels.Trinidad.id = "Trinidad";
	worldMap.labels.Trinidad.locator = "Trinidad";
	worldMap.labels.Trinidad.text = CheckingTranslate(idLngFile,"Trinidad");
	
	worldMap.labels.Tenotchitlan.type = "Island";
	worldMap.labels.Tenotchitlan.id = "Tenotchitlan";
	worldMap.labels.Tenotchitlan.locator = "Tenotchitlan";
	worldMap.labels.Tenotchitlan.text = CheckingTranslate(idLngFile,"Tenotchitlan");

	//-------  TOWN ----------------------------------------------------
	
	worldMap.labels.Bridgetown_town.type = "Town";
	worldMap.labels.Bridgetown_town.id = "Bridgetown_town";
	worldMap.labels.Bridgetown_town.locator = "Bridge_town";
	worldMap.labels.Bridgetown_town.text = CheckingTranslate(idLngFile,"Bridge_town");
	worldMap.labels.Bridgetown_town.icon = ENGLAND;
	
	worldMap.labels.Pirates_town.type = "Town";
	worldMap.labels.Pirates_town.id = "Pirates_town";
	worldMap.labels.Pirates_town.locator = "Pirates_town";
	worldMap.labels.Pirates_town.text = CheckingTranslate(idLngFile,"Pirates_town");
	worldMap.labels.Pirates_town.icon = PIRATE;
	
	worldMap.labels.SentJons_town.type = "Town";
	worldMap.labels.SentJons_town.id = "SentJons_town";
	worldMap.labels.SentJons_town.locator = "SentJons_town";
	worldMap.labels.SentJons_town.text = CheckingTranslate(idLngFile,"SentJons_town");
	worldMap.labels.SentJons_town.icon = ENGLAND;
	
	worldMap.labels.Beliz_town.type = "Town";
	worldMap.labels.Beliz_town.id = "Beliz_town";
	worldMap.labels.Beliz_town.locator = "Beliz_town";
	worldMap.labels.Beliz_town.text = CheckingTranslate(idLngFile,"Beliz_town");
	worldMap.labels.Beliz_town.icon = SPAIN;
	
	worldMap.labels.Santiago_town.type = "Town";
	worldMap.labels.Santiago_town.id = "Santiago_town";
	worldMap.labels.Santiago_town.locator = "Santiago_town";
	worldMap.labels.Santiago_town.text = CheckingTranslate(idLngFile,"Santiago_town");
	worldMap.labels.Santiago_town.icon = SPAIN;
	
	worldMap.labels.PuertoPrincipe_town.type = "Town";
	worldMap.labels.PuertoPrincipe_town.id = "PuertoPrincipe_town";
	worldMap.labels.PuertoPrincipe_town.locator = "PuertoPrincipe_town";
	worldMap.labels.PuertoPrincipe_town.text = CheckingTranslate(idLngFile,"PuertoPrincipe_town");
	worldMap.labels.PuertoPrincipe_town.icon = PIRATE;
	
	worldMap.labels.Havana_town.type = "Town";
	worldMap.labels.Havana_town.id = "Havana_town";
	worldMap.labels.Havana_town.locator = "Havana_town";
	worldMap.labels.Havana_town.text = CheckingTranslate(idLngFile,"Havana_town");
	worldMap.labels.Havana_town.icon = SPAIN;
	
	worldMap.labels.Cumana_town.type = "Town";
	worldMap.labels.Cumana_town.id = "Cumana_town";
	worldMap.labels.Cumana_town.locator = "Cumana_town";
	worldMap.labels.Cumana_town.text = CheckingTranslate(idLngFile,"Cumana_town");
	worldMap.labels.Cumana_town.icon = SPAIN;
	
	worldMap.labels.Caracas_town.type = "Town";
	worldMap.labels.Caracas_town.id = "Caracas_town";
	worldMap.labels.Caracas_town.locator = "Caracas_town";
	worldMap.labels.Caracas_town.text = CheckingTranslate(idLngFile,"Caracas_town");
	worldMap.labels.Caracas_town.icon = SPAIN;
	
	worldMap.labels.Havana_town.type = "Town";
	worldMap.labels.Havana_town.id = "Havana_town";
	worldMap.labels.Havana_town.locator = "Havana_town";
	worldMap.labels.Havana_town.text = CheckingTranslate(idLngFile,"Havana_town");
	worldMap.labels.Havana_town.icon = SPAIN;
	
	worldMap.labels.Villemstad_town.type = "Town";
	worldMap.labels.Villemstad_town.id = "Villemstad_town";
	worldMap.labels.Villemstad_town.locator = "Villemstad_town";
	worldMap.labels.Villemstad_town.text = CheckingTranslate(idLngFile,"Villemstad_town");
	worldMap.labels.Villemstad_town.icon = HOLLAND;
	
	worldMap.labels.Cartahena_town.type = "Town";
	worldMap.labels.Cartahena_town.id = "Cartahena_town";
	worldMap.labels.Cartahena_town.locator = "Cartahena_town";
	worldMap.labels.Cartahena_town.text = CheckingTranslate(idLngFile,"Cartahena_town");
	worldMap.labels.Cartahena_town.icon = SPAIN;
	
	worldMap.labels.BasTer_town.type = "Town";
	worldMap.labels.BasTer_town.id = "BasTer_town";
	worldMap.labels.BasTer_town.locator = "BasTer_town";
	worldMap.labels.BasTer_town.text = CheckingTranslate(idLngFile,"BasTer_town");
	worldMap.labels.BasTer_town.icon = FRANCE;
	
	worldMap.labels.SantoDomingo_town.type = "Town";
	worldMap.labels.SantoDomingo_town.id = "SantoDomingo_town";
	worldMap.labels.SantoDomingo_town.locator = "SantoDomingo_town";
	worldMap.labels.SantoDomingo_town.text = CheckingTranslate(idLngFile,"SantoDomingo_town");
	worldMap.labels.SantoDomingo_town.icon = SPAIN;
	
	worldMap.labels.LaVega_town.type = "Town";
	worldMap.labels.LaVega_town.id = "LaVega_town";
	worldMap.labels.LaVega_town.locator = "LaVega_town";
	worldMap.labels.LaVega_town.text = CheckingTranslate(idLngFile,"LaVega_town");
	worldMap.labels.LaVega_town.icon = PIRATE;
	
	worldMap.labels.PortPax_town.type = "Town";
	worldMap.labels.PortPax_town.id = "PortPax_town";
	worldMap.labels.PortPax_town.locator = "PortPax_town";
	worldMap.labels.PortPax_town.text = CheckingTranslate(idLngFile,"PortPax_town");
	worldMap.labels.PortPax_town.icon = FRANCE;
	
	worldMap.labels.Maracaibo_town.type = "Town";
	worldMap.labels.Maracaibo_town.id = "Maracaibo_town";
	worldMap.labels.Maracaibo_town.locator = "Maracaibo_town";
	worldMap.labels.Maracaibo_town.text = CheckingTranslate(idLngFile,"Maracaibo_town");
	worldMap.labels.Maracaibo_town.icon = SPAIN;
	
	worldMap.labels.PortRoyal_town.type = "Town";
	worldMap.labels.PortRoyal_town.id = "PortRoyal_town";
	worldMap.labels.PortRoyal_town.locator = "PortRoyal_town";
	worldMap.labels.PortRoyal_town.text = CheckingTranslate(idLngFile,"PortRoyal_town");
	worldMap.labels.PortRoyal_town.icon = ENGLAND;
	
	worldMap.labels.FortFrance_town.type = "Town";
	worldMap.labels.FortFrance_town.id = "FortFrance_town";
	worldMap.labels.FortFrance_town.locator = "FortFrance_town";
	worldMap.labels.FortFrance_town.text = CheckingTranslate(idLngFile,"FortFrance_town");
	worldMap.labels.FortFrance_town.icon = FRANCE;
	
	worldMap.labels.LeFransua_town.type = "Town";
	worldMap.labels.LeFransua_town.id = "LeFransua_town";
	worldMap.labels.LeFransua_town.locator = "LeFransua_town";
	worldMap.labels.LeFransua_town.text = CheckingTranslate(idLngFile,"LeFransua_town");
	worldMap.labels.LeFransua_town.icon = PIRATE;
	
	worldMap.labels.Marigo_town.type = "Town";
	worldMap.labels.Marigo_town.id = "Marigo_town";
	worldMap.labels.Marigo_town.locator = "Marigo_town";
	worldMap.labels.Marigo_town.text = CheckingTranslate(idLngFile,"Marigo_town");
	worldMap.labels.Marigo_town.icon = HOLLAND;
	
	worldMap.labels.Charles_town.type = "Town";
	worldMap.labels.Charles_town.id = "Charles_town";
	worldMap.labels.Charles_town.locator = "Charles_town";
	worldMap.labels.Charles_town.text = CheckingTranslate(idLngFile,"Charles_town");
	worldMap.labels.Charles_town.icon = ENGLAND;
	
	worldMap.labels.SanJuan_town.type = "Town";
	worldMap.labels.SanJuan_town.id = "SanJuan_town";
	worldMap.labels.SanJuan_town.locator = "SanJuan_town";
	worldMap.labels.SanJuan_town.text = CheckingTranslate(idLngFile,"SanJuan_town");
	worldMap.labels.SanJuan_town.icon = SPAIN;
	
	worldMap.labels.PortoBello_town.type = "Town";
	worldMap.labels.PortoBello_town.id = "PortoBello_town";
	worldMap.labels.PortoBello_town.locator = "PortoBello_town";
	worldMap.labels.PortoBello_town.text = CheckingTranslate(idLngFile,"PortoBello_town");
	worldMap.labels.PortoBello_town.icon = SPAIN;
	
	worldMap.labels.Panama_town.type = "Town";
	worldMap.labels.Panama_town.id = "Panama_town";
	worldMap.labels.Panama_town.locator = "Panama_town";
	worldMap.labels.Panama_town.text = CheckingTranslate(idLngFile,"Panama_town");
	worldMap.labels.Panama_town.icon = SPAIN;
	
	worldMap.labels.SantaCatalina_town.type = "Town";
	worldMap.labels.SantaCatalina_town.id = "SantaCatalina_town";
	worldMap.labels.SantaCatalina_town.locator = "SantaCatalina_town";
	worldMap.labels.SantaCatalina_town.text = CheckingTranslate(idLngFile,"SantaCatalina_town");
	worldMap.labels.SantaCatalina_town.icon = SPAIN;
	
	worldMap.labels.PortSpein_town.type = "Town";
	worldMap.labels.PortSpein_town.id = "PortSpein_town";
	worldMap.labels.PortSpein_town.locator = "PortSpein_town";
	worldMap.labels.PortSpein_town.text = CheckingTranslate(idLngFile,"PortSpein_town");
	worldMap.labels.PortSpein_town.icon = ENGLAND;
	
	worldMap.labels.Tortuga_town.type = "Town";
	worldMap.labels.Tortuga_town.id = "Tortuga_town";
	worldMap.labels.Tortuga_town.locator = "Tortuga_town";
	worldMap.labels.Tortuga_town.text = CheckingTranslate(idLngFile,"Tortuga_town");
	worldMap.labels.Tortuga_town.icon = FRANCE;
	
	worldMap.labels.Tenotchitlan_town.type = "Town";
	worldMap.labels.Tenotchitlan_town.id = "Tenotchitlan_town";
	worldMap.labels.Tenotchitlan_town.locator = "Tenotchitlan_town";
	worldMap.labels.Tenotchitlan_town.text = CheckingTranslate(idLngFile,"Tenotchitlan_town");
	//worldMap.labels.Tenotchitlan_town.icon = ENGLAND;
	
	////  SHORE /////////////////////////////
	worldMap.labels.Shore1.type = "Shore";
	worldMap.labels.Shore1.id = "Shore1";
	worldMap.labels.Shore1.locator = "Shore1";
	worldMap.labels.Shore1.text = CheckingTranslate(idLngFile,"Shore1");
	
	worldMap.labels.Shore2.type = "Shore";
	worldMap.labels.Shore2.id = "Shore2";
	worldMap.labels.Shore2.locator = "Shore2";
	worldMap.labels.Shore2.text = CheckingTranslate(idLngFile,"Shore2");
	
	worldMap.labels.Shore3.type = "Shore";
	worldMap.labels.Shore3.id = "Shore3";
	worldMap.labels.Shore3.locator = "Shore3";
	worldMap.labels.Shore3.text = CheckingTranslate(idLngFile,"Shore3");
	
	worldMap.labels.Shore4.type = "Shore";
	worldMap.labels.Shore4.id = "Shore4";
	worldMap.labels.Shore4.locator = "Shore4";
	worldMap.labels.Shore4.text = CheckingTranslate(idLngFile,"Shore4");
	
	worldMap.labels.Shore5.type = "Shore";
	worldMap.labels.Shore5.id = "Shore5";
	worldMap.labels.Shore5.locator = "Shore5";
	worldMap.labels.Shore5.text = CheckingTranslate(idLngFile,"Shore5");
	
	worldMap.labels.Shore6.type = "Shore";
	worldMap.labels.Shore6.id = "Shore6";
	worldMap.labels.Shore6.locator = "Shore6";
	worldMap.labels.Shore6.text = CheckingTranslate(idLngFile,"Shore6");
	
	worldMap.labels.Shore7.type = "Shore";
	worldMap.labels.Shore7.id = "Shore7";
	worldMap.labels.Shore7.locator = "Shore7";
	worldMap.labels.Shore7.text = CheckingTranslate(idLngFile,"Shore7");
	
	worldMap.labels.Shore8.type = "Shore";
	worldMap.labels.Shore8.id = "Shore8";
	worldMap.labels.Shore8.locator = "Shore8";
	worldMap.labels.Shore8.text = CheckingTranslate(idLngFile,"Shore8");
	
	worldMap.labels.Shore9.type = "Shore";
	worldMap.labels.Shore9.id = "Shore9";
	worldMap.labels.Shore9.locator = "Shore9";
	worldMap.labels.Shore9.text = CheckingTranslate(idLngFile,"Shore9");
	
	worldMap.labels.Shore10.type = "Shore";
	worldMap.labels.Shore10.id = "Shore10";
	worldMap.labels.Shore10.locator = "Shore10";
	worldMap.labels.Shore10.text = CheckingTranslate(idLngFile,"Shore10");
	
	worldMap.labels.Shore11.type = "Shore";
	worldMap.labels.Shore11.id = "Shore11";
	worldMap.labels.Shore11.locator = "Shore11";
	worldMap.labels.Shore11.text = CheckingTranslate(idLngFile,"Shore11");
	
	worldMap.labels.Shore12.type = "Shore";
	worldMap.labels.Shore12.id = "Shore12";
	worldMap.labels.Shore12.locator = "Shore12";
	worldMap.labels.Shore12.text = CheckingTranslate(idLngFile,"Shore12");
	
	worldMap.labels.Shore13.type = "Shore";
	worldMap.labels.Shore13.id = "Shore13";
	worldMap.labels.Shore13.locator = "Shore13";
	worldMap.labels.Shore13.text = CheckingTranslate(idLngFile,"Shore13");
	
	worldMap.labels.Shore14.type = "Shore";
	worldMap.labels.Shore14.id = "Shore14";
	worldMap.labels.Shore14.locator = "Shore14";
	worldMap.labels.Shore14.text = CheckingTranslate(idLngFile,"Shore14");
	
	worldMap.labels.Shore15.type = "Shore";
	worldMap.labels.Shore15.id = "Shore15";
	worldMap.labels.Shore15.locator = "Shore15";
	worldMap.labels.Shore15.text = CheckingTranslate(idLngFile,"Shore15");
	
	worldMap.labels.Shore16.type = "Shore";
	worldMap.labels.Shore16.id = "Shore16";
	worldMap.labels.Shore16.locator = "Shore16";
	worldMap.labels.Shore16.text = CheckingTranslate(idLngFile,"Shore16");
	
	worldMap.labels.Shore17.type = "Shore";
	worldMap.labels.Shore17.id = "Shore17";
	worldMap.labels.Shore17.locator = "Shore17";
	worldMap.labels.Shore17.text = CheckingTranslate(idLngFile,"Shore17");
	
	worldMap.labels.Shore18.type = "Shore";
	worldMap.labels.Shore18.id = "Shore18";
	worldMap.labels.Shore18.locator = "Shore18";
	worldMap.labels.Shore18.text = CheckingTranslate(idLngFile,"Shore18");
	
	worldMap.labels.Shore19.type = "Shore";
	worldMap.labels.Shore19.id = "Shore19";
	worldMap.labels.Shore19.locator = "Shore19";
	worldMap.labels.Shore19.text = CheckingTranslate(idLngFile,"Shore19");
	
	worldMap.labels.Shore20.type = "Shore";
	worldMap.labels.Shore20.id = "Shore20";
	worldMap.labels.Shore20.locator = "Shore20";
	worldMap.labels.Shore20.text = CheckingTranslate(idLngFile,"Shore20");
	
	worldMap.labels.Shore21.type = "Shore";
	worldMap.labels.Shore21.id = "Shore21";
	worldMap.labels.Shore21.locator = "Shore21";
	worldMap.labels.Shore21.text = CheckingTranslate(idLngFile,"Shore21");
	
	worldMap.labels.Shore22.type = "Shore";
	worldMap.labels.Shore22.id = "Shore22";
	worldMap.labels.Shore22.locator = "Shore22";
	worldMap.labels.Shore22.text = CheckingTranslate(idLngFile,"Shore22");
	
	worldMap.labels.Shore23.type = "Shore";
	worldMap.labels.Shore23.id = "Shore23";
	worldMap.labels.Shore23.locator = "Shore23";
	worldMap.labels.Shore23.text = CheckingTranslate(idLngFile,"Shore23");
	
	worldMap.labels.Shore24.type = "Shore";
	worldMap.labels.Shore24.id = "Shore24";
	worldMap.labels.Shore24.locator = "Shore24";
	worldMap.labels.Shore24.text = CheckingTranslate(idLngFile,"Shore24");
	
	worldMap.labels.Shore25.type = "Shore";
	worldMap.labels.Shore25.id = "Shore25";
	worldMap.labels.Shore25.locator = "Shore25";
	worldMap.labels.Shore25.text = CheckingTranslate(idLngFile,"Shore25");
	
	worldMap.labels.Shore26.type = "Shore";
	worldMap.labels.Shore26.id = "Shore26";
	worldMap.labels.Shore26.locator = "Shore26";
	worldMap.labels.Shore26.text = CheckingTranslate(idLngFile,"Shore26");
	
	worldMap.labels.Shore27.type = "Shore";
	worldMap.labels.Shore27.id = "Shore27";
	worldMap.labels.Shore27.locator = "Shore27";
	worldMap.labels.Shore27.text = CheckingTranslate(idLngFile,"Shore27");
	
	worldMap.labels.Shore28.type = "Shore";
	worldMap.labels.Shore28.id = "Shore28";
	worldMap.labels.Shore28.locator = "Shore28";
	worldMap.labels.Shore28.text = CheckingTranslate(idLngFile,"Shore28");
	
	worldMap.labels.Shore29.type = "Shore";
	worldMap.labels.Shore29.id = "Shore29";
	worldMap.labels.Shore29.locator = "Shore29";
	worldMap.labels.Shore29.text = CheckingTranslate(idLngFile,"Shore29");
	
	worldMap.labels.Shore30.type = "Shore";
	worldMap.labels.Shore30.id = "Shore30";
	worldMap.labels.Shore30.locator = "Shore30";
	worldMap.labels.Shore30.text = CheckingTranslate(idLngFile,"Shore30");
	
	worldMap.labels.Shore31.type = "Shore";
	worldMap.labels.Shore31.id = "Shore31";
	worldMap.labels.Shore31.locator = "Shore31";
	worldMap.labels.Shore31.text = CheckingTranslate(idLngFile,"Shore31");
	
	worldMap.labels.Shore32.type = "Shore";
	worldMap.labels.Shore32.id = "Shore32";
	worldMap.labels.Shore32.locator = "Shore32";
	worldMap.labels.Shore32.text = CheckingTranslate(idLngFile,"Shore32");
	
	worldMap.labels.Shore33.type = "Shore";
	worldMap.labels.Shore33.id = "Shore33";
	worldMap.labels.Shore33.locator = "Shore33";
	worldMap.labels.Shore33.text = CheckingTranslate(idLngFile,"Shore33");
	
	worldMap.labels.Shore34.type = "Shore";
	worldMap.labels.Shore34.id = "Shore34";
	worldMap.labels.Shore34.locator = "Shore34";
	worldMap.labels.Shore34.text = CheckingTranslate(idLngFile,"Shore34");
	
	worldMap.labels.Shore35.type = "Shore";
	worldMap.labels.Shore35.id = "Shore35";
	worldMap.labels.Shore35.locator = "Shore35";
	worldMap.labels.Shore35.text = CheckingTranslate(idLngFile,"Shore35");
	
	worldMap.labels.Shore36.type = "Shore";
	worldMap.labels.Shore36.id = "Shore36";
	worldMap.labels.Shore36.locator = "Shore36";
	worldMap.labels.Shore36.text = CheckingTranslate(idLngFile,"Shore36");
	
	worldMap.labels.Shore37.type = "Shore";
	worldMap.labels.Shore37.id = "Shore37";
	worldMap.labels.Shore37.locator = "Shore37";
	worldMap.labels.Shore37.text = CheckingTranslate(idLngFile,"Shore37");
	
	worldMap.labels.Shore38.type = "Shore";
	worldMap.labels.Shore38.id = "Shore38";
	worldMap.labels.Shore38.locator = "Shore38";
	worldMap.labels.Shore38.text = CheckingTranslate(idLngFile,"Shore38");
	
	worldMap.labels.Shore39.type = "Shore";
	worldMap.labels.Shore39.id = "Shore39";
	worldMap.labels.Shore39.locator = "Shore39";
	worldMap.labels.Shore39.text = CheckingTranslate(idLngFile,"Shore39");
	
	worldMap.labels.Shore40.type = "Shore";
	worldMap.labels.Shore40.id = "Shore40";
	worldMap.labels.Shore40.locator = "Shore40";
	worldMap.labels.Shore40.text = CheckingTranslate(idLngFile,"Shore40");
	
	worldMap.labels.Shore41.type = "Shore";
	worldMap.labels.Shore41.id = "Shore41";
	worldMap.labels.Shore41.locator = "Shore41";
	worldMap.labels.Shore41.text = CheckingTranslate(idLngFile,"Shore41");
	
	worldMap.labels.Shore42.type = "Shore";
	worldMap.labels.Shore42.id = "Shore42";
	worldMap.labels.Shore42.locator = "Shore42";
	worldMap.labels.Shore42.text = CheckingTranslate(idLngFile,"Shore42");
	
	worldMap.labels.Shore43.type = "Shore";
	worldMap.labels.Shore43.id = "Shore43";
	worldMap.labels.Shore43.locator = "Shore43";
	worldMap.labels.Shore43.text = CheckingTranslate(idLngFile,"Shore43");

    worldMap.labels.Shore44.type = "Shore";
	worldMap.labels.Shore44.id = "Shore44";
	worldMap.labels.Shore44.locator = "Shore44";
	worldMap.labels.Shore44.text = CheckingTranslate(idLngFile,"Shore44");
	
	worldMap.labels.Shore45.type = "Shore";
	worldMap.labels.Shore45.id = "Shore45";
	worldMap.labels.Shore45.locator = "Shore45";
	worldMap.labels.Shore45.text = CheckingTranslate(idLngFile,"Shore45");
	
	worldMap.labels.Shore46.type = "Shore";
	worldMap.labels.Shore46.id = "Shore46";
	worldMap.labels.Shore46.locator = "Shore46";
	worldMap.labels.Shore46.text = CheckingTranslate(idLngFile,"Shore46");
	
	worldMap.labels.Shore47.type = "Shore";
	worldMap.labels.Shore47.id = "Shore47";
	worldMap.labels.Shore47.locator = "Shore47";
	worldMap.labels.Shore47.text = CheckingTranslate(idLngFile,"Shore47");
	
	worldMap.labels.Shore48.type = "Shore";
	worldMap.labels.Shore48.id = "Shore48";
	worldMap.labels.Shore48.locator = "Shore48";
	worldMap.labels.Shore48.text = CheckingTranslate(idLngFile,"Shore48");
	
	worldMap.labels.Shore49.type = "Shore";
	worldMap.labels.Shore49.id = "Shore49";
	worldMap.labels.Shore49.locator = "Shore49";
	worldMap.labels.Shore49.text = CheckingTranslate(idLngFile,"Shore49");
	
	worldMap.labels.Shore50.type = "Shore";
	worldMap.labels.Shore50.id = "Shore50";
	worldMap.labels.Shore50.locator = "Shore50";
	worldMap.labels.Shore50.text = CheckingTranslate(idLngFile,"Shore50");
	
	worldMap.labels.Shore51.type = "Shore";
	worldMap.labels.Shore51.id = "Shore51";
	worldMap.labels.Shore51.locator = "Shore51";
	worldMap.labels.Shore51.text = CheckingTranslate(idLngFile,"Shore51");
	
	worldMap.labels.Shore52.type = "Shore";
	worldMap.labels.Shore52.id = "Shore52";
	worldMap.labels.Shore52.locator = "Shore52";
	worldMap.labels.Shore52.text = CheckingTranslate(idLngFile,"Shore52");
	
	worldMap.labels.Shore53.type = "Shore";
	worldMap.labels.Shore53.id = "Shore53";
	worldMap.labels.Shore53.locator = "Shore53";
	worldMap.labels.Shore53.text = CheckingTranslate(idLngFile,"Shore53");
	
	worldMap.labels.Shore54.type = "Shore";
	worldMap.labels.Shore54.id = "Shore54";
	worldMap.labels.Shore54.locator = "Shore54";
	worldMap.labels.Shore54.text = CheckingTranslate(idLngFile,"Shore54");
	
	worldMap.labels.Shore55.type = "Shore";
	worldMap.labels.Shore55.id = "Shore55";
	worldMap.labels.Shore55.locator = "Shore55";
	worldMap.labels.Shore55.text = CheckingTranslate(idLngFile,"Shore55");
	
	worldMap.labels.Shore56.type = "Shore";
	worldMap.labels.Shore56.id = "Shore56";
	worldMap.labels.Shore56.locator = "Shore56";
	worldMap.labels.Shore56.text = CheckingTranslate(idLngFile,"Shore56");
	
	worldMap.labels.Shore57.type = "Shore";
	worldMap.labels.Shore57.id = "Shore57";
	worldMap.labels.Shore57.locator = "Shore57";
	worldMap.labels.Shore57.text = CheckingTranslate(idLngFile,"Shore57");
	
	worldMap.labels.Shore58.type = "Shore";
	worldMap.labels.Shore58.id = "Shore58";
	worldMap.labels.Shore58.locator = "Shore58";
	worldMap.labels.Shore58.text = CheckingTranslate(idLngFile,"Shore58");
	
	worldMap.labels.Shore59.type = "Shore";
	worldMap.labels.Shore59.id = "Shore59";
	worldMap.labels.Shore59.locator = "Shore59";
	worldMap.labels.Shore59.text = CheckingTranslate(idLngFile,"Shore59");
	
	worldMap.labels.Shore60.type = "Shore";
	worldMap.labels.Shore60.id = "Shore60";
	worldMap.labels.Shore60.locator = "Shore60";
	worldMap.labels.Shore60.text = CheckingTranslate(idLngFile,"Shore60");
	
	worldMap.labels.Shore61.type = "Shore";
	worldMap.labels.Shore61.id = "Shore61";
	worldMap.labels.Shore61.locator = "Shore61";
	worldMap.labels.Shore61.text = CheckingTranslate(idLngFile,"Shore61");
	
	worldMap.labels.Shore62.type = "Shore";
	worldMap.labels.Shore62.id = "Shore62";
	worldMap.labels.Shore62.locator = "Shore62";
	worldMap.labels.Shore62.text = CheckingTranslate(idLngFile,"Shore62");
	
	worldMap.labels.Shore63.type = "Shore";
	worldMap.labels.Shore63.id = "Shore63";
	worldMap.labels.Shore63.locator = "Shore63";
	worldMap.labels.Shore63.text = CheckingTranslate(idLngFile,"Shore63");
	
	worldMap.labels.Shore_ship1.type = "Shore";
	worldMap.labels.Shore_ship1.id = "Shore_ship1";
	worldMap.labels.Shore_ship1.locator = "Shoreship1";
	worldMap.labels.Shore_ship1.text = CheckingTranslate(idLngFile,"Shore_ship1");
	
	worldMap.labels.Shore_ship2.type = "Shore";
	worldMap.labels.Shore_ship2.id = "Shore_ship2";
	worldMap.labels.Shore_ship2.locator = "Shoreship2";
	worldMap.labels.Shore_ship2.text = CheckingTranslate(idLngFile,"Shore_ship2");
	
	worldMap.labels.Shore_ship3.type = "Shore";
	worldMap.labels.Shore_ship3.id = "Shore_ship3";
	worldMap.labels.Shore_ship3.locator = "Shoreship3";
	worldMap.labels.Shore_ship3.text = CheckingTranslate(idLngFile,"Shore_ship3");
	
	worldMap.labels.Mayak1.type = "Shore";
	worldMap.labels.Mayak1.id = "Mayak1";
	worldMap.labels.Mayak1.locator = "Mayak1";
	worldMap.labels.Mayak1.text = CheckingTranslate(idLngFile,"Mayak1");
	
	worldMap.labels.Mayak2.type = "Shore";
	worldMap.labels.Mayak2.id = "Mayak2";
	worldMap.labels.Mayak2.locator = "Mayak2";
	worldMap.labels.Mayak2.text = CheckingTranslate(idLngFile,"Mayak2");
	
	worldMap.labels.Mayak3.type = "Shore";
	worldMap.labels.Mayak3.id = "Mayak3";
	worldMap.labels.Mayak3.locator = "Mayak3";
	worldMap.labels.Mayak3.text = CheckingTranslate(idLngFile,"Mayak3");
	
	worldMap.labels.Mayak4.type = "Shore";
	worldMap.labels.Mayak4.id = "Mayak4";
	worldMap.labels.Mayak4.locator = "Mayak4";
	worldMap.labels.Mayak4.text = CheckingTranslate(idLngFile,"Mayak4");
	
	worldMap.labels.Mayak5.type = "Shore";
	worldMap.labels.Mayak5.id = "Mayak5";
	worldMap.labels.Mayak5.locator = "Mayak5";
	worldMap.labels.Mayak5.text = CheckingTranslate(idLngFile,"Mayak5");
	
	worldMap.labels.Mayak6.type = "Shore";
	worldMap.labels.Mayak6.id = "Mayak6";
	worldMap.labels.Mayak6.locator = "Mayak6";
	worldMap.labels.Mayak6.text = CheckingTranslate(idLngFile,"Mayak6");
	
	worldMap.labels.Mayak7.type = "Shore";
	worldMap.labels.Mayak7.id = "Mayak7";
	worldMap.labels.Mayak7.locator = "Mayak7";
	worldMap.labels.Mayak7.text = CheckingTranslate(idLngFile,"Mayak7");
	
	worldMap.labels.Mayak8.type = "Shore";
	worldMap.labels.Mayak8.id = "Mayak8";
	worldMap.labels.Mayak8.locator = "Mayak8";
	worldMap.labels.Mayak8.text = CheckingTranslate(idLngFile,"Mayak8");
	
	worldMap.labels.Mayak9.type = "Shore";
	worldMap.labels.Mayak9.id = "Mayak9";
	worldMap.labels.Mayak9.locator = "Mayak9";
	worldMap.labels.Mayak9.text = CheckingTranslate(idLngFile,"Mayak9");
	
	worldMap.labels.Mayak10.type = "Shore";
	worldMap.labels.Mayak10.id = "Mayak10";
	worldMap.labels.Mayak10.locator = "Mayak10";
	worldMap.labels.Mayak10.text = CheckingTranslate(idLngFile,"Mayak10");
	
	worldMap.labels.Mayak11.type = "Shore";
	worldMap.labels.Mayak11.id = "Mayak11";
	worldMap.labels.Mayak11.locator = "Mayak11";
	worldMap.labels.Mayak11.text = CheckingTranslate(idLngFile,"Mayak11");

	//=========================================================================
	//Внутреняя информация - !!!Осторожно, ни каких проверок!!!
	//=========================================================================
	//Прараметры влияния ветра на скорость коробля (относительная скорость)
	worldMap.shipSpeedOppositeWind = 0.3;
	worldMap.shipSpeedOverWind = 0.8;

	//Player ship
	worldMap.playerShipActionRadius = 20.0;
	worldMap.playerShipX = -1000.0;
	worldMap.playerShipZ = -300.0;
	worldMap.playerShipAy = 0.0;
	//Camera
	worldMap.wdmCameraAY = 10.0;					//Угол поворота камеры
	worldMap.wdmCameraY = 130.0;				//Высота камеры
	worldMap.wdmCameraRotLock = 1;				//Заблокировать вращение камеры
	//Enemy ship	
	worldMap.enemyshipViewDistMin = 60.0;		//Растояние на котором корабль начинает исчезать
	worldMap.enemyshipViewDistMax = 120.0;		//Растояние на котором корабль исчезает полностью
    worldMap.enemyshipDistKill = 3000;          // homo 07/10/06
    //worldMap.enemyshipDistKill = 150.0;			//Расстояние на котором убиваем корабль
	worldMap.enemyshipBrnDistMin = 80.0;		//Минимальное растояние на котором рожается корабль
	worldMap.enemyshipBrnDistMax = 130.0;		//Максимальное растояние на котором рожается корабль
	//storm
	worldMap.stormViewDistMin = 90.0;			//Растояние на котором шторм начинает исчезать
	worldMap.stormViewDistMax = 180.0;			//Растояние на котором шторм исчезает полностью
	worldMap.stormDistKill = 190.0;				//Расстояние на котором убиваем шторм
	worldMap.stormBrnDistMin = 100.0;			//Минимальное растояние на котором рожается шторм
	worldMap.stormBrnDistMax = 140.0;			//Максимальное растояние на котором рожается шторм
	worldMap.stormZone = 90.0;// 120					//Общий радиус действия шторма

	//=========================================================================
	//Storm interface
	//=========================================================================
	worldMap.storm.num = 0;
	worldMap.storm.cur = 0;
	worldMap.storm.x = 0;
	worldMap.storm.z = 0;
	worldMap.storm.time = 0;
	worldMap.playerInStorm = 0;
	worldMap.stormWhithTornado = 0;
	//=========================================================================
	//Encounter interface
	//=========================================================================
	worldMap.encounter.num = 0;
	worldMap.encounter.cur = 0;
	worldMap.encounter.x = 0;
	worldMap.encounter.z = 0;
	worldMap.encounter.ay = 0;
	worldMap.encounter.time = 0;
	worldMap.encounter.type = -1;
	worldMap.encounter.attack = -1;
	worldMap.encounter.id = "";
	//=========================================================================
	//System, NOT FOR EDIT!!!
	//=========================================================================
	DeleteAttribute(&worldMap, "encounters");
	DeleteAttribute(&worldMap, "addQuestEncounters");
	worldMap.encounters = "";
	worldMap.addQuestEncounters = "";
	//=========================================================================
	//Current island
	//=========================================================================
	worldMap.island = WDM_NONE_ISLAND;
	worldMap.island.x = 0;
	worldMap.island.z = 0;
	worldMap.zeroX = 0;
	worldMap.zeroZ = 0;
	//=========================================================================
	//Wind (read only)
	//=========================================================================
	worldMap.WindX = 0.0;//Нормализованное направление
	worldMap.WindZ = 1.0;//ветра
	worldMap.WindF = 0.5;//Нормализованая сила ветра 0..1
	//=========================================================================
	//Old internal GUI params
	//=========================================================================
	worldMap.eventWindow.font.header = "normal";
	worldMap.eventWindow.font.text = "normal";
	worldMap.eventWindow.font.button = "normal";
	worldMap.date.font = "normal";

	//=========================================================================
	//Initialization labels
	//=========================================================================
	InitializationLabels();
	DeleteAttribute(&worldMap, "labeltype");
	//==========================================================================
	// координаты для начала игры, городов на бумажной карте и телепортов
	worldMap.islands.Barbados.position.x = 946.0062;
	worldMap.islands.Barbados.position.z = -486.9014;
	worldMap.islands.Barbados.Bridgetown_town.position.x = 903.6217;
	worldMap.islands.Barbados.Bridgetown_town.position.z = -522.2799;
	worldMap.islands.Barbados.Mayak2.position.x = 918.619;
	worldMap.islands.Barbados.Mayak2.position.z = -506.827;
	worldMap.islands.Barbados.Shore4.position.x = 919.808;
	worldMap.islands.Barbados.Shore4.position.z = -448.061;
	worldMap.islands.Barbados.Shore5.position.x = 983.787;
	worldMap.islands.Barbados.Shore5.position.z = -518.14;
	
	worldMap.islands.Antigua.position.x = 903.9222;
	worldMap.islands.Antigua.position.z = 109.0605;
	worldMap.islands.Antigua.SentJons_town.position.x = 852.4083;
	worldMap.islands.Antigua.SentJons_town.position.z = 128.9092;
	worldMap.islands.Antigua.Shore1.position.x = 880.892;
	worldMap.islands.Antigua.Shore1.position.z = 148.897;
	worldMap.islands.Antigua.Shore2.position.x = 931.028;
	worldMap.islands.Antigua.Shore2.position.z = 74.5353;
	
	worldMap.islands.Curacao.position.x = 383.4142;
	worldMap.islands.Curacao.position.z = -691.11;
	worldMap.islands.Curacao.Villemstad_town.position.x = 396.1;
	worldMap.islands.Curacao.Villemstad_town.position.z = -722.8;
	worldMap.islands.Curacao.Shore22.position.x = 354.604;
	worldMap.islands.Curacao.Shore22.position.z = -678.54;
	worldMap.islands.Curacao.Shore23.position.x = 347.313;
	worldMap.islands.Curacao.Shore23.position.z = -705.112;
	worldMap.islands.Curacao.Shore24.position.x = 455.427;
	worldMap.islands.Curacao.Shore24.position.z = -740.655;
	
	//worldMap.islands.SentMartin.position.x = 926.4412;
	//worldMap.islands.SentMartin.position.z = 478.7172;
	//у меня по другому. Что правильно??
	worldMap.islands.SentMartin.position.x = 921.559;
	worldMap.islands.SentMartin.position.z = 523.596;
	worldMap.islands.SentMartin.Marigo_town.position.x = 900.0994;
	worldMap.islands.SentMartin.Marigo_town.position.z = 501.6531;
	worldMap.islands.SentMartin.Shore40.position.x = 924.442;
	worldMap.islands.SentMartin.Shore40.position.z = 554.385;
	worldMap.islands.SentMartin.Shore41.position.x = 898.877;
	worldMap.islands.SentMartin.Shore41.position.z = 522.703;
	
	//worldMap.islands.Guadeloupe.position.x = 469.6634;
	//worldMap.islands.Guadeloupe.position.z = -4.893611;
	worldMap.islands.Guadeloupe.position.x = 537.114;
	worldMap.islands.Guadeloupe.position.z = -25.3853;
	worldMap.islands.Guadeloupe.BasTer_town.position.x = 462.2107;
	worldMap.islands.Guadeloupe.BasTer_town.position.z = -73.0837;
	worldMap.islands.Guadeloupe.Mayak4.position.x = 465.076;
	worldMap.islands.Guadeloupe.Mayak4.position.z = -38.9423;
	worldMap.islands.Guadeloupe.Shore28.position.x = 536.636;
	worldMap.islands.Guadeloupe.Shore28.position.z = 26.1279;
	worldMap.islands.Guadeloupe.Shore29.position.x = 507.669;
	worldMap.islands.Guadeloupe.Shore29.position.z = -75.4176;
	worldMap.islands.Guadeloupe.Shore30.position.x = 549.807;
	worldMap.islands.Guadeloupe.Shore30.position.z = -69.7107;
	worldMap.islands.Guadeloupe.Shore31.position.x = 589889;
	worldMap.islands.Guadeloupe.Shore31.position.z = -65.0601;
	
	//worldMap.islands.Martinique.position.x = 511.8408;
	//worldMap.islands.Martinique.position.z = -355.744;
	worldMap.islands.Martinique.position.x = 538.524;
	worldMap.islands.Martinique.position.z = -385.567;
	worldMap.islands.Martinique.FortFrance_town.position.x = 513.51;
	worldMap.islands.Martinique.FortFrance_town.position.z = -397.645;
	worldMap.islands.Martinique.LeFransua_town.position.x = 559.008;
	worldMap.islands.Martinique.LeFransua_town.position.z = -368.16;
	worldMap.islands.Martinique.Shore38.position.x = 481.181;
	worldMap.islands.Martinique.Shore38.position.z = -340.832;
	worldMap.islands.Martinique.Shore39.position.x = 557.181;
	worldMap.islands.Martinique.Shore39.position.z = -422.987;
	
	//worldMap.islands.Bermudes.position.x = 947.57;
	//worldMap.islands.Bermudes.position.z = 947.607;
	worldMap.islands.Bermudes.position.x = 920.738;
	worldMap.islands.Bermudes.position.z = 920.767;
	worldMap.islands.Bermudes.Pirates_town.position.x = 931.059;
	worldMap.islands.Bermudes.Pirates_town.position.z = 928.281;
	worldMap.islands.Bermudes.Shore3.position.x = 915.745;
	worldMap.islands.Bermudes.Shore3.position.z = 966.469;
	worldMap.islands.Bermudes.Shore_ship1.position.x = 968.947;
	worldMap.islands.Bermudes.Shore_ship1.position.z = 923.636;
	
	worldMap.islands.Maracaibo.position.x = 178.51;
	worldMap.islands.Maracaibo.position.z = -870.37;
	worldMap.islands.Maracaibo.Maracaibo_town.position.x = 134.82;
	worldMap.islands.Maracaibo.Maracaibo_town.position.z = -939.65;
	worldMap.islands.Maracaibo.Shore37.position.x = 140.753;
	worldMap.islands.Maracaibo.Shore37.position.z = -828.99;
	worldMap.islands.Maracaibo.Shore_ship3.position.x = 198.162;
	worldMap.islands.Maracaibo.Shore_ship3.position.z = -994.035;
	
	worldMap.islands.Beliz.position.x = -896.084;
	worldMap.islands.Beliz.position.z = 230.227;
	worldMap.islands.Beliz.Shore_ship2.position.x = -1022.94;
	worldMap.islands.Beliz.Shore_ship2.position.z = 629.355;
	worldMap.islands.Beliz.Shore6.position.x = -909.655;
	worldMap.islands.Beliz.Shore6.position.z = 629.079;
	worldMap.islands.Beliz.Shore7.position.x = -915.581;
	worldMap.islands.Beliz.Shore7.position.z = 409.002;
	worldMap.islands.Beliz.Shore8.position.x = -963.489;
	worldMap.islands.Beliz.Shore8.position.z = 281.628;
	worldMap.islands.Beliz.Beliz_town.position.x = -978.396;
	worldMap.islands.Beliz.Beliz_town.position.z = 158.751;
	worldMap.islands.Beliz.Shore9.position.x = -1017.2;
	worldMap.islands.Beliz.Shore9.position.z = -33.7293;
	worldMap.islands.Beliz.Shore10.position.x = -818.198;
	worldMap.islands.Beliz.Shore10.position.z = -177.208;
	//  не сделано -->
	worldMap.islands.Jamaica.position.x = -298.0;
	worldMap.islands.Jamaica.position.z = -128.0;
	worldMap.islands.Jamaica.PortRoyal_town.position.x = -250.157;
	worldMap.islands.Jamaica.PortRoyal_town.position.z = -150.839;
	worldMap.islands.Jamaica.Shore35.position.x = -446.951;
	worldMap.islands.Jamaica.Shore35.position.z = -107.118;
	worldMap.islands.Jamaica.Shore36.position.x = -281.543;
	worldMap.islands.Jamaica.Shore36.position.z = -164.245;
	worldMap.islands.Jamaica.Mayak3.position.x = -224.127;
	worldMap.islands.Jamaica.Mayak3.position.z = -163.435;
	
	worldMap.islands.PuertoRico.position.x = 506.489;
	worldMap.islands.PuertoRico.position.z = 460.815;
	worldMap.islands.PuertoRico.SanJuan_town.position.x = 528.645;
	worldMap.islands.PuertoRico.SanJuan_town.position.z = 484.18;
	worldMap.islands.PuertoRico.Shore44.position.x = 392.82;
	worldMap.islands.PuertoRico.Shore44.position.z = 501.808;
	worldMap.islands.PuertoRico.Shore45.position.x = 602.049;
	worldMap.islands.PuertoRico.Shore45.position.z = 458.322;
	
	worldMap.islands.Cuba1.position.x = -358.027;
	worldMap.islands.Cuba1.position.z = 819.223;
	worldMap.islands.Cuba1.Santiago_town.position.x = -339.796;
	worldMap.islands.Cuba1.Santiago_town.position.z = 771.839;
	worldMap.islands.Cuba1.PuertoPrincipe_town.position.x = -282.032;
	worldMap.islands.Cuba1.PuertoPrincipe_town.position.z = 839.056;
	worldMap.islands.Cuba1.Shore11.position.x = -386.907;
	worldMap.islands.Cuba1.Shore11.position.z = 883.044;
	worldMap.islands.Cuba1.Shore12.position.x = -468.486;
	worldMap.islands.Cuba1.Shore12.position.z = 838.497;
	worldMap.islands.Cuba1.Mayak9.position.x = -333.295;
	worldMap.islands.Cuba1.Mayak9.position.z = 723.486;
	
	worldMap.islands.Cuba2.position.x = -640.61;
	worldMap.islands.Cuba2.position.z = 870.058;
	worldMap.islands.Cuba2.Havana_town.position.x = -663.685;
	worldMap.islands.Cuba2.Havana_town.position.z = 913.862;
	worldMap.islands.Cuba2.Shore13.position.x = -613.56;
	worldMap.islands.Cuba2.Shore13.position.z = 886.009;
	worldMap.islands.Cuba2.Shore14.position.x = -653.269;
	worldMap.islands.Cuba2.Shore14.position.z = 871.013;
	worldMap.islands.Cuba2.Shore15.position.x = -715.129;
	worldMap.islands.Cuba2.Shore15.position.z = 797.212;
	worldMap.islands.Cuba2.Mayak10.position.x = -647.18;
	worldMap.islands.Cuba2.Mayak10.position.z = 925.046;
	
	worldMap.islands.Tortuga.position.x = 102.328;
	worldMap.islands.Tortuga.position.z = 852.912;
	worldMap.islands.Tortuga.Tortuga_town.position.x = 116.875;
	worldMap.islands.Tortuga.Tortuga_town.position.z = 817.942;
	worldMap.islands.Tortuga.Shore58.position.x = 131.064;
	worldMap.islands.Tortuga.Shore58.position.z = 849.44;
	worldMap.islands.Tortuga.Mayak6.position.x = 103.23;
	worldMap.islands.Tortuga.Mayak6.position.z = 812.356;
	
	worldMap.islands.Trinidad.position.x = 889.76;
	worldMap.islands.Trinidad.position.z = -774.746;
	worldMap.islands.Trinidad.PortSpein_town.position.x = 848.282;
	worldMap.islands.Trinidad.PortSpein_town.position.z = -822.588;
	worldMap.islands.Trinidad.Shore59.position.x = 937.322;
	worldMap.islands.Trinidad.Shore59.position.z = -737.478;
	worldMap.islands.Trinidad.Shore60.position.x = 906.25;
	worldMap.islands.Trinidad.Shore60.position.z = -757.928;
	worldMap.islands.Trinidad.Mayak1.position.x = 821.24;
	worldMap.islands.Trinidad.Mayak1.position.z = -847.69;
	
	worldMap.islands.Nevis.position.x = 681.595;
	worldMap.islands.Nevis.position.z = 256.013;
	worldMap.islands.Nevis.Charles_town.position.x = 658.918;
	worldMap.islands.Nevis.Charles_town.position.z = 241.972;
	worldMap.islands.Nevis.Shore42.position.x = 690.047;
	worldMap.islands.Nevis.Shore42.position.z = 291.621;
	worldMap.islands.Nevis.Shore43.position.x = 684.026;
	worldMap.islands.Nevis.Shore43.position.z = 212.793;
	worldMap.islands.Nevis.Mayak5.position.x = 663.418;
	worldMap.islands.Nevis.Mayak5.position.z = 228.281;
	
	worldMap.islands.Hispaniola1.position.x = 198.977;
	worldMap.islands.Hispaniola1.position.z = 588.696;
	worldMap.islands.Hispaniola1.SantoDomingo_town.position.x = 198.683;
	worldMap.islands.Hispaniola1.SantoDomingo_town.position.z = 547.97;
	worldMap.islands.Hispaniola1.LaVega_port.position.x = 166.708;
	worldMap.islands.Hispaniola1.LaVega_port.position.z = 662.952;
	worldMap.islands.Hispaniola1.Shore32.position.x = 130.591;
	worldMap.islands.Hispaniola1.Shore32.position.z = 682.362;
	worldMap.islands.Hispaniola1.Shore33.position.x = 229.081;
	worldMap.islands.Hispaniola1.Shore33.position.z = 593.727;
	worldMap.islands.Hispaniola1.Mayak8.position.x = 168.314;
	worldMap.islands.Hispaniola1.Mayak8.position.z = 520.218;
	
	worldMap.islands.Hispaniola2.position.x = -47.1762;
	worldMap.islands.Hispaniola2.position.z = 625.596;
 	worldMap.islands.Hispaniola2.PortPax_town.position.x = 22.521;
	worldMap.islands.Hispaniola2.PortPax_town.position.z = 592.233;
	worldMap.islands.Hispaniola2.Shore34.position.x = 0.999614;
	worldMap.islands.Hispaniola2.Shore34.position.z = 643.122;
	worldMap.islands.Hispaniola2.Mayak7.position.x = -14.9756;
	worldMap.islands.Hispaniola2.Mayak7.position.z = 606.258;
	
	worldMap.islands.Dominica.position.x = 783.057;
	worldMap.islands.Dominica.position.z = -256.833;
	worldMap.islands.Dominica.Shore27.position.x = 779.89;
	worldMap.islands.Dominica.Shore27.position.z = -295.002;
	worldMap.islands.Dominica.Shore26.position.x = 803.577;
	worldMap.islands.Dominica.Shore26.position.z = -252.617;

	
	worldMap.islands.Terks.position.x = 443.257;
	worldMap.islands.Terks.position.z = 939.102;
	worldMap.islands.Terks.Shore57.position.x = 434.944;
	worldMap.islands.Terks.Shore57.position.z = 923.524;
	worldMap.islands.Terks.Shore56.position.x = 444.298;
	worldMap.islands.Terks.Shore56.position.z = 954.975;

	
	worldMap.islands.Caiman.position.x = -489.6;
	worldMap.islands.Caiman.position.z = 362.455;
	worldMap.islands.Caiman.Shore17.position.x = -505.038;
	worldMap.islands.Caiman.Shore17.position.z = 348.115;
	worldMap.islands.Caiman.Shore16.position.x = -449.837;
	worldMap.islands.Caiman.Shore16.position.z = 366.308;

	
	worldMap.islands.Panama.position.x = -395.198;
	worldMap.islands.Panama.position.z = -972.801;
	worldMap.islands.Panama.Panama_town.position.x = -389.965;
	worldMap.islands.Panama.Panama_town.position.z = -949.118;
	worldMap.islands.Panama.Shore63.position.x = -328.166;
	worldMap.islands.Panama.Shore63.position.z = -1023.72;
	
	worldMap.islands.PortoBello.position.x = -435.73;
	worldMap.islands.PortoBello.position.z = -895.763;
	worldMap.islands.PortoBello.PortoBello_town.position.x = -427.183;
	worldMap.islands.PortoBello.PortoBello_town.position.z = -907.277;
	worldMap.islands.PortoBello.Shore46.position.x = -627.474;
	worldMap.islands.PortoBello.Shore46.position.z = -940.853;
	worldMap.islands.PortoBello.Shore47.position.x = -486.49;
	worldMap.islands.PortoBello.Shore47.position.z = -976.966;
	worldMap.islands.PortoBello.Shore48.position.x = -279.316;
	worldMap.islands.PortoBello.Shore48.position.z = -1005.49;
	
	worldMap.islands.Cartahena.position.x = -51.2104;
	worldMap.islands.Cartahena.position.z = -851.77;
	worldMap.islands.Cartahena.Cartahena_town.position.x = -26.7669;
	worldMap.islands.Cartahena.Cartahena_town.position.z = -872.535;
	worldMap.islands.Cartahena.Shore25.position.x = -115.589;
	worldMap.islands.Cartahena.Shore25.position.z = -937.387;
	worldMap.islands.Cartahena.Mayak11.position.x = -65.6347;
	worldMap.islands.Cartahena.Mayak11.position.z = -842.072;
	
	worldMap.islands.Caracas.position.x = 435.126;
	worldMap.islands.Caracas.position.z = -936.197;
	worldMap.islands.Caracas.Caracas_town.position.x = 440.684;
	worldMap.islands.Caracas.Caracas_town.position.z = -960.339;
	worldMap.islands.Caracas.Shore21.position.x = 410.688;
	worldMap.islands.Caracas.Shore21.position.z = -913.531;
	
	worldMap.islands.Cumana.position.x = 767.999;
	worldMap.islands.Cumana.position.z = -936.266;
	worldMap.islands.Cumana.Cumana_town.position.x = 691.343;
	worldMap.islands.Cumana.Cumana_town.position.z = -896.619;
	worldMap.islands.Cumana.Shore20.position.x = 574.327;
	worldMap.islands.Cumana.Shore20.position.z = -929.042;
	worldMap.islands.Cumana.Shore19.position.x = 768.622;
	worldMap.islands.Cumana.Shore19.position.z = -914.469;
	worldMap.islands.Cumana.Shore18.position.x = 924.423;
	worldMap.islands.Cumana.Shore18.position.z = -969.921;
	
	worldMap.islands.SantaCatalina.position.x = -742.269;
	worldMap.islands.SantaCatalina.position.z = -563.205;
	worldMap.islands.SantaCatalina.SantaCatalina_town.position.x = -769.375;
	worldMap.islands.SantaCatalina.SantaCatalina_town.position.z = -537.051;
	worldMap.islands.SantaCatalina.Shore53.position.x = -792.718;
	worldMap.islands.SantaCatalina.Shore53.position.z = -331.728;
	worldMap.islands.SantaCatalina.Shore54.position.x = -760.343;
	worldMap.islands.SantaCatalina.Shore54.position.z = -484.106;
	worldMap.islands.SantaCatalina.Shore55.position.x = -736.763;
	worldMap.islands.SantaCatalina.Shore55.position.z = -756.295;

	worldMap.islands.LostShipsCity.position.x = -938.667;
	worldMap.islands.LostShipsCity.position.z = 938.654;

// Не знаю- нужно или пока нет.
	//worldMap.islands.Tenotchitlan.position.x = -966.062;
	//worldMap.islands.Tenotchitlan.position.z = -294.232;
	//worldMap.islands.Tenotchitlan.Tenotchitlan_town.position.x = -969.194;
	//worldMap.islands.Tenotchitlan.Tenotchitlan_town.position.z = -305.237;
	//worldMap.islands.Tenotchitlan.Shore61.position.x = -955.553;
	//worldMap.islands.Tenotchitlan.Shore61.position.z = -281.197;
	//worldMap.islands.Tenotchitlan.Shore62.position.x = -935.811;
	//worldMap.islands.Tenotchitlan.Shore62.position.z = -316.162;

	////////////////////////////////////////////////////////////////////////////
	wdmReset();
	LanguageCloseFile(idLngFile);
}

void InitializationLabels()
{
	//Переберём все острова
	aref arrayLabels;
	makearef(arrayLabels, worldMap.labels);
	int num = GetAttributesNum(arrayLabels);
	for(int i = 0; i < num; i++)
	{
		//Ссылка на описание метки
		aref label = GetAttributeN(arrayLabels, i);
		//Получаем тип метки
		if(!CheckAttribute(label, "type")) continue;
		string typeName = label.type;
		//Параметры типа метки
		aref typeParams;
		makearef(typeParams, worldMap.labeltype.(typeName));
		//Перебераем параметры типа метки и добавляем к метке
		int numParams = GetAttributesNum(typeParams);
		for(int j = 0; j < numParams; j++)
		{
			//Имя параметра
			string param = GetAttributeName(GetAttributeN(typeParams, j));
			//Проверяем наличие в метке
			if(CheckAttribute(label, param) != 0) continue;
			//Копируем параметр
			label.(param) = typeParams.(param);
		}
	}
}