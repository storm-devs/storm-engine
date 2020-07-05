
int CreateBermudesCharacters(int n)
{
	ref ch;
	
	///////////////////////////////////////////////////////////////////////////////
	// ОСНОВНЫЕ ПЕРСОНАЖИ
	///////////////////////////////////////////////////////////////////////////////

	makeref(ch,Characters[n]);			//Глава пиратов
	ch.id		= "Jackman";
	ch.name 	= "Джекмен";
	ch.rank = 30;
    ch.lastname = "";
	ch.model	= "capitan";
	ch.sex = "man";
	ch.model.animation = "man";
	ch.City = "Pirates";
	ch.location	= "Pirates_townhall";
	ch.location.group = "sit";
	ch.location.locator = "sit1";
	ch.Dialog.Filename = "Mayor\Jackman.c";
	ch.nation = PIRATE;
	ch.quest.type = "hovernor";
	ch.reputation = "35";
    SetRandSPECIAL(ch);
    SetSelfSkill(ch, 90, 90, 90, 70, 90);
    SetShipSkill(ch, 90, 80, 70, 10, 10, 20, 10, 10, 50);
	GiveItem2Character(ch, GUN_COMMON);
	ch.equip.gun = GUN_COMMON;
	GiveItem2Character(ch, BLADE_LONG);
	ch.equip.blade = BLADE_LONG;
	ch.Ship.Name = "Манчестер";
	ch.Ship.Type = GenerateShip(SHIP_CORVETTE, true);
	ch.Ship.Stopped = true;
   	ch.Ship.Cannons.Type = CANNON_TYPE_CULVERINE_LBS24;
    SetCrewQuantityFull(ch);
    Fantom_SetBalls(ch, "pirate");    
	ch.rank 	= 20;
	ch.reputation = "25";
	ch.money = "25000";
	ch.greeting = "pirat_common";
	SetCharacterPerk(ch, "FastReload");
	SetCharacterPerk(ch, "HullDamageUp");
	SetCharacterPerk(ch, "SailsDamageUp");
	SetCharacterPerk(ch, "CrewDamageUp");
	SetCharacterPerk(ch, "CriticalShoot");
	SetCharacterPerk(ch, "LongRangeShoot");
	SetCharacterPerk(ch, "CannonProfessional");
	SetCharacterPerk(ch, "ShipDefenseProfessional");
	SetCharacterPerk(ch, "ShipSpeedUp");
	SetCharacterPerk(ch, "ShipTurnRateUp");
	SetCharacterPerk(ch, "SwordplayProfessional");
	SetCharacterPerk(ch, "AdvancedDefense");
	SetCharacterPerk(ch, "CriticalHit");
	SetCharacterPerk(ch, "Sliding");
    SetSelfSkill(ch, 90, 90, 90, 60, 70);
    SetShipSkill(ch, 100, 10, 100, 100, 90, 60, 80, 90, 50);
	LAi_NoRebirthDisable(ch);
	LAi_RebirthOldName(ch);
	LAi_SetHuberType(ch);
    LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetLoginTime(ch, 0.0, 24.0);
	LAi_SetHP(ch, 200.0, 200.0);
	LAi_SetImmortal(ch, true);
	n = n + 1;

	makeref(ch,Characters[n]);		// ОФициантка
	ch.id		= "Pirates_waitress";
	ch.model	= "girl_3";
	ch.sex = "woman";
	ch.City = "Pirates";
	ch.location	= "Pirates_tavern";
	ch.location.group = "waitress";
	ch.location.locator = "barmen";
	ch.Dialog.Filename = "Waitress_dialog.c";
	ch.greeting = "Gr_officiant";
	ch.nation = PIRATE;
	LAi_SetWaitressType(ch);
	LAi_RemoveLoginTime(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetImmortal(ch, true);
	n = n + 1;

	makeref(ch,Characters[n]);			//Трактирщик
	ch.id		= "Pirates_tavernkeeper";
	ch.model	= "pirate_2";
	ch.greeting = "Gr_Barmen";
	ch.sex = "man";
	ch.City = "Pirates";
	ch.location	= "Pirates_Tavern";
	ch.location.group = "barmen";
	ch.location.locator = "stay";
	ch.Dialog.Filename = "Common_Tavern.c";
	ch.TavernMan = true;
	ch.nation = PIRATE;
	LAi_SetBarmanType(ch);
	LAi_RemoveLoginTime(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetImmortal(ch, true);
	n = n + 1;

	makeref(ch,Characters[n]);			//Торговец
	ch.id		= "Pirates_trader";
	ch.name		= "Паскаль";
	ch.lastname = "Вуазье";
	ch.model	= "trader_1";
	ch.greeting = "tra_common";
	ch.sex = "man";
	ch.City = "Pirates";
	ch.location	= "Pirates_Store";
	ch.location.group = "barmen";
	ch.location.locator = "stay";
	ch.Dialog.Filename = "Common_Store.c";
	ch.quest.type = "trader";
	ch.nation = PIRATE;
	LAi_SetBarmanType(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetImmortal(ch, true);
	ch.item_date = "";
	// настройка магазина -->
	ch.StoreNum   = Pirates_STORE;
	ch.from_sea   = "Pirates_town";
	// настройка магазина <--
	n = n + 1;

	makeref(ch,Characters[n]);			//Корабел
	ch.id		= "Pirates_shipyarder";
	ch.model	= "trader_15";
	ch.greeting = "Gr_shipyarder";
	ch.name 	= "Мастер";
    ch.lastname = "Алексус";
	ch.sex = "man";
	ch.City = "Pirates";
	ch.location	= "Pirates_Shipyard";
	ch.location.group = "sit";
	ch.location.locator = "sit1";
	ch.Dialog.Filename = "Common_Shipyard.c";
	ch.nation = PIRATE;
	ch.quest.shiping = "0";
	LAi_SetHuberType(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetImmortal(ch, true);
	n = n + 1;

	return n;
}
