string isUsersName="";

//ref rCharacter; // boal
int iChar;
#event_handler("Control Activation","TempIControlProcess");

void TempIControlProcess()
{
	string ControlName;
	ControlName = GetEventData();
	if (ControlName == "WhrPrevWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
	if (ControlName == "WhrNextWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
	if (ControlName == "WhrUpdateWeather")
	{
		Sea.MaxSeaHeight = 1.0;
		Sea.isDone = "";
	}
}

void InitInterface(string iniName)
{
	Event("DoInfoShower","sl","MainMenuLaunch",false);

	aref arScrShoter;
	if( !FindClass(&arScrShoter,"scrshoter") ) {
		CreateScreenShoter();
	}

	CreateBackEnvironment();

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	// boal ver num -->
    CreateString(true,"VerNum", VERSION_NUMBER1 + GetVerNum(), FONT_NORMAL, COLOR_NORMAL, 780, 580, SCRIPT_ALIGN_RIGHT, 0.9);
    // boal ver num <--
	
	SetEventHandler("backgroundcommand","ProcCommand",0);

	GameInterface.SavePath = "SAVE";

 //ResetSoundScheme();
	ResetSound(); // new
	// ВВОД СВОИХ СХЕМ В ЗАВИСИМОСТИ ОТ ПОГОДЫ (BY LOKK)
	if (Whr_IsRain())
	{ if (Whr_IsNight())
		{
			SetSoundScheme("mainmenu_night_rain");
		}
		else
		{
			SetSoundScheme("mainmenu_day_rain");
		}
    }
	else
	{ if (Whr_IsNight())
		{
			SetSoundScheme("mainmenu_night");
		}
		else
		{
			SetSoundScheme("mainmenu_day");
		}

	}
	oldMusicID = 0;
	musicName = "";
	SetMusic("Music_main_theme");
}

void NewGamePress()
{
	IDoExit( RC_INTERFACE_DO_NEW_GAME, true );
}

void LoadPress()
{
	IDoExit( RC_INTERFACE_DO_LOAD_GAME, false );
}

void SavePress()
{
	IDoExit( RC_INTERFACE_DO_SAVE_GAME, false );
}

void OptionsPress()
{
	IDoExit( RC_INTERFACE_DO_OPTIONS, false );
}

void ControlsPress()
{
	IDoExit( RC_INTERFACE_DO_CONTROLS, false );
}

void CreditsPress()
{
	IDoExit( RC_INTERFACE_DO_CREDITS, false );

	/*SetEventHandler(EVENT_END_VIDEO,"LaunchMainMenu_afterVideo",0);
	bMainMenuLaunchAfterVideo = true;
	StartPostVideo("credits",1);*/
}

void QuitPress()
{
    DelEventHandler("frame","QuitPress");
	EngineLayersOffOn(false);
	IDoExit(-1, false);
	ExitProgram();
}

void procGameQuit()
{
	ExitProgram();
}

void MultiPress()
{
	//PrepareNetEnvironment();
	DeleteBackEnvironment();
	IDoExit(RC_INTERFACE_NET_FINDGAME, false);
	InterfaceStates.BackEnvironmentIsCreated = false;
}

void IDoExit(int exitCode, bool bClear)
{
	InterfaceStates.BackEnvironmentIsCreated = true;

	DelEventHandler("backgroundcommand","ProcCommand");
	
	interfaceResultCommand = exitCode;
	EndCancelInterface(bClear);
}

void ExitCancel()
{
	IDoExit(-1,true);
}

void ProcCommand()
{
	string comName = GetEventData();

	switch(comName)
	{
	case "New":			NewGamePress(); break;
	case "Load":		LoadPress(); break;
	case "Save":		SavePress(); break;
	case "Options":		OptionsPress(); break;
	//case "Multiplayer":	MultiPress(); break;
	case "Credits":		CreditsPress(); break;
	case "Exit":		SetEventHandler("frame","QuitPress",0);  break;
	}
}

object InterfaceBackScene;
void CreateBackEnvironment()
{
	LayerFreeze("execute",false);
	LayerFreeze("realize",false);

	if( CheckAttribute(&InterfaceStates,"BackEnvironmentIsCreated") && InterfaceStates.BackEnvironmentIsCreated=="1" ) {
		return;
	}

	// create weather
	ICreateWeather();

	CreateEntity(&InterfaceBackScene,"InterfaceBackScene");
	LayerAddObject("execute", &InterfaceBackScene, -1);
	LayerAddObject("realize", &InterfaceBackScene, 1000);

	SendMessage(&InterfaceBackScene, "ls", 0, "MainMenu\MainMenu"); // set model
	SendMessage(&InterfaceBackScene, "ls", 1, "camera"); // set camera

	aref arMenu;
	makearef(arMenu,InterfaceBackScene.menu);
	arMenu.l1.locname = "menu02";	arMenu.l1.sel = "mainmenu\menu02_active";	arMenu.l1.norm = "mainmenu\menu02_passive"; arMenu.l1.event = "New";			arMenu.l1.path = LanguageGetLanguage();
	arMenu.l2.locname = "menu03";	arMenu.l2.sel = "mainmenu\menu03_active";	arMenu.l2.norm = "mainmenu\menu03_passive"; arMenu.l2.event = "Load";			arMenu.l2.path = LanguageGetLanguage();
	arMenu.l3.locname = "menu04";	arMenu.l3.sel = "mainmenu\menu04_active";	arMenu.l3.norm = "mainmenu\menu04_passive"; arMenu.l3.event = "Options";		arMenu.l3.path = LanguageGetLanguage();
	//arMenu.l4.locname = "menu05";	arMenu.l4.sel = "mainmenu\menu05_active";	arMenu.l4.norm = "mainmenu\menu05_passive"; arMenu.l4.event = "Multiplayer";	arMenu.l4.path = LanguageGetLanguage();
	//arMenu.l5.locname = "menu06";	arMenu.l5.sel = "mainmenu\menu06_active";	arMenu.l5.norm = "mainmenu\menu06_passive"; arMenu.l5.event = "Credits";		arMenu.l5.path = LanguageGetLanguage();
	//arMenu.l6.locname = "menu07";	arMenu.l6.sel = "mainmenu\menu07_active";	arMenu.l6.norm = "mainmenu\menu07_passive"; arMenu.l6.event = "Exit";			arMenu.l6.path = LanguageGetLanguage();
	arMenu.l4.locname = "menu05";	arMenu.l4.sel = "mainmenu\menu05_active";	arMenu.l4.norm = "mainmenu\menu05_passive"; arMenu.l4.event = "Credits";		arMenu.l4.path = LanguageGetLanguage();
	arMenu.l5.locname = "menu06";	arMenu.l5.sel = "mainmenu\menu06_active";	arMenu.l5.norm = "mainmenu\menu06_passive"; arMenu.l5.event = "Exit";			arMenu.l5.path = LanguageGetLanguage();
	SendMessage(&InterfaceBackScene, "lla", 3, 1, arMenu ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 0, true ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 1, true ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 2, true ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 3, true ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 4, true ); // set menu
	//SendMessage(&InterfaceBackScene, "lll", 7, 5, true ); // set menu

	// create ship
	MainMenu_CreateShip();

	if( LanguageGetLanguage() != "Russian" )
	{
		MenuCreateLogo();
	}

	if( Whr_IsNight() ) {
		InterfaceBackScene.light.turnon = true;
		InterfaceBackScene.light.model = "mainmenu\Fonar_night";
		InterfaceBackScene.light.lightcolormin = argb(0,200,200,120);//argb(255,114,114,80);
		InterfaceBackScene.light.lightcolormax = argb(48,255,255,180);
		InterfaceBackScene.light.colorperiod = 0.4;
		InterfaceBackScene.light.addcolorperiod = 1.0;
		InterfaceBackScene.light.rangemin = 10.0;
		InterfaceBackScene.light.rangemax = 14.0;
		InterfaceBackScene.light.rangeperiod = 1.5;
		InterfaceBackScene.light.locator = "Light";
		InterfaceBackScene.light.lightlocator = "fonar";
		InterfaceBackScene.light.flarelocator = "fire";
		InterfaceBackScene.light.flaresize = 0.5;
		InterfaceBackScene.light.minflarecolor = 120.0;
		InterfaceBackScene.light.maxflarecolor = 200.0;
	} else {
		MainMenu_CreateAnimals();

		InterfaceBackScene.light.turnon = false;
		InterfaceBackScene.light.model = "mainmenu\Fonar_day";
		InterfaceBackScene.light.locator = "Light";
		InterfaceBackScene.light.lightlocator = "fonar";
	}
	SendMessage(&InterfaceBackScene, "ls", 8, "light" );

	if( Whr_IsNight() )
	{
		// create particles
		InitParticles();
		CreateParticleSystem("candle", stf(InterfaceBackScene.lightpos.x),stf(InterfaceBackScene.lightpos.y),stf(InterfaceBackScene.lightpos.z), 0.0,0.0,0.0, 0);
	}
}

void MainMenu_CreateShip()
{
	//int
	iChar = GenerateCharacter(rand(4), WITHOUT_SHIP, "citizen", MAN, 0, WARRIOR);   //PIRATE
	ref	rCharacter = &characters[iChar];
	int nChoosedBaseShipType = rand(SHIP_GALEON_H) + 2;//SHIP_TARTANE;SHIP_LUGGER;SHIP_CARAVEL;
	rCharacter.ship.type = GenerateShip(nChoosedBaseShipType, false);
	SetBaseShipData( rCharacter );
	ref refBaseShip = GetRealShip( sti(rCharacter.ship.type) );
	refBaseShip.SpeedRate = 3.0; // чтоб не летал
	Ship_SetFantomData( rCharacter );
	Ship_ClearImpulseStrength( rCharacter );
	Ship_SetLightsAndFlares( rCharacter );
	Ship_SetTrackSettings( rCharacter );

	rCharacter.Features.GeraldSails = true;
	rCharacter.Ship.Speed.z = 1.0;
	//rCharacter.Ship.Stopped = true;
	rCharacter.Ship.Pos.Mode = SHIP_SAIL;
	rCharacter.Ship.Pos.x = 0.0;
	rCharacter.Ship.Pos.z = 0.0;
	rCharacter.Ship.Ang.y = 0.0;
	rCharacter.Ship.Strand = false;
	rCharacter.Ship.Strand.SndID = -1;
	
	rCharacter.Ship.LastBallCharacter = -1;
	rCharacter.Ship.WindDotShip = 1.0;
	
	rCharacter.TmpPerks = "";
	aref aTmpPerks; makearef(aTmpPerks, rCharacter.TmpPerks);
	aTmpPerks.StormProfessional		= 0;
	
	SendMessage(&InterfaceBackScene, "lsa", 2, "ship", &rCharacter); // set ship position
	CreateEntity( rCharacter, "ship" );
	ref rBaseShip = GetRealShip(sti(rCharacter.ship.type));
	SendMessage( rCharacter, "laa", MSG_SHIP_CREATE, &rCharacter, &rBaseShip );
}

void DeleteBackEnvironment()
{
	LayerDelObject("execute", &InterfaceBackScene);
	LayerDelObject("realize", &InterfaceBackScene);

	MainMenu_DeleteAnimals();
	DeleteClass( GetCharacter(iChar));//  boal fix FANTOM_CHARACTERS) );
	DeleteShipEnvironment();
	DeleteWeather();
	DeleteSea();
	DeleteClass( InterfaceBackScene );
	DeleteCoastFoamEnvironment();

	iNextWeatherNum = -1;
	iCurWeatherHour = -1;
}

void MainMenu_CreateAnimals()
{
	InterfaceBackScene.seagull.locator = "seagull";
	InterfaceBackScene.seagull.model = "mainmenu\seagull";
	InterfaceBackScene.seagull.animation = "seagull";
	InterfaceBackScene.seagull.aniaction = "idle";
	SendMessage(&InterfaceBackScene, "ls", 9, "seagull" );
}

void MenuCreateLogo()
{
	InterfaceBackScene.Logo.locator = "";
	InterfaceBackScene.Logo.model = "mainmenu\AOPBoard";
	if( !Whr_IsNight() ) {
		InterfaceBackScene.Logo.technique = "InterfaceBackScene_Logo";
	}
	SendMessage(&InterfaceBackScene, "ls", 10, "Logo" );
}

void MainMenu_DeleteAnimals()
{
	if (IsEntity(Animals))
		DeleteClass(Animals);
}

void ICreateWeather()
{
	// choose random weather
	int n = 0;
	if( CheckAttribute(&InterfaceStates,"mainmenuweather") )
	{
		n = sti(InterfaceStates.mainmenuweather);
	}
	else
	{
		n = rand(iTotalNumWeathers-1);
		int oldN=n;
		while (true)
		{
			if( !CheckAttribute( &Weathers[n], "skip" ) || Weathers[n].skip!="1" )
			{
				break;
			}
			//candle
			n++;
			if( n==iTotalNumWeathers ) {n = 0;}
			if( n==oldN ) {break;}
		}
	}
	if (n < 0 || n >= iTotalNumWeathers) n = 0;
	SetNextWeather(Weathers[n].id);
	iBlendWeatherNum = -1; // залоченная погода
	Whr_LoadNextWeather(0);
	iBlendWeatherNum = -1; // залоченная погода
	InterfaceBackScene.current_weather = n;
	InterfaceStates.mainmenuweather    = n;

	CreateSea("execute","realize");
	CreateWeather("execute","realize");
	CreateShipEnvironment();
	Sea.MaxSeaHeight = 1.0;
	Sea.isDone = "";

	CreateCoastFoamEnvironment("MainMenu", "execute", "realize");
	iBlendWeatherNum = -1; // залоченная погода
}