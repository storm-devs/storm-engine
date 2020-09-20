

int locations_TestCounter = 0;
int locations_TestWarnCounter = 0;
int locations_TestBad = 0;
bool locations_TestIsBad = false;
bool locations_TestIsWarn = false;
string locations_TestNewLine = "";
string locations_TestResult = "";

void Locations_TestAll()
{
	locations_TestBad = 0;
	locations_TestWarnCounter = 0;
	Trace("======================================================================");
	Trace("");
	Trace("");
	Trace("Location locators test started...");
	Trace("");
	Trace("");
	Trace("======================================================================");
	Trace("");
	Trace("");
	Trace("");
	Trace("");
	locations_TestCounter = 0;
	SetEventHandler("frame","Locations_TestAllProcess", 1);
	locations_TestNewLine = GetNextLineString();
}

//Покадровая проверка локацй
void Locations_TestAllProcess()
{
	int i = locations_TestCounter;
	locations_TestCounter = locations_TestCounter + 1;
	if(i >= nLocationsNum)
	{
		DelEventHandler("frame","Locations_TestAllProcess");
		Trace("");
		Trace("");
		Trace("");
		Trace("");
		Trace("======================================================================");
		Trace("");
		if(locations_TestBad > 0)
		{
			Trace("Finded bad locations: " + locations_TestBad);
		}else{
			Trace("No bad locations.");
		}
		Trace("");
		Trace("======================================================================");
		return;
	}
	if(Locations[i].id == "none") return;
	locations_TestIsBad = false;
	locations_TestIsWarn = false;
	bool res = Locations_TestLocation(&Locations[i], true);
	DeleteClass(&Locations[i]);
	string resStr;
	if(locations_TestIsBad) res = false;
	if(!res)
	{
		Trace("");
		Trace("");
		resStr = locations_TestResult;
//		resStr = "";
		Trace(resStr);
		Trace("");
		Trace("");
		locations_TestBad = locations_TestBad + 1;
	}else{
		if(locations_TestIsWarn)
		{
			Trace("");
			Trace("");
			resStr = locations_TestResult;
//			resStr = "";
			Trace(resStr);
			Trace("");
			Trace("");
		}
	}
	locations_TestIsBad = false;
	locations_TestIsWarn = false;
	res = Locations_TestLocation(&Locations[i], false);
	DeleteClass(&Locations[i]);
	if(locations_TestIsBad) res = false;
	if(!res)
	{
		Trace("");
		Trace("");
//		resStr = locations_TestResult;
//		Trace(resStr);
		Trace("");
		Trace("");
		locations_TestBad = locations_TestBad + 1;
	}else{
		if(locations_TestIsWarn)
		{
			Trace("");
			Trace("");
//			resStr = locations_TestResult;
//			Trace(resStr);
			Trace("");
			Trace("");
		}
	}
}

//Загрузить геометрию локации для теста и проверить
bool Locations_TestLocation(ref loc, bool isDayTest)
{
	locations_TestResult = "";
	ReloadProgressUpdate();
	bool isOk = true;
	Location_TestTrace("======================================================================");
	Location_TestTrace("Analise location: " + loc.id);
	Location_TestTrace("index: " + loc.index);
	Location_TestTrace("type: " + loc.type);
	if(isDayTest)
	{
		Location_TestTrace("Day test");
	}else{
		Location_TestTrace("Night test");
	}
	Location_TestTrace("======================================================================");
	//Create location======================================================================
	if(CreateEntity(&loc, "location") == 0) return 0;
	//Set models path
	if(CheckAttribute(loc, "filespath.models"))
	{		
		SendMessage(loc, "ls", MSG_LOCATION_MODELSPATH, loc.filespath.models);
	}
	//Set textures path
	if(CheckAttribute(loc, "filespath.textures"))
	{
		SendMessage(loc, "ls", MSG_LOCATION_TEXTURESPATH, loc.filespath.textures);
	}
	//Set lighting path
	SendMessage(loc, "ls", MSG_LOCATION_LIGHTPATH, GetLightingPath());
	SendMessage(loc, "ls", MSG_LOCATION_SHADOWPATH, GetLmLightingPath());
	ReloadProgressUpdate();
	//Loading always models================================================================
	if(!CheckAttribute(loc, "environment.sea"))
	{
		Location_TestTrace("Missed attribute: loc.environment.sea");
	}
	if(!CheckAttribute(loc, "environment.weather"))
	{
		Location_TestTrace("Missed attribute: loc.environment.weather");
	}
	//Loading always models================================================================
	aref st, at, lit, lit1;
	int res;
	string sat;
	string ssat;
	makearef(st, loc.models.always);
	int num = GetAttributesNum(st);
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(st, i);
		sat = GetAttributeName(at);
		if(sat != "grassPatch")
		{
			sat = "models.always." + sat;
			res = Locations_TestLoadModel(loc, sat, "");
			if(res == 0)
			{
				Location_TestTrace("Model not loaded in location: loc." + sat + " = " + loc.(sat) + "; id = " + loc.id);
				isOk = false;
			}
		}else{
			res = LocLoadGrass(loc, "models.always." + sat);
			if(res == 0)
			{
				ssat = "models.always." + sat;
				Location_TestTrace("Not loaded grass patch in location: loc.models.always." + sat + " = " + loc.(ssat) + "; id = " + loc.id);
				isOk = false;
			}
		}
	}
	ReloadProgressUpdate();
	//Loading day/night models=============================================================
	if(isDayTest)
	{
		//Day
		makearef(st, loc.models.day);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			sat = GetAttributeName(at);
			if((sat != "charactersPatch") && (sat != "jumpPatch") && (sat != "lights"))
			{
				sat = "models.day." + sat;
				res = Locations_TestLoadModel(loc, sat, "");
				if(res == 0)
				{
					Location_TestTrace("Day model not loaded in location: loc." + sat + " = " + loc.(sat) + "; id = " + loc.id);
					isOk = false;
				}
			}
		}
		//Loading patches
		if(CheckAttribute(loc, "models.day.charactersPatch") != 0)
		{
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_CHRS_PATCH, loc.models.day.charactersPatch);
			if(res == 0)
			{
				Location_TestTrace("");
				Location_TestTrace("Day character patch not loaded! models.day.charactersPatch = " + loc.models.day.charactersPatch);
				Location_TestTrace("");
				return 0;
			}
		}else{
			Location_TestTrace("");
			Location_TestTrace("Day character patch not setting for location.models.day.charactersPatch!");
			Location_TestTrace("");
			return 0;
		}
		if(CheckAttribute(loc, "models.day.jumpPatch") != 0)
		{
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_JMP_PATCH, loc.models.day.jumpPatch);
			if(res == 0)
			{
				Location_TestTrace("Day jump patch not loaded in location: loc.models.day.jumpPatch = " + loc.models.day.jumpPatch);
				isOk = false;
			}
		}
	}else{
		//Night
		makearef(st, loc.models.night);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);
			sat = GetAttributeName(at);
			if((sat != "charactersPatch") && (sat != "jumpPatch") && (sat != "lights"))
			{
				sat = "models.night." + sat;
				res = Locations_TestLoadModel(loc, sat, "");
				if(res == 0)
				{
					Location_TestTrace("Night model not loaded in location: loc." + sat + " = " + loc.(sat) + "; id = " + loc.id);
					isOk = false;
				}
			}
		}		
		//Loading patches
		if(CheckAttribute(loc, "models.night.charactersPatch") != 0)
		{
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_CHRS_PATCH, loc.models.night.charactersPatch);
			if(res == 0)
			{
				Location_TestTrace("");
				Location_TestTrace("Night character patch not loaded! models.night.charactersPatch = " + loc.models.night.charactersPatch);
				Location_TestTrace("");
				return 0;
			}
		}else{
			Location_TestTrace("");
			Location_TestTrace("Night character patch not setting for location.models.night.charactersPatch!");
			Location_TestTrace("");
			return 0;
		}
		if(CheckAttribute(loc, "models.night.jumpPatch") != 0)
		{
			res = SendMessage(loc, "ls", MSG_LOCATION_SET_JMP_PATCH, loc.models.night.jumpPatch);
			if(res == 0)
			{
				Location_TestTrace("Night jump patch not loaded in location: loc.models.night.jumpPatch = " + loc.models.night.jumpPatch);
				isOk = false;
			}
		}
	}
	ReloadProgressUpdate();
	//Locators=============================================================================
	//Locator's radiuses
	int j, k, gnum, lnum;
	aref locator_rad;
	if(CheckAttribute(loc, "locators_radius") != 0)
	{
		makearef(locator_rad, loc.locators_radius);
		gnum = GetAttributesNum(locator_rad);
		for(j = 0; j < gnum; j++)
		{
			//Group radius
			aref rdgrp = GetAttributeN(locator_rad, j);
			string rdgname = GetAttributeName(rdgrp);			
			float rad = MakeFloat("" + rdgrp);
			SetLocatorGroupRadius(loc, rdgname, rad);
			//Some locators radius
			lnum = GetAttributesNum(rdgrp);
			for(k = 0; k < lnum; k++)
			{
				aref rdloc = GetAttributeN(rdgrp, k);
				string rdlname = GetAttributeName(rdloc);
				rad = MakeFloat("" + rdloc);
				SetLocatorRadius(loc, rdgname, rdlname, rad);
			}
		}
	}
	ReloadProgressUpdate();
	//Updatelocators=======================================================================
	SendMessage(loc, "l", MSG_LOCATION_UPDATELOCATORS);
	Location_TestTrace("----------------------------------------------------------------------------");
	if(CheckAttribute(loc, "boarding"))
	{
		ReloadProgressUpdate();
		SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "rld");
	}else{
		ReloadProgressUpdate();
		if(CheckAttribute(loc, "locators.reload"))
		{
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "reload");
		}
		ReloadProgressUpdate();
		if(CheckAttribute(loc, "locators.goto"))
		{
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "goto");
		}
		ReloadProgressUpdate();
		if(CheckAttribute(loc, "locators.encdetector"))
		{
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "encdetector");
			makearef(st, loc.locators.encdetector);
			num = GetAttributesNum(st);
			for(i = 0; i < num; i++)
			{
				at = GetAttributeN(st, i);
				sat = GetAttributeName(at);
				SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", sat);
			}
		}
		if(CheckAttribute(loc, "locators.smugglers"))
		{
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "smugglers");
		}		
		if(CheckAttribute(loc, "locators.officers"))
		{
			//Проверяем локаторы для офицеров
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "officers");
			makearef(st, loc.locators.reload);
			num = GetAttributesNum(st);
			for(i = 0; i < num; i++)
			{
				at = GetAttributeN(st, i);
				sat = "locators.officers." + GetAttributeName(at);
				res = 0;
				if(CheckAttribute(loc, sat + "_1")) res = res + 1;
				if(CheckAttribute(loc, sat + "_2")) res = res + 1;
				if(CheckAttribute(loc, sat + "_3")) res = res + 1;
				if(res > 0)
				{
					if(res != 3)
					{
						if(!CheckAttribute(loc, sat + "_1"))
						{
							Location_TestTrace("Error: reload locator don't have an officer locator: officers::" + sat + "_1");
						}
						if(!CheckAttribute(loc, sat + "_2"))
						{
							Location_TestTrace("Error: reload locator don't have an officer locator: officers::" + sat + "_2");
						}
						if(!CheckAttribute(loc, sat + "_3"))
						{
							Location_TestTrace("Error: reload locator don't have an officer locator: officers::" + sat + "_3");
						}
					}
				}else{
					Location_TestTrace("Warning: locator reload::" + GetAttributeName(at) + " don't have an officers locators.");
					locations_TestWarnCounter = locations_TestWarnCounter + 1;
					locations_TestIsWarn = true;
				}
			}
		}
		ReloadProgressUpdate();
		if(loc.type == "tavern")
		{
			ReloadProgressUpdate();
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "waitress");
			ReloadProgressUpdate();
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "tables");
			ReloadProgressUpdate();
			SendMessage(loc, "lss", MSG_LOCATION_EX_MSG, "TestLocatorsGroup", "barmen");
			ReloadProgressUpdate();
		}
	}
	return isOk;
}

bool Locations_TestLoadModel(aref loc, string sat, string addition)
{
	//Пропустим пустое имя
	if(loc.(sat) == "") return false;
	//Считываем параметры модельки
	string attr, attr1;
	bool res;
	string tech = "";
	int level = 10;
	attr = sat + ".tech";
	if(CheckAttribute(loc, attr)) tech = loc.(attr);
	attr = sat + ".level";
	if(CheckAttribute(loc, attr)) level = MakeInt(loc.(attr));
	//Грузим модельку
	res = SendMessage(loc, "lssl", MSG_LOCATION_ADD_MODEL, loc.(sat) + addition, tech, level);
	if(res == 0) return false;
	return true;
}


#event_handler("LocatorsEventTrace", "LocationLocatorsTrace");
void LocationLocatorsTrace()
{
	int isError = GetEventData();
	string s = GetEventData();
	string group = GetEventData();
	string locator = GetEventData();
	if(group == "reload")
	{
		if(locator == "boat") return;
	}
	if(isError)
	{
		Location_TestTrace(s);
		locations_TestIsBad = true;
	}else{
		if(group == "reload") return;
		Location_TestTrace(s);
		locations_TestWarnCounter = locations_TestWarnCounter + 1;
		locations_TestIsWarn = true;
	}
}


void Location_TestTrace(string outStr)
{
	locations_TestResult = locations_TestResult + outStr + locations_TestNewLine;
}


