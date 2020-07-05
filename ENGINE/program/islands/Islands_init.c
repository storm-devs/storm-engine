void InitIslands()
{
	int		n, i;

	for (i=0; i<MAX_ISLANDS; i++)
	{
		DeleteAttribute(&Islands[i], "");

		Islands[i].id = "";
		Islands[i].reload_enable = true;
		Islands[i].visible = true;
		Islands[i].colonyquantity = 1;
		Islands[i].net = true;
		
		Islands[i].ImmersionDistance = 450000.0;
		Islands[i].ImmersionDepth = 0.0;
		Islands[i].maxviewdist = 5000.0;
	}
	

	// ==================================================================================================
	// Create temp module for enumerate locators
		object	locator;
		CreateEntity(&locator,"locator");


	/////////////////////////////////////////////////////////////////////////
	/// Barbados (Bridgetown)
	/////////////////////////////////////////////////////////////////////////
		n = 0;
		Islands[n].id = "Barbados";
		Islands[n].model = "barbados";
		Islands[n].filespath.models = "islands\barbados";
		Islands[n].refl_model = "barbados_refl";
		Islands[n].locators = "barbados_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Barbados_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 3500;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "BARBADOS";

		Islands[n].reload.l1.label = "Bridgetown Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Bridgetown_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 300.0;
		Islands[n].reload.l1.istown = true;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		
		Islands[n].reload.l2.label = "Bridgetown Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Bridgetown_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Bridgetown";
		Islands[n].reload.l2.colonyname = "Bridgetown";
		Islands[n].reload.l2.fort.model = "Barbados_fort1";
		Islands[n].reload.l2.fort.locators = "barbados_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Mayak2";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak2";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 300.0;
		Islands[n].reload.l3.pic = 1;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore4";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore4";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 300.0;
		Islands[n].reload.l4.pic = 1;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore5";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore5";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 300.0;
		Islands[n].reload.l5.pic = 1;
		Islands[n].reload.l5.tex = "t1";

		// Islands[n].net.deathmatch.l1..lxx - deathmatch and team deathmatch locators
		// Islands[n].net.convoy.l1..l2 - defend convoy(l1 start point)
		// Islands[n].net.fort.l1..lxx - 
		// boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_SANDAL;
		Islands[n].Trade.Export.id2 = GOOD_WEAPON;
		Islands[n].Trade.Export.id3 = GOOD_COFFEE;
		Islands[n].Trade.Export.id4 = GOOD_OIL;
		Islands[n].Trade.Export.id5 = GOOD_POWDER;
		Islands[n].Trade.Export.id6 = GOOD_SUGAR;
		Islands[n].Trade.Export.id7 = GOOD_PAPRIKA;

		Islands[n].Trade.Import.id1 = GOOD_CINNAMON;
		Islands[n].Trade.Import.id2 = GOOD_FOOD;
		Islands[n].Trade.Import.id3 = GOOD_BRICK;
		Islands[n].Trade.Import.id4 = GOOD_FRUITS;
		Islands[n].Trade.Import.id5 = GOOD_PLANKS;
		Islands[n].Trade.Import.id6 = GOOD_COPRA;
		Islands[n].Trade.Import.id7 = GOOD_ALE;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_LINEN;
		Islands[n].Trade.Contraband.id3 = GOOD_CHOCOLATE;
        Islands[n].Trade.Contraband.id4 = GOOD_SAILCLOTH;
        Islands[n].Trade.Contraband.id5 = GOOD_CLOTHES;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Jamaica (PortRoyal)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Jamaica";
		Islands[n].model = "Jamaica";
		Islands[n].filespath.models = "islands\Jamaica";
		Islands[n].refl_model = "jamaica_refl";
		Islands[n].locators = "jamaica_locators";
		Islands[n].visible = true;
		Islands[n].jungle.patch = "Jamaica_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 3500;
		//Islands[n].ImmersionDepth = 0;
		Islands[n].maxviewdist = 11000.0;
		Islands[n].TexturePath = "Jamaica";
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].mayak.model_day = "Jamaica_dayLight";     //день на маяке 
		Islands[n].mayak.model_night = "Jamaica_nightLight"; //ночь на маяке 

		Islands[n].reload.l1.label = "PortRoyal Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "PortRoyal_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 300.0;
		Islands[n].reload.l1.istown = true;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";

		Islands[n].reload.l2.label = "PortRoyal Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "PortRoyal_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_PortRoyal";
		Islands[n].reload.l2.colonyname = "PortRoyal";
		Islands[n].reload.l2.fort.model = "Jamaica_fort1";
		Islands[n].reload.l2.fort.locators = "jamaica_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore36";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore36";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 300.0;
		Islands[n].reload.l3.pic = 1;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore35";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore35";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 300.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Mayak3";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Mayak3";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

		// Islands[n].net.deathmatch.l1..lxx - deathmatch and team deathmatch locators
		// Islands[n].net.convoy.l1..l2 - defend convoy(l1 start point)
		// Islands[n].net.fort.l1..lxx -
        // boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_CLOTHES;
		Islands[n].Trade.Export.id2 = GOOD_BRICK;
		Islands[n].Trade.Export.id3 = GOOD_POWDER;
		Islands[n].Trade.Export.id4 = GOOD_ALE;
		Islands[n].Trade.Export.id5 = GOOD_SUGAR;
		Islands[n].Trade.Export.id6 = GOOD_COFFEE;
		Islands[n].Trade.Export.id7 = GOOD_RUM;

		Islands[n].Trade.Import.id1 = GOOD_MAHOGANY;
		Islands[n].Trade.Import.id2 = GOOD_EBONY;
		Islands[n].Trade.Import.id3 = GOOD_SANDAL;
		Islands[n].Trade.Import.id4 = GOOD_GOLD;
		Islands[n].Trade.Import.id5 = GOOD_LEATHER;
		Islands[n].Trade.Import.id6 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id7 = GOOD_MEDICAMENT;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_PLANKS;
		Islands[n].Trade.Contraband.id3 = GOOD_WEAPON;
        Islands[n].Trade.Contraband.id4 = GOOD_CHOCOLATE;
        Islands[n].Trade.Contraband.id5 = GOOD_OIL;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// PuertoRico  (SanJuan)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "PuertoRico";
		Islands[n].model = "PuertoRico";
		Islands[n].filespath.models = "islands\PuertoRico";
		Islands[n].refl_model = "PuertoRico_refl";
		Islands[n].locators = "PuertoRico_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "PuertoRico_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 20;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "PuertoRico";

		Islands[n].reload.l1.label = "SanJuan Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "SanJuan_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";	
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "SanJuan Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "SanJuan_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 1200.0;
		Islands[n].reload.l2.fortname = "fort_SanJuan";
		Islands[n].reload.l2.colonyname = "SanJuan";
		Islands[n].reload.l2.fort.model = "PuertoRico_fort1";
		Islands[n].reload.l2.fort.locators = "PuertoRico_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";
		
		Islands[n].reload.l3.label = "Shore45";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore45";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore44";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore44";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
		
		// boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_ALE;
		Islands[n].Trade.Export.id2 = GOOD_PLANKS;
		Islands[n].Trade.Export.id3 = GOOD_COFFEE;
		Islands[n].Trade.Export.id4 = GOOD_CINNAMON;
		Islands[n].Trade.Export.id5 = GOOD_LEATHER;
		Islands[n].Trade.Export.id6 = GOOD_SUGAR;
		Islands[n].Trade.Export.id7 = GOOD_LINEN;

		Islands[n].Trade.Import.id1 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id2 = GOOD_BRICK;
		Islands[n].Trade.Import.id3 = GOOD_FRUITS;
		Islands[n].Trade.Import.id4 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id5 = GOOD_WINE;
		Islands[n].Trade.Import.id6 = GOOD_WEAPON;
		Islands[n].Trade.Import.id7 = GOOD_POWDER;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_COTTON;
		Islands[n].Trade.Contraband.id3 = GOOD_SILK;
        Islands[n].Trade.Contraband.id4 = GOOD_COPRA;
        Islands[n].Trade.Contraband.id5 = GOOD_WHEAT;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Cuba1 (Santiago, PuertoPrincipe)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Cuba1";
		Islands[n].model = "Cuba1";
		Islands[n].filespath.models = "islands\Cuba1";
		Islands[n].refl_model = "Cuba1_refl";
		Islands[n].locators = "Cuba1_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Cuba1_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Cuba1";

		Islands[n].reload.l1.label = "Santiago Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Santiago_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "Santiago Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Santiago_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Santiago";
		Islands[n].reload.l2.colonyname = "Santiago";
		Islands[n].reload.l2.fort.model = "Cuba1_fort1";
		Islands[n].reload.l2.fort.locators = "Cuba1_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore12";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore12";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore11";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore11";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "PuertoPrincipe Port";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "PuertoPrincipe_port";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";
		Islands[n].reload.l5.istown = true;

		Islands[n].reload.l6.label = "Mayak9";
		Islands[n].reload.l6.name = "reload_5";
		Islands[n].reload.l6.go = "Mayak9";
		Islands[n].reload.l6.emerge = "sea";
		Islands[n].reload.l6.radius = 600.0;
		Islands[n].reload.l6.pic = 0;
		Islands[n].reload.l6.tex = "t1";		
		// boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_COFFEE;
		Islands[n].Trade.Export.id2 = GOOD_FRUITS;
		Islands[n].Trade.Export.id3 = GOOD_CHOCOLATE;
		Islands[n].Trade.Export.id4 = GOOD_LINEN;
		Islands[n].Trade.Export.id5 = GOOD_SANDAL;
		Islands[n].Trade.Export.id6 = GOOD_WHEAT;
		Islands[n].Trade.Export.id7 = GOOD_PLANKS;

		Islands[n].Trade.Import.id1 = GOOD_EBONY;
		Islands[n].Trade.Import.id2 = GOOD_BRICK;
		Islands[n].Trade.Import.id3 = GOOD_GOLD;
		Islands[n].Trade.Import.id4 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id5 = GOOD_BALLS;
		Islands[n].Trade.Import.id6 = GOOD_GRAPES;
		Islands[n].Trade.Import.id7 = GOOD_OIL;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_TOBACCO;
		Islands[n].Trade.Contraband.id3 = GOOD_POWDER;
        Islands[n].Trade.Contraband.id4 = GOOD_WEAPON;
        Islands[n].Trade.Contraband.id5 = GOOD_COTTON;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);	

	/////////////////////////////////////////////////////////////////////////
	/// Cuba2 (Havana)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Cuba2";
		Islands[n].model = "Cuba2";
		Islands[n].filespath.models = "islands\Cuba2";
		Islands[n].refl_model = "Cuba2_refl";
		Islands[n].locators = "Cuba2_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Cuba2_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Cuba2";

		Islands[n].reload.l1.label = "Havana Port";
		Islands[n].reload.l1.name = "Reload_1";
		Islands[n].reload.l1.go = "Havana_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Havana Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Havana_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Havana";
		Islands[n].reload.l2.colonyname = "Havana";
		Islands[n].reload.l2.fort.model = "Cuba2_fort1";
		Islands[n].reload.l2.fort.locators = "Cuba2_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Mayak10";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak10";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore13";
		Islands[n].reload.l4.name = "reload_5";
		Islands[n].reload.l4.go = "Shore13";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore14";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore14";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

		Islands[n].reload.l6.label = "Shore15";
		Islands[n].reload.l6.name = "reload_3";
		Islands[n].reload.l6.go = "Shore15";
		Islands[n].reload.l6.emerge = "sea";
		Islands[n].reload.l6.radius = 600.0;
		Islands[n].reload.l6.pic = 0;
		Islands[n].reload.l6.tex = "t1";
		// boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_MAHOGANY;
		Islands[n].Trade.Export.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Export.id3 = GOOD_LEATHER;
		Islands[n].Trade.Export.id4 = GOOD_FOOD;
		Islands[n].Trade.Export.id5 = GOOD_WINE;
		Islands[n].Trade.Export.id6 = GOOD_SUGAR;
		Islands[n].Trade.Export.id7 = GOOD_PAPRIKA;

		Islands[n].Trade.Import.id1 = GOOD_OIL;
		Islands[n].Trade.Import.id2 = GOOD_COPRA;
		Islands[n].Trade.Import.id3 = GOOD_BRICK;
		Islands[n].Trade.Import.id4 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id5 = GOOD_GRAPES;
		Islands[n].Trade.Import.id6 = GOOD_SILVER;
		Islands[n].Trade.Import.id7 = GOOD_SILK;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_TOBACCO;
		Islands[n].Trade.Contraband.id3 = GOOD_POWDER;
        Islands[n].Trade.Contraband.id4 = GOOD_WEAPON;
        Islands[n].Trade.Contraband.id5 = GOOD_COTTON;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Curacao (Villemstad)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Curacao";
		Islands[n].model = "Curacao";
		Islands[n].filespath.models = "islands\Curacao";
		Islands[n].refl_model = "Curacao_refl";
		Islands[n].locators = "Curacao_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Curacao_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Curacao";

		Islands[n].reload.l1.label = "Villemstad Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Villemstad_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";	
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "Villemstad Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Villemstad_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Villemstad";
		Islands[n].reload.l2.colonyname = "Villemstad";
		Islands[n].reload.l2.fort.model = "Curacao_fort1";
		Islands[n].reload.l2.fort.locators = "Curacao_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";
		
		Islands[n].reload.l3.label = "Shore23";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore23";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore22";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore22";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore24";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore24";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";
		// boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_ALE;
		Islands[n].Trade.Export.id2 = GOOD_SANDAL;
		Islands[n].Trade.Export.id3 = GOOD_FOOD;
		Islands[n].Trade.Export.id4 = GOOD_MAHOGANY;
		Islands[n].Trade.Export.id5 = GOOD_OIL;
		Islands[n].Trade.Export.id6 = GOOD_LINEN;
		Islands[n].Trade.Export.id7 = GOOD_COTTON;

		Islands[n].Trade.Import.id1 = GOOD_SUGAR;
		Islands[n].Trade.Import.id2 = GOOD_BRICK;
		Islands[n].Trade.Import.id3 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id4 = GOOD_FRUITS;
		Islands[n].Trade.Import.id5 = GOOD_WEAPON;
		Islands[n].Trade.Import.id6 = GOOD_CHOCOLATE;
		Islands[n].Trade.Import.id7 = GOOD_PAPRIKA;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_WINE;
		Islands[n].Trade.Contraband.id3 = GOOD_TOBACCO;
        Islands[n].Trade.Contraband.id4 = GOOD_CINNAMON;
        Islands[n].Trade.Contraband.id5 = GOOD_POWDER;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);	
		
	/////////////////////////////////////////////////////////////////////////
	/// Tortuga
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Tortuga";
		Islands[n].model = "Tortuga";
		Islands[n].filespath.models = "islands\Tortuga";
		Islands[n].refl_model = "Tortuga_refl";
		Islands[n].locators = "Tortuga_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Tortuga_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10;
		//Islands[n].ImmersionDistance = 6000;
		//Islands[n].ImmersionDepth = 100;
		Islands[n].TexturePath = "Tortuga";

		Islands[n].reload.l1.label = "Tortuga Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Tortuga_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "Tortuga Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Tortuga_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Tortuga";
		Islands[n].reload.l2.colonyname = "Tortuga";
		Islands[n].reload.l2.fort.model = "Tortuga_fort1";
		Islands[n].reload.l2.fort.locators = "Tortuga_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";
		
		Islands[n].reload.l3.label = "Mayak6";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak6";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";
				
		Islands[n].reload.l4.label = "Shore58";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore58";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

        // boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_ALE;
		Islands[n].Trade.Export.id2 = GOOD_KNIPPELS;
		Islands[n].Trade.Export.id3 = GOOD_WHEAT;
		Islands[n].Trade.Export.id4 = GOOD_LEATHER;
		Islands[n].Trade.Export.id5 = GOOD_COFFEE;
		Islands[n].Trade.Export.id6 = GOOD_LINEN;
		Islands[n].Trade.Export.id7 = GOOD_PAPRIKA;

		Islands[n].Trade.Import.id1 = GOOD_SILVER;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id4 = GOOD_MAHOGANY;
		Islands[n].Trade.Import.id5 = GOOD_BALLS;
		Islands[n].Trade.Import.id6 = GOOD_SILK;
		Islands[n].Trade.Import.id7 = GOOD_WEAPON;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_COPRA;
		Islands[n].Trade.Contraband.id3 = GOOD_OIL;
        Islands[n].Trade.Contraband.id4 = GOOD_BRICK;
        Islands[n].Trade.Contraband.id5 = GOOD_EBONY;
        // boal 16.04.04 <--
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
		
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);		

		/////////////////////////////////////////////////////////////////////////
		/// SentMartin (Marigo)
		/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "SentMartin";
		Islands[n].model = "SentMartin"; // меняем название модели на новое
		Islands[n].filespath.models = "islands\SentMartin"; // меняем путь к модели на новый
		Islands[n].refl_model = "SentMartin_refl"; // меняем название модели на новое
		Islands[n].locators = "SentMartin_locators"; // меняем название модели на новое
		Islands[n].visible = true;
		Islands[n].jungle.patch = "SentMartin_jungle";// сажаем джунгли
		Islands[n].jungle.texture = "junglesprites"; // указываем текстуру джунглей
		Islands[n].jungle.scale = 10; // указываем масштаб для джунглей
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].ImmersionDistance = 4000;
		Islands[n].ImmersionDepth = 0;// ставим глубину погружения =0
		Islands[n].maxviewdist = 5000.0; //ставим максимальную дистанцию видимости, кстати она считается от корабля, в отличие от утопления
		Islands[n].TexturePath = "StMartin"; // хотя там теперь ничего нет

		Islands[n].reload.l1.label = "Marigo Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Marigo_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Marigo Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Marigo_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Marigo";
		Islands[n].reload.l2.colonyname = "Marigo";
		Islands[n].reload.l2.fort.model = "SentMartin_fort1";// меняем название модели на новое
		Islands[n].reload.l2.fort.locators = "SentMartin_fort1_locators"; // меняем название модели на новое
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore41";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore41";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore40";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore40";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
        // boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_CHOCOLATE;
		Islands[n].Trade.Export.id2 = GOOD_PLANKS;
		Islands[n].Trade.Export.id3 = GOOD_MAHOGANY;
		Islands[n].Trade.Export.id4 = GOOD_MEDICAMENT;
		Islands[n].Trade.Export.id5 = GOOD_LINEN;
		Islands[n].Trade.Export.id6 = GOOD_LEATHER;
		Islands[n].Trade.Export.id7 = GOOD_OIL;

		Islands[n].Trade.Import.id1 = GOOD_FOOD;
		Islands[n].Trade.Import.id2 = GOOD_WHEAT;
		Islands[n].Trade.Import.id3 = GOOD_GRAPES;
		Islands[n].Trade.Import.id4 = GOOD_COPRA;
		Islands[n].Trade.Import.id5 = GOOD_BRICK;
		Islands[n].Trade.Import.id6 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id7 = GOOD_SAILCLOTH;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_TOBACCO;
		Islands[n].Trade.Contraband.id3 = GOOD_WINE;
        Islands[n].Trade.Contraband.id4 = GOOD_WEAPON;
        Islands[n].Trade.Contraband.id5 = GOOD_SUGAR;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);		

	/////////////////////////////////////////////////////////////////////////
	/// Trinidad  (PortSpein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Trinidad";
		Islands[n].model = "Trinidad";
		Islands[n].filespath.models = "islands\Trinidad";
		Islands[n].refl_model = "Trinidad_refl";
		Islands[n].locators = "Trinidad_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Trinidad_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Trinidad";

		Islands[n].reload.l1.label = "PortSpein Port";
		Islands[n].reload.l1.name = "Reload_1";
		Islands[n].reload.l1.go = "PortSpein_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "PortSpein Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "PortSpein_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_PortSpein";
		Islands[n].reload.l2.colonyname = "PortSpein";
		Islands[n].reload.l2.fort.model = "Trinidad_fort1";
		Islands[n].reload.l2.fort.locators = "Trinidad_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";
				
		Islands[n].reload.l3.label = "Shore60";
		Islands[n].reload.l3.name = "reload_3";
		Islands[n].reload.l3.go = "Shore60";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore59";
		Islands[n].reload.l4.name = "reload_2";
		Islands[n].reload.l4.go = "Shore59";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Mayak1";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Mayak1";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

        Islands[n].Trade.Export.id1 = GOOD_COTTON;
		Islands[n].Trade.Export.id2 = GOOD_WINE;
		Islands[n].Trade.Export.id3 = GOOD_SANDAL;
		Islands[n].Trade.Export.id4 = GOOD_POWDER;
		Islands[n].Trade.Export.id5 = GOOD_TOBACCO;
		Islands[n].Trade.Export.id6 = GOOD_WHEAT;
		Islands[n].Trade.Export.id7 = GOOD_GRAPES;

		Islands[n].Trade.Import.id1 = GOOD_WEAPON;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_COPRA;
		Islands[n].Trade.Import.id4 = GOOD_LEATHER;
		Islands[n].Trade.Import.id5 = GOOD_CHOCOLATE;
		Islands[n].Trade.Import.id6 = GOOD_CINNAMON;
		Islands[n].Trade.Import.id7 = GOOD_BRICK;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_ALE;
		Islands[n].Trade.Contraband.id3 = GOOD_SILK;
        Islands[n].Trade.Contraband.id4 = GOOD_FOOD;
        Islands[n].Trade.Contraband.id5 = GOOD_MAHOGANY;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
	
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Nevis   (Charles)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Nevis";
		Islands[n].model = "Nevis";
		Islands[n].filespath.models = "islands\Nevis";
		Islands[n].refl_model = "Nevis_refl";
		Islands[n].locators = "Nevis_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Nevis_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Nevis";

		Islands[n].reload.l1.label = "Charles Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Charles_town";
		Islands[n].reload.l1.emerge = "reload1"; 
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "Charles Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Charles_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Charles";
		Islands[n].reload.l2.colonyname = "Charles";
		Islands[n].reload.l2.fort.model = "Nevis_fort1";
		Islands[n].reload.l2.fort.locators = "Nevis_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore43";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore43";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";
		
		Islands[n].reload.l4.label = "Shore42";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore42";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Mayak5";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Mayak5";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";
		
        Islands[n].Trade.Export.id1 = GOOD_RUM;
		Islands[n].Trade.Export.id2 = GOOD_BRICK;
		Islands[n].Trade.Export.id3 = GOOD_BOMBS;
		Islands[n].Trade.Export.id4 = GOOD_PAPRIKA;
		Islands[n].Trade.Export.id5 = GOOD_MEDICAMENT;
		Islands[n].Trade.Export.id6 = GOOD_ALE;
		Islands[n].Trade.Export.id7 = GOOD_KNIPPELS;

		Islands[n].Trade.Import.id1 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id2 = GOOD_WHEAT;
		Islands[n].Trade.Import.id3 = GOOD_COTTON;
		Islands[n].Trade.Import.id4 = GOOD_SILK;
		Islands[n].Trade.Import.id5 = GOOD_FRUITS;
		Islands[n].Trade.Import.id6 = GOOD_SUGAR;
		Islands[n].Trade.Import.id7 = GOOD_SAILCLOTH;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_FOOD;
		Islands[n].Trade.Contraband.id3 = GOOD_WINE;
        Islands[n].Trade.Contraband.id4 = GOOD_SANDAL;
        Islands[n].Trade.Contraband.id5 = GOOD_EBONY;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
	
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Antigua  (SentJons)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Antigua";
		Islands[n].model = "Antigua";
		Islands[n].filespath.models = "islands\Antigua";
		Islands[n].refl_model = "Antigua_refl";
		Islands[n].locators = "antigua_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Antigua_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Antigua";

		Islands[n].reload.l1.label = "SentJons Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "SentJons_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "SentJons Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "SentJons_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_SentJons";
		Islands[n].reload.l2.colonyname = "SentJons";
		Islands[n].reload.l2.fort.model = "Antigua_fort1";
		Islands[n].reload.l2.fort.locators = "Antigua_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore1";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore1";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore2";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore2";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
		// boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_RUM;
		Islands[n].Trade.Export.id2 = GOOD_BALLS;
		Islands[n].Trade.Export.id3 = GOOD_LINEN;
		Islands[n].Trade.Export.id4 = GOOD_GOLD;
		Islands[n].Trade.Export.id5 = GOOD_SILVER;
		Islands[n].Trade.Export.id6 = GOOD_ALE;
		Islands[n].Trade.Export.id7 = GOOD_WEAPON;

		Islands[n].Trade.Import.id1 = GOOD_SUGAR;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_EBONY;
		Islands[n].Trade.Import.id4 = GOOD_COPRA;
		Islands[n].Trade.Import.id5 = GOOD_FRUITS;
		Islands[n].Trade.Import.id6 = GOOD_WINE;
		Islands[n].Trade.Import.id7 = GOOD_LEATHER;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_WHEAT;
		Islands[n].Trade.Contraband.id3 = GOOD_COTTON;
        Islands[n].Trade.Contraband.id4 = GOOD_OIL;
        Islands[n].Trade.Contraband.id5 = GOOD_COFFEE;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
	
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Guadeloupe  (BasTer)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Guadeloupe";
		Islands[n].model = "Guadeloupe";
		Islands[n].filespath.models = "islands\Guadeloupe";
		Islands[n].refl_model = "Guadeloupe_refl";
		Islands[n].locators = "Guadeloupe_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Guadeloupe_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Guadeloupe";

		Islands[n].reload.l1.label = "BasTer Port";
		Islands[n].reload.l1.name = "Reload_1";
		Islands[n].reload.l1.go = "BasTer_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 300.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "BasTer Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "BasTer_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_BasTer";
		Islands[n].reload.l2.colonyname = "BasTer";
		Islands[n].reload.l2.fort.model = "Guadeloupe_fort1";
		Islands[n].reload.l2.fort.locators = "Guadeloupe_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Mayak4";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak4";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 300.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore28";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore28";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore31";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore31";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

		Islands[n].reload.l6.label = "Shore30";
		Islands[n].reload.l6.name = "reload_5";
		Islands[n].reload.l6.go = "Shore30";
		Islands[n].reload.l6.emerge = "sea";
		Islands[n].reload.l6.radius = 600.0;
		Islands[n].reload.l6.pic = 0;
		Islands[n].reload.l6.tex = "t1";

		Islands[n].reload.l7.label = "Shore29";
		Islands[n].reload.l7.name = "reload_6";
		Islands[n].reload.l7.go = "Shore29";
		Islands[n].reload.l7.emerge = "sea";
		Islands[n].reload.l7.radius = 600.0;
		Islands[n].reload.l7.pic = 0;
		Islands[n].reload.l7.tex = "t1";

		// boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_PAPRIKA;
		Islands[n].Trade.Export.id2 = GOOD_CINNAMON;
		Islands[n].Trade.Export.id3 = GOOD_BRICK;
		Islands[n].Trade.Export.id4 = GOOD_SILK;
		Islands[n].Trade.Export.id5 = GOOD_POWDER;
		Islands[n].Trade.Export.id6 = GOOD_SILVER;
		Islands[n].Trade.Export.id7 = GOOD_BALLS;

		Islands[n].Trade.Import.id1 = GOOD_PLANKS;
		Islands[n].Trade.Import.id2 = GOOD_EBONY;
		Islands[n].Trade.Import.id3 = GOOD_MAHOGANY;
		Islands[n].Trade.Import.id4 = GOOD_SANDAL;
		Islands[n].Trade.Import.id5 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id6 = GOOD_OIL;
		Islands[n].Trade.Import.id7 = GOOD_GRAPES;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Contraband.id3 = GOOD_LEATHER;
        Islands[n].Trade.Contraband.id4 = GOOD_CHOCOLATE;
        Islands[n].Trade.Contraband.id5 = GOOD_COPRA;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
	
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Martinique (FortFrance, LeFransua)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Martinique";
		Islands[n].model = "Martinique";
		Islands[n].filespath.models = "islands\Martinique";
		Islands[n].refl_model = "Martinique_refl";
		Islands[n].locators = "Martinique_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Martinique_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Martinique";

		Islands[n].reload.l1.label = "FortFrance Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "FortFrance_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "FortFrance Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "FortFrance_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_FortFrance";
		Islands[n].reload.l2.colonyname = "FortFrance";
		Islands[n].reload.l2.fort.model = "Martinique_fort1";
		Islands[n].reload.l2.fort.locators = "Martinique_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";
				
		Islands[n].reload.l3.label = "Shore38";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore38";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "LeFransua Port";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "LeFransua_port";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l5.label = "Shore39";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore39";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

        // boal 20.01.2004 -->
		Islands[n].Trade.Export.id1 = GOOD_WINE;
		Islands[n].Trade.Export.id2 = GOOD_FOOD;
		Islands[n].Trade.Export.id3 = GOOD_WHEAT;
		Islands[n].Trade.Export.id4 = GOOD_SUGAR;
		Islands[n].Trade.Export.id5 = GOOD_TOBACCO;
		Islands[n].Trade.Export.id6 = GOOD_COTTON;
		Islands[n].Trade.Export.id7 = GOOD_KNIPPELS;

		Islands[n].Trade.Import.id1 = GOOD_RUM;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id4 = GOOD_GOLD;
		Islands[n].Trade.Import.id5 = GOOD_CHOCOLATE;
		Islands[n].Trade.Import.id6 = GOOD_LEATHER;
		Islands[n].Trade.Import.id7 = GOOD_LINEN;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_PLANKS;
		Islands[n].Trade.Contraband.id3 = GOOD_FRUITS;
        Islands[n].Trade.Contraband.id4 = GOOD_MAHOGANY;
        Islands[n].Trade.Contraband.id5 = GOOD_EBONY;
        // boal 16.04.04 <--
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
		
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);


	/////////////////////////////////////////////////////////////////////////
	/// Hispaniola1
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Hispaniola1";
		Islands[n].model = "Hispaniola1";
		Islands[n].filespath.models = "islands\Hispaniola1";
		Islands[n].refl_model = "Hispaniola1_refl";
		Islands[n].locators = "Hispaniola1_locators";
		Islands[n].visible = true;
		Islands[n].colonyquantity = 2;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		//Islands[n].LoadGroup.g2 = "IslandShips2";
		//Islands[n].ImmersionDistance = 5000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].jungle.patch = "Hispaniola1_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 20;
		Islands[n].TexturePath = "Hispaniola1";

		Islands[n].reload.l1.label = "SantoDomingo Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "SantoDomingo_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "SantoDomingo Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "SantoDomingo_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_SantoDomingo";
		Islands[n].reload.l2.colonyname = "SantoDomingo";
		Islands[n].reload.l2.fort.model = "Hispaniola1_fort1";
		Islands[n].reload.l2.fort.locators = "Hispaniola1_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Mayak8";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak8";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore32";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore32";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1"; 

		Islands[n].reload.l6.label = "LaVega Port";
		Islands[n].reload.l6.name = "reload_4";
		Islands[n].reload.l6.go = "LaVega_port";
		Islands[n].reload.l6.emerge = "sea";
		Islands[n].reload.l6.radius = 600.0;
		Islands[n].reload.l6.pic = 0;
		Islands[n].reload.l6.tex = "t1";
		Islands[n].reload.l6.istown = true;

		Islands[n].reload.l5.label = "Shore33";
		Islands[n].reload.l5.name = "reload_5";
		Islands[n].reload.l5.go = "Shore33";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";  

        // boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_ALE;
		Islands[n].Trade.Export.id2 = GOOD_RUM;
		Islands[n].Trade.Export.id3 = GOOD_WINE;
		Islands[n].Trade.Export.id4 = GOOD_CHOCOLATE;
		Islands[n].Trade.Export.id5 = GOOD_CINNAMON;
		Islands[n].Trade.Export.id6 = GOOD_SANDAL;
		Islands[n].Trade.Export.id7 = GOOD_FOOD;

		Islands[n].Trade.Import.id1 = GOOD_FRUITS;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id4 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id5 = GOOD_GRAPES;
		Islands[n].Trade.Import.id6 = GOOD_BALLS;
		Islands[n].Trade.Import.id7 = GOOD_WEAPON;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_OIL;
		Islands[n].Trade.Contraband.id3 = GOOD_COPRA;
        Islands[n].Trade.Contraband.id4 = GOOD_BRICK;
        Islands[n].Trade.Contraband.id5 = GOOD_EBONY;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
	
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Hispaniola2
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Hispaniola2";
		Islands[n].model = "Hispaniola2";
		Islands[n].filespath.models = "islands\Hispaniola2";
		Islands[n].refl_model = "Hispaniola2_refl";
		Islands[n].locators = "Hispaniola2_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Hispaniola2_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 20;
		//Islands[n].ImmersionDistance = 6000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Hispaniola2";

		Islands[n].reload.l1.label = "PortPax Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "PortPax_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "PortPax Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "PortPax_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_PortPax";
		Islands[n].reload.l2.colonyname = "PortPax";
		Islands[n].reload.l2.fort.model = "Hispaniola2_fort1";
		Islands[n].reload.l2.fort.locators = "Hispaniola2_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Mayak7";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Mayak7";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore34";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore34";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
        // boal 16.04.04 -->
        Islands[n].Trade.Export.id1 = GOOD_BOMBS;
		Islands[n].Trade.Export.id2 = GOOD_RUM;
		Islands[n].Trade.Export.id3 = GOOD_WINE;
		Islands[n].Trade.Export.id4 = GOOD_SUGAR;
		Islands[n].Trade.Export.id5 = GOOD_CLOTHES;
		Islands[n].Trade.Export.id6 = GOOD_LINEN;
		Islands[n].Trade.Export.id7 = GOOD_FOOD;

		Islands[n].Trade.Import.id1 = GOOD_GOLD;
		Islands[n].Trade.Import.id2 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id3 = GOOD_FRUITS;
		Islands[n].Trade.Import.id4 = GOOD_COTTON;
		Islands[n].Trade.Import.id5 = GOOD_GRAPES;
		Islands[n].Trade.Import.id6 = GOOD_SILK;
		Islands[n].Trade.Import.id7 = GOOD_POWDER;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_OIL;
		Islands[n].Trade.Contraband.id3 = GOOD_COPRA;
        Islands[n].Trade.Contraband.id4 = GOOD_BRICK;
        Islands[n].Trade.Contraband.id5 = GOOD_EBONY;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Bermudes (Pirates Town)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Bermudes";
		Islands[n].model = "Bermudes";
		Islands[n].filespath.models = "islands\Bermudes";
		Islands[n].refl_model = "Bermudes_refl";
		Islands[n].locators = "Bermudes_locators";// прописываем пути к новой модели
		Islands[n].visible = true;
		Islands[n].jungle.patch = "Bermudes_jungle";// сажаем джунгли
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;  
		Islands[n].colonyquantity = 0;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 0;// ставим глубину погружения =0
		Islands[n].maxviewdist = 5000.0; //ставим максимальную дистанцию видимости
		Islands[n].TexturePath = "Bermudes"

		Islands[n].reload.l1.label = "Pirates Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Pirates_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;
		
		Islands[n].reload.l2.label = "Shore3";
		Islands[n].reload.l2.name = "reload_2";
		Islands[n].reload.l2.go = "Shore3";
		Islands[n].reload.l2.emerge = "sea";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.pic = 0;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore_ship1";
		Islands[n].reload.l3.name = "reload_3";
		Islands[n].reload.l3.go = "Shore_ship1";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

        // boal 28.01.2004 -->
        Islands[n].Trade.Export.id1 = GOOD_RUM;
		Islands[n].Trade.Export.id2 = GOOD_BOMBS;

		Islands[n].Trade.Import.id1 = GOOD_SLAVES;
		Islands[n].Trade.Import.id2 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id3 = GOOD_ALE;
		Islands[n].Trade.Import.id4 = GOOD_SILVER;
		Islands[n].Trade.Import.id5 = GOOD_POWDER;
		Islands[n].Trade.Import.id6 = GOOD_ALE;
		Islands[n].Trade.Import.id7 = GOOD_GOLD;
        // boal 28.01.2004 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;
		
		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Dominica
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Dominica";
		Islands[n].model = "Dominica";
		Islands[n].filespath.models = "islands\Dominica";
		Islands[n].refl_model = "Dominica_refl";
		Islands[n].locators = "Dominica_locators";
		Islands[n].visible = true;
		Islands[n].colonyquantity = 0;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Dominica_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Dominica";

		Islands[n].reload.l1.label = "Shore27";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Shore27";
		Islands[n].reload.l1.emerge = "sea";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";

		Islands[n].reload.l2.label = "Shore26";
		Islands[n].reload.l2.name = "reload_2";
		Islands[n].reload.l2.go = "Shore26";
		Islands[n].reload.l2.emerge = "sea";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.pic = 0;
		Islands[n].reload.l2.tex = "t1";

        Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

	/////////////////////////////////////////////////////////////////////////
	/// Terks
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Terks";
		Islands[n].model = "Terks";
		Islands[n].filespath.models = "islands\Terks";
		Islands[n].refl_model = "Terks_refl";
		Islands[n].locators = "Terks_locators";
		Islands[n].visible = true;
		Islands[n].colonyquantity = 0;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Terks_jungle";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "IslaMona";

		Islands[n].reload.l1.label = "Shore57";
		Islands[n].reload.l1.name = "reload_2";
		Islands[n].reload.l1.go = "Shore57";
		Islands[n].reload.l1.emerge = "sea";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";

		Islands[n].reload.l2.label = "Shore56";
		Islands[n].reload.l2.name = "reload_1";
		Islands[n].reload.l2.go = "Shore56";
		Islands[n].reload.l2.emerge = "sea";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.pic = 0;
		Islands[n].reload.l2.tex = "t1";

        Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);


	/////////////////////////////////////////////////////////////////////////
	/// Caiman
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Caiman";
		Islands[n].model = "Caiman";
		Islands[n].filespath.models = "islands\Caiman";
		Islands[n].refl_model = "Caiman_refl";
		Islands[n].locators = "Caiman_locators";
		Islands[n].visible = true;
		Islands[n].colonyquantity = 0;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Caiman_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Caiman";

		Islands[n].reload.l1.label = "Shore17";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Shore17";
		Islands[n].reload.l1.emerge = "sea";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";

		Islands[n].reload.l2.label = "Shore16";
		Islands[n].reload.l2.name = "reload_2";
		Islands[n].reload.l2.go = "Shore16";
		Islands[n].reload.l2.emerge = "sea";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.pic = 0;
		Islands[n].reload.l2.tex = "t1";
		
        Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);


	/////////////////////////////////////////////////////////////////////////
	/// PortoBello   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "PortoBello";
		Islands[n].model = "PortoBello";
		Islands[n].filespath.models = "islands\PortoBello";
		Islands[n].refl_model = "PortoBello_refl";
		Islands[n].locators = "PortoBello_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "PortoBello_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 12.5;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "PortoBello";

		Islands[n].reload.l1.label = "PortoBello Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "PortoBello_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "PortoBello Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "PortoBello_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_PortoBello";
		Islands[n].reload.l2.colonyname = "PortoBello";
		Islands[n].reload.l2.fort.model = "PortoBello_fort1";
		Islands[n].reload.l2.fort.locators = "PortoBello_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore46";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore46";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore47";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore47";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
		
		Islands[n].reload.l5.label = "Shore48";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore48";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";
		
	    Islands[n].Trade.Export.id1 = GOOD_MEDICAMENT;
		Islands[n].Trade.Export.id2 = GOOD_OIL;
		Islands[n].Trade.Export.id3 = GOOD_WINE;
		Islands[n].Trade.Export.id4 = GOOD_SUGAR;
		Islands[n].Trade.Export.id5 = GOOD_ALE;
		Islands[n].Trade.Export.id6 = GOOD_LINEN;
		Islands[n].Trade.Export.id7 = GOOD_CINNAMON;

		Islands[n].Trade.Import.id1 = GOOD_RUM;
		Islands[n].Trade.Import.id2 = GOOD_BALLS;
		Islands[n].Trade.Import.id3 = GOOD_SILK;
		Islands[n].Trade.Import.id4 = GOOD_PAPRIKA;
		Islands[n].Trade.Import.id5 = GOOD_SANDAL;
		Islands[n].Trade.Import.id6 = GOOD_LEATHER;
		Islands[n].Trade.Import.id7 = GOOD_SAILCLOTH;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_BRICK;
		Islands[n].Trade.Contraband.id3 = GOOD_COPRA;
        Islands[n].Trade.Contraband.id4 = GOOD_FRUITS;
        Islands[n].Trade.Contraband.id5 = GOOD_CLOTHES;

		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Cartahena   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Cartahena";
		Islands[n].model = "Cartahena";
		Islands[n].filespath.models = "islands\Cartahena";
		Islands[n].refl_model = "Cartahena_refl";
		Islands[n].locators = "Cartahena_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Cartahena_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10.0;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Cartahena";

		Islands[n].reload.l1.label = "Cartahena Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Cartahena_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Cartahena Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Cartahena_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Cartahena";
		Islands[n].reload.l2.colonyname = "Cartahena";
		Islands[n].reload.l2.fort.model = "Cartahena_fort1";
		Islands[n].reload.l2.fort.locators = "Cartahena_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore25";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore25";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Mayak11";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Mayak11";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";
		
        Islands[n].Trade.Export.id1 = GOOD_WHEAT;
		Islands[n].Trade.Export.id2 = GOOD_CHOCOLATE;
		Islands[n].Trade.Export.id3 = GOOD_GRAPES;
		Islands[n].Trade.Export.id4 = GOOD_OIL;
		Islands[n].Trade.Export.id5 = GOOD_PLANKS;
		Islands[n].Trade.Export.id6 = GOOD_COTTON;
		Islands[n].Trade.Export.id7 = GOOD_EBONY;

		Islands[n].Trade.Import.id1 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id2 = GOOD_COFFEE;
		Islands[n].Trade.Import.id3 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id4 = GOOD_RUM;
		Islands[n].Trade.Import.id5 = GOOD_FRUITS;
		Islands[n].Trade.Import.id6 = GOOD_LEATHER;
		Islands[n].Trade.Import.id7 = GOOD_TOBACCO;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_BRICK;
		Islands[n].Trade.Contraband.id3 = GOOD_SAILCLOTH;
        Islands[n].Trade.Contraband.id4 = GOOD_PAPRIKA;
        Islands[n].Trade.Contraband.id5 = GOOD_SILK;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Maracaibo   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Maracaibo";
		Islands[n].model = "Maracaibo";
		Islands[n].filespath.models = "islands\Maracaibo";
		Islands[n].refl_model = "Maracaibo_refl";
		Islands[n].locators = "Maracaibo_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Maracaibo_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10.0;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Maracaibo";

		Islands[n].reload.l1.label = "Maracaibo Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Maracaibo_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Maracaibo Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Maracaibo_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Maracaibo";
		Islands[n].reload.l2.colonyname = "Maracaibo";
		Islands[n].reload.l2.fort.model = "Maracaibo_fort1";
		Islands[n].reload.l2.fort.locators = "Maracaibo_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore37";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore37";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore_ship3";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore_ship3";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

        Islands[n].Trade.Export.id1 = GOOD_GOLD;
		Islands[n].Trade.Export.id2 = GOOD_SILVER;
		Islands[n].Trade.Export.id3 = GOOD_MAHOGANY;
		Islands[n].Trade.Export.id4 = GOOD_EBONY;
		Islands[n].Trade.Export.id5 = GOOD_PLANKS;
		Islands[n].Trade.Export.id6 = GOOD_SANDAL;
		Islands[n].Trade.Export.id7 = GOOD_SUGAR;

		Islands[n].Trade.Import.id1 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id2 = GOOD_BRICK;
		Islands[n].Trade.Import.id3 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id4 = GOOD_RUM;
		Islands[n].Trade.Import.id5 = GOOD_FRUITS;
		Islands[n].Trade.Import.id6 = GOOD_WINE;
		Islands[n].Trade.Import.id7 = GOOD_WHEAT;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_WEAPON;
		Islands[n].Trade.Contraband.id3 = GOOD_POWDER;
        Islands[n].Trade.Contraband.id4 = GOOD_FOOD;
        Islands[n].Trade.Contraband.id5 = GOOD_LEATHER;
        
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Caracas   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Caracas";
		Islands[n].model = "Caracas";
		Islands[n].filespath.models = "islands\Caracas";
		Islands[n].refl_model = "Caracas_refl";
		Islands[n].locators = "Caracas_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Caracas_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10.0;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Caracas";

		Islands[n].reload.l1.label = "Caracas Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Caracas_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Caracas Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Caracas_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Caracas";
		Islands[n].reload.l2.colonyname = "Caracas";
		Islands[n].reload.l2.fort.model = "Caracas_fort1";
		Islands[n].reload.l2.fort.locators = "Caracas_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore21";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore21";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";
		
        Islands[n].Trade.Export.id1 = GOOD_COFFEE;
		Islands[n].Trade.Export.id2 = GOOD_COTTON;
		Islands[n].Trade.Export.id3 = GOOD_CLOTHES;
		Islands[n].Trade.Export.id4 = GOOD_SUGAR;
		Islands[n].Trade.Export.id5 = GOOD_COPRA;
		Islands[n].Trade.Export.id6 = GOOD_ALE;
		Islands[n].Trade.Export.id7 = GOOD_PLANKS;

		Islands[n].Trade.Import.id1 = GOOD_BALLS;
		Islands[n].Trade.Import.id2 = GOOD_GRAPES;
		Islands[n].Trade.Import.id3 = GOOD_KNIPPELS;
		Islands[n].Trade.Import.id4 = GOOD_BOMBS;
		Islands[n].Trade.Import.id5 = GOOD_POWDER;
		Islands[n].Trade.Import.id6 = GOOD_MEDICAMENT;
		Islands[n].Trade.Import.id7 = GOOD_WEAPON;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_LEATHER;
		Islands[n].Trade.Contraband.id3 = GOOD_WINE;
        Islands[n].Trade.Contraband.id4 = GOOD_RUM;
        Islands[n].Trade.Contraband.id5 = GOOD_TOBACCO;
         
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Cumana   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Cumana";
		Islands[n].model = "Cumana";
		Islands[n].filespath.models = "islands\Cumana";
		Islands[n].refl_model = "Cumana_refl";
		Islands[n].locators = "Cumana_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Cumana_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 10.0;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Cumana";

		Islands[n].reload.l1.label = "Cumana Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Cumana_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Cumana Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Cumana_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Cumana";
		Islands[n].reload.l2.colonyname = "Cumana";
		Islands[n].reload.l2.fort.model = "Cumana_fort1";
		Islands[n].reload.l2.fort.locators = "Cumana_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore20";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore20";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore19";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore19";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore18";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore18";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1"; 

        // boal 16.04.04 -->
		Islands[n].Trade.Export.id1 = GOOD_SAILCLOTH;
		Islands[n].Trade.Export.id2 = GOOD_WHEAT;
		Islands[n].Trade.Export.id3 = GOOD_LINEN;
		Islands[n].Trade.Export.id4 = GOOD_SANDAL;
		Islands[n].Trade.Export.id5 = GOOD_EBONY;
		Islands[n].Trade.Export.id6 = GOOD_OIL;
		Islands[n].Trade.Export.id7 = GOOD_COFFEE;

		Islands[n].Trade.Import.id1 = GOOD_PLANKS;
		Islands[n].Trade.Import.id2 = GOOD_WINE;
		Islands[n].Trade.Import.id3 = GOOD_RUM;
		Islands[n].Trade.Import.id4 = GOOD_CHOCOLATE;
		Islands[n].Trade.Import.id5 = GOOD_PAPRIKA;
		Islands[n].Trade.Import.id6 = GOOD_GOLD;
		Islands[n].Trade.Import.id7 = GOOD_SILVER;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_CINNAMON;
		Islands[n].Trade.Contraband.id3 = GOOD_MEDICAMENT;
        Islands[n].Trade.Contraband.id4 = GOOD_SILK;
        Islands[n].Trade.Contraband.id5 = GOOD_FRUITS;
        // boal 16.04.04 <--
		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
		
	/////////////////////////////////////////////////////////////////////////
	/// SantaCatalina   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "SantaCatalina";
		Islands[n].model = "SantaCatalina";
		Islands[n].filespath.models = "islands\SantaCatalina";
		Islands[n].refl_model = "SantaCatalina_refl";
		Islands[n].locators = "SantaCatalina_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "SantaCatalina_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 16.0;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "SantaCatalina";

		Islands[n].reload.l1.label = "SantaCatalina Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "SantaCatalina_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "SantaCatalina Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "SantaCatalina_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_SantaCatalina";
		Islands[n].reload.l2.colonyname = "SantaCatalina";
		Islands[n].reload.l2.fort.model = "SantaCatalina_fort1";
		Islands[n].reload.l2.fort.locators = "SantaCatalina_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore53";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore53";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore54";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore54";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore55";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore55";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

	    Islands[n].Trade.Export.id1 = GOOD_BALLS;
		Islands[n].Trade.Export.id2 = GOOD_GRAPES;
		Islands[n].Trade.Export.id3 = GOOD_KNIPPELS;
		Islands[n].Trade.Export.id4 = GOOD_BOMBS;
		Islands[n].Trade.Export.id5 = GOOD_POWDER;
		Islands[n].Trade.Export.id6 = GOOD_WINE;
		Islands[n].Trade.Export.id7 = GOOD_WEAPON;

		Islands[n].Trade.Import.id1 = GOOD_SILK;
		Islands[n].Trade.Import.id2 = GOOD_COTTON;
		Islands[n].Trade.Import.id3 = GOOD_SAILCLOTH;
		Islands[n].Trade.Import.id4 = GOOD_CLOTHES;
		Islands[n].Trade.Import.id5 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id6 = GOOD_GOLD;
		Islands[n].Trade.Import.id7 = GOOD_SILVER;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_COPRA;
		Islands[n].Trade.Contraband.id3 = GOOD_PAPRIKA;
        Islands[n].Trade.Contraband.id4 = GOOD_SANDAL;
        Islands[n].Trade.Contraband.id5 = GOOD_LEATHER;

		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
		
	/////////////////////////////////////////////////////////////////////////
	/// Beliz   (Mein)
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "Beliz";
		Islands[n].model = "Beliz";
		Islands[n].filespath.models = "islands\Beliz";
		Islands[n].refl_model = "Beliz_refl";
		Islands[n].locators = "Beliz_locators";
		Islands[n].visible = true;
		Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].jungle.patch = "Beliz_jungles";
		Islands[n].jungle.texture = "junglesprites";
		Islands[n].jungle.scale = 16;
		//Islands[n].ImmersionDistance = 4000;
		//Islands[n].ImmersionDepth = 250;
		Islands[n].TexturePath = "Beliz";

		Islands[n].reload.l1.label = "Beliz Port";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "Beliz_town";
		Islands[n].reload.l1.emerge = "reload1";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true;

		Islands[n].reload.l2.label = "Beliz Fort";
		Islands[n].reload.l2.name = "reload_fort1";
		Islands[n].reload.l2.go = "Beliz_fort";
		Islands[n].reload.l2.GoAfterBoard.location = "reload_1";
		Islands[n].reload.l2.emerge = "see";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.fortname = "fort_Beliz";
		Islands[n].reload.l2.colonyname = "Beliz";
		Islands[n].reload.l2.fort.model = "Beliz_fort1";
		Islands[n].reload.l2.fort.locators = "Beliz_fort1_locators";
		Islands[n].reload.l2.pic = 1;
		Islands[n].reload.l2.tex = "t1";

		Islands[n].reload.l3.label = "Shore8";
		Islands[n].reload.l3.name = "reload_2";
		Islands[n].reload.l3.go = "Shore8";
		Islands[n].reload.l3.emerge = "sea";
		Islands[n].reload.l3.radius = 600.0;
		Islands[n].reload.l3.pic = 0;
		Islands[n].reload.l3.tex = "t1";

		Islands[n].reload.l4.label = "Shore7";
		Islands[n].reload.l4.name = "reload_3";
		Islands[n].reload.l4.go = "Shore7";
		Islands[n].reload.l4.emerge = "sea";
		Islands[n].reload.l4.radius = 600.0;
		Islands[n].reload.l4.pic = 0;
		Islands[n].reload.l4.tex = "t1";

		Islands[n].reload.l5.label = "Shore6";
		Islands[n].reload.l5.name = "reload_4";
		Islands[n].reload.l5.go = "Shore6";
		Islands[n].reload.l5.emerge = "sea";
		Islands[n].reload.l5.radius = 600.0;
		Islands[n].reload.l5.pic = 0;
		Islands[n].reload.l5.tex = "t1";

		Islands[n].reload.l6.label = "Shore_ship2";
		Islands[n].reload.l6.name = "reload_5";
		Islands[n].reload.l6.go = "Shore_ship2";
		Islands[n].reload.l6.emerge = "sea";
		Islands[n].reload.l6.radius = 600.0;
		Islands[n].reload.l6.pic = 0;
		Islands[n].reload.l6.tex = "t1";

		Islands[n].reload.l7.label = "Shore9";
		Islands[n].reload.l7.name = "reload_6";
		Islands[n].reload.l7.go = "Shore9";
		Islands[n].reload.l7.emerge = "sea";
		Islands[n].reload.l7.radius = 600.0;
		Islands[n].reload.l7.pic = 0;
		Islands[n].reload.l7.tex = "t1";

		Islands[n].reload.l8.label = "Shore10";
		Islands[n].reload.l8.name = "reload_7";
		Islands[n].reload.l8.go = "Shore10";
		Islands[n].reload.l8.emerge = "sea";
		Islands[n].reload.l8.radius = 600.0;
		Islands[n].reload.l8.pic = 0;
		Islands[n].reload.l8.tex = "t1";

        Islands[n].Trade.Export.id1 = GOOD_SILVER;
		Islands[n].Trade.Export.id2 = GOOD_EBONY;
		Islands[n].Trade.Export.id3 = GOOD_CINNAMON;
		Islands[n].Trade.Export.id4 = GOOD_WINE;
		Islands[n].Trade.Export.id5 = GOOD_RUM;
		Islands[n].Trade.Export.id6 = GOOD_WHEAT;
		Islands[n].Trade.Export.id7 = GOOD_COTTON;

		Islands[n].Trade.Import.id1 = GOOD_PLANKS;
		Islands[n].Trade.Import.id2 = GOOD_WEAPON;
		Islands[n].Trade.Import.id3 = GOOD_SANDAL;
		Islands[n].Trade.Import.id4 = GOOD_PAPRIKA;
		Islands[n].Trade.Import.id5 = GOOD_TOBACCO;
		Islands[n].Trade.Import.id6 = GOOD_LINEN;
		Islands[n].Trade.Import.id7 = GOOD_CHOCOLATE;

		Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
		Islands[n].Trade.Contraband.id2 = GOOD_OIL;
		Islands[n].Trade.Contraband.id3 = GOOD_SAILCLOTH;
        Islands[n].Trade.Contraband.id4 = GOOD_COFFEE;
        Islands[n].Trade.Contraband.id5 = GOOD_LEATHER;

		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);

        // boal 28.01.2004 -->
        Islands[n].Trade.Contraband.id1 = GOOD_SLAVES;
        // boal 28.01.2004 <--

	/////////////////////////////////////////////////////////////////////////
	/// ГПК
	/////////////////////////////////////////////////////////////////////////
		n = n + 1;
		Islands[n].id = "LostShipsCity";
		Islands[n].model = "LostShipsCity";
		Islands[n].filespath.models = "islands\LostShipsCity";
		Islands[n].refl_model = "LostShipsCity_refl";
		Islands[n].locators = "LostShipsCity_locators";
		Islands[n].visible = true;
		Islands[n].reload_enable = true;
		//Islands[n].LoadGroup.g1 = "IslandShips1";
		Islands[n].TexturePath = "LostShipsCity";

		Islands[n].reload.l1.label = "LSC North";
		Islands[n].reload.l1.name = "reload_1";
		Islands[n].reload.l1.go = "LostShipsCity_town";
		Islands[n].reload.l1.emerge = "reload1_back";
		Islands[n].reload.l1.radius = 600.0;
		Islands[n].reload.l1.pic = 0;
		Islands[n].reload.l1.tex = "t1";
		Islands[n].reload.l1.istown = true; 

		Islands[n].reload.l2.label = "LSC South";
		Islands[n].reload.l2.name = "reload_2";
		Islands[n].reload.l2.go = "LostShipsCity_town";
		Islands[n].reload.l2.emerge = "reload1_back";
		Islands[n].reload.l2.radius = 600.0;
		Islands[n].reload.l2.pic = 0;
		Islands[n].reload.l2.tex = "t1";
		Islands[n].reload.l2.istown = true;

		Islands[n].InterfaceTextures.t1 = "battle_interface\moor_7.tga";
		Islands[n].InterfaceTextures.t1.h = 4;
		Islands[n].InterfaceTextures.t1.v = 1;

		SendMessage(&locator,"le",LM_LOCATE_I,&Islands[n]);
        
		iNumIslands = n;
		Trace("How islands = " + iNumIslands);
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		
		DeleteClass(&locator);		
}
