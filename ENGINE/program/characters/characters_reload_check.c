

//Disable reload locator
bool chrEnableReloadLocator(string locationID, string reloadLocatorName, bool isEnable)
{
	Event("EnableReloadLocatorEvent", "ssl", locationID, reloadLocatorName, isEnable);
	//Location
	int li = FindLocation(locationID);
	if(li < 0) 
	{
		return false;
	}
	//Reload locator
	aref rl, at;
	makearef(rl, locations[li].reload);
	int disableVal = 0;
	int num = GetAttributesNum(rl);
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(rl, i);
		if(at.name == reloadLocatorName)
		{
			if(CheckAttribute(at,"disable")) 
			{
				disableVal = sti(at.disable);
			}
			if(isEnable) 
			{
				disableVal--; 
			}
			else 
			{
				disableVal++;
			}
			if(disableVal <= 0) 
			{
				DeleteAttribute(at,"disable"); 
			}
			else 
			{
				at.disable = disableVal;
			}
			return true;
		}
	}
	return false;
}


//locatorName
bool chrCheckReload(ref loc, string reloadLocatorName)
{
	//Reload locator
	aref rl, at;
	makearef(rl, loc.reload);
	int num = GetAttributesNum(rl);
	int disableVal = 0;
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(rl, i);
		if(at.name == reloadLocatorName)
		{
			if(CheckAttribute(at, "disable")) disableVal = sti(at.disable);
   			if(CheckAttribute(at, "close_for_night") && sti(at.close_for_night)>0 && !IsLocationCaptured(loc.id))//boal fix
			{
				if(GetTime()>21.0 || GetTime()<6.0) disableVal++;
			}
			return disableVal <= 0;
		}
	}
	return false;
}

//locatorName
bool chrCheckReloadLink(ref loc, string reloadLocatorName)
{
	//Reload locator
	aref rl, at;
	makearef(rl, loc.reload);
	int num = GetAttributesNum(rl);
	int disableVal = 0;
	ref mc = GetMainCharacter();
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(rl, i);
		if(at.name == reloadLocatorName)
		{
			if(FindLocation(at.go) > 0) 
			{
				// boal -->
				if (bLocatorShow)
				{
				    Log_Info("Reload to: " + at.go + " locator = " + at.emerge);
				}
				// boal <--
				return 1;
			}
			int iShip = sti(mc.ship.type);
			if(CheckAttribute(mc, "location.from_sea") && iShip != SHIP_NOTUSED)
			{
				if(mc.location.from_sea == mc.location) 
				{
					return 1;
				}
				return 2;
			}
			return 0;
		}
	}
	return 0;
}


//locatorName
bool chrCheckAutoReload(ref loc, string reloadLocatorName)
{
	//Reload locator
	aref rl, at;
	makearef(rl, loc.reload);
	int num = GetAttributesNum(rl);
	for(int i = 0; i < num; i++)
	{
		at = GetAttributeN(rl, i);
		if(at.name == reloadLocatorName)
		{
			if(CheckAttribute(at, "autoreload") == 0) return false;
			return MakeInt(at.autoreload) != 0;
		}
	}
	return false;
}













