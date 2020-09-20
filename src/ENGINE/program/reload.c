
#define RELOAD_TIME_FADE_OUT	0.5
#define RELOAD_TIME_FADE_IN		0.5

object reload_fader;
aref reload_locator_ref;
int reload_cur_island_index, reload_cur_location_index;
int reload_island_index, reload_location_index;

object reload_xaddress;

bool LoadMainCharacterInFirstLocation(string location_id, string emerge_locator, string ship_location)
{
	int lindex;
	//Find ship location
	if(ship_location != "")
	{
		lindex = FindLocation(ship_location);
		if(lindex < 0)
		{
			Trace("Incorrect location id for location character's ship (location " + ship_location + " not found");
		}
	}
	if(lindex >= 0)
	{
		SetPlayerShipLocation(lindex);
	}
	//Find location
	lindex = FindLocation(location_id);
	if(lindex < 0)
	{
		Trace("Incorrect location id for first location (location " + location_id + " not found");
		return false;
	}
	//Trace("!!! Reload to " + lindex);
	//Find reload data
	aref reload_group;
	aref locator_ref;
	makearef(reload_group, Locations[lindex].reload);
	
	int num = GetAttributesNum(reload_group);
	if(num == 0)
	{
		Trace("Location " + location_id + " not have reload groups!");
		return false;
	}
	if(emerge_locator != "")
	{
		for(int i = 0; i < num; i++)
		{
            locator_ref = GetAttributeN(reload_group, i);
			if(emerge_locator == locator_ref.name) break;
		}
		if(i == num)
		{
			Trace("Emerge locator " + emerge_locator + " in location " + location_id + " not found!");
			locator_ref = GetAttributeN(reload_group, 0);
			emerge_locator = locator_ref.name;
			Trace("Character emerge on first locator (" + emerge_locator + ")");
		}
	}else{
		locator_ref = GetAttributeN(reload_group, 0);
		emerge_locator = locator_ref.name;
	}
    //  Eddy -->
	if (findsubstr(emerge_locator, "_back" , 0) != -1)
	{
        emerge_locator = strcut(emerge_locator, 0, strlen(emerge_locator)-6);
	}
    // <--  Eddy
	lockedReloadLocator = emerge_locator;
	ref mc = GetMainCharacter();
	mc.location = Locations[lindex].id;
	mc.location.group = "reload";
	mc.location.locator = emerge_locator;
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	ReloadProgressUpdate();
	LoadLocation(&Locations[lindex]);
	//Fader
	PostEvent("LoadSceneSound", 500);
	return true;
}

// boal 03.10.06 блок предыстории
bool LoadMainCharacterInFirstLocationGroup(string location_id, string sGroup, string emerge_locator)
{
	int lindex;
	
	//Find location
	lindex = FindLocation(location_id);
	if(lindex < 0)
	{
		Trace("Incorrect location id for first location (location " + location_id + " not found");
		return false;
	}

	lockedReloadLocator = emerge_locator;
	ref mc = GetMainCharacter();
	mc.location         = location_id;
	mc.location.group   = sGroup;
	mc.location.locator = emerge_locator;
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	ReloadProgressUpdate();
	LoadLocation(&Locations[lindex]);
	//Fader
	PostEvent("LoadSceneSound", 500);
	return true;
}

void SetPlayerShipLocation(int location_index)
{
	//Main character
	if(location_index < 0)
	{
		Trace("SetPlayerShipLocation - incorrect location index");
		return;
	}
	ref mc = GetMainCharacter();	
	mc.location.from_sea = Locations[location_index].id;
}


//reload_group = xxx.reload
int Reload(aref reload_group, string locator_name, string current_location)
{
	//EmptySelectedTutorials();
	SetReloadNextTipsImage();
	dialogDisable = true;
	reload_xaddress.active = "false";
	//Trace("locator_name = " + locator_name + " lockedReloadLocator = " + lockedReloadLocator);
	//Check locked locator
	if(locator_name == lockedReloadLocator)
	{
		lockedReloadLocator = "";
		return -1;
	}
	//Find reload data
	int num = GetAttributesNum(reload_group);
	if(num == 0) return 0;
	for(int i = 0; i < num; i++)
	{
		reload_locator_ref = GetAttributeN(reload_group, i);
		if(locator_name == reload_locator_ref.name) break;
	}
	if(i == num)
	{
		Trace("Can't reload...");
		Trace("Reload locator '" + locator_name + "' in currend location not found!");
		return 0;
	}
	//Check main attributes
	if(CheckAttribute(reload_locator_ref, "name") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'name' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "go") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'go' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "emerge") == 0)
	{
		Trace("Can't reload...");
		Trace("Reload: attribute 'emerge' in " + GetAttributeName(reload_locator_ref) + " not accessible");
		return 0;
	}
	//Find current location
	reload_cur_island_index = -1;
	reload_cur_location_index = -1;
	if(current_location != "")
	{		
		reload_cur_island_index = FindIsland(current_location);
		if(reload_cur_island_index < 0)
		{
			reload_cur_location_index = FindLocation(current_location);
			if(reload_cur_location_index < 0)
			{
				Trace("Can't unload currend location...");
				Trace("Current location not found: id = " + current_location);
			}
		}
	}
	//Find location & reload
	reload_island_index = -1;
	reload_location_index = -1;
	reload_island_index = FindIsland(reload_locator_ref.go);
	if(reload_island_index < 0)
	{
		reload_location_index = FindLocation(reload_locator_ref.go);
		if(reload_location_index < 0)
		{
			Trace("Can't reload...");
			Trace("Location not found: id = " + reload_locator_ref.go);
			return 0;
		}
	}
	
	//Main character
	ref mc = GetMainCharacter();
	//Trace("reload_cur_island_index = " + reload_cur_island_index);
	//Trace("reload_cur_location_index = " + reload_cur_location_index);
	//Trace("reload_island_index = " + reload_island_index);
	//Trace("reload_location_index = " + reload_location_index);
	//Test in sea exit

	if((reload_cur_island_index < 0) && (reload_island_index >= 0))
	{
		if(reload_cur_location_index >= 0)
		{
			//From location to sea
			if(Locations[reload_cur_location_index].id != mc.location.from_sea)
			{
				Trace("Not reload to sea, no ships: cur:" + Locations[reload_cur_location_index].id + " from:" + mc.location.from_sea);
				return -1;
			}
		}
	}
	
	//Start fader
	SetEventHandler("FaderEvent_StartFade", "ReloadStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "ReloadEndFade", 0);
	CreateEntity(&reload_fader, "fader");
	if(reload_island_index >= 0)
	{
		if(!CheckAttribute(mc, "todeck"))
		{
		SendMessage(&reload_fader, "ls",FADER_PICTURE,"loading\sea.tga");
	}
	else
	{
			SendMessage(&reload_fader, "ls",FADER_PICTURE,"loading\cabine.tga");
		}
	}
	else
	{
		int loc_pict_index = reload_location_index;
		if(loc_pict_index < 0) loc_pict_index = reload_cur_location_index;
		if(loc_pict_index >= 0)
		{		
			if(CheckAttribute(&Locations[loc_pict_index],"image"))
			{
				SendMessage(&reload_fader, "ls", FADER_PICTURE, Locations[loc_pict_index].image);
			}
		}
	}		

	bool IsMakeShot = true;
	float fadeOutTime = RELOAD_TIME_FADE_OUT;
	if((reload_cur_island_index < 0) && (reload_cur_location_index < 0))
	{
		fadeOutTime = 0.00001;	//Can't need fade out - no loaded location
		IsMakeShot = false;
	}
	SendMessage(&reload_fader, "lfl", FADER_OUT, fadeOutTime, false);
	if(IsMakeShot == true) SendMessage(&reload_fader, "l", FADER_STARTFRAME);
	//Set lockedReloadLocator
	if(reload_cur_island_index < 0)
	{
		//From location
		if(reload_island_index < 0)
		{		//To location
			lockedReloadLocator = reload_locator_ref.emerge;
		}
		else
		{	//To sea
			lockedReloadLocator = "";
		}
	}
	else
	{
		//From sea
		if(reload_island_index < 0)
		{		//To location
			lockedReloadLocator = reload_locator_ref.emerge;
		}
		else
		{	//To sea			
			lockedReloadLocator = "";
		}
	}
	//Trace("Start reload");
	return 1;
}

void ReloadStartFade()
{
	//ResetSoundScheme();
	ResetSound(); // new
	PauseAllSounds();
	DelEventHandler("FaderEvent_StartFade", "ReloadStartFade");
	//Trace("ReloadStartFade");
	//Main character
	ref mc = GetMainCharacter();
	if(reload_cur_island_index < 0)
	{
		//From location
		if(reload_island_index < 0)
		{
			//To location
			if(reload_cur_location_index >= 0) UnloadLocation(&Locations[reload_cur_location_index]);
			return;
		}else{
			//To sea
			if(reload_cur_location_index >= 0)
			{
				UnloadLocation(&Locations[reload_cur_location_index]);
			}else mc.location.from_sea = "";			
			DeleteClass(&chrAnimationKipper);
			return;
		}
	}else{
		//From sea
		if(reload_island_index < 0)
		{
			//To location
			DeleteSeaEnvironment();
			mc.location.from_sea = reload_locator_ref.go;
			return;
		}else{
			//To sea
			DeleteSeaEnvironment();
			mc.location.from_sea = "";
			return;
		}
	}
}

void ReloadEndFade()
{
    EmptyAllFantomCharacter(); // fix место тут!!!! а не выше, вот вам и баги по квестам, блин boal
    PGG_DailyUpdate();
    Siege_DailyUpdate();//homo осады 05/11/06
    DetectBlades();
	dialogDisable = false;
	ReloadProgressStart();
	DelEventHandler("FaderEvent_EndFade", "ReloadEndFade");
	//Trace("ReloadEndFade");
	ReloadProgressUpdate();
	//Main character
	ref mc = GetMainCharacter();
	int iLoc;
	if(reload_cur_island_index < 0)
	{
		//From location
		if(reload_island_index < 0)
		{
			//To location
			ReloadToLocation(reload_location_index, reload_locator_ref);
		}
		else
		{
			//To sea
			ReloadToSea(reload_island_index, reload_locator_ref);
		}
	}
	else
	{
		//From sea
		if(reload_island_index < 0)
		{
			//To location
			ReloadToLocation(reload_location_index, reload_locator_ref);
		}
		else
		{
			//To sea
			ReloadToSea(reload_island_index, reload_locator_ref);
		}
	}
	SendMessage(&reload_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
	PostEvent("LoadSceneSound", 500);
	ReloadProgressEnd();
}

string FindEmergeLocator(ref rObject, string emerge_str)
{
	aref arReload;
	makearef(arReload,rObject.reload);
	int n = GetAttributesNum(arReload);
	for (int i=0;i<n;i++)
	{
		aref arLoc;
		arLoc = GetAttributeN(arReload,i);
		if (arLoc.name == emerge_str) 
		{
			string loc_name = GetAttributeName(arLoc);
			return loc_name;
		}
	}
 	Trace("ERROR(no find emerge string): string FindEmergeLocator(ref rObject, string emerge_str) : " + emerge_str);
	return "";
}

object chrAnimationKipper;

int ReloadToLocation(int location_index, aref reload_data)
{
	ref mc = GetMainCharacter();
	// boal переход в зашлушки, ставим дверь обратно-->
	if (CheckAttribute(&Locations[location_index], "MustSetReloadBack"))
	{
	    //reload_data.name = "reload3_back";  - откуда идем
		//reload_data.go = "Antigua_townhall";  - куда идем
		//reload_data.emerge = "reload1";  - локатор куда он же выход обратно
		// reload_data.FromSea = true; - есть только на острове, ведет к пляжу заглушке
		aref	arReload, arLocator;

		makearef(arReload, Locations[location_index].reload);
		int		iNumReload = GetAttributesNum(arReload);
		int i;
		if (!CheckAttribute(reload_data, "FromSea"))
		{   // переход суша-суша
			for (i=0; i<iNumReload; i++)
			{
				arLocator = GetAttributeN(arReload, i);
				if (arLocator.Name == reload_data.emerge)
				{
	                arLocator.go     = mc.location; // где был
	                arLocator.emerge = reload_data.name; // откуда шагнул
	                if (CheckAttribute(&Locations[reload_cur_location_index], "fastreload"))
	                {
	                    Locations[location_index].fastreload = Locations[reload_cur_location_index].fastreload;
	                }
	                else
	                {
	                    DeleteAttribute(&Locations[location_index], "fastreload");
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "id.label"))
	                {
	                	arLocator.label  = Locations[reload_cur_location_index].id.label; // как завать, куда вернуться
	                }
	                else
	                {
	                    arLocator.label  = "";
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "islandId"))
	                {
	                    Locations[location_index].islandId = Locations[reload_cur_location_index].islandId;
	                }
	                else
	                {
	                    DeleteAttribute(&Locations[location_index], "islandId");
	                }
	                if (CheckAttribute(&Locations[reload_cur_location_index], "id.label") && locations[reload_cur_location_index].type != "town")
	                {
	                    Locations[location_index].id.label = Locations[reload_cur_location_index].id.label;
	                }
					break;
				}
			}
		}
		else
		{ // переход море-суша
			// идея как работает
			// в Islands_init.c на локаторах входа в бухту-заглушку приписочка  .FromSea = true;
			// на суше в Locations\init\ на локаторах выхода в море приписочка  .ToSea = true;
			// на самой локации .MustSetReloadBack = true;
		    for (i=0; i<iNumReload; i++)
			{
				arLocator = GetAttributeN(arReload, i);
				if (CheckAttribute(arLocator, "ToSea")) // признак, что локатор выхода в море
				{
	                arLocator.go     = mc.location; // где был
	                arLocator.emerge = reload_data.name; // откуда шагнул
                    // метка == море всегда
				}
			}
		}
	}
	// boal <--
	mc.location = Locations[location_index].id;
	mc.location.group = "reload";
	mc.location.locator = reload_data.emerge;
	if(reload_xaddress.active == "true")
	{
		mc.location.group = reload_xaddress.group;
		mc.location.locator = reload_xaddress.locator;
	}
	if(IsEntity(&chrAnimationKipper) == false) CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");

    // eddy --> открыть/закрыть двери в заглушках городов.
    if (CheckNPCQuestDate(&Locations[location_index], "Common_date"))
    {
    	if (Locations[location_index].type == "town" && CheckAttribute(&locations[location_index], "houseEnc"))
    	{
            string  sName;
            aref	arDis, arRld;
    		makearef(arRld, Locations[location_index].reload);
    		int		Qty = GetAttributesNum(arRld);
            SetNPCQuestDate(&Locations[location_index], "Common_date");
    		for (int a=0; a<Qty; a++)
    		{
    			arDis = GetAttributeN(arRld, a);
                sName = arDis.go;
    			if (findsubstr(sName, "Common" , 0) != -1)
    			{
                    if (CheckAttribute(arDis, "canEnter"))
					{
						arDis.disable = false;
						DeleteAttribute(arDis, "canEnter"); //автосъем флага на открытую дверь
					}
					else arDis.disable = rand(1); 
    			}
    		}
    	}
    }
	//меняем радиусы детекторов энкаунтеров
	Locations[location_index].locators_radius.encdetector = 8 + rand(10);
    // <-- eddy
	return LoadLocation(&Locations[location_index]);
}

int ReloadToLocationEx(int location_index, string sLocator)
{
	ref mc = GetMainCharacter();
	mc.location = Locations[location_index].id;
	mc.location.group = "reload";
	mc.location.locator = sLocator;

	/*
	if(reload_xaddress.active == "true")
	{
		mc.location.group = reload_xaddress.group;
		mc.location.locator = reload_xaddress.locator;
	}
	*/
	
	if(IsEntity(&chrAnimationKipper) == false) 
	{
		CreateEntity(&chrAnimationKipper, "CharacterAnimationKipper");
	}
	return LoadLocation(&Locations[location_index]);
}

int ReloadToSea(int island_index, aref reload_data)
{
	
	object Login;

	ref rPlayer = GetMainCharacter();
	rPlayer.lastFightMode = 0;

	ref rIsland = GetIslandByIndex(island_index);
	/*Trace("============================");
	Trace("island_index = " + island_index);
	DumpAttributes(rIsland);
	Trace("============================");*/

	rPlayer.location = rIsland.id;
	rPlayer.location.group = "reload";
	rPlayer.location.locator = reload_data.emerge;

	Login.Island = rIsland.id;

	Login.Encounters = "";

	string sLoc = FindEmergeLocator(rIsland,reload_data.emerge);

	Login.PlayerGroup.x = rIsland.reload.(sLoc).ships.l0.x;	// FIX-ME
	Login.PlayerGroup.z = rIsland.reload.(sLoc).ships.l0.z; 
	Login.PlayerGroup.ay = rIsland.reload.(sLoc).ships.l0.ay;
	Login.FromCoast = true;

	Trace("Load from location to sea sLoc = " + sLoc + ", x = " + Login.PlayerGroup.x + ",z = " + Login.PlayerGroup.z);
		
	SeaLogin(Login);
	return 1;

	/*
	reload_data.x
	reload_data.y
	reload_data.z
	reload_data.ships.l1.x
	*/
	return -1;
}


bool TeleportCharacterFromCurLocationToLocation(string locatorExit, string group, string locator)
{
	reload_xaddress.group = group;
	reload_xaddress.locator = locator;
	aref reload_group;
	makearef(reload_group, loadedLocation.reload);
	if(Reload(reload_group, locatorExit, loadedLocation.id) == false) return false;
	reload_xaddress.active = "true";
	return true;
}

//boal -->
int BOAL_ReloadToLoc(aref reload_group, string locator_name)
{
	int num = GetAttributesNum(reload_group);
	if(num == 0) return 0;
	for(int i = 0; i < num; i++)
	{
		reload_locator_ref = GetAttributeN(reload_group, i);
		if(locator_name == reload_locator_ref.name) break;
	}
	if(i == num)
	{
		return 0;
	}
	//Check main attributes
	if(CheckAttribute(reload_locator_ref, "name") == 0)
	{
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "emerge") == 0)
	{
		return 0;
	}
	if(CheckAttribute(reload_locator_ref, "go") == 0)  // нужна только эта проверка
	{
		return 0;
	}
	//Main character
	Pchar.quest.SetNPCInShipDeck.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipDeck.win_condition.l1.location = "My_Deck";
	Pchar.quest.SetNPCInShipDeck.win_condition             = "SetNPCInShipDeck";
	
	Pchar.quest.SetNPCInShipCampus.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipCampus.win_condition.l1.location = "My_Campus";
	Pchar.quest.SetNPCInShipCampus.win_condition             = "SetNPCInShipDeck";

	Pchar.quest.SetNPCInShipCabin.win_condition.l1          = "location";
	Pchar.quest.SetNPCInShipCabin.win_condition.l1.location = Get_My_Cabin();
	Pchar.quest.SetNPCInShipCabin.win_condition             = "SetNPCInShipDeck";

	boarding_location  = FindLocation(reload_locator_ref.go); // че потом грохать

	//DoReloadCharacterToLocation(reload_locator_ref.go, "reload", reload_locator_ref.emerge);
	Reload(reload_group, locator_name, Pchar.location);

	return 1;
}
// boal <--