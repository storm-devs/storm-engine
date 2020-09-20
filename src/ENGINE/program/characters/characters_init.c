
// Set characters initial data
void CreateCharacters()
{
 	ref ch;
	int n;

	//InitBio();
	

	for(n=0; n<TOTAL_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
  		InitCharacter(ch, n);
	}

	ReloadProgressUpdate();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ПЕРВЫЙ ПЕРСОНАЖ - ПАРЕНЬ
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	makeref(ch,Characters[1]);		//Blaze Sharp
	ch.name 	= "Блэйз";
	ch.lastname = "Шарп";
	ch.id		= "Blaze";
	ch.nation	= ENGLAND;
	ch.model	= "devlin";
	ch.sex = "man";
	ch.location	= "";
	ch.location.group = "";
	ch.location.locator = "";
	ch.Dialog.Filename = "MainHero_dialog.c";
	ch.rank 	= 1;
	ch.reputation = REPUTATION_NEUTRAL;
	ch.experience = 0;

	ch.skill.freeskill = 0;// нет, все сам
	ch.perks.freepoints = 0;
	ch.money = "0";
	ch.timescale = 1;
	ch.pause = 0;
	ch.shotgun = "0";
	ch.Ship.Type = GenerateShip(SHIP_LUGGER, 0); // переопределяется ниже по коду от выбора гг
	ch.Ship.Name = "Быстрый вепрь";
	ch.Ship.Stopped = true;
	ch.FaceId = 1;
	ch.ShipSlot1.Type=SHIP_NOTUSED;
	ch.ShipSlot1.Name="NoName";
	ch.ShipSlot2.Type=SHIP_NOTUSED;
	ch.ShipSlot2.Name="NoName";
	ch.ShipSlot3.Type=SHIP_NOTUSED;
	ch.ShipSlot3.Name="NoName";
	ch.paymenttype = 50;  // to_do del
	ch.repair = "0";
	//ch.stealmoney = 0;
	//ch.month.money = "0";
	ch.colony_quantity = "0";
	ch.quest.work = "0";
	ch.quest.DeliveryTradeWork = "0";
	ch.quest.free_adventure.character_0 = "";
	ch.quest.free_adventure.character_1 = "";
	ch.quest.free_adventure.character_2 = "";
	//GiveItem2Character(ch, "blade1");
	//EquipCharacterByItem(ch, "blade1");
	//GiveItem2Character(ch, "pistol1");
	//EquipCharacterByItem(ch, "pistol1");
	//TakeNItems(ch, "bullet", 5+rand(15));// boal gun bullet
	//GiveItem2Character(ch, "spyglass1");
	//EquipCharacterByItem(ch, "spyglass1");
	//TakeNItems(ch, "potion1", 3);
	//ch.quest.OfficerPrice = 0;

	ch.quest.RelationAgentMet = "0";
	ch.quest.gambling = "0";
	ch.quest.friend_in_tavern = "";
	ch.time_events_counter = 0;
	ch.rumourquest = 0;
	ch.abordage = 0;
	ch.isSmuggPossible = 1;
	
	// Награда за голову -->
    ch.reputation.Enghunter = "-5";
    ch.reputation.Frahunter = "-5";
    ch.reputation.Spahunter = "-5";
    ch.reputation.Holhunter = "-5";
    // Награда за голову <--
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ОСТАЛЬНЫЕ ПЕРСОНАЖИ
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ReloadProgressUpdate();

	n = 2;
	n = CreateAntiguaCharacters(n);
	ReloadProgressUpdate();
	Trace("Antigua: " + n);
	
	n = CreateBarbadosCharacters(n);
	ReloadProgressUpdate();
	Trace("Barbados: " + n);

	n = CreateJamaicaCharacters(n);
	ReloadProgressUpdate();
	Trace("Jamaica: " + n);
	
	n = CreateCuracaoCharacters(n);
	ReloadProgressUpdate();
	Trace("Curacao: " + n);
	
	n = CreateGuadeloupeCharacters(n);
	ReloadProgressUpdate();
	Trace("Guadeloupe: " + n);
	
	n = CreateSantiagoCharacters(n);
	ReloadProgressUpdate();
	Trace("Santiago: " + n);

	n = CreatePuertoPrincipeCharacters(n);
	ReloadProgressUpdate();
	Trace("PuertoPrincipe: " + n);
	
	n = CreateMartiniqueCharacters(n);
	ReloadProgressUpdate();
	Trace("Martinique: " + n);
	
	n = CreateHavanaCharacters(n);
	ReloadProgressUpdate();
	Trace("Havana: " + n);
	
	n = CreateNevisCharacters(n);
	ReloadProgressUpdate();
	Trace("Nevis: " + n);
	
	n = CreatePuertoRicoCharacters(n);
	ReloadProgressUpdate();
	Trace("PuertoRico: " + n);
	
	n = CreateSentMartinCharacters(n);
	ReloadProgressUpdate();
	Trace("SentMartin: " + n);

	n = CreateBermudesCharacters(n);
	ReloadProgressUpdate();
	Trace("Bermudes: " + n);
	
	n = CreateTrinidadCharacters(n);
	ReloadProgressUpdate();
	Trace("Trinidad: " + n);
	
	n = CreatePortPaxCharacters(n);
	ReloadProgressUpdate();
	Trace("PortPax: " + n);
	
	n = CreateSantoDomingoCharacters(n);
	ReloadProgressUpdate();
	Trace("SantoDomingo: " + n);
	
	n = CreateLaVegaCharacters(n);
	ReloadProgressUpdate();
	Trace("LaVega: " + n);
	
	n = CreateTortugaCharacters(n);
	ReloadProgressUpdate();
	Trace("Tortuga: " + n);

	n = CreatePanamaCharacters(n);
	ReloadProgressUpdate();
	Trace("Panama: " + n);
		
	n = CreatePortoBelloCharacters(n);
	ReloadProgressUpdate();
	Trace("PortoBello: " + n);

	n = CreateCartahenaCharacters(n);
	ReloadProgressUpdate();
	Trace("Cartahena: " + n);

	n = CreateCaracasCharacters(n);
	ReloadProgressUpdate();
	Trace("Caracas: " + n);

	n = CreateCumanaCharacters(n);
	ReloadProgressUpdate();
	Trace("Cumana: " + n);

	n = CreateSantaCatalinaCharacters(n);
	ReloadProgressUpdate();
	Trace("SantaCatalina: " + n);

	n = CreateBelizCharacters(n);
	ReloadProgressUpdate();
	Trace("Beliz: " + n);

	n = CreateMaracaiboCharacters(n);
	ReloadProgressUpdate();
	Trace("Maracaibo: " + n);
	
	n = CreateStoryCharacters(n);
	ReloadProgressUpdate();
	Trace("Story: " + n);
	
	CreateOtherCharacters();
	globalCharacters = n; //eddy. если лодочник выведен из конкурса, то и инкремент не нужен. иначе болтается левый нпс
	MAX_CHARACTERS = globalCharacters;
	
    trace("ВСЕГО НПС В ИГРЕ (статиков) = " + MAX_CHARACTERS);
    trace("Version = " + GetVerNum());
	ReloadProgressUpdate();

	SetMainCharacterIndex(1); // boal fix

	//SetAllShipData();
	//SetAllFellows();

	//Post init
	for(n=0; n<TOTAL_CHARACTERS; n++) // 1
	{
		ref rCharacter = GetCharacter(n);

		if (rCharacter.model.animation == "")
        {
			if(rCharacter.sex == "man")
			{

	        		rCharacter.model.animation = "man";
	        		rCharacter.model.height = 1.8;

			}else{
				rCharacter.model.animation = "towngirl";
				rCharacter.model.height = 1.75;
			}
			if(rCharacter.sex == "skeleton")
			{
				rCharacter.model.animation = "man";
				rCharacter.model.height = 1.8;
			}
		}
		// boal -->
		if (CheckAttribute(rCharacter, "City"))
        {
			rCharacter.reputation = (1 + rand(44) + rand(44));// репа всем горожанам
        }
        if (CheckAttribute(rCharacter, "Dialog.Filename") && rCharacter.Dialog.Filename == "Common_Shipyard.c")
		{
			rCharacter.ShipCostRate = 0.8 + frnd()*0.4;
		}
        // boal <--
		rCharacter.FaceGroup = 0;

		// set fellows
		if (CheckAttribute(rCharacter,"fellows")) { SetBaseFellows(rCharacter); }
		// set base ship data
		SetBaseShipData(rCharacter);

		if(n > 1) rCharacter.FaceId = 0;

		rCharacter.FaceId = makeint(rand(2) + 2);
		
		FaceMaker(rCharacter);

		rCharacter.headModel = "h_" + rCharacter.model;
		if(CheckAttribute(rCharacter,"blade"))
		{
			if(CheckAttribute(rCharacter,"blade.itemID"))
			{
				rCharacter.equip.blade = rCharacter.blade.itemID;
			}
			else
			{
				rCharacter.equip.blade = BLADE_SABER;
			}
			EquipCharacterByItem(rCharacter, rCharacter.equip.blade);
			DeleteAttribute(rCharacter,"blade");
		}
		if(CheckAttribute(rCharacter,"gun"))
		{
			if(CheckAttribute(rCharacter,"gun.itemID"))
			{
				rCharacter.equip.gun = rCharacter.gun.itemID;
			}
			else
			{
				rCharacter.equip.gun = GUN_COMMON;
			}
			EquipCharacterByItem(rCharacter, rCharacter.equip.gun);
			DeleteAttribute(rCharacter,"gun");
		}
		if(CheckAttribute(rCharacter,"spyglass.itemID"))
		{
			rCharacter.equip.spyglass = rCharacter.spyglass.itemID;
			EquipCharacterByItem(rCharacter, rCharacter.equip.spyglass);
			DeleteAttribute(rCharacter,"spyglass");
		}
	}

	ReloadProgressUpdate();
	
	//Characters[GetCharacterIndex("Beatriss")].model.animation = "woman";
	Characters[GetCharacterIndex("Blaze")].model.animation = "Blaze";

	// boal -->
	initStartState2Character(pchar); // заполнение всяких инитов
	
	NullCharacter.id       = "none"; // заглушка
	NullCharacter.location = "none";

	NullCharacter.SalayPayMonth = STARTGAME_MONTH; //месяц когда ЗП уже плотили
	
	// карты, деньги, два ствола... boal 16/05/05
	NullCharacter.Cards.c1.pic = "hearts_A";
	NullCharacter.Cards.c1.count = 11;
	NullCharacter.Cards.c2.pic = "hearts_K";
	NullCharacter.Cards.c2.count = 4;
	NullCharacter.Cards.c3.pic = "hearts_Q";
	NullCharacter.Cards.c3.count = 3;
	NullCharacter.Cards.c4.pic = "hearts_J";
	NullCharacter.Cards.c4.count = 2;
	NullCharacter.Cards.c5.pic = "hearts_10";
	NullCharacter.Cards.c5.count = 10;
	NullCharacter.Cards.c6.pic = "hearts_9";
	NullCharacter.Cards.c6.count = 9;
	NullCharacter.Cards.c7.pic = "hearts_8";
	NullCharacter.Cards.c7.count = 8;
	NullCharacter.Cards.c8.pic = "hearts_7";
	NullCharacter.Cards.c8.count = 7;
	NullCharacter.Cards.c9.pic = "hearts_6";
	NullCharacter.Cards.c9.count = 6;

	NullCharacter.Cards.c10.pic = "diamonds_A";
	NullCharacter.Cards.c10.count = 11;
	NullCharacter.Cards.c11.pic = "diamonds_K";
	NullCharacter.Cards.c11.count = 4;
	NullCharacter.Cards.c12.pic = "diamonds_Q";
	NullCharacter.Cards.c12.count = 3;
	NullCharacter.Cards.c13.pic = "diamonds_J";
	NullCharacter.Cards.c13.count = 2;
	NullCharacter.Cards.c14.pic = "diamonds_10";
	NullCharacter.Cards.c14.count = 10;
	NullCharacter.Cards.c15.pic = "diamonds_9";
	NullCharacter.Cards.c15.count = 9;
	NullCharacter.Cards.c16.pic = "diamonds_8";
	NullCharacter.Cards.c16.count = 8;
	NullCharacter.Cards.c17.pic = "diamonds_7";
	NullCharacter.Cards.c17.count = 7;
	NullCharacter.Cards.c36.pic = "diamonds_6";
	NullCharacter.Cards.c36.count = 6;

	NullCharacter.Cards.c18.pic = "spades_A";
	NullCharacter.Cards.c18.count = 11;
	NullCharacter.Cards.c19.pic = "spades_K";
	NullCharacter.Cards.c19.count = 4;
	NullCharacter.Cards.c20.pic = "spades_Q";
	NullCharacter.Cards.c20.count = 3;
	NullCharacter.Cards.c21.pic = "spades_J";
	NullCharacter.Cards.c21.count = 2;
	NullCharacter.Cards.c22.pic = "spades_10";
	NullCharacter.Cards.c22.count = 10;
	NullCharacter.Cards.c23.pic = "spades_9";
	NullCharacter.Cards.c23.count = 9;
	NullCharacter.Cards.c24.pic = "spades_8";
	NullCharacter.Cards.c24.count = 8;
	NullCharacter.Cards.c25.pic = "spades_7";
	NullCharacter.Cards.c25.count = 7;
	NullCharacter.Cards.c26.pic = "spades_6";
	NullCharacter.Cards.c26.count = 6;

	NullCharacter.Cards.c27.pic = "clubs_A";
	NullCharacter.Cards.c27.count = 11;
	NullCharacter.Cards.c28.pic = "clubs_K";
	NullCharacter.Cards.c28.count = 4;
	NullCharacter.Cards.c29.pic = "clubs_Q";
	NullCharacter.Cards.c29.count = 3;
	NullCharacter.Cards.c30.pic = "clubs_J";
	NullCharacter.Cards.c30.count = 2;
	NullCharacter.Cards.c31.pic = "clubs_10";
	NullCharacter.Cards.c31.count = 10;
	NullCharacter.Cards.c32.pic = "clubs_9";
	NullCharacter.Cards.c32.count = 9;
	NullCharacter.Cards.c33.pic = "clubs_8";
	NullCharacter.Cards.c33.count = 8;
	NullCharacter.Cards.c34.pic = "clubs_7";
	NullCharacter.Cards.c34.count = 7;
	NullCharacter.Cards.c35.pic = "clubs_6";
	NullCharacter.Cards.c35.count = 6;
	
	// ограничения по типам ГГ в специал
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Strength = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Perception = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Endurance = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Charisma = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Intellect = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Agility = 8;
	NullCharacter.HeroLimitSPEC.SecretAgent.Max_Luck = 8;

	NullCharacter.HeroLimitSPEC.Master.Min_Strength = 5;
	NullCharacter.HeroLimitSPEC.Master.Max_Perception = 8;

	NullCharacter.HeroLimitSPEC.Merchant.Min_Intellect = 5;
	NullCharacter.HeroLimitSPEC.Merchant.Max_Strength = 7;

	NullCharacter.HeroLimitSPEC.Corsair.Min_Agility = 7;
	NullCharacter.HeroLimitSPEC.Corsair.Max_Charisma = 5;

	NullCharacter.HeroLimitSPEC.Adventurer.Min_Luck = 6;
	NullCharacter.HeroLimitSPEC.Adventurer.Max_Endurance = 8;

	NullCharacter.HeroLimitSPEC.Inquisitor.Min_Charisma = 6;
	NullCharacter.HeroLimitSPEC.Inquisitor.Max_Intellect = 8;
	
	InitRPGType();
	// boal <--
	// зават городов -->
    NullCharacter.GenQuestFort.Bridgetown.how        = 3;
    NullCharacter.GenQuestFort.Bridgetown.howSolder  = 3;
	NullCharacter.GenQuestFort.Bridgetown.next0  = "Bridgetown_town";
    NullCharacter.GenQuestFort.Bridgetown.next1  = "Bridgetown_townhall";
    NullCharacter.GenQuestFort.Bridgetown.next2  = "Bridgetown_ExitTown";

    NullCharacter.GenQuestFort.SanJuan.how        = 3;
    NullCharacter.GenQuestFort.SanJuan.howSolder  = 3;
	NullCharacter.GenQuestFort.SanJuan.next0  = "SanJuan_town";
    NullCharacter.GenQuestFort.SanJuan.next1  = "SanJuan_townhall";
    NullCharacter.GenQuestFort.SanJuan.next2  = "SanJuan_ExitTown";

    NullCharacter.GenQuestFort.Santiago.how        = 3;
    NullCharacter.GenQuestFort.Santiago.howSolder  = 3;
	NullCharacter.GenQuestFort.Santiago.next0  = "Santiago_town";
    NullCharacter.GenQuestFort.Santiago.next1  = "Santiago_townhall";
    NullCharacter.GenQuestFort.Santiago.next2  = "Santiago_ExitTown";
    
    NullCharacter.GenQuestFort.SentJons.how        = 3;
    NullCharacter.GenQuestFort.SentJons.howSolder  = 3;
	NullCharacter.GenQuestFort.SentJons.next0  = "SentJons_town";
    NullCharacter.GenQuestFort.SentJons.next1  = "SentJons_townhall";
    NullCharacter.GenQuestFort.SentJons.next2  = "SentJons_ExitTown";

    NullCharacter.GenQuestFort.PortRoyal.how        = 3;
    NullCharacter.GenQuestFort.PortRoyal.howSolder  = 3;
	NullCharacter.GenQuestFort.PortRoyal.next0  = "PortRoyal_town";
    NullCharacter.GenQuestFort.PortRoyal.next1  = "PortRoyal_townhall";
    NullCharacter.GenQuestFort.PortRoyal.next2  = "PortRoyal_ExitTown";

    NullCharacter.GenQuestFort.FortOrange.how        = 3;
    NullCharacter.GenQuestFort.FortOrange.howSolder  = 3;
	NullCharacter.GenQuestFort.FortOrange.next0  = "FortOrange_town";
    NullCharacter.GenQuestFort.FortOrange.next1  = "FortOrange_townhall";
    NullCharacter.GenQuestFort.FortOrange.next2  = "FortOrange_ExitTown";

    NullCharacter.GenQuestFort.Villemstad.how        = 3;
    NullCharacter.GenQuestFort.Villemstad.howSolder  = 3;
	NullCharacter.GenQuestFort.Villemstad.next0  = "Villemstad_town";
    NullCharacter.GenQuestFort.Villemstad.next1  = "Villemstad_townhall";
    NullCharacter.GenQuestFort.Villemstad.next2  = "Villemstad_ExitTown";

    NullCharacter.GenQuestFort.BasTer.how        = 3;
    NullCharacter.GenQuestFort.BasTer.howSolder  = 3;
	NullCharacter.GenQuestFort.BasTer.next0  = "BasTer_town";
    NullCharacter.GenQuestFort.BasTer.next1  = "BasTer_townhall";
    NullCharacter.GenQuestFort.BasTer.next2  = "BasTer_ExitTown";

    NullCharacter.GenQuestFort.SantoDomingo.how        = 3;
    NullCharacter.GenQuestFort.SantoDomingo.howSolder  = 3;
	NullCharacter.GenQuestFort.SantoDomingo.next0  = "SantoDomingo_town";
    NullCharacter.GenQuestFort.SantoDomingo.next1  = "SantoDomingo_townhall";
    NullCharacter.GenQuestFort.SantoDomingo.next2  = "SantoDomingo_ExitTown";
    
    NullCharacter.GenQuestFort.PortPax.how        = 3;
    NullCharacter.GenQuestFort.PortPax.howSolder  = 3;
	NullCharacter.GenQuestFort.PortPax.next0  = "PortPax_town";
    NullCharacter.GenQuestFort.PortPax.next1  = "PortPax_townhall";
    NullCharacter.GenQuestFort.PortPax.next2  = "PortPax_ExitTown";
    
    NullCharacter.GenQuestFort.FortFrance.how        = 3;
    NullCharacter.GenQuestFort.FortFrance.howSolder  = 3;
	NullCharacter.GenQuestFort.FortFrance.next0  = "FortFrance_town";
    NullCharacter.GenQuestFort.FortFrance.next1  = "FortFrance_townhall";
    NullCharacter.GenQuestFort.FortFrance.next2  = "FortFrance_ExitTown";
    
    NullCharacter.GenQuestFort.Havana.how        = 3;
    NullCharacter.GenQuestFort.Havana.howSolder  = 3;
	NullCharacter.GenQuestFort.Havana.next0  = "Havana_town";
    NullCharacter.GenQuestFort.Havana.next1  = "Havana_townhall";
    NullCharacter.GenQuestFort.Havana.next2  = "Havana_ExitTown";
    
    NullCharacter.GenQuestFort.Charles.how        = 3;
    NullCharacter.GenQuestFort.Charles.howSolder  = 3;
	NullCharacter.GenQuestFort.Charles.next0  = "Charles_town";
    NullCharacter.GenQuestFort.Charles.next1  = "Charles_townhall";
    NullCharacter.GenQuestFort.Charles.next2  = "Charles_ExitTown";
    
    NullCharacter.GenQuestFort.PortoBello.how        = 3;
    NullCharacter.GenQuestFort.PortoBello.howSolder  = 3;
	NullCharacter.GenQuestFort.PortoBello.next0  = "PortoBello_town";
    NullCharacter.GenQuestFort.PortoBello.next1  = "PortoBello_townhall";
    NullCharacter.GenQuestFort.PortoBello.next2  = "PortoBello_ExitTown";
    
    NullCharacter.GenQuestFort.PortSpein.how        = 3;
    NullCharacter.GenQuestFort.PortSpein.howSolder  = 3;
	NullCharacter.GenQuestFort.PortSpein.next0  = "PortSpein_town";
    NullCharacter.GenQuestFort.PortSpein.next1  = "PortSpein_townhall";
    NullCharacter.GenQuestFort.PortSpein.next2  = "PortSpein_ExitTown";

    NullCharacter.GenQuestFort.Tortuga.how        = 3;
    NullCharacter.GenQuestFort.Tortuga.howSolder  = 3;
	NullCharacter.GenQuestFort.Tortuga.next0  = "Tortuga_town";
    NullCharacter.GenQuestFort.Tortuga.next1  = "Tortuga_townhall";
    NullCharacter.GenQuestFort.Tortuga.next2  = "Tortuga_ExitTown";

    NullCharacter.GenQuestFort.Marigo.how        = 3;
    NullCharacter.GenQuestFort.Marigo.howSolder  = 3;
	NullCharacter.GenQuestFort.Marigo.next0  = "Marigo_town";
    NullCharacter.GenQuestFort.Marigo.next1  = "Marigo_townhall";
    NullCharacter.GenQuestFort.Marigo.next2  = "Marigo_ExitTown";

    NullCharacter.GenQuestFort.Panama.how        = 3;
    NullCharacter.GenQuestFort.Panama.howSolder  = 3;
	NullCharacter.GenQuestFort.Panama.next0  = "Panama_town";
    NullCharacter.GenQuestFort.Panama.next1  = "Panama_townhall";
    NullCharacter.GenQuestFort.Panama.next2  = "Panama_ExitTown";

    NullCharacter.GenQuestFort.Cartahena.how        = 3;
    NullCharacter.GenQuestFort.Cartahena.howSolder  = 3;
	NullCharacter.GenQuestFort.Cartahena.next0  = "Cartahena_town";
    NullCharacter.GenQuestFort.Cartahena.next1  = "Cartahena_townhall";
    NullCharacter.GenQuestFort.Cartahena.next2  = "Cartahena_ExitTown";

    NullCharacter.GenQuestFort.Maracaibo.how        = 3;
    NullCharacter.GenQuestFort.Maracaibo.howSolder  = 3;
	NullCharacter.GenQuestFort.Maracaibo.next0  = "Maracaibo_town";
    NullCharacter.GenQuestFort.Maracaibo.next1  = "Maracaibo_townhall";
    NullCharacter.GenQuestFort.Maracaibo.next2  = "Maracaibo_ExitTown";

    NullCharacter.GenQuestFort.Caracas.how        = 3;
    NullCharacter.GenQuestFort.Caracas.howSolder  = 3;
	NullCharacter.GenQuestFort.Caracas.next0  = "Caracas_town";
    NullCharacter.GenQuestFort.Caracas.next1  = "Caracas_townhall";
    NullCharacter.GenQuestFort.Caracas.next2  = "Caracas_ExitTown";
    
    NullCharacter.GenQuestFort.Cumana.how        = 3;
    NullCharacter.GenQuestFort.Cumana.howSolder  = 3;
	NullCharacter.GenQuestFort.Cumana.next0  = "Cumana_town";
    NullCharacter.GenQuestFort.Cumana.next1  = "Cumana_townhall";
    NullCharacter.GenQuestFort.Cumana.next2  = "Cumana_ExitTown";
    
    NullCharacter.GenQuestFort.SantaCatalina.how        = 3;
    NullCharacter.GenQuestFort.SantaCatalina.howSolder  = 3;
	NullCharacter.GenQuestFort.SantaCatalina.next0  = "SantaCatalina_town";
    NullCharacter.GenQuestFort.SantaCatalina.next1  = "SantaCatalina_townhall";
    NullCharacter.GenQuestFort.SantaCatalina.next2  = "SantaCatalina_ExitTown";

    NullCharacter.GenQuestFort.Beliz.how        = 3;
    NullCharacter.GenQuestFort.Beliz.howSolder  = 3;
	NullCharacter.GenQuestFort.Beliz.next0  = "Beliz_town";
    NullCharacter.GenQuestFort.Beliz.next1  = "Beliz_townhall";
    NullCharacter.GenQuestFort.Beliz.next2  = "Beliz_ExitTown";
    
    NullCharacter.GenQuestFort.LaVega.how        = 3;
    NullCharacter.GenQuestFort.LaVega.howSolder  = 3;
	NullCharacter.GenQuestFort.LaVega.next0  = "LaVega_town";
    NullCharacter.GenQuestFort.LaVega.next1  = "LaVega_townhall";
    NullCharacter.GenQuestFort.LaVega.next2  = "LaVega_ExitTown";

	// зават городов <--
}

void SetAllShipData()
{
	for(int i=0;i<MAX_CHARACTERS;i++)
	{
		SetBaseShipData(GetCharacter(i));
	}
}

void SetAllFellows()
{
	ref rcc;
	for(int i=0;i<MAX_CHARACTERS;i++)
	{
		rcc = GetCharacter(i);
		if(CheckAttribute(rcc,"fellows"))
		{
			SetBaseFellows(rcc);
		}
	}
}