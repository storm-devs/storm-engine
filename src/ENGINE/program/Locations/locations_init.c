
#include "locations\init\Antigua.c"
#include "locations\init\Barbados.c"
#include "locations\init\Curacao.c"
#include "locations\init\Guadeloupe.c"
#include "locations\init\laVega.c"
#include "locations\init\Bermudes.c"
#include "locations\init\SantoDomingo.c"
#include "locations\init\PortPax.c"
#include "locations\init\Santiago.c"
#include "locations\init\PuertoPrincipe.c"
#include "locations\init\Martinique.c"
#include "locations\init\Havana.c"
#include "locations\init\Nevis.c"
#include "locations\init\PuertoRico.c"
#include "locations\init\PortoBello.c"
#include "locations\init\Trinidad.c"
#include "locations\init\SentMartin.c"
#include "locations\init\Tortuga.c"
#include "locations\init\QuestLocations.c"
#include "locations\init\boarding.c"
#include "locations\init\CommonLoc.c"
#include "locations\init\Dominica.c"
#include "locations\init\Terks.c"
#include "locations\init\Caiman.c"
#include "locations\init\Jamaica.c"
#include "locations\init\Panama.c"
#include "locations\init\Cartahena.c"
#include "locations\init\Maracaibo.c"
#include "locations\init\Caracas.c"
#include "locations\init\Cumana.c"
#include "locations\init\SantaCatalina.c"
#include "locations\init\Beliz.c"
#include "locations\init\Pearl.c"
#include "locations\init\Tenotchitlan.c"
#include "locations\init\LostShipsCity.c"

void InitLocations()
{
	ref loc;
	for(int i = 0; i < MAX_LOCATIONS; i++)
	{
		makeref(loc, Locations[i]);
		DeleteAttribute(loc, "");
		loc.id = "";
		loc.index = i;
		loc.image = "loading\sea.tga";
		loc.music = "enplav.ogg";
		loc.locators_radius.reload = 1.0;
		loc.locators_radius.rld = 1.0;
		loc.locators_radius.camdetector = 1.0;
		loc.locators_radius.camera = 0.3;
		loc.locators_radius.characters = 0.5;
		loc.locators_radius.goto = 0.5;		
		loc.locators_radius.sit = 0.5;		
		loc.locators_radius.item = 0.5;		
		loc.locators_radius.officers = 0.5;
		loc.locators_radius.merchant = 1.0;
		loc.locators_radius.box = 1.0;
		loc.locators_radius.candles = 0.1;
		loc.locators_radius.candles_medium = 0.2;
		loc.locators_radius.chandeliers = 0.5;
		loc.locators_radius.heaters = 1.0;
		loc.locators_radius.torchlightes = 0.3;
		loc.locators_radius.torchlightes_o = 0.3;
		loc.locators_radius.fonar = 0.4;
		loc.locators_radius.outside = 2.0;
		loc.locators_radius.incas_light = 0.8;
		loc.locators_radius.incas_sky = 1.0;
		loc.locators_radius.randitem = 1.5;
		loc.locators_radius.waitress = 1.0;
		loc.locators_radius.tables = 0.5;
		loc.locators_radius.barmen = 1.0;
		loc.locators_radius.lamp = 0.2;
		loc.locators_radius.blueteleport = 0.2;
		loc.locators_radius.redteleport = 0.2;
		loc.locators_radius.greenteleport = 0.2;
		loc.locators_radius.magsteleport = 0.2;
		loc.locators_radius.EncDetector = 8.0;
		loc.locators_radius.teleport = 1.0;
		loc.locators_radius.quest = 1.0;
		if (MOD_BETTATESTMODE == "On")
		{// чтоб было видно в отладке Boal 18.08.06
			loc.locators_radius.soldiers = 0.5;
			loc.locators_radius.patrol = 0.5;
			loc.locators_radius.Smugglers = 0.5;
			loc.locators_radius.monsters = 0.5;
		}
		
		//Day dynamic light
		loc.models.day.lights.candles = "candle";
		loc.models.day.lights.candles_medium = "candelabrum";
		loc.models.day.lights.chandeliers = "chandelier";
		loc.models.day.lights.heaters = "heater";
		loc.models.day.lights.torchlightes = "torchlight";		
		loc.models.day.lights.outside = "outside_day";
		loc.models.day.lights.incas_light = "incas";
		loc.models.day.lights.incas_sky = "incasskyday";
		loc.models.day.lights.lamp = "lamp";
		loc.models.day.lights.blueteleport = "blueteleport";
		loc.models.day.lights.redteleport = "redteleport";
		loc.models.day.lights.greenteleport = "greenteleport";
		loc.models.day.lights.magsteleport = "magsteleport";

		//Night dynamic light
		loc.models.night.lights.candles = "candle";
		loc.models.night.lights.candles_medium = "candelabrum";
		loc.models.night.lights.chandeliers = "chandelier";
		loc.models.night.lights.heaters = "heater";
		loc.models.night.lights.torchlightes = "torchlight";
		loc.models.night.lights.torchlightes_o = "torchlight";
		loc.models.night.lights.fonar = "lamp";
		loc.models.night.lights.outside = "outside_night";		
		loc.models.night.lights.incas_light = "incas";
		loc.models.night.lights.incas_sky = "incasskynight";
		loc.models.night.lights.lamp = "lamp";
		loc.models.night.lights.blueteleport = "blueteleport";
		loc.models.night.lights.redteleport = "redteleport";
		loc.models.night.lights.greenteleport = "greenteleport";
		loc.models.night.lights.magsteleport = "magsteleport";
	}


	ReloadProgressUpdate();

	//Init locations
	int n = 0;
	Locations[0].IslandsList.Antigua.begin = n;  //SentJons
	n = LocationInitAntigua(n);
	Locations[0].IslandsList.Antigua.end = n;
	Trace("Antigua locations " + n);
	
	Locations[0].IslandsList.Barbados.begin = n;  //Bridgetown
	n = LocationInitBarbados(n);
	Locations[0].IslandsList.Barbados.end = n;
	Trace("Barbados locations " + n);
	
	Locations[0].IslandsList.Curacao.begin = n;  //Villemstad
	n = LocationInitCuracao(n);
	Locations[0].IslandsList.Curacao.end = n;
	Trace("Curacao locations " + n);

	Locations[0].IslandsList.LostShipsIsland.begin = n;  //ГПК
	n = LocationInitLostShipsCity(n);
	Locations[0].IslandsList.LostShipsIsland.end = n;
	Trace("LostShipsIsland locations " + n);	

	ReloadProgressUpdate();
	
	Locations[0].IslandsList.Guadeloupe.begin = n;
	n = LocationInitGuadeloupe(n);
	Locations[0].IslandsList.Guadeloupe.end = n;
	Trace("Guadeloupe locations " + n);
	
	Locations[0].IslandsList.Jamaica.begin = n;
	n = LocationInitJamaica(n);
	Locations[0].IslandsList.Jamaica.end = n;
	Trace("Jamaica locations " + n);

	Locations[0].IslandsList.Caracas.begin = n;
	n = LocationInitCaracas(n);
	Locations[0].IslandsList.Caracas.end = n;
	Trace("Caracas locations " + n);
	
	Locations[0].IslandsList.Pearl.begin = n;
	n = LocationInitPearl(n);
	Locations[0].IslandsList.Pearl.end = n;
	Trace("Pearl locations " + n);

	ReloadProgressUpdate();

	Locations[0].IslandsList.Bermudes.begin = n;
	n = LocationInitBermudes(n);
	Locations[0].IslandsList.Bermudes.end = n;
	Trace("Bermudes locations " + n);

	Locations[0].IslandsList.Cartahena.begin = n;
	n = LocationInitCartahena(n);
	Locations[0].IslandsList.Cartahena.end = n;
	Trace("Cartahena locations " + n);

	Locations[0].IslandsList.Maracaibo.begin = n;
	n = LocationInitMaracaibo(n);
	Locations[0].IslandsList.Maracaibo.end = n;
	Trace("Maracaibo locations " + n);

	Locations[0].IslandsList.Tenotchitlan.begin = n;
	n = LocationInitTenotchitlan(n);
	Locations[0].IslandsList.Tenotchitlan.end = n;
	Trace("Tenotchitlan locations " + n);

	ReloadProgressUpdate();
	
	Locations[0].IslandsList.Hispaniola1.begin = n;
	n = LocationInitLaVega(n);
	Trace("LaVega locations " + n);
	n = LocationInitSantoDomingo(n);
	Trace("SantoDomingo locations " + n);
	Locations[0].IslandsList.Hispaniola1.end = n;

	Locations[0].IslandsList.Hispaniola2.begin = n;
	n = LocationInitPortPax(n);
	Trace("PortPax locations " + n);
	Locations[0].IslandsList.Hispaniola2.end = n;
	
	Locations[0].IslandsList.Cuba1.begin = n;
	n = LocationInitSantiago(n);
	Trace("Santiago locations " + n);
	n = LocationInitPuertoPrincipe(n);
	Trace("PuertoPrincipe locations " + n);
	Locations[0].IslandsList.Cuba1.end = n;

	Locations[0].IslandsList.Cuba2.begin = n;
	n = LocationInitHavana(n);
	Locations[0].IslandsList.Cuba2.end = n;
	Trace("Havana locations " + n);
	
	ReloadProgressUpdate();
	
	Locations[0].IslandsList.Martinique.begin = n; //FortFrance
	n = LocationInitMartinique(n);
	Locations[0].IslandsList.Martinique.end = n;
	Trace("Martinique locations " + n);
	
	Locations[0].IslandsList.Panama.begin = n;
	n = LocationInitPanama(n);
	Locations[0].IslandsList.Panama.end = n;
	Trace("Panama locations " + n);

	Locations[0].IslandsList.PortoBello.begin = n;
	n = LocationInitPortoBello(n);
	Locations[0].IslandsList.PortoBello.end = n;
	Trace("PortoBello locations " + n);

	Locations[0].IslandsList.Beliz.begin = n;
	n = LocationInitBeliz(n);
	Locations[0].IslandsList.Beliz.end = n;
	Trace("Beliz locations " + n);

	ReloadProgressUpdate();
	
	Locations[0].IslandsList.Nevis.begin = n;
	n = LocationInitNevis(n);
	Locations[0].IslandsList.Nevis.end = n;
	Trace("Nevis locations " + n);
	
	Locations[0].IslandsList.PuertoRico.begin = n;
	n = LocationInitPuertoRico(n);
	Locations[0].IslandsList.PuertoRico.end = n;
	Trace("PuertoRico locations " + n);
	
	Locations[0].IslandsList.Cumana.begin = n;
	n = LocationInitCumana(n);
	Locations[0].IslandsList.Cumana.end = n;
	Trace("Cumana locations " + n);

	Locations[0].IslandsList.SantaCatalina.begin = n;
	n = LocationInitSantaCatalina(n);
	Locations[0].IslandsList.SantaCatalina.end = n;
	Trace("SantaCatalina locations " + n);

	ReloadProgressUpdate();
	
	Locations[0].IslandsList.Trinidad.begin = n; //PortSpein
	n = LocationInitTrinidad(n);
	Locations[0].IslandsList.Trinidad.end = n;
 	Trace("Trinidad locations " + n);

	Locations[0].IslandsList.SentMartin.begin = n;
	n = LocationInitSentMartin(n);
	Locations[0].IslandsList.SentMartin.end = n;
	Trace("SentMartin locations " + n);

	Locations[0].IslandsList.Tortuga.begin = n;
	n = LocationInitTortuga(n);
	Locations[0].IslandsList.Tortuga.end = n;
	Trace("Tortuga locations " + n);

	Locations[0].IslandsList.Dominica.begin = n;
	n = LocationInitDominica(n);
	Locations[0].IslandsList.Dominica.end = n;
	Trace("Dominica locations " + n);

	Locations[0].IslandsList.Terks.begin = n;
	n = LocationInitTerks(n);
	Locations[0].IslandsList.Terks.end = n;
	Trace("Terks locations " + n);
	
	Locations[0].IslandsList.Caiman.begin = n;
	n = LocationInitCaiman(n);
	Locations[0].IslandsList.Caiman.end = n;
	Trace("Caiman locations " + n);

	n = LocationInitCommonLoc(n);
	Trace("Common locations " + n);
	
	ReloadProgressUpdate();
	
	n = LocationInitQuestLocations(n);
	n = LocationInitBoarding(n);
	nLocationsNum = n;
	Trace("Number of locations: " + nLocationsNum);

}
