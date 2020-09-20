//«апретить сражатьс€ персонажам в заданной локации
void LAi_LocationFightDisable(ref location, bool isDisable)
{
	if(isDisable)
	{
		location.noFight = "1";
	}else{
		DeleteAttribute(location, "noFight");
	}
}

//–азрешить или запретить генерации монстров 
void LAi_LocationDisableMonstersGen(string locationId, bool isDisable)
{
	ref location = &locations[FindLocation(locationId)];
	if(isDisable)
	{
		location.monsters = "1";
	}else{
		DeleteAttribute(location, "monsters");
	}
}

//«апретить генерации монстров в локации locationId на Days дней
void LAi_LocationDisableMonGenTimer(string locationId, int Days)
{
	ref location = &locations[FindLocation(locationId)];
	SaveCurrentNpcQuestDateParam(location, "monstersTimer");
	location.monstersTimer = Days;
}

//”становить врем€ генерации монстров
void LAi_LocationSetMonstersTime(ref location, float start, float end)
{
	if(!CheckAttribute(location, "monsters"))
	{
		location.monsters = "0";
	}		
	location.monsters.stime = start;
	location.monsters.etime = end;	
}

bool LAi_LocationIsMonstersGen(ref location)
{
	//ѕроверим таймер на запрет генерации монстров
	if(CheckAttribute(location, "monstersTimer"))
	{
		if (GetNpcQuestPastDayParam(location, "monstersTimer") <= sti(location.monstersTimer))
			return false;
		else
			DeleteAttribute(location, "monstersTimer");
	}
	//ѕроверим разрешение на генерацию монстров
	if(!CheckAttribute(location, "monsters")) return true;
	//ѕроверим врем€ логина
	if(CheckAttribute(location, "monsters.stime"))
	{
		if(CheckAttribute(location, "monsters.etime"))
		{
			float start = stf(location.monsters.stime);
			float end = stf(location.monsters.etime);
			if(LAi_login_CheckTime(start, end)) return true;
		}
	}
	return false;
}

//–азрешить или запретить генерации фантомов при убийстве в данной локации
void LAi_LocationFantomsGen(ref location, bool isEnable)
{
	if(isEnable)
	{
		location.fantoms = "1";
	}else{
		DeleteAttribute(location, "fantoms");
	}
}

bool LAi_LocationIsFantomsGen(ref location)
{
	if(CheckAttribute(location, "fantoms") == false) return false;
	if(sti(location.fantoms) != true) return false;
	return true;
}

//«апретить генерацию офицеров в локации, или разрешить после запрещени€.
void LAi_LocationDisableOfficersGen(string locationId, bool isDisable)
{
	ref location = &locations[FindLocation(locationId)];
	if(isDisable)
	{
		location.DisableOfficers = "1";
	}else{
		DeleteAttribute(location, "DisableOfficers");
	}
}

//«апретить генерацию офицеров в локации locationId на Days дней.
void LAi_LocationDisableOffGenTimer(string locationId, int Days)
{
	ref location = &locations[FindLocation(locationId)];
	location.DisableOfficers = true;
	location.DisableOfficers.QtyDays = Days;
	SaveCurrentNpcQuestDateParam(location, "DisableOfficers");
}